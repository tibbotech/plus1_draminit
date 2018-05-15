include Makefile.in

GEMINI_ROOT = $(shell cd ../../ && pwd)
COMMON_DIR = common
LIB = lib
BIN = bin
CROSS ?= ../../build/tools/armv5-eabi--glibc--stable/bin/armv5-glibc-linux-

ifeq ($(MK_DRAM_INIT),1)
	TARGET = draminit
	DRAM_INIT = 1
else ifeq ($(MK_DEBUG),1)
	TARGET = draminit
	DRAM_INIT = 1
	DRAM_INIT_DEBUG = 1
else ifeq ($(MK_SISCOPE),1)
	TARGET = siscope
	DRAM_INIT_DEBUG = 1
	SISCOPE = 1
else ifeq ($(MK_SCAN),1)
	TARGET = dramscan
	DRAM_INIT_DEBUG = 1
	DRAMSCAN = 1
endif

LDFLAGS = -T autogen.ld
LDFLAGS += -L $(shell dirname `$(CROSS)gcc -print-libgcc-file-name`) -lgcc

CFLAGS = -Os -Wall -g -march=armv5te -nostdlib -fno-builtin -Iinclude
ifeq ($(DRAM_INIT),1)
CFLAGS += -mthumb -mthumb-interwork
endif

# Get DRAM configuration from ../../.config or Makefile.in
USE_DRAM_CFG := $(shell cat $(GEMINI_ROOT)/.config | grep "CONFIG_USE_DRAM_CFG")
DRAM0_SIZE := $(shell cat $(GEMINI_ROOT)/.config | grep "CONFIG_DRAM0_SIZE_CFG")

CFLAGS += -D$(SIM_TYPE)\
          -D$(QUICK_SIM)_QUICK_SIM

# Following parameters are defined in include/dram_param.h already:
# CFLAGS += -DMPEG_DRAM_TYPE_$(MPEG_DRAM_TYPE) \
#           -DMPEG_DRAM_DDR_$(MPEG_DRAM_SPEED) \
#           -DMPEG_DRAM_$(MPEG_DRAM_BITS)

ifneq (${USE_DRAM_CFG},CONFIG_USE_DRAM_CFG=y)
# from Makefile.in
CFLAGS += -DSDRAM0_SIZE_$(MPEG_DRAM0_SIZE) \
          -DSDRAM1_SIZE_$(MPEG_DRAM1_SIZE)
else
# CONFIG_USE_DRAM_CFG=y, => from ../../.config
CFLAGS += -DCONFIG_USE_DRAM_CFG
ifeq (${DRAM0_SIZE},CONFIG_DRAM0_SIZE_CFG="1Gb")
CFLAGS += -DCONFIG_DRAM0_SIZE_1Gb
else ifeq (${DRAM0_SIZE},CONFIG_DRAM0_SIZE_CFG="2Gb")
CFLAGS += -DCONFIG_DRAM0_SIZE_2Gb
else ifeq (${DRAM0_SIZE},CONFIG_DRAM0_SIZE_CFG="4Gb")
CFLAGS += -DCONFIG_DRAM0_SIZE_4Gb
else
CFLAGS += -DCONFIG_DRAM_SIZE_NOT_SUPPORT
endif
endif

ifeq ($(DRAM_INIT),1)
	CFLAGS += -DDRAM_INIT=1
endif

ifeq ($(DRAM_INIT_DEBUG),1)
	CFLAGS += -DDRAM_INIT_DEBUG=1
endif

ifeq ($(DRAMSCAN),1)
	CFLAGS += -DDRAMSCAN=1
endif

ifeq ($(SISCOPE),1)
	CFLAGS += -DSISCOPE=1
endif

# Boot up
ASOURCES = start.S
CSOURCES = plf_dram.c

# common
CSOURCES += $(COMMON_DIR)/diag.c $(COMMON_DIR)/common.c

# lib
#ASOURCES += $(LIB)/_udivsi3.S $(LIB)/_divsi3.S
#CSOURCES += $(LIB)/div0.c
CSOURCES += $(LIB)/eabi_compat.c

OBJS = $(ASOURCES:.S=.o) $(CSOURCES:.c=.o)


all: $(TARGET)

$(TARGET): $(OBJS)
ifeq ($(MK_DRAM_INIT),1)
	gcc -E -x c -DDRAM_INIT gen_ld.lds | grep -v '^#' > autogen.ld
else ifeq ($(MK_DEBUG),1)
	gcc -E -x c -DDRAM_INIT gen_ld.lds | grep -v '^#' > autogen.ld
else ifeq ($(MK_SISCOPE),1)
	gcc -E -x c -DSISCOPE   gen_ld.lds | grep -v '^#' > autogen.ld
else ifeq ($(MK_SCAN),1)
	gcc -E -x c -DDRAMSCAN  gen_ld.lds | grep -v '^#' > autogen.ld
endif
	@mkdir -p $(BIN)
	$(CROSS)gcc $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(BIN)/$(TARGET) -Wl,-Map,$(BIN)/$(TARGET).map
	$(CROSS)objcopy -O binary -S $(BIN)/$(TARGET) $(BIN)/$(TARGET).bin
	$(CROSS)objdump -d -S $(BIN)/$(TARGET) > $(BIN)/$(TARGET).dis
ifeq ($(DRAM_INIT),1)
	@# Add image header
	@bash ./add_uhdr.sh draminit-`date +%Y%m%d-%H%M%S` $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).img
	@sz=`du -sb bin/$(TARGET).img|cut -f1` ; \
         printf "draminit size = %d (hex %x)\n" $$sz $$sz
	@if echo $(CFLAGS) | grep -q "DRAM_INIT_DEBUG=1" ;then \
		echo ">> Debug version is built" ; \
	 else \
		echo ">> Release verson is built" ; \
	 fi
endif

%.o: %.S
	$(CROSS)gcc $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CROSS)gcc $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) autogen.ld
	rm -rf $(BIN)