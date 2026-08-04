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