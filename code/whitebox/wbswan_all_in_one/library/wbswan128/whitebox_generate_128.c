#include <stdio.h>
#include <math/martrix_gen.h>
#include <wbswan/wbswan.h>
#include <time.h>

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

int Key_Schedule_128(const uint8_t *Seedkey, int KeyLen, unsigned char Direction, unsigned char *Subkey, int L, int rounds)
{
    int i;
    uint16_t *key;
    uint16_t subkey[4];
    uint64_t round_constant = 0;
    uint16_t(*ekey)[4] = (uint16_t(*)[4])malloc(sizeof(uint16_t) * (rounds)*4);
    key = (uint16_t *)malloc(sizeof(uint16_t) * (KeyLen / 8));
    memcpy(key, Seedkey, (KeyLen / 8) * sizeof(uint16_t));
    MatGf2 mat = make_transposition(64);
    MatGf2 mat_back = make_transposition_back(64);
    for (i = 0; i < rounds; i++)
    {
        RotateKeyByte((uint8_t *)key, KeyLen, L);
        subkey[0] = key[0];
        subkey[1] = key[1];
        subkey[2] = key[2];
        subkey[3] = key[3];
        round_constant = round_constant + DELTA_128;
        AddRoundConstant(subkey, round_constant);

        *((uint64_t *)subkey) = ApplyMatToU64(mat, *((uint64_t *)subkey));

        ekey[i][0] = subkey[0];
        ekey[i][1] = subkey[1];
        ekey[i][2] = subkey[2];
        ekey[i][3] = subkey[3];

        *((uint64_t *)subkey) = ApplyMatToU64(mat_back, *((uint64_t *)subkey));

        key[0] = subkey[0];
        key[1] = subkey[1];
        key[2] = subkey[2];
        key[3] = subkey[3];
    }
    memcpy(Subkey, ekey, (rounds)*4 * sizeof(uint16_t));
    free(key);
    free(ekey);
    MatGf2Free(mat);
    MatGf2Free(mat_back);
    mat = NULL;
    mat_back = NULL;
    return 0;
}

int swan_whitebox_128_content_init(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    swc->cfg = swh->cfg;
    swc->rounds = swh->rounds;
    swc->block_size = swh->block_size;
    swc->piece_count = swh->piece_count;
    int E_length = swc->block_size / TABLE_SIZE;
    const int beta_count = E_length / 2;
    swc->lut_128 = (uint64_t(*)[4][2][256])malloc(sizeof(uint64_t) * swc->rounds * 256 * 4 * 2);
    swc->lut_64 = NULL;
    swc->lut_256 = NULL;
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
        combined_affine_init(B_ptr++, swan_cfg_piecebit[swh->cfg] / 2, beta_count);
        combined_affine_init(C_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
        combined_affine_init(D_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
        combined_affine_init(E_ptr++, swan_cfg_piecebit[swh->cfg], swh->piece_count);
    }

    for (i = 0; i < (swh->rounds + 2); i++)
    {
        combined_affine_init(P_ptr++, swan_cfg_piecebit[swh->cfg] / 2, E_length / 2);
    }

    return 0;
}

int swan_whitebox_128_content_assemble(swan_whitebox_helper *swh, swan_whitebox_content *swc)
{
    int piece_count = swh->piece_count;
    int j, k, r;
    uint16_t i;
    const int E_length = swc->block_size / TABLE_SIZE;

    /*input encoding and output encoding*/
    CombinedAffine *P_ptr = swc->P;
    CombinedAffine *end_Ptr = swc->P + swh->rounds;
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

    P_ptr = swc->P;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *C_ptr = swc->C;
    MatGf2 temp;
    CombinedAffine *PQ_ptr = swc->PQ;

    /*
        theta:
            1.inv previous affine(P)
            2.shiftLanes
            3.transpose for beta
            4.add new affine(B)
    */
    MatGf2 transposition = make_transposition(64);
    MatGf2 rotate = make_right_rotate_shift(64, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);
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
    MatGf2 rotate_back = make_transposition_back(64);
    rotate = make_right_rotate_shift(64, ROL_A[swh->cfg], ROL_B[swh->cfg], ROL_C[swh->cfg]);
    uint16_t key_schedule[MAX_RK_SIZE][4];
    //key schedule
    Key_Schedule_128(swh->key, swan_cfg_keysizes[swh->cfg], 1, (uint8_t *)key_schedule, swan_cfg_keyrotates[swh->cfg], swh->rounds);
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
    uint64_t(*lut)[4][2][256] = swc->lut_128;
    if (swh->encrypt == 1)
    {
        for (r = 0; r < swh->rounds; r++)
        {

            uint16_t randomNum[4][4][2] = {0};

            int j = 0;
            int t = 0;
            int y;
            for (j = 0; j < 4; j++)
            {
                for (t = 0; t < 4; t++)
                {
                    for (y = 0; y < 2; y++)
                    {
                        randomNum[j][t][y] = rand();
                    }
                }
            }

            for (k = 0; k < piece_count; k++)
            {

                for (i = 0; i < (1 << TABLE_SIZE); i++)
                {

                    int n;
                    for (n = 0; n < 2; n++)
                    {
                        //  1.inv previous affine(B)
                        //  2.beta
                        uint8_t t8;
                        if (n == 0)
                        {
                            t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[2 * k], i) ^ ((key_schedule[r][k]) & 0x00ff);
                        }
                        else
                        {
                            t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[2 * k + 1], i) ^ ((key_schedule[r][k] >> 8) & 0x00ff);
                        }
                        uint16_t yc[4] = {0};
                        t8 = (S[(t8 >> 4) & 0x0f]) << 4 | (S[t8 & 0x0f]);
                        int t = 0;

                        if (n == 1)
                        {
                            yc[k] = ((uint16_t)t8 << 8) & 0xff00;
                        }
                        else
                        {
                            yc[k] = ((uint16_t)t8) & 0x00ff;
                        }
                        // 3. transpose back
                        *((uint64_t *)yc) = ApplyMatToU64(rotate_back, *((uint64_t *)yc));
                        // 4. shiftLanes
                        *((uint64_t *)yc) = ApplyMatToU64(rotate, *((uint64_t *)yc));

                        // 5.add random
                        // 6.add new affine(C)
                        *((uint64_t *)yc) = ApplyMatToU64((C_ptr->combined_affine->linear_map), *((uint64_t *)yc));

                        for (j = 0; j < 4; j++)
                        {
                            yc[j] = yc[j] ^ randomNum[k][j][n] ^ randomNum[(k + 1) % 4][j][n];
                        }

                        if (k == 0 && n == 0)
                        {
                            *((uint64_t *)yc) = AddMatToU64(C_ptr->combined_affine->vector_translation, *((uint64_t *)yc));
                        }
                        lut[r][k][n][i] = *((uint64_t *)yc);
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

            uint16_t randomNum[4][4][2];

            int j = 0;
            int t = 0;
            int y;
            for (j = 0; j < 4; j++)
            {
                for (t = 0; t < 4; t++)
                {
                    for (y = 0; y < 2; y++)
                    {
                        randomNum[j][t][y] = rand();
                    }
                }
            }

            for (k = 0; k < piece_count; k++)
            {

                for (i = 0; i < (1 << TABLE_SIZE); i++)
                {
                    int n;

                    for (n = 0; n < 2; n++)
                    {
                        uint8_t t8;
                        if (n == 0)
                        {
                            t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[2 * k], i) ^ ((key_schedule[r][k]) & 0x00ff);
                        }
                        else
                        {
                            t8 = ApplyAffineToU8((B_ptr)->sub_affine_inv[2 * k + 1], i) ^ ((key_schedule[r][k] >> 8) & 0x00ff);
                        }

                        uint16_t yc[4] = {0};
                        t8 = (S[(t8 >> 4) & 0x0f]) << 4 | (S[t8 & 0x0f]);
                        int t = 0;
                        if (n == 1)
                        {
                            yc[k] = ((uint16_t)t8 << 8) & 0xff00;
                        }
                        else
                        {
                            yc[k] = ((uint16_t)t8) & 0x00ff;
                        }

                        *((uint64_t *)yc) = ApplyMatToU64(rotate_back, *((uint64_t *)yc));
                        *((uint64_t *)yc) = ApplyMatToU64(rotate, *((uint64_t *)yc));

                        *((uint64_t *)yc) = ApplyMatToU64((C_ptr->combined_affine->linear_map), *((uint64_t *)yc));

                        for (j = 0; j < 4; j++)
                        {
                            yc[j] = yc[j] ^ randomNum[k][j][n] ^ randomNum[(k + 1) % 4][j][n];
                        }

                        if (k == 0 && n == 0)
                        {
                            *((uint64_t *)yc) = AddMatToU64(C_ptr->combined_affine->vector_translation, *((uint64_t *)yc));
                        }
                        lut[r][k][n][i] = *((uint64_t *)yc);
                    }
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
    MatGf2 switchmat = make_switchLanes(64);
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

int swan_whitebox_128_init(const uint8_t *key, int enc, int weak_or_strong, swan_whitebox_content *swc, enum swan_cipher_config_t cfg)
{
    swan_whitebox_helper *swh = (swan_whitebox_helper *)malloc(sizeof(swan_whitebox_helper));
    swan_whitebox_helper_init(key, swh, enc, weak_or_strong, cfg);
    swan_whitebox_128_content_init(swh, swc);
    swan_whitebox_128_content_assemble(swh, swc);

    return 0;
}
