#include <string.h>
#include <stdio.h>
#include "players.h"

//Player players[MAX_PLAYERS];

void initPlayers(Player *players){
    for(int i = 0; i < MAX_PLAYERS; i++){
        // Initialize player attributes
        players[i].position = 0; // Start at the beginning of the board
        players[i].cash = 30000; // Starting cash balance

        players[i].numOwnedProperties = 0; // No properties owned initially
        
        players[i].inJail = 0; // Not in jail
        players[i].jailTurnsRemaining = 0; // No jail turns remaining
        
        players[i].hasActiveLoan = 0; // No active loan
        
        players[i].isBankrupt = 0; // Not bankrupt

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