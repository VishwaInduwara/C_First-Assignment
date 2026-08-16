#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int calculateRent(Square *landedSquare, Square *squares, int diceRoll,GameState *game);      //finance.c:11
int checkBankruptcy(Player *p,int currentPlayerIndex,Square *squares);                        //finance.c:175

void payTax(Player *p, int taxAmount,int currentPlayerIndex,Square *squares,GameState *game); //finance.c:215
int calculateMaxLoan(int playerIndex,Square *squares,GameState *game);                        //finance.c:236

int mortgageValue(Square *square, GameState *game);                       //LK-31/32 boom/decline :255
int findMortgageableProperty(int playerIndex, Square *squares, GameState *game); //best index or -1 :265
void raiseCashByMortgage(Player *p, int playerIndex, Square *squares, GameState *game, int amount); //:279

void repayLoan(Player *p, int repaymentAmount,Square *squares,int playerIndex);                //finance.c:293
void extendLoanTurn(Player *p);                                                               //finance.c:325
void increaseLoanAmount(Player *p, int playerIndex, Square *squares,GameState *game);         //finance.c:331

int canBuildEvenly(Square *squares , PropertyGroup group ,  int propertyIndex);                //finance.c:345
void buildHouse(Player *p , int playerIndex ,Square *squares,Square *square,GameState *game); //finance.c:356
void buildHotel(Player *p , int playerIndex, Square *square,GameState *game,Square *squares); //finance.c:405
int buildingConditionRentFactor(int condition);            //Rule-LK 26 Table 3 :449

int maintenanceCost(Square *square);                       //Rule-LK 27 (+50% if damaged) :457
void maintainProperty(Player *p, int playerIndex, Square *squares, Square *square); //Rule-LK 27 :470

void renovateDamagedBuilding(Player *p, int playerIndex, Square *squares, Square *square); //Rule-LK 29 :485

int currentMarketValue(Square *square);                    //Rule-LK 16: market price minus depreciation :514
int totalPropertyAssets(int playerIndex, Square *squares); //sum of current market value of owned properties :524

int marketTaxRate(GameState *game, int baseRate);          //base rate + boom/decline + inflation :534

void renovateProperty(Player *p, int playerIndex, Square *squares, Square *square); //Rule-LK 17 :545
void calculateInflation(Square *squares,int currentRound,GameState *game);                   //finance.c:565
void purchaseInsurance(Player *p, Square *square, int policyChoice,GameState *game);         //finance.c:585
#endif