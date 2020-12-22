.PHONY: all dram_init debug siscope scan clean

all: dram_init

dram_init:
	$(MAKE) -f main.mk MK_DRAM_INIT=1

debug: clean
	$(MAKE) -f main.mk MK_DEBUG=1

siscope: clean
	$(MAKE) -f main.mk MK_SISCOPE=1

scan: clean
	$(MAKE) -f main.mk MK_SCAN=1

clean:
	$(MAKE) -f main.mk clean

