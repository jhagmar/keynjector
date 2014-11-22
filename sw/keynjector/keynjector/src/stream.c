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
#include "stream.h"

// stream context
struct mpgp_stream
{
  mpgp_sread_t sread;
  mpgp_seof_t seof;
  mpgp_serror_t serror;
  mpgp_sclose_t sclose;
};

// stream read
size_t mpgp_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb) {
	if (mpgp_seof(stream) || mpgp_serror(stream)) {
		return 0;
	}
	return stream->sread(stream, ptr, size, nmemb);
}

// stream end of file
int mpgp_seof(mpgp_stream_t *stream) {
	return stream->seof(stream);
}

// stream error
int mpgp_serror(mpgp_stream_t *stream) {
	return stream->serror(stream);
}

// stream close
int mpgp_sclose(mpgp_stream_t *stream) {
	return stream->sclose(stream);
}
