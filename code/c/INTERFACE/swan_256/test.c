#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "swan256.h"
#define TEST 100000
void dump(const uint8_t *li, int len)
{
    int line_ctrl = 16;
    for (int i = 0; i < len; i++)
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
uint64_t start_rdtsc()
{
    uint32_t cycles_high, cycles_low;
    __asm__ volatile(
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)cycles_low) | (((uint64_t)cycles_high) << 32);
}

uint64_t end_rdtsc()
{
    uint32_t cycles_high, cycles_low;
    __asm__ volatile(
        "RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t"
        : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)cycles_low) | (((uint64_t)cycles_high) << 32);
}

int main()
{
    uint64_t begin;
    uint64_t end;
    uint64_t ans = 0;
    uint32_t subkey[128][4];
    int i;

    unsigned char in[32] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    unsigned char out[32];
    unsigned char outt[32];
    unsigned char key[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    

    printf("--------------------SWAN-256block-256keysize--------------------\n");
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        Key_Schedule(key, 256, 1, subkey);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("\ key_schedule for keysize 256 cost %llu CPU cycles\n", (ans) / TEST);

    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\n");
    begin = start_rdtsc();
    for (int i = 0; i < TEST; i++)
    {
        Crypt_Enc_Block(in,BLOACK256,out,BLOACK256,subkey,KEY256);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the ciphertext\n");
    dump(out, sizeof(out));
    printf("\nSWAN256K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    begin = start_rdtsc();
    for (int i = 0; i < TEST; i++)
    {
       Crypt_Dec_Block(out,BLOACK256,in,BLOACK256,subkey,KEY256);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\nSWAN256K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);
    printf("round test-swan256-256:\n");
    Crypt_Enc_Block_Round(in, 256, out, 256, subkey, 256, 2);
    dump(out, sizeof(out));
    printf("\n");
    return 0;
}
