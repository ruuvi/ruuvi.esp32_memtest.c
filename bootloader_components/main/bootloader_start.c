/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "bootloader_init.h"
#include "bootloader_utility.h"
#include "bootloader_common.h"
#include "bootloader_mem.h"
#include "bootloader_clock.h"
#include "bootloader_console.h"
#include "bootloader_flash_config.h"
#include "bootloader_flash.h"
#include "bootloader_flash_priv.h"
#include <assert.h>
#include "soc/cpu.h"
#include "soc/soc.h"
#include "soc/spi_reg.h"
#include "soc/dport_reg.h"
#include "rom/cache.h"
#include "rom/spi_flash.h"

static void bootloader_reset_mmu(void)
{
  /* completely reset MMU in case serial bootloader was running */
  Cache_Read_Disable(0);
#if !CONFIG_FREERTOS_UNICORE
  Cache_Read_Disable(1);
#endif
  Cache_Flush(0);
#if !CONFIG_FREERTOS_UNICORE
  Cache_Flush(1);
#endif
  mmu_init(0);
#if !CONFIG_FREERTOS_UNICORE
  /* The lines which manipulate DPORT_APP_CACHE_MMU_IA_CLR bit are
      necessary to work around a hardware bug. */
  DPORT_REG_SET_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
  mmu_init(1);
  DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MMU_IA_CLR);
#endif

  /* normal ROM boot exits with DROM0 cache unmasked,
      but serial bootloader exits with it masked. */
  DPORT_REG_CLR_BIT(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MASK_DROM0);
#if !CONFIG_FREERTOS_UNICORE
  DPORT_REG_CLR_BIT(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DROM0);
#endif
}

void bootloader2_init(void)
{
  bootloader_init_mem();

  // clear bss section
  bootloader_clear_bss_section();

  // bootst up vddsdio
  bootloader_common_vddsdio_configure();

  // reset MMU
  bootloader_reset_mmu();

  // config clock
  bootloader_clock_configure();
  // initialize uart console, from now on, we can use esp_log
  bootloader_console_init();

  // config WDT
  bootloader_config_wdt();
}

/*
 * We arrive here after the ROM bootloader finished loading this second stage bootloader from flash.
 * The hardware is mostly uninitialized, flash cache is down and the app CPU is in reset.
 * We do have a stack, so we can do the initialization in C.
 */
void __attribute__((noreturn)) call_start_cpu0(void)
{
  bootloader2_init();

  // Switch the stack pointer to the end of RTC Slow memory
  asm	(
      "mov.n sp, %0\n"
      :
      :"r"(0x50001FFC)
  );

  // Disable IRQs
  XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);

  while (1) {
  }
}

// Return global reent struct if any newlib functions are linked to bootloader
struct _reent *__getreent(void)
{
    return _GLOBAL_REENT;
}
