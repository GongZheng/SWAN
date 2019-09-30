/*
 * Copyright (c) 2018,2019.
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
/*
 *  SWAN64_k256.c
 *
 *  Description: The c file of SWAN64.h. The key schedule is precomputed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-09-01
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Tao Sun, Guojun Tang, Bing Sun, Siwei Sun, Zhaoji Xu, Yingjie Zhang.
 */

#include "swan64_k256.h"
//Define for GCC and VS CPU Cycles tests
#ifdef __GNUC__
#include <x86intrin.h>
#endif
#ifdef _MSC_VER
#include <intrin.h>
#endif
#pragma intrinsic(__rdtsc)

//Repeat test times and calculate on average for accuracy
#define TEST 100000

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

//ShiftLanes: The first affine function before the Beta function;
#define THETA(a0, a1, a2, a3) do{\
    a1 = ROL8(a1, A);\
    a2 = ROL8(a2, B);\
    a3 = ROL8(a3, C);\
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

//Print out the result
void dump(const uint8_t *li, int len)
{
    int line_ctrl = 16;
    int i;
    for (i = 0; i < len; i++)
    {
        printf("%02X", (*li++));
        if ((i + 1) % line_ctrl == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }
}

//CPU cycles set start;
uint64_t start_rdtsc()
{
    return __rdtsc();
}

//CPU cycles set end;
uint64_t end_rdtsc()
{
    return __rdtsc();
}

//The encrypiton interface of SWAN64-K256 in the precompute round key mode
int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
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
		k0 += ROUND_DELTA[2*i];
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		ROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA[2*i+1];
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

//The decrypiton interface of SWAN64-K256 in the precompute round key mode
int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
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
		k0 += ROUND_DELTA[i];
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
		k0 -= ROUND_DELTA[j];
		INVROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		k0 -= ROUND_DELTA[j-1];
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

//The single encryption round interface of SWAN64-K256 in the precompute round key mode
int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen, int CryptRound)
{
    const uint8_t *plain = (uint8_t*)input;
	const uint8_t rounds = CryptRound;
	uint8_t *cipher = (uint8_t*)output;
	uint32_t* masterKey = (uint32_t*)key;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint8_t  key0, key1, key2, key3, key4, key5, key6, key7;
	size_t i;
	uint32_t  k0, k1, k2, k3, k4, k5, k6, k7, tmp;	
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
		k0 += ROUND_DELTA[2*i];
		key0 = k0;
		key1 = k0>>8;
		key2 = k0>>16;
		key3 = k0>>24;
		ROTATE_256(k7, k6, k5, k4, k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA[2*i+1];
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


int main()
{
    uint64_t begin;
	uint64_t end;
    uint64_t ans = 0;
    int i;

    unsigned char in[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    unsigned char out[8];
    unsigned char key[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	int out_len = 64;

    printf("--------------------SWAN-64block-256keysize--------------------\n");
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\n");

    begin = start_rdtsc();

    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block(in, 64, out, &out_len, key, KEY256);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("the ciphertext\n");
    dump(out, sizeof(out));
    printf("\nSWAN64K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);

    begin = start_rdtsc();

    for (i = 0; i < TEST; i++)
    {
        Crypt_Dec_Block(out, 64, in,  &out_len, key, KEY256);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\nSWAN64K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //Encryption round test of SWAN64-K256
    printf("round test-swan128-256:\n");
    Crypt_Enc_Block_Round(in, 64, out,  &out_len, key, 256, 1);
    dump(out, sizeof(out));
    printf("\n");

    return 0;
}
