#include <stdio.h>
#include <wbswan/wbswan.h>
#include <stdint.h>

int _swan_whitebox_helper_init(swan_whitebox_helper *swh, const uint8_t *key, int weak_or_strong, enum swan_cipher_config_t cfg)
{

    int block_size = swh->block_size;
    int semi_block = block_size / 2;
    swh->piece_bit = swan_cfg_piecebit[cfg];
    swh->piece_count = semi_block / swh->piece_bit;
    swh->key = key;
    swh->weak_or_strong = weak_or_strong;

    return 0;
}

int swan_whitebox_helper_init(const uint8_t *key, swan_whitebox_helper *swh, int enc, int weak_or_strong, enum swan_cipher_config_t cfg)
{

    // uint8_t rk[MAX_RK_SIZE];
    int ret;
    swh->cfg = cfg;
    swh->encrypt = enc;
    swh->rounds = swan_cfg_rounds[swh->cfg];
    swh->block_size = swan_cfg_blocksizes[swh->cfg];
    return _swan_whitebox_helper_init(swh, key, weak_or_strong, cfg);
}
void RotateKeyByte(uint8_t *key, uint16_t keylength, int L)
{
    uint8_t i;
    uint8_t temp[L];
    uint8_t N = keylength / 8 - 1;
    for (i = 0; i < L; i++)
    {
        temp[i] = key[i];
    }

    //Right rotate every byte of the key;
    for (i = 0; i <= N - L; i++)
    {
        key[i] = key[i + L];
    }

    //Cyclic the first byte of the key to the MSB;
    for (i = 0; i < L; i++)
    {
        key[(N + 1) - L + i] = temp[i];
    }
}

int swan_wb_export_to_bytes(const swan_whitebox_content *swc, uint8_t **dest)
{
    if (*dest != NULL)
        return -1;
    int sz = 0;
    sz = sizeof(swan_whitebox_content);
    switch (swan_cfg_blocksizes[swc->cfg])
    {
    case 64:
        sz += swc->rounds * swc->piece_count * sizeof(uint32_t) * 256; //LUT
        sz += 2 * swc->piece_count * sizeof(uint8_t) * 256;            //SE
        sz += 2 * swc->piece_count * sizeof(uint8_t) * 256;            //EE

        break;
    case 128:
        sz += 2 * swc->rounds * swc->piece_count * sizeof(uint64_t) * 256; //LUT
        sz += 4 * swc->piece_count * sizeof(uint8_t) * 256;                //SE
        sz += 4 * swc->piece_count * sizeof(uint8_t) * 256;                //EE
        break;
    case 256:
        sz += 4 * swc->rounds * swc->piece_count * sizeof(__uint128_t) * 256; //LUT
        sz += 8 * swc->piece_count * sizeof(uint8_t) * 256;                   //SE
        sz += 8 * swc->piece_count * sizeof(uint8_t) * 256;                   //EE
        break;
    default:
        sz += 4 * swc->rounds * swc->piece_count * sizeof(__uint128_t) * 256; //LUT
        sz += 8 * swc->piece_count * sizeof(uint8_t) * 256;                   //SE
        sz += 8 * swc->piece_count * sizeof(uint8_t) * 256;                   //EE

        break;
    }

    void **B_ptr_list = malloc(swc->rounds * sizeof(void *));
    void **D_ptr_list = malloc(swc->rounds * sizeof(void *));
    void **E_ptr_list = malloc(swc->rounds * sizeof(void *));

    void **F_ptr_list;
    if (swc->weak_or_strong)
    {
        F_ptr_list = malloc(swc->rounds * sizeof(void *));
    }
    else
    {
        F_ptr_list = NULL;
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
    int k_lut;
    int k_e;
    switch (swan_cfg_blocksizes[swc->cfg])
    {
    case 64:
        k_lut = swc->rounds * swc->piece_count * sizeof(uint32_t) * 256; //LUT
        memcpy(ds, swc->lut_64, k_lut);
        ds += k_lut;
        k_e = 2 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE
        break;
    case 128:
        k_lut = 2 * swc->rounds * swc->piece_count * sizeof(uint64_t) * 256; //LUT
        memcpy(ds, swc->lut_128, k_lut);
        ds += k_lut;
        k_e = 4 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE
        break;
    case 256:
        k_lut = 4 * swc->rounds * swc->piece_count * sizeof(__uint128_t) * 256; //LUT
        memcpy(ds, swc->lut_256, k_lut);
        ds += k_lut;
        k_e = 8 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE
        break;
    default:
        k_lut = 4 * swc->rounds * swc->piece_count * sizeof(__uint128_t) * 256; //LUT
        k_e = 8 * swc->piece_count * sizeof(uint8_t) * 256;                     //SE EE
        break;
    }

    memcpy(ds, swc->SE, k_e);
    ds += k_e;
    memcpy(ds, swc->EE, k_e);
    ds += k_e;

    int k;
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
    int k_lut;
    int k_e;

    switch (swan_cfg_blocksizes[swc->cfg])
    {
    case 64:
        k_lut = swc->rounds * swc->piece_count * sizeof(uint32_t) * 256; //LUT
        swc->lut_64 = (uint32_t(*)[4][256])malloc(k_lut);
        memcpy(swc->lut_64, ptr, k_lut);
        ptr += k_lut;
        k_e = 2 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE
        break;
    case 128:
        k_lut = 2 * swc->rounds * swc->piece_count * sizeof(uint64_t) * 256; //LUT
        swc->lut_128 = (uint64_t(*)[4][2][256])malloc(k_lut);
        memcpy(swc->lut_128, ptr, k_lut);
        ptr += k_lut;
        k_e = 4 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE
        break;
    case 256:
        k_lut = 4 * swc->rounds * swc->piece_count * sizeof(__uint128_t) * 256; //LUT
        swc->lut_256 = (uint32_t(*)[4][4][256])malloc(k_lut);
        memcpy(swc->lut_256, ptr, k_lut);
        ptr += k_lut;
        k_e = 8 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE
        break;
    default:
        k_lut = 4 * swc->rounds * swc->piece_count * sizeof(__uint128_t) * 256; //LUT
        swc->lut_256 = (uint32_t(*)[4][4][256])malloc(k_lut);
        memcpy(swc->lut_256, ptr, k_lut);
        ptr += k_lut;
        k_e = 8 * swc->piece_count * sizeof(uint8_t) * 256; //SE EE

        break;
    }

    memcpy(swc->SE, ptr, k_e);
    ptr += k_e;
    memcpy(swc->EE, ptr, k_e);
    ptr += k_e;

    swc->B = (CombinedAffine *)malloc((swc->rounds) * sizeof(CombinedAffine));

    swc->D = (CombinedAffine *)malloc(swc->rounds * sizeof(CombinedAffine));

    swc->E = (CombinedAffine *)malloc(swc->rounds * sizeof(CombinedAffine));

    if (swc->weak_or_strong)
    {
        swc->F = (CombinedAffine *)malloc(swc->rounds * sizeof(CombinedAffine));
    }
    else
    {
        swc->F = NULL;
    }

    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;

    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(B_ptr++, swan_cfg_piecebit[swc->cfg], swc->piece_count);
    }
    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(D_ptr++, swan_cfg_piecebit[swc->cfg], swc->piece_count);
    }

    for (i = 0; i < (swc->rounds); i++)
    {

        combined_affine_init(E_ptr++, swan_cfg_piecebit[swc->cfg], swc->piece_count);
    }
    if (swc->weak_or_strong)
    {
        for (i = 0; i < (swc->rounds); i++)
        {

            combined_affine_init(F_ptr++, swan_cfg_piecebit[swc->cfg], swc->piece_count);
        }
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

    if (swc->weak_or_strong)
    {
        for (i = 0; i < swc->rounds; i++)
        {
            uint32_t aff_sz = *((uint32_t *)ptr);
            *((swc->F + i)->combined_affine) = ImportAffineFromStr(ptr);
            ptr += aff_sz;
        }
    }

    return 0;
}
int swan_whitebox_release(swan_whitebox_content *swc, int table_or_content)
{
    // table_or_content: 1 for table ,0 for content

    int i, j;
    CombinedAffine *B_ptr = swc->B;
    CombinedAffine *C_ptr = swc->C;
    CombinedAffine *D_ptr = swc->D;
    CombinedAffine *E_ptr = swc->E;
    CombinedAffine *F_ptr = swc->F;
    CombinedAffine *P_ptr = swc->P;

    for (i = 0; i < swc->rounds; i++)
    {
        combined_affine_free(B_ptr++);
        combined_affine_free(D_ptr++);
        combined_affine_free(E_ptr++);
    }
    free(swc->B);
    free(swc->D);
    free(swc->E);
    swc->B = NULL;
    swc->D = NULL;
    swc->E = NULL;
    if (!table_or_content)
    {

        for (i = 0; i < swc->rounds; i++)
        {
            combined_affine_free(C_ptr++);
        }

        for (i = 0; i < swc->rounds + 2; i++)
        {
            combined_affine_free(P_ptr++);
        }

        free(swc->C);
        free(swc->P);
        swc->C = NULL;
        swc->P = NULL;
        if (swc->lut_64 != NULL)
        {
            free(swc->lut_64);
            swc->lut_64 = NULL;
        }
        if (swc->lut_128 != NULL)
        {
            free(swc->lut_128);
            swc->lut_128 = NULL;
        }
        if (swc->lut_256 != NULL)
        {
            free(swc->lut_256);
            swc->lut_256 = NULL;
        }
    }

    if (swc->weak_or_strong)
    {

        for (i = 0; i < swc->rounds; i++)
        {
            combined_affine_free(F_ptr++);
        }
        free(swc->F);
        swc->F = NULL;
    }

    return 0;
}