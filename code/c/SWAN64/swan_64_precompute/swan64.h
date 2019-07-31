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
 *  swan64.h
 *
 *  Description: The header file of SWAN64. The key schedule is precomputed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-07-12
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Tao Sun, Guojun Tang, Bing Sun, Siwei Sun, Zhaoji Xu, Yingjie Zhang.
 */

#ifndef SWAN64_H_INCLUDED
#define SWAN64_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



#define ROUNDS64_K128 32
#define ROUNDS64_K256 64

#define KEY128 128
#define KEY256 256

#define A 1
#define B 2
#define C 7

//For SWAN64 DELTA = 2^32 / golden ration
// #define DELTA 0x9e3779b9
#define DELTA 0x9e3779b9
//After ROUNDS64_K128=32 (add 65 times), round_constant = round_constant + DELTA; here we use 1 rounds
#define INV_DELTA_KEY128 0x2c15e7f9

//After ROUNDS64_K256=64 (add 129 times), round_constant = round_constant + DELTA; here we use 1 rounds
#define INV_DELTA_KEY256 0xb9f45639

#define ROL8(x, n) ((x >> n) | (x << (8 - n)))

const uint32_t ROUND_DELTA[129] = {0x9e3779b9,
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

int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen);

int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen);

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char* key, int keylen, int CryptRound);

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char* Subkey);
#endif // SWAN64_H_INCLUDED
