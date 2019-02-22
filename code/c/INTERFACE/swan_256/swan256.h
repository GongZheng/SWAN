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
 *  SWAN256.h
 *
 *  Description: SWAN256 with a clear structure. The key schedule is on-the-fly computed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-02-21
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Bing Sun, Siwei Sun, Tao Sun, Guojun Tang, Zhaoji Xu, Yingjie Zhang.
 */
#ifndef SWAN256_H_INCLUDED
#define SWAN256_H_INCLUDED

#include <stdint.h>

#define ROUNDS256_256 64

#define KEY256 256
#define BLOACK256 256

#define A 1
#define B 9
#define C 19

/*
    Phi = 1.61803398874989484820458683436563811772
    DELTA = int(2**128 / Phi)
*/
#define DELTA 0x9e3779b97f4a7c15f39cc0605cedc834
/*
    DELTA_VER = DELTA
    for i in range(0,ROUNDS256):
        DELTA_VER = (DELTA_VER + DELTA) % 2**128
*/

#define DELTA_VER 0x2c15e81951e98192daccd877985fd534

#define DELTA1 0x9e3779b9
#define DELTA2 0x7f4a7c15
#define DELTA3 0xf39cc060
#define DELTA4 0x5cedc834

#define DELTA_VER1 0xb9f45679
#define DELTA_VER2 0x2488870f
#define DELTA_VER3 0xc1fcf08e
#define DELTA_VER4 0xd3d1e234

#define ROL32(x, n) ((x >> n) | (x << (32 - n)))

#define SWAN256_encrypt(plain, key, cipher) SWAN256_encrypt_rounds((plain), (key), ROUNDS256_256, (cipher))
#define SWAN256_decrypt(cipher, key, plain) SWAN256_decrypt_rounds((cipher), (key), ROUNDS256_256, (plain))

void SWAN256_encrypt_rounds(const uint32_t *plain, const uint32_t *masterkey, const uint8_t rounds, uint32_t *cipher);
void SWAN256_decrypt_rounds(const uint32_t *cipher, uint32_t *masterkey, const uint8_t rounds, uint32_t *plain);

int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound);

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey);

#endif // SWAN256_H_INCLUDED
