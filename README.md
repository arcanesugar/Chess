A c++ chess program that runs in the console

## UI
A list of commands can be accessed with "h" or "help"\
If pieces are not displating properly, try switching to ASCII with ">>dsp"\
Commands are case sensetive and must match exactly, or they will not be recognised\
When no command is entered, the last is repeated
  
Loading a position from a fen string is supported, but currently requires editing the main.cpp file\
UCI is not supported yet

## Changelog
- 10/15/24 merged move-generation-bugfixes
    - Move generator now functional
    - UI was refactored
    - Debug was refactored
    - Move representation now only takes up 4 bytes
    - Added magic bitboards for sliding pieces
    - Perft functions added(tst, mgs)
    - Check detection no longer has to call a search
    - Knights and kings use a lookup table for move generation
    - Every kind of move is possible, and can be made from the console
    - Pieces now always print with a black font color, for visibility
    - Black pawns are no longer bolded
    - The program prints what step of setup it's at, for debug reasons
    - Added debug view
    - Added better bitboard visualisation
    - Added README
    - And probably a bunch of other stuff I forgot, because this branch isn't just move generation bugfixes, I got carried away and basically rewrote the entire program
