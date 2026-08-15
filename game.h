#ifndef GAME_H
#define GAME_H

#include "types.h"

int rollDice(void);

void playerDice(Player *players);
void determineTurnOrder(Player *players,int turnOrder[]);
void runGame(Player *players,int turnOrder[],Square *squares,GameState *game);
void resolveLanding(Player *players,int currentPlayerIndex,Square *squares,int diceRoll,GameState *game);
int resolveJailTurn(Player *p);
void handleBankSquare(int playerIndex,Player *p,Square *squares,GameState *game);
void calculateNetWorth(Player *players,Square *squares);
void tryBuildMonopolies(Player *p , int playerIndex , Square *squares,GameState *game);
void runAuction(Player *players , Square *square,GameState *game,Square *squares);

#endif // GAME_H