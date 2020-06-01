#include <comik.h>
#include <global_memmap.h>
#include <5kk03-utils.h>
#include <hw_tifu.h>

#include "structures.h"
#include "actors.h"
#include <application_memmap.h>

volatile unsigned int *output_mem = (unsigned int*) shared_pt_REMOTEADDR;
volatile unsigned int *shared_mem = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024);
volatile unsigned char *remote_buffer = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024 + 512*1024);

int main (void)
{
	volatile unsigned int *header_ready = (unsigned int *)(mb2_cmem0_pt_REMOTEADDR + HEADER_READY_FLAG_OFFSET);
	volatile VldHeader *header = (VldHeader *)(remote_buffer + VLD_HEADER_OFFSET);
	// Sync with the monitor.
	mk_mon_sync();
	// Enable stack checking.
	start_stack_check();

	// Paint it our 'group' color so we can identify it.
	for ( int i = 0 ; i < (768*1024);i++ ){
		output_mem[i] = 0xFF1122FF;
	}

	TIME t1, t2, diff;
	// Start decoding the JPEG.
	t1 = hw_tifu_systimer_get();
	init_header_vld(header, shared_mem, output_mem);
	t2 = hw_tifu_systimer_get();
	diff = t2-t1;
	mk_mon_debug_info(diff);
	*header_ready = 1;

	if (header->mcu_rst > 0)
	{
		header->mcu_rst = header->mcu_rst/ 2 + 1;
		header->leftover = 0;
	}
	else
	{
		header->leftover /= 2;
	}

	// From this point on pixel data begins.
	mk_mon_debug_info((header->fp).vld_count);

    SubHeader2 sh2;
    while(1)
    {
        FValue fv[10];
        SubHeader1 sh1;
		t1 = hw_tifu_systimer_get();
		header_vld(header, header, &(fv[0]), &sh1, &sh2);
		t2 = hw_tifu_systimer_get();
		diff = t2 - t1;
		mk_mon_debug_info(diff);
        PBlock pbout[10];
        for ( int i = 0; i < 10 ; i++ )
        {
            FBlock fbout;
			t1 = hw_tifu_systimer_get();
			iqzz(&(fv[i]), &fbout);
			t2 = hw_tifu_systimer_get();
			diff = t2 - t1;
			mk_mon_debug_info(diff);
			t1 = hw_tifu_systimer_get();
			idct(&fbout, &(pbout[i]));
			t2 = hw_tifu_systimer_get();
			diff = t2 - t1;
			mk_mon_debug_info(diff);
        }
        ColorBuffer cbout;
		t1 = hw_tifu_systimer_get();
		cc(&sh1, &(pbout[0]), &cbout);
		t2 = hw_tifu_systimer_get();
		diff = t2 - t1;
		mk_mon_debug_info(diff);
		t1 = hw_tifu_systimer_get();
		raster(&sh2, &cbout);
		t2 = hw_tifu_systimer_get();
		diff = t2 - t1;
		mk_mon_debug_info(diff);

        if ( sh2.leftover == 0 ){
            break;
        }
    }

	// Signal the monitor we are done.
	mk_mon_debug_tile_finished();
	return 0;
}
