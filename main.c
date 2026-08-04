#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "players.h"
#include "game.h"
#include "types.h"

int main() {
    srand(time(NULL)); // Seed the random number generator with the current time

    //Game begin message
    printf("MONOPOLY-LK Simulation\n\n");
    printf("Player 1 : Aggressive Investor\n");
    printf("Player 2 : Conservative Investor\n");
    printf("Player 3 : Risk Investor\n");
    printf("Player 4 : Opportunistic Investor\n\n");
    printf("Each player begins with LKR 30,000.\n\n");

    Square squares[BOARD_SIZE]; //Array of 40 squares on the board
    Player players[4]; //Array of 4 players in the game
      
    initBoard(squares);
    initPlayers(players);

    playerDice(players);
       /*for (int i = 0; i < BOARD_SIZE; i++) {
           printf("Square %d: %s, Type: %d, Group: %d\n", squares[i].index, squares[i].name, squares[i].type, squares[i].group);
       }*/
}