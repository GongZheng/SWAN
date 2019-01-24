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



#define SWAN128_K128_encrypt(plain, key, cipher) SWAN128_K128_encrypt_rounds((plain), (key), ROUNDS_128, (cipher))
#define SWAN128_K128_decrypt(cipher, key, plain) SWAN128_K128_decrypt_rounds((cipher), (key), ROUNDS_128, (plain))

#define SWAN128_K256_encrypt(plain, key, cipher) SWAN128_K256_encrypt_rounds((plain), (key), ROUNDS_256, (cipher))
#define SWAN128_K256_decrypt(cipher, key, plain) SWAN128_K256_decrypt_rounds((cipher), (key), ROUNDS_256, (plain))

void AddRoundConstant64(uint16_t *subkey, uint64_t sum)
{

    uint64_t *a = (uint64_t *)subkey;
    uint16_t *b = (uint16_t *)a;
    (*a) = (*a) + sum;

    subkey[0] = b[0];
    subkey[1] = b[1];
    subkey[2] = b[2];
    subkey[3] = b[3];
}

void MINUSRoundConstant64(uint16_t *subkey, uint64_t sum)
{

    uint64_t *a = (uint64_t *)subkey;
    uint16_t *b = (uint16_t *)a;
    (*a) = (*a) - sum;

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
    uint16_t subkey[4];
    uint64_t round_constant = 0;
    memcpy(key, masterkey, KEY128 / 8);

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

        ShiftLanes(tempL,BLOCK128);

        RotateKeyByte(key, KEY128, ROTATE_128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        //Modular add the subkey with the delta value;
        round_constant = round_constant + DELTA_128;
        AddRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];

        Beta(tempL,BLOCK128);

        SwitchLanes(tempL,BLOCK128);

        R[0] = R[0] ^ tempL[0];
        R[1] = R[1] ^ tempL[1];
        R[2] = R[2] ^ tempL[2];
        R[3] = R[3] ^ tempL[3];

        //Second half round encryption
        tempR[0] = R[0];
        tempR[1] = R[1];
        tempR[2] = R[2];
        tempR[3] = R[3];

        ShiftLanes(tempR,BLOCK128);

        RotateKeyByte(key, KEY128,ROTATE_128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA_128;
        AddRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        Beta(tempR,BLOCK128);

        SwitchLanes(tempR,BLOCK128);

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
    uint16_t subkey[4];
    uint64_t round_constant = 0;
    memcpy(key, masterkey, KEY128 / 8);

    //Rotate the key to the final round state;
    for (i = 1; i <= 2 * rounds; i++)
    {
        RotateKeyByte(key, KEY128,ROTATE_128);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA_128;
        AddRoundConstant64(subkey, round_constant);
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    RotateKeyByte(key, KEY128, ROTATE_128);

    round_constant = INV_DELTA_KEY128_128;

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

        ShiftLanes(tempR,BLOCK128);

        //Generate the final round decryption subkey;
        InvRotateKeyByte(key, KEY128, ROTATE_128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA_128;
        MINUSRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        Beta(tempR,BLOCK128);

        SwitchLanes(tempR, BLOCK128);

        L[0] = L[0] ^ tempR[0];
        L[1] = L[1] ^ tempR[1];
        L[2] = L[2] ^ tempR[2];
        L[3] = L[3] ^ tempR[3];

        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        //Second half round decryption
        ShiftLanes(tempL,BLOCK128);

        //inverse rotate the key for subkey;
        InvRotateKeyByte(key, KEY128, ROTATE_128);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];
        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA_128;
        MINUSRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        Beta(tempL,BLOCK128);

        SwitchLanes(tempL,BLOCK128);

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
    uint16_t subkey[4];
    uint64_t round_constant = 0;
    memcpy(key, masterkey, KEY256 / 8);

    //initialize the plaintext as the first round input;
    L[0] = plain[0];
    L[1] = plain[1];
    L[2] = plain[2];
    L[3] = plain[3];

    R[0] = plain[4];
    R[1] = plain[5];
    R[2] = plain[6];
    R[3] = plain[7];

    //whitening input
    //    __u8 * a = (__u8 *)&(subkey[0]);
    //    L[0] = L[0] ^ a[0];
    //    L[1] = L[1] ^ a[1];
    //    L[2] = L[2] ^ a[2];
    //    L[3] = L[3] ^ a[3];

    for (i = 1; i <= rounds; i++)
    {
        //first half round encryption;
        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        ShiftLanes(tempL,BLOCK128);

        RotateKeyByte(key, KEY256, ROTATE_128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        //Modular add the subkey with the delta value;
        round_constant = round_constant + DELTA_128;
        AddRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];

        Beta(tempL,BLOCK128);

        SwitchLanes(tempL,BLOCK128);

        R[0] = R[0] ^ tempL[0];
        R[1] = R[1] ^ tempL[1];
        R[2] = R[2] ^ tempL[2];
        R[3] = R[3] ^ tempL[3];

        //Second half round encryption
        tempR[0] = R[0];
        tempR[1] = R[1];
        tempR[2] = R[2];
        tempR[3] = R[3];

        ShiftLanes(tempR,BLOCK128);

        RotateKeyByte(key, KEY256, ROTATE_128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA_128;
        AddRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        Beta(tempR,BLOCK128);

        SwitchLanes(tempR,BLOCK128);

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
    uint16_t subkey[4];
    uint64_t round_constant = 0;
    memcpy(key, masterkey, KEY256 / 8);

    //Rotate the key to the final round state;
    for (i = 1; i <= 2 * rounds; i++)
    {
        RotateKeyByte(key, KEY256, ROTATE_128);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA_128;
        AddRoundConstant64(subkey, round_constant);
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    RotateKeyByte(key, KEY256, ROTATE_128);

    round_constant = INV_DELTA_KEY256_128;

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

        ShiftLanes(tempR,BLOCK128);

        //Generate the final round decryption subkey;
        InvRotateKeyByte(key, KEY256, ROTATE_128);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        tempR[0] = tempR[0] ^ subkey[0];
        tempR[1] = tempR[1] ^ subkey[1];
        tempR[2] = tempR[2] ^ subkey[2];
        tempR[3] = tempR[3] ^ subkey[3];

        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA_128;
        MINUSRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        Beta(tempR,BLOCK128);

        SwitchLanes(tempR,BLOCK128);

        L[0] = L[0] ^ tempR[0];
        L[1] = L[1] ^ tempR[1];
        L[2] = L[2] ^ tempR[2];
        L[3] = L[3] ^ tempR[3];

        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        //Second half round decryption
        ShiftLanes(tempL,BLOCK128);

        //inverse rotate the key for subkey;
        InvRotateKeyByte(key, KEY256, ROTATE_128);

        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];

        tempL[0] = tempL[0] ^ subkey[0];
        tempL[1] = tempL[1] ^ subkey[1];
        tempL[2] = tempL[2] ^ subkey[2];
        tempL[3] = tempL[3] ^ subkey[3];
        //Modular minus the subkey with the delta value;
        round_constant = round_constant - DELTA_128;
        MINUSRoundConstant64(subkey, round_constant);
        //update the round key K_i with the subkey+delta_i
        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];

        Beta(tempL,BLOCK128);

        SwitchLanes(tempL,BLOCK128);

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


