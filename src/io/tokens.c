#include "tokens.h"

void createTokenList(TokenList *tl){
  tl->len = 0;
  for(int i = 0; i<MAX_TOKENS; i++){
    tl->tokens[i].buf = NULL;
  }
}

void destroyTokenList(TokenList *tl){
  tl->len = 0;
  for(int i = 0; i<MAX_TOKENS; i++){
    destroyRstr(&tl->tokens[i]);
  }
}

void tokeniseRstr(rstr *source, TokenList *tl){
  tl->len = 0;
  int tokenIndex = -1;
  bool creatingToken = false;
  bool inQuotes = false;
  for(int i = 0; i<rstrLen(source); i++){
    char c = rstrGetChar(source,i);
    if(c != ' ' && creatingToken == false){
      tokenIndex++;
      if(tl->tokens[tokenIndex].buf != NULL){
        rstrSet(&tl->tokens[tokenIndex],"");
      }else{
        tl->tokens[tokenIndex] = createRstr();
      }
      creatingToken = true;
    }
    if(creatingToken){
      if(c == ' ' && !inQuotes){
        creatingToken = false;
        continue;
      }
      if(c == '"'){
        inQuotes = !inQuotes;
        continue;
      }
      rstrAppendChar(&tl->tokens[tokenIndex],c);
    }
  }
  tl->len = tokenIndex+1;
}
