/*
 * Copyright (c) 2018, 2019.
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
 *  SWAN256.c
 *
 *  Description: The c file of SWAN256. The key schedule is precomputed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-07-13
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Bing Sun, Siwei Sun, Tao Sun, Guojun Tang, Zhaoji Xu, Yingjie Zhang.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "swan256.h"
#define TEST 100000

#define ROTATE(k0, k1, k2, k3, k4, k5, k6, k7, tmp1, tmp2, tmp3, tmp4) do{\
    tmp1 = (k7 << 8) | (k0 >> 24);\
	tmp2 = (k6 << 8) | (k7 >> 24);\
	tmp3 = (k5 << 8) | (k6 >> 24);\
	tmp4 = (k4 << 8) | (k5 >> 24);\
	k7 = (k3 << 8) | (k4 >> 24);\
    k6 = (k2 << 8) | (k3 >> 24);\
	k5 = (k1 << 8) | (k2 >> 24);\
	k4 = (k0 << 8) | (k1 >> 24);\
	k3 = tmp1;\
    k2 = tmp2;\
    k1 = tmp3;\
	k0 = tmp4;\
}while(0);

//The bitslicing of SWAN Sboxes for the Beta function;
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
    a1 = ROL32(a1, A);\
    a2 = ROL32(a2, B);\
    a3 = ROL32(a3, C);\
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
uint64_t start_rdtsc()
{
    return __rdtsc();
}

uint64_t end_rdtsc()
{
    return __rdtsc();
}

// a += b for 128-bit vector
inline void ADD128(uint32_t a[4], const uint32_t b[4])
{
    uint64_t *a1 = (uint64_t *)a;
    uint64_t *b1 = (uint64_t *)b;
    uint64_t M = (((a1[0] & b1[0]) & 1) + (a1[0] >> 1) + (b1[0] >> 1)) >> 63;
    a1[1] = a1[1] + b1[1] + M;
    a1[0] = b1[0] + a1[0];
}

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char* Subkey)
{
    uint8_t i;
    uint32_t key[8],tmp1, tmp2, tmp3, tmp4;
	uint32_t* masterKey = (uint32_t*)Seedkey;
	uint32_t (*ekey)[4] = (uint32_t(*)[4]) Subkey;
	key[0] = masterKey[0];
	key[1] = masterKey[1];
	key[2] = masterKey[2];
	key[3] = masterKey[3];
	key[4] = masterKey[4];
	key[5] = masterKey[5];
	key[6] = masterKey[6];
	key[7] = masterKey[7];
    //Rotate the key to the final round state;
    for (i = 0; i < 2 * ROUNDS256_K256; i++)
    {
		ROTATE(key[7], key[6], key[5], key[4], key[3], key[2], key[1], key[0], tmp1, tmp2, tmp3, tmp4);
		ADD128(key, temp_constant[i]);
        ekey[i][0] = key[0];
        ekey[i][1] = key[1];
        ekey[i][2] = key[2];
        ekey[i][3] = key[3];
    }
	return 0;
}

//The encrypiton interface of SWAN256-K256 in the precompute key mode
int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
	const uint32_t *plain = (uint32_t*)input;
	const uint8_t rounds = ROUNDS256_K256;
	uint32_t *cipher = (uint32_t*)output;
	uint32_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	const uint32_t (*subkey)[4] = (uint32_t(*)[4])key;
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

//The decrypiton interface of SWAN256-K256 in the precompute key mode
int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
	const uint32_t *cipher = (uint32_t*)input;
	const uint8_t rounds = ROUNDS256_K256;
	uint32_t *plain = (uint32_t*)output;
	uint32_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	const uint32_t (*subkey)[4] = (uint32_t(*)[4])key;
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

//The single encryption round interface of SWAN256-K256 in the precompute key mode
int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound)
{
	const uint32_t *plain = (uint32_t*)input;
	const uint8_t rounds = CryptRound;
	uint32_t *cipher = (uint32_t*)output;
	uint32_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	const uint32_t (*subkey)[4] = (uint32_t(*)[4])key;
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
    uint32_t subkey[128][4];
    int i;
    int out_len = BLOACK256;

    unsigned char in[32] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    unsigned char out[32];
    unsigned char outt[32];
    unsigned char key[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
   

	printf("--------------------SWAN-256block-256keysize--------------------\n");
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Key_Schedule(key, 256, 1, (uint8_t*)subkey);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf(" key_schedule for keysize 256 cost %llu CPU cycles\n", (ans) / TEST);

    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\n");
    begin = start_rdtsc();
	
    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block(in,BLOACK256,out,&out_len,(uint8_t*)subkey,KEY256);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the ciphertext\n");
    dump(out, sizeof(out));
    printf("\nSWAN256K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
       Crypt_Dec_Block(out,BLOACK256,in,&out_len,(uint8_t*)subkey,KEY256);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\nSWAN256K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);
    printf("round test-swan256-256:\n");
    Crypt_Enc_Block_Round(in, 256, out, &out_len, (uint8_t*)subkey, 256, 2);
    dump(out, sizeof(out));
    printf("\n");
	
    return 0;
}