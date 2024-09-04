#include "bitboard.h"

u64 signedShift(u64 bb, int s){//positive = left shift
  if(s>0){
    return bb<<s;
  }
  return bb>>abs(s);
}

bool getBit(u64 bb, int index){
  return (bb>>index) & (u64)1;
};
void setBit(u64 &bb, int index){
  bb |= (u64)1<<index;
};
void resetBit(u64 &bb, int index){
  bb &= ~((u64)1<<index);
};

const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
int bitScanForward(u64 bb) {
  if(bb == 0) return -1;
  const u64 debruijn64 = 0x03f79d71b4cb0a89;
  return index64[((bb ^ (bb-1)) * debruijn64) >> 58];
}

int popls1b(u64 &bb){
  int index = bitScanForward(bb);
  resetBit(bb,index);
  return index;
}
