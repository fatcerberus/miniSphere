/**
 *  Sphere: the JavaScript game platform
 *  Copyright (c) 2015-2025, Where'd She Go? LLC
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Spherical nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
**/

#ifndef CELL_BUILD_H_INCLUDED
#define CELL_BUILD_H_INCLUDED

enum cell_class
{
	CELL_DIR_STREAM = 300,
	CELL_FILE_STREAM,
	CELL_IMAGE,
	CELL_RNG,
	CELL_TARGET,
	CELL_TEXT_DEC,
	CELL_TEXT_ENC,
	CELL_TOOL,
};

typedef struct build build_t;

build_t* build_new      (const path_t* source_path, const path_t* out_path, bool debuggable);
void     build_free     (build_t* build);
bool     build_clean    (build_t* build);
bool     build_eval     (build_t* build, const char* filename);
bool     build_init_dir (build_t* build);
bool     build_package  (build_t* build, const char* filename, bool rebuilding);
bool     build_run      (build_t* build, bool rebuilding);

#endif // !CELL_BUILD_H_INCLUDED
