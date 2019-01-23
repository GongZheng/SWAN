// #include "SWAN64.h"
// #include "SWAN128.h"
// #include "SWAN256.h"
#include<stdint.h>
#include<stdlib.h>

#define ROUNDS64_K128 32
#define ROUNDS64_K256 64

#define ROUNDS128_128 48
#define ROUNDS128_256 64

#define ROUNDS256_256 64

#define BLOCK64 64
#define BLOCK128 128
#define BLOCK256 256

#define KEY128 128
#define KEY256 256

#define A_64 1
#define B_64 3
#define C_64 5

#define A_128 1
#define B_128 7
#define C_128 13

#define A_256 1
#define B_256 9
#define C_256 19

#define ROTATE_64 3
#define ROTATE_128 7
#define ROTATE_256 15

#define DELTA_64 0x9e3779b9
#define INV_DELTA_KEY128_64 0x2c15e7f9
#define INV_DELTA_KEY256_64 0xb9f45639

#define DELTA_128 0x9e3779b97f4a7c15
#define INV_DELTA_KEY128_128 0xf3051f493b3903f5
#define INV_DELTA_KEY256_128 0xb9f4567924888695

#define ROL8(x, n) ((x >> n) | (x << (8 - n)))
#define ROL16(x, n) ((x >> n) | (x << (16 - n)))
#define ROL32(x, n) ((x >> n) | (x << (32 - n)))

void InvRotateKeyByte(uint8_t *key, uint16_t keylength, uint8_t Rlength) ;
void ShiftLanes(uint8_t a[4], uint16_t blocksize);
void Beta(uint8_t _a[4], uint16_t blocksize) ;
void SWAN64_K128_encrypt_rounds(const uint8_t *plain, const uint8_t *masterkey, const uint8_t rounds, uint8_t *cipher);

void SWAN64_K128_decrypt_rounds(const uint8_t *cipher, const uint8_t *masterkey, const uint8_t rounds, uint8_t *plain);

void SWAN64_K256_encrypt_rounds(const uint8_t *plain, const uint8_t *masterkey, const uint8_t rounds, uint8_t *cipher);

void SWAN64_K256_decrypt_rounds(const uint8_t *cipher, const uint8_t *masterkey, const uint8_t rounds, uint8_t *plain);

void SWAN128_K128_encrypt_rounds(const uint16_t *plain, const uint16_t *masterkey, const uint8_t rounds, uint16_t *cipher);

void SWAN128_K128_decrypt_rounds(const uint16_t *cipher, const uint16_t *masterkey, const uint8_t rounds, uint16_t *plain);

void SWAN128_K256_encrypt_rounds(const uint16_t *plain, const uint16_t *masterkey, const uint8_t rounds, uint16_t *cipher);

void SWAN128_K256_decrypt_rounds(const uint16_t *cipher, const uint16_t *masterkey, const uint8_t rounds, uint16_t *plain);

