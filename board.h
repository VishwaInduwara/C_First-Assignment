#ifndef BOARD_H
#define BOARD_H

#include "types.h"

void initBoard(GameBoard *board);//prototype for the function that initializes the board
//this function exists,here's how to call it
//game.c need realtime update of the board, so we need to pass the board as a pointer to the functions that modify it

#endif // BOARD_H