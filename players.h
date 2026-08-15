#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

//extern Player players[4]; //Array of 4 players in the game

void initPlayers(Player *players); //prototype for the function that initializes the players
int shouldBuyProperty(Player *p, Square *square);


int shouldTakeLoan(Player *p,int maxLoanAvailable,Square *squares);
int shouldRepayLoan(Player *p);
int decideRepaymentAmount(Player *p);

int hasMonopoly(int playerIndex , Square *squares, PropertyGroup group);
int shouldBuild(Player *p);
int wantsToBid(Player *p,Square *square, int bidAmount);

int decideInsurancePolicy(Player *p,Square *square);

#endif // PLAYERS_H