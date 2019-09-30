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
 *  SWAN64_k128.c
 *
 *  Description: The c file of SWAN64.h. The key schedule is precomputed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-09-01
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Tao Sun, Guojun Tang, Bing Sun, Siwei Sun, Zhaoji Xu, Yingjie Zhang.
 */

#include "swan64_k128.h"
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

#define ROTATE_128(k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k0 << (32 - bits);\
    k0 = (k0 >> bits) | (k1 << (32 - bits));\
    k1 = (k1 >> bits) | (k2 << (32 - bits));\
    k2 = (k2 >> bits) | (k3 << (32 - bits));\
    k3 = (k3 >> bits) | tmp;\
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

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char* Subkey)
{
	uint32_t* masterKey = (uint32_t*)Seedkey;
	uint32_t *ekey = (uint32_t*)Subkey;
	const uint16_t rounds = ROUNDS64_K128;
	uint32_t  k0, k1, k2, k3, tmp;
	int i;
	k0 = masterKey[0];
	k1 = masterKey[1];
	k2 = masterKey[2];
	k3 = masterKey[3];
	for(i = 0; i < rounds * 2 ; i++){
		ROTATE_128(k3, k2, k1, k0, tmp, 24);
		k0 += ROUND_DELTA[i];
		ekey[i] = k0;
	}
}

//The encrypiton interface of SWAN64-K128 in the precompute round key mode
int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
{
	const uint8_t *plain = (uint8_t*)input;
	const uint8_t rounds = ROUNDS64_K128;
	uint8_t *cipher = (uint8_t*)output;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	const uint8_t (*subkey)[4] = (uint8_t(*)[4])key;
	size_t i;
	L0 = plain[0];
	L1 = plain[1];
	L2 = plain[2];
	L3 = plain[3];
	R0 = plain[4];
	R1 = plain[5];
	R2 = plain[6];
	R3 = plain[7];
	for (i = 0; i < rounds/4; i++)
	{
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i][0], subkey[8*i][1], subkey[8*i][2], subkey[8*i][3],\
		 subkey[8*i+1][0], subkey[8*i+1][1], subkey[8*i+1][2], subkey[8*i+1][3]);
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+2][0], subkey[8*i+2][1], subkey[8*i+2][2], subkey[8*i+2][3],\
		 subkey[8*i+3][0], subkey[8*i+3][1], subkey[8*i+3][2], subkey[8*i+3][3]);
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+4][0], subkey[8*i+4][1], subkey[8*i+4][2], subkey[8*i+4][3],\
		 subkey[8*i+5][0], subkey[8*i+5][1], subkey[8*i+5][2], subkey[8*i+5][3]);
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+6][0], subkey[8*i+6][1], subkey[8*i+6][2], subkey[8*i+6][3],\
		 subkey[8*i+7][0], subkey[8*i+7][1], subkey[8*i+7][2], subkey[8*i+7][3]);
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

//The decrypiton interface of SWAN64-K128 in the precompute round key mode
int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen)
{
	const uint8_t *cipher = (uint8_t*)input;
	const uint8_t rounds = ROUNDS64_K128;
	uint8_t *plain = (uint8_t*)output;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	const uint8_t (*subkey)[4] = (uint8_t(*)[4])key;
	size_t i;
	L0 = cipher[0];
	L1 = cipher[1];
	L2 = cipher[2];
	L3 = cipher[3];
	R0 = cipher[4];
	R1 = cipher[5];
	R2 = cipher[6];
	R3 = cipher[7];
	for (i = rounds/4 - 1;i <= rounds/4 - 1; i--)
	{
		SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+7][0], subkey[8*i+7][1], subkey[8*i+7][2], subkey[8*i+7][3],\
		 subkey[8*i+6][0], subkey[8*i+6][1], subkey[8*i+6][2], subkey[8*i+6][3]);
		SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+5][0], subkey[8*i+5][1], subkey[8*i+5][2], subkey[8*i+5][3],\
		 subkey[8*i+4][0], subkey[8*i+4][1], subkey[8*i+4][2], subkey[8*i+4][3]);
		SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+3][0], subkey[8*i+3][1], subkey[8*i+3][2], subkey[8*i+3][3],\
		 subkey[8*i+2][0], subkey[8*i+2][1], subkey[8*i+2][2], subkey[8*i+2][3]);
		SWAN_DEC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[8*i+1][0], subkey[8*i+1][1], subkey[8*i+1][2], subkey[8*i+1][3],\
		 subkey[8*i][0], subkey[8*i][1], subkey[8*i][2], subkey[8*i][3]);
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

//The single encryption round interface of SWAN64-K128 in the precompute round key mode
int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen, int CryptRound)
{
    const uint8_t *plain = (uint8_t*)input;
	const uint8_t rounds = CryptRound;
	uint8_t *cipher = (uint8_t*)output;
	uint8_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	const uint8_t (*subkey)[4] = (uint8_t(*)[4])key;
	size_t i;
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
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 subkey[2*i][0], subkey[2*i][1], subkey[2*i][2], subkey[2*i][3],\
		 subkey[2*i+1][0], subkey[2*i+1][1], subkey[2*i+1][2], subkey[2*i+1][3]);
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
    uint8_t subkey[128][4];
	uint32_t round_constant;

    printf("--------------------SWAN-64block-128keysize--------------------\n");
    printf("the plaintext\n");
    dump(in, sizeof(in));
    begin = start_rdtsc();

    for (i = 0; i < TEST; i++)
    {
        Key_Schedule(key, 128, 1,(unsigned char*)subkey);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("\nkey_schedule for keysize 128 cost %llu CPU cycles\n", (ans) / TEST);

    begin = start_rdtsc();

    for(i = 0;i < TEST;i++){
        Crypt_Enc_Block(in, 64, out, &out_len, (unsigned char*)subkey, KEY128);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("the ciphertext\n");
    dump(out, sizeof(out));
    printf("\nSWAN64K128 encrypt cost %llu CPU cycles\n", (ans) / TEST);

    begin = start_rdtsc();

    for(i = 0;i < TEST;i++){
        Crypt_Dec_Block(out, 64, in,  &out_len, (unsigned char*)subkey, KEY128);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\nSWAN64K128 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //Encryption round test of SWAN64-K128
    printf("round test-swan128-128:\n");
    Key_Schedule(key, 128, 1, (unsigned char*)subkey);
    Crypt_Enc_Block_Round(in, 64, out,  &out_len, (unsigned char*)subkey, 128, 1);
    dump(out, sizeof(out));
    return 0;
}
