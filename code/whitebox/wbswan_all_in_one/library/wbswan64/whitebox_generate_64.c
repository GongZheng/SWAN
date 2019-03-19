#include <stdio.h>
#include <math/martrix_gen.h>
#include <wbswan/wbswan.h>
#include <time.h>

void AddRoundConstant_64(uint8_t *subkey, uint32_t sum)
{

    uint32_t *a = (uint32_t *)subkey;
    uint8_t *b = (uint8_t *)a;
    (*a) = (*a) + sum;

    subkey[0] = b[0];
    subkey[1] = b[1];
    subkey[2] = b[2];
    subkey[3] = b[3];
}
int Key_Schedule_64(const uint8_t *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey, int L, int rounds)
{
    int i;
    uint8_t *key;
    uint8_t subkey[4];
    uint32_t round_constant = 0;
    uint8_t(*ekey)[4] = (uint8_t(*)[4])malloc(sizeof(uint8_t) * (rounds)*4);
    key = (uint8_t *)malloc(sizeof(uint8_t) * (KeyLen / 8));
    memcpy(key, Seedkey, KeyLen / 8);
    MatGf2 mat = make_transposition(32);
    MatGf2 mat_back = make_transposition_back(32);

    for (i = 0; i < rounds; i++)
    {
        RotateKeyByte(key, KeyLen, L);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA_64;
        AddRoundConstant_64(subkey, round_constant);

        *((uint32_t *)subkey) = ApplyMatToU32(mat, *((uint32_t *)subkey));

        ekey[i][0] = subkey[0];
        ekey[i][1] = subkey[1];
        ekey[i][2] = subkey[2];
        ekey[i][3] = subkey[3];

        *((uint32_t *)subkey) = ApplyMatToU32(mat_back, *((uint32_t *)subkey));

        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }

    memcpy(Subkey, ekey, (rounds)*4);

    free(key);
    free(ekey);
    MatGf2Free(mat);
    MatGf2Free(mat_back);
    mat = NULL;
    mat_back = NULL;

    return 0;
}

int swan_whitebox_64_content_init(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    int i;
    swc->cfg = swh->cfg;
    swc->rounds = swh->rounds;
    swc->block_size = swh->block_size;
    swc->piece_count = swh->piece_count;
    const int E_length = swc->block_size / TABLE_SIZE;
    const int beta_count = E_length / 2;

    swc->lut_64 = (uint32_t(*)[4][256])malloc(sizeof(uint32_t) * swc->rounds * 256 * 4);
    swc->lut_128 = NULL;
    swc->lut_256 = NULL;
    swc->P = (CombinedAffine *)malloc((swh->rounds + 2) * sizeof(CombinedAffine));
    swc->B = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->C = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->D = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->E = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
    swc->weak_or_strong = swh->weak_or_strong;

    if (swc->weak_or_strong)
    {
        swc->PQ = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
        swc->F = (CombinedAffine *)malloc(swh->rounds * sizeof(CombinedAffine));
        CombinedAffine *PQ_ptr = swc->PQ;
        CombinedAffine *F_ptr = swc->F;

        for (i = 0; i < (swh->rounds); i++)
        {
            combined_affine_init(PQ_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
            combined_affine_init(F_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
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

        combined_affine_init(B_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
        combined_affine_init(C_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
        combined_affine_init(D_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
        combined_affine_init(E_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
    }

    for (i = 0; i < (swh->rounds + 2); i++)
    {
        combined_affine_init(P_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
    }

    return 0;
}

int swan_whitebox_64_content_assemble(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    int piece_count = swh->piece_count;
    int i, j, k, r;
    int piece_range = 1 << swan_cfg_piecebit[swh->cfg];
    const int E_length = swc->block_size / TABLE_SIZE;

    CombinedAffine *P_ptr = swc->P;
    CombinedAffine *end_Ptr = swc->P + swh->rounds;
    CombinedAffine *B_ptr = swc->B;
    MatGf2 temp;

    // first round input encoding and last round output decoding
    int p = 0;
    for (k = 0; k < E_length; k++)
    {
        if (k == E_length / 2)
        {
            P_ptr++;
            end_Ptr++;
        }
        for (p = 0; p < (1 << TABLE_SIZE); p++)
        {

            swc->SE[k][p] = ApplyAffineToU8(P_ptr->sub_affine[k % (E_length / 2)], p);
            swc->EE[k][p] = ApplyAffineToU8(end_Ptr->sub_affine_inv[k % (E_length / 2)], p);
        }
    }

    /*
        theta:
            1.inv previous affine(P)
            2.shiftLanes
            3.transpose for beta
            4.add new affine(B)
    */
    P_ptr = swc->P;
    CombinedAffine *PQ_ptr = swc->PQ;
    MatGf2 transposition = make_transposition(32);
    MatGf2 rotate = make_right_rotate_shift(32, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);
    MatGf2Mul(transposition, rotate, &rotate);
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
    CombinedAffine *C_ptr = swc->C;
    MatGf2 rotate_back = make_transposition_back(32);
    uint8_t key_schedule[MAX_RK_SIZE][swc->piece_count];

    //key schedule
    Key_Schedule_64(swh->key, swan_cfg_keysizes[swh->cfg], 1, (uint8_t *)key_schedule, swan_cfg_keyrotates[swh->cfg], swh->rounds);

    // reset random
    srand(time(NULL));

    /*
        w:
            1.inv previous affine(B)
            2.beta
            3.transpose back
            4.shiftLanes
            5.add random
            6.add new affine(C)
    */
    PQ_ptr = swc->PQ;
    uint32_t(*lut)[4][256] = swc->lut_64;
    if (swh->encrypt == 1)
    {
        for (r = 0; r < swh->rounds; r++)
        {

            uint8_t randomNum[4][4];

            int j = 0;
            int t = 0;

            // random
            for (j = 0; j < 4; j++)
            {
                for (t = 0; t < 4; t++)
                {
                    randomNum[j][t] = rand();
                }
            }

            for (k = 0; k < piece_count; k++)
            {
                for (i = 0; i < piece_range; i++)
                {
                    //  1.inv previous affine(B)
                    //  2.beta
                    uint8_t t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[k], i) ^ key_schedule[r][k];
                    uint8_t temp;
                    uint8_t yc[4] = {0};
                    temp = (t8 >> 4) & 0x0f;
                    t8 = (S[(t8 >> 4) & 0x0f]) << 4 | (S[t8 & 0x0f]);
                    int t = 0;

                    yc[k] = t8;

                    // 3. transpose back
                    *((uint32_t *)yc) = ApplyMatToU32(rotate_back, *((uint32_t *)yc));

                    // 4.shiftLanes
                    rotate = make_right_rotate_shift(32, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);

                    *((uint32_t *)yc) = ApplyMatToU32(rotate, *((uint32_t *)yc));

                    // 5.add random
                    // 6.add new affine(C)
                    *((uint32_t *)yc) = ApplyMatToU32((C_ptr->combined_affine->linear_map), *((uint32_t *)yc));

                    for (j = 0; j < swc->piece_count; j++)
                    {
                        yc[j] = yc[j] ^ randomNum[k][j] ^ randomNum[(k + 1) % swc->piece_count][j];
                    }

                    if (k == 0)
                    {
                        *((uint32_t *)yc) = AddMatToU32(C_ptr->combined_affine->vector_translation, *((uint32_t *)yc));
                    }

                    lut[r][k][i] = *((uint32_t *)yc);
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

            uint8_t randomNum[4][4];

            int j = 0;
            int t = 0;
            // random
            for (j = 0; j < 4; j++)
            {
                for (t = 0; t < 4; t++)
                {
                    randomNum[j][t] = 0;
                }
            }

            for (k = 0; k < piece_count; k++)
            {

                for (i = 0; i < piece_range; i++)
                {

                    uint8_t t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[k], i) ^ key_schedule[r][k];
                    uint8_t temp;
                    uint8_t yc[4] = {0};
                    temp = (t8 >> 4) & 0x0f;
                    t8 = (S[(t8 >> 4) & 0x0f]) << 4 | (S[t8 & 0x0f]);
                    int t = 0;

                    yc[k] = t8;

                    *((uint32_t *)yc) = ApplyMatToU32(rotate_back, *((uint32_t *)yc));

                    rotate = make_right_rotate_shift(piece_count * swan_cfg_piecebit[swh->cfg], ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);

                    *((uint32_t *)yc) = ApplyMatToU32(rotate, *((uint32_t *)yc));

                    *((uint32_t *)yc) = ApplyMatToU32((C_ptr->combined_affine->linear_map), *((uint32_t *)yc));

                    for (j = 0; j < 4; j++)
                    {
                        yc[j] = yc[j] + randomNum[k][j] + randomNum[(k + 1) % 4][j];
                    }

                    if (k == 0)
                    {
                        *((uint32_t *)yc) = AddMatToU32(C_ptr->combined_affine->vector_translation, *((uint32_t *)yc));
                    }

                    lut[r][k][i] = *((uint32_t *)yc);
                }
            }
            B_ptr++;
            C_ptr++;
        }
    }

    /*
        switchLanes:
            1.inv previous affine(C)
            2.switchLanes;
            3.add new affine(D)
    */
    MatGf2 switchmat = make_switchLanes(32);
    CombinedAffine *D_ptr = swc->D;
    P_ptr = swc->P + 1;
    C_ptr = swc->C;
    for (i = 0; i < swc->rounds; i++)
    {
        MatGf2 temp = GenMatGf2Mul(P_ptr->combined_affine->linear_map, switchmat);
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

    MatGf2Free(transposition);
    transposition = NULL;
    MatGf2Free(rotate);
    rotate = NULL;
    MatGf2Free(temp);
    temp = NULL;
    MatGf2Free(switchmat);
    switchmat = NULL;

    return 0;
}

/* 
params:
    enc: 1 for encrypt, 0 for decrypt
    weak_or_strong, 1 for strong , 0 for weak;

*/
int swan_whitebox_64_init(const uint8_t *key, int enc, int weak_or_strong, swan_whitebox_content *swc, enum swan_cipher_config_t cfg)
{
    swan_whitebox_helper *swh = (swan_whitebox_helper *)malloc(sizeof(swan_whitebox_helper));
    swan_whitebox_helper_init(key, swh, enc, weak_or_strong, cfg);
    swan_whitebox_64_content_init(swh, swc);
    swan_whitebox_64_content_assemble(swh, swc);
    free(swh);
    return 0;
}
