#ifndef GAME_H
#define GAME_H

#include "types.h"

int rollDice(void);//game.c:13

void playerDice(Player *players);//game.c:23

void determineTurnOrder(Player *players,int turnOrder[]);//game.c:44

void handleBankSquare(int playerIndex,Player *p,Square *squares,GameState *game); //game.c:104

void runAuction(Player *players , Square *square,GameState *game,Square *squares); //game.c:158

void resolveLanding(Player *players,int currentPlayerIndex,Square *squares,int diceRoll,GameState *game); //game.c:237

void calculateNetWorth(Player *players,Square *squares);//game.c:354

int resolveJailTurn(Player *p, int playerIndex, Square *squares, GameState *game); //game.c:377

void tryBuildMonopolies(Player *p , int playerIndex , Square *squares,GameState *game); //game.c:419

void runGame(Player *players,int turnOrder[],Square *squares,GameState *game);      //game.c:450

#endif // GAME_H