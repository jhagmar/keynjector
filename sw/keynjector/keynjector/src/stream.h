/*
* Copyright 2014 Jonas Hagmar
*
* This file is part of keynjector.
*
* keynjector is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* keynjector is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with keynjector.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef STREAM_H
#define STREAM_H

/// Common stream interface

#include <stdlib.h>

// forward declaration of stream context
typedef struct mpgp_stream mpgp_stream_t;

// stream read function pointer typedef
typedef size_t(*mpgp_sread_t)(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb);
// stream end of file function pointer typedef
typedef int(*mpgp_seof_t)(mpgp_stream_t *stream);
// stream error function pointer typedef
typedef int(*mpgp_serror_t)(mpgp_stream_t *stream);
// stream close function pointer typedef
typedef int(*mpgp_sclose_t)(mpgp_stream_t *stream);

// stream read
size_t mpgp_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb);
// stream end of file
int mpgp_seof(mpgp_stream_t *stream);
// stream error
int mpgp_serror(mpgp_stream_t *stream);
// stream close
int mpgp_sclose(mpgp_stream_t *stream);

#endif
