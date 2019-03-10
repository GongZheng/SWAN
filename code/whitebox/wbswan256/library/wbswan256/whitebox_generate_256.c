#include <stdio.h>
#include <math/affine_transform.h>
#include <math/matrix_utils.h>
#include <wbswan/wbswan.h>
#include <time.h>

#define DELTA1 0x9e3779b9
#define DELTA2 0x7f4a7c15
#define DELTA3 0xf39cc060
#define DELTA4 0x5cedc834

// #define DELTA1 0x00
// #define DELTA2 0x00
// #define DELTA3 0x00
// #define DELTA4 0x00

const uint32_t delta[4] = {DELTA4, DELTA3, DELTA2, DELTA1};

static unsigned char S[16] = {0x01, 0x02, 0x0C, 0x05, 0x07, 0x08, 0x0A, 0x0F, 0x04, 0x0D, 0x0B, 0x0E, 0x09, 0x06, 0x00, 0x03};

void RotateKeyByte(uint8_t *key, uint16_t keylength)
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

//The inverse of right rotate key bytes for SWAN128 decryption
void InvRotateKeyByte(uint8_t *key, uint16_t keylength)
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

void ADD128(uint32_t a[4], const uint32_t b[4])
{
    uint64_t *a1 = (uint64_t *)a;
    uint64_t *b1 = (uint64_t *)b;
    uint64_t M = (((a1[0] & b1[0]) & 1) + (a1[0] >> 1) + (b1[0] >> 1)) >> 63;
    a1[1] = a1[1] + b1[1] + M;
    a1[0] = b1[0] + a1[0];
}

// int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey, int rounds)
// {
//     int i;
//     uint32_t *key;
//     uint32_t subkey[4];
//     uint32_t temp_constant[4];
//     uint32_t(*ekey)[4] = (uint32_t(*)[4])malloc(sizeof(uint32_t) * (rounds)*4);
//     key = (uint32_t *)malloc((KeyLen / 8));
//     memcpy(key, Seedkey, (KeyLen / 8) * sizeof(uint8_t));
//     memset(temp_constant, 0, sizeof(temp_constant));
//     MatGf2 mat = make_transposition_128(128);
//     MatGf2 mat_back = make_transposition_back_128(128);

//     for (i = 0; i < rounds; i++)
//     {
//         RotateKeyByte(key, KeyLen);
//         subkey[0] = key[0];
//         subkey[1] = key[1];
//         subkey[2] = key[2];
//         subkey[3] = key[3];
//         ADD128(temp_constant, delta);
//         ADD128(subkey, temp_constant);

//         *((__uint128_t *)subkey) = ApplyMatToU128(mat, *((__uint128_t *)subkey));

//         ekey[i][0] = subkey[0];
//         ekey[i][1] = subkey[1];
//         ekey[i][2] = subkey[2];
//         ekey[i][3] = subkey[3];

//         *((__uint128_t *)subkey) = ApplyMatToU128(mat_back, *((__uint128_t *)subkey));

//         key[0] = subkey[0];
//         key[1] = subkey[1];
//         key[2] = subkey[2];
//         key[3] = subkey[3];
//     }
//     memcpy(Subkey, ekey, (rounds)*4 * sizeof(uint32_t));
//     free(key);
//     free(ekey);
//     MatGf2Free(mat);
//     MatGf2Free(mat_back);
//     mat = NULL;
//     mat_back = NULL;
//     return 0;
// }

// int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey,int rounds)
// {
//     uint8_t i;
//     uint32_t *key;
//     uint32_t subkey[4];
//     uint32_t temp_constant[4];
//     uint32_t (*ekey)[4] = (uint32_t(*)[4])malloc(sizeof(uint32_t) * (rounds)*4);
//     memcpy(key, Seedkey, (KeyLen / 8) * sizeof(uint32_t));
//     memset(temp_constant, 0, sizeof(temp_constant));
//     key = (uint32_t *)malloc(sizeof(uint32_t) * (KeyLen / 8));
//     MatGf2 mat = make_transposition_128(128);
//     MatGf2 mat_back = make_transposition_back_128(128);
//     //Rotate the key to the final round state;
//     for (i = 0; i < rounds; i++)
//     {
//         RotateKeyByte((uint8_t *)key, 256);

//         subkey[0] = key[0];
//         subkey[1] = key[1];
//         subkey[2] = key[2];
//         subkey[3] = key[3];

//         ADD128(temp_constant, delta);
//         ADD128(subkey, temp_constant);

//         int j;
//         printf("ROUND %d\n", i);
//         // for (j = 0; j < 4; j++)
//         // {
//         //     printf("%016LLX\n", subkey[j]);
//         // }
//         printf("%016LLX\n", subkey[0]);
//         printf("%016LLX\n", subkey[1]);
//         printf("%016LLX\n", subkey[2]);
//         printf("%016LLX\n", subkey[3]);

//         // *((__uint128_t *)subkey) = ApplyMatToU128(mat, *((__uint128_t *)subkey));

//         ekey[i][0] = subkey[0];
//         ekey[i][1] = subkey[1];
//         ekey[i][2] = subkey[2];
//         ekey[i][3] = subkey[3];

//         // *((__uint128_t *)subkey) = ApplyMatToU128(mat_back, *((__uint128_t *)subkey));

//         // printf("%016LLX\n", subkey[0]);
//         // printf("%016LLX\n", subkey[1]);
//         // printf("%016LLX\n", subkey[2]);
//         // printf("%016LLX\n", subkey[3]);

//         key[0] = subkey[0];
//         key[1] = subkey[1];
//         key[2] = subkey[2];
//         key[3] = subkey[3];
//     }

//     free(key);
//     free(ekey);

//     return 0;
// }


int Key_Schedule(unsigned char *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey, int rounds)
{
    int i;
    uint32_t *key;
    uint32_t subkey[4];
    uint32_t temp_constant[4];
    uint32_t(*ekey)[4] = (uint32_t(*)[4])malloc(sizeof(uint32_t) * (rounds)*4);
    key = (uint32_t *)malloc(sizeof(uint32_t) * (KeyLen / 8));
    memcpy(key, Seedkey, (KeyLen / 8) * sizeof(uint32_t));
    memset(temp_constant, 0, sizeof(temp_constant));
    MatGf2 mat = make_transposition_128(128);
    MatGf2 mat_back = make_transposition_back_128(128);
    for (i = 0; i < rounds; i++)
    {
        RotateKeyByte(key, KeyLen);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        ADD128(temp_constant, delta);
        ADD128(subkey, temp_constant);


        *((__uint128_t *)subkey) = ApplyMatToU128(mat, *((__uint128_t *)subkey));

        ekey[i][0] = subkey[0];
        ekey[i][1] = subkey[1];
        ekey[i][2] = subkey[2];
        ekey[i][3] = subkey[3];

        *((__uint128_t *)subkey) = ApplyMatToU128(mat_back, *((__uint128_t *)subkey));

        // printf("ROUND %d\n", i);
        // printf("%016LLX\n", subkey[0]);
        // printf("%016LLX\n", subkey[1]);
        // printf("%016LLX\n", subkey[2]);
        // printf("%016LLX\n", subkey[3]);

        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    memcpy(Subkey, ekey, (rounds)*4 * sizeof(uint32_t));
    free(key);
    free(ekey);
    MatGf2Free(mat);
    MatGf2Free(mat_back);
    mat = NULL;
    mat_back = NULL;
    return 0;
}

int _swan_whitebox_helper_init(swan_whitebox_helper *swh, uint8_t *key, int weak_or_strong)
{

    int block_size = swh->block_size;
    int semi_block = block_size / 2;
    swh->piece_count = semi_block / SWAN_PIECE_BIT;
    swh->key = key;
    swh->weak_or_strong = weak_or_strong;

    return 0;
}

int swan_whitebox_256_strong_helper_init(const uint8_t *key, swan_whitebox_helper *swh, int enc)
// designed for swan_64_128
{

    // uint8_t rk[MAX_RK_SIZE];
    int ret;
    swh->cfg = CFG;
    swh->encrypt = enc;
    swh->rounds = swan_cfg_rounds[swh->cfg];
    swh->block_size = swan_cfg_blocksizes[swh->cfg];
    return _swan_whitebox_helper_init(swh, key, 1);
}

int swan_whitebox_256_weak_helper_init(const uint8_t *key, swan_whitebox_helper *swh, int enc)
// designed for swan_128_128
{

    // uint8_t rk[MAX_RK_SIZE];
    int ret;
    swh->cfg = CFG;
    swh->encrypt = enc;
    swh->rounds = swan_cfg_rounds[swh->cfg];
    swh->block_size = swan_cfg_blocksizes[swh->cfg];
    return _swan_whitebox_helper_init(swh, key, 0);
}

int _swan_whitebox_content_init(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    // TODO:
    swc->cfg = swh->cfg;
    swc->rounds = swh->rounds;
    swc->block_size = swh->block_size;
    swc->piece_count = swh->piece_count;

    swc->lut = (swan_wb_semi(*)[4][4][256])malloc(sizeof(swan_wb_semi) * swc->rounds * 256 * 4 * 4);
    swc->P = (CombinedAffine *)malloc((swh->rounds + 2) * sizeof(CombinedAffine));
    swc->B = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->C = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->D = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->E = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));

    swc->weak_or_strong = swh->weak_or_strong;

    int i;

    if (swc->weak_or_strong)
    {
        swc->PQ = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
        swc->F = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
        CombinedAffine *PQ_ptr = swc->PQ;
        CombinedAffine *F_ptr = swc->F;

        for (i = 0; i < (swh->rounds); i++)
        {
            combined_affine_init(PQ_ptr++, SWAN_PIECE_BIT, swh->piece_count);
            combined_affine_init(F_ptr++, SWAN_PIECE_BIT, swh->piece_count);
        }
    }
    else
    {
        swc->PQ = NULL;
        swc->F = NULL;
    }
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *P_ptr = swc->P;
    CombinedAffine *C_ptr = swc->C;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;

    for (i = 0; i < (swh->rounds); i++)
    {

        combined_affine_init(B_ptr++, SWAN_PIECE_BIT / 4, swh->piece_count * 4);
        combined_affine_init(C_ptr++, SWAN_PIECE_BIT, swh->piece_count);
        combined_affine_init(D_ptr++, SWAN_PIECE_BIT, swh->piece_count);
        combined_affine_init(E_ptr++, SWAN_PIECE_BIT, swh->piece_count);
    }
    for (i = 0; i < (swh->rounds + 2); i++)
    {
        combined_affine_init(P_ptr++, SWAN_PIECE_BIT / 4, swh->piece_count * 4);
        // combined_affine_init(P_ptr++, SWAN_PIECE_BIT, swh->piece_count);
    }

    return 0;
}

int _swan_whitebox_content_assemble(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    int piece_count = swh->piece_count;
    int j, k, r;
    uint16_t i;
    int piece_range = 1 << SWAN_PIECE_BIT;

    /*input encoding and output encoding*/
    CombinedAffine *P_ptr = swc->P;
    CombinedAffine *end_Ptr = swc->P + swh->rounds;

    // input encoding and output encoding
    int p = 0;
    for (k = 0; k < 32; k++)
    {
        if (k == 16)
        {
            P_ptr++;
            end_Ptr++;
        }
        for (p = 0; p < 256; p++)
        {

            swc->SE[k][p] = ApplyAffineToU8(P_ptr->sub_affine[k % 16], p);
            swc->EE[k][p] = ApplyAffineToU8(end_Ptr->sub_affine_inv[k % 16], p);
        }
    }

    /* theta */
    MatGf2 transposition = make_transposition_128(128);
    MatGf2 rotate = make_right_rotate_shift_128(piece_count * SWAN_PIECE_BIT, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);
    MatGf2Mul(transposition, rotate, &rotate);

    P_ptr = swc->P;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *C_ptr = swc->C;
    MatGf2 temp;

    CombinedAffine *PQ_ptr = swc->PQ;

    for (i = 0; i < swh->rounds; i++)
    {
        //B * rotate * P'* X + B * rotate * p + b

        temp = GenMatGf2Mul(B_ptr->combined_affine->linear_map, rotate);
        B_ptr->combined_affine->linear_map = GenMatGf2Mul(temp, P_ptr->combined_affine_inv->linear_map);
        B_ptr->combined_affine->vector_translation = GenMatGf2Add(GenMatGf2Mul(temp, P_ptr->combined_affine_inv->vector_translation), B_ptr->combined_affine->vector_translation);
        P_ptr++;
        B_ptr++;
    }

    B_ptr = swc->B;
    MatGf2 rotate_back = make_transposition_back_128(piece_count * SWAN_PIECE_BIT);
    rotate = make_right_rotate_shift_128(piece_count * SWAN_PIECE_BIT, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);
    swan_wb_unit key_schedule[MAX_RK_SIZE][4];
    Key_Schedule(swh->key, swan_cfg_keysizes[swh->cfg], 1, (uint8_t *)key_schedule, swh->rounds);




    srand(time(NULL));
    /* w */
    PQ_ptr = swc->PQ;
    if (swh->encrypt == 1)
    {
        for (r = 0; r < swh->rounds; r++)
        {

            uint32_t randomNum[4][4][4];

            int j = 0;
            int t = 0;
            // random
            int num = 0;
            int y;
            for (j = 0; j < 4; j++)
            {
                for (t = 0; t < 4; t++)
                {
                    for (y = 0; y < 4; y++)
                    {
                        randomNum[j][t][y] = rand();
                    }
                }
            }

            for (k = 0; k < piece_count; k++)
            {

                for (i = 0; i < 256; i++)
                {
                    int n;
                    for (n = 0; n < 4; n++)
                    {
                        uint8_t t8;
 
                            t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[4 * k + n], i) ^ ((key_schedule[r][k] >> (8 * n)) && 0x000000ff);
                        
                        uint32_t yc[4] = {0};

                        t8 = (S[(t8 >> 4) & 0x0f]) << 4 | (S[t8 & 0x0f]);
                        int t = 0;

                        if (n == 0)
                        {
                            yc[k] = ((uint32_t)t8) & 0x000000ff;
                        }

                        if (n == 1)
                        {
                            uint32_t at;
                            at = t8;
                            yc[k] = (at << 8) & 0x0000ff00;
                        }
                        if (n == 2)
                        {
                            uint32_t at;
                            at = t8;
                            yc[k] = (at << 16) & 0x00ff0000;
                        }
                        if (n == 3)
                        {
                            uint32_t at;
                            at = t8;
                            yc[k] = (at << 24) & 0xff000000;
                        }
                        rotate = make_right_rotate_shift_128(piece_count * SWAN_PIECE_BIT, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);

                        *((__uint128_t *)yc) = ApplyMatToU128(rotate_back, *((__uint128_t *)yc));

                        *((__uint128_t *)yc) = ApplyMatToU128(rotate, *((__uint128_t *)yc));

                        *((__uint128_t *)yc) = ApplyMatToU128((C_ptr->combined_affine->linear_map), *((__uint128_t *)yc));

                        for (j = 0; j < 4; j++)
                        {
                            yc[j] = yc[j] ^ randomNum[k][j][n] ^ randomNum[(k + 1) % 4][j][n];
                        }

                        if (k == 0 && n ==0)
                        {
                            *((__uint128_t *)yc) = AddMatToU128(C_ptr->combined_affine->vector_translation, *((__uint128_t *)yc));
                        }

                        swc->lut[r][k][n][i] = *((__uint128_t *)yc);
                    }
                }
            }
            B_ptr++;
            C_ptr++;
        }
    }
    else
    {
        for (r = swh->rounds - 1; r >= 0; r--)
        {

            uint32_t randomNum[4][4][4];

            int j = 0;
            int t = 0;
            // random
            int num = 0;
            int y;
            for (j = 0; j < 4; j++)
            {
                for (t = 0; t < 4; t++)
                {
                    for (y = 0; y < 4; y++)
                    {
                        randomNum[j][t][y] = rand();
                    }
                }
            }

            for (k = 0; k < piece_count; k++)
            {

                for (i = 0; i < 256; i++)
                {
                    int n;
                    for (n = 0; n < 4; n++)
                    {
                        uint8_t t8;

                        t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[4 * k + n], i) ^ ((key_schedule[r][k] >> (8 * n)) && 0x000000ff);
                       
                        uint32_t yc[4] = {0};

                        t8 = (S[(t8 >> 4) & 0x0f]) << 4 | (S[t8 & 0x0f]);
                        int t = 0;

                        if (n == 0)
                        {
                            yc[k] = ((uint32_t)t8) & 0x000000ff;
                        }

                        if (n == 1)
                        {
                            uint32_t at;
                            at = t8;
                            yc[k] = (at << 8) & 0x0000ff00;
                        }
                        if (n == 2)
                        {
                            uint32_t at;
                            at = t8;
                            yc[k] = (at << 16) & 0x00ff0000;
                        }
                        if (n == 3)
                        {
                            uint32_t at;
                            at = t8;
                            yc[k] = (at << 24) & 0xff000000;
                        }
                        rotate = make_right_rotate_shift_128(piece_count * SWAN_PIECE_BIT, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);

                        *((__uint128_t *)yc) = ApplyMatToU128(rotate_back, *((__uint128_t *)yc));
                        *((__uint128_t *)yc) = ApplyMatToU128(rotate, *((__uint128_t *)yc));

                        *((__uint128_t *)yc) = ApplyMatToU128((C_ptr->combined_affine->linear_map), *((__uint128_t *)yc));

                        for (j = 0; j < 4; j++)
                        {
                            yc[j] = yc[j] ^ randomNum[k][j][n] ^ randomNum[(k + 1) % 4][j][n];
                        }

                        if (k == 0 && n == 0)
                        {
                            *((__uint128_t *)yc) = AddMatToU128(C_ptr->combined_affine->vector_translation, *((__uint128_t *)yc));
                        }


                        swc->lut[r][k][n][i] = *((__uint128_t *)yc);
                    }
                }
            }
            B_ptr++;
            C_ptr++;
        }
    }

    // generate the P*C*rotate matrix
    MatGf2 switchmat = make_swithlane_128(128);
    CombinedAffine *D_ptr = swc->D;
    P_ptr = swc->P + 1;
    C_ptr = swc->C;

    for (i = 0; i < swc->rounds; i++)
    {
        MatGf2 temp = GenMatGf2Mul(P_ptr->combined_affine->linear_map, switchmat);
        // MatGf2 temp = P_ptr->combined_affine->linear_map;
        D_ptr->combined_affine->linear_map = GenMatGf2Mul(temp, C_ptr->combined_affine_inv->linear_map);
        D_ptr->combined_affine->vector_translation = GenMatGf2Mul(D_ptr->combined_affine->linear_map, C_ptr->combined_affine->vector_translation);
        D_ptr++;
        C_ptr++;
        P_ptr++;
    }

    PQ_ptr = swc->PQ;
    P_ptr = swc->P;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;

    for (i = 0; i < swc->rounds; i++)
    {

        E_ptr->combined_affine->linear_map = GenMatGf2Mul((P_ptr + 2)->combined_affine->linear_map, P_ptr->combined_affine_inv->linear_map);
        E_ptr->combined_affine->vector_translation = GenMatGf2Add(GenMatGf2Mul((P_ptr + 2)->combined_affine->linear_map, P_ptr->combined_affine_inv->vector_translation), (P_ptr + 2)->combined_affine->vector_translation);
        E_ptr++;

        if (swh->weak_or_strong)
        {
            F_ptr->combined_affine->linear_map = GenMatGf2Mul(PQ_ptr->combined_affine->linear_map, (P_ptr + 1)->combined_affine_inv->linear_map);
            F_ptr->combined_affine->vector_translation = GenMatGf2Add(GenMatGf2Mul(PQ_ptr->combined_affine->linear_map, (P_ptr + 1)->combined_affine_inv->vector_translation), (PQ_ptr)->combined_affine->vector_translation);
            F_ptr++;
        }
        P_ptr++;

        if (swh->weak_or_strong)
        {
            E_ptr->combined_affine->linear_map = GenMatGf2Mul((P_ptr + 2)->combined_affine->linear_map, PQ_ptr->combined_affine_inv->linear_map);
            E_ptr->combined_affine->vector_translation = GenMatGf2Add(GenMatGf2Mul((P_ptr + 2)->combined_affine->linear_map, PQ_ptr->combined_affine_inv->vector_translation), (P_ptr + 2)->combined_affine->vector_translation);
            PQ_ptr++;
        }
        else
        {
            E_ptr->combined_affine->linear_map = GenMatGf2Mul((P_ptr + 2)->combined_affine->linear_map, P_ptr->combined_affine_inv->linear_map);
            E_ptr->combined_affine->vector_translation = GenMatGf2Add(GenMatGf2Mul((P_ptr + 2)->combined_affine->linear_map, P_ptr->combined_affine_inv->vector_translation), (P_ptr + 2)->combined_affine->vector_translation);
        }

        E_ptr++;
        P_ptr++;
        i++;
    }

    // MatGf2Free(special);
    // special = NULL;
    MatGf2Free(rotate);
    rotate = NULL;
    return 0;
}

int swan_whitebox_256_init(const uint8_t *key, int enc, swan_whitebox_content *swc)
{
    swan_whitebox_helper *swh = (swan_whitebox_helper *)malloc(sizeof(swan_whitebox_helper));
    //    swan_whitebox_128_weak_helper_init(key, swh, enc);
    swan_whitebox_256_strong_helper_init(key, swh, enc);
    swan_whitebox_256_content_init(swh, swc);
    swan_whitebox_256_content_assemble(swh, swc);

    return 0;
}
int swan_whitebox_256_content_init(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    return _swan_whitebox_content_init(swh, swc);
}
int swan_whitebox_256_content_assemble(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    return _swan_whitebox_content_assemble(swh, swc);
}
