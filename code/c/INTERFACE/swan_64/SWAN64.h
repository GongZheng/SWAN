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
 *  SWAN64.h
 *
 *  Description: SWAN64 with a clear structure. The key schedule is on-the-fly computed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-02-21
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Tao Sun, Guojun Tang, Bing Sun, Siwei Sun, Zhaoji Xu, Yingjie Zhang.
 */

#ifndef SWAN64_H_INCLUDED
#define SWAN64_H_INCLUDED

#include <stdlib.h>
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
//After ROUND_128=32 (add 65 times), round_constant = round_constant + DELTA; here we use 1 rounds
#define INV_DELTA_KEY128 0x2c15e7f9
#define INV_DELTA_KEY256 0xb9f45639

#define ROL8(x, n) ((x >> n) | (x << (8 - n)))

static unsigned char S[16] = {0x01, 0x02, 0x0C, 0x5, 0x7, 0x08, 0x0A, 0x0F, 0x04, 0x0D, 0x0B, 0x0E, 0x09, 0x06, 0x00, 0x03};

#define SWAN64_K128_encrypt(plain, key, cipher) SWAN64_K128_encrypt_rounds((plain), (key), ROUNDS64_K128, (cipher))
#define SWAN64_K128_decrypt(cipher, key, plain) SWAN64_K128_decrypt_rounds((cipher), (key), ROUNDS64_K128, (plain))

#define SWAN64_K256_encrypt(plain, key, cipher) SWAN64_K256_encrypt_rounds((plain), (key), ROUNDS64_K256, (cipher))
#define SWAN64_K256_decrypt(cipher, key, plain) SWAN64_K256_decrypt_rounds((cipher), (key), ROUNDS64_K256, (plain))

int SWAN64_K128_encrypt_rounds(const uint8_t *plain, const uint8_t *masterkey, const uint8_t rounds, uint8_t *cipher);

int SWAN64_K128_decrypt_rounds(const uint8_t *cipher, const uint8_t *masterkey, const uint8_t rounds, uint8_t *plain);

int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound);

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey);
#endif // SWAN64_H_INCLUDED
