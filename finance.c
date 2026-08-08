#include <stdio.h>
#include "types.h"

// Rent Calculation
int calculateRent(Square *landedSquare, Square *squares, int diceRoll) {
    int rent = 0;

    if (landedSquare->type == SQ_PROPERTY) {
        rent = landedSquare->baseRent;

    } else if (landedSquare->type == SQ_RAILWAY) {
        int railwayCount = 0;
        int ownerIndex = landedSquare->owner;

        for (int j = 0; j < BOARD_SIZE; j++) {
            if (squares[j].type == SQ_RAILWAY && squares[j].owner == ownerIndex) {
                railwayCount++;
            }
        }
        if (railwayCount == 1){
            rent = 250;
        }else if (railwayCount == 2){
            rent = 500;
        }else if (railwayCount == 3){
            rent = 1000;
        }else if (railwayCount == 4){
            rent = 2000;
        }

    } else if (landedSquare->type == SQ_UTILITY) {
        int utilityCount = 0;
        int ownerIndex = landedSquare->owner;
        for (int k = 0; k < BOARD_SIZE; k++) {
            if (squares[k].type == SQ_UTILITY && squares[k].owner == ownerIndex) {
                utilityCount++;
            }
        }
        if (utilityCount == 1){
            rent = diceRoll * 4;
        }
        else if (utilityCount == 2){
            rent = diceRoll * 10;
        }
    }

    return rent;
}


int checkBankruptcy(Player *p) {
    if (p->cash < 0) {
        p->isBankrupt = 1;
        printf("%s has been declared bankrupt.\n", p->name);
        // TODO: Rule 14 - remove buildings, expire insurance, loans due immediately, transfer remaining assets
        return 1;
    }
    return 0;
}

// finance.c
void payTax(Player *p, int taxAmount) {
    printf("%s landed on Income Tax.\n", p->name);

    if (p->cash >= taxAmount) {
        p->cash -= taxAmount;

        printf("Paid Income Tax: LKR %d.\n", taxAmount);
        printf("Remaining Balance : LKR %d.\n", p->cash);

    } else {
        printf("%s cannot afford Income Tax!\n", p->name);
    }

    checkBankruptcy(p);
}