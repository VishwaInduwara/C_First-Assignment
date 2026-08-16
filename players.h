#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"



void initPlayers(Player *players); //prototype for the function that initializes the players
int shouldBuyProperty(Player *p, Square *square);


int shouldTakeLoan(Player *p,int maxLoanAvailable,Square *squares);
int shouldRepayLoan(Player *p);
int decideRepaymentAmount(Player *p);

int hasMonopoly(int playerIndex , Square *squares, PropertyGroup group);
int shouldBuild(Player *p,int isHotel);
int wantsToBid(Player *p,Square *square, int bidAmount);

int decideInsurancePolicy(Player *p,Square *square);

int shouldRenovate(Player *p, int depreciationPct);

int shouldMaintain(Player *p, int condition);

int shouldPayBail(Player *p); //Rule 13: decide whether to pay LKR 300 bail

#endif // PLAYERS_H