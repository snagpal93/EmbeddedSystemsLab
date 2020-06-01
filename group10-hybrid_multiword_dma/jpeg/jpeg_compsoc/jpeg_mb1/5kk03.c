#include "structures.h"
#include "stdarg.h"
#include "5kk03.h"
#include <hw_dma.h>
#include <global_memmap.h>

#include <application_structures.h>
#include <application_memmap.h>
#include <usedma.h>

int vld_count = 0;
int counter = 0;
int current_even = 0;
int next_even = 0;
int current_odd = 1;
int next_odd = 1;
int line;
unsigned int c;

#ifdef USEDMA
volatile unsigned char *dst_odd = (unsigned char *)(mb1_dmamem0_BASEADDR + FGETC_DST_1);
volatile unsigned char *dst_even = (unsigned char *)(mb1_dmamem0_BASEADDR + FGETC_DST_2);
unsigned int *src;
DMA dma = { 1, mb1_dma0_BASEADDR };
#endif // USEDMA

void my_fprintf ( FILE *stream, char *d, ... )
{
#ifdef PC
    va_list list;
    va_start ( list, d );
    vfprintf ( stream, d, list );
    va_end ( list );
#endif
}

void initial_dma_fetch(JPGFile *fp)
{
	current_even = 0;
	src = (unsigned int *)&fp->data[current_even * BURSTSIZE_LONG];  //data is int so use only 1024
	hw_dma_receive(dst_even, src, BURSTSIZE_LONG, &dma, BLOCKING);
	while (hw_dma_status(&dma));
	mk_mon_debug_info(12);
	next_even = current_even + 2;
	current_odd = 1;
	src = (unsigned int *)&fp->data[current_odd * BURSTSIZE_LONG];  //data is int so use only 1024
	hw_dma_receive(dst_odd, src, BURSTSIZE_LONG, &dma, NON_BLOCKING);
	//while (hw_dma_status(&dma));
	next_odd = current_odd + 2;
	counter = 0;
}
unsigned int FGETC ( JPGFile *fp )
{
#ifdef USEDMA
	/*every "line" has 1024 pixels and each one is 4 bytes long.*/
	line = fp->vld_count / (4 * BURSTSIZE_LONG);
	//mk_mon_debug_info(31);
	if (line == current_even)
	{
		//mk_mon_debug_info(32);
		if (current_even > current_odd)
		{
		//	mk_mon_debug_info(321);
			//mk_mon_debug_info(current_even);
			//mk_mon_debug_info(current_odd);
			counter = 0;
			src = (unsigned int *)&fp->data[next_odd * BURSTSIZE_LONG];  //data is int so use only 1024
			hw_dma_receive(dst_odd, src, BURSTSIZE_LONG, &dma, NON_BLOCKING);
			current_odd = next_odd;
			next_odd = current_odd + 2;
			//mk_mon_debug_info(322);
			//mk_mon_debug_info(current_odd);
		}
		//mk_mon_debug_info(33);
		c = ((unsigned int)dst_even[counter]) & 0x00ff;
		counter++;
	}
	else if (line == current_odd)
	{
		//mk_mon_debug_info(34);
		if (current_odd > current_even)
		{
			//mk_mon_debug_info(341);
			//mk_mon_debug_info(current_even);
//			mk_mon_debug_info(current_odd);
			counter = 0;
			src = (unsigned int *)&fp->data[next_even * BURSTSIZE_LONG];  //data is int so use only 1024
			hw_dma_receive(dst_even, src, BURSTSIZE_LONG, &dma, NON_BLOCKING);
			current_even = next_even;
			next_even = current_even + 2;
	//		mk_mon_debug_info(342);
		//	mk_mon_debug_info(current_even);
		}
		//mk_mon_debug_info(35);
		c = ((unsigned int)dst_odd[counter]) & 0x00ff;
		counter++;
	}
#endif // USEDMA
#ifndef USEDMA
	unsigned int c = ((fp->data[fp->vld_count / 4] << (8 * (3 - (fp->vld_count % 4)))) >> 24) & 0x00ff;
#endif // USEDMA    
    fp->vld_count++;
	//mk_mon_debug_info(c);
    return c;
}

int FSEEK ( JPGFile *fp, int offset, int start )
{
   // fp->vld_count += offset + ( start - start );    /* Just to use start... */
	/*if (fp->vld_count < current_line*(4 * BURSTSIZE_LONG) || fp->vld_count >= (current_line+1)*(4 * BURSTSIZE_LONG))
	{
		current_line = fp->vld_count / (4 * BURSTSIZE_LONG);
		src = (unsigned int *)&fp->data[current_line * BURSTSIZE_LONG];  //data is int so use only 1024
		hw_dma_receive(dst, src, BURSTSIZE_LONG, &dma, NON_BLOCKING);
		while (hw_dma_status(&dma));
	}
	counter = fp->vld_count % (4 * BURSTSIZE_LONG);*/
    return 0;
}

size_t FTELL ( JPGFile *fp )
{
    return fp->vld_count;
}
