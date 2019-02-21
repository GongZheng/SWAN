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

#define SWAN128_K128_encrypt(plain, key, cipher) SWAN128_K128_encrypt_rounds((plain), (key), ROUNDS_128, (cipher))
#define SWAN128_K128_decrypt(cipher, key, plain) SWAN128_K128_decrypt_rounds((cipher), (key), ROUNDS_128, (plain))

#define SWAN128_K256_encrypt(plain, key, cipher) SWAN128_K256_encrypt_rounds((plain), (key), ROUNDS_256, (cipher))
#define SWAN128_K256_decrypt(cipher, key, plain) SWAN128_K256_decrypt_rounds((cipher), (key), ROUNDS_256, (plain))

//The bitslicing of SWAN Sboxes for the Beta function, a[0] and b[0] is the lsb bit of sbox input;
//SWAN_SBox = {0x01, 0x02, 0x0C, 0x05, 0x07, 0x08, 0x0A, 0x0F, 0x04, 0x0D, 0x0B, 0x0E, 0x09, 0x06, 0x00, 0x03}
//y[0] = 1 + x[0] + x[1] + x[3] + x[2]x[3])
//y[1] = x[0] + x[0]x1 + x[2] + x[0]x[3] + x[1]x[3] + x[0]x[1]x[3] + x[2]x[3] + x[0]x[2]x[3] + x[1]x[2]x[3]
//y[2] = x[1] + x[2] + x[0]x[2] + x[3] + x[0]x[1]x[3] + x[1]x[2]x[3]
//y[3] = x[1] + x[0]x[1] + x[0]x[2] + x[0]x[3] + x[2]x[3] + x[0]x[2]x[3]
void Beta(uint16_t a[4])
{
    uint16_t b[4];
    b[0] = ~(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
    //b[0] = 0x1 ^ (a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
    b[1] = a[0] ^ (a[0] & a[1]) ^ a[2] ^ (a[0] & a[3]) ^ (a[1] & a[3]) ^ (a[0] & a[1] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]) ^ (a[1] & a[2] & a[3]);
    b[2] = a[1] ^ a[2] ^ (a[0] & a[2]) ^ a[3] ^ (a[0] & a[1] & a[3]) ^ (a[1] & a[2] & a[3]);
    b[3] = a[1] ^ (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[0] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]);
    memcpy(a, b, sizeof(b));
}

//ShiftLanes: The first affine function before the Beta function;
void ShiftLanes(uint16_t a[4])
{
    a[1] = ROL16(a[1], A);
    a[2] = ROL16(a[2], B);
    a[3] = ROL16(a[3], C);
}

//SwitchLanes:The second affine function after the Beta function;
void SwitchLanes(uint16_t a[4])
{
      uint16_t b[4];
      b[0] = a[1] ^ a[2] ^ a[3];
      b[1] = a[0] ^ a[2] ^ a[3];
      b[2] = a[0] ^ a[1] ^ a[3];
      b[3] = a[0] ^ a[1] ^ a[2];
      memcpy(a, b, sizeof(b));
}

//d = 7 bytes
void RotateKeyByte(uint8_t *key, uint16_t keylength)
{
    uint8_t i;
    uint8_t temp[7];

    for(i=0;i<7;i++){
        temp[i] = key[i];
    }

    //Right rotate every byte of the key;
    for (i = 0; i < (keylength / 8) - 1; i++)
    {
        key[i] = key[i + 7];
    }

    //Cyclic the first byte of the key to the MSB;
   for(i=0;i<7;i++){
       key[(keylength / 8) - (7-i)] = temp[i];
   }
}

void InvRotateKeyByte(uint8_t *key, uint16_t keylength)
{
    uint8_t i;
    uint8_t temp[7];
    for(i = 0;i<7;i++){
        temp[i] = key[(keylength / 8) - (7-i)];
    }
    //Right rotate every byte of the key;
    for (i = (keylength / 8) - 1; i > 0; i--)
    {
        key[i] = key[i - 7];
    }

    //Cyclic the first byte of the key to the MSB;
   for(i=0;i<7;i++){
      key[i] = temp[i];
   }
}

void AddRoundConstant(uint16_t *subkey, uint64_t sum)
{

    uint64_t *a = (uint64_t *)subkey;
    (*a) = (*a) + sum;
    uint16_t *b = (uint16_t *)a;

    subkey[0] = b[0];
    subkey[1] = b[1];
    subkey[2] = b[2];
    subkey[3] = b[3];
}

void MINUSRoundConstant(uint16_t *subkey, uint64_t sum)
{

    uint64_t *a = (uint64_t *)subkey;
    (*a) = (*a) - sum;
    uint16_t *b = (uint16_t *)a;

    subkey[0] = b[0];
    subkey[1] = b[1];
    subkey[2] = b[2];
    subkey[3] = b[3];
}

void SWAN128_K128_encrypt_rounds(const uint16_t *plain, const uint16_t *masterkey, const uint8_t rounds, uint16_t *cipher)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint16_t key[KEY128 / 16];
    memcpy(key, masterkey, KEY128 / 8);
    uint16_t subkey[4];
    uint64_t round_constant = 0;

    //initialize the plaintext as the first round input;
    L[0] = plain[0];
    L[1] = plain[1];
    L[2] = plain[2];
    L[3] = plain[3];

    R[0] = plain[4];
    R[1] = plain[5];
    R[2] = plain[6];
    R[3] = plain[7];


    for (i = 1; i <= rounds; i++)
    {
        //first half round encryption;
        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        ShiftLanes(tempL);

        RotateKeyByte(key, KEY128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        //Modular add the subkey with the delta value;
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);
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

        RotateKeyByte(key, KEY128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);
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

void SWAN128_K128_decrypt_rounds(const uint16_t *cipher, const uint16_t *masterkey, const uint8_t rounds, uint16_t *plain)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint16_t key[KEY128 / 16];
    memcpy(key, masterkey, KEY128 / 8);
    uint16_t subkey[4];
    uint64_t round_constant = 0;

    //Rotate the key to the final round state;
    for (i = 1; i <= 2 * rounds; i++)
    {
        RotateKeyByte(key, KEY128);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    
    RotateKeyByte(key, KEY128);

    round_constant = INV_DELTA_KEY128;

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
        InvRotateKeyByte(key, KEY128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];


        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA;
        MINUSRoundConstant(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];



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
        InvRotateKeyByte(key, KEY128);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];


        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];
        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA;
        MINUSRoundConstant(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];


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

void SWAN128_K256_encrypt_rounds(const uint16_t *plain, const uint16_t *masterkey, const uint8_t rounds, uint16_t *cipher)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint16_t key[KEY256 / 16];
    memcpy(key, masterkey, KEY256 / 8);
    uint16_t subkey[4];
    uint64_t round_constant = 0;

    //initialize the plaintext as the first round input;
    L[0] = plain[0];
    L[1] = plain[1];
    L[2] = plain[2];
    L[3] = plain[3];

    R[0] = plain[4];
    R[1] = plain[5];
    R[2] = plain[6];
    R[3] = plain[7];


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
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);
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
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);
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

void SWAN128_K256_decrypt_rounds(const uint16_t *cipher, const uint16_t *masterkey, const uint8_t rounds, uint16_t *plain)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint16_t key[KEY256 / 16];
    memcpy(key, masterkey, KEY256 / 8);
    uint16_t subkey[4];
    uint64_t round_constant = 0;

    //Rotate the key to the final round state;
    for (i = 1; i <= 2 * rounds; i++)
    {
        RotateKeyByte(key, KEY256);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    
    RotateKeyByte(key, KEY256);

    round_constant = INV_DELTA_KEY256;

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
        InvRotateKeyByte(key, KEY256);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA;
        MINUSRoundConstant(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

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
        InvRotateKeyByte(key, KEY256);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];


        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];
        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA;
        MINUSRoundConstant(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

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

#endif
