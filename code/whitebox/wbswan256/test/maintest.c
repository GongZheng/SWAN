#include <stdint.h>
#include <stdio.h>
#include <math/affine_transform.h>
#include <math/matrix_utils.h>
#include <wbswan/wbswan.h>
#include <time.h>
#define TEST 1

void tobin(uint32_t x, int SIZE) //要和函数声明一致，所以后面也要填int x,int a[]
{
    int a[32] = {0};
    int n = 0, t, k;
    do
    {
        a[n] = x % 2;
        x = (unsigned int)x >> 1; //要考虑到参数int x有可能为负数所以填x=x/2是不行的。
        //如果x>=0的话填x=x/2没有问题，实际上我估计这里出题者的本意希望填x/2，但是如果当x为负数的时候
        //会出错的，要么填 x=(unsigned int)x/2也是可以的，不过 x=(unsigned int)x/2的运行效率远远低于x=(unsigned int)x>>1。这里牵涉的东西比较多，三言两语说不清
        //如果想了解原因，建议回去看谭浩强的强制类型转换、正数和负数的2进制表示、移位3个知识点
        n++;
    } while (x != 0);
    //do...while()这个功能就是把这个数的二进制的位存入这个数组中
    for (k = 0; k < n / 2; k++)
    {
        t = a[k];
        a[k] = a[n - k - 1]; //实现数组中2个数交换
        a[n - k - 1] = t;
        //for循环是为了交换顺序，比如x=11是的二进制码是1011这4个码一次存在a[3] a[2] a[1] a[0]中，而输出的时候是按a[0] a[1] a[2] a[3]这样输出的如果没有这个交换屏幕上看到的会是1101
    }
    for (k = SIZE; k > n; k--)
    {
        printf("%d-", a[k]);
    }
    for (k = 0; k < n; k++)
        printf("%d-", a[k]);
    int num = 0;
    for (int i = 0; i < 32; i++)
    {
        num += a[i];
    }
    // printf("%d\n", num);
    printf("\n");
}

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
    unsigned char in[32] = {0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
    uint8_t key[32] = {0};
    uint8_t outt[32];
    uint8_t out[32];
    time_t t_start, t_end;

    swan_whitebox_content swc_enc;
    swan_whitebox_content swc_dec;
    swan_whitebox_content swc_enct;
    swan_whitebox_content swc_dect;
    uint8_t *swct_ptr = NULL;

    t_start = time(NULL);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        swan_whitebox_256_init(key, 1, &swc_enc);
    }
    end = end_rdtsc();
    t_end = time(NULL);
    ans = (end - begin);
    swan_wb_export_to_bytes(&swc_enc, &swct_ptr);
    printf("Whitebox bytes size %d\n", *(uint32_t *)swct_ptr);
    swan_wb_import_from_bytes(swct_ptr, &swc_enct);
    printf("wbswan128 generate whitebox cost %llu CPU cycles and cost times %llu μs\n", (ans) / TEST, difftime(t_end, t_start) / TEST);

    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        swan_whitebox_encrypt(in, out, &swc_enct);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("wbswan128 encrypt cost %llu CPU cycles\n", (ans) / TEST);

    // swan_whitebox_release(&swc_enc);
    dump(out, sizeof(out));
    printf("\n");
    swan_whitebox_256_init(key, 0, &swc_dect);
    begin = start_rdtsc();
    for (i = 0; i < TEST; i++)
    {
        swan_whitebox_decrypt(out, outt, &swc_dect);
    }
    end = end_rdtsc();
    ans = (end - begin);
    printf("wbswan64 decrypt cost %llu CPU cycles\n", (ans) / TEST);

    // swan_whitebox_release(&swc_dec);
    dump(outt, sizeof(outt));
    printf("\n");


    // uint32_t x[4] = {0xf1,0xf1,0xf1,0xf1};

    // *((__uint128_t *)x) = ApplyMatToU128(make_transposition_128(128), *((__uint128_t *)x));
    // printf("------------------\n");
    // tobin(x[0],32);
    // printf("------------------\n");
    // tobin(x[1], 32);
    // printf("------------------\n");
    // tobin(x[2], 32);
    // printf("------------------\n");
    // tobin(x[3], 32);

    // *((__uint128_t *)x) = ApplyMatToU128(make_transposition_back_128(128), *((__uint128_t *)x));
    // printf("------------------\n");
    // tobin(x[0], 32);
    // printf("------------------\n");
    // tobin(x[1], 32);
    // printf("------------------\n");
    // tobin(x[2], 32);
    // printf("------------------\n");
    // tobin(x[3], 32);
}