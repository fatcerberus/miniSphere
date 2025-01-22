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

#ifndef NEOSPHERE_PEGASUS_H_INCLUDED
#define NEOSPHERE_PEGASUS_H_INCLUDED

enum pegasus_type
{
	PEGASUS_BLENDER = 200,
	PEGASUS_COLOR,
	PEGASUS_DIR_STREAM,
	PEGASUS_FILE_STREAM,
	PEGASUS_FONT,
	PEGASUS_INDEX_LIST,
	PEGASUS_JOB_TOKEN,
	PEGASUS_JOYSTICK,
	PEGASUS_KEYBOARD,
	PEGASUS_MIXER,
	PEGASUS_MODEL,
	PEGASUS_MOUSE,
	PEGASUS_QUERY,
	PEGASUS_RNG,
	PEGASUS_SAMPLE,
	PEGASUS_SERVER,
	PEGASUS_SHADER,
	PEGASUS_SHAPE,
	PEGASUS_SOCKET,
	PEGASUS_SOUND,
	PEGASUS_SOUND_STREAM,
	PEGASUS_SURFACE,
	PEGASUS_TEXT_DEC,
	PEGASUS_TEXT_ENC,
	PEGASUS_TEXTURE,
	PEGASUS_TRANSFORM,
	PEGASUS_VERTEX_LIST,
};

void pegasus_init   (int api_level, int target_api_level);
void pegasus_uninit (void);

#endif // !NEOSPHERE_PEGASUS_H_INCLUDED
