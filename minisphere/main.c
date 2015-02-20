#include "minisphere.h"

#include "api.h"
#include "color.h"
#include "font.h"
#include "image.h"
#include "input.h"
#include "log.h"
#include "map_engine.h"
#include "rfn_handler.h"
#include "sound.h"
#include "spriteset.h"
#include "surface.h"
#include "windowstyle.h"

// enable visual styles (VC++)
#ifdef _MSC_VER
#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")
#endif

static const int MAX_FRAME_SKIPS = 5;

static void on_duk_fatal    (duk_context* ctx, duk_errcode_t code, const char* msg);
static void handle_js_error ();
static void shutdown_engine ();

static int     s_current_fps;
static int     s_current_game_fps;
static int     s_frame_skips;
static clock_t s_last_fps_poll_time;
static clock_t s_last_frame_time;
static int     s_num_flips;
static int     s_num_frames;
static bool    s_show_fps = true;
static bool    s_take_screenshot = false;

ALLEGRO_DISPLAY*     g_display   = NULL;
duk_context*         g_duktape   = NULL;
ALLEGRO_EVENT_QUEUE* g_events    = NULL;
ALLEGRO_CONFIG*      g_game_conf = NULL;
ALLEGRO_PATH*        g_game_path = NULL;
key_queue_t          g_key_queue;
int                  g_render_scale;
ALLEGRO_CONFIG*      g_sys_conf;
ALLEGRO_FONT*        g_sys_font  = NULL;
int                  g_res_x, g_res_y;

int
main(int argc, char** argv)
{
	ALLEGRO_BITMAP*   icon;
	char*             icon_path;
	char*             path;
	ALLEGRO_TRANSFORM trans;
	
	// initialize Allegro
	al_init();
	al_init_native_dialog_addon();
	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_install_keyboard();

	// load system configuraton
	path = get_sys_asset_path("system.ini", NULL);
	g_sys_conf = al_load_config_file(path);
	
	// determine location of game.sgm and try to load it
	g_game_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_append_path_component(g_game_path, "startup");
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-game") == 0 && i < argc - 1) {
			al_destroy_path(g_game_path);
			g_game_path = al_create_path(argv[i + 1]);
			if (strcmp(al_get_path_filename(g_game_path), "game.sgm") != 0) {
				al_destroy_path(g_game_path);
				g_game_path = al_create_path_for_directory(argv[i + 1]);
			}
		}
	}
	al_set_path_filename(g_game_path, NULL);
	al_make_path_canonical(g_game_path);
	char* sgm_path = get_asset_path("game.sgm", NULL, false);
	g_game_conf = al_load_config_file(sgm_path);
	free(sgm_path);
	if (g_game_conf == NULL) {
		al_show_native_message_box(NULL, "Unable to Load Game",
			al_path_cstr(g_game_path, ALLEGRO_NATIVE_PATH_SEP),
			"minisphere was unable to load game.sgm or it was not found. Check to make sure the above directory exists and contains a valid Sphere game.",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return EXIT_FAILURE;
	}

	// set up engine and create display window
	icon_path = get_asset_path("game-icon.png", NULL, false);
	icon = al_load_bitmap(icon_path);
	free(icon_path);
	al_register_font_loader(".rfn", &al_load_rfn_font);
	al_reserve_samples(8);
	al_set_mixer_gain(al_get_default_mixer(), 1.0);
	g_res_x = atoi(al_get_config_value(g_game_conf, NULL, "screen_width"));
	g_res_y = atoi(al_get_config_value(g_game_conf, NULL, "screen_height"));
	g_render_scale = (g_res_x < 400 && g_res_y < 300) ? 2 : 1;
		// ^ default to 2x if resolution <= 400x300
	g_display = al_create_display(g_res_x * g_render_scale, g_res_y * g_render_scale);
	al_identity_transform(&trans);
	al_scale_transform(&trans, g_render_scale, g_render_scale);
	al_use_transform(&trans);
	if (icon != NULL) al_set_display_icon(g_display, icon);
	al_set_window_title(g_display, al_get_config_value(g_game_conf, NULL, "name"));
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	g_events = al_create_event_queue();
	al_register_event_source(g_events, al_get_display_event_source(g_display));
	al_register_event_source(g_events, al_get_keyboard_event_source());
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	// initialize JavaScript engine
	g_duktape = duk_create_heap(NULL, NULL, NULL, NULL, &on_duk_fatal);
	init_api(g_duktape);
	init_color_api();
	init_font_api(g_duktape);
	init_image_api(g_duktape);
	init_input_api(g_duktape);
	init_log_api(g_duktape);
	init_map_engine_api(g_duktape);
	init_sound_api(g_duktape);
	init_spriteset_api(g_duktape);
	init_surface_api();
	init_windowstyle_api();
	char* sys_font_path = get_sys_asset_path("system.rfn", NULL);
	g_sys_font = al_load_font(sys_font_path, 0, 0x0);
	free(sys_font_path);
	duk_push_global_stash(g_duktape);
	duk_push_sphere_Font(g_duktape, g_sys_font);
	duk_put_prop_string(g_duktape, -2, "system_font");
	duk_pop(g_duktape);

	// load startup script
	duk_int_t exec_result;
	char* script_path = get_asset_path(al_get_config_value(g_game_conf, NULL, "script"), "scripts", false);
	exec_result = duk_pcompile_file(g_duktape, 0x0, script_path);
	free(script_path);
	if (exec_result != DUK_EXEC_SUCCESS) {
		handle_js_error();
	}
	exec_result = duk_pcall(g_duktape, 0);
	if (exec_result != DUK_EXEC_SUCCESS) {
		handle_js_error();
	}
	duk_pop(g_duktape);

	// call game() function in script
	s_last_frame_time = s_last_fps_poll_time = clock();
	s_num_frames = s_num_flips = 0;
	s_current_fps = s_current_game_fps = 0;
	duk_push_global_object(g_duktape);
	duk_get_prop_string(g_duktape, -1, "game");
	exec_result = duk_pcall(g_duktape, 0);
	if (exec_result != DUK_EXEC_SUCCESS) {
		handle_js_error();
	}
	duk_pop(g_duktape);
	duk_pop(g_duktape);
	
	// teardown
	shutdown_engine();
	return EXIT_SUCCESS;
}

bool
do_events(void)
{
	ALLEGRO_EVENT event;
	int           key_index;

	while (al_get_next_event(g_events, &event)) {
		switch (event.type) {
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			return false;
		case ALLEGRO_EVENT_KEY_CHAR:
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_F11:
				s_show_fps = !s_show_fps;
				break;
			case ALLEGRO_KEY_F12:
				s_take_screenshot = true;
				break;
			default:
				if (g_key_queue.num_keys < 255) {
					key_index = g_key_queue.num_keys;
					++g_key_queue.num_keys;
					g_key_queue.keys[key_index] = event.keyboard.keycode;
				}
				break;
			}
		}
	}
	return true;
}

bool
end_frame(int framerate)
{
	clock_t           current_time;
	char              filename[50];
	char              fps_text[20];
	clock_t           frame_ticks;
	clock_t           next_frame_time;
	char*             path;
	ALLEGRO_BITMAP*   screenshot;
	bool              skipping_frame = false;
	ALLEGRO_TRANSFORM trans;
	int               x, y;
	
	if (framerate > 0) {
		frame_ticks = CLOCKS_PER_SEC / framerate;
		current_time = clock();
		next_frame_time = s_last_frame_time + frame_ticks;
		skipping_frame = s_frame_skips < MAX_FRAME_SKIPS && current_time > next_frame_time;
		do {
			if (!do_events()) return false;
		} while (clock() < next_frame_time);
		s_last_frame_time += frame_ticks;
	}
	else {
		if (!do_events()) return false;
	}
	++s_num_frames;
	if (!skipping_frame) {
		++s_num_flips;
		s_last_frame_time = clock();
		s_frame_skips = 0;
		if (s_take_screenshot) {
			sprintf(filename, "snapshot-%i.png", (int)time(NULL));
			path = get_asset_path(filename, "screenshots", true);
			screenshot = al_create_bitmap(g_res_x, g_res_y);
			al_set_target_bitmap(screenshot);
			al_draw_scaled_bitmap(al_get_backbuffer(g_display),
				0, 0, al_get_display_width(g_display), al_get_display_height(g_display), 0, 0, g_res_x, g_res_y,
				0x0);
			al_set_target_backbuffer(g_display);
			al_save_bitmap(path, screenshot);
			al_destroy_bitmap(screenshot);
			free(path);
			s_take_screenshot = false;
		}
		if (s_show_fps) {
			if (framerate > 0) sprintf(fps_text, "%i/%i fps", s_current_fps, s_current_game_fps);
				else sprintf(fps_text, "%i fps", s_current_fps);
			al_identity_transform(&trans);
			al_use_transform(&trans);
			x = al_get_display_width(g_display) - 108;
			y = 8;
			al_draw_filled_rounded_rectangle(x, y, x + 100, y + 16, 4, 4, al_map_rgba(0, 0, 0, 128));
			al_draw_text(g_sys_font, al_map_rgba(0, 0, 0, 128), x + 51, y + 3, ALLEGRO_ALIGN_CENTER, fps_text);
			al_draw_text(g_sys_font, al_map_rgba(255, 255, 255, 128), x + 50, y + 2, ALLEGRO_ALIGN_CENTER, fps_text);
			al_scale_transform(&trans, g_render_scale, g_render_scale);
			al_use_transform(&trans);
		}
		al_flip_display();
		al_clear_to_color(al_map_rgba(0, 0, 0, 255));
	}
	else {
		++s_frame_skips;
	}
	if (s_last_frame_time >= s_last_fps_poll_time + CLOCKS_PER_SEC) {
		s_current_fps = s_num_flips;
		s_current_game_fps = s_num_frames;
		s_last_fps_poll_time = s_last_frame_time;
		s_num_flips = 0;
		s_num_frames = 0;
	}
	return true;
}

void
free_lstring(lstring_t* string)
{
	if (string != NULL) {
		free(string->buffer);
		free(string);
	}
}

char*
get_asset_path(const char* path, const char* base_dir, bool allow_mkdir)
{
	bool is_homed = (strstr(path, "~/") == path || strstr(path, "~\\") == path);
	ALLEGRO_PATH* base_path = al_create_path_for_directory(base_dir);
	al_rebase_path(g_game_path, base_path);
	if (allow_mkdir) {
		const char* dir_path = al_path_cstr(base_path, ALLEGRO_NATIVE_PATH_SEP);
		al_make_directory(dir_path);
	}
	ALLEGRO_PATH* asset_path = al_create_path(is_homed ? &path[2] : path);
	bool is_absolute = al_get_path_num_components(asset_path) > 0
		&& strcmp(al_get_path_component(asset_path, 0), "") == 0;
	char* out_path = NULL;
	if (!is_absolute) {
		al_rebase_path(is_homed ? g_game_path : base_path, asset_path);
		al_make_path_canonical(asset_path);
		out_path = strdup(al_path_cstr(asset_path, ALLEGRO_NATIVE_PATH_SEP));
	}
	al_destroy_path(asset_path);
	al_destroy_path(base_path);
	return out_path;
}

char*
get_sys_asset_path(const char* path, const char* base_dir)
{
	bool is_homed = (strstr(path, "~/") == path || strstr(path, "~\\") == path);
	ALLEGRO_PATH* base_path = al_create_path_for_directory(base_dir);
	ALLEGRO_PATH* system_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_append_path_component(system_path, "system");
	al_rebase_path(system_path, base_path);
	ALLEGRO_PATH* asset_path = al_create_path(is_homed ? &path[2] : path);
	bool is_absolute = al_get_path_num_components(asset_path) > 0
		&& strcmp(al_get_path_component(asset_path, 0), "") == 0;
	char* out_path = NULL;
	if (!is_absolute) {
		al_rebase_path(is_homed ? system_path : base_path, asset_path);
		al_make_path_canonical(asset_path);
		out_path = strdup(al_path_cstr(asset_path, ALLEGRO_NATIVE_PATH_SEP));
	}
	al_destroy_path(asset_path);
	al_destroy_path(system_path);
	al_destroy_path(base_path);
	return out_path;
}

void
al_draw_tiled_bitmap(ALLEGRO_BITMAP* bitmap, float x, float y, float width, float height)
{
	bool draw_held;
	int  src_w, src_h;
	int  x_tiles, y_tiles;
	int  i_x, i_y;
	
	src_w = al_get_bitmap_width(bitmap);
	src_h = al_get_bitmap_height(bitmap);
	x_tiles = (int)(width / src_w);
	y_tiles = (int)(height / src_h);
	draw_held = al_is_bitmap_drawing_held();
	al_hold_bitmap_drawing(true);
	for (i_x = 0; i_x < x_tiles; ++i_x) {
		for (i_y = 0; i_y < y_tiles; ++i_y) {
			al_draw_bitmap(bitmap, x + i_x * src_w, y + i_y * src_h, 0x0);
			al_draw_bitmap_region(bitmap, 0, 0, (int)width % src_w, src_h, x + x_tiles * src_w, y + i_y * src_h, 0x0);
		}
		al_draw_bitmap_region(bitmap, 0, 0, src_w, (int)height % src_h, x + i_x * src_w, y + y_tiles * src_h, 0x0);
	}
	al_draw_bitmap_region(bitmap, 0, 0, (int)width % src_w, (int)height % src_h, x + x_tiles * src_w, y + y_tiles * src_h, 0x0);
	al_hold_bitmap_drawing(draw_held);
}

ALLEGRO_BITMAP*
al_fread_bitmap(ALLEGRO_FILE* file, int width, int height)
{
	ALLEGRO_BITMAP*        bitmap = NULL;
	ALLEGRO_LOCKED_REGION* lock = NULL;

	if ((bitmap = al_create_bitmap(width, height)) == NULL)
		goto on_error;
	if ((lock = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888, ALLEGRO_LOCK_WRITEONLY)) == NULL)
		goto on_error;
	size_t data_size = width * height * 4;
	if (al_fread(file, lock->data, data_size) != data_size)
		goto on_error;
	al_unlock_bitmap(bitmap);
	return bitmap;

on_error:
	if (lock != NULL) al_unlock_bitmap(bitmap);
	if (bitmap != NULL) al_destroy_bitmap(bitmap);
	return NULL;
}

lstring_t*
al_fread_lstring(ALLEGRO_FILE* file)
{
	lstring_t* string = NULL;
	uint16_t   length;

	if ((string = calloc(1, sizeof(lstring_t))) == NULL)
		goto on_error;
	if (al_fread(file, &length, 2) != 2) goto on_error;
	if ((string->buffer = calloc(length + 1, sizeof(char))) == NULL) goto on_error;
	if (al_fread(file, string->buffer, length) != length) goto on_error;
	return string;

on_error:
	if (string != NULL) {
		free(string->buffer);
		free(string);
	}
	return NULL;
}

static void
on_duk_fatal(duk_context* ctx, duk_errcode_t code, const char* msg)
{
	al_show_native_message_box(g_display, "Script Error", msg, NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
	shutdown_engine();
	exit(0);
}

static void
handle_js_error()
{
	duk_errcode_t err_code = duk_get_error_code(g_duktape, -1);
	duk_dup(g_duktape, -1);
	const char* err_msg = duk_safe_to_string(g_duktape, -1);
	if (err_code != DUK_ERR_ERROR || strcmp(err_msg, "Error: !exit") != 0) {
		duk_get_prop_string(g_duktape, -2, "lineNumber");
		duk_int_t line_num = duk_get_int(g_duktape, -1);
		duk_pop(g_duktape);
		duk_get_prop_string(g_duktape, -2, "fileName");
		const char* file_path = duk_get_string(g_duktape, -1);
		if (file_path != NULL) {
			char* file_name = strrchr(file_path, '/');
			file_name = file_name != NULL ? (file_name + 1) : file_path;
			duk_push_sprintf(g_duktape, "%s (line %d)\n\n%s", file_name, (int)line_num, err_msg);
		}
		else {
			duk_push_string(g_duktape, err_msg);
		}
		duk_fatal(g_duktape, err_code, duk_get_string(g_duktape, -1));
	}
}

static void
shutdown_engine(void)
{
	duk_destroy_heap(g_duktape);
	al_uninstall_audio();
	al_destroy_display(g_display);
	al_destroy_event_queue(g_events);
	al_destroy_config(g_game_conf);
	al_destroy_path(g_game_path);
	if (g_sys_conf != NULL) al_destroy_config(g_sys_conf);
	al_uninstall_system();
}
