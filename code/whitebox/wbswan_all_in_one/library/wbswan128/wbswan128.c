#include <stdio.h>
#include <wbswan/wbswan.h>

int swan_whitebox_128_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t LT[4];
    uint16_t RT[4];
    uint64_t *tempL = (uint64_t *)LT;
    uint64_t *tempR = (uint64_t *)RT;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    uint64_t(*lut_ptr)[4][2][256] = swc->lut_128;
    uint8_t tempIn[16];

    // input encoding
    for (i = 0; i < 16; i++)
    {
        tempIn[i] = swc->SE[i][in[i]];
    }

    L[0] = LT[0] = *((uint16_t *)tempIn);
    L[1] = LT[1] = *((uint16_t *)tempIn + 1);
    L[2] = LT[2] = *((uint16_t *)tempIn + 2);
    L[3] = LT[3] = *((uint16_t *)tempIn + 3);

    R[0] = RT[0] = *((uint16_t *)tempIn + 4);
    R[1] = RT[1] = *((uint16_t *)tempIn + 5);
    R[2] = RT[2] = *((uint16_t *)tempIn + 6);
    R[3] = RT[3] = *((uint16_t *)tempIn + 7);

    for (i = 0; i < swc->rounds; i++)
    {

        LT[0] = L[0];
        LT[1] = L[1];
        LT[2] = L[2];
        LT[3] = L[3];

        // start theta
        *tempL = ApplyAffineToU64(*(B_ptr->combined_affine), *tempL);

        // start w
        uint64_t LC[4][2];
        uint64_t temp = 0x0000000000000000;
        int j;
        int x;

        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 2; x++)
            {
                LC[j][x] = lut_ptr[i][j][x][x ? ((LT[j] >> 8) & 0x00ff) : LT[j] & 0x00ff];
                temp ^= LC[j][x];
            }
        }

        *tempL = temp;

        // switchLanes
        *tempL = ApplyAffineToU64(*(D_ptr->combined_affine), *tempL);

        uint16_t tempd[4];

        *((uint64_t *)tempd) = ApplyAffineToU64(*(E_ptr->combined_affine), *((uint64_t *)L));

        L[0] = R[0] ^ LT[0];
        L[1] = R[1] ^ LT[1];
        L[2] = R[2] ^ LT[2];
        L[3] = R[3] ^ LT[3];

        R[0] = tempd[0];
        R[1] = tempd[1];
        R[2] = tempd[2];
        R[3] = tempd[3];

        uint64_t LP;
        if (swc->weak_or_strong)
        {
            uint64_t p = *((uint64_t *)L);

            uint16_t ULP[4];
            uint64_t *ulp_ptr = (uint64_t *)ULP;
            *ulp_ptr = ApplyAffineToU64(*(F_ptr->combined_affine), p);
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
            *((uint64_t *)L) = LP;
        }

        // start theta
        *tempL = ApplyAffineToU64(*(B_ptr->combined_affine), *tempL);

        // start w
        temp = 0x0000000000000000;
        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 2; x++)
            {
                LC[j][x] = lut_ptr[i][j][x][x ? ((LT[j] >> 8) & 0x00ff) : LT[j] & 0x00ff];
                temp ^= LC[j][x];
            }
        }

        *tempL = temp;

        // switchLanes
        *tempL = ApplyAffineToU64(*(D_ptr->combined_affine), *tempL);

        *((uint64_t *)tempd) = ApplyAffineToU64(*(E_ptr->combined_affine), *((uint64_t *)L));

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

    // output decoding
    for (i = 0; i < 8; i++)
    {
        out[i] = swc->EE[i][*((uint8_t *)L + i)];
    }

    for (i = 8; i < 16; i++)
    {
        out[i] = swc->EE[i][*((uint8_t *)R + i - 8)];
    }

    return 0;
}

int swan_whitebox_128_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    uint16_t L[4];
    uint16_t R[4];
    uint16_t LT[4];
    uint16_t RT[4];
    uint64_t *tempL = (uint64_t *)LT;
    uint64_t *tempR = (uint64_t *)RT;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    uint64_t(*lut_ptr)[4][2][256] = swc->lut_128;
    uint8_t tempIn[16];

    // input encoding
    for (i = 0; i < 8; i++)
    {
        tempIn[i] = swc->SE[i + 8][in[i]];
    }
    for (i = 8; i < 16; i++)
    {
        tempIn[i] = swc->SE[i - 8][in[i]];
    }

    L[0] = LT[0] = *((uint16_t *)tempIn);
    L[1] = LT[1] = *((uint16_t *)tempIn + 1);
    L[2] = LT[2] = *((uint16_t *)tempIn + 2);
    L[3] = LT[3] = *((uint16_t *)tempIn + 3);

    R[0] = RT[0] = *((uint16_t *)tempIn + 4);
    R[1] = RT[1] = *((uint16_t *)tempIn + 5);
    R[2] = RT[2] = *((uint16_t *)tempIn + 6);
    R[3] = RT[3] = *((uint16_t *)tempIn + 7);

    for (i = 0; i < swc->rounds; i++)
    {

        RT[0] = R[0];
        RT[1] = R[1];
        RT[2] = R[2];
        RT[3] = R[3];

        // start theta
        *tempR = ApplyAffineToU64(*(B_ptr->combined_affine), *tempR);

        //start w
        uint64_t RC[4][2];
        uint64_t temp = 0x0000000000000000;
        int j;
        int x;

        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 2; x++)
            {
                RC[j][x] = lut_ptr[swc->rounds - 1 - (i)][j][x][x ? ((RT[j] >> 8) & 0x00ff) : RT[j] & 0x00ff];
                temp ^= RC[j][x];
            }
        }

        *tempR = temp;

        // switchLanes
        *tempR = ApplyAffineToU64(*(D_ptr->combined_affine), *tempR);

        uint16_t tempd[4];

        *((uint64_t *)tempd) = ApplyAffineToU64(*(E_ptr->combined_affine), *((uint64_t *)R));

        R[0] = L[0] ^ RT[0];
        R[1] = L[1] ^ RT[1];
        R[2] = L[2] ^ RT[2];
        R[3] = L[3] ^ RT[3];

        L[0] = tempd[0];
        L[1] = tempd[1];
        L[2] = tempd[2];
        L[3] = tempd[3];

        uint64_t LP;
        if (swc->weak_or_strong)
        {
            uint64_t p = *((uint64_t *)R);

            uint16_t ULP[4];
            uint64_t *ulp_ptr = (uint64_t *)ULP;
            *ulp_ptr = ApplyAffineToU64(*(F_ptr->combined_affine), p);
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
            *((uint64_t *)R) = LP;
        }

        *tempR = ApplyAffineToU64(*(B_ptr->combined_affine), *tempR);

        //start w
        temp = 0x0000000000000000;
        for (j = 0; j < 4; j++)
        {
            for (x = 0; x < 2; x++)
            {
                RC[j][x] = lut_ptr[swc->rounds - 1 - (i)][j][x][x ? ((RT[j] >> 8) & 0x00ff) : RT[j] & 0x00ff];
                temp ^= RC[j][x];
            }
        }

        *tempR = temp;

        *tempR = ApplyAffineToU64(*(D_ptr->combined_affine), *tempR);

        *((uint64_t *)tempd) = ApplyAffineToU64(*(E_ptr->combined_affine), *((uint64_t *)R));

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
    for (i = 0; i < 8; i++)
    {

        out[i] = swc->EE[8 + i][*((uint8_t *)L + i)];
    }
    for (i = 8; i < 16; i++)
    {
        out[i] = swc->EE[i - 8][*((uint8_t *)R + i - 8)];
    }

    return 0;
}
