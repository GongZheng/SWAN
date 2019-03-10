#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "SboxAnalysis.h"


int main()
{
    //Piccolo's Sbox
    static uint8_t Piccolo_SBox[16] = {
	0x0E, 0x04, 0x0B, 0x02, 0x03, 0x08, 0x00, 0x09, 0x01, 0x0A, 0x07, 0x0F, 0x06, 0x0C, 0x05, 0x0D,
    };

    static uint8_t SWANSBox[16] = {
	0x01, 0x02, 0x0C, 0x05, 0x07, 0x08, 0x0A, 0x0F, 0x04, 0x0D, 0x0B, 0x0E, 0x09, 0x06, 0x00, 0x03,
    };

    int ddt[16][16];
    int lat[16][16];
    int i,j;

    /*for(i=0;i<=15;i++)
        for(j=0;j<=15;j++) {
            ddt[i][j]=0;
            lat[i][j]=0;
    }*/

    /*for(i = 0; i<=15; i++)
        printf("%d ,", HammingWeight(i));
    */


    DifferenceDistributionTable(SWANSBox, ddt);

    for(i=0;i<=15;i++) {
        for(j=0;j<=15;j++) {
            printf("%d ,", ddt[i][j]);
        }
        printf("\n");
    }

    printf("\n");

    LinearAprroximationTable(SWANSBox, lat);


    for(i=0; i<=15; i++) {
        for(j=0; j<=15; j++) {
            printf("%d ,", lat[i][j]);
        }
        printf("\n");
    }

    printf("\n%d, %d, %d", FixedPoint(SWANSBox), StrictAvalancheEffect(SWANSBox), StrictLinearEffect(lat));



    return 0;
}
