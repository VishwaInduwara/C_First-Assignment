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
       
    for(int i = 0; i < MAX_PLAYERS; i++){
        players[i].playerrolls = rollDice();
        printf("%s rolled: %d\n", players[i].name, players[i].playerrolls);
    }
    printf("\n");
}

void determineTurnOrder(Player *players,int turnOrder[]){
    playerDice(players);

    int tie = 0;

    do{
        tie = 0;
        //check for dublicates
        for(int i = 0; i < MAX_PLAYERS; i++){
            for(int j = i+1; j < MAX_PLAYERS; j++){

                if(players[i].playerrolls == players[j].playerrolls){
                    tie = 1;

                    //roll again for the tied players
                    players[i].playerrolls = rollDice();
                    players[j].playerrolls = rollDice();
                    
                    printf("%s rolled again: %d\n", players[i].name, players[i].playerrolls);
                    printf("%s rolled again: %d\n", players[j].name, players[j].playerrolls);
                }
            }
        }
    }while(tie != 0);

    // Initialize turn order with player indices
    for(int i = 0; i < MAX_PLAYERS; i++){
        turnOrder[i] = i;
    }

    // Sort players based on their rolls in descending order
    for(int i=0; i<MAX_PLAYERS; i++){

        int maxIndex = i;

        for(int j=i+1; j<MAX_PLAYERS; j++){

            if(players[turnOrder[j]].playerrolls > players[turnOrder[maxIndex]].playerrolls){
            maxIndex = j;
            }
        }
        // Swap the players based on the maximum roll
        int tempRoll = turnOrder[i];
        turnOrder[i] = turnOrder[maxIndex];
        turnOrder[maxIndex] = tempRoll;
    }
    printf("%s will begin the game.\n",players[turnOrder[0]].name);

    
}