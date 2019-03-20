#ifndef _WBSWAN_H_
#define _WBSWAN_H_
#include <math/affine_transform.h>
#include <math/matrix_utils.h>

#define MAX_RK_SIZE 128

#define TABLE_SIZE 8 // lookup table size 8-bit in

#define DELTA_64 0x9e3779b9 // wbswan64 DELTA 

#define DELTA_128 0x9e3779b97f4a7c15 //wbswan128 DELTA

// wbswan256 DELTA (4 32-bit DELTAX combined a 128-bit DELTA)
#define DELTA1 0x9e3779b9
#define DELTA2 0x7f4a7c15
#define DELTA3 0xf39cc060
#define DELTA4 0x5cedc834

enum swan_cipher_config_t
{
    swan_cfg_B64_K128,  
    swan_cfg_B64_K256,
    swan_cfg_B128_K128,
    swan_cfg_B128_K256,
    swan_cfg_B256_K256,
};

static int swan_cfg_piecebit[] = {8, 8, 16, 16, 32};

static int swan_cfg_rounds[] = {64, 128, 96, 128, 128};

static int swan_cfg_keysizes[] = {128, 256, 128, 256, 256};

static int swan_cfg_blocksizes[] = {64, 64, 128, 128, 256};

static int swan_cfg_keyrotates[] = {3, 3, 7, 7, 15};

//shiftlanes params in swan
static int ROL_A[] = {1, 1, 1, 1, 1};
static int ROL_B[] = {2, 2, 3, 3, 9};
static int ROL_C[] = {7, 7, 13, 13, 19};

#pragma pack(1)
typedef struct swan_whitebox_helper
{
    enum swan_cipher_config_t cfg;
    int block_size;
    int rounds;
    int piece_count;
    int piece_bit;
    int encrypt;         //1 for encrypt, 0 for decrypt
    const uint8_t *key; //masterkey
    int weak_or_strong; //0 for weak , 1 for strong
} swan_whitebox_helper;
#pragma pack()


#pragma pack(1)
typedef struct swan_wb_t
{
    enum swan_cipher_config_t cfg;
    uint32_t rounds;
    uint32_t block_size;
    uint32_t piece_count; // piece_count = block_size / 8, every 8 bit combined as a piece

    uint32_t (*lut_64)[4][256]; // wbswan64 lookuptable for w
    uint64_t (*lut_128)[4][2][256]; //wbswan128 lookuptable for w
    __uint128_t (*lut_256)[4][4][256]; //wbswan256 lookuptable for w

    uint8_t SE[32][256]; // first round input encode lookuptable
    uint8_t EE[32][256]; // last round output decode lookutable

    CombinedAffine *P;
    CombinedAffine *B;
    CombinedAffine *C;
    CombinedAffine *D;
    CombinedAffine *PQ;
    CombinedAffine *E;
    CombinedAffine *F;
    int weak_or_strong; //0 for weak , 1 for strong
} swan_whitebox_content;
#pragma pack()

static unsigned char S[16] = {0x01, 0x02, 0x0C, 0x05, 0x07, 0x08, 0x0A, 0x0F, 0x04, 0x0D, 0x0B, 0x0E, 0x09, 0x06, 0x00, 0x03};

int swan_whitebox_helper_init(const uint8_t *key, swan_whitebox_helper *swh, int enc, int weak_or_strong, enum swan_cipher_config_t cfg);

int swan_whitebox_256_init(const uint8_t *key, int enc, int weak_or_strong, swan_whitebox_content *swc, enum swan_cipher_config_t cfg);

int swan_whitebox_128_init(const uint8_t *key, int enc, int weak_or_strong, swan_whitebox_content *swc, enum swan_cipher_config_t cfg);

int swan_whitebox_64_init(const uint8_t *key, int enc, int weak_or_strong, swan_whitebox_content *swc, enum swan_cipher_config_t cfg);

int swan_wb_export_to_bytes(const swan_whitebox_content *swc, uint8_t **dest);
int swan_wb_import_from_bytes(const uint8_t *source, swan_whitebox_content *swc);

int swan_whitebox_256_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);
int swan_whitebox_256_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);

int swan_whitebox_128_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);
int swan_whitebox_128_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);

int swan_whitebox_64_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);
int swan_whitebox_64_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);

void RotateKeyByte(uint8_t *key, uint16_t keylength, int L);

int swan_whitebox_release(swan_whitebox_content *swc, int table_or_content);


#endif
