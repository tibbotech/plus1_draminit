include Makefile.in

TOPDIR := ../../
GEMINI_ROOT=$(TOPDIR)
COMMON_DIR = common
LIB = lib

CROSS ?= armv7a-unknown-eabi-
BIN = bin
TARGET = draminit
LD_FILE = dram_init.ld
LDFLAGS = -T $(LD_FILE)
CFLAGS = -O2 -Wall -g -mtune=cortex-a9 -nostdlib -fno-builtin -Iinclude -mthumb -mthumb-interwork

# SIM_TYPE and these dram options are defined in Makefile.in
CFLAGS += -D$(SIM_TYPE) \
          -DMPEG_DRAM_TYPE_$(MPEG_DRAM_TYPE) \
          -DMPEG_DRAM_DDR_$(MPEG_DRAM_SPEED) \
          -DMPEG_DRAM_$(MPEG_DRAM_BITS) \
          -DSDRAM0_SIZE_$(MPEG_DRAM0_SIZE) \
          -DSDRAM1_SIZE_$(MPEG_DRAM1_SIZE) \
          -D$(QUICK_SIM)_QUICK_SIM

# DEFINE DRAM SIZE FROM CONFIG
USE_DRAM_CFG := $(shell cat $(GEMINI_ROOT).config | grep "CONFIG_USE_DRAM_CFG")
DRAM0_SIZE := $(shell cat $(GEMINI_ROOT).config | grep "CONFIG_DRAM0_SIZE_CFG")
ifeq (${USE_DRAM_CFG},CONFIG_USE_DRAM_CFG=y)

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

# Boot up
ASOURCES = start.S
CSOURCES = plf_dram.c

# common
CSOURCES += $(COMMON_DIR)/diag.c $(COMMON_DIR)/common.c

# lib
ASOURCES += $(LIB)/_udivsi3.S $(LIB)/_divsi3.S
CSOURCES += $(LIB)/div0.c

OBJS = $(ASOURCES:.S=.o) $(CSOURCES:.c=.o)


all: clean
	$(MAKE) $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN)
	$(CROSS)ld $(LDFLAGS) $(OBJS) -o $(BIN)/$(TARGET) -Map $(BIN)/$(TARGET).map
	$(CROSS)objcopy -O binary -S $(BIN)/$(TARGET) $(BIN)/$(TARGET).bin
	$(CROSS)objdump -d -S $(BIN)/$(TARGET) > $(BIN)/$(TARGET).dis
	@# Add image header
	@bash ./add_uhdr.sh draminit-`date +%Y%m%d-%H%M%S` $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).img
	@sz=`du -sb bin/$(TARGET).img|cut -f1` ; \
         printf "draminit size = %d (hex %x)\n" $$sz $$sz
	@if echo $(CFLAGS) | grep -q "DRAM_INIT_DEBUG=1" ;then \
		echo ">> Debug version is built" ; \
	 else \
		echo ">> Release verson is built" ; \
	 fi

%.o: %.S
	@echo $(CROSS)gcc $(CFLAGS) -c -o $@ $<
	$(CROSS)gcc $(CFLAGS) -c -o $@ $<

%.o: %.c
	@echo $(CROSS)gcc $(CFLAGS) -c -o $@ $<
	$(CROSS)gcc $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm -f $(OBJS)
	-cd $(BIN); rm -f $(TARGET) $(TARGET).bin $(TARGET).map $(TARGET).dis $(TARGET).img
	-rm -f scan/*.o $(BIN)/dramscan $(BIN)/dramscan.bin $(BIN)/dramscan.map $(BIN)/dramscan.dis

###################
# debug version
.PHONY: debug
debug: CFLAGS+=-DDRAM_INIT_DEBUG=1
debug: all


###################
# dramscan
.PHONY: scan dramscan
scan: dramscan
dramscan: CFLAGS+=-DBUILD_DRAM_SCAN -DDRAM_INIT_DEBUG=1
dramscan: CSOURCES+=scan/dram_scan.c scan/hw_init.c scan/regRW.c
dramscan: ASOURCES+=scan/scan_start.S
dramscan: $(OBJS) scan/dram_scan.o scan/scan_start.o scan/hw_init.o scan/regRW.o
	@mkdir -p $(BIN)
	$(CROSS)ld -T scan/dram_scan.ld $(OBJS) -o $(BIN)/$@ -Map $(BIN)/$@.map
	$(CROSS)objcopy -O binary -S $(BIN)/$@ $(BIN)/$@.bin
	$(CROSS)objdump -d -S $(BIN)/$@ > $(BIN)/$@.dis
	@echo ">> $@ is built"
