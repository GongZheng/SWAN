/*
 * @Author: Weijie Li 
 * @Date: 2017-11-27 16:32:03 
 * @Last Modified by: Weijie Li
 * @Last Modified time: 2018-06-27 21:41:46
 */
#ifndef _MATH_AFFINE_TRANSFORM_H_
#define _MATH_AFFINE_TRANSFORM_H_

#include<math/matrix_gf2.h>

typedef struct AffineTransform {
    MatGf2 linear_map;
    MatGf2 vector_translation;
} AffineTransform;

int GenRandomAffineTransform(AffineTransform *at, AffineTransform *at_inv, int dim);

int GenIndAffineTransform(AffineTransform *at, AffineTransform *at_inv, int dim);

int ApplyAffineTransform(const AffineTransform at, const MatGf2 mat, MatGf2* dst);

uint32_t ApplyAffineToU32(const AffineTransform aff, uint32_t x);

uint64_t ApplyAffineToU64(const AffineTransform aff, uint64_t x);

uint32_t MATtoU32(const MatGf2 mat, uint32_t x);

uint64_t MATtoU64(const MatGf2 mat, uint64_t x);

uint16_t ApplyAffineToU16(const AffineTransform aff, uint16_t data);

uint8_t ApplyAffineToU8(const AffineTransform aff, uint8_t data);

__uint128_t ApplyAffineToU128(const AffineTransform aff, __uint128_t x);

    int AffineTransformFree(AffineTransform *aff);

/**
 * @brief export AffineTransform to str
 * 
 * @param aff 
 * @return void* 
 */
uint8_t * ExportAffineToStr(const AffineTransform* aff);

/**
 * @brief import AffineTransform from str
 * 
 * @param source 
 * @return AffineTransform 
 */
AffineTransform ImportAffineFromStr(const uint8_t *source);

#endif /* _MATH_AFFINE_TRANSFORM_H_ */