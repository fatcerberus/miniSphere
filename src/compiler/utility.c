#include "cell.h"
#include "utility.h"

void
duk_ref_heapptr(duk_context* ctx, void* heapptr)
{
	duk_push_global_stash(ctx);
	if (!duk_get_prop_string(ctx, -1, "refs")) {
		dukrub_push_bare_object(ctx);
		duk_put_prop_string(ctx, -3, "refs");
		duk_get_prop_string(ctx, -2, "refs");
		duk_replace(ctx, -2);
	}

	/* [ ... stash refs ] */
	
	duk_push_sprintf(ctx, "%p", heapptr);
	if (duk_get_prop(ctx, -2)) {
		/* [ stash refs ref_obj ] */
		
		duk_get_prop_string(ctx, -1, "refcount");
		duk_push_number(ctx, duk_get_number(ctx, -1) + 1);
		duk_put_prop_string(ctx, -3, "refcount");
		duk_pop_n(ctx, 4);
	}
	else {
		/* [ stash refs undefined ] */

		duk_push_sprintf(ctx, "%p", heapptr);
		dukrub_push_bare_object(ctx);
		duk_push_number(ctx, 1.0);
		duk_put_prop_string(ctx, -2, "refcount");
		duk_push_heapptr(ctx, heapptr);
		duk_put_prop_string(ctx, -2, "value");

		/* [ stash refs undefined key ref_obj ] */

		duk_put_prop(ctx, -4);
		duk_pop_3(ctx);
	}
}

void
duk_unref_heapptr(duk_context* ctx, void* heapptr)
{
	double refcount;

	duk_push_global_stash(ctx);
	if (!duk_get_prop_string(ctx, -1, "refs")) {
		dukrub_push_bare_object(ctx);
		duk_put_prop_string(ctx, -3, "refs");
		duk_get_prop_string(ctx, -2, "refs");
		duk_replace(ctx, -2);
	}

	/* [ ... stash refs ] */

	duk_push_sprintf(ctx, "%p", heapptr);
	if (duk_get_prop(ctx, -2)) {
		/* [ stash refs ref_obj ] */

		duk_get_prop_string(ctx, -1, "refcount");
		refcount = duk_get_number(ctx, -1) - 1.0;
		if (refcount > 0.0) {
			duk_push_number(ctx, refcount);
			duk_put_prop_string(ctx, -3, "refcount");
		}
		else {
			duk_push_sprintf(ctx, "%p", heapptr);
			duk_del_prop(ctx, -4);
		}
		duk_pop_n(ctx, 4);
	}
	else {
		/* [ stash refs undefined ] */

		duk_pop_3(ctx);
	}
}

void*
fslurp(const char* filename, size_t *out_size)
{
	void* buffer;
	FILE* file = NULL;

	if (!(file = fopen(filename, "rb")))
		return false;
	*out_size = (fseek(file, 0, SEEK_END), ftell(file));
	if (!(buffer = malloc(*out_size))) goto on_error;
	fseek(file, 0, SEEK_SET);
	if (fread(buffer, 1, *out_size, file) != *out_size)
		goto on_error;
	fclose(file);
	return buffer;

on_error:
	return NULL;
}

bool
fspew(const void* buffer, size_t size, const char* filename)
{
	FILE* file = NULL;

	if (!(file = fopen(filename, "wb")))
		return false;
	fwrite(buffer, size, 1, file);
	fclose(file);
	return true;
}

bool
wildcmp(const char* filename, const char* pattern)
{
	const char* cp = NULL;
	const char* mp = NULL;
	bool        is_match = 0;

	// check filename against the specified filter string
	while (*filename != '\0' && *pattern != '*') {
		if (*pattern != *filename && *pattern != '?')
			return false;
		++pattern;
		++filename;
	}
	while (*filename != '\0') {
		if (*pattern == '*') {
			if (*++pattern == '\0') return true;
			mp = pattern;
			cp = filename + 1;
		}
		else if (*pattern == *filename || *pattern == '?') {
			pattern++;
			filename++;
		}
		else {
			pattern = mp;
			filename = cp++;
		}
	}
	while (*pattern == '*')
		pattern++;
	return *pattern == '\0';
}
