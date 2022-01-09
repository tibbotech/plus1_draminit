include Makefile.in
SIM_TYPE := CSIM_ASIC
QUICK_SIM :=  NO


# Toolchain path
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
endif

# Get Platform configuration from xboot/.config
PROJECT_ROOT = $(shell cd ../../xboot/ && pwd)
PLATFROM_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_PLATFORM_")
ifeq (${PLATFROM_CFG},CONFIG_PLATFORM_Q645=y)
CFLAGS = -DCONFIG_PLATFORM_Q645
endif
ifeq (${PLATFROM_CFG},CONFIG_PLATFORM_SP7350=y)
CFLAGS = -DCONFIG_PLATFORM_SP7350
endif
SPI_NAND_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_HAVE_SPI_NAND")
ifeq (${SPI_NAND_CFG},CONFIG_HAVE_SPI_NAND=y)
CFLAGS += -DCONFIG_HAVE_SPI_NAND
endif

CFLAGS += -D$(SIM_TYPE)\
	  -D$(QUICK_SIM)_QUICK_SIM

CFLAGS += -Os -Wall -g  -nostdlib -fno-builtin -Isoftware/lpddr4/include -Iinclude -I../../draminit/include

ifeq ($(ARCH),arm)
CFLAGS	+= -march=armv8-a
else
CFLAGS	+= -march=rv64gc -mabi=lp64d -mcmodel=medany -msave-restore
endif

# CFLAGS = -O1 -Wall -g -nostdlib -fno-builtin -Iinclude
ifeq ($(ARCH),arm)
	CFLAGS += -mthumb -mthumb-interwork
endif
CFLAGS += -fno-pie -fno-PIE -fno-pic -fno-PIC
CFLAGS += -fno-partial-inlining
CFLAGS += -fno-jump-tables
CFLAGS += -fdata-sections -ffunction-sections

CFLAGS += -DDRAM_INIT=1
CFLAGS += -DDRAM_INIT_DEBUG=1


CFLAGS += -D$(SIM_TYPE)\
	  -D$(QUICK_SIM)_QUICK_SIM

# software
DWC_SRC_DIR=software/lpddr4/src
DWC_USER_DIR=software/lpddr4/userCustom
DWC=dwc_ddrphy_phyinit_
DWC_USER=dwc_ddrphy_phyinit_userCustom_
CSOURCES = dwc_dram.c
CSOURCES += dwc_umctl2.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)main.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)globals.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)sequence.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)restore_sequence.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)C_initPhyConfig.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)I_loadPIEImage.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)progCsrSkipTrain.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)setUserInput.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)getUserInput.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)cmnt.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)print.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)assert.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)print_dat.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)setMb.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)softSetMb.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)getMb.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)LoadPieProdCode.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)calcMb.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)G_execFW.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)H_readMsgBlock.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)mapDrvStren.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)D_loadIMEM.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)F_loadDMEM.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)storeIncvFile.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)storeMsgBlk.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)WriteOutMem.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)IsDbyteDisabled.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)initStruct.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)regInterface.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)setStruct.c
CSOURCES += $(DWC_USER_DIR)/$(DWC)setDefault.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)overrideUserInput.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)A_bringupPower.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)B_startClockResetPhy.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)customPreTrain.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)customPostTrain.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)E_setDfiClk.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)G_waitFwDone.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)H_readMsgBlock.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)J_enterMissionMode.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)io_write16.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)io_read16.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)saveRetRegs.c
OBJS = $(CSOURCES:.c=.o)


all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependency
.depend: $(CSOURCES)
	@rm -f .depend >/dev/null
	@$(CC) $(CFLAGS) -MM $^ >> ./.depend
sinclude .depend

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f .depend
