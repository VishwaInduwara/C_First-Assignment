#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

//extern Player players[4]; //Array of 4 players in the game

void initPlayers(Player *players); //prototype for the function that initializes the players
int shouldBuyProperty(Player *p, Square *square);
void payTax(Player *p, int taxAmount);
int shouldTakeLoan(Player *p,int maxLoanAvailable,Square *squares);
int hasMonopoly(int playerIndex , Square *squares, PropertyGroup group);
int shouldBuild(Player *p);

#endif // PLAYERS_H