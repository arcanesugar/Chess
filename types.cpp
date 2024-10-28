#include "types.h"
#include <string.h>

void strcatchar(char *string, char c){
  char temp[2] = " ";
  temp[0] = c;
  strcat(string,temp);
}

//Move List
MoveList createMoveList(){
  MoveList ml; ml.end = 0; return ml;
}
void moveListAppend(MoveList *ml, Move m){
  ml->moves[ml->end] = m;
  ml->end++;
}
void moveListRemove(MoveList *ml, byte index){
  for (byte i = index; i < ml->end; i++)
    ml->moves[i] = ml->moves[i + 1]; // copy next element left
  ml->end-=1;
}