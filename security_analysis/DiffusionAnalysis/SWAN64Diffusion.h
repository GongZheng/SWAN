#ifndef SWAN64DIFFUSION_H_INCLUDED
#define SWAN64DIFFUSION_H_INCLUDED

#define ROUND64_K128 32

#define ROL8(x, n) ((x >> n) | (x << (8 - n)))

//ShiftLanes:The right rotate bits affine function;
void ShiftLanes(uint8_t a[4], uint8_t R1, uint8_t R2, uint8_t R3)
{
    a[1] = ROL8(a[1], R1);
    a[2] = ROL8(a[2], R2);
    a[3] = ROL8(a[3], R3);
}

//SwitchLanes:The almost MDS affine function based on XOR;
void SwitchLanes(uint8_t a[4])
{
    uint8_t b[4];
    b[0] = a[1] | a[2] | a[3];
    b[1] = a[0] | a[2] | a[3];
    b[2] = a[0] | a[1] | a[3];
    b[3] = a[0] | a[1] | a[2];
    memcpy(a, b, sizeof(b));
}

//To define the diffusion property of the sbox, if an input bit is 1, then this column becomes 1;
void Beta(uint8_t a[4])
{
    a[0] = a[0] | a[1] | a[2] | a[3];
    a[1] = a[0];
    a[2] = a[0];
    a[3] = a[0];
}

void SWAN64_Diffusion_Round(uint8_t a[4], uint8_t R1, uint8_t R2, uint8_t R3)
{
    ShiftLanes(a, R1, R2, R3);

    Beta(a);

    ShiftLanes(a, R1, R2, R3);

    SwitchLanes(a);

    //ShiftLanes(a, R1, R2, R3);

    //Beta(a);

    //ShiftLanes(a, R1, R2, R3);

    //SwitchLanes(a);
}

int isFullDiffusion(uint8_t a[4])
{
    uint8_t is_full_diffusion = a[0] & a[1] & a[2] & a[3];
    if (is_full_diffusion != 0xFF)
        return 0;

    return 1;
}

int testSWAN64FullDiffusion(uint8_t a[4],  uint8_t R1, uint8_t R2, uint8_t R3)
{

    //a[4]={8,0,0,0};
    //uint32_t *han;
    uint8_t round = 1;
    uint8_t is_full_diffusion = 0;

    while (!is_full_diffusion & (round != ROUND64_K128))
    {
        SWAN64_Diffusion_Round(a, R1, R2, R3);
        is_full_diffusion = isFullDiffusion(a);
        round++;
    }
    return round;
}

int swan64_theta_analysis(int a, int b, int x, int y, int z)
{
    int c=0;
    int d=0;
    int e=0;

    if((a+x)%8 == (b+y)%8||(a+x)%8 == (b+z)%8)
        c = 1;

    if((a+y)%8 == (b+x)%8||(a+y)%8 == (b+z)%8)
        d = 1;

    if((a+z)%8 == (b+x)%8||(a+z)%8 == (b+y)%8)
        e = 1;

    //if c+d+e>=2, it means x,y,z is not good for theta diffusion;
    return c+d+e;

}

#endif // SWAN64DIFFUSION_H_INCLUDED
