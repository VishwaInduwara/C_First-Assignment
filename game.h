#ifndef GAME_H
#define GAME_H

#include "types.h"

int rollDice(void);
void playerDice(Player *players);
void determineTurnOrder(Player *players,int turnOrder[]);
void runGame(Player *players,int turnOrder[],Square *squares);
void resolveLanding(Player *players,int currentPlayerIndex,Square *squares,int diceRoll);

#endif // GAME_H