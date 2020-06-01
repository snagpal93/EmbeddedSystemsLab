#ifdef PC
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "structures.h"
#include "usedma.h"
#include <hw_dma.h>
#include <global_memmap.h>
//#define USEDMA
typedef union RGBA
{
    unsigned int data;
    struct pixel
    {
        unsigned char b, g, r, a;
    } pixel;
}RGBA;

void raster ( const SubHeader2 * SH2, const ColorBuffer * CB )
{
    const int     goodrows    = SH2->goodrows;
    const int     goodcolumns = SH2->goodcolumns;
    const int     MCU_sx      = SH2->MCU_sx;
    const int     n_comp      = SH2->n_comp;
    int           i, j;
    const int     x_size = 1024;

    volatile RGBA *mem = (RGBA *) SH2->fp.fb;

    DMA dma = {0, mb1_dma0_BASEADDR };
    /*volatile unsigned int *dmamem   = (unsigned int *)mb1_dmamem0_BASEADDR;
    volatile unsigned int *fb   = (unsigned int *)shared_pt_REMOTEADDR;*/
    volatile struct pixel *dmamem   = (struct pixel *)mb1_dmamem0_BASEADDR;
//    volatile struct pixel *fb   = (struct pixel *)shared_pt_REMOTEADDR;

    for ( i = 0; i < goodrows; i++ ) {
        const int row_offset = ( i + SH2->MCU_row * SH2->MCU_sy ) * x_size;
        for ( j = 0; j < goodcolumns; j++ ) {
            int col_offset = j + ( SH2->MCU_column - 1 ) * SH2->MCU_sx;
//            dmamem = dmamem + 4*j;
            struct pixel *pix = dmamem;
            if ( n_comp == 3 ) {
                pix->r = CB->data[n_comp * i * MCU_sx + 2 + j * n_comp];
                pix->g = CB->data[n_comp * i * MCU_sx + 1 + j * n_comp];
                pix->b = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];

                #ifndef USEDMA
                mem[row_offset + col_offset].pixel.r = CB->data[n_comp * i * MCU_sx + 2 + j * n_comp];
                mem[row_offset + col_offset].pixel.g = CB->data[n_comp * i * MCU_sx + 1 + j * n_comp];
                mem[row_offset + col_offset].pixel.b = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
                #endif
            }
            else if ( n_comp == 1 ) {
                pix->r = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
                pix->g = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
                pix->b = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
 

                #ifndef USEDMA
                mem[row_offset + col_offset].pixel.r = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
                mem[row_offset + col_offset].pixel.g = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
                mem[row_offset + col_offset].pixel.b = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
                #endif
            }
//            mk_mon_debug_info(pix->r);
  //          mk_mon_debug_info(pix->g);
    //        mk_mon_debug_info(pix->b);
            #ifdef USEDMA
            pix->a = 0xFF ;
            hw_dma_send( &mem[row_offset + col_offset].pixel , dmamem, 1, &dma, NON_BLOCKING);
            while(hw_dma_status(&dma));

            #endif
        }
    }
}
