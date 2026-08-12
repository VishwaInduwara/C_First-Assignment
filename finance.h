#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int calculateRent(Square *landedSquare, Square *squares, int diceRoll);
int checkBankruptcy(Player *p);
int calculateMaxLoan(int playerIndex,Square *squares);
int canBuildEvenly(Square *squares , PropertyGroup group ,  int propertyIndex);
void buildHouse(Player *p , int playerIndex ,Square *squares,Square *square);
void buildHotel(Player *p , int playerIndex, Square *square);
#endif