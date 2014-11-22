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
#include "sha1.h"

void sha1_init(sha1_ctx *ctx) {
	ctx->h0 = 0x67452301;
	ctx->h1 = 0xefcdab89;
	ctx->h2 = 0x98badcfe;
	ctx->h3 = 0x10325476;
	ctx->h4 = 0xc3d2e1f0;
	ctx->len = 0;
}

#ifdef SHA1_ASM
extern void sha1_process(sha1_ctx *ctx, uint8_t const *m);
#else

#define ROTL1(x) (((x) << 1) | ((x) >> 31))
#define ROTL5(x) (((x) << 5) | ((x) >> 27))
#define ROTL30(x) (((x) << 30) | ((x) >> 2))
#define CH(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define PARITY(x, y, z) ((x) ^ (y) ^ (z))
#define MAJ(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))

static uint32_t const C1 = 0x5a827999;
static uint32_t const C2 = 0x6ed9eba1;
static uint32_t const C3 = 0x8f1bbcdc;
static uint32_t const C4 = 0xca62c1d6;

#define SHA1_ITER_1(a, b, c, d, e, w) e += ROTL5((a)) +  CH((b),(c),(d)) + C1 + (w); b = ROTL30((b));
#define SHA1_ITER_2(a, b, c, d, e, w) e += ROTL5((a)) +  PARITY((b),(c),(d)) + C2 + (w); b = ROTL30((b));
#define SHA1_ITER_3(a, b, c, d, e, w) e += ROTL5((a)) +  MAJ((b),(c),(d)) + C3 + (w); b = ROTL30((b));
#define SHA1_ITER_4(a, b, c, d, e, w) e += ROTL5((a)) +  PARITY((b),(c),(d)) + C4 + (w); b = ROTL30((b));

#define W(t) (w[(t) & 0x0F] = ROTL1(w[((t) -  3) & 0x0F] ^ w[((t) - 8) & 0x0F] ^ w[((t) - 14) & 0x0F] ^ w[(t) & 0x0F]))

#define READ_BE_32(m, i) (((uint32_t)m[i] << 24) | ((uint32_t)m[(i) + 1] << 16) | ((uint32_t)m[(i) + 2] << 8) | ((uint32_t)m[(i) + 3]))

void sha1_process(sha1_ctx *ctx, uint8_t const *m) {
	uint32_t w[16];
	uint32_t a, b, c, d, e;

	a = ctx->h0;
	b = ctx->h1;
	c = ctx->h2;
	d = ctx->h3;
	e = ctx->h4;

	w[0] = READ_BE_32(m, 0);
	w[1] = READ_BE_32(m, 4);
	w[2] = READ_BE_32(m, 8);
	w[3] = READ_BE_32(m, 12);
	w[4] = READ_BE_32(m, 16);
	w[5] = READ_BE_32(m, 20);
	w[6] = READ_BE_32(m, 24);
	w[7] = READ_BE_32(m, 28);
	w[8] = READ_BE_32(m, 32);
	w[9] = READ_BE_32(m, 36);
	w[10] = READ_BE_32(m, 40);
	w[11] = READ_BE_32(m, 44);
	w[12] = READ_BE_32(m, 48);
	w[13] = READ_BE_32(m, 52);
	w[14] = READ_BE_32(m, 56);
	w[15] = READ_BE_32(m, 60);

	SHA1_ITER_1(a, b, c, d, e, w[0]);
	SHA1_ITER_1(e, a, b, c, d, w[1]);
	SHA1_ITER_1(d, e, a, b, c, w[2]);
	SHA1_ITER_1(c, d, e, a, b, w[3]);
	SHA1_ITER_1(b, c, d, e, a, w[4]);
	SHA1_ITER_1(a, b, c, d, e, w[5]);
	SHA1_ITER_1(e, a, b, c, d, w[6]);
	SHA1_ITER_1(d, e, a, b, c, w[7]);
	SHA1_ITER_1(c, d, e, a, b, w[8]);
	SHA1_ITER_1(b, c, d, e, a, w[9]);
	SHA1_ITER_1(a, b, c, d, e, w[10]);
	SHA1_ITER_1(e, a, b, c, d, w[11]);
	SHA1_ITER_1(d, e, a, b, c, w[12]);
	SHA1_ITER_1(c, d, e, a, b, w[13]);
	SHA1_ITER_1(b, c, d, e, a, w[14]);
	SHA1_ITER_1(a, b, c, d, e, w[15]);
	SHA1_ITER_1(e, a, b, c, d, W(16));
	SHA1_ITER_1(d, e, a, b, c, W(17));
	SHA1_ITER_1(c, d, e, a, b, W(18));
	SHA1_ITER_1(b, c, d, e, a, W(19));

	SHA1_ITER_2(a, b, c, d, e, W(20));
	SHA1_ITER_2(e, a, b, c, d, W(21));
	SHA1_ITER_2(d, e, a, b, c, W(22));
	SHA1_ITER_2(c, d, e, a, b, W(23));
	SHA1_ITER_2(b, c, d, e, a, W(24));
	SHA1_ITER_2(a, b, c, d, e, W(25));
	SHA1_ITER_2(e, a, b, c, d, W(26));
	SHA1_ITER_2(d, e, a, b, c, W(27));
	SHA1_ITER_2(c, d, e, a, b, W(28));
	SHA1_ITER_2(b, c, d, e, a, W(29));
	SHA1_ITER_2(a, b, c, d, e, W(30));
	SHA1_ITER_2(e, a, b, c, d, W(31));
	SHA1_ITER_2(d, e, a, b, c, W(32));
	SHA1_ITER_2(c, d, e, a, b, W(33));
	SHA1_ITER_2(b, c, d, e, a, W(34));
	SHA1_ITER_2(a, b, c, d, e, W(35));
	SHA1_ITER_2(e, a, b, c, d, W(36));
	SHA1_ITER_2(d, e, a, b, c, W(37));
	SHA1_ITER_2(c, d, e, a, b, W(38));
	SHA1_ITER_2(b, c, d, e, a, W(39));

	SHA1_ITER_3(a, b, c, d, e, W(40));
	SHA1_ITER_3(e, a, b, c, d, W(41));
	SHA1_ITER_3(d, e, a, b, c, W(42));
	SHA1_ITER_3(c, d, e, a, b, W(43));
	SHA1_ITER_3(b, c, d, e, a, W(44));
	SHA1_ITER_3(a, b, c, d, e, W(45));
	SHA1_ITER_3(e, a, b, c, d, W(46));
	SHA1_ITER_3(d, e, a, b, c, W(47));
	SHA1_ITER_3(c, d, e, a, b, W(48));
	SHA1_ITER_3(b, c, d, e, a, W(49));
	SHA1_ITER_3(a, b, c, d, e, W(50));
	SHA1_ITER_3(e, a, b, c, d, W(51));
	SHA1_ITER_3(d, e, a, b, c, W(52));
	SHA1_ITER_3(c, d, e, a, b, W(53));
	SHA1_ITER_3(b, c, d, e, a, W(54));
	SHA1_ITER_3(a, b, c, d, e, W(55));
	SHA1_ITER_3(e, a, b, c, d, W(56));
	SHA1_ITER_3(d, e, a, b, c, W(57));
	SHA1_ITER_3(c, d, e, a, b, W(58));
	SHA1_ITER_3(b, c, d, e, a, W(59));

	SHA1_ITER_4(a, b, c, d, e, W(60));
	SHA1_ITER_4(e, a, b, c, d, W(61));
	SHA1_ITER_4(d, e, a, b, c, W(62));
	SHA1_ITER_4(c, d, e, a, b, W(63));
	SHA1_ITER_4(b, c, d, e, a, W(64));
	SHA1_ITER_4(a, b, c, d, e, W(65));
	SHA1_ITER_4(e, a, b, c, d, W(66));
	SHA1_ITER_4(d, e, a, b, c, W(67));
	SHA1_ITER_4(c, d, e, a, b, W(68));
	SHA1_ITER_4(b, c, d, e, a, W(69));
	SHA1_ITER_4(a, b, c, d, e, W(70));
	SHA1_ITER_4(e, a, b, c, d, W(71));
	SHA1_ITER_4(d, e, a, b, c, W(72));
	SHA1_ITER_4(c, d, e, a, b, W(73));
	SHA1_ITER_4(b, c, d, e, a, W(74));
	SHA1_ITER_4(a, b, c, d, e, W(75));
	SHA1_ITER_4(e, a, b, c, d, W(76));
	SHA1_ITER_4(d, e, a, b, c, W(77));
	SHA1_ITER_4(c, d, e, a, b, W(78));
	SHA1_ITER_4(b, c, d, e, a, W(79));

	ctx->h0 += a;
	ctx->h1 += b;
	ctx->h2 += c;
	ctx->h3 += d;
	ctx->h4 += e;
}
#endif

void sha1_update(sha1_ctx *ctx, uint8_t const *m, uint32_t len) {

	uint32_t fill;
	uint32_t left;

	fill = (uint32_t)ctx->len & (SHA1_CHUNK_SIZE - 1);
	left = SHA1_CHUNK_SIZE - fill;

	ctx->len += len;

	if ((fill > 0) && (len >= left)) {
		memcpy(ctx->buf + fill, m, left);
		sha1_process(ctx, ctx->buf);
		m += left;
		len -= left;
		fill = 0;
	}

	while (len >= SHA1_CHUNK_SIZE) {
		sha1_process(ctx, m);
		m += SHA1_CHUNK_SIZE;
		len -= SHA1_CHUNK_SIZE;
	}

	if (len > 0) {
		memcpy(ctx->buf + fill, m, len);
	}
}

// padding zeros used in finalization
static const uint8_t pad[SHA1_CHUNK_SIZE] = {
		0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

#define WRITE_BE_64(b, x) b[0] = (uint8_t)((x) >> 56); b[1] = (uint8_t)((x) >> 48); \
b[2] = (uint8_t)((x) >> 40); b[3] = (uint8_t)((x) >> 32); \
b[4] = (uint8_t)((x) >> 24); b[5] = (uint8_t)((x) >> 16); \
b[6] = (uint8_t)((x) >> 8); b[7] = (uint8_t)(x);

void sha1_finish(sha1_ctx *ctx)
{
	uint8_t be_len[8];
	uint64_t bit_len;
	uint32_t fill;
	uint32_t n_pad;

	bit_len = ctx->len << 3;

	WRITE_BE_64(be_len, bit_len);
	fill = (uint32_t)ctx->len & (SHA1_CHUNK_SIZE - 1);

	n_pad = (fill < 56) ? (56 - fill) : (120 - fill);

	sha1_update(ctx, pad, n_pad);
	sha1_update(ctx, be_len, 8);
}

void sha1_hash(sha1_ctx *ctx, uint8_t const *m, uint32_t len) {
	sha1_init(ctx);
	sha1_update(ctx, m, len);
	sha1_finish(ctx);
}

#define WRITE_BE_32(b, i, x) b[(i)] = (uint8_t)((x) >> 24); b[(i) + 1] = (uint8_t)((x) >> 16); \
		b[(i) + 2] = (uint8_t)((x) >> 8); b[(i) + 3] = (uint8_t)(x);

void sha1_digest(sha1_ctx *ctx, uint8_t d[20]) {
	WRITE_BE_32(d, 0, ctx->h0);
	WRITE_BE_32(d, 4, ctx->h1);
	WRITE_BE_32(d, 8, ctx->h2);
	WRITE_BE_32(d, 12, ctx->h3);
	WRITE_BE_32(d, 16, ctx->h4);
}

static int check(sha1_ctx *ctx, uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4) {
	return ((ctx->h0 == h0) && (ctx->h1 == h1) && (ctx->h2 == h2) && (ctx->h3 == h3) && (ctx->h4 == h4));
}

int sha1_test(void) {
	sha1_ctx ctx;
	int i;
	uint8_t d[20];

	sha1_hash(&ctx, (uint8_t const *)"abc", 3);
	if (!check(&ctx, 0xa9993e36, 0x4706816a, 0xba3e2571, 0x7850c26c, 0x9cd0d89d)) return 0;

	sha1_hash(&ctx, (uint8_t const *)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
	if (!check(&ctx, 0x84983e44, 0x1c3bd26e, 0xbaae4aa1, 0xf95129e5, 0xe54670f1)) return 0;
	
	sha1_hash(&ctx, (uint8_t const *)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu", 112);
	if (!check(&ctx, 0xa49b2446, 0xa02c645b, 0xf419f995, 0xb6709125, 0x3a04a259)) return 0;

	sha1_init(&ctx);
	for (i = 0; i < 10000; i++)
		sha1_update(&ctx, (uint8_t const *)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 100);
	sha1_finish(&ctx);
	if (!check(&ctx, 0x34aa973c, 0xd4c4daa4, 0xf61eeb2b, 0xdbad2731, 0x6534016f)) return 0;

	sha1_init(&ctx);
	sha1_finish(&ctx);
	if (!check(&ctx, 0xda39a3ee, 0x5e6b4b0d, 0x3255bfef, 0x95601890, 0xafd80709)) return 0;

	ctx.h0 = 0;
	ctx.h1 = 0;
	ctx.h2 = 0;
	ctx.h3 = 0;
	ctx.h4 = 0;

	sha1_digest(&ctx, d);

	for (i = 0; i < 20; i++) {
		if (d[i] != 0) return 0;
	}

	ctx.h0 = 0x01020304;
	ctx.h1 = 0x05060708;
	ctx.h2 = 0x090a0b0c;
	ctx.h3 = 0x0d0e0f10;
	ctx.h4 = 0x11121314;

	sha1_digest(&ctx, d);

	for (i = 0; i < 20; i++) {
		if (d[i] != (i + 1)) return 0;
	}

	return 1;
}
