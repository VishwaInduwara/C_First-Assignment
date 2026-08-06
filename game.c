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
        int tied[MAX_PLAYERS] = {0}; //Array to store the indices of tied players

        //Find ties
        for(int i = 0; i < MAX_PLAYERS; i++){
            for(int j = i+1; j < MAX_PLAYERS; j++){
                if(players[i].playerrolls == players[j].playerrolls){
                    tie = 1;
                    tied[i] = 1;
                    tied[j] = 1;
                }
            }
        }

        //Reroll for tied players
        for(int i = 0; i < MAX_PLAYERS; i++){
            if(tied[i]){
                players[i].playerrolls = rollDice();
                printf("%s rolled again: %d\n", players[i].name, players[i].playerrolls);
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
    printf("\n");

    
}

//500 Round
void runGame(Player *players,int turnOrder[]){
    int currentRound = 0;
    int gameOver = 0; //if 1 game ends

    while(/*!gameOver &&*/ currentRound < 5 ){

        for(int i = 0;i < MAX_PLAYERS; i++){
            int currentPlayerIndex = turnOrder[i];

           /*if(players[currentPlayerIndex].isBankrupt == 1){
                continue;
            }*/

            int diceRoll = rollDice();
            printf("%s rolled %d\n",players[currentPlayerIndex].name,diceRoll);

            movePlayer(players,currentPlayerIndex,diceRoll);
        }

        int allCompleted = 1;//we assume everyone completed
        for(int i = 0;i < MAX_PLAYERS; i++){
            if(players[i].isRoundCompleted==0){
                allCompleted = 0;
                break;
            }
        }
            if(allCompleted == 1){
            currentRound ++;
            printf("Current Round: %d\n\n",currentRound);

            //Reset for next round
            for(int i = 0;i < MAX_PLAYERS; i++){
                players[i].isRoundCompleted = 0;
            }
        }
    }
}