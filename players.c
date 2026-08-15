#include <string.h>
#include <stdio.h>
#include "players.h"

//Player players[MAX_PLAYERS];

void initPlayers(Player *players){
    for(int i = 0; i < MAX_PLAYERS; i++){
        // Initialize player attributes
        players[i].index = i;
        players[i].position = 0; // Start at the beginning of the board
        players[i].cash = 30000; // Starting cash balance

        players[i].numOwnedProperties = 0; // No properties owned initially
        players[i].numHotelCount = 0;
        players[i].inJail = 0; // Not in jail
        players[i].jailTurnsRemaining = 0; // No jail turns remaining
        
        players[i].hasActiveLoan = 0; // No active loan
        players[i].hasMonopoly = 0;
        
        players[i].isRoundCompleted = 0; // Round not completed
        players[i].isBankrupt = 0; // Not bankrupt
        players[i].loanAmount = 0;
        players[i].loanTurnsRemaining = 0;
        players[i].netWorth =0;
        players[i].totalPropertyValue=0;

        // Initialize owned properties array to -1 (indicating no properties owned)
        for(int j = 0; j < 28; j++){
            players[i].ownedProperties[j] = -1;
        }
    }

    //Player 1: Aggressive Investor
    strcpy(players[0].name, "Aggressive Investor");
    players[0].strategy = STRATEGY_AGGRESSIVE_INVESTOR;

    //Player 2: Conservative Banker
    strcpy(players[1].name, "Conservative Banker");
    players[1].strategy = STRATEGY_CONSERVATIVE_BANKER;

    //Player 3: Risk Averse
    strcpy(players[2].name, "Risk Taker");
    players[2].strategy = STRATEGY_RISK_TAKER;

    //Player 4: Opportunist
    strcpy(players[3].name, "Opportunist Trader");
    players[3].strategy = STRATEGY_OPPORTUNISTIC_TRADER;
}

int shouldBuyProperty(Player *p,Square *square){ //p and square because we need only one player details
    switch (p->strategy){
         case STRATEGY_AGGRESSIVE_INVESTOR:
            // "Always purchases an unowned property if sufficient funds remain to pay at least one future rent."
            return (p->cash - square->marketPrice) >= square->baseRent;

        case STRATEGY_CONSERVATIVE_BANKER:
            // "Purchases properties only if at least 50% of current cash remains after purchase."
            return (p->cash - square->marketPrice) >= (p->cash / 2);

        case STRATEGY_RISK_TAKER:
            // "Purchases every available property whenever legally possible."
            return p->cash >= square->marketPrice;

        case STRATEGY_OPPORTUNISTIC_TRADER:
            // "Purchases properties only when projected appreciation exceeds construction costs."
            // (simplify for now - refine later once you have appreciation logic)
            return p->cash >= square->marketPrice;

        default:
            return 0;
    }
}

int shouldTakeLoan(Player *p,int maxLoanAvailable,Square *squares){
    if(p -> hasActiveLoan || maxLoanAvailable <=0){
        return 0;//Each player can hold at most 1 active loan
    }
    switch(p -> strategy){
        case STRATEGY_AGGRESSIVE_INVESTOR:
            if(!squares[p -> position].hasHotel == 1 && p -> hasMonopoly ==1){
                return 1;
            }else{
                return(p -> cash < LOAN_THRESHOLD_AGGRESSIVE);
            }

        case STRATEGY_CONSERVATIVE_BANKER:
            return (p -> cash < LOAN_THRESHOLD_CONSERVATIVE);

        case STRATEGY_RISK_TAKER:
            return 1;

        case STRATEGY_OPPORTUNISTIC_TRADER:
            return (p -> cash < LOAN_THRESHOLD_OPPORTUNISTIC);

        default:
            return 0;
    }
}


// Returns: 0 = repay
//          1 = extend
//          2 = increase loan amount
//          3 = do nothing
int shouldRepayLoan(Player *p){
    if(!p -> hasActiveLoan){
        return 0;
    }
    switch(p -> strategy){
        case STRATEGY_AGGRESSIVE_INVESTOR:
            if(p -> cash > (p -> loanAmount * 2)){
                return 0; //
            }
            return 2; //Increase loan amount if cash is less than double the loan amount

        case STRATEGY_CONSERVATIVE_BANKER:
            if(p -> cash > (p -> loanAmount)){
                return 0; //Repay full loan if cash is more than the loan amount
            }
            return 1;

        case STRATEGY_RISK_TAKER:
            return 2;//Always try to increase Loan amount

        case STRATEGY_OPPORTUNISTIC_TRADER:
            if(p -> cash > (p -> loanAmount)){
                return 0; //Repay full loan if cash is more than the loan amount
            }
            return 1;

        default:
            return 0;
    }
}

int decideRepaymentAmount(Player *p){
    if(!p -> hasActiveLoan){
        return 0;
    }
    switch(p->strategy){
        case STRATEGY_AGGRESSIVE_INVESTOR:
            if(p->cash > (p->loanAmount * 2)){
                return p->loanAmount; //Repay full loan if cash is more than double the loan amount
            }
            return 0; //No repayment if cash is less than double the loan amount

        case STRATEGY_CONSERVATIVE_BANKER:
            if(p->cash > (p->loanAmount)){
                return p->loanAmount; //Repay full loan if cash is more than the loan amount
            }
            return p->cash/2; //Repay half the cash if cash is less than the loan amount
        case STRATEGY_RISK_TAKER:
            return 0; //No repayment, always try to increase loan amount

        case STRATEGY_OPPORTUNISTIC_TRADER:
            if(p->cash > (p->loanAmount)){
                return p->loanAmount; //Repay full loan if cash is more than the loan amount
            }
            return 0; //No repayment if cash is less than the loan amount
    
        default:
            return 0;
    }
}
//Check every turn of player is he has monopoly
int hasMonopoly(int playerIndex , Square *squares, PropertyGroup group){
    int groupCount = 0;
    int ownedCount = 0;

    for (int i = 0; i<BOARD_SIZE; i++){
        if(squares[i].propertytype == PropertyType_Regular && squares[i].group == group){
            groupCount++;
            if(squares[i].owner == playerIndex){
            ownedCount++;
            }
        }
    }
    return (groupCount > 0 && groupCount == ownedCount);
}

//Player strategy for build houses
int shouldBuild(Player *p){
    switch(p -> strategy){
        case STRATEGY_AGGRESSIVE_INVESTOR:
            return 1; //Always build if eligible

        case STRATEGY_CONSERVATIVE_BANKER:
            //Never develop hotel until all loan are settle
            //But he can build houses
            return 1;

        case STRATEGY_RISK_TAKER:
            //construct hotels as soon as possible
        return 1;

        case STRATEGY_OPPORTUNISTIC_TRADER:
            //Delay during inlation
        return 1;

        default:
            return 0;
    }
}

//Player strategy for Auction
int wantsToBid(Player *p,Square *square, int bidAmount){
    switch(p -> strategy){
        case STRATEGY_AGGRESSIVE_INVESTOR:
            return bidAmount <= (square -> marketPrice *1.2); //Bids upto 120%

        case STRATEGY_CONSERVATIVE_BANKER:
            return bidAmount <= (square -> marketPrice); //Bidding below market value

        case STRATEGY_RISK_TAKER:
            return p -> cash >= bidAmount;

        case STRATEGY_OPPORTUNISTIC_TRADER:
            return bidAmount < (square -> marketPrice);

    }
}

int decideInsurancePolicy(Player *p,Square *square){
    //return: 0=none, 1=Basic, 2=Comprehensive, 3=Business
    switch(p -> strategy){
        case STRATEGY_AGGRESSIVE_INVESTOR:
            if(square -> hasHotel){
                return 2;
            }
            if(square -> numHouses > 0){
                return 1;
            }
            return 0;

        case STRATEGY_CONSERVATIVE_BANKER:
            return 2; //Always Comprehensive

        case STRATEGY_RISK_TAKER:
            return 0; //Never take insurance
        
        case STRATEGY_OPPORTUNISTIC_TRADER:
            if(square -> hasHotel){
                return 2;
            }
            return 0;
        
        default:
            return 0;
    }
}