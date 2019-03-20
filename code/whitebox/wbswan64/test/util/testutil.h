#include<stdio.h>
#include<stdint.h>

uint64_t start_rdtsc();
uint64_t end_rdtsc();

void dump(const uint8_t *li, int len);