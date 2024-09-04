#pragma once
typedef unsigned char byte;

// https://chess.stackexchange.com/questions/4490/maximum-possible-movement-in-a-turn#:~:text=I've%20read%20that%20for,average%20approximately%2035%20possible%20moves.
struct Move {
  byte from = 0;
  byte to = 0;
};
struct Moves {
  Move moves[218]; // 218 is the maximum number of legal moves possible from one
                   // position(source at top of file)
  int end = 0;
};