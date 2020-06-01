#ifdef PC
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "structures.h"
#include <hw_dma.h>
#include <global_memmap.h>

#include <application_memmap.h>
#include <usedma.h>

typedef union RGBA
{
    unsigned int data;
    struct pixel
    {
        unsigned char b, g, r, a;
    } pixel;
}RGBA;

DMA dma = { 1, mb4_dma0_BASEADDR };

void raster ( const SubHeader2 * SH2, const ColorBuffer * CB )
{
    const int     goodrows    = SH2->goodrows;
    const int     goodcolumns = SH2->goodcolumns;
    const int     MCU_sx      = SH2->MCU_sx;
    const int     n_comp      = SH2->n_comp;
    int           i, j;
    const int     x_size = 1024;

    volatile RGBA *mem = (RGBA *) SH2->fp.fb;
    for ( i = 0; i < goodrows; i++ ) {
		const int row_offset = (i + SH2->MCU_row * SH2->MCU_sy) * x_size; //row*sy = # of whole lines we have to skip
#ifdef USEDMA
		int col_offset = (SH2->MCU_column - 1) * SH2->MCU_sx;
		/* add 1 for the alpha channel*/
		hw_dma_send((unsigned int *)&mem[row_offset + col_offset], (unsigned int*)&CB->data[(3 + 1) * i * MCU_sx], goodcolumns, &dma, NON_BLOCKING);
#endif // USEDMA
#ifndef USEDMA
		for (j = 0; j < goodcolumns; j++) {
			int col_offset = j + (SH2->MCU_column - 1) * SH2->MCU_sx;
			if (n_comp == 3) {
				mem[row_offset + col_offset].pixel.r = CB->data[n_comp * i * MCU_sx + 2 + j * n_comp];
				mem[row_offset + col_offset].pixel.g = CB->data[n_comp * i * MCU_sx + 1 + j * n_comp];
				mem[row_offset + col_offset].pixel.b = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
			}
			else if (n_comp == 1) {
				mem[row_offset + col_offset].pixel.r = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
				mem[row_offset + col_offset].pixel.g = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
				mem[row_offset + col_offset].pixel.b = CB->data[n_comp * i * MCU_sx + 0 + j * n_comp];
			}
		}
#endif // !USEDMA
	}
}