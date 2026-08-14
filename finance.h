#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int calculateRent(Square *landedSquare, Square *squares, int diceRoll,GameState *game);
int checkBankruptcy(Player *p,int currentPlayerIndex,Square *squares);

void payTax(Player *p, int taxAmount,int currentPlayerIndex,Square *squares);

int calculateMaxLoan(int playerIndex,Square *squares,GameState *game);
void repayLoan(Player *p, int repaymentAmount,Square *squares,int playerIndex);
void increaseLoanAmount(Player *p, int playerIndex, Square *squares,GameState *game);
void extendLoanTurn(Player *p);

int canBuildEvenly(Square *squares , PropertyGroup group ,  int propertyIndex);
void buildHouse(Player *p , int playerIndex ,Square *squares,Square *square,GameState *game);
void buildHotel(Player *p , int playerIndex, Square *square,GameState *game,Square *squares);
void calculateInflation(Square *squares,int currentRound,GameState *game);
#endif