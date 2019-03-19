#include <stdio.h>
#include <wbswan/wbswan.h>
#include <math/martrix_gen.h>
int swan_whitebox_256_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    uint32_t L[4];
    uint32_t R[4];
    uint32_t LT[4];
    uint32_t RT[4];
    __uint128_t *tempL = (__uint128_t *)LT;
    __uint128_t *tempR = (__uint128_t *)RT;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    __uint128_t(*lut_ptr)[4][4][256] = swc->lut_256;
    uint8_t tempIn[32];

    // input encoding
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

        // start w
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

        // switchLanes
        *tempL = ApplyAffineToU128(*(D_ptr->combined_affine), *tempL);

        uint32_t tempd[4];

        *((__uint128_t *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((__uint128_t *)L));

        L[0] = R[0] ^ LT[0];
        L[1] = R[1] ^ LT[1];
        L[2] = R[2] ^ LT[2];
        L[3] = R[3] ^ LT[3];

        R[0] = tempd[0];
        R[1] = tempd[1];
        R[2] = tempd[2];
        R[3] = tempd[3];

        __uint128_t LP;
        if (swc->weak_or_strong)
        {
            __uint128_t p = *((__uint128_t *)L);
            uint32_t ULP[4];
            __uint128_t *ulp_ptr = (__uint128_t *)ULP;
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
            *((__uint128_t *)L) = LP;
        }

        // start theta
        *tempL = ApplyAffineToU128(*(B_ptr->combined_affine), *tempL);

        //start w
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

        // switchLanes
        *tempL = ApplyAffineToU128(*(D_ptr->combined_affine), *tempL);

        *((__uint128_t *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((__uint128_t *)L));

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

int swan_whitebox_256_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    uint32_t L[4];
    uint32_t R[4];
    uint32_t LT[4];
    uint32_t RT[4];
    __uint128_t *tempL = (__uint128_t *)LT;
    __uint128_t *tempR = (__uint128_t *)RT;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    __uint128_t(*lut_ptr)[4][4][256] = swc->lut_256;

    uint8_t tempIn[32];

    // output decoding
    for (i = 0; i < 16; i++)
    {
        tempIn[i] = swc->SE[i + 16][in[i]];
    }
    for (i = 16; i < 32; i++)
    {
        tempIn[i] = swc->SE[i - 16][in[i]];
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

        //start w
        __uint128_t RC[4][4];

        __uint128_t temp = 0x00;
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

        uint32_t tempd[4];

        *((__uint128_t *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((__uint128_t *)R));

        R[0] = L[0] ^ RT[0];
        R[1] = L[1] ^ RT[1];
        R[2] = L[2] ^ RT[2];
        R[3] = L[3] ^ RT[3];

        L[0] = tempd[0];
        L[1] = tempd[1];
        L[2] = tempd[2];
        L[3] = tempd[3];

        __uint128_t LP;
        if (swc->weak_or_strong)
        {
            __uint128_t p = *((__uint128_t *)R);

            uint32_t ULP[4];
            __uint128_t *ulp_ptr = (__uint128_t *)ULP;
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
            *((__uint128_t *)R) = LP;
        }

        *tempR = ApplyAffineToU128(*(B_ptr->combined_affine), *tempR);

        //start w
        temp = 0x0000000000000000;

        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 4; x++)
            {
                RC[j][x] = lut_ptr[swc->rounds - 1 - (i)][j][x][((RT[j] >> (8 * x)) & 0x000000ff)];
                temp ^= RC[j][x];
            }
        }

        *tempR = temp;

        // switchLanes
        *tempR = ApplyAffineToU128(*(D_ptr->combined_affine), *tempR);

        *((__uint128_t *)tempd) = ApplyAffineToU128(*(E_ptr->combined_affine), *((__uint128_t *)R));

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

    // output decoding
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
