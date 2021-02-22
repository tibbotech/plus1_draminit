include Makefile.in
SIM_TYPE := CSIM_ASIC
QUICK_SIM :=  NO


# Toolchain path
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
endif

CFLAGS += -D$(SIM_TYPE)\
	  -D$(QUICK_SIM)_QUICK_SIM

CFLAGS = -Os -Wall -g  -nostdlib -fno-builtin -Isoftware/lpddr4/include -Iinclude -I../../draminit/include

ifeq ($(ARCH),arm)
CFLAGS	+= -march=armv5te
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
CSOURCES += $(DWC_SRC_DIR)/$(DWC)print.c $(DWC_SRC_DIR)/$(DWC)cmnt.c $(DWC_USER_DIR)/$(DWC_USER)A_bringupPower.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)B_startClockResetPhy.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)G_waitFwDone.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)overrideUserInput.c
CSOURCES += $(DWC_SRC_DIR)/$(DWC)print.c $(DWC_SRC_DIR)/$(DWC)cmnt.c $(DWC_USER_DIR)/$(DWC_USER)A_bringupPower.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)B_startClockResetPhy.c $(DWC_USER_DIR)/$(DWC_USER)E_setDfiClk.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)G_waitFwDone.c $(DWC_USER_DIR)/$(DWC_USER)H_readMsgBlock.c $(DWC_USER_DIR)/$(DWC_USER)customPostTrain.c
CSOURCES += $(DWC_USER_DIR)/$(DWC_USER)overrideUserInput.c $(DWC_USER_DIR)/$(DWC_USER)J_enterMissionMode.c

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
