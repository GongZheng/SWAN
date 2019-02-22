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
 *  SWAN128.h
 *
 *  Description: SWAN128 with a clear structure. The key schedule is on-the-fly computed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-02-21
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Bing Sun, Siwei Sun, Tao Sun, Guojun Tang, Zhaoji Xu, Yingjie Zhang.
 */
#ifndef SWAN128_H_INCLUDED
#define SWAN128_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ROUNDS128_128 48
#define ROUNDS128_256 64

#define KEY128 128
#define KEY256 256

#define A 1
#define B 3
#define C 13

//For SWAN128 DELTA = 2^64 / golden ration

#define DELTA 0x9e3779b97f4a7c15
//After ROUND_128=32 (add 65 times), round_constant = round_constant + DELTA;

#define INV_DELTA_KEY128 0xf3051f493b3903f5

#define INV_DELTA_KEY256 0xb9f4567924888695

#define ROL16(x, n) ((x >> n) | (x << (16 - n)))

#define SWAN128_K128_encrypt(plain, key, cipher) SWAN128_K128_encrypt_rounds((plain), (key), ROUNDS128_128, (cipher))
#define SWAN128_K128_decrypt(cipher, key, plain) SWAN128_K128_decrypt_rounds((cipher), (key), ROUNDS128_128, (plain))

#define SWAN128_K256_encrypt(plain, key, cipher) SWAN128_K256_encrypt_rounds((plain), (key), ROUNDS128_256, (cipher))
#define SWAN128_K256_decrypt(cipher, key, plain) SWAN128_K256_decrypt_rounds((cipher), (key), ROUNDS128_256, (plain))

int SWAN128_K128_encrypt_rounds(const uint16_t *plain, uint16_t *_key, const uint8_t rounds, uint16_t *cipher);

int SWAN128_K128_decrypt_rounds(const uint16_t *cipher, uint16_t *_key, const uint8_t rounds, uint16_t *plain);

int SWAN128_K256_encrypt_rounds(const uint16_t *plain, uint16_t *_key, const uint8_t rounds, uint16_t *cipher);

int SWAN128_K256_decrypt_rounds(const uint16_t *cipher, uint16_t *_key, const uint8_t rounds, uint16_t *plain);

int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound);

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey);

#endif
