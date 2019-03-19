#include <math/affine_transform.h>
#include <math/matrix_utils.h>
MatGf2 make_switchLanes(int dim)
{
    MatGf2 ind = GenMatGf2(dim, dim);
    int block = dim / 4;
    int i;
    for (i = 0; i < block; i++)
    {
        MatGf2Set(ind, i, i + block, 1);
        MatGf2Set(ind, i, i + block * 2, 1);
        MatGf2Set(ind, i, i + block * 3, 1);
    }
    for (i = block; i < 2 * block; i++)
    {
        MatGf2Set(ind, i, i - block, 1);
        MatGf2Set(ind, i, i + block * 2, 1);
        MatGf2Set(ind, i, i + block, 1);
    }
    for (i = 2 * block; i < 3 * block; i++)
    {
        MatGf2Set(ind, i, i + block, 1);
        MatGf2Set(ind, i, i - block * 2, 1);
        MatGf2Set(ind, i, i - block, 1);
    }
    for (i = 3 * block; i < 4 * block; i++)
    {
        MatGf2Set(ind, i, i - block, 1);
        MatGf2Set(ind, i, i - block * 2, 1);
        MatGf2Set(ind, i, i - block * 3, 1);
    }
    // DumpMatGf2(ind);
    return ind;
}

MatGf2 make_right_rotate_shift(int dim, int r1, int r2, int r3)
{
    int block = dim / 4;
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    for (i = 0; i < block; i++)
    {
        MatGf2Set(ind, i, ((i + block - r3) % block + 0), 1);
    }
    for (i = block; i < block * 2; i++)
    {
        MatGf2Set(ind, i, (i + block - r2) % block + block, 1);
    }
    for (i = block * 2; i < block * 3; i++)
    {
        MatGf2Set(ind, i, (i + block - r1) % block + block * 2, 1);
    }
    for (i = block * 3; i < block * 4; i++)
    {
        MatGf2Set(ind, i, (i + block - 0) % block + block * 3, 1);
    }

    return ind;
}
MatGf2 make_transposition(int dim)
{
    uint8_t rot[32] = {0};
    int block = dim / 4;
    int c = block / 4;
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    int j;
    int row;

    for(i = 0;i<4;i++){
        rot[i] = i * block;
        for(j = 1;j<c;j++){
            rot[4*j+i] = rot[i] + j;

        }

    }

    for (i = 1; i <= 4; i++)
    {
        row = dim - block * i;

        for (j = 0; j < block; j++)
        {
            MatGf2Set(ind, row++, rot[j] + c * (i - 1), 1);
            // printf("%4d", rot[j] + c * (i - 1));
        }
        // printf("\n");
    }
    return ind;
}

MatGf2 make_transposition_back(int dim)
{

    // uint8_t rot[32] = {96, 100, 104, 108, 112, 116, 120, 124, 64, 68, 72, 76, 80, 84, 88, 92, 32, 36, 40, 44, 48, 52, 56, 60, 0, 4, 8, 12, 16, 20, 24, 28};
    uint8_t rot[32] = {0};
    int block = dim / 4;
    int c = block / 4;
    MatGf2 ind = GenMatGf2(dim, dim);
    int i;
    int j;
    int row = 0;

    for (i = 0; i < 4; i++)
    {
        rot[c * i] = dim - block * (i + 1);
        for (j = 1; j < c; j++)
        {
            rot[c * i + j] = rot[c * i] + 4 * j;
       }
    }

    for(i = 0;i<32;i++){

        // printf("%d ",rot[i]);
    }
    // printf("\n");



    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < block; j++)
        {
            MatGf2Set(ind, row++, rot[j] + i, 1);
            // printf("%4d", rot[j] + i);
        }
        // printf("-----------------------------\n");
    }
    return ind;
}
