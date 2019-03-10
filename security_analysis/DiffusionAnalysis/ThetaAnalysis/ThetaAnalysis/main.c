#include <stdio.h>
#include <stdlib.h>

int swan64_theta_analysis(int x, int y, int a, int b, int c)
{
    int f1=0;
    int f2=0;
    int f3=0;

    if((x+a)%8 == (y+b)%8||(x+a)%8 == (y+c)%8)
        f1 = 1;

    if((x+b)%8 == (y+a)%8||(x+b)%8 == (y+c)%8)
        f2 = 1;

    if((x+c)%8 == (y+a)%8||(x+c)%8 == (y+b)%8)
        f3 = 1;

    //if c+d+e>=2, it means x,y,z is not good for theta diffusion;
    return f1+f2+f3;

}

int swan128_theta_analysis(int x, int y, int a, int b, int c)
{
    int f1=0;
    int f2=0;
    int f3=0;

    if((x+a)%16 == (y+b)%16||(x+a)%16 == (y+c)%16)
        f1 = 1;

    if((x+b)%16 == (y+a)%16||(x+b)%16 == (y+c)%16)
        f2 = 1;

    if((x+c)%16 == (y+a)%16||(x+c)%16 == (y+b)%16)
        f3 = 1;

    //if c+d+e>=2, it means x,y,z is not good for theta diffusion;
    return f1+f2+f3;
}

int swan256_theta_analysis(int x, int y, int a, int b, int c)
{
    int f1=0;
    int f2=0;
    int f3=0;

    if((x+a)%32 == (y+b)%32||(x+a)%32 == (y+c)%32)
        f1 = 1;

    if((x+b)%32 == (y+a)%32||(x+b)%32 == (y+c)%32)
        f2 = 1;

    if((x+c)%32 == (y+a)%32||(x+c)%32 == (y+b)%32)
        f3 = 1;

    //if c+d+e>=2, it means x,y,z is not good for theta diffusion;
    return f1+f2+f3;
}

int main()
{
    int a=1;
    int b=3;
    int c=7;

    int isBad=0;

    //for swan64 theta test
    for(int x = 0; x<=7; x++)
    {
        for(int y = 0; y<=7; y++){
            if(x==y)
                break;
            isBad = swan64_theta_analysis(x,y,a,b,c);
            if(isBad >=2)
            {
               printf("x=%d, y=%d, a=%d, b=%d, c=%d", x,y,a,b,c);
               return 0;
            }
        }
    }

    //for swan128 theta test
//    for(int x = 0; x<=15; x++)
//    {
//        for(int y = 0; y<=15; y++){
//            if(x==y)
//                break;
//            isBad = swan128_theta_analysis(x,y,a,b,c);
//            if(isBad >=2)
//            {
//               printf("x=%d, y=%d, a=%d, b=%d, c=%d", x,y,a,b,c);
//               return 0;
//            }
//        }
//    }

    //for swan256 theta test
//    for(int x = 0; x<=31; x++)
//    {
//        for(int y =0; y<=31; y++){
//            if(x==y)
//                break;
//            isBad = swan256_theta_analysis(x,y,a,b,c);
//            if(isBad >=2)
//            {
//               printf("x=%d, y=%d, a=%d, b=%d, c=%d", x,y,a,b,c);
//               return 0;
//            }
//        }
//    }




    return 0;
}
