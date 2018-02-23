
This dram_init source is ported from 8700 dram_init code.

draminit.img is loaded and run by xboot to initialize DRAM.

(draminit.img = 64-byte header + draminit.bin)

The dram scan function is ported from DRAM_SCAN_SOURCE code.


Build
~~~~~~

Build release version
> make clean
> make
--> Output: bin/output: bin/draminit.img


Build debug version (big size due to debug messages)
> make clean
> make debug
--> Output: bin/output: bin/draminit.img


Build dramscan (always debug version)
> make clean
> make scan
--> Output: bin/dramscan.bin


DRAM configuration
~~~~~~~~~~~~~~~~~~~~

The dram config options are in Makefile.in.
Source plf_dram.c refers to these options.


System configuration
~~~~~~~~~~~~~~~~~~~~~

Systemm options are in include/config.h.


Code Flow
~~~~~~~~~~~

* draminit :
   [start.S   ] _start ->
   [plf_dram.c] dram_init_main() -> dram_init(0)

   Linker script: dram_init.ld

* dramscan :
   [scan/scan_start.S] _scan_start ->
   [scan/dram_scan.c ] dram_scan_main() ->
   [scan/hw_init.c   ] hardware_init()
   [scan/regRW.c     ] registerRW() (MON shell)
   [scan/dram_scan.c ] DRAM_SCAN(0)
   [plf_dram.c       ] dram_init(0)

   Linker script: scan/dram_scan.ld

