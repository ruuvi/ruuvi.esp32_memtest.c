/*
* MemTest86+ V5 Specific code (GPL V2.0)
* By Samuel DEMEULEMEESTER, sdemeule@memtest.org
* http://www.canardpc.com - http://www.memtest.org
* ------------------------------------------------
* main.c - MemTest-86  Version 3.5
*
* Released under version 2 of the Gnu Public License.
* By Chris Brady
*/

/**
* @file memtest.c
* @brief The MemTest86+ port for ESP32
* @author TheSomeMan
* @date 2022-09-18
*/

#include "memtest.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "test.h"
#include "memtest_random.h"

struct tseq tseq[] = {
    {1, -1, 0,  6,   0, "[Address test, walking ones, no cache] "},
    {1, -1, 1,  6,   0, "[Address test, own address Sequential] "},
    {1, 32, 2,  6,   0, "[Address test, own address Parallel]   "},
    {1, 32, 3,  6,   0, "[Moving inversions, 1s & 0s Parallel]  "},
    {1, 32, 5,  3,   0, "[Moving inversions, 8 bit pattern]     "},
    {1, 32, 6,  30,  0, "[Moving inversions, random pattern]    "},
    {1, 32, 7,  81,  0, "[Block move]                           "},
    {1, 1,  8,  3,   0, "[Moving inversions, 32 bit pattern]    "},
    {1, 32, 9,  48,  0, "[Random number sequence]               "},
    {1, 32, 10, 6,   0, "[Modulo 20, Random pattern]            "},
    {1, 1,  11, 240, 0, "[Bit fade test, 2 patterns]            "},
    {1, 0,  0,  0,   0, NULL}
};

volatile int run_cpus = 1;
struct vars variables = {};
struct vars *const v = &variables;
volatile int segs = 3;
volatile int mstr_cpu = 0;
uint32_t g_err_cnt = 0;

void ad_err1(ulong *adr1, ulong *adr2, ulong good, ulong bad)
{
  g_err_cnt += 1;
  esp_rom_printf("ad_err1 failed: adr1=%p, adr2=%p, good=0x%08x, bad=0x%08x\n",
                 adr1, adr2, good, bad);
}

void ad_err2(ulong *adr, ulong bad)
{
  g_err_cnt += 1;
  esp_rom_printf("ad_err2 failed: adr=%p, bad=0x%08x\n", adr, bad);
}

void error(ulong *adr, ulong good, ulong bad)
{
  g_err_cnt += 1;
  esp_rom_printf("error: adr=%p, good=0x%08x, bad=0x%08x\n", adr, good, bad);
}

void hprint(int y, int x, unsigned long val)
{

}

static void print_mem_region_info(const int idx, const volatile struct mmap *p_mmap)
{
  esp_rom_printf("SRAM%d: %p..%p, size=0x%08x\n",
                 idx,
                 v->map[idx].start,
                 v->map[idx].end + 1,
                 (uintptr_t) v->map[idx].end - (uintptr_t) v->map[idx].start + sizeof(uint32_t));
}

static void memtest_init_map(volatile struct mmap *const p_mmap, void *p_begin, void *p_end)
{
  p_mmap->start = p_begin;
  p_mmap->end = (void *) ((uint32_t *) p_end - 1);
  p_mmap->pbase_addr = 0;
}

void memtest_init(void)
{
  segs = 3;
  g_err_cnt = 0;

  memtest_init_map(&v->map[0], (void *) &__sram0_begin, (void *) &__sram0_end);
  memtest_init_map(&v->map[1], (void *) &__sram1_begin, (void *) &__sram1_end);
  memtest_init_map(&v->map[2], (void *) &__sram2_begin, (void *) &__sram2_end);

  for (int i = 0; i < segs; ++i) {
    v->pmap[i].start = (ulong) v->map[i].start;
    v->pmap[i].end = (ulong) v->map[i].end;

    print_mem_region_info(i, &v->map[i]);
  }
  v->msegs = segs;
}

void mem_fill(const uint32_t addr, const uint32_t val, const uint32_t len)
{
  uint32_t *p_addr = (uint32_t *) addr;
  uint32_t word_cnt = len / sizeof(uint32_t);
  while (word_cnt--) {
    *p_addr++ = val;
  }
}

bool mem_check(const uint32_t addr, const uint32_t val, const uint32_t len)
{
  uint32_t *p_addr = (uint32_t *) addr;
  uint32_t word_cnt = len / sizeof(uint32_t);
  while (word_cnt--) {
    if (*p_addr++ != val) {
      return false;
    }
  }
  return true;
}


bool test_mem_fill(uint32_t fill_pattern)
{
  for (int i = 0; i < segs; ++i) {
    const uint32_t seg_size = v->map[i].end - v->map[i].start;
    mem_fill((uint32_t) v->map[i].start, fill_pattern, seg_size);
  }
  for (int i = 0; i < segs; ++i) {
    const uint32_t seg_size = v->map[i].end - v->map[i].start;
    if (!mem_check((uint32_t) v->map[i].start, fill_pattern, seg_size)) {
      esp_rom_printf("test_mem_fill failed: segment %d\n");
      return false;
    }
  }
  return true;
}

bool do_test(void)
{
  const int my_ord = 0;

  esp_rom_printf("Do test: memory fill\n");
  if (!test_mem_fill(0x00000000)) {
    return false;
  }
  if (!test_mem_fill(0xFFFFFFFF)) {
    return false;
  }

#if MEMTEST_DO_TEST_0
  esp_rom_printf("Do test: Address test, walking ones (test #0)\n");
  addr_tst1(my_ord);
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_1
  esp_rom_printf("Do test: Address test, own address (test #1, 2)\n");
  addr_tst2(my_ord);
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_3
  esp_rom_printf("Do test: Moving inversions, all ones and zeros (tests #3, 4)\n");
  {
    const int test_idx = 3 - 1;
    const int c_iter = tseq[test_idx].iter;
    ulong p1 = 0;
    ulong p2 = ~p1;
    movinv1(c_iter, p1, p2, my_ord);
    BAILOUT;

    /* Switch patterns */
    movinv1(c_iter, p2, p1, my_ord);
    BAILOUT;
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_5
  esp_rom_printf("Do test: Moving inversions, 8 bit walking ones and zeros (test #5)\n");
  {
    const int test_idx = 5 - 1;
    const int c_iter = tseq[test_idx].iter;
    ulong p0 = 0x80;
    for (int i=0; i<8; i++, p0=p0>>1) {
      ulong p1 = p0 | (p0<<8) | (p0<<16) | (p0<<24);
      ulong p2 = ~p1;
      movinv1(c_iter,p1,p2, my_ord);
      BAILOUT;
      esp_rom_printf(".");

      /* Switch patterns */
      movinv1(c_iter,p2,p1, my_ord);
      BAILOUT
      esp_rom_printf(".");
    }
    esp_rom_printf("\n");
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_6
  esp_rom_printf("Do test: Random Data (test #6)\n");
  {
    const int test_idx = 6 - 1;
    const int c_iter = tseq[test_idx].iter;
    /* Seed the random number generator */
    ulong sp1 = 521288629 + v->pass;
    ulong sp2 = 362436069 - v->pass;
    memtest_rand_seed(sp1, sp2, 0);

    for (int i=0; i < c_iter; i++) {
      if (my_ord == mstr_cpu) {
        sp1 = rand(0);
        sp2 = ~sp1;
      }
      movinv1(2,sp1,sp2, my_ord);
      BAILOUT;
      esp_rom_printf(".");
    }
    esp_rom_printf("\n");
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if 0
  esp_rom_printf("Skip test: Block move (test #7) - not implemented\n");
  {
    const int test_idx = 7 - 1;
    const int c_iter = tseq[test_idx].iter;
    block_move(c_iter, my_ord);
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_8
  esp_rom_printf("Do test: Moving inversions, 32 bit shifting pattern (test #8)\n");
  {
    const int test_idx = 8 - 1;
    const int c_iter = tseq[test_idx].iter;
    ulong p1;
    int i;
    for (i=0, p1=1; p1; p1=p1<<1, i++) {
      movinv32(c_iter,p1, 1, 0x80000000, 0, i, my_ord);
      BAILOUT
      esp_rom_printf(".");
      movinv32(c_iter,~p1, 0xfffffffe, 0x7fffffff, 1, i, my_ord);
      BAILOUT
      esp_rom_printf(".");
    }
    esp_rom_printf("\n");
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_9
  esp_rom_printf("Do test: Random Data Sequence (test #9)\n");
  {
    const int test_idx = 9 - 1;
    const int c_iter = tseq[test_idx].iter;
    for (int i=0; i < c_iter; i++) {
      movinvr(my_ord);
      BAILOUT;
      esp_rom_printf(".");
    }
    esp_rom_printf("\n");
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

#if MEMTEST_DO_TEST_10
  esp_rom_printf("Do test: Modulo 20 check, Random pattern (test #10)\n");
  {
    const int test_idx = 10 - 1;
    const int c_iter = tseq[test_idx].iter;
    for (int j=0; j<c_iter; j++) {
      ulong p1 = rand(0);
      esp_rom_printf("#");
      for (int i=0; i<MOD_SZ; i++) {
        ulong p2 = ~p1;
        modtst(i, 2, p1, p2, my_ord);
        BAILOUT

        /* Switch patterns */
        modtst(i, 2, p2, p1, my_ord);
        BAILOUT
        esp_rom_printf(".");
      }
    }
    esp_rom_printf("\n");
  }
  if (0 != g_err_cnt)
  {
    return false;
  }
#endif

  return true;
}