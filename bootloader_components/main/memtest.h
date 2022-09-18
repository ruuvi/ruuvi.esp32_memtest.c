#ifndef MAIN_MEMTEST_H
#define MAIN_MEMTEST_H

#include <stdbool.h>

extern int __sram0_begin;
extern int __sram0_end;

extern int __sram1_begin;
extern int __sram1_end;

extern int __sram2_begin;
extern int __sram2_end;

void memtest_init(void);

bool do_test(void);

#endif // MAIN_MEMTEST_H
