#ifndef _WBSWAN_H_
#define _WBSWAN_H_
#include <math/affine_transform.h>
#include <math/matrix_utils.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SWAN_PIECE_BIT 8
#define DEBUG 0
#define CFG swan_cfg_B64_K128
enum swan_cipher_config_t
{
    swan_cfg_B64_K128,
    swan_cfg_B64_K256,
    swan_cfg_B128_K128,
    swan_cfg_B128_K256,
    swan_cfg_B256_K256,
};

#define MAX_RK_SIZE 128

#if SWAN_PIECE_BIT == 8
typedef uint32_t swan_wb_semi;
typedef uint8_t swan_wb_unit;


#elif SWAN_PIECE_BIT == 16
typedef uint64_t swan_wb_semi;
typedef uint16_t swan_wb_unit;

#elif SWAN_PIECE_BIT == 32
typedef uint128_t swan_wb_semi;
typedef uint32_t swan_wb_unit;

#endif

static int swan_cfg_rounds[] = {64, 128, 64, 64, 64};

static int swan_cfg_keysizes[] = {128, 256, 128, 256, 256};

static int swan_cfg_blocksizes[] = {64, 64, 128, 128, 256};

static MatGf2 make_special_rotate(int dim);



static int ROL_A[] = {1, 1, 1, 1, 1, 1};
static int ROL_B[] = {2, 2, 3, 3, 9, 9};
static int ROL_C[] = {7, 7, 13,13,19,19};

#pragma pack(1)
typedef struct swan_whitebox_helper
{
    enum swan_cipher_config_t cfg;
    int aff_in_round;
    int block_size;
    int rounds;
    int piece_count;
    int encrypt;
    CombinedAffine *P;
    CombinedAffine *B;
    CombinedAffine *C;
    CombinedAffine *D;
    CombinedAffine *PQ;
    uint8_t *key;
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
    swan_wb_semi (*lut)[4][256];
    swan_wb_unit SE[16][256];
    swan_wb_unit EE[16][256];
    CombinedAffine *P;
    CombinedAffine *B;
    CombinedAffine *C;
    CombinedAffine *D;
    CombinedAffine *PQ;
    CombinedAffine *E;
    CombinedAffine *F;
    AffineTransform q;
    int weak_or_strong; //0 for weak , 1 for strong
} swan_whitebox_content;
#pragma pack()

#define ROL16(x, n) ((x >> n) | (x << (16 - n)))

int swan_whitebox_64_init(const uint8_t *key, int enc, swan_whitebox_content *swc);

int swan_whitebox_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);

int swan_whitebox_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc);

MatGf2 make_right_rotate_shift(int dim, int r1, int r2, int r3);
MatGf2 make_transposition_back(int dim);
MatGf2 make_transposition(int dim);
MatGf2 make_switchlane(int dim);
MatGf2 make_switchlane_64(int dim);
MatGf2 make_right_rotate_shift_64(int dim, int r1, int r2, int r3);
MatGf2 make_transposition_64(int dim);
MatGf2 make_transposition_back_64(int dim);

void tobin(uint32_t x, int SIZE);

int swan_wb_export_to_bytes(const swan_whitebox_content *swc, uint8_t **dest);
int swan_wb_import_from_bytes(const uint8_t *source, swan_whitebox_content *swc);
#endif
