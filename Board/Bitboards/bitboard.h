#pragma once

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <math.h>

typedef unsigned long long u64;
char getBit(u64 bb, int index);
void setBit(u64 *bb, int index);
void resetBit(u64 *bb, int index);

int bitScanForward(u64 bb);
int popls1b(u64 *bb);
u64 signedShift(u64 bb, int s);
int bitcount(u64 bb);
#ifdef __cplusplus
}
#endif
