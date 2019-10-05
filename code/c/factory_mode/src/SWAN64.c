/*
 * Copyright (c) 2018,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "SWAN.h"

const uint32_t ROUND_DELTA64[129] = {0x9e3779b9,
									0x3c6ef372,
									0xdaa66d2b,
									0x78dde6e4,
									0x1715609d,
									0xb54cda56,
									0x5384540f,
									0xf1bbcdc8,
									0x8ff34781,
									0x2e2ac13a,
									0xcc623af3,
									0x6a99b4ac,
									0x8d12e65,
									0xa708a81e,
									0x454021d7,
									0xe3779b90,
									0x81af1549,
									0x1fe68f02,
									0xbe1e08bb,
									0x5c558274,
									0xfa8cfc2d,
									0x98c475e6,
									0x36fbef9f,
									0xd5336958,
									0x736ae311,
									0x11a25cca,
									0xafd9d683,
									0x4e11503c,
									0xec48c9f5,
									0x8a8043ae,
									0x28b7bd67,
									0xc6ef3720,
									0x6526b0d9,
									0x35e2a92,
									0xa195a44b,
									0x3fcd1e04,
									0xde0497bd,
									0x7c3c1176,
									0x1a738b2f,
									0xb8ab04e8,
									0x56e27ea1,
									0xf519f85a,
									0x93517213,
									0x3188ebcc,
									0xcfc06585,
									0x6df7df3e,
									0xc2f58f7,
									0xaa66d2b0,
									0x489e4c69,
									0xe6d5c622,
									0x850d3fdb,
									0x2344b994,
									0xc17c334d,
									0x5fb3ad06,
									0xfdeb26bf,
									0x9c22a078,
									0x3a5a1a31,
									0xd89193ea,
									0x76c90da3,
									0x1500875c,
									0xb3380115,
									0x516f7ace,
									0xefa6f487,
									0x8dde6e40,
									0x2c15e7f9,
									0xca4d61b2,
									0x6884db6b,
									0x6bc5524,
									0xa4f3cedd,
									0x432b4896,
									0xe162c24f,
									0x7f9a3c08,
									0x1dd1b5c1,
									0xbc092f7a,
									0x5a40a933,
									0xf87822ec,
									0x96af9ca5,
									0x34e7165e,
									0xd31e9017,
									0x715609d0,
									0xf8d8389,
									0xadc4fd42,
									0x4bfc76fb,
									0xea33f0b4,
									0x886b6a6d,
									0x26a2e426,
									0xc4da5ddf,
									0x6311d798,
									0x1495151,
									0x9f80cb0a,
									0x3db844c3,
									0xdbefbe7c,
									0x7a273835,
									0x185eb1ee,
									0xb6962ba7,
									0x54cda560,
									0xf3051f19,
									0x913c98d2,
									0x2f74128b,
									0xcdab8c44,
									0x6be305fd,
									0xa1a7fb6,
									0xa851f96f,
									0x46897328,
									0xe4c0ece1,
									0x82f8669a,
									0x212fe053,
									0xbf675a0c,
									0x5d9ed3c5,
									0xfbd64d7e,
									0x9a0dc737,
									0x384540f0,
									0xd67cbaa9,
									0x74b43462,
									0x12ebae1b,
									0xb12327d4,
									0x4f5aa18d,
									0xed921b46,
									0x8bc994ff,
									0x2a010eb8,
									0xc8388871,
									0x6670022a,
									0x4a77be3,
									0xa2def59c,
									0x41166f55,
									0xdf4de90e,
									0x7d8562c7,
									0x1bbcdc80,
									0xb9f45639};
                                    
#define ROTATE_128(k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k0 << (32 - bits);\
    k0 = (k0 >> bits) | (k1 << (32 - bits));\
    k1 = (k1 >> bits) | (k2 << (32 - bits));\
    k2 = (k2 >> bits) | (k3 << (32 - bits));\
    k3 = (k3 >> bits) | tmp;\
}while(0);

#define INVROTATE_128(k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k3 >> (32 - bits);\
    k3 = (k3 << bits) | (k2 >> (32 - bits));\
    k2 = (k2 << bits) | (k1 >> (32 - bits));\
    k1 = (k1 << bits) | (k0 >> (32 - bits));\
    k0 = (k0 << bits) | tmp;\
}while(0);

#define ROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k0 << (32 - bits);\
    k0 = (k0 >> bits) | (k1 << (32 - bits));\
    k1 = (k1 >> bits) | (k2 << (32 - bits));\
    k2 = (k2 >> bits) | (k3 << (32 - bits));\
    k3 = (k3 >> bits) | (k4 << (32 - bits));\
	k4 = (k4 >> bits) | (k5 << (32 - bits));\
    k5 = (k5 >> bits) | (k6 << (32 - bits));\
    k6 = (k6 >> bits) | (k7 << (32 - bits));\
    k7 = (k7 >> bits) | tmp;\
}while(0);

#define INVROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k7 >> (32 - bits);\
    k7 = (k7 << bits) | (k6 >> (32 - bits));\
    k6 = (k6 << bits) | (k5 >> (32 - bits));\
    k5 = (k5 << bits) | (k4 >> (32 - bits));\
    k4 = (k4 << bits) | (k3 >> (32 - bits));\
	k3 = (k3 << bits) | (k2 >> (32 - bits));\
    k2 = (k2 << bits) | (k1 >> (32 - bits));\
    k1 = (k1 << bits) | (k0 >> (32 - bits));\
    k0 = (k0 << bits) | tmp;\
}while(0);

//ShiftLanes: The first affine function before the Beta function;
#define THETA(a0, a1, a2, a3) do{\
    a1 = ROL8(a1, A_64);\
    a2 = ROL8(a2, B_64);\
    a3 = ROL8(a3, C_64);\
}while(0);

//ShiftLanes: The first affine function before the Beta function;
#define BETA(b0, b1, b2, b3, a0, a1, a2, a3) do {\
    a0 = b0;\
    a1 = b1;\
    a2 = b2;\
    a3 = b3;\
	b0 = ~(a0 ^ a1 ^ a3 ^ (a2 & a3));\
    b1 = a0 ^ (a0 & a1) ^ a2 ^ (a0 & a3) ^ (a1 & a3) ^ (a0 & a1 & a3) ^ (a2 & a3) ^ (a0 & a2 & a3) ^ (a1 & a2 & a3);\
    b2 = a1 ^ a2 ^ (a0 & a2) ^ a3 ^ (a0 & a1 & a3) ^ (a1 & a2 & a3);\
    b3 = a1 ^ (a0 & a1) ^ (a0 & a2) ^ (a0 & a3) ^ (a2 & a3) ^ (a0 & a2 & a3);\
}while(0);

//SwitchLanes:The almost MDS affine function based on XOR;
#define RHO(a0, a1, a2, a3, t) do{\
    t = a0 ^ a1 ^ a2 ^a3;\
    a0 ^= t;\
    a1 ^= t;\
    a2 ^= t;\
    a3 ^= t;\
}while(0);

#define SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3, K0, K1, K2, K3, K4, K5, K6, K7) do{\
    T0 = L0;\
    T1 = L1;\
    T2 = L2;\
    T3 = L3;\
    THETA(T0, T1, T2, T3);\
    T0 ^= K0;\
    T1 ^= K1;\
    T2 ^= K2;\
    T3 ^= K3;\
    BETA(T0, T1, T2, T3, tmp0, tmp1, tmp2, tmp3);\
	THETA(T0, T1, T2, T3);\
    RHO(T0, T1, T2, T3, tmp0);\
    R0 ^= T0;\
    R1 ^= T1;\
    R2 ^= T2;\
    R3 ^= T3;\
    /* half round */ \
    T0 = R0;\
    T1 = R1;\
    T2 = R2;\
    T3 = R3;\
    THETA(T0, T1, T2, T3);\
    T0 ^= K4;\
    T1 ^= K5;\
    T2 ^= K6;\
    T3 ^= K7;\
    BETA(T0, T1, T2, T3, tmp0, tmp1, tmp2, tmp3);\
	THETA(T0, T1, T2, T3);\
    RHO(T0, T1, T2, T3, tmp0);\
    L0 ^= T0;\
    L1 ^= T1;\
    L2 ^= T2;\
    L3 ^= T3;\
}while(0);

#define SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3, K0, K1, K2, K3, K4, K5, K6, K7) do{\
    T0 = R0;\
    T1 = R1;\
    T2 = R2;\
    T3 = R3;\
    THETA(T0, T1, T2, T3);\
    T0 ^= K0;\
    T1 ^= K1;\
    T2 ^= K2;\
    T3 ^= K3;\
    BETA(T0, T1, T2, T3, tmp0, tmp1, tmp2, tmp3);\
	THETA(T0, T1, T2, T3);\
    RHO(T0, T1, T2, T3, tmp0);\
    L0 ^= T0;\
    L1 ^= T1;\
    L2 ^= T2;\
    L3 ^= T3;\
    /* half round */ \
    T0 = L0;\
    T1 = L1;\
    T2 = L2;\
    T3 = L3;\
    THETA(T0, T1, T2, T3);\
    T0 ^= K4;\
    T1 ^= K5;\
    T2 ^= K6;\
    T3 ^= K7;\
    BETA(T0, T1, T2, T3, tmp0, tmp1, tmp2, tmp3);\
	THETA(T0, T1, T2, T3);\
    RHO(T0, T1, T2, T3, tmp0);\
    R0 ^= T0;\
    R1 ^= T1;\
    R2 ^= T2;\
    R3 ^= T3;\
}while(0);

int Crypt_Enc_Block64_k128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
{
	const uint8_t *plain = (uint8_t*)input;
	const uint8_t rounds = ROUNDS64_K128;
	uint8_t *cipher = (uint8_t*)output;
	uint32_t* masterKey = (uint32_t*)key;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint8_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  k0, k1, k2, k3, tmp;			
	size_t i;
	k0 = masterKey[0];
	k1 = masterKey[1];
	k2 = masterKey[2];
	k3 = masterKey[3];
	
	L0 = plain[0];
	L1 = plain[1];
	L2 = plain[2];
	L3 = plain[3];
	R0 = plain[4];
	R1 = plain[5];
	R2 = plain[6];
	R3 = plain[7];
	for (i = 0; i < rounds; i++)
	{
		ROTATE_128(k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA64[2*i];
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		ROTATE_128(k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA64[2*i+1];
		key4 = k0;
		key5 = k0>>8;
		key6 = k0>>16;
		key7 = k0>>24;
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 key0, key1, key2, key3, key4, key5, key6, key7);
	}
	cipher[0] = L0;
	cipher[1] = L1;
	cipher[2] = L2;
	cipher[3] = L3;
	cipher[4] = R0;
	cipher[5] = R1;
	cipher[6] = R2;
	cipher[7] = R3;
	return 0;
}

int Crypt_Dec_Block64_k128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
{
	const uint8_t *cipher = (uint8_t*)input;
	const uint8_t rounds = ROUNDS64_K128;
	uint32_t* masterKey = (uint32_t*)key;
	uint8_t *plain = (uint8_t*)output;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint8_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  k0, k1, k2, k3, tmp;	
	size_t i,j;		
	k0 = masterKey[0];
	k1 = masterKey[1];
	k2 = masterKey[2];
	k3 = masterKey[3];
	for(i = 0; i < rounds * 2 + 1; i++){
		ROTATE_128(k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA64[i];
	}		
	
	L0 = cipher[0];
	L1 = cipher[1];
	L2 = cipher[2];
	L3 = cipher[3];
	R0 = cipher[4];
	R1 = cipher[5];
	R2 = cipher[6];
	R3 = cipher[7];		
	j = rounds * 2;		
	for (i =  0;i < rounds ; i++)
	{
		k0 -= ROUND_DELTA64[j];
		INVROTATE_128(k3, k2, k1, k0, tmp, 24);	
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		k0 -= ROUND_DELTA64[j-1];
		INVROTATE_128(k3, k2, k1, k0, tmp, 24);
		key4 = k0;
		key5 = k0>>8;
		key6 = k0>>16;
		key7 = k0>>24;;			
		SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 key0, key1, key2, key3, key4, key5, key6, key7);			
		j = j-2;
	}
	plain[0] = L0;
	plain[1] = L1;
	plain[2] = L2;
	plain[3] = L3;
	plain[4] = R0;
	plain[5] = R1;
	plain[6] = R2;
	plain[7] = R3;
	return 0;
}

int Crypt_Enc_Block64_k256(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
{
	const uint8_t *plain = (uint8_t*)input;
	const uint8_t rounds = ROUNDS64_K256;
	uint8_t *cipher = (uint8_t*)output;
	uint32_t* masterKey = (uint32_t*)key;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint8_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  k0, k1, k2, k3, k4, k5, k6, k7, tmp;	
	size_t i;
	k0 = masterKey[0];
	k1 = masterKey[1];
	k2 = masterKey[2];
	k3 = masterKey[3];
	k4 = masterKey[4];
	k5 = masterKey[5];
	k6 = masterKey[6];
	k7 = masterKey[7];
	
	L0 = plain[0];
	L1 = plain[1];
	L2 = plain[2];
	L3 = plain[3];
	R0 = plain[4];
	R1 = plain[5];
	R2 = plain[6];
	R3 = plain[7];
	for (i = 0; i < rounds; i++)
	{
		ROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);				
		k0 += ROUND_DELTA64[2*i];
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		ROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA64[2*i+1];
		key4 = k0;
		key5 = k0>>8;
		key6 = k0>>16;
		key7 = k0>>24;
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 key0, key1, key2, key3, key4, key5, key6, key7);
	}
	cipher[0] = L0;
	cipher[1] = L1;
	cipher[2] = L2;
	cipher[3] = L3;
	cipher[4] = R0;
	cipher[5] = R1;
	cipher[6] = R2;
	cipher[7] = R3;
	return 0;
}

int Crypt_Dec_Block64_k256(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
{
	const uint8_t *cipher = (uint8_t*)input;
	const uint8_t rounds = ROUNDS64_K256;
	uint8_t *plain = (uint8_t*)output;
	uint32_t* masterKey = (uint32_t*)key;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint8_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  k0, k1, k2, k3, k4, k5, k6, k7, tmp;	
	size_t i,j;
	k0 = masterKey[0];
	k1 = masterKey[1];
	k2 = masterKey[2];
	k3 = masterKey[3];
	k4 = masterKey[4];
	k5 = masterKey[5];
	k6 = masterKey[6];
	k7 = masterKey[7];
	for(i = 0; i < rounds * 2 + 1; i++){
		ROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA64[i];
	}
	
	L0 = cipher[0];
	L1 = cipher[1];
	L2 = cipher[2];
	L3 = cipher[3];
	R0 = cipher[4];
	R1 = cipher[5];
	R2 = cipher[6];
	R3 = cipher[7];
	j = rounds * 2;	
	for (i = 0;i < rounds; i++)
	{
		k0 -= ROUND_DELTA64[j];
		INVROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		k0 -= ROUND_DELTA64[j-1];
		INVROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		key4 = k0;
		key5 = k0>>8;
		key6 = k0>>16;
		key7 = k0>>24;;			
		SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 key0, key1, key2, key3, key4, key5, key6, key7);			
		j = j-2;
	}
	plain[0] = L0;
	plain[1] = L1;
	plain[2] = L2;
	plain[3] = L3;
	plain[4] = R0;
	plain[5] = R1;
	plain[6] = R2;
	plain[7] = R3;
	return 0;
}