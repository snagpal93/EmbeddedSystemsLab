/* Remote Buffer memory map */
#define MCU_ARRAY_OFFSET (0*sizeof(int))

/* Core 2,3,4 Communication Memory map */
#define STOP_FLAG_OFFSET (1*sizeof(int))
#define TOTAL_MCUS (1*sizeof(int))

/* Core 1 Communication Memory map */
#define RC_12_OFFSET (0*sizeof(int))
#define RC_13_OFFSET (1*sizeof(int))
#define RC_23_OFFSET (4*sizeof(int))
#define DONE2_FLAG_OFFSET (2*sizeof(int))
#define DONE1_FLAG_OFFSET (3*sizeof(int))
#define DONE3_FLAG_OFFSET (3*sizeof(int))

#define DONE_FLAG_VALUE_OFFSET (10*sizeof(int))
#define STOP_FLAG_VALUE_OFFSET (15*sizeof(int))

/* Core 1 DMA Memory map */
#define WC_12_OFFSET (0*sizeof(int))
#define WC_13_OFFSET (1*sizeof(int))
#define WC_23_OFFSET (4*sizeof(int))
#define BUFFER_12_OFFSET (2*sizeof(int))
#define BUFFER_13_OFFSET (2*sizeof(int) + 2*sizeof(FF12))

/* Producer FIFO settings*/
/*#define MB1_FIFO_WC  	  (int*)(mb1_dmamem0_BASEADDR + WC_12_OFFSET)
#define MB1_FIFO_RWC 	  (int*)(mb2_cmem0_pt_REMOTEADDR)
#define MB1_FIFO_RC  	  (int*)(mb1_cmem0_BASEADDR + RC_12_OFFSET)
#define MB1_FIFO_RRC  	  (int*)(NULL)
#define MB1_FIFO_BUFFER   (int*)(mb1_dmamem0_BASEADDR + BUFFER_12_OFFSET)
#define MB1_FIFO_RBUFFER  (int*)(mb2_cmem0_pt_REMOTEADDR+6*sizeof(int))


#define MB23_FIFO_WC  	  (int*)(mb2_dmamem0_BASEADDR + WC_23_OFFSET)
#define MB23_FIFO_RWC 	  (int*)(mb3_cmem0_pt_REMOTEADDR)
#define MB23_FIFO_RC  	  (int*)(mb2_cmem0_BASEADDR + RC_23_OFFSET)
#define MB23_FIFO_RRC  	  (int*)(NULL)
#define MB23_FIFO_BUFFER   (int*)(mb2_cmem0_BASEADDR+4*sizeof(int)+4*sizeof(FF12))
#define MB23_FIFO_RBUFFER  (int*)(mb3_cmem0_pt_REMOTEADDR+2*sizeof(int))

#define MB13_FIFO_WC  	  (int*)(mb1_dmamem0_BASEADDR + WC_13_OFFSET)
#define MB13_FIFO_RWC 	  (int*)(mb3_cmem0_pt_REMOTEADDR)
#define MB13_FIFO_RC  	  (int*)(mb1_cmem0_BASEADDR + RC_13_OFFSET)
#define MB13_FIFO_RRC  	  (int*)(NULL)
#define MB13_FIFO_BUFFER   (int*)(mb1_dmamem0_BASEADDR + BUFFER_13_OFFSET)
#define MB13_FIFO_RBUFFER  (int*)(mb3_cmem0_pt_REMOTEADDR+2*sizeof(int))*/


#define MB32_FIFO_WC_S  	  (int*)(mb3_dmamem0_BASEADDR + WC_12_OFFSET)
#define MB32_FIFO_RWC_S 	  (int*)(mb2_cmem0_pt_REMOTEADDR)
#define MB32_FIFO_RC_S 	      (int*)(mb3_cmem0_BASEADDR + RC_12_OFFSET)
#define MB32_FIFO_RRC_S  	  (int*)(NULL)
#define MB32_FIFO_BUFFER_S   (int*)(mb3_dmamem0_BASEADDR + BUFFER_12_OFFSET)
#define MB32_FIFO_RBUFFER_S  (int*)(mb2_cmem0_pt_REMOTEADDR+6*sizeof(int))

#define MB21_FIFO_WC_S  	  (int*)(mb2_dmamem0_BASEADDR + WC_23_OFFSET)
#define MB21_FIFO_RWC_S 	  (int*)(mb1_cmem0_pt_REMOTEADDR)
#define MB21_FIFO_RC_S  	  (int*)(mb2_cmem0_BASEADDR + RC_23_OFFSET)
#define MB21_FIFO_RRC_S  	  (int*)(NULL)
#define MB21_FIFO_BUFFER_S   (int*)(mb2_cmem0_BASEADDR+4*sizeof(int)+4*sizeof(FF32))
#define MB21_FIFO_RBUFFER_S  (int*)(mb1_cmem0_pt_REMOTEADDR+2*sizeof(int))

/* Consumer FIFO settings*/
#define MB32_FIFO_WC_R  	 (int*)(mb2_cmem0_BASEADDR)
#define MB32_FIFO_RWC_R 	 (int*)(NULL)
#define MB32_FIFO_RC_R  	 (int*)(mb2_dmamem0_BASEADDR)
#define MB32_FIFO_RRC_R 	 (int*)(mb3_cmem0_pt_REMOTEADDR)
#define MB32_FIFO_BUFFER_R  (int*)(mb2_cmem0_BASEADDR+6*sizeof(int))
#define MB32_FIFO_RBUFFER_R (int*)(NULL)

#define MB3_FIFO_WC  	 (int*)(mb3_cmem0_BASEADDR)
#define MB3_FIFO_RWC 	 (int*)(NULL)
#define MB3_FIFO_RC  	 (int*)(mb3_dmamem0_BASEADDR)
#define MB3_FIFO_RRC 	 (int*)(mb2_cmem0_pt_REMOTEADDR+RC_23_OFFSET)
#define MB3_FIFO_BUFFER  (int*)(mb3_cmem0_BASEADDR+2*sizeof(int))
#define MB3_FIFO_RBUFFER (int*)(NULL)

#define MB21_FIFO_WC_R  	 (int*)(mb1_cmem0_BASEADDR)
#define MB21_FIFO_RWC_R 	 (int*)(NULL)
#define MB21_FIFO_RC_R 	 (int*)(mb1_dmamem0_BASEADDR)
#define MB21_FIFO_RRC_R 	 (int*)(mb2_cmem0_pt_REMOTEADDR+RC_23_OFFSET)
#define MB21_FIFO_BUFFER_R  (int*)(mb1_cmem0_BASEADDR+2*sizeof(int))
#define MB21_FIFO_RBUFFER_R (int*)(NULL)