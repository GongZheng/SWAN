#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "SWAN256Diffusion.h"

int main()
{
    uint32_t a[4]={0x80000000,0x0,0x0,0x0};
    //uint32_t *han;

    uint8_t full_diffusion_round = 1;

    uint8_t R1 = 5;
    uint8_t R2 = 13;
    uint8_t R3 = 17;

    for(int i=0; i<=31; i++)
        for(int j=0; j<=31; j++)
        {
            if(swan256_theta_analysis(i,j, R1, R2, R3)>=2)
            {
                printf("R1=%d, R2=%d, R3=%d is not good for diffusion.", R1, R2, R3);
                return 0;
            }
        }

    full_diffusion_round = testSWAN256FullDiffusion(a, R1, R2, R3);

    printf("The minimum diffusion round is %d\n", full_diffusion_round);

    return 0;
}
