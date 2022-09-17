MEMORY
{
  sram0_seg (RW) :                 org = 0x40070000, len = 0x30000
  sram2_seg (RW) :                 org = 0x3FFAE000, len = 0x32000
  sram1_seg (RW) :                 org = 0x3FFE0000, len = 0x20000

  /* 0x1000 bytes are reserved for stack */
  rtc_slow_seg (RW) :              org = 0x50000000, len = 0x1000

  rtc_fast_seg (RX) :              org = 0x400c0000, len = 0x2000
}

/*  Default entry point:  */
ENTRY(call_start_cpu0);


SECTIONS
{

  .sram0 :
  {
    __sram0_begin = .;
    __sram0_end = ORIGIN(sram0_seg) + LENGTH(sram0_seg);
  } > sram0_seg

  .sram1 :
  {
    . += 0x400; /* Vectors table ? */
    __sram1_begin = .;
    __sram1_end = ORIGIN(sram1_seg) + LENGTH(sram1_seg);
  } > sram1_seg

  .sram2 :
  {
    . += 0x20; /* Some global memory is used by ROM bootloader ? */
    __sram2_begin = .;
    __sram2_end = ORIGIN(sram2_seg) + LENGTH(sram2_seg);
  } > sram2_seg

  .iram_loader.text :
  {
    . = ALIGN (16);
    _loader_text_start = ABSOLUTE(.);
    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
     *(.iram1 .iram1.*) /* catch stray IRAM_ATTR */
    *liblog.a:(.literal .text .literal.* .text.*)
    *libgcc.a:(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_clock_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_common_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_flash.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_disable .text.bootloader_random_disable)
    *libesp_common.a:fpga_overrides.*(.literal.bootloader_fill_random .text.bootloader_fill_random)
    *libbootloader_support.a:bootloader_efuse_esp32.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_utility.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_sha.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_console_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_panic.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_soc.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:esp_image_format.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encrypt.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encryption_secure_features.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_partitions.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_secure_features.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_signatures_bootloader.*(.literal .text .literal.* .text.*)
    *libmicro-ecc.a:*.*(.literal .text .literal.* .text.*)
    *libspi_flash.a:*.*(.literal .text .literal.* .text.*)
    *libhal.a:wdt_hal_iram.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:rtc_clk.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:rtc_time.*(.literal .text .literal.* .text.*)
    *libefuse.a:*.*(.literal .text .literal.* .text.*)
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _loader_text_end = ABSOLUTE(.);
  } > rtc_fast_seg

  .iram.text :
  {
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
  } > rtc_fast_seg


  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _dram_start = ABSOLUTE(.);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } >rtc_slow_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    _data_end = ABSOLUTE(.);
  } >rtc_slow_seg

  .dram0.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    *(.sdata2 .sdata2.*)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    *(.eh_frame)
    . = (. + 3) & ~ 3;
    /*  C++ constructor and destructor tables, properly ordered:  */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.*(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    _rodata_end = ABSOLUTE(.);
	/* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _dram_end = ABSOLUTE(.);
  } > rtc_slow_seg

  .iram.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.iram .iram.*) /* catch stray IRAM_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += 16;

    _text_end = ABSOLUTE(.);
    _etext = .;
  } > rtc_fast_seg

}
