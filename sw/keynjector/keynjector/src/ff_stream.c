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
#include "ff_stream.h"

// FatFS stream context
typedef struct
{
	mpgp_sread_t sread;
	mpgp_seof_t seof;
	mpgp_serror_t serror;
	mpgp_sclose_t sclose;
	FIL *file;
} mpgp_ff_stream_t;


// read from stream
static size_t ff_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb) {
	mpgp_ff_stream_t *s;
	UINT len = size * nmemb;
	UINT bytes_read;

	s = (mpgp_ff_stream_t*) stream;
	
	f_read(s->file, ptr, len, &bytes_read);

	return bytes_read;
}

// stream end of file
static int ff_seof(mpgp_stream_t *stream) {
	mpgp_ff_stream_t *s;

	s = (mpgp_ff_stream_t*) stream;

	return (f_eof(s->file));
}

// stream error
static int ff_serror(mpgp_stream_t *stream) {
	mpgp_ff_stream_t *s;

	s = (mpgp_ff_stream_t*) stream;
	
	return f_error(s->file);
}

// stream close
static int ff_sclose(mpgp_stream_t *stream) {
	mpgp_ff_stream_t *s;

	s = (mpgp_ff_stream_t*) stream;

	f_close(s->file);
	free(s);
	return 0;
}

mpgp_stream_t *mpgp_ff_sopen(FIL *file) {
	mpgp_ff_stream_t *s;

	s = malloc(sizeof(mpgp_ff_stream_t));
	if (s != NULL) {
		s->sread = ff_sread;
		s->seof = ff_seof;
		s->serror = ff_serror;
		s->sclose = ff_sclose;
		s->file = file;
	}

	return (mpgp_stream_t*)s;
}
