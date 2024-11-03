#include "uci.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Core/types.h"

struct String{// captilalized because its a user defined data type
  char* buf;
  size_t size;
  int capacity;//the number of characters the string can hold (size-1)
};
typedef struct String String;

void resizeString(String *str, int newCapacity){
  str->size = sizeof(char)*(newCapacity+1);
  str->buf = realloc(str->buf,str->size);
  str->capacity = newCapacity;
  str->buf[str->capacity] = '\0';//just in case
}

int stringLen(String *str){
  return strlen(str->buf);
}

void stringSet(String *str, const char *cstr){
  if(strlen(cstr)>str->capacity){
    resizeString(str,strlen(cstr));
  }
  strcpy(str->buf,cstr);
}

char* stringGetCstr(String *str){
  return str->buf;
}

void stringAppend(String *str, const char *cstr){
  int newSize = strlen(str->buf)+strlen(cstr);
  if(newSize>str->capacity){
    resizeString(str,newSize);
  }
  strcat(str->buf,cstr);
}

bool stringEqual(String *str, const char *cstr){
  return (strcmp(str->buf,cstr) == 0);
}

void stringSetChar(String *str, int index, char c){
  if(index>=str->capacity) resizeString(str,index+1);
  str->buf[index] = c;
}

void stringAppendChar(String *str, char c){
  int index = stringLen(str);
  stringSetChar(str,index,c);
  stringSetChar(str,index+1,'\0');
}

char stringGetChar(String *str, int index){
  if(index>=str->capacity){printf("Index out of range\n"); return 0;}
  return str->buf[index];
}

String createString(){
  String str;
  str.buf = NULL;
  resizeString(&str,8);
  stringSet(&str,"");
  return str;
}

void destroyString(String *str){
  free(str->buf);
}

struct TokenList{
  String tokens[50];
  int len;
};
typedef struct TokenList TokenList;

void createTokenList(String *source, TokenList *tl){
  tl->len = 0;
  int tokenIndex = 0;
  tl->tokens[0] = createString();
  printf("%s",tl->tokens[0].buf);
  bool creatingToken = false;
  for(int i = 0; i<stringLen(source); i++){
    char c = stringGetChar(source,i);
    if(c != ' ') creatingToken = true;
    if(creatingToken){
      if(c == ' '){
        tokenIndex++;
        tl->tokens[tokenIndex] = createString();
        creatingToken = false;
      }else{
        stringAppendChar(&tl->tokens[tokenIndex],c);
      }
    }
  }
  tl->len = tokenIndex+1;
}

void stringFromStdin(String *str){
  stringSet(str,"");
  char buf[8];
  while(fgets(buf,8,stdin) != NULL){
    stringAppend(str,buf);
    if(buf[strlen(buf)-1] == '\n') break;
  }
  stringSetChar(str,stringLen(str)-1,'\0');//remove newline character
}

void runUCI(){
  String testString = createString();
  bool quit = false;
  while(!quit){
    stringFromStdin(&testString);
    TokenList tl;
    createTokenList(&testString,&tl);
    printf("\ntokens:");
    for(int i = 0; i<tl.len; i++){
      printf("%s;", tl.tokens[i].buf);
    }
    printf("\ntestString:%s\n",testString.buf);
    if(stringEqual(&testString,"quit")){
      quit = true;
    }
  }
  destroyString(&testString);
}
