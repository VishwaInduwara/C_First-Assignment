#include <stdio.h>
#include "board.h"
#include "types.h"

int main() {
    //Game begin message
    printf("MONOPOLY-LK Simulation\n\n");
    printf("Player 1 : Aggressive Investor\n");
    printf("Player 2 : Conservative Investor\n");
    printf("Player 3 : Risk Investor\n");
    printf("Player 4 : Opportunistic Investor\n");
    printf("Each player begins with LKR 30,000.\n");

      
    initBoard(squares);
       for (int i = 0; i < BOARD_SIZE; i++) {
           printf("Square %d: %s, Type: %d, Group: %d\n", squares[i].index, squares[i].name, squares[i].type, squares[i].group);
       }
}