#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "actors.h"

#define FILENAME    "output.bmp"
#ifndef NO_WRITE
static unsigned char BMP_Header[54] = {
    66, 77, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0,  0, 0, 40, 0,
    0,   0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 24, 0,  0, 0, 0,
    0,   0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0, 0,
    0,   0, 0, 0
};
#endif

int main ( int argc, char **argv )
{
    unsigned char *output_mem;
    unsigned char *shared_mem;
    // Load shared mem.
    FILE          *fd = fopen ( argv[1], "r" );
    if ( fd ) {
        fseek ( fd, 0, SEEK_END );
        long size = ftell ( fd );
        fseek ( fd, 0, SEEK_SET );
        shared_mem = malloc ( size );

        size_t read = 0, tread = 0;
        while ( ( read = fread ( &( shared_mem[tread] ), 1, 1024, fd ) ) > 0 ) {
            tread += read;
        }
        fclose ( fd );
    }
    /**
     * Main JPEG code
     */
    VldHeader header;
    init_header_vld ( &header, (unsigned int *) shared_mem, NULL );

    output_mem = malloc ( 4 * get_num_pixels ( &header ) + 54 );
    // Dirty hack, fix on platform
    header.fp.fb = (unsigned int *) output_mem;

    SubHeader2 sh2;
    while ( 1 ) {
        FValue     fv[10];
        SubHeader1 sh1;
        header_vld ( &header, &header, &( fv[0] ), &sh1, &sh2  );
        PBlock     pbout[10];
        for ( int i = 0; i < 10; i++ ) {
            FBlock fbout;
            iqzz ( &( fv[i] ), &fbout );
            idct ( &fbout, &( pbout[i] ) );
        }
        ColorBuffer cbout;
        cc ( &sh1, &( pbout[0] ), &cbout );
        raster ( &sh2, &cbout );

        if ( sh2.leftover == 0 ) {
            break;
        }
    }

    /**
     * Write BMP to output.bmp.
     */
    const int x_size = sh2.x_size;
    const int y_size = sh2.y_size;
    printf ( "Writing bmp: %d %d\n", x_size, y_size );
    int       j;
    {
        int  i;
        FILE *fpBMP;

        // Header and 3 bytes per pixel
        unsigned long ulBitmapSize = ( y_size * x_size * 3 ) + 54;

        /* Create bitmap file */
        fpBMP = fopen ( FILENAME, "w" );

        if ( fpBMP == 0 ) {
            my_fprintf ( stderr, "Error: Couldn't open file to write BMP file\n" );
            exit ( -1 );
        }

        /* Write header */
        /* All values are in big endian order (LSB first) */
        BMP_Header[2] = ( ulBitmapSize & 0x000000FF );
        BMP_Header[3] = ( ulBitmapSize & 0x0000FF00 ) >> 8;
        BMP_Header[4] = ( ulBitmapSize & 0x00FF0000 ) >> 16;
        BMP_Header[5] = ( ulBitmapSize & 0xFF000000 ) >> 24;

        BMP_Header[18] = x_size & 0x00ff;
        BMP_Header[19] = ( x_size & 0xff00 ) >> 8;

        BMP_Header[22] = y_size & 0x00ff;
        BMP_Header[23] = ( y_size & 0xff00 ) >> 8;

        unsigned char cc = 0;

        for ( cc = 0; cc < sizeof ( BMP_Header ); cc++ ) {
            putc ( BMP_Header[cc], fpBMP );
        }

        /* Write data */
        for ( i = y_size - 1; i >= 0; i-- ) {
            /* in bitmaps the bottom line of the image is at the
               beginning of the file */
            for ( j = 0; j < x_size; j++ ) {
                putc ( output_mem[4 * ( i * x_size + j ) + 0], fpBMP );
                putc ( output_mem[4 * ( i * x_size + j ) + 1], fpBMP );
                putc ( output_mem[4 * ( i * x_size + j ) + 2], fpBMP );
            }
            for ( j = 0; j < x_size % 4; j++ ) {
                putc ( 0, fpBMP );
            }
        }

        fclose ( fpBMP );
        //printf("DONE!\n");
        exit ( 0 );
    }
}
