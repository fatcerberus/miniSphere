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

#ifndef NEOSPHERE_FILE_H_INCLUDED
#define NEOSPHERE_FILE_H_INCLUDED

#include "game.h"

typedef struct kev_file kev_file_t;

kev_file_t*  kev_open         (game_t* game, const char* filename, bool can_create);
void         kev_close        (kev_file_t* it);
int          kev_num_keys     (kev_file_t* it);
const char*  kev_get_key      (kev_file_t* it, int index);
bool         kev_read_bool    (kev_file_t* it, const char* key, bool def_value);
double       kev_read_float   (kev_file_t* it, const char* key, double def_value);
int          kev_read_int     (kev_file_t* it, const char* key, int def_value);
const char*  kev_read_string  (kev_file_t* it, const char* key, const char* def_value);
bool         kev_save         (kev_file_t* it);
void         kev_write_bool   (kev_file_t* it, const char* key, bool value);
void         kev_write_float  (kev_file_t* it, const char* key, double value);
void         kev_write_int    (kev_file_t* it, const char* key, int value);
void         kev_write_string (kev_file_t* it, const char* key, const char* value);

#endif // !NEOSPHERE_FILE_H_INCLUDED
