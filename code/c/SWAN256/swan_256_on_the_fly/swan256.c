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
 *  Description: The c file of SWAN256. The key schedule is on-the-fly computed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-08-29
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Bing Sun, Siwei Sun, Tao Sun, Guojun Tang, Zhaoji Xu, Yingjie Zhang.
 */

#include "swan256.h"
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

#define ROTATE(k7, k6, k5, k4, k3, k2, k1, k0, tmp4, tmp5, tmp6, tmp7) do{\
    tmp4 = (k0 << 8) | (k7 >> 24);\
	tmp5 = (k1 << 8) | (k0 >> 24);\
	tmp6 = (k2 << 8) | (k1 >> 24);\
	tmp7 = (k3 << 8) | (k2 >> 24);\
	k0 = (k4 << 8) | (k3 >> 24);\
    k1 = (k5 << 8) | (k4 >> 24);\
	k2 = (k6 << 8) | (k5 >> 24);\
	k3 = (k7 << 8) | (k6 >> 24);\
	k4 = tmp4;\
    k5 = tmp5;\
    k6 = tmp6;\
	k7 = tmp7;\
}while(0);

#define INVROTATE(k7, k6, k5, k4, k3, k2, k1, k0, tmp4, tmp5, tmp6, tmp7) do{\
    tmp4 = (k7 >> 8) | (k0 << 24);\
	tmp5 = (k6 >> 8) | (k7 << 24);\
	tmp6 = (k5 >> 8) | (k6 << 24);\
	tmp7 = (k4 >> 8) | (k5 << 24);\
	k7 = (k3 >> 8) | (k4 << 24);\
    k6 = (k2 >> 8) | (k3 << 24);\
	k5 = (k1 >> 8) | (k2 << 24);\
	k4 = (k0 >> 8) | (k1 << 24);\
	k3 = tmp4;\
    k2 = tmp5;\
    k1 = tmp6;\
	k0 = tmp7;\
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

// a += b for 128-bit vector
inline void ADD128(uint32_t a[4], const uint32_t b[4])
{
    uint64_t *a1 = (uint64_t *)a;
    uint64_t *b1 = (uint64_t *)b;
    uint64_t M = (((a1[0] & b1[0]) & 1) + (a1[0] >> 1) + (b1[0] >> 1)) >> 63;
    a1[1] = a1[1]+b1[1]+M;
    a1[0] = b1[0] + a1[0];


}

// a -=b for 128-bit vector 
inline void MINUS128(uint32_t a[4], const uint32_t b[4])
{
    uint64_t *a1 = (uint64_t *)a;
    uint64_t *b1 = (uint64_t *)b;
	uint64_t M;
    a1[0] = a1[0] - b1[0];
    M = (((a1[0] & b1[0]) & 1) + (b1[0] >> 1) + (a1[0] >> 1)) >> 63;
    a1[1] = a1[1] - (b1[1] + M);

}

//The encrypiton interface of SWAN256-K256 in the on-the-fly key mode
int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
	const uint32_t *plain = (uint32_t*)input;
	const uint8_t rounds = ROUNDS256_K256;
	uint32_t* masterKey = (uint32_t*)key;
	uint32_t *cipher = (uint32_t*)output;
	uint32_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint32_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  skey[8], tmp4, tmp5, tmp6, tmp7;
	size_t i;
	skey[0] = masterKey[0];
	skey[1] = masterKey[1];
	skey[2] = masterKey[2];
	skey[3] = masterKey[3];
	skey[4] = masterKey[4];
	skey[5] = masterKey[5];
	skey[6] = masterKey[6];
	skey[7] = masterKey[7];
		
    //initialize the plaintext as the first round input;
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
		ROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		ADD128(skey, temp_constant[2*i]);
		key0 = skey[0];
		key1 = skey[1];
		key2 = skey[2];
		key3 = skey[3];
		ROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		ADD128(skey, temp_constant[2*i+1]);
		key4 = skey[0];
		key5 = skey[1];
		key6 = skey[2];
		key7 = skey[3];
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 key0, key1, key2, key3, key4, key5, key6, key7);			 
	}
    //output the ciphertext;
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

//The decrypiton interface of SWAN256-K256 in the on-the-fly key mode
int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
	const uint32_t *cipher = (uint32_t*)input;
	const uint8_t rounds = ROUNDS256_K256;
	uint32_t* masterKey = (uint32_t*)key;
	uint32_t *plain = (uint32_t*)output;
	uint32_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint32_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  skey[8], tmp4, tmp5, tmp6, tmp7;
	size_t i,j;
	skey[0] = masterKey[0];
	skey[1] = masterKey[1];
	skey[2] = masterKey[2];
	skey[3] = masterKey[3];
	skey[4] = masterKey[4];
	skey[5] = masterKey[5];
	skey[6] = masterKey[6];
	skey[7] = masterKey[7];
	
	for(i = 0; i < rounds * 2 + 1; i++){
		ROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		ADD128(skey, temp_constant[i]);
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
		MINUS128(skey, temp_constant[j]);
		INVROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		key0 = skey[0];
		key1 = skey[1];
		key2 = skey[2];
		key3 = skey[3];
		MINUS128(skey, temp_constant[j-1]);
		INVROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		key4 = skey[0];
		key5 = skey[1];
		key6 = skey[2];
		key7 = skey[3];			
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

//The single encryption round interface of SWAN256-K256 in the on-the-fly key mode
int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound)
{
	const uint32_t *plain = (uint32_t*)input;
	const uint8_t rounds = CryptRound;
	uint32_t* masterKey = (uint32_t*)key;
	uint32_t *cipher = (uint32_t*)output;
	uint32_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
	uint32_t  key0, key1, key2, key3, key4, key5, key6, key7;
	uint32_t  skey[8], tmp4, tmp5, tmp6, tmp7;
	size_t i;
	skey[0] = masterKey[0];
	skey[1] = masterKey[1];
	skey[2] = masterKey[2];
	skey[3] = masterKey[3];
	skey[4] = masterKey[4];
	skey[5] = masterKey[5];
	skey[6] = masterKey[6];
	skey[7] = masterKey[7];
		
    //initialize the plaintext as the first round input;
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
		ROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		ADD128(skey, temp_constant[2*i]);
		key0 = skey[0];
		key1 = skey[1];
		key2 = skey[2];
		key3 = skey[3];
		ROTATE(skey[7], skey[6], skey[5], skey[4], skey[3], skey[2], skey[1], skey[0], tmp4, tmp5, tmp6, tmp7);
		ADD128(skey, temp_constant[2*i+1]);
		key4 = skey[0];
		key5 = skey[1];
		key6 = skey[2];
		key7 = skey[3];
		SWAN_ENC_ROUND(L0, L1, L2, L3, R0, R1, R2, R3, T0, T1, T2, T3,\
		 key0, key1, key2, key3, key4, key5, key6, key7);			 
	}
    //output the ciphertext;
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
    int out_len = BLOACK256;

    unsigned char in[32] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    unsigned char out[32];
    unsigned char outt[32];
    unsigned char key[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    printf("--------------------SWAN-256block-256keysize--------------------\n");

    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\n");
    begin = start_rdtsc();
	
    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block(in,BLOACK256,out,&out_len,key,KEY256);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the ciphertext\n");
    dump(out, sizeof(out));
    printf("\nSWAN256K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
  
	begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
       Crypt_Dec_Block(out,BLOACK256,in,&out_len,(uint8_t*)key,KEY256);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\nSWAN256K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

	printf("round test-swan256-256:\n");
    Crypt_Enc_Block_Round(in, 256, out, &out_len, (uint8_t*)key, 256, 1);
    dump(out, sizeof(out));
    printf("\n");
    return 0;
}