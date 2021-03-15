include Makefile.in

PROJECT_ROOT = $(shell cd ../xboot/ && pwd)
COMMON_DIR = common
LIB = lib
BIN = bin

# Toolchain path
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
endif

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
LDFLAGS += -L $(shell dirname `$(CC) -print-libgcc-file-name`) -lgcc
LDFLAGS += -Wl,--build-id=none
CFLAGS = -Os -Wall -g  -nostdlib -fno-builtin -Iinclude

ifeq ($(ARCH),arm)
CFLAGS	+= -march=armv5te
else
CFLAGS	+= -march=rv64gc -mabi=lp64d -mcmodel=medany -msave-restore
endif
SRC_LD	:= arch/$(ARCH)/gen_ld.lds
START_S	:= arch/$(ARCH)/start.S
# CFLAGS = -O1 -Wall -g -nostdlib -fno-builtin -Iinclude
ifeq ($(DRAM_INIT),1)
ifeq ($(ARCH),arm)
	CFLAGS += -mthumb -mthumb-interwork
endif
endif
CFLAGS += -fno-pie -fno-PIE -fno-pic -fno-PIC
CFLAGS += -fno-partial-inlining
CFLAGS += -fno-jump-tables
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,--gc-sections

# Get Platform configuration from xboot/.config
PLATFROM_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_PLATFORM_I143")
ifeq (${PLATFROM_CFG},CONFIG_PLATFORM_I143=y)
CC = $(CROSS)gcc
CFLAGS += -DCONFIG_PLATFORM_I143
endif

# Get DRAM configuration from ../../.config or Makefile.in
USE_DRAM_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_USE_DRAM_CFG")
DRAM0_SIZE := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_DRAM0_SIZE_CFG")

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
#CFLAGS += -DCONFIG_DRAM_SIZE_USE_OTP

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
ASOURCES = $(START_S)
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
	$(CC) -E -x c -DDRAM_INIT $(SRC_LD) | grep -v '^#' > autogen.ld
else ifeq ($(MK_DEBUG),1)
	$(CC) -E -x c -DDRAM_INIT_DEBUG $(SRC_LD) | grep -v '^#' > autogen.ld
else ifeq ($(MK_SISCOPE),1)
	$(CC) -E -x c -DSISCOPE   $(SRC_LD) | grep -v '^#' > autogen.ld
else ifeq ($(MK_SCAN),1)
	$(CC) -E -x c -DDRAMSCAN  $(SRC_LD) | grep -v '^#' > autogen.ld
endif
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(BIN)/$(TARGET) -Wl,-Map,$(BIN)/$(TARGET).map
	$(OBJCOPY) -O binary -S $(BIN)/$(TARGET) $(BIN)/$(TARGET).bin
	$(OBJDUMP) -d -S $(BIN)/$(TARGET) > $(BIN)/$(TARGET).dis
ifeq ($(DRAM_INIT),1)
	@# Add image header
	@bash ./add_uhdr.sh draminit-`date +%Y%m%d-%H%M%S` $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).img $(ARCH)
	@sz=`du -sb bin/$(TARGET).img|cut -f1` ; \
	printf "draminit size = %d (hex %x)\n" $$sz $$sz
	@if echo $(CFLAGS) | grep -q "DRAM_INIT_DEBUG=1" ;then \
		echo ">> Debug version is built" ; \
	else \
		echo ">> Release verson is built" ; \
	fi
endif

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependency
.depend: $(ASOURCES) $(CSOURCES)
	@rm -f .depend >/dev/null
	@$(CC) $(CFLAGS) -MM $^ >> ./.depend
sinclude .depend

.PHONY: clean
clean:
	rm -f $(OBJS) autogen.ld
	rm -rf $(BIN)
	rm -f .depend
