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
