#ifndef MINISPHERE__SCRIPT_H__INCLUDED
#define MINISPHERE__SCRIPT_H__INCLUDED

typedef struct script script_t;

extern bool      try_evaluate_file (const char* path);
extern script_t* compile_script    (const lstring_t* script, bool is_cp1252, const char* fmt_name, ...);
extern script_t* ref_script        (script_t* script);
extern void      free_script       (script_t* script);
extern void      run_script        (script_t* script, bool allow_reentry);

extern script_t* duk_require_sphere_script (duk_context* ctx, duk_idx_t index, const char* name);

#endif // MINISPHERE__SCRIPT_H__INCLUDED
