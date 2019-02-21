/*
 * Copyright (c) 2018, 2019
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
 *  Description: SWAN256 with a clear structure. The key schedule is precomputed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-01-13
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Bing Sun, Siwei Sun, Tao Sun, Guojun Tang, Zhaoji Xu, Yingjie Zhang.
*/
#ifndef SWAN256_H_INCLUDED
#define SWAN256_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ROUNDS256_256 64

#define KEY256 256

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

#define SWAN256_encrypt(plain, key, cipher) SWAN256_encrypt_rounds((plain), (key), ROUNDS_256, (cipher))
#define SWAN256_decrypt(cipher, key, plain) SWAN256_decrypt_rounds((cipher), (key), ROUNDS_256, (plain))

const uint32_t delta[4] = {DELTA4, DELTA3, DELTA2, DELTA1};
const uint32_t delta_ver[4] = {DELTA_VER4, DELTA_VER3, DELTA_VER2, DELTA_VER1};

void ADD128(uint32_t a[4], const uint32_t b[4])
{
    uint64_t *a1 = (uint64_t *)a;
    uint64_t *b1 = (uint64_t *)b;
    uint64_t M = (((a1[0] & b1[0]) & 1) + (a1[0] >> 1) + (b1[0] >> 1)) >> 63;
    a1[1] = a1[1] + b1[1] + M;
    a1[0] = b1[0] + a1[0];
}
// a -=b
void MINUS128(uint32_t a[4], const uint32_t b[4])
{
    uint64_t *a1 = (uint64_t *)a;
    uint64_t *b1 = (uint64_t *)b;
    a1[0] = a1[0] - b1[0];

    uint64_t M = (((a1[0] & b1[0]) & 1) + (b1[0] >> 1) + (a1[0] >> 1)) >> 63;

    a1[1] = a1[1] - (b1[1] + M);
}


void Beta(uint32_t a[4])
{
    uint32_t b[4];
    b[0] = ~(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
    //b[0] = 0x1 ^ (a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
    b[1] = a[0] ^ (a[0] & a[1]) ^ a[2] ^ (a[0] & a[3]) ^ (a[1] & a[3]) ^ (a[0] & a[1] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]) ^ (a[1] & a[2] & a[3]);
    b[2] = a[1] ^ a[2] ^ (a[0] & a[2]) ^ a[3] ^ (a[0] & a[1] & a[3]) ^ (a[1] & a[2] & a[3]);
    b[3] = a[1] ^ (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[0] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]);
    memcpy(a, b, sizeof(b));
}

//ShiftLanes: The first affine function before the Beta function;
void ShiftLanes(uint32_t a[4])
{
    a[1] = ROL32(a[1], A);
    a[2] = ROL32(a[2], B);
    a[3] = ROL32(a[3], C);
}

//SwitchLanes:The second affine function after the Beta function;
void SwitchLanes(uint32_t a[4])
{
    uint32_t b[4];
    b[0] = a[1] ^ a[2] ^ a[3];
    b[1] = a[0] ^ a[2] ^ a[3];
    b[2] = a[0] ^ a[1] ^ a[3];
    b[3] = a[0] ^ a[1] ^ a[2];
    memcpy(a, b, sizeof(b));
}

//d = 15 bytes
void RotateKeyByte(uint8_t *key, uint16_t keylength)
{
    uint8_t i;
    uint8_t temp[15];

    for (i = 0; i < 15; i++)
    {
        temp[i] = key[i];
    }

    //Right rotate every byte of the key;
    for (i = 0; i < (keylength / 8) - 1; i++)
    {
        key[i] = key[i + 15];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < 15; i++)
    {
        key[(keylength / 8) - (15 - i)] = temp[i];
    }
}

void InvRotateKeyByte(uint8_t *key, uint16_t keylength)
{
    uint8_t i;
    uint8_t temp[15];
    for (i = 0; i < 15; i++)
    {
        temp[i] = key[(keylength / 8) - (15 - i)];
    }
    //Right rotate every byte of the key;
    for (i = (keylength / 8) - 1; i > 0; i--)
    {
        key[i] = key[i - 15];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < 15; i++)
    {
        key[i] = temp[i];
    }
}


void SWAN256_encrypt_rounds(const uint32_t *plain, const uint32_t *_key, const uint8_t rounds, uint32_t *cipher)
{
    uint8_t i;
    uint32_t L[4];
    uint32_t R[4];
    uint32_t tempL[4];
    uint32_t tempR[4];
    uint32_t key[KEY256 / 32];
    memcpy(key, _key, KEY256 / 8);
    uint32_t subkey[4];
    uint32_t round_constant[4];
    //uint64_t round_constant = 0;

    //initialize the plaintext as the first round input;
    L[0] = plain[0];
    L[1] = plain[1];
    L[2] = plain[2];
    L[3] = plain[3];

    R[0] = plain[4];
    R[1] = plain[5];
    R[2] = plain[6];
    R[3] = plain[7];


    //init round_constant
    memset(round_constant, 0, sizeof(round_constant));

    for (i = 1; i <= rounds; i++)
    {
        //first half round encryption;
        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        ShiftLanes(tempL);

        RotateKeyByte(key, KEY256);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        //Modular add the subkey with the delta value;
        //round_constant = round_constant + DELTA;
        ADD128(round_constant, delta);
        //AddRoundConstant(subkey, round_constant);
        ADD128(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];

        Beta(tempL);

        ShiftLanes(tempL);

        SwitchLanes(tempL);

        R[0] = R[0] ^ tempL[0];
        R[1] = R[1] ^ tempL[1];
        R[2] = R[2] ^ tempL[2];
        R[3] = R[3] ^ tempL[3];

        //Second half round encryption
        tempR[0] = R[0];
        tempR[1] = R[1];
        tempR[2] = R[2];
        tempR[3] = R[3];

        ShiftLanes(tempR);

        RotateKeyByte(key, KEY256);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        ADD128(round_constant, delta);
        //AddRoundConstant(subkey, round_constant);
        ADD128(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        Beta(tempR);

        ShiftLanes(tempR);

        SwitchLanes(tempR);

        L[0] = L[0] ^ tempR[0];
        L[1] = L[1] ^ tempR[1];
        L[2] = L[2] ^ tempR[2];
        L[3] = L[3] ^ tempR[3];
    }

    //output the ciphertext;
    cipher[0] = L[0];
    cipher[1] = L[1];
    cipher[2] = L[2];
    cipher[3] = L[3];

    cipher[4] = R[0];
    cipher[5] = R[1];
    cipher[6] = R[2];
    cipher[7] = R[3];
}

void GenerateKey(uint32_t *_key, uint8_t rounds, uint32_t *keyarray, uint32_t keysize)
{
    uint8_t i;

    uint32_t key[keysize / 32];
    memcpy(key, _key, keysize / 8);
    uint32_t subkey[4];
    uint32_t ekey[2 * rounds + 1][4];
    uint32_t temp_constant[4];

    memset(temp_constant, 0, sizeof(temp_constant));
    //Rotate the key to the final round state;
    for (i = 1; i <= 2 * rounds; i++)
    {
        RotateKeyByte(key, KEY256);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        ADD128(temp_constant, delta);
        ADD128(subkey, temp_constant);
        ekey[i][0] = subkey[0];
        ekey[i][1] = subkey[1];
        ekey[i][2] = subkey[2];
        ekey[i][3] = subkey[3];

        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    memcpy(keyarray, ekey, (2 * rounds + 1) * 4 * 4);
}

void SWAN256_decrypt_rounds(const uint32_t *cipher, uint32_t *_key, const uint8_t rounds, uint32_t *plain)
{
    uint8_t i;
    uint32_t L[4];
    uint32_t R[4];
    uint32_t tempL[4];
    uint32_t tempR[4];
    uint32_t subkey[4];
    // uint32_t temp_constant[4];
    uint32_t ekey[2 * rounds + 1][4];
    memcpy(ekey, _key, (2 * rounds + 1) * 4 * 4);


    //initialize the ciphertext as the first decryption round input;
    L[0] = cipher[0];
    L[1] = cipher[1];
    L[2] = cipher[2];
    L[3] = cipher[3];

    R[0] = cipher[4];
    R[1] = cipher[5];
    R[2] = cipher[6];
    R[3] = cipher[7];

    for (i = 1; i <= rounds; i++)
    {
        //First half round decryption;
        tempR[0] = R[0];
        tempR[1] = R[1];
        tempR[2] = R[2];
        tempR[3] = R[3];

        ShiftLanes(tempR);

        //Generate the final round decryption subkey;
        subkey[0] = ekey[2 * rounds + 1 - (2 * i - 1)][0];
        subkey[1] = ekey[2 * rounds + 1 - (2 * i - 1)][1];
        subkey[2] = ekey[2 * rounds + 1 - (2 * i - 1)][2];
        subkey[3] = ekey[2 * rounds + 1 - (2 * i - 1)][3];



        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];


        Beta(tempR);

        ShiftLanes(tempR);

        SwitchLanes(tempR);

        L[0] = L[0] ^ tempR[0];
        L[1] = L[1] ^ tempR[1];
        L[2] = L[2] ^ tempR[2];
        L[3] = L[3] ^ tempR[3];

        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        //Second half round decryption
        ShiftLanes(tempL);

        //inverse rotate the key for subkey;
        subkey[0] = ekey[2 * rounds + 1 - (2 * i)][0];
        subkey[1] = ekey[2 * rounds + 1 - (2 * i)][1];
        subkey[2] = ekey[2 * rounds + 1 - (2 * i)][2];
        subkey[3] = ekey[2 * rounds + 1 - (2 * i)][3];


        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];


        Beta(tempL);

        ShiftLanes(tempL);

        SwitchLanes(tempL);

        R[0] = tempL[0] ^ R[0];
        R[1] = tempL[1] ^ R[1];
        R[2] = tempL[2] ^ R[2];
        R[3] = tempL[3] ^ R[3];
    }

    //output the plaintext;
    plain[0] = L[0];
    plain[1] = L[1];
    plain[2] = L[2];
    plain[3] = L[3];

    plain[4] = R[0];
    plain[5] = R[1];
    plain[6] = R[2];
    plain[7] = R[3];
}

#endif // SWAN256_H_INCLUDED
