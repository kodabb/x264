/*****************************************************************************
 * mathematics.c: mathematics functions
 *****************************************************************************
 * Copyright (C) 2003-2017 x264 project
 *
 * Authors: Loren Merritt <lorenm@u.washington.edu>
 *          Laurent Aimar <fenrir@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#include "mathematics.h"

const uint8_t x264_exp2_lut[64] =
{
      0,   3,   6,   8,  11,  14,  17,  20,  23,  26,  29,  32,  36,  39,  42,  45,
     48,  52,  55,  58,  62,  65,  69,  72,  76,  80,  83,  87,  91,  94,  98, 102,
    106, 110, 114, 118, 122, 126, 130, 135, 139, 143, 147, 152, 156, 161, 165, 170,
    175, 179, 184, 189, 194, 198, 203, 208, 214, 219, 224, 229, 234, 240, 245, 250
};

const float x264_log2_lut[128] =
{
    0.00000, 0.01123, 0.02237, 0.03342, 0.04439, 0.05528, 0.06609, 0.07682,
    0.08746, 0.09803, 0.10852, 0.11894, 0.12928, 0.13955, 0.14975, 0.15987,
    0.16993, 0.17991, 0.18982, 0.19967, 0.20945, 0.21917, 0.22882, 0.23840,
    0.24793, 0.25739, 0.26679, 0.27612, 0.28540, 0.29462, 0.30378, 0.31288,
    0.32193, 0.33092, 0.33985, 0.34873, 0.35755, 0.36632, 0.37504, 0.38370,
    0.39232, 0.40088, 0.40939, 0.41785, 0.42626, 0.43463, 0.44294, 0.45121,
    0.45943, 0.46761, 0.47573, 0.48382, 0.49185, 0.49985, 0.50779, 0.51570,
    0.52356, 0.53138, 0.53916, 0.54689, 0.55459, 0.56224, 0.56986, 0.57743,
    0.58496, 0.59246, 0.59991, 0.60733, 0.61471, 0.62205, 0.62936, 0.63662,
    0.64386, 0.65105, 0.65821, 0.66534, 0.67243, 0.67948, 0.68650, 0.69349,
    0.70044, 0.70736, 0.71425, 0.72110, 0.72792, 0.73471, 0.74147, 0.74819,
    0.75489, 0.76155, 0.76818, 0.77479, 0.78136, 0.78790, 0.79442, 0.80090,
    0.80735, 0.81378, 0.82018, 0.82655, 0.83289, 0.83920, 0.84549, 0.85175,
    0.85798, 0.86419, 0.87036, 0.87652, 0.88264, 0.88874, 0.89482, 0.90087,
    0.90689, 0.91289, 0.91886, 0.92481, 0.93074, 0.93664, 0.94251, 0.94837,
    0.95420, 0.96000, 0.96578, 0.97154, 0.97728, 0.98299, 0.98868, 0.99435,
};

/* Avoid an int/float conversion. */
const float x264_log2_lz_lut[32] =
{
    31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
};

/****************************************************************************
 * x264_reduce_fraction:
 ****************************************************************************/
#define REDUCE_FRACTION( name, type )\
void name( type *n, type *d )\
{                   \
    type a = *n;    \
    type b = *d;    \
    type c;         \
    if( !a || !b )  \
        return;     \
    c = a % b;      \
    while( c )      \
    {               \
        a = b;      \
        b = c;      \
        c = a % b;  \
    }               \
    *n /= b;        \
    *d /= b;        \
}

REDUCE_FRACTION( x264_reduce_fraction  , uint32_t )
REDUCE_FRACTION( x264_reduce_fraction64, uint64_t )
