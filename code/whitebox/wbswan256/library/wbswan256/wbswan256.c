#include <stdio.h>
#include <math/affine_transform.h>
#include <math/matrix_utils.h>
#include <wbswan/wbswan.h>
MatGf2 make_swithlane_128(int dim)
{
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    for (i = 0; i < 32; i++)
    {
        MatGf2Set(ind, i, i + 32, 1);
        MatGf2Set(ind, i, i + 64, 1);
        MatGf2Set(ind, i, i + 96, 1);
    }
    for (i = 32; i < 64; i++)
    {
        MatGf2Set(ind, i, i - 32, 1);
        MatGf2Set(ind, i, i + 64, 1);
        MatGf2Set(ind, i, i + 32, 1);
    }
    for (i = 64; i < 96; i++)
    {
        MatGf2Set(ind, i, i + 32, 1);
        MatGf2Set(ind, i, i - 64, 1);
        MatGf2Set(ind, i, i - 32, 1);
    }
    for (i = 96; i < 128; i++)
    {
        MatGf2Set(ind, i, i - 32, 1);
        MatGf2Set(ind, i, i - 64, 1);
        MatGf2Set(ind, i, i - 96, 1);
    }

    return ind;
}

MatGf2 make_right_rotate_shift_128(int dim, int r1, int r2, int r3)
{
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    for (i = 0; i < 32; i++)
    {
        MatGf2Set(ind, i, ((i + 32 - r3) % 32 + 0), 1);
    }
    for (i = 32; i < 64; i++)
    {
        MatGf2Set(ind, i, (i + 32 - r2) % 32 + 32, 1);
    }
    for (i = 64; i < 96; i++)
    {
        MatGf2Set(ind, i, (i + 32 - r1) % 32 + 64, 1);
    }
    for (i = 96; i < 128; i++)
    {
        MatGf2Set(ind, i, (i + 32 - 0) % 32 + 96, 1);
    }

    return ind;
}
MatGf2 make_transposition_128(int dim)
{
    // uint8_t rot[32] = {0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51};

    uint8_t rot[32] = {0, 32, 64, 96, 1, 33, 65, 97, 2, 34, 66, 98, 3, 35, 67, 99, 4, 36, 68, 100, 5, 37, 69, 101, 6, 38, 70, 102, 7, 39, 71, 103};
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    int j;
    int row;
    for (i = 1; i <= 4; i++)
    {
        row = 128 - 32 * i;

        for (j = 0; j < 32; j++)
        {
            MatGf2Set(ind, row++, rot[j] + 8 * (i - 1), 1);
        }
    }
    return ind;
}

MatGf2 make_transposition_back_128(int dim)
{

    uint8_t rot[32] = {96, 100, 104, 108, 112, 116, 120, 124, 64, 68, 72, 76, 80, 84, 88, 92, 32, 36, 40, 44, 48, 52, 56, 60, 0, 4, 8, 12, 16, 20, 24, 28};
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    int j;
    int row = 0;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 32; j++)
        {
            MatGf2Set(ind, row++, rot[j] + i, 1);
        }
    }
    return ind;
}

int swan_whitebox_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    swan_wb_unit L[4];
    swan_wb_unit R[4];
    swan_wb_unit LT[4];
    swan_wb_unit RT[4];
    swan_wb_semi *tempL;
    swan_wb_semi *tempR;

    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    swan_wb_semi(*lut_ptr)[4][4][256] = swc->lut;

    tempL = (swan_wb_semi *)LT;
    tempR = (swan_wb_semi *)RT;

    uint8_t tempIn[32];

    for (i = 0; i < 32; i++)
    {
        tempIn[i] = swc->SE[i][in[i]];
    }

    L[0] = LT[0] = *((uint32_t *)tempIn);
    L[1] = LT[1] = *((uint32_t *)tempIn + 1);
    L[2] = LT[2] = *((uint32_t *)tempIn + 2);
    L[3] = LT[3] = *((uint32_t *)tempIn + 3);

    R[0] = RT[0] = *((uint32_t *)tempIn + 4);
    R[1] = RT[1] = *((uint32_t *)tempIn + 5);
    R[2] = RT[2] = *((uint32_t *)tempIn + 6);
    R[3] = RT[3] = *((uint32_t *)tempIn + 7);

    for (i = 0; i < swc->rounds; i++)
    {

        LT[0] = L[0];
        LT[1] = L[1];
        LT[2] = L[2];
        LT[3] = L[3];

        // start theta
        *tempL = ApplyAffineToU128(*(B_ptr->combined_affine), *tempL);

        // start beta

        __uint128_t LC[4][4];

        __uint128_t temp = 0x0000000000000000;

        int j;
        int x;
        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 4; x++)
            {
                LC[j][x] = lut_ptr[i][j][x][((LT[j] >> (8 * x)) & 0x000000ff)];
                temp ^= LC[j][x];
            }
        }

        *tempL = temp;

        *tempL = ApplyAffineToU128(*(D_ptr->combined_affine), *tempL);

        swan_wb_unit tempd[4];

        *((swan_wb_semi *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((swan_wb_semi *)L));

        L[0] = R[0] ^ LT[0];
        L[1] = R[1] ^ LT[1];
        L[2] = R[2] ^ LT[2];
        L[3] = R[3] ^ LT[3];

        R[0] = tempd[0];
        R[1] = tempd[1];
        R[2] = tempd[2];
        R[3] = tempd[3];

        swan_wb_semi LP;
        if (swc->weak_or_strong)
        {
            swan_wb_semi p = *((swan_wb_semi *)L);

            swan_wb_unit ULP[4];
            swan_wb_semi *ulp_ptr = (swan_wb_semi *)ULP;
            *ulp_ptr = ApplyAffineToU128(*(F_ptr->combined_affine), p);
            LP = *ulp_ptr;
            F_ptr++;
        }

        B_ptr++;
        D_ptr++;
        E_ptr++;
        i++;

        // next round

        LT[0] = L[0];
        LT[1] = L[1];
        LT[2] = L[2];
        LT[3] = L[3];

        if (swc->weak_or_strong)
        {
            *((swan_wb_semi *)L) = LP;
        }

        // start theta
        *tempL = ApplyAffineToU128(*(B_ptr->combined_affine), *tempL);

        //start beta
        temp = 0x0000000000000000;

        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 4; x++)
            {
                LC[j][x] = lut_ptr[i][j][x][((LT[j] >> (8 * x)) & 0x000000ff)];
                temp ^= LC[j][x];
            }
        }

        *tempL = temp;

        *tempL = ApplyAffineToU128(*(D_ptr->combined_affine), *tempL);

        *((swan_wb_semi *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((swan_wb_semi *)L));

        L[0] = R[0] ^ LT[0];
        L[1] = R[1] ^ LT[1];
        L[2] = R[2] ^ LT[2];
        L[3] = R[3] ^ LT[3];

        R[0] = tempd[0];
        R[1] = tempd[1];
        R[2] = tempd[2];
        R[3] = tempd[3];

        B_ptr++;
        D_ptr++;
        E_ptr++;
    }

    for (i = 0; i < 16; i++)
    {
        out[i] = swc->EE[i][*((uint8_t *)L + i)];
    }

    for (i = 16; i < 32; i++)
    {
        out[i] = swc->EE[i][*((uint8_t *)R + i - 16)];
    }

    return 0;
}

int swan_whitebox_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    swan_wb_unit L[4];
    swan_wb_unit R[4];
    swan_wb_unit LT[4];
    swan_wb_unit RT[4];
    swan_wb_semi *tempL;
    swan_wb_semi *tempR;

    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    swan_wb_semi(*lut_ptr)[4][4][256] = swc->lut;

    tempL = (swan_wb_semi *)LT;
    tempR = (swan_wb_semi *)RT;

    uint8_t tempIn[32];

    for (i = 0; i < 16; i++)
    {
        tempIn[i] = swc->SE[i + 16][in[i]];
    }
    for (i = 16; i < 32; i++)
    {
        tempIn[i] = swc->SE[i-16][in[i]];
    }

    L[0] = LT[0] = *((uint32_t *)tempIn);
    L[1] = LT[1] = *((uint32_t *)tempIn + 1);
    L[2] = LT[2] = *((uint32_t *)tempIn + 2);
    L[3] = LT[3] = *((uint32_t *)tempIn + 3);

    R[0] = RT[0] = *((uint32_t *)tempIn + 4);
    R[1] = RT[1] = *((uint32_t *)tempIn + 5);
    R[2] = RT[2] = *((uint32_t *)tempIn + 6);
    R[3] = RT[3] = *((uint32_t *)tempIn + 7);

    for (i = 0; i < swc->rounds; i++)
    {

        RT[0] = R[0];
        RT[1] = R[1];
        RT[2] = R[2];
        RT[3] = R[3];

        // start theta
        *tempR = ApplyAffineToU128(*(B_ptr->combined_affine), *tempR);

        //start beta
        swan_wb_semi RC[4][4];

        swan_wb_semi temp = 0x00;
        int j;
        int x;
        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 4; x++)
            {
                RC[j][x] = lut_ptr[swc->rounds - 1 - (i)][j][x][((RT[j] >> (8 * x)) & 0x000000ff)];
                temp ^= RC[j][x];
            }
        }

        *tempR = temp;

        *tempR = ApplyAffineToU128(*(D_ptr->combined_affine), *tempR);

        swan_wb_unit tempd[4];

        *((swan_wb_semi *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((swan_wb_semi *)R));

        R[0] = L[0] ^ RT[0];
        R[1] = L[1] ^ RT[1];
        R[2] = L[2] ^ RT[2];
        R[3] = L[3] ^ RT[3];

        L[0] = tempd[0];
        L[1] = tempd[1];
        L[2] = tempd[2];
        L[3] = tempd[3];

        swan_wb_semi LP;
        if (swc->weak_or_strong)
        {
            swan_wb_semi p = *((swan_wb_semi *)R);

            swan_wb_unit ULP[4];
            swan_wb_semi *ulp_ptr = (swan_wb_semi *)ULP;
            *ulp_ptr = ApplyAffineToU128(*(F_ptr->combined_affine), p);
            LP = *ulp_ptr;
            F_ptr++;
        }

        B_ptr++;
        D_ptr++;
        E_ptr++;
        i++;
        // next round

        RT[0] = R[0];
        RT[1] = R[1];
        RT[2] = R[2];
        RT[3] = R[3];

        if (swc->weak_or_strong)
        {
            *((swan_wb_semi *)R) = LP;
        }

        *tempR = ApplyAffineToU128(*(B_ptr->combined_affine), *tempR);

        //start beta
        temp = 0x0000000000000000;

        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 4; x++)
            {
                RC[j][x] = lut_ptr[swc->rounds - 1 - (i)][j][x][((RT[j] >> 8 * x) & 0x000000ff)];
                temp ^= RC[j][x];
            }
        }

        *tempR = temp;

        *tempR = ApplyAffineToU128(*(D_ptr->combined_affine), *tempR);

        *((swan_wb_semi *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((swan_wb_semi *)R));

        R[0] = L[0] ^ RT[0];
        R[1] = L[1] ^ RT[1];
        R[2] = L[2] ^ RT[2];
        R[3] = L[3] ^ RT[3];

        L[0] = tempd[0];
        L[1] = tempd[1];
        L[2] = tempd[2];
        L[3] = tempd[3];

        B_ptr++;
        D_ptr++;
        E_ptr++;
    }

    for (i = 0; i < 16; i++)
    {

        out[i] = swc->EE[16 + i][*((uint8_t *)L + i)];
    }
    for (i = 16; i < 32; i++)
    {
        out[i] = swc->EE[i - 16][*((uint8_t *)R + i - 16)];
    }

    return 0;
}

int swan_whitebox_release(swan_whitebox_content *swc)
{
    // TODO:
    // AffineTransformFree(swc->)

    int i, j;
    CombinedAffine *B_ptr = swc->B;
    for (i = 0; i < swc->rounds; i++)
    {
        combined_affine_free(B_ptr++);
    }
    free(swc->B);
    swc->B = NULL;
    CombinedAffine *C_ptr = swc->C;
    for (i = 0; i < swc->rounds; i++)
    {
        combined_affine_free(C_ptr++);
    }
    free(swc->C);
    swc->C = NULL;
    CombinedAffine *P_ptr = swc->P;
    for (i = 0; i < swc->rounds + 2; i++)
    {
        combined_affine_free(P_ptr++);
    }
    free(swc->P);
    swc->P = NULL;

    //free memory
    free(swc->lut);
    swc->lut = NULL;

    return 0;
}

int swan_wb_export_to_bytes(const swan_whitebox_content *swc, uint8_t **dest)
{
    if (*dest != NULL)
        return -1;
    int sz = 0;
    sz = sizeof(swan_whitebox_content);
    // sz += swc->rounds * swc->aff_in_round * sizeof(AffineTransform);    //round_aff
    sz += 4 * swc->rounds * swc->piece_count * sizeof(swan_wb_semi) * 256; //LUT
    sz += 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;               //SE
    sz += 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;               //EE

    void **B_ptr_list = malloc(swc->rounds * sizeof(void *));
    void **D_ptr_list = malloc(swc->rounds * sizeof(void *));
    void **E_ptr_list = malloc(swc->rounds * sizeof(void *));

    void **F_ptr_list;
    if (swc->weak_or_strong)
    {
        F_ptr_list = malloc(swc->rounds * sizeof(void *));
    }

    int i;
    int j;
    int sum = 0;
    for (i = 0; i < swc->rounds; i++)
    {

        B_ptr_list[i] = ExportAffineToStr((swc->B + i)->combined_affine);
        sz += *(uint32_t *)B_ptr_list[i];
    }

    for (i = 0; i < swc->rounds; i++)
    {

        D_ptr_list[i] = ExportAffineToStr((swc->D + i)->combined_affine);
        sz += *(uint32_t *)D_ptr_list[i];
    }

    for (i = 0; i < swc->rounds; i++)
    {

        E_ptr_list[i] = ExportAffineToStr((swc->E + i)->combined_affine);
        sz += *(uint32_t *)E_ptr_list[i];
    }
    if (swc->weak_or_strong)
    {
        for (i = 0; i < (swc->rounds); i++)
        {
            F_ptr_list[i] = ExportAffineToStr((swc->F + i)->combined_affine);
            sz += *(uint32_t *)F_ptr_list[i];
        }
    }

    *dest = malloc(sz);
    *((uint32_t *)*dest) = sz;
    uint8_t *ds = *dest + sizeof(uint32_t);
    memcpy(ds, swc, sizeof(swan_whitebox_content));
    ds += sizeof(swan_whitebox_content);
    int k;
    k = 4 * swc->rounds * swc->piece_count * sizeof(swan_wb_semi) * 256;
    memcpy(ds, swc->lut, k);
    ds += k;

    k = 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;
    memcpy(ds, swc->SE, k);
    ds += k;

    k = 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;
    memcpy(ds, swc->EE, k);
    ds += k;

    for (i = 0; i < swc->rounds; i++)
    {
        k = *(uint32_t *)B_ptr_list[i];
        memcpy(ds, B_ptr_list[i], k);
        ds += k;
    }

    for (i = 0; i < swc->rounds; i++)
    {
        k = *(uint32_t *)D_ptr_list[i];
        memcpy(ds, D_ptr_list[i], k);
        ds += k;
    }

    for (i = 0; i < swc->rounds; i++)
    {
        k = *(uint32_t *)E_ptr_list[i];
        memcpy(ds, E_ptr_list[i], k);
        ds += k;
    }
    if (swc->weak_or_strong)
    {
        for (i = 0; i < swc->rounds; i++)
        {
            k = *(uint32_t *)F_ptr_list[i];
            memcpy(ds, F_ptr_list[i], k);
            ds += k;
        }
    }

    return sz;
}

int swan_wb_import_from_bytes(const uint8_t *source, swan_whitebox_content *swc)
{
    const void *ptr = source;
    ptr += sizeof(uint32_t);

    memcpy(swc, ptr, sizeof(swan_whitebox_content));
    ptr += sizeof(swan_whitebox_content);

    int i;
    int j;
    int k;
    k = 4 * swc->rounds * swc->piece_count * sizeof(swan_wb_semi) * 256;
    swc->lut = (swan_wb_semi(*)[4][4][256])malloc(k);
    memcpy(swc->lut, ptr, k);
    ptr += k;

    k = 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;
    memcpy(swc->SE, ptr, k);
    ptr += k;

    k = 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;
    memcpy(swc->EE, ptr, k);
    ptr += k;

    swc->B = (CombinedAffine *)malloc((swc->rounds) * sizeof(CombinedAffine));

    swc->D = (CombinedAffine *)malloc(swc->rounds * sizeof(CombinedAffine));

    swc->E = (CombinedAffine *)malloc(swc->rounds * sizeof(CombinedAffine));

    swc->F = (CombinedAffine *)malloc(swc->rounds * sizeof(CombinedAffine));

    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;

    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(B_ptr++, SWAN_PIECE_BIT, swc->piece_count);
    }
    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(D_ptr++, SWAN_PIECE_BIT, swc->piece_count);
    }

    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(E_ptr++, SWAN_PIECE_BIT, swc->piece_count);
    }
    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(F_ptr++, SWAN_PIECE_BIT, swc->piece_count);
    }

    for (i = 0; i < swc->rounds; i++)
    {
        uint32_t aff_sz = *((uint32_t *)ptr);
        *((swc->B + i)->combined_affine) = ImportAffineFromStr(ptr);
        ptr += aff_sz;
    }
    for (i = 0; i < swc->rounds; i++)
    {
        uint32_t aff_sz = *((uint32_t *)ptr);
        *((swc->D + i)->combined_affine) = ImportAffineFromStr(ptr);
        ptr += aff_sz;
    }
    for (i = 0; i < swc->rounds; i++)
    {
        uint32_t aff_sz = *((uint32_t *)ptr);
        *((swc->E + i)->combined_affine) = ImportAffineFromStr(ptr);
        ptr += aff_sz;
    }
    for (i = 0; i < swc->rounds; i++)
    {
        uint32_t aff_sz = *((uint32_t *)ptr);
        *((swc->F + i)->combined_affine) = ImportAffineFromStr(ptr);
        ptr += aff_sz;
    }

    return 0;
}
