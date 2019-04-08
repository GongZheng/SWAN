#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "Diffusion.h"
#include "SWAN128Diffusion.h"

int SWAN128Diffusion(int level)
{
    uint8_t full_diffusion_round = 1;
    int IS_NOT_GOOD = 0;
    uint8_t R1;
    uint8_t R2;
    uint8_t R3;

    for (R1 = 1; R1 < 16; R1++)
    {
        for (R2 = 2; R2 < 16; R2++)
        {
            for (R3 = 3; R3 < 16; R3++)
            {

                uint16_t a[4] = {0x8000, 0x0, 0x0, 0x0};
                IS_NOT_GOOD = 0;
                if (R3 > R2 && R2 > R1)
                {
                    for (int i = 0; i <= 16; i++)
                    {
                        for (int j = 0; j <= 16; j++)
                        {
                            if (swan128_theta_analysis(i, j, R1, R2, R3) >= 2)
                            {
                                // printf("R1=%d, R2=%d, R3=%d is not good for diffusion.\n", R1, R2, R3);
                                IS_NOT_GOOD = 1;
                            }
                        }
                        if (IS_NOT_GOOD)
                        {
                            break;
                        }
                    }

                    if (!IS_NOT_GOOD)
                    {
                        if (is_triple_coprime(R1, R2, R3, level) && is_double_coprime(R1, 16, level) && is_double_coprime(R2, 16, level) && is_double_coprime(R3, 16, level) && checkAverage16(R1, R2, R3, level))
                        {

                            full_diffusion_round = testSWAN128FullDiffusion(a, R1, R2, R3);

                            printf("R1=%d,R2=%d,R3=%d,full_diffusion_round=%d,R1+R2+R3=%d\n", R1, R2, R3, full_diffusion_round, R1 + R2 + R3);
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[], char *envp[])
{
    int level = 0;
    if (argc != 3)
    {
        printf("usage: -l <num>\n");
        printf("num:\n");
        printf("\t1  turn on the swan_theta_analysis\n");
        printf("\t2  a,b,c coprime on base on 1 level\n");
        printf("\t3  a,b,c are prime to bitsize(8,16,32) base on 2 level\n");
        printf("\t3  a,b,c are uniform distribute base on 3 level\n");
        return 0;
    }
    level = atoi(argv[2]);
    if (*(argv[1] + 1) == 'l')
    {
        SWAN128Diffusion(level);
    }
    else
    {

        printf("usage: -l <num>\n");
        printf("num:\n");
        printf("\t1  turn on the swan_theta_analysis\n");
        printf("\t2  a,b,c coprime on base on 1 level\n");
        printf("\t3  a,b,c are prime to bitsize(8,16,32) base on 2 level\n");
        printf("\t3  a,b,c are uniform distribute base on 3 level\n");
        return 0;
    }

    return 0;
}
