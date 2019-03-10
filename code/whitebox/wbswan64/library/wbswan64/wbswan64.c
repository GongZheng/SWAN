#include <stdio.h>
#include <math/affine_transform.h>
#include <math/matrix_utils.h>
#include <wbswan/wbswan.h>


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
    swan_wb_semi(*lut_ptr)[4][256] = swc->lut;

    tempL = (swan_wb_semi *)LT;
    tempR = (swan_wb_semi *)RT;
    

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

        

        //start beta
        uint32_t LC[4];
        LC[0] = lut_ptr[i][0][LT[0]];
        LC[1] = lut_ptr[i][1][LT[1]];
        LC[2] = lut_ptr[i][2][LT[2]];
        LC[3] = lut_ptr[i][3][LT[3]];
        *tempL = LC[0] ^ LC[1] ^ LC[2] ^ LC[3];
    
        // inv C and P for next 
        *tempL = ApplyAffineToU32(*(D_ptr->combined_affine), *tempL);

        swan_wb_unit tempd[4];

        *((swan_wb_semi *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine),*((uint32_t *)L));

        L[0] = R[0] ^ LT[0];
        L[1] = R[1] ^ LT[1];
        L[2] = R[2] ^ LT[2];
        L[3] = R[3] ^ LT[3];

        R[0] = tempd[0];
        R[1] = tempd[1];
        R[2] = tempd[2];
        R[3] = tempd[3];

        swan_wb_semi LP;
        if (swc->weak_or_strong) {
            swan_wb_semi p = *((swan_wb_semi *)L);
            swan_wb_unit ULP[4];
            swan_wb_semi *ulp_ptr = (swan_wb_semi *) ULP;
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
            *((swan_wb_semi *)L) = LP;
        }
          
         // start theta
        *tempL = ApplyAffineToU32(*(B_ptr->combined_affine), *tempL);

         //start beta      
        LC[0] = lut_ptr[i][0][LT[0]];
        LC[1] = lut_ptr[i][1][LT[1]];
        LC[2] = lut_ptr[i][2][LT[2]];
        LC[3] = lut_ptr[i][3][LT[3]];
        *tempL = LC[0] ^ LC[1] ^ LC[2] ^ LC[3];

        *tempL = ApplyAffineToU32(*(D_ptr->combined_affine), *tempL);
        *((swan_wb_semi *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine), *((uint32_t *)L));

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
    swan_wb_semi(*lut_ptr)[4][256] = swc->lut;

    LT[0] = in[0];
    LT[1] = in[1];
    LT[2] = in[2];
    LT[3] = in[3];

    RT[0] = in[4];
    RT[1] = in[5];
    RT[2] = in[6];
    RT[3] = in[7];

    tempL = (swan_wb_semi *)LT;
    tempR = (swan_wb_semi *)RT;

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

        //start beta
        uint32_t RC[4];
        RC[0] = lut_ptr[swc->rounds - 1 - (i)][0][RT[0]];
        RC[1] = lut_ptr[swc->rounds - 1 - (i)][1][RT[1]];
        RC[2] = lut_ptr[swc->rounds - 1 - (i)][2][RT[2]];
        RC[3] = lut_ptr[swc->rounds - 1 - (i)][3][RT[3]];

        *tempR = RC[0] ^ RC[1] ^ RC[2] ^ RC[3];
     
        *tempR = ApplyAffineToU32(*(D_ptr->combined_affine), *tempR);

        swan_wb_unit tempd[4];

        *((swan_wb_semi *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine), *((uint32_t *)R));

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
            *ulp_ptr = ApplyAffineToU32(*(F_ptr->combined_affine),  p);
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

        // start theta
        *tempR = ApplyAffineToU32(*(B_ptr->combined_affine), *tempR);

        //start beta
        RC[0] = lut_ptr[swc->rounds - 1 - (i)][0][RT[0]];
        RC[1] = lut_ptr[swc->rounds - 1 - (i)][1][RT[1]];
        RC[2] = lut_ptr[swc->rounds - 1 - (i)][2][RT[2]];
        RC[3] = lut_ptr[swc->rounds - 1 - (i)][3][RT[3]];

        *tempR = RC[0] ^ RC[1] ^ RC[2] ^ RC[3];

        *tempR = ApplyAffineToU32(*(D_ptr->combined_affine), *tempR);

        *((swan_wb_semi *)tempd) = ApplyAffineToU32(*(E_ptr->combined_affine),  *((uint32_t *)R));

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
    sz += swc->rounds * swc->piece_count * sizeof(swan_wb_semi) * 256; //LUT
    sz += 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;           //SE
    sz += 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;           //EE

    void **B_ptr_list = malloc(swc->rounds  * sizeof(void *));
    void **D_ptr_list = malloc(swc->rounds * sizeof(void *));
    void **E_ptr_list = malloc(swc->rounds * sizeof(void *));
    void **F_ptr_list = malloc(swc->rounds * sizeof(void *));
    

    int i;
    int j;
    int sum = 0;
    for (i = 0; i < swc->rounds ; i++)
    {

        B_ptr_list[i] = ExportAffineToStr((swc->B+i)->combined_affine);
        sz += *(uint32_t *)B_ptr_list[i];
    }

    for (i = 0; i < swc->rounds; i++)
    {

        D_ptr_list[i] = ExportAffineToStr((swc->D + i)->combined_affine);
        sz += *(uint32_t *)D_ptr_list[i];
    }

    for (i = 0; i < swc->rounds ; i++)
    {

        E_ptr_list[i] = ExportAffineToStr((swc->E + i)->combined_affine);
        sz += *(uint32_t *)E_ptr_list[i];
    }

    for (i = 0; i < (swc->rounds); i++)
    {
        F_ptr_list[i] = ExportAffineToStr((swc->F + i)->combined_affine);
        sz += *(uint32_t *)F_ptr_list[i];
        }
    



    *dest = malloc(sz);
    *((uint32_t *)*dest) = sz;
    uint8_t *ds = *dest + sizeof(uint32_t);
    memcpy(ds, swc, sizeof(swan_whitebox_content));
    ds += sizeof(swan_whitebox_content);
    int k;
    k = swc->rounds * swc->piece_count * sizeof(swan_wb_semi) * 256;
    memcpy(ds, swc->lut, k);
    ds += k;

    k = 2 * swc->piece_count * sizeof(swan_wb_unit) * 256;
    memcpy(ds, swc->SE, k);
    ds += k;

    k = 2 * swc->piece_count *  sizeof(swan_wb_unit) * 256;
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

    for (i = 0; i < swc->rounds; i++)
    {
        k = *(uint32_t *)F_ptr_list[i];
        memcpy(ds, F_ptr_list[i], k);
        ds += k;
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
    k = swc->rounds * swc->piece_count * sizeof(swan_wb_semi) * 256;
    swc->lut = (swan_wb_semi(*)[4][256])malloc(k);
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

    for (i = 0; i < swc->rounds ; i++)
    {
        uint32_t aff_sz = *((uint32_t *)ptr);
        *((swc->B+i)->combined_affine) = ImportAffineFromStr(ptr);
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
