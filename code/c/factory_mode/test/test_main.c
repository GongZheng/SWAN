#include <SWAN.h>
#include <stdint.h>
#include <stdio.h>
#include "count_cycles.h"
#define TEST 10000

void dump(const uint8_t *li, int len)
{
    int line_ctrl = 16;
    int i;
    printf("\n");
    for (i = 0; i < len; i++)
    {
        printf("%02X", (*li++));
        if ((i + 1) % line_ctrl == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }
}

int main()
{
    uint32_t i; 
    uint64_t begin;
    uint64_t end;
    uint64_t ans = 0;
    unsigned char in[32] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    unsigned char out[32];
    unsigned char key[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    //swan64_k128
    printf("\n--------------------swan64_k128--------------------\n");
    dump(in, sizeof(in));
    begin = start_rdtsc();
    for(i = 0;i<TEST;i++)
    {
        SWAN64_K128_encrypt_rounds(in, key, ROUNDS64_K128, out);
    }
    end = end_rdtsc();
    dump(out, sizeof(out));
    ans = (end - begin);
    printf("\nSWAN64K128 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN64_K128_decrypt_rounds(out, key, ROUNDS64_K128, in);
    }
    end = end_rdtsc();
    dump(in, sizeof(in));
    ans = (end - begin);
    printf("\nSWAN64K128 decrypt cost %llu CPU cycles\n", (ans) / TEST);
    
    //swan64_k256
    printf("\n--------------------swan64_k256--------------------\n");
    dump(in, sizeof(in));
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN64_K256_encrypt_rounds(in, key, ROUNDS64_K256, out);
    }
    end = end_rdtsc();
    dump(out, sizeof(out));
    ans = (end - begin);
    printf("\nSWAN64K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN64_K256_decrypt_rounds(out, key, ROUNDS64_K256, in);
    }
    end = end_rdtsc();
    dump(in, sizeof(in));
    ans = (end - begin);
    printf("\nSWAN64K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //swan128_k128
    printf("\n--------------------swan128_k128--------------------\n");
    dump(in, sizeof(in));
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN128_K128_encrypt_rounds(in, key, ROUNDS128_128, out);
    }
    end = end_rdtsc();
    dump(out, sizeof(out));
    ans = (end - begin);
    printf("\nSWAN128K128 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN128_K128_decrypt_rounds(out, key, ROUNDS128_128, in);
    }
    end = end_rdtsc();
    dump(in, sizeof(in));
    ans = (end - begin);
    printf("\nSWAN128K128 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //128_256
    printf("\n--------------------swan128_k256--------------------\n");
    dump(in, sizeof(in));
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN128_K256_encrypt_rounds(in, key, ROUNDS128_256, out);
    }
    end = end_rdtsc();
    dump(out, sizeof(out));
    ans = (end - begin);
    printf("\nSWAN128K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN128_K256_decrypt_rounds(out, key, ROUNDS128_256, in);
    }
    end = end_rdtsc();
    dump(in, sizeof(in));
    ans = (end - begin);
    printf("\nSWAN128K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //swan256_k256
    printf("\n--------------------swan256_k256--------------------\n");
    dump(in, sizeof(in));
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN256_encrypt_rounds(in, key, ROUNDS256_256, out);
    }
    end = end_rdtsc();
    dump(out, sizeof(out));
    ans = (end - begin);
    printf("\nSWAN256K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        SWAN256_decrypt_rounds(out, key, ROUNDS256_256, in);
    }
    end = end_rdtsc();
    dump(in, sizeof(in));
    ans = (end - begin);
    printf("\nSWAN256k256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    return 0;
}
