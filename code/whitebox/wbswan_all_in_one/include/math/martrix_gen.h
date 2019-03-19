
#include <math/affine_transform.h>
#include <math/matrix_utils.h>

/*
  swan switchLanes eg:
    b[0] = a[1] ^ a[2] ^ a[3]
    b[1] = a[0] ^ a[2] ^ a[3]
    b[2] = a[0] ^ a[1] ^ a[3]
    b[3] = a[0] ^ a[1] ^ a[2]

*/
MatGf2 make_switchLanes(int dim);

MatGf2 make_right_rotate_shift(int dim, int r1, int r2, int r3);

MatGf2 make_transposition(int dim);

MatGf2 make_transposition_back(int dim);