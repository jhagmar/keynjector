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
#ifndef CAST128_H
#define CAST128_H

/// An implementation of CAST-128 (RFC 2144) specifically designed
/// for decryption of symmetric OpenPGP (RFC 4880) blocks, where
/// CFB mode is used, with a special reinitialization of the IV. 

// CAST-128 context
typedef struct {
	uint32_t Km[16];
	uint8_t Kr[16];
} cast128_ctx;

// CAST-128 CFB mode context
typedef struct {
	cast128_ctx ctx;
	uint8_t fr[8];
	uint8_t fre[8];
	uint32_t n;
} cast128_cfb_ctx;

#define CAST128_KEY_SIZE 16
#define CAST128_BLOCK_SIZE 8

/// initialize ctx with key
void cast128_init(cast128_ctx *ctx, uint8_t const key[CAST128_KEY_SIZE]);

/// encrypt m to c in ECB mode using ctx
void cast128_encrypt(cast128_ctx *ctx, uint8_t c[CAST128_BLOCK_SIZE], uint8_t const m[CAST128_BLOCK_SIZE]);

/// reinitialize ctx with iv
void cast128_cfb_reinit(cast128_cfb_ctx *ctx, uint8_t const iv[CAST128_BLOCK_SIZE]);

/// initialize ctx with key and iv
void cast128_cfb_init(cast128_cfb_ctx *ctx, uint8_t const key[CAST128_KEY_SIZE], uint8_t const iv[CAST128_BLOCK_SIZE]);

/// decrypt c of length len to m using ctx
void cast128_cfb_decrypt(cast128_cfb_ctx *ctx, uint8_t *m, uint8_t const *c, uint32_t len);

/// test CAST-128 using test vectors from RFC-2144
int cast128_test(void);


#endif
