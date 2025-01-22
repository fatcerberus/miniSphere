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

#ifndef NEOSPHERE_BYTE_ARRAY_H_INCLUDED
#define NEOSPHERE_BYTE_ARRAY_H_INCLUDED

typedef struct bytearray bytearray_t;

bytearray_t*   bytearray_new          (int size);
bytearray_t*   bytearray_from_buffer  (const void* buffer, int size);
bytearray_t*   bytearray_from_lstring (const lstring_t* string);
bytearray_t*   bytearray_ref          (bytearray_t* array);
void           bytearray_unref        (bytearray_t* array);
uint8_t*       bytearray_buffer       (bytearray_t* array);
int            bytearray_len          (bytearray_t* array);
bytearray_t*   bytearray_concat       (bytearray_t* array1, bytearray_t* array2);
bytearray_t*   bytearray_deflate      (bytearray_t* array, int level);
uint8_t        bytearray_get          (bytearray_t* array, int index);
bytearray_t*   bytearray_inflate      (bytearray_t* array, int max_size);
void           bytearray_set          (bytearray_t* array, int index, uint8_t value);
bytearray_t*   bytearray_slice        (bytearray_t* array, int start, int length);

#endif // !NEOSPHERE_BYTE_ARRAY_H_INCLUDED
