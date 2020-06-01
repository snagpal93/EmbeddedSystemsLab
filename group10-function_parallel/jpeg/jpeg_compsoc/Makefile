OPTFLAGS=-O3 -Wall -Wl,--gc-sections -std=c99

# download image, or not
DO_IMAGE_OUTPUT?=1
IMAGE_OUTPUT_WIDTH?=1024
IMAGE_OUTPUT_HEIGHT?=768

# code for each tile.
MB1_DIR=jpeg_mb1
MB2_DIR=jpeg_mb2
MB3_DIR=jpeg_mb3
MB4_DIR=jpeg_mb4

# shared code and header files.
MB_SHARED=jpeg_shared

# Enable DMA mode (instead of MMIO)
USE_DMA=0

# Upload files before the run
DATA_FILES=/home/emb18/emb18058/group10_6/group10/jpeg/jpeg_compsoc/images/test.jpg

# force use of FPGA board connected to VGA framegrabber
USE_VGA_GRABBER?=0

# force image to be downloaded in binary form (not Huffman encoded)
#FORCE_BINARY?=1

# specify timeout
USER_TIMEOUT?=120

include /home/emb18/material/platform/platform.mk
