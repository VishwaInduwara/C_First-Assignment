#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "types.h"


// Rent Calculation
int calculateRent(Square *landedSquare, Square *squares, int diceRoll,GameState *game) {
    int rent = 0;

    if (landedSquare->type == SQ_PROPERTY) {
        if(landedSquare -> hasHotel){
            rent = 10*(landedSquare -> baseRent);
        }else{
            switch (landedSquare -> numHouses){
                case 1:
                    rent = 2*(landedSquare -> baseRent);
                    break;
                case 2:
                    rent = 3*(landedSquare -> baseRent);
                    break;
                case 3:
                    rent = 5*(landedSquare -> baseRent);
                    break;
                case 4:
                    rent = 7*(landedSquare -> baseRent);
                    break;
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
    if(landedSquare -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        rent = (rent *125)/100; //Rule LK 31
    }else if(landedSquare -> group == game -> declinedGroup && game -> declineRoundsRemaining > 0){
        rent = (rent *80)/100; //Rule LK 32
    }

    return rent;
}


int checkBankruptcy(Player *p,int playerIndex,Square *squares){
    if (p->cash < 0) {
        p->isBankrupt = 1;
        printf("%s has been declared bankrupt.\n", p->name);
        
        //Rule 14 - remove buildings, expire insurance, loans due immediately, transfer remaining assets
        for(int i = 0; i < BOARD_SIZE; i++) {
            if(squares[i].owner == playerIndex) {
                squares[i].owner = -1; // Return to bank
                squares[i].numHouses = 0; // Remove houses
                squares[i].hasHotel = 0; // Remove hotel
                squares[i].insurancePolicyType = None_Insurance; // Expire insurance
                squares[i].insuranceRoundsRemaining = 0; // Reset insurance rounds
                squares[i].isLoanLocked = 0; // Unlock property from loan
            }
        }

        //Loan become void
        p->hasActiveLoan = 0;
        p->loanAmount = 0;
        p->loanTurnsRemaining = 0;
        p->numOwnedProperties = 0;

        printf("All buildings removed, insurance expired, properties transferred to bank.\n");
        return 1; // Player is bankrupt
    }
    return 0;
}



void payTax(Player *p, int taxAmount,int currentPlayerIndex,Square *squares) {
    printf("%s landed on Income Tax.\n", p->name);

    if (p->cash >= taxAmount) {
        p->cash -= taxAmount;

        printf("Paid Income Tax: LKR %d.\n", taxAmount);
        printf("Remaining Balance : LKR %d.\n", p->cash);

    } else {
        printf("%s cannot afford Income Tax!\n", p->name);
    }

    checkBankruptcy(p,currentPlayerIndex,squares);
}

int calculateMaxLoan(int playerIndex,Square *squares,GameState *game){
    int totalMortgageValue = 0;
//Must be owned by player and not currently mortgaged 
    for (int i=0 ; i<BOARD_SIZE ; i++){
         if(squares[i].owner == playerIndex && !squares[i].isLoanLocked){
            if(squares[i].group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
                totalMortgageValue += (squares[i].baseMortgageValue * 115)/100; //Rule LK 31
            }else if(squares[i].group == game -> declinedGroup && game -> declineRoundsRemaining > 0){
                totalMortgageValue += (squares[i].baseMortgageValue * 90)/100; //Rule LK 32
            }else{
                totalMortgageValue += squares[i].baseMortgageValue;
            }
        }
    }

    //75% of eligible property collateral
    return (totalMortgageValue * 75)/100;
}

void repayLoan(Player *p, int repaymentAmount,Square *squares,int playerIndex){
    //repaymentAmoount comes from players.c strategy function
    if(!p -> hasActiveLoan){
        printf("%s has no active loan to repay.\n",p -> name);
        return;
    }

    if(repaymentAmount > p -> cash){
        repaymentAmount = p -> cash; //Repay as much as possible
    }
    p -> cash -= repaymentAmount;
    p -> loanAmount -= repaymentAmount;

    printf("%s repaid LKR %d towards the loan.\n",p -> name,repaymentAmount);
    printf("Remaining Loan Amount: LKR %d\n",p -> loanAmount);

    if(p -> loanAmount <= 0){
        p -> loanAmount = 0;
        p -> hasActiveLoan = 0;
        p -> loanTurnsRemaining = 0;

        //Unlock all properties that were locked for the loan
        for(int i=0 ; i<BOARD_SIZE ; i++){
            if(squares[i].owner == playerIndex){
                squares[i].isLoanLocked = 0;
            }
        }
        printf("%s has fully repaid the loan and all properties are unlocked.\n",p -> name);
    }
}

//This will call in handelBankSquare in game.c when player decide to do it in players.c
void extendLoanTurn(Player *p){
    p -> loanTurnsRemaining = 20; //Reset to 20 turns
    printf("%s has extended the loan repayment period to 20 turns.\n",p -> name);
}

//This will call in handelBankSquare in game.c when player decide to do it in players.c
void increaseLoanAmount(Player *p, int playerIndex, Square *squares,GameState *game){
    int maxLoan = calculateMaxLoan(playerIndex,squares,game);
    if(maxLoan > p->loanAmount){
        int increaseAmount = maxLoan - p->loanAmount;
        p->cash += increaseAmount;
        p->loanAmount = maxLoan;
        printf("%s has increased the loan amount by LKR %d. New loan amount: LKR %d\n", p->name, increaseAmount, p->loanAmount);
    }else{
        printf("%s has no additional collateral to increase the loan amount.\n", p->name);
    }
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

void buildHouse(Player *p, int playerIndex, Square *squares, Square *square, GameState *game){
    printf("%s is attempting to build a house on %s.\n", p->name, square->name);
    if(square -> numHouses >= 4 || square -> hasHotel == 1){
        return;
    }

    int actualCost = square -> baseHouseCost;
    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        actualCost = (square -> baseHouseCost * 110) / 100;
    }

    if(p -> cash < actualCost){
        return;
    }
    if(!canBuildEvenly(squares, square->group, square->index)){
        return;
    }

    p -> cash -= actualCost;
    square -> numHouses++;

    checkBankruptcy(p, playerIndex, squares);   // ADD THIS

    printf("%s construct one house on %s.\n", p->name, square->name);
    printf("Construction Cost : LKR %d\n", actualCost);
}

void buildHotel(Player *p, int playerIndex, Square *square, GameState *game,Square *squares){
    if(square -> numHouses != 4 || square -> hasHotel == 1){
        return;
    }

    int actualCost = square -> baseHotelCost;
    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        actualCost = (square -> baseHotelCost * 110) / 100;
    }

    if(p -> cash < actualCost){
        return;
    }

    p -> cash -= actualCost;
    square -> numHouses = 0;
    square -> hasHotel = 1;
    p -> numHotelCount++;

    checkBankruptcy(p, playerIndex, squares);  

    printf("%s upgraded %s to a Hotel", p->name, square->name);
}
/*void buildHouse(Player *p , int playerIndex ,Square *squares,Square *square,GameState *game){ //square -> because we need one square details, it will pass when fuction call
    //printf("Build houses function called\n\n");
    if(square -> numHouses >= 4 || square -> hasHotel == 1){
        return;
    }
    if(p -> cash < square -> baseHouseCost){
        return;
    }
    if(!canBuildEvenly(squares , square->group ,square->index)){
        return;//Rule 9 , must build evenly in property group
    }

    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        p -> cash -= (square -> baseHouseCost * 110)/100; //Rule LK 31
    }else{
        p -> cash -= square -> baseHouseCost;
    }
    square -> numHouses++;
    printf("Build houses called\n\n");

    printf("%s construct one house on %s.\n", p->name , square->name);
    printf("Construction Cost : LKR %d\n",square->baseHouseCost);
}*/

/*void buildHotel(Player *p , int playerIndex, Square *square,GameState *game){ //square -> because we need one square details, it will pass when fuction call
    //printf("Build hotels function called\n\n");
    if(square -> numHouses !=4 || square -> hasHotel == 1){
        return; //Rule 10 - player need exactly 4 houses first to build a hotel
    }
    if(p -> cash < square -> baseHotelCost){
        return;
    }

    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        p -> cash -= (square -> baseHotelCost * 110)/100; //Rule LK 31
    }else{
        p -> cash -= square -> baseHotelCost;
    }
    square -> numHouses = 0;
    square -> hasHotel = 1;
    printf("Build hotels called\n\n");

    p -> numHotelCount++;

    printf("%s upgraded %s to a Hotel",p->name , square->name);
}*/

//get random inflation
void calculateInflation(Square *squares,int currentRound,GameState *game){
    float inflation[6]={-0.03,00.0,0.02,0.05,0.08,0.12};

    float InflationRate = inflation[rand()%6];
    if(InflationRate>=0.08){
        game -> currentLoanInterest = 0.12;
    }else{
        game -> currentLoanInterest = 0.1;
    }
    game -> currentInflactionRate = InflationRate;
    
    for(int i=0 ; i<BOARD_SIZE;i++){
        squares[i].marketPrice = (int)((squares[i].marketPrice)*(1+InflationRate));
    }
}
