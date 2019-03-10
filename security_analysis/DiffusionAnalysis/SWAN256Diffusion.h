#ifndef SWAN256DIFFUSION_H_INCLUDED
#define SWAN256DIFFUSION_H_INCLUDED

#define ROUND256_K256 64

#define ROL32(x, n) ((x >> n) | (x << (32 - n)))

//ShiftLanes:The right rotate bits affine function;
void ShiftLanes(uint32_t a[4], uint32_t R1, uint32_t R2, uint32_t R3)
{
    a[1] = ROL32(a[1], R1);
    a[2] = ROL32(a[2], R2);
    a[3] = ROL32(a[3], R3);
}

//SwitchLanes:The almost MDS affine function based on XOR;
void SwitchLanes(uint32_t a[4])
{
    uint32_t b[4];
    b[0] = a[1] | a[2] | a[3];
    b[1] = a[0] | a[2] | a[3];
    b[2] = a[0] | a[1] | a[3];
    b[3] = a[0] | a[1] | a[2];
    memcpy(a, b, sizeof(b));
}

//To define the diffusion property of the sbox, if an input bit is 1, then this column becomes 1;
void Beta(uint32_t a[4])
{
    a[0] = a[0] | a[1] | a[2] | a[3];
    a[1] = a[0];
    a[2] = a[0];
    a[3] = a[0];
}

void SWAN256_Diffusion_Round(uint32_t a[4], uint32_t R1, uint32_t R2, uint32_t R3)
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

int isFullDiffusion(uint32_t a[4])
{
    uint32_t is_full_diffusion = a[0] & a[1] & a[2] & a[3];
    if (is_full_diffusion != 0xFFFFFFFF)
        return 0;

    return 1;
}

int testSWAN256FullDiffusion(uint32_t a[4],  uint8_t R1, uint8_t R2, uint8_t R3)
{

    //a[4]={8,0,0,0};
    //uint32_t *han;
    uint8_t round = 1;
    uint8_t is_full_diffusion = 0;

    while (!is_full_diffusion & (round != ROUND256_K256))
    {
        SWAN256_Diffusion_Round(a, R1, R2, R3);
        is_full_diffusion = isFullDiffusion(a);
        round++;
    }
    return round;
}

int swan256_theta_analysis(int a, int b, int x, int y, int z)
{
    int c=0;
    int d=0;
    int e=0;

    if((a+x)%32 == (b+y)%32||(a+x)%32 == (b+z)%32)
        c = 1;

    if((a+y)%32 == (b+x)%32||(a+y)%32 == (b+z)%32)
        d = 1;

    if((a+z)%32 == (b+x)%32||(a+z)%32 == (b+y)%32)
        e = 1;

    //if c+d+e>=2, it means x,y,z is not good for theta diffusion;
    return c+d+e;
}


#endif // SWAN256DIFFUSION_H_INCLUDED
