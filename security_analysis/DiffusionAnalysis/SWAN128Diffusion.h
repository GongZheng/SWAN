#ifndef SWAN128DIFFUSION_H_INCLUDED
#define SWAN128DIFFUSION_H_INCLUDED

#define ROUND128_K128 48

#define ROL16(x, n) ((x >> n) | (x << (16 - n)))

//ShiftLanes:The right rotate bits affine function;
void ShiftLanes(uint16_t a[4], uint16_t R1, uint16_t R2, uint16_t R3)
{
    a[1] = ROL16(a[1], R1);
    a[2] = ROL16(a[2], R2);
    a[3] = ROL16(a[3], R3);
}

//SwitchLanes:The almost MDS affine function based on XOR;
void SwitchLanes(uint16_t a[4])
{
    uint16_t b[4];
    b[0] = a[1] | a[2] | a[3];
    b[1] = a[0] | a[2] | a[3];
    b[2] = a[0] | a[1] | a[3];
    b[3] = a[0] | a[1] | a[2];
    memcpy(a, b, sizeof(b));
}

//To define the diffusion property of the sbox, if an input bit is 1, then this column becomes 1;
void Beta(uint16_t a[4])
{
    a[0] = a[0] | a[1] | a[2] | a[3];
    a[1] = a[0];
    a[2] = a[0];
    a[3] = a[0];
}

void SWAN64_Diffusion_Round(uint16_t a[4], uint16_t R1, uint16_t R2, uint16_t R3)
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

int isFullDiffusion(uint16_t a[4])
{
    uint16_t is_full_diffusion = a[0] & a[1] & a[2] & a[3];
    if (is_full_diffusion != 0xFFFF)
        return 0;

    return 1;
}

int testSWAN128FullDiffusion(uint16_t a[4],  uint8_t R1, uint8_t R2, uint8_t R3)
{

    //a[4]={8,0,0,0};
    //uint32_t *han;
    uint8_t round = 1;
    uint8_t is_full_diffusion = 0;

    while (!is_full_diffusion & (round != ROUND128_K128))
    {
        SWAN128_Diffusion_Round(a, R1, R2, R3);
        is_full_diffusion = isFullDiffusion(a);
        round++;
    }
    return round;
}

int swan128_theta_analysis(int a, int b, int x, int y, int z)
{
    int c=0;
    int d=0;
    int e=0;

    if((a+x)%16 == (b+y)%16||(a+x)%16 == (b+z)%16)
        c = 1;

    if((a+y)%16 == (b+x)%16||(a+y)%16 == (b+z)%16)
        d = 1;

    if((a+z)%16 == (b+x)%16||(a+z)%16 == (b+y)%16)
        e = 1;

    //if c+d+e>=2, it means x,y,z is not good for theta diffusion;
    return c+d+e;
}

#endif // SWAN128DIFFUSION_H_INCLUDED
