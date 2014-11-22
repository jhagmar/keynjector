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
#ifndef OPGP_H
#define OPGP_H

/// a limited OpenPGP (RFC 4880) parser with a stream interface

#include <stdint.h>
#include "stream.h"

/// construct decryptor stream from input stream and passphrase with length passphrase_len
mpgp_stream_t *mpgp_open(mpgp_stream_t *stream, uint8_t const *passphrase, uint32_t passphrase_len);

#endif
