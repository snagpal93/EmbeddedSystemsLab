#include <comik.h>
#include <global_memmap.h>
#include <5kk03-utils.h>
#include <application_memmap.h>
#include "structures.h"
#include "actors.h"
#include <benchmarking.h>

volatile unsigned int *output_mem = (unsigned int*) shared_pt_REMOTEADDR;
volatile unsigned int *shared_mem = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024);
volatile unsigned char *remote_buffer = (unsigned int*) (shared_pt_REMOTEADDR + 4*1024*1024 + 512*1024);
VldHeader header;
SubHeader1 sh1;
SubHeader2 sh2;
FValue fv[10];
PBlock pbout[10];
FBlock fbout;
ColorBuffer cbout;

/*volatile int* mb2_sync_flag = (int*) mb2_remot_SYNC;
volatile int* mb3_sync_flag = (int*) mb3_remot_SYNC;
volatile int* mb4_sync_flag = (int*) mb4_remot_SYNC;*/

int main (void)
{
#ifdef BENCH
	TIME t1, t2, diff;
	t1 = hw_tifu_systimer_get();
#endif // BENCH

	// Sync with the monitor.
	mk_mon_sync();
	// Enable stack checking.
	start_stack_check();

	// Paint it our 'group' color so we can identify it.
/*	for ( int i = 0 ; i < (768*1024);i++ ){
		output_mem[i] = 0xFF1122FF;
	}

    *mb2_sync_flag = 1;
    *mb3_sync_flag = 1;
    *mb4_sync_flag = 1;*/
	// Start decoding the JPEG.
#ifdef BENCH_ACTORS
	TIME t1, t2, diff_init, diff_vld = 0, diff_iqzz = 0, diff_idct = 0, diff_raster = 0, diff_cc = 0;
	t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
    init_header_vld ( &header, shared_mem, output_mem);
#ifdef BENCH_ACTORS
	t2 = hw_tifu_systimer_get();
	diff_init = t2 - t1;
#endif // BENCH_ACTORS
    while(1)
    {
#ifdef BENCH_ACTORS
		t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
        header_vld ( &header, &header, &(fv[0]), &sh1, &sh2  );
#ifdef BENCH_ACTORS
		t2 = hw_tifu_systimer_get();
		diff_vld += (t2 - t1);
#endif // BENCH_ACTORS
        if(sh2.leftover%6<=1){
            for ( int i = 0; i < 10 ; i++ )
            {
#ifdef BENCH_ACTORS
				t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
                iqzz( &(fv[i]), &fbout);
#ifdef BENCH_ACTORS
				t2 = hw_tifu_systimer_get();
				diff_iqzz += (t2 - t1);
#endif // BENCH_ACTORS

#ifdef BENCH_ACTORS
				t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
                idct( &fbout, &(pbout[i]));
#ifdef BENCH_ACTORS
				t2 = hw_tifu_systimer_get();
				diff_idct += (t2 - t1);
#endif // BENCH_ACTORS
            }
#ifdef BENCH_ACTORS
			t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
			cc(&sh1, &(pbout[0]), &cbout);
#ifdef BENCH_ACTORS
			t2 = hw_tifu_systimer_get();
			diff_cc += (t2 - t1);
#endif // BENCH_ACTORS

#ifdef BENCH_ACTORS
			t1 = hw_tifu_systimer_get();
#endif // BENCH_ACTORS
            raster( &sh2, &cbout );
#ifdef BENCH_ACTORS
			t2 = hw_tifu_systimer_get();
			diff_raster += (t2 - t1);
#endif // BENCH_ACTORS
        }
        if ( sh2.leftover == 0 ){
            break;
        }
    }

#ifdef BENCH_IDLE
	mk_mon_debug_info(4);
#endif //BENCH_IDLE

#ifdef BENCH
	t2 = hw_tifu_systimer_get();
	diff = t2 - t1;
	mk_mon_debug_info(1);
	mk_mon_debug_info(LO_64(diff));
#endif // BENCH

#ifdef BENCH_ACTORS
	mk_mon_debug_info(3);
	mk_mon_debug_info(LO_64(diff_init));
	mk_mon_debug_info(LO_64(diff_vld));
	mk_mon_debug_info(LO_64(diff_iqzz));
	mk_mon_debug_info(LO_64(diff_idct));
	mk_mon_debug_info(LO_64(diff_cc));
	mk_mon_debug_info(LO_64(diff_raster));
#endif // BENCH_ACTORS

	// Signal the monitor we are done.
	mk_mon_debug_tile_finished();
	return 0;
}
