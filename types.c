#include "types.h"
#include <string.h>

void strcatchar(char *string, char c){
  char temp[2] = " ";
  temp[0] = c;
  strcat(string,temp);
}

//Move List
struct MoveList createMoveList(){
  struct MoveList ml; ml.end = 0; return ml;
}
void moveListAppend(struct MoveList *ml, Move m){
  ml->moves[ml->end] = m;
  ml->end++;
}
void moveListRemove(struct MoveList *ml, byte index){
  for (byte i = index; i < ml->end; i++)
    ml->moves[i] = ml->moves[i + 1]; // copy next element left
  ml->end-=1;
}
