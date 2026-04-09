# memory-card-game

console based memory card game written in C for my CS coursework at NAU

## how it works

4x4 grid of face down cards, 8 matching pairs. flip two cards per turn and try to find matches. tracks your moves and score.

built using structs, 2D arrays, and file I/O for the save/load feature

## how to play

run the game and type the row and column of the card you want to flip (ex: `2 3`)

other commands:
- `save` - saves your current game to a file
- `load` - loads the saved game
- `quit` - exits

cards show up as `[ ]` when hidden, `(X)` when flipped, and `[X]` when matched

## building

```
make
./memory_game
```

or just compile it manually:

```
gcc -Wall -std=c11 -g -o memory_game memory_game.c
```

## stuff used

- structs for the card data
- 2D array for the board
- file I/O to save and load game state
- fisher-yates shuffle to randomize the cards

## TODO
- add a timer
- maybe add different board sizes
