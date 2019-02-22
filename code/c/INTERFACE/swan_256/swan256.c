#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "swan256.h"

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
    uint8_t N = keylength / 8 - 1;
    for (i = 0; i < 15; i++)
    {
        temp[14 - i] = key[N - i];
    }

    //Right rotate every byte of the key;
    for (i = N; i >= 15; i--)
    {
        key[i] = key[i - 15];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < 15; i++)
    {
        key[i] = temp[i];
    }
}

void InvRotateKeyByte(uint8_t *key, uint16_t keylength)
{
    uint8_t i;
    uint8_t temp[15];
    uint8_t N = keylength / 8 - 1;
    for (i = 0; i < 15; i++)
    {
        temp[i] = key[i];
    }

    //Right rotate every byte of the key;
    for (i = 0; i <= N - 15; i++)
    {
        key[i] = key[i + 15];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < 15; i++)
    {
        key[(N + 1) - 15 + i] = temp[i];
    }
}

int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey)
{
    uint8_t i;
    uint32_t *key;
    uint32_t subkey[4];
    uint32_t temp_constant[4];
    key = (uint32_t *)malloc(sizeof(uint32_t) * (KeyLen / 8));
    uint32_t(*ekey)[4] = (uint32_t(*)[4])malloc(sizeof(uint32_t) * (2 * ROUNDS256_256) * 4);
    memcpy(key, Seedkey, (KeyLen / 8) * sizeof(uint32_t));
    memset(temp_constant, 0, sizeof(temp_constant));
    //Rotate the key to the final round state;
    for (i = 0; i < 2 * ROUNDS256_256; i++)
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
    memcpy(Subkey, ekey, (sizeof(uint32_t) * (2 * ROUNDS256_256) * 4));
    free(key);
    free(ekey);
}

void SWAN256_encrypt_rounds(const uint32_t *plain, const uint32_t *_subkey, const uint8_t rounds, uint32_t *cipher)
{
    uint8_t i;
    uint32_t L[4];
    uint32_t R[4];
    uint32_t tempL[4];
    uint32_t tempR[4];
    uint32_t round_constant[4];
    uint32_t(*subkey)[4] = (uint32_t(*)[4])malloc(sizeof(uint32_t) * (2 * ROUNDS256_256) * 4);
    memcpy(subkey, _subkey, sizeof(uint32_t) * (2 * ROUNDS256_256) * 4);
    
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

    //init round_constant

    for (i = 0; i < rounds; i++)
    {
        //first half round encryption;
        tempL[0] = L[0];
        tempL[1] = L[1];
        tempL[2] = L[2];
        tempL[3] = L[3];

        ShiftLanes(tempL);

        tempL[0] = tempL[0] ^ subkey[2 * i][0];
        tempL[1] = tempL[1] ^ subkey[2 * i][1];
        tempL[2] = tempL[2] ^ subkey[2 * i][2];
        tempL[3] = tempL[3] ^ subkey[2 * i][3];

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

        tempR[0] = tempR[0] ^ subkey[2 * i + 1][0];
        tempR[1] = tempR[1] ^ subkey[2 * i + 1][1];
        tempR[2] = tempR[2] ^ subkey[2 * i + 1][2];
        tempR[3] = tempR[3] ^ subkey[2 * i + 1][3];

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

void SWAN256_decrypt_rounds(const uint32_t *cipher, uint32_t *_subkey, const uint8_t rounds, uint32_t *plain)
{
    uint8_t i;
    uint32_t L[4];
    uint32_t R[4];
    uint32_t tempL[4];
    uint32_t tempR[4];
    uint32_t(*subkey)[4] = (uint32_t(*)[4])malloc(sizeof(uint32_t) * (2 * ROUNDS256_256) * 4);
    memcpy(subkey,_subkey,sizeof(uint32_t) * (2 * ROUNDS256_256) * 4);

    L[0] = cipher[0];
    L[1] = cipher[1];
    L[2] = cipher[2];
    L[3] = cipher[3];

    R[0] = cipher[4];
    R[1] = cipher[5];
    R[2] = cipher[6];
    R[3] = cipher[7];

    for (i = 0; i < rounds; i++)
    {
        //First half round decryption;
        tempR[0] = R[0];
        tempR[1] = R[1];
        tempR[2] = R[2];
        tempR[3] = R[3];

        ShiftLanes(tempR);

         tempR[0] = tempR[0] ^ subkey[2 * rounds - 1 - (2 * i)][0];
         tempR[1] = tempR[1] ^ subkey[2 * rounds - 1 - (2 * i)][1];
         tempR[2] = tempR[2] ^ subkey[2 * rounds - 1 - (2 * i)][2];
         tempR[3] = tempR[3] ^ subkey[2 * rounds - 1 - (2 * i)][3];

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

         tempL[0] = tempL[0] ^ subkey[2 * rounds - 1 - (2 * i + 1)][0];
         tempL[1] = tempL[1] ^ subkey[2 * rounds - 1 - (2 * i + 1)][1];
         tempL[2] = tempL[2] ^ subkey[2 * rounds - 1 - (2 * i + 1)][2];
         tempL[3] = tempL[3] ^ subkey[2 * rounds - 1 - (2 * i + 1)][3];

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
int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen){
    SWAN256_encrypt(input,key,output);
    return 0;
}

int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen){
    SWAN256_decrypt(input,key,output);
    return 0;
}

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound){
    SWAN256_encrypt_rounds(input, key, CryptRound,output);
    return 0;
}