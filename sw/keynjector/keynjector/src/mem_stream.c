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
#include <string.h>
#include "mem_stream.h"

// stream context
typedef struct
{
  mpgp_sread_t sread;
  mpgp_seof_t seof;
  mpgp_serror_t serror;
  mpgp_sclose_t sclose;
  uint8_t const *p;
  size_t left;
} mpgp_mem_stream_t;


// read from memory stream
static size_t mem_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb) {
	mpgp_mem_stream_t *s;
	size_t len = size * nmemb;

	s = (mpgp_mem_stream_t*) stream;
	if (len > s->left) {
		len = s->left;
	}

	memcpy(ptr, s->p, len);
	s->p += len;
	s->left -= len;
	return len;
}

// memory stream end of file
static int mem_seof(mpgp_stream_t *stream) {
	mpgp_mem_stream_t *s;

	s = (mpgp_mem_stream_t*) stream;

	return (s->left == 0);
}

// memory stream error
static int mem_serror(mpgp_stream_t *stream) {
	return 0;
}

// memory stream close
static int mem_sclose(mpgp_stream_t *stream) {
	mpgp_mem_stream_t *s;

	s = (mpgp_mem_stream_t*) stream;

	free(s);
	return 0;
}

mpgp_stream_t *mpgp_mem_sopen(uint8_t const *p, size_t len) {
	mpgp_mem_stream_t *s;

	s = malloc(sizeof(mpgp_mem_stream_t));
	if (s != NULL) {
		s->sread = mem_sread;
		s->seof = mem_seof;
		s->serror = mem_serror;
		s->sclose = mem_sclose;
		s->p = p;
		s->left = len;
	}

	return (mpgp_stream_t*)s;
}
