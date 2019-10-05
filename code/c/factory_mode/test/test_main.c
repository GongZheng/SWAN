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
    dump(in, 8);
    begin = start_rdtsc();
    for(i = 0;i<TEST;i++)
    {
        Crypt_Enc_Block64_k128(in, 64, out, NULL, key, KEY128);
    }
    end = end_rdtsc();
    dump(out, 8);
    ans = (end - begin);
    printf("\nSWAN64K128 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Dec_Block64_k128(out, 64, in,  NULL, key, KEY128);
    }
    end = end_rdtsc();
    dump(in, 8);
    ans = (end - begin);
    printf("\nSWAN64K128 decrypt cost %llu CPU cycles\n", (ans) / TEST);
    
    //swan64_k256
    printf("\n--------------------swan64_k256--------------------\n");
    dump(in, 8);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block64_k256(in, 64, out, NULL, key, KEY256);
    }
    end = end_rdtsc();
    dump(out, 8);
    ans = (end - begin);
    printf("\nSWAN64K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Dec_Block64_k256(out, 64, in,  NULL, key, KEY256);
    }
    end = end_rdtsc();
    dump(in,8);
    ans = (end - begin);
    printf("\nSWAN64K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //swan128_k128
    printf("\n--------------------swan128_k128--------------------\n");
    dump(in, 16);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block128(in, 128, out, NULL, key, KEY128);
    }
    end = end_rdtsc();
    dump(out, 16);
    ans = (end - begin);
    printf("\nSWAN128K128 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Dec_Block128(out, 128, in, NULL, key, KEY128);
    }
    end = end_rdtsc();
    dump(in, 16);
    ans = (end - begin);
    printf("\nSWAN128K128 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //128_256
    printf("\n--------------------swan128_k256--------------------\n");
    dump(in, 16);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block128(in, 128, out, NULL, key, KEY256);
    }
    end = end_rdtsc();
    dump(out, 16);
    ans = (end - begin);
    printf("\nSWAN128K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Dec_Block128(out, 128, in, NULL, key, KEY256);
    }
    end = end_rdtsc();
    dump(in, 16);
    ans = (end - begin);
    printf("\nSWAN128K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    //swan256_k256
    printf("\n--------------------swan256_k256--------------------\n");
    dump(in, 32);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block256(in,256,out,NULL,key,KEY256);
    }
    end = end_rdtsc();
    dump(out, 32);
    ans = (end - begin);
    printf("\nSWAN256K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Crypt_Dec_Block256(out,256,in,NULL,(uint8_t*)key,KEY256);
    }
    end = end_rdtsc();
    dump(in, 32);
    ans = (end - begin);
    printf("\nSWAN256k256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    return 0;
}
