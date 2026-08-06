#ifndef GAME_H
#define GAME_H

#include "types.h"

int rollDice(void);
void playerDice(Player *players);
void determineTurnOrder(Player *players,int turnOrder[]);
void runGame(Player *players,int turnOrder[]);

#endif // GAME_H