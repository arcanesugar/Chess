#include "uci.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Core/types.h"

#define maxTokens 20

struct String{// captilalized because its a user defined data type
  char* buf;
  size_t size;
  int len;//the number of characters the string can hold (size-1)
};
typedef struct String String;

void resizeString(String *str, int newCapacity){
  int reqBlocks = (newCapacity/8)+1;
  size_t newSize = reqBlocks*8;
  str->buf = realloc(str->buf,newSize);
  str->size = newSize;
  str->len = newSize-1;
}

void stringSet(String *str, const char *cstr){
  if(strlen(cstr)>str->len){
    resizeString(str,strlen(cstr));
  }
  strcpy(str->buf,cstr);
}

void stringAppend(String *str, const char *cstr){
  int newSize = strlen(str->buf)+strlen(cstr);
  if(newSize>str->len){
    resizeString(str,newSize);
  }
  strcat(str->buf,cstr);
}
bool stringEqual(String *str, const char *cstr){
  return (strcmp(str->buf,cstr) == 0);
}
String createString(){
  String str;
  str.buf = malloc(sizeof(char)*8);
  str.size = sizeof(char)*8;
  str.len = str.size-1;
  stringSet(&str,"");
  return str;
};

void destroyString(String *str){
  free(str->buf);
}

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

void stringFromStdin(String *str){
  stringSet(str,"");
  char buf[8];
  while(fgets(buf,8,stdin) != NULL){
    stringAppend(str,buf);
    if(buf[strlen(buf)-1] == '\n') break;//we have reached the end of the line
  }
}

void runUCI(){
  String testString = createString();
  bool quit = false;
  while(!quit){
    stringFromStdin(&testString);
    printf("%d;%s",testString.len,testString.buf);
    if(stringEqual(&testString,"quit\n")){
      quit = true;
    }
  }
  destroyString(&testString);
}
