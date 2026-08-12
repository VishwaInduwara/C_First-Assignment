#include <stdio.h>
#include "types.h"

// Rent Calculation
int calculateRent(Square *landedSquare, Square *squares, int diceRoll) {
    int rent = 0;

    if (landedSquare->type == SQ_PROPERTY) {
        if(landedSquare -> hasHotel){
            rent = 10*(landedSquare -> baseRent);
        }else{
            switch (landedSquare -> numHouses){
                case 1:
                    rent = 2*(landedSquare -> baseRent);
                case 2:
                    rent = 3*(landedSquare -> baseRent);
                case 3:
                    rent = 5*(landedSquare -> baseRent);
                case 4:
                    rent = 7*(landedSquare -> baseRent);
                default:
                    rent = landedSquare -> baseRent;
            }
        }
        

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

int calculateMaxLoan(int playerIndex,Square *squares){
    int totalMortgageValue = 0;
//Must be owned by player and not currently mortgaged 
    for (int i=0 ; i<BOARD_SIZE ; i++){
         if(squares[i].owner == playerIndex && !squares[i].isLoanLocked){
            totalMortgageValue += squares[i].baseMortgageValue;
        }
    }

    //75% of eligible property collateral
    return (totalMortgageValue * 75)/100;
}

//Helper function to build house to check can build houses
//Rule 9
int canBuildEvenly(Square *squares , PropertyGroup group ,  int propertyIndex){
    for(int i = 0; i<BOARD_SIZE; i++){
        if(squares[i].group == group && squares[i].propertytype == PropertyType_Regular){
            if(squares[i].numHouses < squares[propertyIndex].numHouses){
                return 0;//another property in the group has fewer , so can't build
            }
        }
    }
    return 1;//This property has fewest house , Can build
}

void buildHouse(Player *p , int playerIndex ,Square *squares,Square *square){ //square -> because we need one square details, it will pass when fuction call
    printf("Build houses function called\n\n");
    if(square -> numHouses >= 4 || square -> hasHotel == 1){
        return;
    }
    if(p -> cash < square -> baseHouseCost){
        return;
    }
    if(!canBuildEvenly(squares , square->group ,square->index)){
        return;//Rule 9 , must build evenly in property group
    }

    p -> cash -= square -> baseHouseCost;
    square -> numHouses++;
    printf("Build houses called\n\n");

    printf("%s construct one house on %s.\n", p->name , square->name);
    printf("Construction Cost : LKR %d\n",square->baseHouseCost);
}

void buildHotel(Player *p , int playerIndex, Square *square){
     printf("Build hotels function called\n\n");
    if(square -> numHouses !=4 || square -> hasHotel == 1){
        return; //Rule 10 - player need exactly 4 houses first to build a hotel
    }
    if(p -> cash < square -> baseHotelCost){
        return;
    }

    p -> cash -= square -> baseHotelCost;
    square -> numHouses = 0;
    square -> hasHotel = 1;
    printf("Build hotels called\n\n");

    p -> numHotelCount++;

    printf("%s upgraded %s to a Hotel",p->name , square->name);
}