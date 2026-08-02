#include <string.h>

#include "board.h"
#include "types.h"

void initBoard(GameBoard *board) {
    // Initialize the board with default values
    
    //Square 0
    board->squares[0].index = 0;
    board->squares[0].type = SQ_START;
    strcpy(board->squares[0].name, "Go");
    board->squares[0].propertyIndex = -1; //No property associated with this square
}