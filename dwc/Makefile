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

PROJECT_ROOT = $(shell cd ../../xboot/ && pwd)
ifneq ($(O),)
PROJECT_ROOT = $(O)../../
endif
SPI_NAND_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_HAVE_SPI_NAND")
ifeq (${SPI_NAND_CFG},CONFIG_HAVE_SPI_NAND=y)
CFLAGS += -DCONFIG_HAVE_SPI_NAND
endif
HAVE_SNPS_USB3_DISK_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_HAVE_SNPS_USB3_DISK")
ifeq (${HAVE_SNPS_USB3_DISK_CFG},CONFIG_HAVE_SNPS_USB3_DISK=y)
CFLAGS += -DCONFIG_HAVE_SNPS_USB3_DISK
endif
HAVE_USB_DISK_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_HAVE_USB_DISK")
ifeq (${HAVE_USB_DISK_CFG},CONFIG_HAVE_USB_DISK=y)
CFLAGS += -DCONFIG_HAVE_USB_DISK
endif
HAVE_PARA_NAND_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_HAVE_PARA_NAND")
ifeq (${HAVE_PARA_NAND_CFG},CONFIG_HAVE_PARA_NAND=y)
CFLAGS += -DCONFIG_HAVE_PARA_NAND
endif

# Get Platform configuration from xboot/.config
PROJECT_ROOT = $(shell cd ../../../boot/xboot && pwd)
ifneq ($(O),)
PROJECT_ROOT = $(O)../../
endif
# Get Dram type
HAVE_LPDDR4_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_LPDDR4")
ifeq (${HAVE_LPDDR4_CFG},CONFIG_LPDDR4=y)
CFLAGS += -DCONFIG_LPDDR4
endif
HAVE_DDR4_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_DDR4")
ifeq (${HAVE_DDR4_CFG},CONFIG_DDR4=y)
CFLAGS += -DCONFIG_DDR4
endif
HAVE_LPDDR3_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_LPDDR3")
ifeq (${HAVE_LPDDR3_CFG},CONFIG_LPDDR3=y)
CFLAGS += -DCONFIG_LPDDR3
endif
HAVE_DDR3_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_DDR3")
ifeq (${HAVE_DDR3_CFG},CONFIG_DDR3=y)
CFLAGS += -DCONFIG_DDR3
endif
# Get Dram part number
HAVE_NT6AN1024F32AV_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_NT6AN1024F32AV")
ifeq (${HAVE_NT6AN1024F32AV_CFG},CONFIG_NT6AN1024F32AV=y)
CFLAGS += -DCONFIG_NT6AN1024F32AV
endif
HAVE_MT53D1024M32D4_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_MT53D1024M32D4")
ifeq (${HAVE_MT53D1024M32D4_CFG},CONFIG_MT53D1024M32D4=y)
CFLAGS += -DCONFIG_MT53D1024M32D4
endif
HAVE_MT53E1G32D2_A_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_MT53E1G32D2_A")
ifeq (${HAVE_MT53E1G32D2_A_CFG},CONFIG_MT53E1G32D2_A=y)
CFLAGS += -DCONFIG_MT53E1G32D2_A
endif
HAVE_MT53E1G32D2_B_CFG := $$(cat $(PROJECT_ROOT)/.config | grep "CONFIG_MT53E1G32D2_B")
ifeq (${HAVE_MT53E1G32D2_B_CFG},CONFIG_MT53E1G32D2_B=y)
CFLAGS += -DCONFIG_MT53E1G32D2_B
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
OBJS = $(addprefix $(O),$(CSOURCES:.c=.o))


all: $(OBJS)

$(O)%.o: %.c
	install -d $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependency
$(O)dwc.depend: $(CSOURCES)
	@rm -f $(O)dwc.depend >/dev/null
	@$(CC) $(CFLAGS) -MM $^ >> $(O)dwc.depend
sinclude $(O)dwc.depend

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(O).depend
