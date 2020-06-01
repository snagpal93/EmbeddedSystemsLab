#include "structures.h"
#include "stdarg.h"
#include "5kk03.h"
#include <hw_dma.h>
#include <global_memmap.h>
#include "usedma.h"
//#define USEDMA
#define BURSTSIZ 1
int vld_count = 0;
int counter = 0;
int seek = 0;
volatile unsigned int *dst ;
volatile unsigned int *src ;
void my_fprintf ( FILE *stream, char *d, ... )
{
#ifdef PC
    va_list list;
    va_start ( list, d );
    vfprintf ( stream, d, list );
    va_end ( list );
#endif
}

unsigned int FGETC ( JPGFile *fp )
{
    DMA dma = {0, mb4_dma0_BASEADDR };
    #ifdef USEDMA
    if (counter >= BURSTSIZ*4 || fp->vld_count ==0 || seek == 1 ) {
        dst  = (unsigned int *)mb4_dmamem0_BASEADDR + 1024 ;
        src   = (unsigned int *)&fp->data[fp->vld_count/4];
        hw_dma_receive(dst, src, BURSTSIZ, &dma, NON_BLOCKING);
        while(hw_dma_status(&dma));
        counter =0;
        seek = 0;
    }
    unsigned int c = ( ( dst[counter / 4] << ( 8 * ( 3 - ( counter % 4 ) ) ) ) >> 24 ) & 0x00ff;
    counter++;
    #endif
    #ifndef USEDMA 
    unsigned int c = ( ( fp->data[fp->vld_count / 4] << ( 8 * ( 3 - ( fp->vld_count % 4 ) ) ) ) >> 24 ) & 0x00ff;
    #endif
//    unsigned int d = ( ( fp->data[fp->vld_count / 4] << ( 8 * ( 3 - ( fp->vld_count % 4 ) ) ) ) >> 24 ) & 0x00ff;
    fp->vld_count++;
    return c;
}

int FSEEK ( JPGFile *fp, int offset, int start )
{
    fp->vld_count += offset + ( start - start );    /* Just to use start... */
    seek =1;
//    counter += offset + ( start - start );    /* Just to use start... */
    
    return 0;
}

size_t FTELL ( JPGFile *fp )
{
    return fp->vld_count;
}
