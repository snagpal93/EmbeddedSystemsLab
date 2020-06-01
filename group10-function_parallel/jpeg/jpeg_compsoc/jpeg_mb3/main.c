#include <comik.h>
#include <global_memmap.h>
#include <5kk03-utils.h>
#include <hw_dma.h>
#include <fifo.h>
#include <fifo_types.h>
#include <hw_tifu.h>

#include "structures.h"
#include "actors.h"
#include <application_memmap.h>
#include <application_structures.h>
#include <usedma.h>
#include <benchmarking.h>

volatile unsigned int *output_mem = (unsigned int*) shared_pt_REMOTEADDR;
volatile unsigned int *shared_mem = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024);

volatile unsigned int *stop2 = (unsigned int *)(mb2_cmem0_pt_REMOTEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *done2 = (unsigned int *)(mb3_cmem0_BASEADDR + DONE2_FLAG_OFFSET);
volatile unsigned int *done1 = (unsigned int *)(mb3_cmem0_BASEADDR + DONE1_FLAG_OFFSET);
volatile unsigned int *stop2_flag = (unsigned int *)(mb2_dmamem0_BASEADDR + STOP_FLAG_VALUE_OFFSET);

int check1 = 0;

int main (void)
{
#ifdef BENCH
	TIME t1, t2, diff;
	t1 = hw_tifu_systimer_get();
#endif // BENCH

	hw_declare_dmas(1);
	DMA *dma = hw_dma_add(1, (int*)mb1_dma0_BASEADDR);
	int dma_num = 0;

	*done2 = 0;
	*done1 = 0;
#ifdef USEDMA

	dma_num = 1;
#endif

#ifdef BENCH_FIFO
	TIME t1_f, t2_f, diff_f;
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO

	FCB *fcb3to2 = fifo_add(
		MB32_FIFO_WC_S,
		MB32_FIFO_RC_S,
		MB32_FIFO_RWC_S,
		MB32_FIFO_RRC_S,
		MB32_FIFO_BUFFER_S,
		MB32_FIFO_RBUFFER_S,
		4,
		sizeof(FF32),
		1, 1,
		dma_num,
		1,
		0,
		1);

/*	FCB *fcb1to3 = fifo_add(
		MB13_FIFO_WC,
		MB13_FIFO_RC,
		MB13_FIFO_RWC,
		MB13_FIFO_RRC,
		MB13_FIFO_BUFFER,
		MB13_FIFO_RBUFFER,
		1,
		sizeof(MCU),
		1, 1,
		0,
		1,
		0,
		1);*/

#ifdef BENCH_FIFO
	t2_f = hw_tifu_systimer_get();
	diff_f = t2_f - t1_f;
#endif // BENCH_FIFO

	// Sync with the monitor.
	mk_mon_sync();
	// Enable stack checking.
	start_stack_check();

	//mk_mon_debug_info(2);
#ifdef BENCH_FIFO
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
	fifo_set_production_token_rate(fcb3to2, 1);
	//fifo_set_production_token_rate(fcb1to3, 1);
#ifdef BENCH_FIFO
	t2_f = hw_tifu_systimer_get();
	diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

	// Paint it our 'group' color so we can identify it.
	for ( int i = 0 ; i < (768*1024);i++ ){
//		output_mem[i] = 0xFF1122FF;
	}

	// Start decoding the JPEG.
	VldHeader header;
#ifdef BENCH_ACTORS
	TIME t1_a, t2_a, diff_init, diff_vld = 0;
	t1_a = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
	init_header_vld(&header, shared_mem, output_mem);
#ifdef BENCH_ACTORS
	t2_a = hw_tifu_systimer_get();
	diff_init = t2_a - t1_a;
#endif // BENCH_ACTORS
	//mk_mon_debug_info(7);

	volatile FF32 *f2;

	int tmp_leftovers;
	FCB *fcb;
	for(int i = 0; 1; i++)
	{
		switch (i % 4)
		{
		case 0:
		case 1:
			fcb = fcb3to2;
			break;
		case 2:
			fcb = fcb3to2;
			break;
		case 3:
			fcb = fcb3to2;
		}
	   // mk_mon_debug_info(111);

#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
		while (!fifo_check_space(fcb3to2));
		//mk_mon_debug_info(*MB1_FIFO_WC);
		//mk_mon_debug_info(*MB1_FIFO_RC);
		f2 = (FF32*)fifo_claim_space(fcb3to2);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

#ifdef BENCH_ACTORS
		t1_a = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
		header_vld(&header, &header, f2->fv, &(f2->sh1), &(f2->sh2));
#ifdef BENCH_ACTORS
		t2_a = hw_tifu_systimer_get();
		diff_vld += (t2_a - t1_a);
#endif // BENCH_ACTORS

#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO	
		fifo_push(fcb3to2);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

		check1++;
		tmp_leftovers = f2->sh2.leftover;

#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
		fifo_release_data(fcb3to2);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO
		/*mk_mon_debug_info(*MB1_FIFO_WC);
		mk_mon_debug_info(*MB1_FIFO_RWC);
		mk_mon_debug_info(*MB1_FIFO_RC);
		mk_mon_debug_info(i);*/

		if (tmp_leftovers == 0) {
			break;
		}
	}
	//mk_mon_debug_info(111);
	//mk_mon_debug_info(check1);
	//mk_mon_debug_info(111);
	#ifndef USEDMA
	*stop2 = 1;
	#endif
	#ifdef USEDMA
	hw_dma_send(stop2, stop2_flag, 1, dma, NON_BLOCKING);
	#endif
	while (!(*done2) || !(*done1));

#ifdef BENCH_IDLE
	mk_mon_debug_info(4);
#endif // BENCH_IDLE

#ifdef BENCH
	t2 = hw_tifu_systimer_get();
	diff = t2 - t1;
	mk_mon_debug_info(1);
	mk_mon_debug_info(LO_64(diff));
#endif // BENCH

#ifdef BENCH_FIFO
	mk_mon_debug_info(2);
	mk_mon_debug_info(LO_64(diff_f));
#endif // BENCH_FIFO

#ifdef BENCH_ACTORS
	mk_mon_debug_info(3);
	mk_mon_debug_info(LO_64(diff_init));
	mk_mon_debug_info(LO_64(diff_vld));
#endif // BENCH_ACTORS

	// Signal the monitor we are done.
	mk_mon_debug_tile_finished();
	return 0;
}



/*#include <comik.h>
#include <global_memmap.h>
#include <5kk03-utils.h>
#include <hw_dma.h>
#include <fifo.h>
#include <fifo_types.h>
#include <hw_tifu.h>

#include "structures.h"
#include "actors.h"
#include <application_memmap.h>
#include <application_structures.h>
#include <usedma.h>
#include <benchmarking.h>

volatile unsigned char *remote_buffer = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024 + 512*1024);
volatile unsigned int *stop = (unsigned int *)(mb3_cmem0_BASEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *done = (unsigned int *)(mb1_cmem0_pt_REMOTEADDR + DONE3_FLAG_OFFSET);
volatile unsigned int *done_flag = (unsigned int *)(mb3_dmamem0_BASEADDR + DONE_FLAG_VALUE_OFFSET);

int check3 = 0 ;

int main (void)
{
#ifdef BENCH
	TIME t1, t2, diff;
	t1 = hw_tifu_systimer_get();
#endif // BENCH

#ifdef BENCH_ACTORS
	TIME t1_a, t2_a, diff_cc = 0, diff_raster = 0;
#endif // BENCH_FIFO

	hw_declare_dmas(1);
	DMA *dma = hw_dma_add(1, (int*)mb3_dma0_BASEADDR);
	int dma_num = 0;
	*stop = 0;
#ifdef USEDMA

	dma_num = 1;
#endif

#ifdef BENCH_FIFO
	TIME t1_f, t2_f, diff_f;
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO

	FCB *fcb = fifo_add(
		MB3_FIFO_WC,
		MB3_FIFO_RC,
		MB3_FIFO_RWC,
		MB3_FIFO_RRC,
		MB3_FIFO_BUFFER,
		MB3_FIFO_RBUFFER,
		1,
		sizeof(FF23),
		1, 1,
		dma_num,
		1,
		0,
		1);
#ifdef BENCH_FIFO
	t2_f = hw_tifu_systimer_get();
	diff_f = t2_f - t1_f;
#endif // BENCH_FIFO

	// Sync with the monitor.
	mk_mon_sync();
	// Enable stack checking.
//	start_stack_check();
#ifdef BENCH_FIFO
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
	fifo_set_consumption_token_rate(fcb, 1);
#ifdef BENCH_FIFO
	t2_f = hw_tifu_systimer_get();
	diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

	PBlock *pb;
	int break_flag = 0;
	for(int i=0;1;i++)
	{   
		int a;
#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
		while (!fifo_check_data(fcb))
		{
			if (*stop)
			{
				break_flag = 1;
				break;
			}
		}
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO
		if (break_flag)
		{
			break;
		}
#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
		volatile FF23 *f2 = fifo_claim_data(fcb);
		fifo_pull(fcb);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

		check3++;

		pb = f2->pb;
		ColorBuffer cbout;

#ifdef BENCH_ACTORS
		t1_a = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
		cc(&(f2->sh1), pb, &cbout);
#ifdef BENCH_ACTORS
		t2_a = hw_tifu_systimer_get();
		diff_cc += (t2_a - t1_a);
#endif // BENCH_ACTORS

#ifdef BENCH_ACTORS
		t1_a = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
		raster(&(f2->sh2), &cbout);
#ifdef BENCH_ACTORS
		t2_a = hw_tifu_systimer_get();
		diff_raster += (t2_a - t1_a);
#endif // BENCH_ACTORS

#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
		fifo_release_space(fcb);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO
	}
	//mk_mon_debug_info(333);
	//mk_mon_debug_info(check3);
	//mk_mon_debug_info(333);
	#ifndef USEDMA
	*done = 1;
	#endif
	#ifdef USEDMA
	hw_dma_send(done, done_flag, 1, dma, NON_BLOCKING);
	#endif

#ifdef BENCH_IDLE
	mk_mon_debug_info(4);
#endif // BENCH_IDLE

#ifdef BENCH
	t2 = hw_tifu_systimer_get();
	diff = t2 - t1;
	mk_mon_debug_info(1);
	mk_mon_debug_info(LO_64(diff));
#endif // BENCH

#ifdef BENCH_FIFO
	mk_mon_debug_info(2);
	mk_mon_debug_info(LO_64(diff_f));
#endif // BENCH_FIFO

#ifdef BENCH_ACTORS
	mk_mon_debug_info(3);
	mk_mon_debug_info(LO_64(diff_cc));
	mk_mon_debug_info(LO_64(diff_raster));
#endif // BENCH_ACTORS

	// Signal the monitor we are done.
	mk_mon_debug_tile_finished();
	return 0;
}*/
