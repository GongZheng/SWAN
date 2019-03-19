#include <stdio.h>
#include <wbswan/wbswan.h>

int swan_whitebox_64_encrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    uint8_t L[4];
    uint8_t R[4];
    uint8_t LT[4];
    uint8_t RT[4];
    uint32_t *tempL = (uint32_t *)LT;
    uint32_t *tempR = (uint32_t *)RT;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    uint32_t(*lut_ptr)[4][256] = swc->lut_64;

    L[0] = LT[0] = swc->SE[0][in[0]];
    L[1] = LT[1] = swc->SE[1][in[1]];
    L[2] = LT[2] = swc->SE[2][in[2]];
    L[3] = LT[3] = swc->SE[3][in[3]];

    R[0] = RT[0] = swc->SE[4][in[4]];
    R[1] = RT[1] = swc->SE[5][in[5]];
    R[2] = RT[2] = swc->SE[6][in[6]];
    R[3] = RT[3] = swc->SE[7][in[7]];

    for (i = 0; i < swc->rounds; i++)
    {

        LT[0] = L[0];
        LT[1] = L[1];
        LT[2] = L[2];
        LT[3] = L[3];

        // start theta
        *tempL = ApplyAffineToU32(*(B_ptr->combined_affine), *tempL);

        // start w
        uint32_t LC[4];
        uint32_t temp = 0x00000000;
        int j;
        int x;

        for (j = 0; j < 4; j++)
        {

            LC[j] = lut_ptr[i][j][LT[j]];
            temp ^= LC[j];
        }

        *tempL = temp;

        //switchLanes
        *tempL = ApplyAffineToU32(*(D_ptr->combined_affine), *tempL);

        uint8_t tempd[4];

        *((uint32_t *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine), *((uint32_t *)L));

        L[0] = R[0] ^ LT[0];
        L[1] = R[1] ^ LT[1];
        L[2] = R[2] ^ LT[2];
        L[3] = R[3] ^ LT[3];

        R[0] = tempd[0];
        R[1] = tempd[1];
        R[2] = tempd[2];
        R[3] = tempd[3];

        uint32_t LP;
        if (swc->weak_or_strong)
        {
            uint32_t p = *((uint32_t *)L);
            uint8_t ULP[4];
            uint32_t *ulp_ptr = (uint32_t *)ULP;
            *ulp_ptr = ApplyAffineToU32(*(F_ptr->combined_affine), p);
            LP = *ulp_ptr;
            F_ptr++;
        }

        B_ptr++;
        D_ptr++;
        E_ptr++;
        i++;

        // ******* next round *****//

        LT[0] = L[0];
        LT[1] = L[1];
        LT[2] = L[2];
        LT[3] = L[3];

        if (swc->weak_or_strong)
        {
            *((uint32_t *)L) = LP;
        }

        // start theta
        *tempL = ApplyAffineToU32(*(B_ptr->combined_affine), *tempL);

        //start w
        temp = 0x00000000;

        for (j = 0; j < 4; j++)
        {

            LC[j] = lut_ptr[i][j][LT[j]];
            temp ^= LC[j];
        }

        *tempL = temp;

        // switchLanes
        *tempL = ApplyAffineToU32(*(D_ptr->combined_affine), *tempL);

        *((uint32_t *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine), *((uint32_t *)L));

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

    //output decoding
    out[0] = swc->EE[0][L[0]];
    out[1] = swc->EE[1][L[1]];
    out[2] = swc->EE[2][L[2]];
    out[3] = swc->EE[3][L[3]];

    out[4] = swc->EE[4][R[0]];
    out[5] = swc->EE[5][R[1]];
    out[6] = swc->EE[6][R[2]];
    out[7] = swc->EE[7][R[3]];

    return 0;
}

int swan_whitebox_64_decrypt(const uint8_t *in, uint8_t *out, swan_whitebox_content *swc)
{
    int i;
    uint8_t L[4];
    uint8_t R[4];
    uint8_t LT[4];
    uint8_t RT[4];
    uint32_t *tempL = (uint32_t *)LT;
    uint32_t *tempR = (uint32_t *)RT;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    uint32_t(*lut_ptr)[4][256] = swc->lut_64;

    LT[0] = in[0];
    LT[1] = in[1];
    LT[2] = in[2];
    LT[3] = in[3];

    RT[0] = in[4];
    RT[1] = in[5];
    RT[2] = in[6];
    RT[3] = in[7];

    // input encoding
    L[0] = LT[0] = swc->SE[4][in[0]];
    L[1] = LT[1] = swc->SE[5][in[1]];
    L[2] = LT[2] = swc->SE[6][in[2]];
    L[3] = LT[3] = swc->SE[7][in[3]];

    R[0] = RT[0] = swc->SE[0][in[4]];
    R[1] = RT[1] = swc->SE[1][in[5]];
    R[2] = RT[2] = swc->SE[2][in[6]];
    R[3] = RT[3] = swc->SE[3][in[7]];

    for (i = 0; i < swc->rounds; i++)
    {

        RT[0] = R[0];
        RT[1] = R[1];
        RT[2] = R[2];
        RT[3] = R[3];

        // start theta
        *tempR = ApplyAffineToU32(*(B_ptr->combined_affine), *tempR);

        //start w
        uint32_t RC[4];
        uint32_t temp = 0x00000000;
        int j;
        int x;

        for (j = 0; j < 4; j++)
        {

            RC[j] = lut_ptr[swc->rounds - 1 - i][j][RT[j]];
            temp ^= RC[j];
        }

        *tempR = temp;

        // switchLanes
        *tempR = ApplyAffineToU32(*(D_ptr->combined_affine), *tempR);

        uint8_t tempd[4];

        *((uint32_t *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine), *((uint32_t *)R));

        R[0] = L[0] ^ RT[0];
        R[1] = L[1] ^ RT[1];
        R[2] = L[2] ^ RT[2];
        R[3] = L[3] ^ RT[3];

        L[0] = tempd[0];
        L[1] = tempd[1];
        L[2] = tempd[2];
        L[3] = tempd[3];

        uint32_t LP;
        if (swc->weak_or_strong)
        {
            uint32_t p = *((uint32_t *)R);

            uint8_t ULP[4];
            uint32_t *ulp_ptr = (uint32_t *)ULP;
            *ulp_ptr = ApplyAffineToU32(*(F_ptr->combined_affine), p);
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
            *((uint32_t *)R) = LP;
        }

        // start theta
        *tempR = ApplyAffineToU32(*(B_ptr->combined_affine), *tempR);

        //start w
        temp = 0x00000000;

        for (j = 0; j < 4; j++)
        {

            RC[j] = lut_ptr[swc->rounds - 1 - i][j][RT[j]];
            temp ^= RC[j];
        }

        *tempR = temp;

        // switchLanes
        *tempR = ApplyAffineToU32(*(D_ptr->combined_affine), *tempR);

        *((uint32_t *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine), *((uint32_t *)R));

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
    out[0] = swc->EE[4][L[0]];
    out[1] = swc->EE[5][L[1]];
    out[2] = swc->EE[6][L[2]];
    out[3] = swc->EE[7][L[3]];

    out[4] = swc->EE[0][R[0]];
    out[5] = swc->EE[1][R[1]];
    out[6] = swc->EE[2][R[2]];
    out[7] = swc->EE[3][R[3]];

    return 0;
}
