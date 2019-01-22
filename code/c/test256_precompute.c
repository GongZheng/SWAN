#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "SWAN256_PRECOMPUTE.h"
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
u_int64_t start_rdtsc()
{
    uint32_t cycles_high, cycles_low;
    asm volatile(
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return ((u_int64_t)cycles_low) | (((u_int64_t)cycles_high) << 32);
}

u_int64_t end_rdtsc()
{
    uint32_t cycles_high, cycles_low;
    asm volatile(
        "RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t"
        : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx", "%rdx");
    return ((u_int64_t)cycles_low) | (((u_int64_t)cycles_high) << 32);
}

int main()
{
    u_int64_t begin;
    u_int64_t end;
    u_int64_t ans = 0;

    unsigned char in[32] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    unsigned char out[32];
    unsigned char key[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    

    printf("--------------------SWAN-256block-256keysize--------------------\n");
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\n");
    begin = start_rdtsc();
    for (int i = 0; i < TEST; i++)
    {
        SWAN256_encrypt_rounds(in, key, ROUNDS256_256, out);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the ciphertext\n");
    dump(out, sizeof(out));
    printf("\nSWAN256K256 encrypt cost %llu CPU cycles\n", (ans) / TEST);
    uint32_t prekey256[2 * ROUNDS256_256 + 1][4];
    GenerateKey(key, ROUNDS256_256, prekey256, KEY256);
    begin = start_rdtsc();
    for (int i = 0; i < TEST; i++)
    {
        SWAN256_decrypt_rounds(out, prekey256, ROUNDS256_256, in);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("the plaintext\n");
    dump(in, sizeof(in));
    printf("\nSWAN256K256 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    return 0;
}
