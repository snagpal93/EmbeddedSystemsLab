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
volatile unsigned char *remote_buffer = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024 + 512*1024);
unsigned int *stop_flag = (unsigned int *)(mb1_dmamem0_BASEADDR + STOP_FLAG_VALUE_OFFSET); //instead of a stop flag add a bool field to the MCU struct that will indicate if it is the last one
volatile unsigned int *stop2 = (unsigned int *)(mb2_cmem0_pt_REMOTEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *stop3 = (unsigned int *)(mb3_cmem0_pt_REMOTEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *stop4 = (unsigned int *)(mb4_cmem0_pt_REMOTEADDR + STOP_FLAG_OFFSET);
volatile unsigned int *done2 = (unsigned int *)(mb1_cmem0_BASEADDR + DONE2_FLAG_OFFSET);
volatile unsigned int *done3 = (unsigned int *)(mb1_cmem0_BASEADDR + DONE3_FLAG_OFFSET);
volatile unsigned int *done4 = (unsigned int *)(mb1_cmem0_BASEADDR + DONE4_FLAG_OFFSET);

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
	*done3 = 0;
	*done4 = 0;
	*stop_flag = 1;
#ifdef USEDMA
	dma_num = 1;
#endif

#ifdef BENCH_FIFO
	TIME t1_f, t2_f, diff_f;
	t1_f = hw_tifu_systimer_get();
#endif // BENCH_FIFO

	FCB *fcb1to2 = fifo_add(
		MB1_FIFO_WC,
		MB1_FIFO_RC,
		MB1_FIFO_RWC,
		MB1_FIFO_RRC,
		MB1_FIFO_BUFFER,
		MB1_FIFO_RBUFFER,
		2,
		sizeof(MCU),
		1, 1,
		dma_num,
		1,
		0,
		1);

	FCB *fcb1to3 = fifo_add(
		MB13_FIFO_WC,
		MB13_FIFO_RC,
		MB13_FIFO_RWC,
		MB13_FIFO_RRC,
		MB13_FIFO_BUFFER,
		MB13_FIFO_RBUFFER,
		2,
		sizeof(MCU),
		1, 1,
		dma_num,
		1,
		0,
		1);

	FCB *fcb1to4 = fifo_add(
		MB14_FIFO_WC,
		MB14_FIFO_RC,
		MB14_FIFO_RWC,
		MB14_FIFO_RRC,
		MB14_FIFO_BUFFER,
		MB14_FIFO_RBUFFER,
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
#endif // BENCH
	fifo_set_production_token_rate(fcb1to2, 1);
	fifo_set_production_token_rate(fcb1to3, 1);
	fifo_set_production_token_rate(fcb1to4, 1);
#ifdef BENCH_FIFO
	t2_f = hw_tifu_systimer_get();
	diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

	/*// Paint it our 'group' color so we can identify it.
	unsigned int *data = (unsigned int *)(mb1_dmamem0_BASEADDR + BUFFER_12_OFFSET);
	for (int i = 0; i < 1024; i++) {
		data[i] = 0xFF1122FF;
	}
	for (int i = 0; i < 768; i++) {
		hw_dma_send(&output_mem[i * 1024], data, 1024, dma, NON_BLOCKING);
	}
	while (hw_dma_status(dma));*/

	// Start decoding the JPEG.
	VldHeader header;
#ifdef BENCH_ACTORS
	TIME t1, t2, diff_init, diff_vld = 0;
	t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
	init_header_vld(&header, shared_mem, output_mem);
#ifdef BENCH_ACTORS
	t2 = hw_tifu_systimer_get();
	diff_init = t2 - t1;
#endif // BENCH_ACTORS

	volatile MCU *sp;
	int tmp_leftovers;
	FCB *fcb;
	for(int i = 0; 1; i++)
	{
		switch (i % 4)
		{
		case 0:
		case 1:
			fcb = fcb1to2;
			break;
		case 2:
			fcb = fcb1to3;
			break;
		case 3:
			fcb = fcb1to4;
			break;
		}

#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH
		while (!fifo_check_space(fcb));
		sp = (MCU *)fifo_claim_space(fcb);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

#ifdef BENCH_ACTORS
		t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
		header_vld(&header, &header, sp->fv, &(sp->sh1), &(sp->sh2));
#ifdef BENCH_ACTORS
		t2 = hw_tifu_systimer_get();
		diff_vld += (t2 - t1);
#endif // BENCH_FIFO
		
#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH
		fifo_push(fcb);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

		tmp_leftovers = sp->sh2.leftover;

#ifdef BENCH_FIFO
		t1_f = hw_tifu_systimer_get();
#endif // BENCH
		fifo_release_data(fcb);
#ifdef BENCH_FIFO
		t2_f = hw_tifu_systimer_get();
		diff_f += (t2_f - t1_f);
#endif // BENCH_FIFO

		if (tmp_leftovers == 0) {
			break;
		}
	}
	hw_dma_send(stop2, stop_flag, 1, dma, NON_BLOCKING);
	hw_dma_send(stop3, stop_flag, 1, dma, NON_BLOCKING);
	hw_dma_send(stop4, stop_flag, 1, dma, NON_BLOCKING);
	while (!(*done2) || !(*done3) || !(*done4));

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
#endif // BENCH_FIFO

	// Signal the monitor we are done.
	mk_mon_debug_tile_finished();
	return 0;
}
