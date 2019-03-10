#ifndef SBOXANALYSIS_H_INCLUDED
#define SBOXANALYSIS_H_INCLUDED
#include <stdint.h>

/*
input an 4-bit sbox, return the difference distribution table of the sbox.
The row indice of the ddt are the output differences, while the column indice imply the input differences.
*/
int DifferenceDistributionTable(uint8_t *sbox, int (*DDT)[16]) {
    int i,j,k;

    for(i=0; i<=15; i++)
        for(j=0; j<=15; j++) {
            DDT[i][j]=0;
    }

    for(i=0;i<=15;i++)
        for(j=0;j<=15;j++) {
            k = sbox[i] ^ sbox[i^j];   //value k equals the output difference;
            *(*(DDT+k)+j) = *(*(DDT+k)+j) + 1;         //value j equals the input difference;
    }



    return 0;
}

//check if an S-box has the fixed point, such that sbox[i]=i;
int FixedPoint(uint8_t *sbox) {
    int i;

    for(i=0; i<=15; i++) {
        if(sbox[i] == i)
            return -1;
    }

    return 0;
}

//return the hammingweight of an input value;
uint8_t HammingWeight(int x) {
    uint8_t weight = 0;
    while(x != 0)
    {
        if(x & 0x01)
        {
            weight++;
        }

        x = x >> 1;
    }

    return weight;

}

int ioSum(int a, int b, int x, int y) {
    //a is input mask, b is output mask, S(a, b) = Sum{ (-1)^{<a, x> + <b, sbox[x]>
    return ((HammingWeight(a & x) % 2) ^ (HammingWeight(b & y) % 2));

}

int StrictAvalancheEffect(uint8_t *sbox) {
    int i,j,k;

    //int DDT[16][16];

    /*for(i=0; i<=15; i++)
        for(j=0; j<=15; j++) {
            DDT[i][j]=0;
    }*/

    for(i=0;i<=15;i++)
        for(j=0;j<=15;j++) {
            k = sbox[i] ^ sbox[i^j];   //value k equals the output difference;
            //*(*(DDT+k)+j) = *(*(DDT+k)+j) + 1;         //value j equals the input difference;

            //return false if the hammingweight of input difference and output difference are both 1;
            if(HammingWeight(k) == 1 && HammingWeight(j) == 1) {
                printf("%d, %d, ", i, j);
                return -1;

            }
    }



    return 0;

}

int LinearAprroximationTable(uint8_t *sbox, int (*LAT)[16]) {
    int i,j,k;

    for(i=0; i<=15; i++)
        for(j=0; j<=15; j++) {
            LAT[i][j]=0;
    }

    for(i=0; i<=15; i++)
        for(j=0; j<=15; j++)
            for(k=0; k<=15; k++) {
                // put linear approximation value into table;
                if(ioSum(i, j, k, sbox[k]) == 0) {
                    *(*(LAT+i)+j) = *(*(LAT+i)+j) + 1;
                }
                else {
                    *(*(LAT+i)+j) = *(*(LAT+i)+j) - 1;
                }

            }



    return 0;

}

int StrictLinearEffect(int (*LAT)[16]) {
    int i,j;

    for(i=0; i<=15; i++)
        for(j=0; j<=15; j++){
                // put linear approximation value into table;
                if(HammingWeight(i) == 1 && HammingWeight(j) == 1) {
                    if(abs(*(*(LAT+i)+j)) > 4) {
                        //printf("\n%d, %d,", i, j);
                        return -1;
                    }
                }

        }

    return 0;

}

int SboxBranchNumber(uint8_t *sbox) {
    int i,j,k;

    int minBranchNumber = 8;

    for(i=0; i<=15; i++)
        for(j=0; j<=15; j++) {
            if(i!=j) {
                k = HammingWeight(i ^ j) + HammingWeight(sbox[i] ^ sbox[j]);
                if(k < minBranchNumber)
                    minBranchNumber = k;
            }



    }

    return minBranchNumber;

}

int TestSboxAffineEquivalence(uint8_t *sbox1, uint8_t *sbox2) {
   uint8_t i,j;
   uint16_t A,B;


   for(A=0; A<=0xFFFF; A++)
   {
       for(B=0; B<=0xFFFF; B++)
       {
           for(i=0; i<=0x0F; i++)
           {
               for(j=0; j<=0x0F; j++)
               {

               }
           }
       }
   }


}



#endif // SBOXANALYSIS_H_INCLUDED
