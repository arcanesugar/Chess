#pragma once
#include <iostream>

typedef unsigned char byte;
typedef unsigned long long u64;

bool getBit(u64 bb, int index);
void setBit(u64 &bb, int index);
void resetBit(u64 &bb, int index);