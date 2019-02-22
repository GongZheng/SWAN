#include "swan128.h"
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
    uint8_t N = keylength / 8 - 1;
    for (i = 0; i < 7; i++)
    {
        temp[6 - i] = key[N - i];
    }

    //Right rotate every byte of the key;
    for (i = N; i >= 7; i--)
    {
        key[i] = key[i - 7];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < 7; i++)
    {
        key[i] = temp[i];
    }
}

void InvRotateKeyByte(uint8_t *key, uint16_t keylength)
{
    uint8_t i;
    uint8_t temp[7];
    uint8_t N = keylength / 8 - 1;
    for (i = 0; i < 7; i++)
    {
        temp[i] = key[i];
    }

    //Right rotate every byte of the key;
    for (i = 0; i <= N - 7; i++)
    {
        key[i] = key[i + 7];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < 7; i++)
    {
        key[(N + 1) - 7 + i] = temp[i];
    }
}

void AddRoundConstant(uint16_t *subkey, uint64_t sum)
{

    uint64_t *a = (uint64_t *)subkey;
    uint16_t *b = (uint16_t *)a;
    (*a) = (*a) + sum;

    subkey[0] = b[0];
    subkey[1] = b[1];
    subkey[2] = b[2];
    subkey[3] = b[3];
}
void MINUSRoundConstant(uint16_t *subkey, uint64_t sum)
{

    uint64_t *a = (uint64_t *)subkey;
    uint16_t *b = (uint16_t *)a;
    (*a) = (*a) - sum;

    subkey[0] = b[0];
    subkey[1] = b[1];
    subkey[2] = b[2];
    subkey[3] = b[3];
}
int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey)
{

    uint16_t *key;
    uint16_t subkey[4];
    uint64_t round_constant = 0;
    uint8_t i;
    uint16_t rounds = (KeyLen == KEY128) ? ROUNDS128_128 : ROUNDS128_256;
    uint16_t(*ekey)[4] = (uint16_t(*)[4])malloc(sizeof(uint16_t) * (2 * rounds) * 4);
    key = (uint16_t *)malloc(sizeof(uint16_t) * (KeyLen / 8));
    memcpy(key, Seedkey, (KeyLen / 8) * sizeof(uint16_t));
    for (i = 0; i < 2 * rounds; i++)
    {
        RotateKeyByte(key, KeyLen);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA;
        AddRoundConstant(subkey, round_constant);

        ekey[i][0] = subkey[0];
        ekey[i][1] = subkey[1];
        ekey[i][2] = subkey[2];
        ekey[i][3] = subkey[3];

        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    memcpy(Subkey, ekey, (2 * rounds) * 4 * sizeof(uint16_t));
    free(key);
    free(ekey);
    return 0;
}

int SWAN128_K128_encrypt_rounds(const uint16_t *plain, uint16_t *_subkey, const uint8_t rounds, uint16_t *cipher)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint16_t key[KEY128 / 16];
    uint64_t round_constant = 0;
    uint16_t (*subkey)[4] = (uint16_t(*)[4])malloc(sizeof(uint16_t) * (2 * ROUNDS128_128) * 4);
    memcpy(subkey, _subkey, sizeof(uint16_t) * (2 * ROUNDS128_128) * 4);

    //initialize the plaintext as the first round input;
    L[0] = plain[0];
    L[1] = plain[1];
    L[2] = plain[2];
    L[3] = plain[3];

    R[0] = plain[4];
    R[1] = plain[5];
    R[2] = plain[6];
    R[3] = plain[7];


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

    return 0;
}

int SWAN128_K128_decrypt_rounds(const uint16_t *cipher, uint16_t *_subkey, const uint8_t rounds, uint16_t *plain)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint64_t round_constant = 0;
    uint16_t(*subkey)[4] = (uint16_t(*)[4])malloc(sizeof(uint16_t) * (2 * ROUNDS128_128) * 4);
    memcpy(subkey, _subkey, sizeof(uint16_t) * (2 * ROUNDS128_128) * 4);

    //initialize the ciphertext as the first decryption round input;
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

        tempR[0] = tempR[0] ^ subkey[2 * rounds - 1 - (2 * i )][0];
        tempR[1] = tempR[1] ^ subkey[2 * rounds - 1 - (2 * i )][1];
        tempR[2] = tempR[2] ^ subkey[2 * rounds - 1 - (2 * i )][2];
        tempR[3] = tempR[3] ^ subkey[2 * rounds - 1 - (2 * i )][3];


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

    return 0;
}

int SWAN128_K256_encrypt_rounds(const uint16_t *plain, uint16_t *_subkey, const uint8_t rounds, uint16_t *cipher)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint64_t round_constant = 0;
    uint16_t(*subkey)[4] = (uint16_t(*)[4])malloc(sizeof(uint16_t) * (2 * ROUNDS128_256) * 4);
    memcpy(subkey, _subkey, sizeof(uint16_t) * (2 * ROUNDS128_256) * 4);

    //initialize the plaintext as the first round input;
    L[0] = plain[0];
    L[1] = plain[1];
    L[2] = plain[2];
    L[3] = plain[3];

    R[0] = plain[4];
    R[1] = plain[5];
    R[2] = plain[6];
    R[3] = plain[7];

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

    return 0;
}

int SWAN128_K256_decrypt_rounds(const uint16_t *cipher, uint16_t *_subkey, const uint8_t rounds, uint16_t *plain)
{
    uint8_t i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t tempL[4];
    uint16_t tempR[4];
    uint64_t round_constant = 0;
    uint16_t(*subkey)[4] = (uint16_t(*)[4])malloc(sizeof(uint16_t) * (2 * ROUNDS128_256) * 4);
    memcpy(subkey, _subkey, sizeof(uint16_t) * (2 * ROUNDS128_256) * 4);

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

        //Generate the final round decryption subkey;
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

        //inverse rotate the key for subkey;
     

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

    return 0;
}
int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
    switch (keylen)
    {
    case KEY128:
        SWAN128_K128_encrypt(input, key, output);
        break;
    case KEY256:
        SWAN128_K256_encrypt(input, key, output);
        break;
    default:
        SWAN128_K128_encrypt(input, key, output);
    }
}
int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen)
{
    switch (keylen)
    {
    case KEY128:
        return SWAN128_K128_decrypt(input, key, output);
        break;
    case KEY256:
        return SWAN128_K256_decrypt(input, key, output);
        break;
    default:
        return SWAN128_K128_decrypt(input, key, output);
    }
}

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound)
{
    switch (keylen)
    {
    case KEY128:
        return SWAN128_K128_encrypt_rounds(input, key, CryptRound, output);
        break;
    case KEY256:
        return SWAN128_K256_encrypt_rounds(input, key, CryptRound, output);
        break;
    default:
        return SWAN128_K128_encrypt_rounds(input, key, CryptRound, output);
    }
}