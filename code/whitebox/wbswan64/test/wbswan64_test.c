#include "util/testutil.h"
#include <wbswan/wbswan.h>


int wbswan64_test(int weak_or_strong,enum swan_cipher_config_t cfg){
    uint64_t begin;
    uint64_t end;
    uint64_t ans = 0;
    int i;
    unsigned char in[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    uint8_t key[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t out[8];

    swan_whitebox_content swc_enc;
    swan_whitebox_content swc_dec;
    swan_whitebox_content swc_enct;

    uint8_t *enc_ptr = NULL;

    printf("*****************************************\n");
    begin = start_rdtsc();
    swan_whitebox_64_init(key, 1, weak_or_strong, &swc_enc, cfg);
    end = end_rdtsc();
    ans = (end - begin);
    swan_wb_export_to_bytes(&swc_enc, &enc_ptr);
    printf("%s_swan_b%d_k%d whitebox bytes size %d\n", weak_or_strong ? "strong" : "weak", swan_cfg_blocksizes[cfg], swan_cfg_keysizes[cfg], *(uint32_t *)enc_ptr);
    swan_wb_import_from_bytes(enc_ptr, &swc_enct);
    printf("%s_swan_b%d_k%d generate whitebox cost %llu CPU cycles\n", weak_or_strong ? "strong" : "weak", swan_cfg_blocksizes[cfg], swan_cfg_keysizes[cfg], (ans));

    begin = start_rdtsc();
    for (i = 0; i < 1000; i++)
    {
        swan_whitebox_64_encrypt(in, out, &swc_enct);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("%s_swan_b%d_k%d encrypt cost %llu CPU cycles\n", weak_or_strong ? "strong" : "weak", swan_cfg_blocksizes[cfg], swan_cfg_keysizes[cfg], (ans) / 1000);
    dump(out, sizeof(out));
    printf("\n");
    swan_whitebox_release(&swc_enc, 0);
    swan_whitebox_release(&swc_enct, 1);

    swan_whitebox_64_init(key, 0, weak_or_strong, &swc_dec, cfg);
    begin = start_rdtsc();
    for (i = 0; i < 1000; i++)
    {
        swan_whitebox_64_decrypt(out, in, &swc_dec);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("%s_swan_b%d_k%d decrypt cost %llu CPU cycles\n", weak_or_strong ? "strong" : "weak", swan_cfg_blocksizes[cfg], swan_cfg_keysizes[cfg], (ans) / 1000);
    dump(in, sizeof(in));
    printf("\n");
    swan_whitebox_release(&swc_dec, 0);

    return 0;
}