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
#include "opgp.h"
#include "cast128.h"
#include "sha1.h"
#include "tinfl.h"

/// error codes
typedef enum {
	OK,
	STREAM_ERROR,
	INVALID_PACKET_HEADER,
	UNHANDLED,
	MEM_ALLOC_ERROR
} err_t;

/// key chain context
typedef struct key_s {
	uint8_t algo;
	uint8_t salt[8];
	uint32_t count;
	uint32_t computed;
	uint8_t key[SHA1_DIGEST_SIZE];
	struct key_s *next;
} key_t;

/// OpenPGP encrypted block stream context
typedef struct
{
	mpgp_sread_t sread;
	mpgp_seof_t seof;
	mpgp_serror_t serror;
	mpgp_sclose_t sclose;
	mpgp_stream_t *in;
	cast128_cfb_ctx *ctx;
	size_t len;
} mpgp_encr_stream_t;

// read stream
static size_t encr_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb) {
	mpgp_encr_stream_t *s;
	size_t n;

	s = (mpgp_encr_stream_t*) stream;

	n = size * nmemb;
	n = (n > s->len) ? s->len : n;
	s->len -= n;

	n = mpgp_sread(s->in, ptr, 1, n);
	cast128_cfb_decrypt(s->ctx, (uint8_t*)ptr, (uint8_t*)ptr, n);

	return n;
}

// stream end of file
static int encr_seof(mpgp_stream_t *stream) {
	mpgp_encr_stream_t *s;

	s = (mpgp_encr_stream_t*) stream;

	if (s->len == 0) return 1;

	return mpgp_seof(s->in);
}

// stream error
static int encr_serror(mpgp_stream_t *stream) {
	mpgp_encr_stream_t *s;

	s = (mpgp_encr_stream_t*) stream;

	return mpgp_serror(s->in);
}

// stream close
static int encr_sclose(mpgp_stream_t *stream) {
	mpgp_encr_stream_t *s;
	mpgp_stream_t *in;

	s = (mpgp_encr_stream_t*) stream;
	in = s->in;

	free(s->ctx);
	free(s);
	return mpgp_sclose(in);
}

#define SALTED_S2K 1

// compute a symmetric key from passphrase and salt
static void compute_key(key_t *key, uint8_t const *passphrase, uint32_t passphrase_len) {
	sha1_ctx ctx;

	uint32_t len;
	uint32_t n, m, p;

	// check if key already has been computed
	if (key->computed != 0) return;

	// key is computed by hashing the passphrase and salt repeatedly
	// until the number of bytes specified in the s2k block have been
	// hashed
	len = passphrase_len + 8;  // passphrase_len + salt length

	// special case for salted (not iterated) S2K
	if (key->count == SALTED_S2K) key->count = len;
	
	n = key->count / len; // # of complete passes over material
	m = key->count % len; // remaining # of bytes to hash

	sha1_init(&ctx);
	while (n--) {
		sha1_update(&ctx, key->salt, 8);
		sha1_update(&ctx, passphrase, passphrase_len);
	}
	p = (m > 8) ? 8 : m;
	sha1_update(&ctx, key->salt, p);
	m -= p;
	sha1_update(&ctx, passphrase, m);
	sha1_finish(&ctx);
	sha1_digest(&ctx, key->key);
	// flag that key has been computed
	key->computed = 1;
}

// check if this key is the correct key for decrypting a symmetric block
// pad is the encrypted bytes prepended to the encrypted data
// keys is first item in key chain
// passphrase is of length passphrase_len
static cast128_cfb_ctx *check_key(uint8_t pad[10], key_t *keys, uint8_t const *passphrase, uint32_t passphrase_len) {

	cast128_cfb_ctx *ctx;
	uint8_t padd[10];

	ctx = malloc(sizeof(cast128_cfb_ctx));
	if (ctx == NULL) return NULL;

	while (keys != NULL) {
		// ensure that key has been computed from s2k material
		compute_key(keys, passphrase, passphrase_len);
		
		cast128_cfb_init(ctx, keys->key, NULL);
		cast128_cfb_decrypt(ctx, padd, pad, 10);

		// The last two cleartext prepended bytes are
		// the same as the preceding two bytes. This is
		// used as a check that we are using the right key 
		if ((padd[6] == padd[8]) && (padd[7] == padd[9])) {
			// the CFB IV is reinitialized with the last
			// 8 bytes of the encrypted prepended bytes
			cast128_cfb_reinit(ctx, pad + 2);
			// key is correct - return decryptor context
			return ctx;
		}

		keys = keys->next;
	}

	// no correct key found - free context memory
	free(ctx);
	return NULL;

}

// open encrypted stream
// in is input stream
// keys is first key in key chain
// passphrase is of length passphrase_len
// packet_len is packet length
static mpgp_stream_t *mpgp_encr_sopen(mpgp_stream_t *in, key_t *keys, uint8_t const *passphrase, uint32_t passphrase_len, size_t packet_len) {
	mpgp_encr_stream_t *s;
	uint8_t pad[10];
	cast128_cfb_ctx *ctx;

	// read prepended bytes
	if (mpgp_sread(in, pad, 1, 10) != 10) return NULL;

	// check if we have correct key
	if ((ctx = check_key(pad, keys, passphrase, passphrase_len)) == NULL) return NULL;

	s = malloc(sizeof(mpgp_encr_stream_t));
	if (s != NULL) {
		s->sread = encr_sread;
		s->seof = encr_seof;
		s->serror = encr_serror;
		s->sclose = encr_sclose;
		s->in = in;
		s->ctx = ctx;
		s->len = packet_len;
	}
	else {
		free(ctx);
	}

	return (mpgp_stream_t*)s;
}

// buffer sizes for compressed stream
#define Z_BUF_SIZE 32
#define Z_OUT_BUF_SIZE TINFL_LZ_DICT_SIZE

// OpenPGP compressed block stream context
typedef struct
{
	mpgp_sread_t sread;
	mpgp_seof_t seof;
	mpgp_serror_t serror;
	mpgp_sclose_t sclose;
	mpgp_stream_t *in;
	tinfl_decompressor d;
	int flags;
	uint8_t buf[Z_BUF_SIZE];
	uint32_t n;
	uint32_t pos;
	uint8_t out_buf[Z_OUT_BUF_SIZE];
	size_t avail_out;
	uint8_t *p_consumed;
	int error;
	int eof;
} mpgp_z_stream_t;

// read from file stream
static size_t z_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb) {
	mpgp_z_stream_t *s;
	size_t avail_in;
	size_t total_out;
	size_t in_bytes;
	size_t out_bytes;
	tinfl_status status;
	int flags;

	uint8_t *p;
	uint8_t *p_out;

	size *= nmemb; // total # of bytes requested
	if (size == 0) return 0; // early return if zero length request
	s = (mpgp_z_stream_t*) stream; 
	p = (uint8_t*)ptr;
	// pointer to next output byte to output
	p_out = s->out_buf + Z_OUT_BUF_SIZE - s->avail_out;
	avail_in = s->n - s->pos;

	total_out = 0;

	// consume already decompressed bytes if available
	while ((size) && (s->p_consumed < p_out)) {
		p[total_out] = *s->p_consumed;
		total_out++;
		s->p_consumed++;
		size--;
	}
	if (!size) return total_out;

	for ( ; ; ) {

		// read new input if needed
		if (avail_in == 0) {
			s->n = avail_in = mpgp_sread(s->in, s->buf, 1, Z_BUF_SIZE);
			s->pos = 0;
		}

		in_bytes = avail_in;
		out_bytes = s->avail_out;

		flags = (avail_in ? TINFL_FLAG_HAS_MORE_INPUT : 0) | s->flags;

		status = tinfl_decompress(&(s->d), s->buf + s->pos, &in_bytes, s->out_buf, p_out, &out_bytes, flags);

		avail_in -= in_bytes;
		s->pos += in_bytes;

		s->avail_out -= out_bytes;
		p_out += out_bytes;

		// consume decompressed output
		while ((size) && (s->p_consumed < p_out)) {
			p[total_out] = *s->p_consumed;
			total_out++;
			s->p_consumed++;
			size--;
		}
		if (!size) return total_out;

		if (!s->avail_out) {
			p_out = s->buf;
			s->p_consumed = s->buf;
			s->avail_out = Z_OUT_BUF_SIZE;
		}

		if (status < TINFL_STATUS_DONE) {
			s->error = 1;
			return total_out;
		}

		if ((status <= TINFL_STATUS_DONE)) {
			s->eof = 1;
			break;
		}

	}

	return total_out;
}

// stream end of file
static int z_seof(mpgp_stream_t *stream) {
	mpgp_z_stream_t *s;

	s = (mpgp_z_stream_t*) stream;

	if (s->eof) return 1;
	return 0;
}

// stream error
static int z_serror(mpgp_stream_t *stream) {
	mpgp_z_stream_t *s;

	s = (mpgp_z_stream_t*) stream;

	if (s->error) return s->error;

	return mpgp_serror(s->in);
}

// stream close
static int z_sclose(mpgp_stream_t *stream) {
	mpgp_z_stream_t *s;
	mpgp_stream_t *in;

	s = (mpgp_z_stream_t*) stream;
	in = s->in;

	free(s);
	return mpgp_sclose(in);
}

// open OpenPGP compressed block
static mpgp_stream_t *mpgp_z_sopen(mpgp_stream_t *in) {
	mpgp_z_stream_t *s;
	uint8_t algo;
	int flags;

	// first byte signals algorithm used
	if (mpgp_sread(in, &algo, 1, 1) != 1) return NULL;
	
	// 1 -> ZIP
	// 2 -> ZLIB
	switch (algo) {
		case 1:
		flags = 0;
		break;
		case 2:
		flags = 1;
		break;
		default:
		return NULL;
	}

	s = malloc(sizeof(mpgp_z_stream_t));
	if (s != NULL) {
		s->sread = z_sread;
		s->seof = z_seof;
		s->serror = z_serror;
		s->sclose = z_sclose;
		s->in = in;
		s->flags = flags;
		s->n = 0;
		s->pos = 0;
		s->avail_out = Z_OUT_BUF_SIZE;
		s->p_consumed = s->out_buf;
		s->error = 0;
		s->eof = 0;
		tinfl_init(&(s->d));
	}

	return (mpgp_stream_t*)s;
}

// OpenPGP literal block stream context
typedef struct
{
	mpgp_sread_t sread;
	mpgp_seof_t seof;
	mpgp_serror_t serror;
	mpgp_sclose_t sclose;
	mpgp_stream_t *in;
	size_t len;
} mpgp_lit_stream_t;

// read from stream
static size_t lit_sread(mpgp_stream_t *stream, void *ptr, size_t size, size_t nmemb) {
	mpgp_lit_stream_t *s;
	size_t n;

	s = (mpgp_lit_stream_t*) stream;
	n = size * nmemb;
	n = (n > s->len) ? s->len : n;
	s->len -= n;

	return mpgp_sread(s->in, ptr, 1, n);
}

// stream end of file
static int lit_seof(mpgp_stream_t *stream) {
	mpgp_lit_stream_t *s;

	s = (mpgp_lit_stream_t*) stream;

	if (s->len == 0) return 1;

	return mpgp_seof(s->in);
}

// stream error
static int lit_serror(mpgp_stream_t *stream) {
	mpgp_lit_stream_t *s;

	s = (mpgp_lit_stream_t*) stream;

	return mpgp_serror(s->in);
}

// stream close
static int lit_sclose(mpgp_stream_t *stream) {
	mpgp_lit_stream_t *s;
	mpgp_stream_t *in;

	s = (mpgp_lit_stream_t*) stream;
	in = s->in;

	free(s);
	return mpgp_sclose(in);
}

#define LIT_BUF_SIZE (1 << 3)

// open a literal block stream
static mpgp_stream_t *mpgp_lit_sopen(mpgp_stream_t *in, size_t packet_len) {
	mpgp_lit_stream_t *s;
	uint8_t buf[LIT_BUF_SIZE];
	uint8_t type;
	uint8_t len;
	size_t n;

	// message type
	if (mpgp_sread(in, &type, 1, 1) != 1) return NULL;
	if (type != 'b') return NULL;
	// file name length
	if (mpgp_sread(in, &len, 1, 1) != 1) return NULL;
	packet_len -= 6 + len;
	// file name
	while (len > 0) {
		n = (len > LIT_BUF_SIZE) ? LIT_BUF_SIZE : len;
		if (mpgp_sread(in, buf, 1, n) != n) return NULL;
		len -= n;
	}
	// date
	if (mpgp_sread(in, buf, 1, 4) != 4) return NULL;

	s = malloc(sizeof(mpgp_lit_stream_t));
	if (s != NULL) {
		s->sread = lit_sread;
		s->seof = lit_seof;
		s->serror = lit_serror;
		s->sclose = lit_sclose;
		s->in = in;
		s->len = packet_len;
	}

	return (mpgp_stream_t*)s;
}

// read OpenPGP packet header
// tag (output) is the packet tag (type)
// length (output) is the packet length
// have_length is a flag telling if the packet length is known
static err_t read_packet_header(mpgp_stream_t *s, uint32_t *tag, uint32_t *length, int *have_length) {
	uint8_t b;
	uint8_t buf[4];

	if (mpgp_sread(s, &b, 1, 1) != 1) return STREAM_ERROR;

	// First byte of header should have bit 7 set
	if ((b & 0x80) == 0) return INVALID_PACKET_HEADER;

	// Bit 6 indicates new or old packet header format
	if ((b & 0x40) == 0) {
		// old packet format
		// bits 5 to 2 contain packet tag
		*tag = (b & 0x3C) >> 2;
		// bits 1 to 0 contain length type
		switch (b & 0x3) {
			case 0:
			// one-octet length
			if (mpgp_sread(s, buf, 1, 1) != 1) return STREAM_ERROR;
			*length = buf[0];
			break;
			case 1:
			// two-octect length
			if (mpgp_sread(s, buf, 1, 2) != 2) return STREAM_ERROR;
			*length = (buf[0] << 8) | buf[1];
			break;
			case 2:
			// four-octect length
			if (mpgp_sread(s, buf, 1, 4) != 4) return STREAM_ERROR;
			*length = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
			break;
			case 3:
			// indeterminate length
			*have_length = 0;
			return OK;
			break;
		}
	}
	else {
		// new packet format
		// bits 5 to 0 contain packet tag
		*tag = b & 0x3F;
		if (mpgp_sread(s, buf, 1, 1) != 1) return STREAM_ERROR;
		if (buf[0] < 192) {
			// one-octet length
			*length = buf[0];
		}
		else if (buf[0] <= 223) {
			// two-octet length
			if (mpgp_sread(s, buf + 1, 1, 1) != 1) return STREAM_ERROR;
			*length = ((buf[0] - 192) << 8) + buf[1] + 192;
		}
		else if (buf[0] == 255) {
			// four octet length
			if (mpgp_sread(s, buf, 1, 4) != 4) return STREAM_ERROR;
			*length = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
		}
		else {
			return UNHANDLED;
		}
	}

	*have_length = 1;
	return OK;
}

// read OpenPGP string-to-key block, adding information to key chain
// specified by keys
static err_t read_s2k(mpgp_stream_t *stream, key_t **keys) {
	uint8_t s2k_type;
	uint8_t c;
	key_t key;
	key_t *k;

	if (mpgp_sread(stream, &s2k_type, 1, 1) != 1) return STREAM_ERROR;

	// 2 is a reserved value for the S2K type
	if (s2k_type == 2) return UNHANDLED;

	if (s2k_type <= 3) {
		if (mpgp_sread(stream, &key.algo, 1, 1) != 1) return STREAM_ERROR;
		// we only handle SHA-1 (algo == 2)
		if (key.algo != 2) return UNHANDLED;
		if (s2k_type > 0) {
			// read salt
			if (mpgp_sread(stream, key.salt, 1, 8) != 8) return STREAM_ERROR;

			// determine how many bytes should be hashed to compute key
			if (s2k_type > 1) {
				if (mpgp_sread(stream, &c, 1, 1) != 1) return STREAM_ERROR;
				key.count = (((uint32_t) 16) + (c & 15)) << ((c >> 4) + 6);
			}
			else {
				key.count = SALTED_S2K;
			}
		}
		else {
			key.count = 0;
		}

		k = malloc(sizeof(key_t));
		if (k == NULL) return MEM_ALLOC_ERROR;

		memcpy(k, &key, sizeof(key_t));
		k->next = *keys;
		k->computed = 0;
		*keys = k;

		return OK;
	}

	return UNHANDLED;

}

// read Symmetric-Key Encrypted Session Key packet from stream
static err_t read_skesk_packet(mpgp_stream_t *stream, key_t **keys) {
	uint8_t version;
	uint8_t algo;
	err_t r;

	// read version
	if (mpgp_sread(stream, &version, 1, 1) != 1) return STREAM_ERROR;
	// only defined version is 4
	if (version != 4) return UNHANDLED;
	// read symmetric algorithm
	if (mpgp_sread(stream, &algo, 1, 1) != 1) return STREAM_ERROR;
	// we only handle CAST5 for now
	switch (algo) {
		case 3:
		// CAST5
		break;
		default:
		return UNHANDLED;
	}
	if ((r = read_s2k(stream, keys)) != OK) return r;

	return OK;
}

// free memory allocated for key chain
static void free_keys(key_t **keys) {
	key_t *next;

	while (*keys != NULL) {
		next = (*keys)->next;
		free(*keys);
		*keys = next;
	}
}

#define SKESK_TAG 3
#define COMPRESSED_TAG 8
#define SYM_ENCR_TAG 9
#define LITERAL_TAG 11

// recursively parse OpenPGP packets from stream
static mpgp_stream_t *mpgp_open_r(mpgp_stream_t *stream, uint8_t const *passphrase, uint32_t passphrase_len, key_t **keys) {
	uint32_t tag;
	uint32_t length;
	int have_length;
	mpgp_stream_t *s;

	tag = 0;
	length = 0;
	have_length = 0;
	for (; ;) {
		if (read_packet_header(stream, &tag, &length, &have_length) != OK) goto bail;

		switch (tag) {
			case SKESK_TAG: // Symmetric-Key Encrypted Session Key packet
			if (read_skesk_packet(stream, keys) != OK) goto bail;
			break;
			case COMPRESSED_TAG: // Compressed packet
			if ((s = mpgp_z_sopen(stream)) == NULL) goto bail;
			return mpgp_open_r(s, passphrase, passphrase_len, keys);
			break;
			case SYM_ENCR_TAG: // Symmetrically Encrypted packet
			if ((s = mpgp_encr_sopen(stream, *keys, passphrase, passphrase_len, length)) == NULL) goto bail;
			return mpgp_open_r(s, passphrase, passphrase_len, keys);
			break;
			case LITERAL_TAG: // literal packet
			if ((s = mpgp_lit_sopen(stream, length)) == NULL) goto bail;
			return s;
			break;
			default:
			goto bail;
		}
	}

	bail:
	mpgp_sclose(stream);
	return NULL;

}

mpgp_stream_t *mpgp_open(mpgp_stream_t *stream, uint8_t const *passphrase, uint32_t passphrase_len) {

	mpgp_stream_t *s;
	key_t *keys;
	keys = NULL;

	s = mpgp_open_r(stream, passphrase, passphrase_len, &keys);
	free_keys(&keys);
	return s;
}
