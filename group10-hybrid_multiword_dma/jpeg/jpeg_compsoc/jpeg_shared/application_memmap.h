/* Core 1 Communication Memory map */
#define RC_12_OFFSET (0*sizeof(int))
#define RC_13_OFFSET (1*sizeof(int))
#define RC_14_OFFSET (2*sizeof(int))
#define DONE2_FLAG_OFFSET (3*sizeof(int))
#define DONE3_FLAG_OFFSET (4*sizeof(int))
#define DONE4_FLAG_OFFSET (5*sizeof(int))

/* Core 1 DMA Memory map */
#define WC_12_OFFSET (0*sizeof(int))
#define WC_13_OFFSET (1*sizeof(int))
#define WC_14_OFFSET (2*sizeof(int))
#define STOP_FLAG_VALUE_OFFSET (3*sizeof(int))
#define BUFFER_12_OFFSET (4*sizeof(int))
#define BUFFER_13_OFFSET (BUFFER_12_OFFSET + 2*sizeof(MCU))
#define BUFFER_14_OFFSET (BUFFER_13_OFFSET + 2*sizeof(MCU))
#define FGETC_DST_1 (BUFFER_14_OFFSET + 2*sizeof(MCU))
#define FGETC_DST_2 (FGETC_DST_1 + 1024*sizeof(int))

/* Core 2,3,4 Communication Memory map */
#define WC_OFFSET (0*sizeof(int))
#define STOP_FLAG_OFFSET (1*sizeof(int))
#define BUFFER_OFFSET (2*sizeof(int))

/* Core 2,3,4 DMA Memory map */
#define RC_OFFSET (0*sizeof(int))
#define FGETC_DST (1*sizeof(int))
#define DONE_FLAG_VALUE_OFFSET (2*sizeof(int))
#define CBOUT_STRUCT_OFFSET (3*sizeof(int))

/* Producer FIFO settings*/
#define MB1_FIFO_WC  	  (int*)(mb1_dmamem0_BASEADDR + WC_12_OFFSET)
#define MB1_FIFO_RWC 	  (int*)(mb2_cmem0_pt_REMOTEADDR)
#define MB1_FIFO_RC  	  (int*)(mb1_cmem0_BASEADDR + RC_12_OFFSET)
#define MB1_FIFO_RRC  	  (int*)(NULL)
#define MB1_FIFO_BUFFER   (int*)(mb1_dmamem0_BASEADDR + BUFFER_12_OFFSET)
#define MB1_FIFO_RBUFFER  (int*)(mb2_cmem0_pt_REMOTEADDR+2*sizeof(int))

#define MB13_FIFO_WC  	  (int*)(mb1_dmamem0_BASEADDR + WC_13_OFFSET)
#define MB13_FIFO_RWC 	  (int*)(mb3_cmem0_pt_REMOTEADDR)
#define MB13_FIFO_RC  	  (int*)(mb1_cmem0_BASEADDR + RC_13_OFFSET)
#define MB13_FIFO_RRC  	  (int*)(NULL)
#define MB13_FIFO_BUFFER   (int*)(mb1_dmamem0_BASEADDR+BUFFER_13_OFFSET)
#define MB13_FIFO_RBUFFER  (int*)(mb3_cmem0_pt_REMOTEADDR+2*sizeof(int))

#define MB14_FIFO_WC  	  (int*)(mb1_dmamem0_BASEADDR + WC_14_OFFSET)
#define MB14_FIFO_RWC 	  (int*)(mb4_cmem0_pt_REMOTEADDR)
#define MB14_FIFO_RC  	  (int*)(mb1_cmem0_BASEADDR + RC_14_OFFSET)
#define MB14_FIFO_RRC  	  (int*)(NULL)
#define MB14_FIFO_BUFFER   (int*)(mb1_dmamem0_BASEADDR + BUFFER_14_OFFSET)
#define MB14_FIFO_RBUFFER  (int*)(mb4_cmem0_pt_REMOTEADDR+2*sizeof(int))

/* Consumer FIFO settings*/
#define MB2_FIFO_WC  	 (int*)(mb2_cmem0_BASEADDR)
#define MB2_FIFO_RWC 	 (int*)(NULL)
#define MB2_FIFO_RC  	 (int*)(mb2_dmamem0_BASEADDR)
#define MB2_FIFO_RRC 	 (int*)(mb1_cmem0_pt_REMOTEADDR + RC_12_OFFSET)
#define MB2_FIFO_BUFFER  (int*)(mb2_cmem0_BASEADDR + BUFFER_OFFSET)
#define MB2_FIFO_RBUFFER (int*)(NULL)

#define MB3_FIFO_WC  	 (int*)(mb3_cmem0_BASEADDR)
#define MB3_FIFO_RWC 	 (int*)(NULL)
#define MB3_FIFO_RC  	 (int*)(mb3_dmamem0_BASEADDR)
#define MB3_FIFO_RRC 	 (int*)(mb1_cmem0_pt_REMOTEADDR + RC_13_OFFSET)
#define MB3_FIFO_BUFFER  (int*)(mb3_cmem0_BASEADDR + BUFFER_OFFSET)
#define MB3_FIFO_RBUFFER (int*)(NULL)

#define MB4_FIFO_WC  	 (int*)(mb4_cmem0_BASEADDR)
#define MB4_FIFO_RWC 	 (int*)(NULL)
#define MB4_FIFO_RC  	 (int*)(mb4_dmamem0_BASEADDR)
#define MB4_FIFO_RRC 	 (int*)(mb1_cmem0_pt_REMOTEADDR + RC_14_OFFSET)
#define MB4_FIFO_BUFFER  (int*)(mb4_cmem0_BASEADDR + BUFFER_OFFSET)
#define MB4_FIFO_RBUFFER (int*)(NULL)