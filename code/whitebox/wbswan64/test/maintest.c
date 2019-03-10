#include <stdint.h>
#include <stdio.h>
#include <math/affine_transform.h>
#include <math/matrix_utils.h>
#include <wbswan/wbswan.h>
#define TEST 1


void dump(const uint8_t *li, int len)
{
    int line_ctrl = 16;
    int i;
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
    int i;
    //unsigned char in[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    //uint8_t key[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char in[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t key[16] = {0};
    uint8_t out[8];
   
    swan_whitebox_content swc_enc;
    swan_whitebox_content swc_dec;
    swan_whitebox_content swc_enct;
    swan_whitebox_content swc_dect;
    uint8_t *swct_ptr = NULL;

    begin = start_rdtsc();

    for (i = 0; i < TEST; i++)
    {
        swan_whitebox_64_init(key, 1, &swc_enc);
    }

    end = end_rdtsc();
    ans = (end - begin);

    swan_wb_export_to_bytes(&swc_enc,&swct_ptr);
    printf("Whitebox bytes size %d\n", *(uint32_t *)swct_ptr);   
    swan_wb_import_from_bytes(swct_ptr,&swc_enct);
    printf("wbswan64 generate whitebox cost %llu CPU cycles\n", (ans) / TEST);

    begin = start_rdtsc();

    for(i = 0;i<TEST;i++)
    {
        swan_whitebox_encrypt(in, out, &swc_enct);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("wbswan64 encrypt cost %llu CPU cycles\n", (ans) / TEST);

    swan_whitebox_release(&swc_enc);
    dump(out, sizeof(out));
    printf("\n");
    swan_whitebox_64_init(key, 0, &swc_dec);
    begin = start_rdtsc();

    for (i = 0; i < TEST; i++)
    {
        swan_whitebox_decrypt(out, in, &swc_dec);
    }

    end = end_rdtsc();
    ans = (end - begin);
    printf("wbswan64 decrypt cost %llu CPU cycles\n", (ans) / TEST);
    swan_whitebox_release(&swc_dec);
    dump(in, sizeof(in));
    printf("\n");
}