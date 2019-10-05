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
#include <SWAN.h>

const uint64_t ROUND_DELTA128[129] = {0x9e3779b97f4a7c15,
                                   0x3c6ef372fe94f82a,
                                   0xdaa66d2c7ddf743f,
                                   0x78dde6e5fd29f054,
                                   0x1715609f7c746c69,
                                   0xb54cda58fbbee87e,
                                   0x538454127b096493,
                                   0xf1bbcdcbfa53e0a8,
                                   0x8ff34785799e5cbd,
                                   0x2e2ac13ef8e8d8d2,
                                   0xcc623af8783354e7,
                                   0x6a99b4b1f77dd0fc,
                                   0x8d12e6b76c84d11,
                                   0xa708a824f612c926,
                                   0x454021de755d453b,
                                   0xe3779b97f4a7c150,
                                   0x81af155173f23d65,
                                   0x1fe68f0af33cb97a,
                                   0xbe1e08c47287358f,
                                   0x5c55827df1d1b1a4,
                                   0xfa8cfc37711c2db9,
                                   0x98c475f0f066a9ce,
                                   0x36fbefaa6fb125e3,
                                   0xd5336963eefba1f8,
                                   0x736ae31d6e461e0d,
                                   0x11a25cd6ed909a22,
                                   0xafd9d6906cdb1637,
                                   0x4e115049ec25924c,
                                   0xec48ca036b700e61,
                                   0x8a8043bceaba8a76,
                                   0x28b7bd766a05068b,
                                   0xc6ef372fe94f82a0,
                                   0x6526b0e96899feb5,
                                   0x35e2aa2e7e47aca,
                                   0xa195a45c672ef6df,
                                   0x3fcd1e15e67972f4,
                                   0xde0497cf65c3ef09,
                                   0x7c3c1188e50e6b1e,
                                   0x1a738b426458e733,
                                   0xb8ab04fbe3a36348,
                                   0x56e27eb562eddf5d,
                                   0xf519f86ee2385b72,
                                   0x935172286182d787,
                                   0x3188ebe1e0cd539c,
                                   0xcfc0659b6017cfb1,
                                   0x6df7df54df624bc6,
                                   0xc2f590e5eacc7db,
                                   0xaa66d2c7ddf743f0,
                                   0x489e4c815d41c005,
                                   0xe6d5c63adc8c3c1a,
                                   0x850d3ff45bd6b82f,
                                   0x2344b9addb213444,
                                   0xc17c33675a6bb059,
                                   0x5fb3ad20d9b62c6e,
                                   0xfdeb26da5900a883,
                                   0x9c22a093d84b2498,
                                   0x3a5a1a4d5795a0ad,
                                   0xd8919406d6e01cc2,
                                   0x76c90dc0562a98d7,
                                   0x15008779d57514ec,
                                   0xb338013354bf9101,
                                   0x516f7aecd40a0d16,
                                   0xefa6f4a65354892b,
                                   0x8dde6e5fd29f0540,
                                   0x2c15e81951e98155,
                                   0xca4d61d2d133fd6a,
                                   0x6884db8c507e797f,
                                   0x6bc5545cfc8f594,
                                   0xa4f3ceff4f1371a9,
                                   0x432b48b8ce5dedbe,
                                   0xe162c2724da869d3,
                                   0x7f9a3c2bccf2e5e8,
                                   0x1dd1b5e54c3d61fd,
                                   0xbc092f9ecb87de12,
                                   0x5a40a9584ad25a27,
                                   0xf8782311ca1cd63c,
                                   0x96af9ccb49675251,
                                   0x34e71684c8b1ce66,
                                   0xd31e903e47fc4a7b,
                                   0x715609f7c746c690,
                                   0xf8d83b1469142a5,
                                   0xadc4fd6ac5dbbeba,
                                   0x4bfc772445263acf,
                                   0xea33f0ddc470b6e4,
                                   0x886b6a9743bb32f9,
                                   0x26a2e450c305af0e,
                                   0xc4da5e0a42502b23,
                                   0x6311d7c3c19aa738,
                                   0x149517d40e5234d,
                                   0x9f80cb36c02f9f62,
                                   0x3db844f03f7a1b77,
                                   0xdbefbea9bec4978c,
                                   0x7a2738633e0f13a1,
                                   0x185eb21cbd598fb6,
                                   0xb6962bd63ca40bcb,
                                   0x54cda58fbbee87e0,
                                   0xf3051f493b3903f5,
                                   0x913c9902ba83800a,
                                   0x2f7412bc39cdfc1f,
                                   0xcdab8c75b9187834,
                                   0x6be3062f3862f449,
                                   0xa1a7fe8b7ad705e,
                                   0xa851f9a236f7ec73,
                                   0x4689735bb6426888,
                                   0xe4c0ed15358ce49d,
                                   0x82f866ceb4d760b2,
                                   0x212fe0883421dcc7,
                                   0xbf675a41b36c58dc,
                                   0x5d9ed3fb32b6d4f1,
                                   0xfbd64db4b2015106,
                                   0x9a0dc76e314bcd1b,
                                   0x38454127b0964930,
                                   0xd67cbae12fe0c545,
                                   0x74b4349aaf2b415a,
                                   0x12ebae542e75bd6f,
                                   0xb123280dadc03984,
                                   0x4f5aa1c72d0ab599,
                                   0xed921b80ac5531ae,
                                   0x8bc9953a2b9fadc3,
                                   0x2a010ef3aaea29d8,
                                   0xc83888ad2a34a5ed,
                                   0x66700266a97f2202,
                                   0x4a77c2028c99e17,
                                   0xa2def5d9a8141a2c,
                                   0x41166f93275e9641,
                                   0xdf4de94ca6a91256,
                                   0x7d85630625f38e6b,
                                   0x1bbcdcbfa53e0a80,
                                   0xb9f4567924888695};
                                   
#define ROTATE_128(k1, k0, tmp, bits) do{\
    tmp =  k0 << (64 - bits);\
    k0 = (k0 >> bits) | (k1 << (64 - bits));\
    k1 = (k1 >> bits) | tmp;\
}while(0);

#define INVROTATE_128(k1, k0, tmp, bits) do{\
    tmp =  k1 >> (64 - bits);\
    k1 = (k1 << bits) | (k0 >> (64 - bits));\
    k0 = (k0 << bits) | tmp;\
}while(0);

#define ROTATE_256(k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k0 << (64 - bits);\
    k0 = (k0 >> bits) | (k1 << (64 - bits));\
    k1 = (k1 >> bits) | (k2 << (64 - bits));\
    k2 = (k2 >> bits) | (k3 << (64 - bits));\
    k3 = (k3 >> bits) | tmp;\
}while(0);

#define INVROTATE_256(k3, k2, k1, k0, tmp, bits) do{\
    tmp =  k3 >> (64 - bits);\
    k3 = (k3 << bits) | (k2 >> (64 - bits));\
    k2 = (k2 << bits) | (k1 >> (64 - bits));\
    k1 = (k1 << bits) | (k0 >> (64 - bits));\
    k0 = (k0 << bits) | tmp;\
}while(0);

//ShiftLanes: The first affine function before the Beta function;
#define THETA(a0, a1, a2, a3) do{\
    a1 = ROL16(a1, A_128);\
    a2 = ROL16(a2, B_128);\
    a3 = ROL16(a3, C_128);\
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

int Crypt_Enc_Block128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
    switch (keylen)
    {
    case KEY128:
	{
		const uint16_t *plain = (uint16_t*)input;
		const uint8_t rounds = ROUNDS128_K128;
		uint16_t *cipher = (uint16_t*)output;
		uint64_t* masterKey = (uint64_t*)key;
		uint16_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
		uint16_t  key0, key1, key2, key3, key4, key5, key6, key7;
		uint64_t  k0, k1, tmp;
		size_t i;
		
		k0 = masterKey[0];
		k1 = masterKey[1];
		
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
			ROTATE_128(k1, k0, tmp, 56);
			k0 += ROUND_DELTA128[2*i];
			key0 = k0;
			key1 = k0>>16;
			key2 = k0>>32;
			key3 = k0>>48;
			ROTATE_128(k1, k0, tmp, 56);
			k0 += ROUND_DELTA128[2*i+1];
			key4 = k0;
			key5 = k0>>16;
			key6 = k0>>32;
			key7 = k0>>48;
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
    case KEY256:
	{
		const uint16_t *plain = (uint16_t*)input;
		const uint8_t rounds = ROUNDS128_K256;
		uint64_t* masterKey = (uint64_t*)key;
		uint16_t *cipher = (uint16_t*)output;
		uint16_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
		uint16_t  key0, key1, key2, key3, key4, key5, key6, key7;
		uint64_t  k0, k1, k2, k3, tmp;
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
			ROTATE_256(k3, k2, k1, k0, tmp, 56);				
			k0 += ROUND_DELTA128[2*i];
			key0 = k0;
			key1 = k0>>16;
			key2 = k0>>32;
			key3 = k0>>48;
			ROTATE_256(k3, k2, k1, k0, tmp, 56);
			k0 += ROUND_DELTA128[2*i+1];
			key4 = k0;
			key5 = k0>>16;
			key6 = k0>>32;
			key7 = k0>>48;
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
    default:
		return -1;
    }
	return -1;
}

int Crypt_Dec_Block128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
    switch (keylen)
    {
    case KEY128:
	{
		const uint16_t *cipher = (uint16_t*)input;
		const uint8_t rounds = ROUNDS128_K128;
		uint64_t* masterKey = (uint64_t*)key;
		uint16_t *plain = (uint16_t*)output;
		uint16_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
		uint16_t  key0, key1, key2, key3, key4, key5, key6, key7;
		uint64_t  k0, k1, tmp;
		size_t i,j;
		
		k0 = masterKey[0];
		k1 = masterKey[1];
		for(i = 0; i < rounds * 2 + 1; i++){
			ROTATE_128(k1, k0, tmp, 56);
			k0 += ROUND_DELTA128[i];
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
			k0 -= ROUND_DELTA128[j];
			INVROTATE_128(k1, k0, tmp, 56);	
			key0 = k0;
			key1 = k0>>16;
			key2 = k0>>32;
			key3 = k0>>48;
			k0 -= ROUND_DELTA128[j-1];
			INVROTATE_128(k1, k0, tmp, 56);
			key4 = k0;
			key5 = k0>>16;
			key6 = k0>>32;
			key7 = k0>>48;;			
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
    case KEY256:
	{
		const uint16_t *cipher = (uint16_t*)input;
		const uint8_t rounds = ROUNDS128_K256;
		uint64_t* masterKey = (uint64_t*)key;
		uint16_t *plain = (uint16_t*)output;	
		uint16_t L0, L1, L2 ,L3 ,R0 ,R1, R2, R3, T0, T1, T2, T3, tmp0, tmp1 ,tmp2 ,tmp3;
		uint16_t  key0, key1, key2, key3, key4, key5, key6, key7;
		uint64_t  k0, k1, k2, k3, tmp;
		size_t i,j;
		
		k0 = masterKey[0];
		k1 = masterKey[1];
		k2 = masterKey[2];
		k3 = masterKey[3];
		
		for(i = 0; i < rounds * 2 + 1; i++){
			ROTATE_256(k3, k2, k1, k0, tmp, 56);
			k0 += ROUND_DELTA128[i];
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
			k0 -= ROUND_DELTA128[j];
			INVROTATE_256(k3, k2, k1, k0, tmp, 56);
			key0 = k0;
			key1 = k0>>16;
			key2 = k0>>32;
			key3 = k0>>48;
			k0 -= ROUND_DELTA128[j-1];
			INVROTATE_256(k3, k2, k1, k0, tmp, 56);
			key4 = k0;
			key5 = k0>>16;
			key6 = k0>>32;
			key7 = k0>>48;;			
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
    default:
		return -1;
    }
    return -1;
}