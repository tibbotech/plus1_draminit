SIScope:
1)make CROSS=.../boot/xboot/tools/armv7a-unknown-eabi_gcc_4.7.2/bin/armv7a-unknown-eabi- siscope

2) Copy dram_init/bin/siscope.bin to USB storage.

3) In U-Boot, load it to SRAM and execute it:
   * setenv src_dev "usb 0"           # or "usb 0:0" if fails in next step.
   * usb start && fatls ${src_dev} /
   * fatload  ${src_dev} 0x9e800000 /siscope.bin && md.b 0x9e800000 0x100
   * go 0x9e800000

4) Functions
    3.1) *Would you like to run SISCOPE?(y/n)
         y: Run SIScope
         n: Skip it.
    3.2) *Would you like to run stress test?(y/n)
         y: *Would you like to test incrementing or decrementing ddr freq?(y/n)
            y: *please input percent of incrementing or decrementing ddr freq:
               Input percentage to start test.
            n: *Loop Test?(y/n)
                y: loop test forever.
                n: run test once.
         n: Skip it.

    3.3) *Would you like to run DDR EYE WINDOW ANALYSIS?(y/n)

5) main code flow:
    -----------------------
    |   dram_init_main    |
    -----------------------
               |
               v
     ---------------------
     |  silent_dram_init |
     ---------------------
               |
               v
    -----------------------
    |  check_run_siscope  |
    -----------------------
               |
               v
    -----------------------
    |  check_stress_test  |
    -----------------------
               |
               v
    --------------------------
    |  check_ddr_eye_window  |
    --------------------------


6) SISCOPE FLOW:
    -----------------------
    |  check_run_siscope  |
    -----------------------
               |
               v
    -----------------------
    |     run_SiScope     |
    -----------------------
               |
               v
    -----------------------
    |      DRAM_SCAN      |
    -----------------------
               |
               v
    -----------------------
    |   setting:         |
    |   sdc_str_dqs_in   |<------------------
    |   sdc_ext_cl_cnt   |                  |
    |   sdc_int_wl_cnt   |                  |
    ----------------------                  |
               |                            |
               v                            |
    -----------------------                 |
    |   silent_dram_init  |                 |
    -----------------------                 |
               |                            |
               v                            |
 -----------------------------              |
 |  CPU R/W TEST:            |              |
 |  1.sequence test ddr R/W  |              |
 |  2.random test ddr R/W    |              |
 |  3.invert test ddr R/W    |              |
 -----------------------------              |
               |                            |
               v                            |
     ---------------------                  |
     |  CBDMA R/W TEST   |                  |
     ---------------------                  |
               |                            |
               v                            |
 ------------------------------             |
 |  caculate 1 PSD = ? pico   |             |
 ------------------------------             |
               |                            |
               v                            |
  ----------------------------              |
  |  add BDD flow:           |              |
  |  (test by CPU R/W TEST)  |              |
  |  1.find DQ setup time    |              |
  |  2.fine DQS hold time    |              |
  ----------------------------              |
               |                            |
               v                            |
 ------------------------------             |
 |  add all DQ/DQS  265 pico  |             |
 ------------------------------             |
               |                            |
               v                            |
 ----------------------------               |
 |  sub BDD flow:           |               |
 |  (test by CPU R/W TEST)  |               |
 |  1.find DQ hold time     |               |
 |  2.fine DQS setup time   |               |
 ----------------------------               |
               |                            |
               v                            |
 ------------------------------             |
 |  sub all DQ/DQS  265 pico  |             |
  ------------------------------            |
               |                            |
               v                            |
  ------------------------------            |
  |  add AC/ACK BDD flow:      |            |
  |  1.find AC/ACK setup time  |            |
  ------------------------------            |
               |                            |
               v                            |
     ---------------------                  |
     |  report result    |                  |
     ---------------------                  |
             |                              |
             |     (if scan not end)        |
             |------------------------------|
             |
             v
      ---------------
      |    DONE     |
      --------------
