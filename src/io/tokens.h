#pragma once
#include "rstr.h"

#define MAX_TOKENS 50
struct TokenList{
  rstr tokens[MAX_TOKENS];
  int len;
};
typedef struct TokenList TokenList;

void tokeniseRstr(rstr *source, TokenList *tl);
void destroyTokenList(TokenList *tl);
void createTokenList(TokenList *tl);
