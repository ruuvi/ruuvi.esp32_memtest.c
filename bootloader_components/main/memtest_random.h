#ifndef MEMTEST_RANDOM_H
#define MEMTEST_RANDOM_H

unsigned long memtest_rand(int cpu);

void memtest_rand_seed( unsigned int seed1, unsigned int seed2, int cpu);

#define rand(cpu) memtest_rand(cpu)
#define rand_seed(seed1, seed2, cpu) memtest_rand_seed(seed1, seed2, cpu)

#endif // MEMTEST_RANDOM_H
