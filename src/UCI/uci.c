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
  str->buf = realloc(str->buf,newCapacity+1);
  str->size = newCapacity+1;
  str->capacity = newCapacity;
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
  if(index>=str->capacity) resizeString(str,index+1);
  str->buf[index] = c;
}

char stringGetChar(String *str, int index){
  if(index>=str->capacity) return 0;
  return str->buf[index];
}

String createString(){
  String str;
  str.buf = malloc(sizeof(char)*8);
  str.size = sizeof(char)*8;
  str.capacity = str.size-1;
  stringSet(&str,"");
  return str;
};

void destroyString(String *str){
  free(str->buf);
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
    stringSetChar(&testString,7,'l');
    if(stringEqual(&testString,"quit")){
      quit = true;
    }
  }
  destroyString(&testString);
}
