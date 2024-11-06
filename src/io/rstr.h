#pragma once
#include <stdlib.h>

#include "../Core/types.h"

struct rstr{//resizable string
  char* buf;
  size_t size;
  int capacity;//the number of characters the rstr can hold (size-1)
};
typedef struct rstr rstr;

rstr createRstr();
void destroyRstr(rstr *str);

int rstrLen(rstr *str);
void rstrSet(rstr *str, const char *cstr);
char* rstrGetCstr(rstr *str);
void rstrAppend(rstr *str, const char *cstr);
bool rstrEqual(rstr *str, const char *cstr);
void rstrSetChar(rstr *str, int index, char c);
void rstrAppendChar(rstr *str, char c);
char rstrGetChar(rstr *str, int index);//returns 0 if the index is out of range
