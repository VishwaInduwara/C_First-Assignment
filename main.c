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
    printf("Player 2 : Conservative Banker\n");
    printf("Player 3 : Risk Taker\n");
    printf("Player 4 : Opportunistic Trader\n\n");
    printf("Each player begins with LKR 30,000.\n\n");

    Square squares[BOARD_SIZE]; //Array of 40 squares on the board
    Player players[MAX_PLAYERS]; //Array of players in the game
    int turnOrder[MAX_PLAYERS]; //Array to store the turn order of players

    initBoard(squares);
    initPlayers(players);

    determineTurnOrder(players, turnOrder);
    runGame(players,turnOrder);
       /*for (int i = 0; i < BOARD_SIZE; i++) {
           printf("Square %d: %s, Type: %d, Group: %d\n", squares[i].index, squares[i].name, squares[i].type, squares[i].group);
       }*/
}