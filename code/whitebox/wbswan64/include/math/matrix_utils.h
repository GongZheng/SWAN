/*
 * @Author: Weijie Li 
 * @Date: 2018-06-21 22:22:23
 * @Last Modified by: Weijie Li
 * @Last Modified time: 2018-06-21 22:22:26
 */

#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include "matrix_gf2.h"
#include "affine_transform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct combined_affine {
    unsigned int size;          // size of sub matrix, n * n
    unsigned int number;    // number of sub matrix
    AffineTransform * sub_affine;
    AffineTransform * sub_affine_inv;
    AffineTransform *combined_affine;
    AffineTransform *combined_affine_inv;
} CombinedAffine;


// typedef mzd_t *MatGf2;

int combined_affine_init(CombinedAffine *cm, int sub_matrix_size, int sub_matrix_number);
int ind_combined_affine_init(CombinedAffine *cm, int sub_matrix_size, int sub_matrix_number);

int combined_affine_free(CombinedAffine *cm);


#ifdef __cplusplus
}
#endif

#endif //_MATH_UTILS_H_