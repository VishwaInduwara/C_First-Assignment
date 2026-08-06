#ifndef BOARD_H
#define BOARD_H

#include "types.h"
//extern Square squares[40]; //Array of 40 squares on the board

void initBoard(Square *squares);//prototype for the function that initializes the board
//this function exists,here's how to call it
//game.c need realtime update of the board, so we need to pass the board as a pointer to the functions that modify it

void movePlayer(Player *players, int currentPlayerIndex,int diceRoll);

#endif // BOARD_H
