A c chess program that runs in the console\
\
./main cli To start in cli mode\
./main     To start in uci mode

## CLI 
A list of commands can be accessed with "hlp" or "help"\
If pieces are not displating properly, try switching to ASCII with ">>dsp"\
Commands are case sensetive\
When no command is entered, the last is repeated
  
Loading a position from a fen string is supported, but currently requires editing the main.cpp file\

## UCI
Supported commands:\
(both depth and movetime can be used, and the search with stop when the first condition is met\
but using infinite in combination with depth/movetime results in unpredictable behaviour)
- uci
- quit
- position \[fen \<fenstring\> | startpos\]  moves \<move1\> .... \<movei\>
- isready
- go \[depth \<x\> | movetime \<x\> | infinite\]
- stop

## Changelog
- uci-fixes
  - added wtime,btime and movestogo to uci
  - fixed fen strings crashing the program
- better-ui
    - Board colors in cli can be changed
    - Side to move shown under the board
    - CLI no longer supports cheating
    - UCI position command ignores invalid or illegal moves
    - MoveFromStr now checks if a move is valid
    - Mode is now determined with command line arguments instead of settings file

- UCI
    - Added the ability to cancel searches
    - Added time constrained searches
    - Added iterative deepening
    - Added actual alpha/beta pruning
    - Added i/o folder
    - Added settings.txt
    - Added tokenizer
    - Added String struct for i/o
      
- search (merged 11/1/24)
    - added basic pruning
    - switched to nmax search
    - Added psq debug command
    - Added mobility bonus
    - Added piece square tables
    - C-migration (merged 10/30/24)
        - reorganised project files
        - cleaned up headers
        - greatly improved magic number search
        - added move.h/move.c
        - added types.h/move.c
        - added movePiece function
        - removed all standard library dependencies
        - moved everything out of classes
    - added bst command (plays the "best" move)
    - added minimax search
    - added evl command (shows the evaluation)
    - added evaluation function (currently just counts material)

- move-gen-optimisation (merged 10/19/24)
    - improved castling
    - improved naming of some functions
    - removed Board.threatened
    - magic numbers store the maximum index they use, to save space
    - magic number search improved
    - magics stored in a struct
    - sch command improved
    - Split search into search, movegen, and magicman classes
    - Ui now takes in various components as pointers, to avoid having multiple copies of the same thing
    - Magic lookup now uses array instead of std::map
    - Reformatted changelog slightly

- move-generation-bugfixes (merged 10/15/24)
    - Move generator now functional
    - UI was refactored
    - Debug was refactored
    - Move representation changed to only use 4 bytes
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
