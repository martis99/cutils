#include "md5.h"

#include "mem.h"

#include <stdio.h>

u32 func0(u32 B, u32 C, u32 D)
{
	return (B & C) | (~B & D);
}

u32 func1(u32 B, u32 C, u32 D)
{
	return (D & B) | (~D & C);
}

u32 func2(u32 B, u32 C, u32 D)
{
	return B ^ C ^ D;
}

u32 func3(u32 B, u32 C, u32 D)
{
	return C ^ (B | ~D);
}

typedef u32 (*dgst_fn)(u32 B, u32 C, u32 D);

u32 rol(u32 r, short N)
{
	u32 mask1 = (1 << N) - 1;
	return ((r >> (32 - N)) & mask1) | ((r << N) & ~mask1);
}

static dgst_fn ff[]   = { func0, func1, func2, func3 };
static u16 M[]	      = { 1, 5, 3, 7 };
static u16 O[]	      = { 0, 1, 5, 0 };
static u16 rots[4][4] = { { 7, 12, 17, 22 }, { 5, 9, 14, 20 }, { 4, 11, 16, 23 }, { 6, 10, 15, 21 } };
static u32 k[64]      = {
	     0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122,
	     0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6,
	     0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60,
	     0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	     0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

int md5(const char *msg, size_t msg_len, byte *buf, size_t buf_size, char *out, size_t out_size)
{
	union {
		u32 h[4];
		u8 b[16];
	} o = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 };

	size_t chunk_cnt = 1 + (msg_len + 8) / 64;
	if (64 * chunk_cnt > buf_size) {
		return 1;
	}

	m_memcpy(buf, buf_size, msg, msg_len);
	buf[msg_len] = (byte)0x80;
	size_t bits  = 8 * msg_len;
	m_memcpy(buf + 64 * chunk_cnt - 8, buf_size, &bits, 4);

	for (size_t chunk = 0; chunk < chunk_cnt; chunk++) {
		u32 *w = (u32 *)(buf + chunk * 64);
		u32 A  = o.h[0];
		u32 B  = o.h[1];
		u32 C  = o.h[2];
		u32 D  = o.h[3];

		for (int i = 0; i < 4; i++) {
			dgst_fn fctn = ff[i];
			u16 *rotn    = rots[i];
			u16 m	     = M[i];
			u16 o	     = O[i];
			for (u16 j = 0; j < 16; j++) {
				u16 g = (m * j + o) % 16;
				u32 f = B + rol(A + fctn(B, C, D) + k[j + 16 * i] + w[g], rotn[j % 4]);

				A = D;
				D = C;
				C = B;
				B = f;
			}
		}

		o.h[0] += A;
		o.h[1] += B;
		o.h[2] += C;
		o.h[3] += D;
	}

	snprintf(out, out_size, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X", o.b[0], o.b[1], o.b[2], o.b[3], o.b[4], o.b[5], o.b[6], o.b[7],
		 o.b[8], o.b[9], o.b[10], o.b[11], o.b[12], o.b[13], o.b[14], o.b[15]);

	return 0;
}
