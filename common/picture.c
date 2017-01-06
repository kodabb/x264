/*****************************************************************************
 * picture.c: picture-related functions
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

#include "osdep.h"
#include "x264.h"
#include "mem.h"
#include <string.h>

/****************************************************************************
 * x264_picture_init:
 ****************************************************************************/
void x264_picture_init( x264_picture_t *pic )
{
    memset( pic, 0, sizeof( x264_picture_t ) );
    pic->i_type = X264_TYPE_AUTO;
    pic->i_qpplus1 = X264_QP_AUTO;
    pic->i_pic_struct = PIC_STRUCT_AUTO;
}

/****************************************************************************
 * x264_picture_alloc:
 ****************************************************************************/
int x264_picture_alloc( x264_picture_t *pic, int i_csp, int i_width, int i_height )
{
    typedef struct
    {
        int planes;
        int width_fix8[3];
        int height_fix8[3];
    } x264_csp_tab_t;

    static const x264_csp_tab_t x264_csp_tab[] =
    {
        [X264_CSP_I420] = { 3, { 256*1, 256/2, 256/2 }, { 256*1, 256/2, 256/2 } },
        [X264_CSP_YV12] = { 3, { 256*1, 256/2, 256/2 }, { 256*1, 256/2, 256/2 } },
        [X264_CSP_NV12] = { 2, { 256*1, 256*1 },        { 256*1, 256/2 },       },
        [X264_CSP_NV21] = { 2, { 256*1, 256*1 },        { 256*1, 256/2 },       },
        [X264_CSP_I422] = { 3, { 256*1, 256/2, 256/2 }, { 256*1, 256*1, 256*1 } },
        [X264_CSP_YV16] = { 3, { 256*1, 256/2, 256/2 }, { 256*1, 256*1, 256*1 } },
        [X264_CSP_NV16] = { 2, { 256*1, 256*1 },        { 256*1, 256*1 },       },
        [X264_CSP_YUYV] = { 1, { 256*2 },               { 256*1 },              },
        [X264_CSP_UYVY] = { 1, { 256*2 },               { 256*1 },              },
        [X264_CSP_I444] = { 3, { 256*1, 256*1, 256*1 }, { 256*1, 256*1, 256*1 } },
        [X264_CSP_YV24] = { 3, { 256*1, 256*1, 256*1 }, { 256*1, 256*1, 256*1 } },
        [X264_CSP_BGR]  = { 1, { 256*3 },               { 256*1 },              },
        [X264_CSP_BGRA] = { 1, { 256*4 },               { 256*1 },              },
        [X264_CSP_RGB]  = { 1, { 256*3 },               { 256*1 },              },
    };

    int csp = i_csp & X264_CSP_MASK;
    if( csp <= X264_CSP_NONE || csp >= X264_CSP_MAX || csp == X264_CSP_V210 )
        return -1;
    x264_picture_init( pic );
    pic->img.i_csp = i_csp;
    pic->img.i_plane = x264_csp_tab[csp].planes;
    int depth_factor = i_csp & X264_CSP_HIGH_DEPTH ? 2 : 1;
    int plane_offset[3] = {0};
    int frame_size = 0;
    for( int i = 0; i < pic->img.i_plane; i++ )
    {
        int stride = (((int64_t)i_width * x264_csp_tab[csp].width_fix8[i]) >> 8) * depth_factor;
        int plane_size = (((int64_t)i_height * x264_csp_tab[csp].height_fix8[i]) >> 8) * stride;
        pic->img.i_stride[i] = stride;
        plane_offset[i] = frame_size;
        frame_size += plane_size;
    }
    pic->img.plane[0] = x264_malloc( frame_size );
    if( !pic->img.plane[0] )
        return -1;
    for( int i = 1; i < pic->img.i_plane; i++ )
        pic->img.plane[i] = pic->img.plane[0] + plane_offset[i];
    return 0;
}

/****************************************************************************
 * x264_picture_clean:
 ****************************************************************************/
void x264_picture_clean( x264_picture_t *pic )
{
    x264_free( pic->img.plane[0] );

    /* just to be safe */
    memset( pic, 0, sizeof( x264_picture_t ) );
}
