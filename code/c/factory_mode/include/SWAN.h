#ifndef  SWAN_H
#define  SWAN_H

#include<stdint.h>
#include<stdlib.h> 

#define ROUNDS64_K128 32
#define ROUNDS64_K256 64

#define ROUNDS128_K128 48
#define ROUNDS128_K256 64

#define ROUNDS256_K256 64

#define KEY128 128
#define KEY256 256

#define A_64 1
#define B_64 2
#define C_64 7

#define A_128 1
#define B_128 3
#define C_128 13

#define A_256 1
#define B_256 9
#define C_256 19

#define ROL8(x, n) ((x >> n) | (x << (8 - n)))
#define ROL16(x, n) ((x >> n) | (x << (16 - n)))
#define ROL32(x, n) ((x >> n) | (x << (32 - n)))

int Crypt_Enc_Block64_k128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Dec_Block64_k128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Enc_Block64_k256(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Dec_Block64_k256(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Enc_Block128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Dec_Block128(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Enc_Block256(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);
int Crypt_Dec_Block256(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

#endif 