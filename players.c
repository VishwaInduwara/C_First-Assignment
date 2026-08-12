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
            return (p->cash - square->basePurchasePrice) >= square->baseRent;

        case STRATEGY_CONSERVATIVE_BANKER:
            // "Purchases properties only if at least 50% of current cash remains after purchase."
            return (p->cash - square->basePurchasePrice) >= (p->cash / 2);

        case STRATEGY_RISK_TAKER:
            // "Purchases every available property whenever legally possible."
            return p->cash >= square->basePurchasePrice;

        case STRATEGY_OPPORTUNISTIC_TRADER:
            // "Purchases properties only when projected appreciation exceeds construction costs."
            // (simplify for now - refine later once you have appreciation logic)
            return p->cash >= square->basePurchasePrice;

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