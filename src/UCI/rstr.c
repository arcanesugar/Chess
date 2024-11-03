#include "rstr.h"

#include <string.h>
#include <stdlib.h>

void rstrResize(rstr *str, int newCapacity){
  str->size = sizeof(char)*(newCapacity+1);
  str->buf = realloc(str->buf,str->size);
  str->capacity = newCapacity;
  str->buf[str->capacity] = '\0';//just in case
}

rstr createRstr(){
  rstr str;
  str.buf = NULL;
  rstrResize(&str,8);
  rstrSet(&str,"");
  return str;
}

void destroyRstr(rstr *str){
  free(str->buf);
}

int rstrLen(rstr *str){
  return strlen(str->buf);
}

void rstrSet(rstr *str, const char *cstr){
  if(strlen(cstr)>str->capacity){
    rstrResize(str,strlen(cstr));
  }
  strcpy(str->buf,cstr);
}

char* rstrGetCstr(rstr *str){
  return str->buf;
}

void rstrAppend(rstr *str, const char *cstr){
  int newSize = strlen(str->buf)+strlen(cstr);
  if(newSize>str->capacity){
    rstrResize(str,newSize);
  }
  strcat(str->buf,cstr);
}

bool rstrEqual(rstr *str, const char *cstr){
  return (strcmp(str->buf,cstr) == 0);
}

void rstrSetChar(rstr *str, int index, char c){
  if(index>=str->capacity) rstrResize(str,index+1);
  str->buf[index] = c;
}

void rstrAppendChar(rstr *str, char c){
  int index = rstrLen(str);
  rstrSetChar(str,index,c);
  rstrSetChar(str,index+1,'\0');
}

char rstrGetChar(rstr *str, int index){
  if(index>=str->capacity) return 0;
  return str->buf[index];
}


