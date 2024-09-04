#include "bitboard.h"

bool getBit(u64 bb, int index){
  return (bb>>index) & (u64)1;
};
void setBit(u64 &bb, int index){
  bb |= (u64)1<<index;
};
void resetBit(u64 &bb, int index){
  bb &= ~((u64)1)<<index;
};