#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
//void playerDice(void);
int rollDice(){

    int die1 = rand() %6 + 1; //Random number between 1 and 6
    int die2 = rand() %6 + 1; //Random number between 1 and 6
    int total = die1 + die2;

    return total;
}


void playerDice(Player *players){
    int playerrolls[4];
    for(int i = 0; i < 4; i++){
        playerrolls[i] = rollDice();
        printf("Player %s rolled: %d\n", players[i].name, playerrolls[i]);
    }
}

/*void determineTurnOrder(int playerrolls[4], int turnOrder[4]){
    // Initialize turn order with player indices
    for(int i = 0; i < 4; i++){
        turnOrder[i] = i;
    }

    // Sort the turn order based on player rolls (descending order)
    for(int i = 0; i < 4 - 1; i++){
        for(int j = 0; j < 4 - i - 1; j++){
            if(playerrolls[j] < playerrolls[j + 1]){
                // Swap rolls
                int tempRoll = playerrolls[j];
                playerrolls[j] = playerrolls[j + 1];
                playerrolls[j + 1] = tempRoll;

                // Swap corresponding turn order
                int tempOrder = turnOrder[j];
                turnOrder[j] = turnOrder[j + 1];
                turnOrder[j + 1] = tempOrder;
            }
        }
    }
}*/
    
