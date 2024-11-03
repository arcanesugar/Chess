#include "uci.h"

#include <stdio.h>
#include <string.h>

#include "../Core/types.h"

#define maxTokens 20

struct TokenizedString{
  char* string;
  int numTokens;
  int tokenStart[maxTokens];
  int tokenEnd[maxTokens];
};
typedef struct TokenizedString TokenizedString;

TokenizedString tokenise(char* string){
  TokenizedString ts;
  ts.string = string;
  bool waitingForTokenStart = true;
  bool creatingToken = false;
  ts.numTokens = 0;
  int tokenIndex = 0;
  for(int i = 0; i<strlen(string); i++){
    if(string[i] == ' ' && creatingToken){
      ts.tokenEnd[tokenIndex] = i;
      tokenIndex++;
      ts.numTokens++;
      waitingForTokenStart = true;
      creatingToken = false;
    }
    if(string[i] != ' ' && waitingForTokenStart){
      ts.tokenStart[tokenIndex] = i;
      waitingForTokenStart = false;
      creatingToken = true;
    }
  }
  if(creatingToken){
    ts.tokenEnd[tokenIndex] = strlen(string);
    ts.numTokens++;
  }
  return ts;
}

void getToken(TokenizedString ts, int token, char* target){
  if(token>ts.numTokens){
    printf("token index out of range");
  }
  int strIndex = ts.tokenStart[token];
  int targetIndex = 0;
  while(strIndex<ts.tokenEnd[token]){
    target[targetIndex] = ts.string[strIndex];
    targetIndex++;
    strIndex++;
  }
  target[targetIndex] = '\0';
}

#define MAX_INPUT_SIZE 255
void runUCI(){
  bool quit = false;
  while(!quit){
    char input[MAX_INPUT_SIZE+1];
    fgets(input,MAX_INPUT_SIZE,stdin);
    if(input[strlen(input)-1] == '\n'){
      input[strlen(input)-1] = '\0';
    }else{
      printf("[error]input does not end with newline");
    }
    TokenizedString ts = tokenise(input);
    char token[255];
    getToken(ts, 0, token);
    if(strcmp(token,"quit") == 0){
      quit = true;
      continue;
    }
    if(strcmp(token,"uci") == 0){
      printf("uciok\n");
      continue;
    }
  }
}
