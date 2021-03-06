/*
 * Copyright (c) 2018, 2019.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  SWAN256.h
 *
 *  Description: The hearder file of SWAN256. The key schedule is on-the-fly computed.
 *  Created on: 2018-12-24
 *  Last modified: 2019-07-29
 *  Author: Zheng Gong, Weijie Li, Guohong Liao, Bing Sun, Siwei Sun, Tao Sun, Guojun Tang, Zhaoji Xu, Yingjie Zhang.
 */
#ifndef SWAN256_H_INCLUDED
#define SWAN256_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define ROUNDS256_K256 64

#define KEY256 256
#define BLOACK256 256

#define A 1
#define B 9
#define C 19

#define uint8_t unsigned __int8
#define uint32_t unsigned __int32
#define uint64_t unsigned __int64

/*
    Phi = 1.61803398874989484820458683436563811772
    DELTA = int(2**128 / Phi)
*/
#define DELTA 0x9e3779b97f4a7c15f39cc0605cedc834
/*
    DELTA_VER = DELTA
    for i in range(0,ROUNDS256):
        DELTA_VER = (DELTA_VER + DELTA) % 2**128
*/

#define DELTA_VER 0x2c15e81951e98192daccd877985fd534

#define DELTA1 0x9e3779b9
#define DELTA2 0x7f4a7c15
#define DELTA3 0xf39cc060
#define DELTA4 0x5cedc834

#define DELTA_VER1 0xb9f45679
#define DELTA_VER2 0x2488870f
#define DELTA_VER3 0xc1fcf08e
#define DELTA_VER4 0xd3d1e234

#define ROL32(x, n) ((x >> n) | (x << (32 - n)))
const uint32_t temp_constant[129][4]={
{0x5cedc834,0xf39cc060,0x7f4a7c15,0x9e3779b9},
{0xb9db9068,0xe73980c0,0xfe94f82b,0x3c6ef372},
{0x16c9589c,0xdad64121,0x7ddf7441,0xdaa66d2c},
{0x73b720d0,0xce730181,0xfd29f057,0x78dde6e5},
{0xd0a4e904,0xc20fc1e1,0x7c746c6d,0x1715609f},
{0x2d92b138,0xb5ac8242,0xfbbee883,0xb54cda58},
{0x8a80796c,0xa94942a2,0x7b096499,0x53845412},
{0xe76e41a0,0x9ce60302,0xfa53e0af,0xf1bbcdcb},
{0x445c09d4,0x9082c363,0x799e5cc5,0x8ff34785},
{0xa149d208,0x841f83c3,0xf8e8d8db,0x2e2ac13e},
{0xfe379a3c,0x77bc4423,0x783354f1,0xcc623af8},
{0x5b256270,0x6b590484,0xf77dd107,0x6a99b4b1},
{0xb8132aa4,0x5ef5c4e4,0x76c84d1d,0x8d12e6b},
{0x1500f2d8,0x52928545,0xf612c933,0xa708a824},
{0x71eebb0c,0x462f45a5,0x755d4549,0x454021de},
{0xcedc8340,0x39cc0605,0xf4a7c15f,0xe3779b97},
{0x2bca4b74,0x2d68c666,0x73f23d75,0x81af1551},
{0x88b813a8,0x210586c6,0xf33cb98b,0x1fe68f0a},
{0xe5a5dbdc,0x14a24726,0x728735a1,0xbe1e08c4},
{0x4293a410,0x83f0787,0xf1d1b1b7,0x5c55827d},
{0x9f816c44,0xfbdbc7e7,0x711c2dcc,0xfa8cfc37},
{0xfc6f3478,0xef788847,0xf066a9e2,0x98c475f0},
{0x595cfcac,0xe31548a8,0x6fb125f8,0x36fbefaa},
{0xb64ac4e0,0xd6b20908,0xeefba20e,0xd5336963},
{0x13388d14,0xca4ec969,0x6e461e24,0x736ae31d},
{0x70265548,0xbdeb89c9,0xed909a3a,0x11a25cd6},
{0xcd141d7c,0xb1884a29,0x6cdb1650,0xafd9d690},
{0x2a01e5b0,0xa5250a8a,0xec259266,0x4e115049},
{0x86efade4,0x98c1caea,0x6b700e7c,0xec48ca03},
{0xe3dd7618,0x8c5e8b4a,0xeaba8a92,0x8a8043bc},
{0x40cb3e4c,0x7ffb4bab,0x6a0506a8,0x28b7bd76},
{0x9db90680,0x73980c0b,0xe94f82be,0xc6ef372f},
{0xfaa6ceb4,0x6734cc6b,0x6899fed4,0x6526b0e9},
{0x579496e8,0x5ad18ccc,0xe7e47aea,0x35e2aa2},
{0xb4825f1c,0x4e6e4d2c,0x672ef700,0xa195a45c},
{0x11702750,0x420b0d8d,0xe6797316,0x3fcd1e15},
{0x6e5def84,0x35a7cded,0x65c3ef2c,0xde0497cf},
{0xcb4bb7b8,0x29448e4d,0xe50e6b42,0x7c3c1188},
{0x28397fec,0x1ce14eae,0x6458e758,0x1a738b42},
{0x85274820,0x107e0f0e,0xe3a3636e,0xb8ab04fb},
{0xe2151054,0x41acf6e,0x62eddf84,0x56e27eb5},
{0x3f02d888,0xf7b78fcf,0xe2385b99,0xf519f86e},
{0x9bf0a0bc,0xeb54502f,0x6182d7af,0x93517228},
{0xf8de68f0,0xdef1108f,0xe0cd53c5,0x3188ebe1},
{0x55cc3124,0xd28dd0f0,0x6017cfdb,0xcfc0659b},
{0xb2b9f958,0xc62a9150,0xdf624bf1,0x6df7df54},
{0xfa7c18c,0xb9c751b1,0x5eacc807,0xc2f590e},
{0x6c9589c0,0xad641211,0xddf7441d,0xaa66d2c7},
{0xc98351f4,0xa100d271,0x5d41c033,0x489e4c81},
{0x26711a28,0x949d92d2,0xdc8c3c49,0xe6d5c63a},
{0x835ee25c,0x883a5332,0x5bd6b85f,0x850d3ff4},
{0xe04caa90,0x7bd71392,0xdb213475,0x2344b9ad},
{0x3d3a72c4,0x6f73d3f3,0x5a6bb08b,0xc17c3367},
{0x9a283af8,0x63109453,0xd9b62ca1,0x5fb3ad20},
{0xf716032c,0x56ad54b3,0x5900a8b7,0xfdeb26da},
{0x5403cb60,0x4a4a1514,0xd84b24cd,0x9c22a093},
{0xb0f19394,0x3de6d574,0x5795a0e3,0x3a5a1a4d},
{0xddf5bc8,0x318395d5,0xd6e01cf9,0xd8919406},
{0x6acd23fc,0x25205635,0x562a990f,0x76c90dc0},
{0xc7baec30,0x18bd1695,0xd5751525,0x15008779},
{0x24a8b464,0xc59d6f6,0x54bf913b,0xb3380133},
{0x81967c98,0xfff69756,0xd40a0d50,0x516f7aec},
{0xde8444cc,0xf39357b6,0x53548966,0xefa6f4a6},
{0x3b720d00,0xe7301817,0xd29f057c,0x8dde6e5f},
{0x985fd534,0xdaccd877,0x51e98192,0x2c15e819},
{0xf54d9d68,0xce6998d7,0xd133fda8,0xca4d61d2},
{0x523b659c,0xc2065938,0x507e79be,0x6884db8c},
{0xaf292dd0,0xb5a31998,0xcfc8f5d4,0x6bc5545},
{0xc16f604,0xa93fd9f9,0x4f1371ea,0xa4f3ceff},
{0x6904be38,0x9cdc9a59,0xce5dee00,0x432b48b8},
{0xc5f2866c,0x90795ab9,0x4da86a16,0xe162c272},
{0x22e04ea0,0x84161b1a,0xccf2e62c,0x7f9a3c2b},
{0x7fce16d4,0x77b2db7a,0x4c3d6242,0x1dd1b5e5},
{0xdcbbdf08,0x6b4f9bda,0xcb87de58,0xbc092f9e},
{0x39a9a73c,0x5eec5c3b,0x4ad25a6e,0x5a40a958},
{0x96976f70,0x52891c9b,0xca1cd684,0xf8782311},
{0xf38537a4,0x4625dcfb,0x4967529a,0x96af9ccb},
{0x5072ffd8,0x39c29d5c,0xc8b1ceb0,0x34e71684},
{0xad60c80c,0x2d5f5dbc,0x47fc4ac6,0xd31e903e},
{0xa4e9040,0x20fc1e1d,0xc746c6dc,0x715609f7},
{0x673c5874,0x1498de7d,0x469142f2,0xf8d83b1},
{0xc42a20a8,0x8359edd,0xc5dbbf08,0xadc4fd6a},
{0x2117e8dc,0xfbd25f3e,0x45263b1d,0x4bfc7724},
{0x7e05b110,0xef6f1f9e,0xc470b733,0xea33f0dd},
{0xdaf37944,0xe30bdffe,0x43bb3349,0x886b6a97},
{0x37e14178,0xd6a8a05f,0xc305af5f,0x26a2e450},
{0x94cf09ac,0xca4560bf,0x42502b75,0xc4da5e0a},
{0xf1bcd1e0,0xbde2211f,0xc19aa78b,0x6311d7c3},
{0x4eaa9a14,0xb17ee180,0x40e523a1,0x149517d},
{0xab986248,0xa51ba1e0,0xc02f9fb7,0x9f80cb36},
{0x8862a7c,0x98b86241,0x3f7a1bcd,0x3db844f0},
{0x6573f2b0,0x8c5522a1,0xbec497e3,0xdbefbea9},
{0xc261bae4,0x7ff1e301,0x3e0f13f9,0x7a273863},
{0x1f4f8318,0x738ea362,0xbd59900f,0x185eb21c},
{0x7c3d4b4c,0x672b63c2,0x3ca40c25,0xb6962bd6},
{0xd92b1380,0x5ac82422,0xbbee883b,0x54cda58f},
{0x3618dbb4,0x4e64e483,0x3b390451,0xf3051f49},
{0x9306a3e8,0x4201a4e3,0xba838067,0x913c9902},
{0xeff46c1c,0x359e6543,0x39cdfc7d,0x2f7412bc},
{0x4ce23450,0x293b25a4,0xb9187893,0xcdab8c75},
{0xa9cffc84,0x1cd7e604,0x3862f4a9,0x6be3062f},
{0x6bdc4b8,0x1074a665,0xb7ad70bf,0xa1a7fe8},
{0x63ab8cec,0x41166c5,0x36f7ecd5,0xa851f9a2},
{0xc0995520,0xf7ae2725,0xb64268ea,0x4689735b},
{0x1d871d54,0xeb4ae786,0x358ce500,0xe4c0ed15},
{0x7a74e588,0xdee7a7e6,0xb4d76116,0x82f866ce},
{0xd762adbc,0xd2846846,0x3421dd2c,0x212fe088},
{0x345075f0,0xc62128a7,0xb36c5942,0xbf675a41},
{0x913e3e24,0xb9bde907,0x32b6d558,0x5d9ed3fb},
{0xee2c0658,0xad5aa967,0xb201516e,0xfbd64db4},
{0x4b19ce8c,0xa0f769c8,0x314bcd84,0x9a0dc76e},
{0xa80796c0,0x94942a28,0xb096499a,0x38454127},
{0x4f55ef4,0x8830ea89,0x2fe0c5b0,0xd67cbae1},
{0x61e32728,0x7bcdaae9,0xaf2b41c6,0x74b4349a},
{0xbed0ef5c,0x6f6a6b49,0x2e75bddc,0x12ebae54},
{0x1bbeb790,0x63072baa,0xadc039f2,0xb123280d},
{0x78ac7fc4,0x56a3ec0a,0x2d0ab608,0x4f5aa1c7},
{0xd59a47f8,0x4a40ac6a,0xac55321e,0xed921b80},
{0x3288102c,0x3ddd6ccb,0x2b9fae34,0x8bc9953a},
{0x8f75d860,0x317a2d2b,0xaaea2a4a,0x2a010ef3},
{0xec63a094,0x2516ed8b,0x2a34a660,0xc83888ad},
{0x495168c8,0x18b3adec,0xa97f2276,0x66700266},
{0xa63f30fc,0xc506e4c,0x28c99e8c,0x4a77c20},
{0x32cf930,0xffed2ead,0xa8141aa1,0xa2def5d9},
{0x601ac164,0xf389ef0d,0x275e96b7,0x41166f93},
{0xbd088998,0xe726af6d,0xa6a912cd,0xdf4de94c},
{0x19f651cc,0xdac36fce,0x25f38ee3,0x7d856306},
{0x76e41a00,0xce60302e,0xa53e0af9,0x1bbcdcbf},
{0xd3d1e234,0xc1fcf08e,0x2488870f,0xb9f45679}};

int Crypt_Enc_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Dec_Block(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen);

int Crypt_Enc_Block_Round(unsigned char *input, int in_len, unsigned char *output, int *out_len, unsigned char *key, int keylen, int CryptRound);

#endif // SWAN256_H_INCLUDED
