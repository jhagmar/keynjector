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
#ifndef SHA1_H
#define SHA1_H

/// An implementation of SHA-1 optimized for Atmel SAM 4S

#define SHA1_CHUNK_SIZE 64
#define SHA1_DIGEST_SIZE 20

/// SHA-1 context
typedef struct {
	uint32_t h0;
	uint32_t h1;
	uint32_t h2;
	uint32_t h3;
	uint32_t h4;
	uint8_t buf[SHA1_CHUNK_SIZE];
	uint64_t len;
} sha1_ctx;

/// Initialize SHA-1 context
void sha1_init(sha1_ctx *ctx);

/// Update SHA-1 context with data from m with length len
void sha1_update(sha1_ctx *ctx, uint8_t const *m, uint32_t len);

/// Finalize SHA-1 context
void sha1_finish(sha1_ctx *ctx);

/// Perform complete hash cycle (init, update, finalize) on m of length len
void sha1_hash(sha1_ctx *ctx, uint8_t const *m, uint32_t len);

/// Extract SHA-1 digest from context
void sha1_digest(sha1_ctx *ctx, uint8_t d[20]);

/// Perform internal testing of SHA-1 implementation
int sha1_test(void);

#endif
