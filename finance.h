#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int calculateRent(Square *landedSquare, Square *squares, int diceRoll,GameState *game);
int checkBankruptcy(Player *p,int currentPlayerIndex,Square *squares);

void payTax(Player *p, int taxAmount,int currentPlayerIndex,Square *squares,GameState *game);

int calculateMaxLoan(int playerIndex,Square *squares,GameState *game);
void repayLoan(Player *p, int repaymentAmount,Square *squares,int playerIndex);
void increaseLoanAmount(Player *p, int playerIndex, Square *squares,GameState *game);
void extendLoanTurn(Player *p);

int canBuildEvenly(Square *squares , PropertyGroup group ,  int propertyIndex);
void buildHouse(Player *p , int playerIndex ,Square *squares,Square *square,GameState *game);
void buildHotel(Player *p , int playerIndex, Square *square,GameState *game,Square *squares);
void calculateInflation(Square *squares,int currentRound,GameState *game);
void purchaseInsurance(Player *p, Square *square, int policyChoice,GameState *game);

int currentMarketValue(Square *square);                        //Rule-LK 16: market price minus depreciation
void renovateProperty(Player *p, int playerIndex, Square *squares, Square *square); //Rule-LK 17

int buildingConditionRentFactor(int condition);            //Rule-LK 26 Table 3
int maintenanceCost(Square *square);                       //Rule-LK 27 (+50% if damaged)
void maintainProperty(Player *p, int playerIndex, Square *squares, Square *square); //Rule-LK 27
void renovateDamagedBuilding(Player *p, int playerIndex, Square *squares, Square *square); //Rule-LK 29

int mortgageValue(Square *square, GameState *game);                       //LK-31/32 boom/decline
int findMortgageableProperty(int playerIndex, Square *squares, GameState *game); //best index or -1
void raiseCashByMortgage(Player *p, int playerIndex, Square *squares, GameState *game, int amount);
void unmortgageProperty(Player *p, int playerIndex, Square *squares, GameState *game);

int totalPropertyAssets(int playerIndex, Square *squares); //sum of current market value of owned properties (no buildings)
int marketTaxRate(GameState *game, int baseRate);          //base rate + boom/decline + inflation

#endif