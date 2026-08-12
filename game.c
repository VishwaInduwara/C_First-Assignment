#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "finance.h"
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

int rollDiceDouble(int *wasDouble){
    int die1 = rand() %6 + 1;
    int die2 = rand() %6 + 1;
    
    if(die1 == die2){
        *wasDouble = 1;
    }else{
        *wasDouble = 0;
    }
    return die1 + die2;
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
    printf("%s will begin the game.\n\n",players[turnOrder[0]].name);
    printf("Turn order:\n");

    for(int i = 0;i<MAX_PLAYERS; i++){
        printf("%s\n",players[turnOrder[i]].name);
    }
    printf("\n\n");
    printf("----------------------------------\n");
}

void handleBankSquare(int playerIndex,Player *p,Square *squares){//playerIndex = currentPlayerIndex
    printf("%s landed on Bank of Ceylon.\n",p -> name);

    //Calculate max loan
    int maxLoan = calculateMaxLoan(playerIndex,squares);

    if(maxLoan <= 0){
        printf("%s has no eligible property collateral for a loan.\n",p -> name);
        return;
    }

    //Strategy decision from players.c
    if(shouldTakeLoan(p,maxLoan,squares)){
        p -> cash += maxLoan;
        p -> hasActiveLoan = 1;
        p -> loanAmount = maxLoan;
        p -> loanTurnsRemaining = 20;

        printf("%s secured a loan of LKR %d from BOC!\n",p -> name,maxLoan);

        for(int i=0 ; i<BOARD_SIZE; i++){
            if(squares[i].owner == playerIndex){
                squares[i].isLoanLocked = 1;
            }
        }
    }else{
        printf("%s chose not to take a loan.\n",p -> name);
    }
}


//Landing resoultion
void resolveLanding(Player *players,int currentPlayerIndex,Square *squares,int diceRoll){

            Square *landedSquare = &squares[players[currentPlayerIndex].position];

            if (landedSquare->type == SQ_PROPERTY || landedSquare->type == SQ_RAILWAY || landedSquare->type == SQ_UTILITY) {

                if (landedSquare->owner == -1) {
                    printf("%s landed on %s (unowned). Price: LKR %d\n",
                        players[currentPlayerIndex].name, landedSquare->name, landedSquare->basePurchasePrice);

                    if (shouldBuyProperty(&players[currentPlayerIndex],landedSquare)) {
                        players[currentPlayerIndex].cash -= landedSquare->basePurchasePrice;
                        players[currentPlayerIndex].numOwnedProperties += 1;

                        landedSquare->owner = currentPlayerIndex;

                        printf("%s purchased %s for LKR %d.\n", players[currentPlayerIndex].name, landedSquare->name, landedSquare->basePurchasePrice);
                        printf("Remaining Balance : LKR %d.\n", players[currentPlayerIndex].cash);
                    }else {
                        printf("%s decline to purchase %s\n",players[currentPlayerIndex].name,landedSquare->name);
                    }

                } else if (landedSquare->owner == currentPlayerIndex) {
                    printf("%s landed on their own property, %s.\n", players[currentPlayerIndex].name, landedSquare->name);

                } else {
                    if (!landedSquare->isMortgaged) {
                        int rent = calculateRent(landedSquare , squares , diceRoll);

                        players[currentPlayerIndex].cash -= rent;
                        players[landedSquare->owner].cash += rent;

                        printf("%s landed on %s.\n", players[currentPlayerIndex].name, landedSquare->name);
                        printf("Rent Paid : LKR %d.\n", rent);
                        printf("Owner : %s.\n", players[landedSquare->owner].name);
                    }
                }
            } else if(landedSquare->type == SQ_TAX){

                payTax(&players[currentPlayerIndex], squares[4].baseRent);
//Special square handeling
            }else if(landedSquare->type == SQ_SPECIAL){
                if(landedSquare->index == 30) { //Go to Jail
                    players[currentPlayerIndex].position = 10; //move to jail square
                    players[currentPlayerIndex].inJail = 1;
                    players[currentPlayerIndex].jailTurnsRemaining = 3;

                    printf("%s landed on Go To Jail.\n",players[currentPlayerIndex].name);
                    printf("%s has been sent to Jail.\n",players[currentPlayerIndex].name);
    
                } else {
                    //Just visiting Square 10 or Free Parking square 20
                    printf("%s landed on %s\n",players[currentPlayerIndex].name,landedSquare->name);
                }
            } else if(landedSquare->type == SQ_BANK){

                handleBankSquare(currentPlayerIndex, &players[currentPlayerIndex],squares);

            }else {
                printf("%s landed on %s (not yet handled).\n", players[currentPlayerIndex].name, landedSquare->name);
            }
        
}

//Just demo , must includem building values and real property value not base value
void calculateNetWorth(Player *players,Square *squares){
    for(int j=0 ; j<MAX_PLAYERS ; j++){
        players[j].totalPropertyValue = 0;
        for(int i=0 ; i<BOARD_SIZE ; i++){ 
            if(squares[i].owner == j){
                
                players[j].totalPropertyValue += squares[i].basePurchasePrice;
            }
        }
        
        //printf("Net worth before cash %d\n",players[j].netWorth);
        players[j].netWorth = players[j].totalPropertyValue + players[j].cash - players[j].loanAmount;
    }

}

int resolveJailTurn(Player *p){
    if(!p -> inJail){
        return 0; //not in jail
    }

    int wasDouble = 0;
    int roll = rollDiceDouble(&wasDouble);

    if(wasDouble = 1){
        printf("%s rolled doubles %d and released from Jail\n",p -> name , roll);
        p -> inJail = 0;
        p -> jailTurnsRemaining = 0;
        return 0; //Player relesed from jail
    }

    p -> jailTurnsRemaining--;
    printf("%s remains in Jail and %d Turns remaining.\n",p -> name,p -> jailTurnsRemaining);

    if(p -> jailTurnsRemaining <= 0){
        printf("%s has served their time and is released from Jail.\n", p ->name);
        p -> inJail =0;
    }
    return 1;
}

//Build Monopoly 
void tryBuildMonopolies(Player *p , int playerIndex , Square *squares){
    printf("TryBuildMONOPOLY called\n\n");
    for(int group = Group_Brown; group <= Group_DarkBlue; group ++){
        if(hasMonopoly(playerIndex , squares , (PropertyGroup)group)){
            p -> hasMonopoly = 1;
            if(shouldBuild(p)){
                for(int i = 0; i<BOARD_SIZE; i++){
                    if(squares[i].numHouses < 4 && squares[i].propertytype == PropertyType_Regular){
                        buildHouse(p,playerIndex,squares,&squares[i]);
                    }
                    else if(squares[i].group == (PropertyGroup)group && squares[i].owner == playerIndex && squares[i].propertytype == PropertyType_Regular){
                        buildHotel(p,playerIndex,&squares[i]);
                    }
                }
            }
        }
    }
}



//500 Round
void runGame(Player *players,int turnOrder[],Square *squares){
    int currentRound = 0;
    int gameOver = 0; //if 1 game ends

    while(/*!gameOver &&*/ currentRound < MAX_ROUND ){

        for(int i = 0;i < MAX_PLAYERS; i++){
            int currentPlayerIndex = turnOrder[i];

            if(resolveJailTurn(&players[currentPlayerIndex])==1){
                continue; //Still in jail , skip rest of this turn
            }

           /*if(players[currentPlayerIndex].isBankrupt == 1){
                continue;
            }*/
            int oldPosition = players[currentPlayerIndex].position;
            int diceRoll = rollDice();
            printf("%s rolled %d\n",players[currentPlayerIndex].name,diceRoll);
            printf("%s moves from Square %d to Square %d\n\n",players[currentPlayerIndex].name,oldPosition,(oldPosition+diceRoll)%40);

            movePlayer(players,currentPlayerIndex,diceRoll);

            resolveLanding(players,currentPlayerIndex,squares,diceRoll);

            if(squares[players[currentPlayerIndex].position].propertytype == PropertyType_Regular){
                tryBuildMonopolies(&players[currentPlayerIndex],currentPlayerIndex,squares);
            }


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

            calculateNetWorth(players,squares);
            printf("\n======================================\n");
            printf("Round %d Summary\n",currentRound);
            printf("======================================\n");

            for(int i = 0;i<MAX_PLAYERS;i++){
                printf("%s\n",players[i].name);
                printf("Cash : LKR %d\n",players[i].cash);
                printf("Net Worth : LKR %d\n",players[i].netWorth);
                printf("Properties : %d\n",players[i].numOwnedProperties);
                printf("Hotels : %d\n",players[i].numHotelCount);
                printf("Outstanding Loan : LKR %d\n",players[i].loanAmount);
                printf("\n---------------------------------\n\n");
            }

            //Reset for next round
            for(int i = 0;i < MAX_PLAYERS; i++){
                players[i].isRoundCompleted = 0;
            }
        }

   
        
    }

    
    printf("=========================================\n\n");
    printf("          End of Game\n\n");
    printf("=========================================\n\n");
    printf("GAME OVER!\n\n");
    printf("Winner:\n");

    int winnerIndex = 0;
    for(int i=1;i<MAX_PLAYERS;i++){
        if(players[i].netWorth>players[winnerIndex].netWorth){
            winnerIndex = i;
        }
    }
    printf("    %s\n\n",players[winnerIndex].name);
    printf("Total Cash\n    LKR %d\n\n",players[winnerIndex].cash);
    printf("Total Property Value\n  LKR %d\n\n",players[winnerIndex].totalPropertyValue);
    printf("Outstanding Loans\n");
    if(players[winnerIndex].loanAmount <= 0){
        printf("    None\n\n");
    }else{
        printf("    %d\n\n",players[winnerIndex].loanAmount);
    }
    printf("Net Worth\n LKR %d\n\n",players[winnerIndex].netWorth);
}

