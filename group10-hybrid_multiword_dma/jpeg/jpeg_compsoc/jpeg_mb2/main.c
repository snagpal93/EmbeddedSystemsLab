#include <comik.h>
#include <global_memmap.h>
#include <5kk03-utils.h>
#include <hw_dma.h>
#include <fifo.h>
#include <fifo_types.h>

#include "structures.h"
#include "actors.h"
#include <application_structures.h>
#include <application_memmap.h>
#include <usedma.h>
#include <benchmarking.h>

volatile unsigned char *remote_buffer = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024 + 512*1024);
unsigned int *done_flag = (unsigned int *)(mb2_dmamem0_BASEADDR + DONE_FLAG_VALUE_OFFSET);
volatile unsigned int *stop = (unsigned int *)(mb2_cmem0_BASEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *done = (unsigned int *)(mb1_cmem0_pt_REMOTEADDR + DONE2_FLAG_OFFSET);
int main (void)
{
#ifdef BENCH
	TIME t1, t2, diff;
	t1 = hw_tifu_systimer_get();
#endif // BENCH

#ifdef BENCH_ACTORS
	TIME t1, t2, diff_iqzz = 0, diff_idct = 0, diff_cc = 0, diff_raster = 0;
#endif // BENCH_FIFO

	hw_declare_dmas(1);
	DMA *dma = hw_dma_add(1, (int*)mb2_dma0_BASEADDR);
	int dma_num = 0;
	*stop = 0;
	*done_flag = 1;
#ifdef USEDMA

	dma_num = 1;
#endif

#ifdef BENCH_FIFO
	TIME t1_f, t2_f, diff_f;
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
	FCB *fcb = fifo_add(
		MB2_FIFO_WC,
		MB2_FIFO_RC,
		MB2_FIFO_RWC,
		MB2_FIFO_RRC,
		MB2_FIFO_BUFFER,
		MB2_FIFO_RBUFFER,
		2,
		sizeof(MCU),
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
	start_stack_check();

#ifdef BENCH_FIFO
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO
	fifo_set_consumption_token_rate(fcb, 1);
#ifdef BENCH_FIFO
	t2_f = hw_tifu_systimer_get();
	diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

	FValue *fv;
	int break_flag = 0;
	while(1)
	{
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
		volatile MCU *sp = fifo_claim_data(fcb);
		fifo_pull(fcb);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

		fv = sp->fv;
		PBlock pbout[10];
		for (int i = 0; i < 10; i++)
		{
			FBlock fbout;
#ifdef BENCH_ACTORS
			t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
			iqzz(&(fv[i]), &fbout);
#ifdef BENCH_ACTORS
			t2 = hw_tifu_systimer_get();
			diff_iqzz += (t2 - t1);
#endif // BENCH_ACTORS

#ifdef BENCH_ACTORS
			t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
			idct(&fbout, &(pbout[i]));
#ifdef BENCH_ACTORS
			t2 = hw_tifu_systimer_get();
			diff_idct += (t2 - t1);
#endif // BENCH_ACTORS
		}

		volatile ColorBuffer *cbout = (ColorBuffer *)(mb2_dmamem0_BASEADDR + CBOUT_STRUCT_OFFSET);
		//ColorBuffer cbout;
#ifdef BENCH_ACTORS
	t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
		cc(&(sp->sh1), &(pbout[0]), cbout);
#ifdef BENCH_ACTORS
		t2 = hw_tifu_systimer_get();
		diff_cc += (t2 - t1);
#endif // BENCH_ACTORS

#ifdef BENCH_ACTORS
		t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
		raster(&(sp->sh2), cbout);
#ifdef BENCH_ACTORS
		t2 = hw_tifu_systimer_get();
		diff_raster += (t2 - t1);
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
	hw_dma_send(done, done_flag, 1, dma, NON_BLOCKING);

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
	mk_mon_debug_info(LO_64(diff_iqzz));
	mk_mon_debug_info(LO_64(diff_idct));
	mk_mon_debug_info(LO_64(diff_cc));
	mk_mon_debug_info(LO_64(diff_raster));
#endif // BENCH_FIFO

	// Signal the monitor we are done.
	mk_mon_debug_tile_finished();
	return 0;
}
