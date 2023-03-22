include Makefile.in
SIM_TYPE := CSIM_ASIC
QUICK_SIM :=  NO

PLATFROM ?= SP7350

# Toolchain path
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
endif

ifeq (${PLATFROM},Q645)
CFLAGS = -DCONFIG_PLATFORM_Q645
endif
ifeq (${PLATFROM},SP7350)
CFLAGS = -DCONFIG_PLATFORM_SP7350
endif

# Get Platform configuration from xboot/.config
PROJECT_ROOT = $(shell cd ../../../boot/xboot && pwd)
# Get Dram type
HAVE_LPDDR4_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_LPDDR4")
ifeq (${HAVE_LPDDR4_CFG},CONFIG_LPDDR4=y)
CFLAGS += -DCONFIG_LPDDR4
endif
HAVE_DDR4_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_DDR4")
ifeq (${HAVE_DDR4_CFG},CONFIG_DDR4=y)
CFLAGS += -DCONFIG_DDR4
endif
HAVE_LPDDR3_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_LPDDR3")
ifeq (${HAVE_LPDDR3_CFG},CONFIG_LPDDR3=y)
CFLAGS += -DCONFIG_LPDDR3
endif
HAVE_DDR3_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_DDR3")
ifeq (${HAVE_DDR3_CFG},CONFIG_DDR3=y)
CFLAGS += -DCONFIG_DDR3
endif
# Get Dram FPGA code
HAVE_NANYA_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_NANYA")
ifeq (${HAVE_NANYA_CFG},CONFIG_NANYA=y)
CFLAGS += -DCONFIG_NANYA
endif
HAVE_D9WHT_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_D9WHT")
ifeq (${HAVE_D9WHT_CFG},CONFIG_D9WHT=y)
CFLAGS += -DCONFIG_D9WHT
endif
HAVE_D9ZQX_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_D9ZQX")
ifeq (${HAVE_D9ZQX_CFG},CONFIG_D9ZQX=y)
CFLAGS += -DCONFIG_D9ZQX
endif
HAVE_D8CJG_CFG := $(shell cat $(PROJECT_ROOT)/.config | grep "CONFIG_D8CJG")
ifeq (${HAVE_D8CJG_CFG},CONFIG_D8CJG=y)
CFLAGS += -DCONFIG_D8CJG
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
