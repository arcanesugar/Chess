#include "types.h"
#include <string.h>

void strcatchar(char *string, char c){
  char temp[2] = " ";
  temp[0] = c;
  strcat(string,temp);
}
