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

volatile unsigned int *stop = (unsigned int *)(mb1_cmem0_BASEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *done = (unsigned int *)(mb3_cmem0_pt_REMOTEADDR + DONE3_FLAG_OFFSET);
volatile unsigned int *done_flag = (unsigned int *)(mb1_dmamem0_BASEADDR + DONE_FLAG_VALUE_OFFSET);

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
		MB21_FIFO_WC_R,
		MB21_FIFO_RC_R,
		MB21_FIFO_RWC_R,
		MB21_FIFO_RRC_R,
		MB21_FIFO_BUFFER_R,
		MB21_FIFO_RBUFFER_R,
		2,
		sizeof(FF21),
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
		volatile FF21 *f2 = fifo_claim_data(fcb);
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
}
