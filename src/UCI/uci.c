#include "uci.h"

#include <stdio.h>
#include <string.h>

#include "../Core/types.h"

#define maxTokens 20//maybe enough
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
void runUCI(){
  char* testString = "word1 word2  word3    word4           word5";
  TokenizedString ts = tokenise(testString);
  for(int i = 0; i<ts.numTokens; i++){
    char token[255];
    getToken(ts,i,token);
    printf("%s;\n",token);
  }
}
