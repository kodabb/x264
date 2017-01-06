/*****************************************************************************
 * mem.c: memory functions
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

#include "mem.h"
#include "log.h"
#include "x264.h"

/****************************************************************************
 * x264_malloc:
 ****************************************************************************/
void *x264_malloc( int i_size )
{
    uint8_t *align_buf = NULL;
#if HAVE_MALLOC_H
#if HAVE_THP
#define HUGE_PAGE_SIZE 2*1024*1024
#define HUGE_PAGE_THRESHOLD HUGE_PAGE_SIZE*7/8 /* FIXME: Is this optimal? */
    /* Attempt to allocate huge pages to reduce TLB misses. */
    if( i_size >= HUGE_PAGE_THRESHOLD )
    {
        align_buf = memalign( HUGE_PAGE_SIZE, i_size );
        if( align_buf )
        {
            /* Round up to the next huge page boundary if we are close enough. */
            size_t madv_size = (i_size + HUGE_PAGE_SIZE - HUGE_PAGE_THRESHOLD) & ~(HUGE_PAGE_SIZE-1);
            madvise( align_buf, madv_size, MADV_HUGEPAGE );
        }
    }
    else
#undef HUGE_PAGE_SIZE
#undef HUGE_PAGE_THRESHOLD
#endif
        align_buf = memalign( NATIVE_ALIGN, i_size );
#else
    uint8_t *buf = malloc( i_size + (NATIVE_ALIGN-1) + sizeof(void **) );
    if( buf )
    {
        align_buf = buf + (NATIVE_ALIGN-1) + sizeof(void **);
        align_buf -= (intptr_t) align_buf & (NATIVE_ALIGN-1);
        *( (void **) ( align_buf - sizeof(void **) ) ) = buf;
    }
#endif
    if( !align_buf )
        x264_log_internal( X264_LOG_ERROR, "malloc of size %d failed\n", i_size );
    return align_buf;
}

/****************************************************************************
 * x264_free:
 ****************************************************************************/
void x264_free( void *p )
{
    if( p )
    {
#if HAVE_MALLOC_H
        free( p );
#else
        free( *( ( ( void **) p ) - 1 ) );
#endif
    }
}

/****************************************************************************
 * x264_slurp_file:
 ****************************************************************************/
char *x264_slurp_file( const char *filename )
{
    int b_error = 0;
    int64_t i_size;
    char *buf;
    FILE *fh = x264_fopen( filename, "rb" );
    if( !fh )
        return NULL;

    b_error |= fseek( fh, 0, SEEK_END ) < 0;
    b_error |= ( i_size = ftell( fh ) ) <= 0;
    if( WORD_SIZE == 4 )
        b_error |= i_size > INT32_MAX;
    b_error |= fseek( fh, 0, SEEK_SET ) < 0;
    if( b_error )
        goto error;

    buf = x264_malloc( i_size+2 );
    if( !buf )
        goto error;

    b_error |= fread( buf, 1, i_size, fh ) != i_size;
    fclose( fh );
    if( b_error )
    {
        x264_free( buf );
        return NULL;
    }

    if( buf[i_size-1] != '\n' )
        buf[i_size++] = '\n';
    buf[i_size] = '\0';

    return buf;
error:
    fclose( fh );
    return NULL;
}
