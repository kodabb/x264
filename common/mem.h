/*****************************************************************************
 * mem.h: memory functions
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

#ifndef COMMON_MEM_H
#define COMMON_MEM_H

#include "osdep.h"

#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#if HAVE_THP
#include <sys/mman.h>
#endif

#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))

/* x264_malloc : will do or emulate a memalign
 * you have to use x264_free for buffers allocated with x264_malloc */
void *x264_malloc( int );
void  x264_free( void * );

/* x264_slurp_file: malloc space for the whole file and read it */
char *x264_slurp_file( const char *filename );

#endif
