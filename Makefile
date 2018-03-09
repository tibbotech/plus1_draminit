.PHONY: all dram_init debug siscope scan clean

all: dram_init

dram_init:
	make -f main.mk MK_DRAM_INIT=1

debug:
	make -f main.mk MK_DEBUG=1

siscope:
	make -f main.mk MK_SISCOPE=1

scan:
	make -f main.mk MK_SCAN=1

clean:
	make -f main.mk clean

