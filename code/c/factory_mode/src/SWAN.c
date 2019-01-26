#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"SWAN.h"
void RotateKeyByte(uint8_t *key, uint16_t keylength, uint8_t Rlength)
{
    uint8_t i;
    uint8_t *temp;
    temp = (uint8_t *)malloc(sizeof(uint8_t) * Rlength);

    for (i = 0; i < Rlength; i++)
    {
        temp[i] = key[i];
    }

    //Right rotate every byte of the key;
    for (i = 0; i < (keylength / 8) - 1; i++)
    {
        key[i] = key[i + Rlength];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < Rlength; i++)
    {
        key[(keylength / 8) - (Rlength - i)] = temp[i];
    }
}
void InvRotateKeyByte(uint8_t *key, uint16_t keylength, uint8_t Rlength)
{
    uint8_t i;
    uint8_t *temp;
    temp = (uint8_t *)malloc(sizeof(uint8_t) * Rlength);
    for (i = 0; i < Rlength; i++)
    {
        temp[i] = key[(keylength / 8) - (Rlength - i)];
    }
    //Right rotate every byte of the key;
    for (i = (keylength / 8) - 1; i > 0; i--)
    {
        key[i] = key[i - Rlength];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < Rlength; i++)
    {
        key[i] = temp[i];
    }
}

void ShiftLanes(uint8_t a[4], uint16_t blocksize)
{
    switch (blocksize)
    {
    case BLOCK64:
    {
        uint8_t *b;
        b = (uint8_t *)a;
        b[1] = ROL8(b[1], A_64);
        b[2] = ROL8(b[2], B_64);
        b[3] = ROL8(b[3], C_64);
        break;
    }

    case BLOCK128:
    {
        uint16_t *b;
        b = (uint16_t *)a;
        b[1] = ROL16(b[1], A_128);
        b[2] = ROL16(b[2], B_128);
        b[3] = ROL16(b[3], C_128);
        break;
    }

    case BLOCK256:
    {
        uint32_t *b;
        b = (uint32_t *)a;
        b[1] = ROL32(b[1], A_256);
        b[2] = ROL32(b[2], B_256);
        b[3] = ROL32(b[3], C_256);
        break;
    }
    }
}

void Beta(uint8_t _a[4], uint16_t blocksize)
{
    switch (blocksize)
    {
    case BLOCK64:
    {
        uint8_t *a;
        uint8_t b[4];
        a = (uint8_t *)_a;
        b[0] = ~(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
        //b[0] = 0x1 r(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
        b[1] = a[0] ^ (a[0] & a[1]) ^ a[2] ^ (a[0] & a[3]) ^ (a[1] & a[3]) ^ (a[0] & a[1] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]) ^ (a[1] & a[2] & a[3]);
        b[2] = a[1] ^ a[2] ^ (a[0] & a[2]) ^ a[3] ^ (a[0] & a[1] & a[3]) ^ (a[1] & a[2] & a[3]);
        b[3] = a[1] ^ (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[0] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]);
        memcpy(a, b, sizeof(b));
        break;
    }
    case BLOCK128:
    {
        uint16_t *a;
        uint16_t b[4];
        a = (uint16_t *)_a;
        b[0] = ~(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
        //b[0] = 0x1 r(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
        b[1] = a[0] ^ (a[0] & a[1]) ^ a[2] ^ (a[0] & a[3]) ^ (a[1] & a[3]) ^ (a[0] & a[1] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]) ^ (a[1] & a[2] & a[3]);
        b[2] = a[1] ^ a[2] ^ (a[0] & a[2]) ^ a[3] ^ (a[0] & a[1] & a[3]) ^ (a[1] & a[2] & a[3]);
        b[3] = a[1] ^ (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[0] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]);
        memcpy(a, b, sizeof(b));
        break;
    }
    case BLOCK256:
    {
        uint32_t *a;
        uint32_t b[4];
        a = (uint32_t *)_a;
        b[0] = ~(a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
        //b[0] = 0x1 ^ (a[0] ^ a[1] ^ a[3] ^ (a[2] & a[3]));
        b[1] = a[0] ^ (a[0] & a[1]) ^ a[2] ^ (a[0] & a[3]) ^ (a[1] & a[3]) ^ (a[0] & a[1] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]) ^ (a[1] & a[2] & a[3]);
        b[2] = a[1] ^ a[2] ^ (a[0] & a[2]) ^ a[3] ^ (a[0] & a[1] & a[3]) ^ (a[1] & a[2] & a[3]);
        b[3] = a[1] ^ (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[0] & a[3]) ^ (a[2] & a[3]) ^ (a[0] & a[2] & a[3]);
        memcpy(a, b, sizeof(b));
        break;
    }
    }
}

void SwitchLanes(uint8_t _a[4], uint16_t blocksize)
{
    switch (blocksize)
    {
    case BLOCK64:
    {
        uint8_t b[4];
        uint8_t *a;
        a = (uint8_t *)_a;
        b[0] = a[1] ^ a[2] ^ a[3];
        b[1] = a[0] ^ a[2] ^ a[3];
        b[2] = a[0] ^ a[1] ^ a[3];
        b[3] = a[0] ^ a[1] ^ a[2];
        memcpy(a, b, sizeof(b));
        break;
    }

    case BLOCK128:
    {
          uint16_t b[4];
          uint16_t *a;
          a = (uint16_t *)_a;
          b[0] = a[1] ^ a[2] ^ a[3];
          b[1] = a[0] ^ a[2] ^ a[3];
          b[2] = a[0] ^ a[1] ^ a[3];
          b[3] = a[0] ^ a[1] ^ a[2];
          memcpy(a, b, sizeof(b));
         break;
    }
    case BLOCK256:
    {
        uint32_t b[4];
        uint32_t *a;
        a = (uint32_t *)_a;
        b[0] = a[1] ^ a[2] ^ a[3];
        b[1] = a[0] ^ a[2] ^ a[3];
        b[2] = a[0] ^ a[1] ^ a[3];
        b[3] = a[0] ^ a[1] ^ a[2];
        memcpy(a, b, sizeof(b));
        break;
    }
    }
}
