#include "types.h"
#include <string.h>

char getPieceColor(char piece) {return (piece>=6)? BLACK:WHITE;}

void strcatchar(char *string, char c){
  char temp[2] = " ";
  temp[0] = c;
  strcat(string,temp);
}
