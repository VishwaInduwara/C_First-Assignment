#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "board.h"
#include "players.h"
#include "finance.h"
#include "events.h"
#include "game.h"


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

void handleBankSquare(int playerIndex,Player *p,Square *squares,GameState *game){ //playerIndex = currentPlayerIndex
    printf("%s landed on Bank of Ceylon.\n",p -> name);

    //Check if player should repay a loan
    if(p->hasActiveLoan){
        // Determine the player's action
        int action = shouldRepayLoan(p); //Call the function from players.c to determine if the player should repay the loan based on strategy

        if(action == 0){        
            int repayAmount = decideRepaymentAmount(p); //Call the function from players.c to determine repayment amount based on strategy
            if(repayAmount > 0){
                repayLoan(p, repayAmount , squares, playerIndex);//call the function from finance.c to process the repayment
            }else{
                printf("%s chose not to repay the loan this turn.\n",p -> name);
            }
        }else if(action == 1){
            extendLoanTurn(p); //Call the function from finance.c to extend the loan repayment period
        }else if(action == 2){
            increaseLoanAmount(p, playerIndex, squares, game); //Call the function from finance.c to increase the loan amount
        }else{
            printf("%s chose not to take any action regarding the loan this turn.\n",p -> name);
        }
        return; //One action per turn, so return after handling loan repayment
    }

    //Calculate max loan
    int maxLoan = calculateMaxLoan(playerIndex,squares,game);

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


void runAuction(Player *players , Square *square,GameState *game,Square *squares){
    int active[MAX_PLAYERS];
    int activeCount = MAX_PLAYERS;
    int currentBid = 0;
    int highestBidder = -1;

    for(int i = 0; i< MAX_PLAYERS;i++){
        active[i] = !players[i].isBankrupt; //Non bankrupt players are active
    }


    //Rule-LK 19: bidding begins at 50% of market value
    currentBid = currentMarketValue(square) / 2;

    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        currentBid = (currentBid * 115) / 100; //Rule LK 31: purchase prices increase by 15% during boom
    }else if(square -> group == game -> declinedGroup && game -> declineRoundsRemaining > 0){
        currentBid = (currentBid * 75) / 100; //Rule LK 32: auction starting prices decrease by 25% during decline
    }

    

    printf("\n--------------------\n");
    printf("Auction Started\n");
    printf("\n--------------------\n");

    //get activecount for run the while loop
    for(int j = 0;j<MAX_PLAYERS; j++){
        if(active[j] == 0){
            activeCount--;
        }
    }

    while(activeCount > 1){
        for(int i=0; i<MAX_PLAYERS; i++){
            if(active[i] == 0){
                continue;
            }
            
            int newBid = currentBid + 250;//Rule LK 20

            //Call wantstoBid function in player.c
            if(wantsToBid(&players[i],square,newBid)&& players[i].cash >= newBid){//Rule LK 22
                if(players[i].cash >=newBid){//Rule LK 22
                    //newBid = currentBid + 250;//Rule LK 20
                    currentBid = newBid;
                    highestBidder = i;
                    printf("%s Bids LKR %d\n",players[i].name,currentBid);
                }else{
                    active[i] = 0;
                    activeCount--;
                    printf("%s withdraws\n",players[i].name);
                }
            }else{
                active[i] = 0;
                activeCount--;
                printf("%s withdraws\n",players[i].name);
            }
            if(activeCount <= 1){
                break;
            }
        }
    }
    if(highestBidder != -1){
        players[highestBidder].cash -= currentBid;
        square -> owner = highestBidder;
        players[highestBidder].numOwnedProperties += 1;

        checkBankruptcy(&players[highestBidder],highestBidder,squares); //Check if player is bankrupt after auction purchase    

        printf("\n-------------------\n");
        printf("%s wins the auction\n",players[highestBidder].name);
        printf("----------------------\n");
    }else{
        printf("No players bids , Ownership remains with the Bank\n");
    }
}

//Landing resoultion
void resolveLanding(Player *players,int currentPlayerIndex,Square *squares,int diceRoll,GameState *game){

            Square *landedSquare = &squares[players[currentPlayerIndex].position];

            if (landedSquare->type == SQ_PROPERTY || landedSquare->type == SQ_RAILWAY || landedSquare->type == SQ_UTILITY) {

                if (landedSquare->owner == -1) {
                    printf("%s landed on %s (unowned). Price: LKR %d\n",
                        players[currentPlayerIndex].name, landedSquare->name, landedSquare->marketPrice);

                    if (shouldBuyProperty(&players[currentPlayerIndex],landedSquare) && (canPurchaseProperty(game,currentPlayerIndex,squares))) {
                        players[currentPlayerIndex].cash -= landedSquare->marketPrice;
                        players[currentPlayerIndex].numOwnedProperties += 1;

                        landedSquare->owner = currentPlayerIndex;

                        checkBankruptcy(&players[currentPlayerIndex],currentPlayerIndex,squares); //Check if player is bankrupt after purchase

                        printf("%s purchased %s for LKR %d.\n", players[currentPlayerIndex].name, landedSquare->name, landedSquare->marketPrice);
                        printf("Remaining Balance : LKR %d.\n", players[currentPlayerIndex].cash);
                    }else {
                        printf("%s decline to purchase %s\n",players[currentPlayerIndex].name,landedSquare->name);
                        printf("\nAuction Called\n");

                        //Call the Auction function
                        runAuction(players , landedSquare,game,squares);
                    }

                } else if (landedSquare->owner == currentPlayerIndex) {
                    printf("%s landed on their own property, %s.\n", players[currentPlayerIndex].name, landedSquare->name);

                    if(shouldRenovate(&players[currentPlayerIndex],landedSquare->depreciationPct)){
                        renovateProperty(&players[currentPlayerIndex],currentPlayerIndex,squares,landedSquare);
                    }

                } else {
                    if (!landedSquare->isMortgaged) {
                        if(landedSquare->isDamaged){
                        
                            printf("%s landed on %s, but the property is damaged and cannot collect rent.\n", players[currentPlayerIndex].name, landedSquare->name);
                        
                        }else{
                        
                            int rent = calculateRent(landedSquare , squares , diceRoll,game);

                            if(players[currentPlayerIndex].cash < rent){
                                raiseCashByMortgage(&players[currentPlayerIndex], currentPlayerIndex, squares, game, rent);
                            }//Mortgage to cover rent

                            players[currentPlayerIndex].cash -= rent;
                            players[landedSquare->owner].cash += rent;

                            checkBankruptcy(&players[currentPlayerIndex],currentPlayerIndex,squares);

                            printf("%s landed on %s.\n", players[currentPlayerIndex].name, landedSquare->name);
                            printf("Rent Paid : LKR %d.\n", rent);
                            printf("Owner : %s.\n", players[landedSquare->owner].name);
                        }
                    }
                }
            } else if(landedSquare->type == SQ_TAX){

                if(landedSquare->index == 2){
                    //Community Development Fund: 10% of property assets (buildings excluded)
                    int tax = (totalPropertyAssets(currentPlayerIndex, squares) * marketTaxRate(game, 10)) / 100;
                    payTax(&players[currentPlayerIndex], tax, currentPlayerIndex, squares, game);
                }else{

                    //Income Tax: 15% of total assets (cash + properties)
                    int rate = 15;

                    if(isRegulationActive(game, REG_INCREASE_PROPERTY_TAX)){
                        rate = (rate * 150) / 100; //Rule-LK 24: Income Tax +50% -> 22.5%
                    }

                    int assets = players[currentPlayerIndex].cash + totalPropertyAssets(currentPlayerIndex, squares);
                    int tax = (assets * marketTaxRate(game, rate)) / 100;
                    payTax(&players[currentPlayerIndex], tax, currentPlayerIndex, squares, game);
                }

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

                handleBankSquare(currentPlayerIndex, &players[currentPlayerIndex],squares,game);

            }else if(landedSquare->type == SQ_INSURANCE){
                for(int i = 0; i < BOARD_SIZE; i++) {
                    if(squares[i].owner == currentPlayerIndex && squares[i].insurancePolicyType == None_Insurance){
                        int choice = decideInsurancePolicy(&players[currentPlayerIndex], &squares[i]); //Call the function from players.c to decide insurance policy
                        if(choice > 0){
                            purchaseInsurance(&players[currentPlayerIndex], &squares[i],choice,game); //Call the function from finance.c to purchase insurance
                            break; // Only allow one insurance purchase per turn
                        }
                    }
                }
            }else if(landedSquare -> type == SQ_EVENT){
                printf("%s landed on %s (National Event Card).\n", players[currentPlayerIndex].name, landedSquare->name);
                NationalCardId card = drawEventCard(game); //Draw a national card from the deck
                applyEventCard(card, players, currentPlayerIndex, squares, game); //Apply the effect of the drawn card
            }
        
}

//Just demo , must includem building values and real property value not base value
void calculateNetWorth(Player *players,Square *squares){
    for(int j=0 ; j<MAX_PLAYERS ; j++){
        players[j].totalPropertyValue = 0;
        int buildingValue = 0;

        for(int i=0 ; i<BOARD_SIZE ; i++){ 
            if(squares[i].owner == j){
                players[j].totalPropertyValue += currentMarketValue(&squares[i]); //Use current market value instead of base purchase price
                if(squares[i].hasHotel){
                    buildingValue += squares[i].baseHotelCost;
                }else if(squares[i].numHouses > 0){
                    buildingValue += squares[i].numHouses * squares[i].baseHouseCost;
                }
            }
        }
        
        
        players[j].netWorth = players[j].totalPropertyValue + buildingValue + players[j].cash - players[j].loanAmount;

    }

}

int resolveJailTurn(Player *p, int playerIndex, Square *squares, GameState *game){
    if(!p -> inJail){
        return 0; //not in jail
    }

    //Rule 13: pay bail of LKR 300 to leave immediately
    if(shouldPayBail(p)){
        if(p -> cash < 300){
            raiseCashByMortgage(p, playerIndex, squares, game, 300); //mortgage to cover bail
        }
        if(p -> cash >= 300){
            p -> cash -= 300;
            p -> inJail = 0;
            p -> jailTurnsRemaining = 0;
            printf("%s paid LKR 300 bail and is released from Jail.\n", p -> name);
            printf("Remaining Balance : LKR %d.\n", p -> cash);
            return 0; //Player released by paying bail
        }
    }

    int wasDouble = 0;
    int roll = rollDiceDouble(&wasDouble);

    if(wasDouble == 1){
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
        return 0; //Player released from jail
    }
    return 1;
}

//Build Monopoly 
void tryBuildMonopolies(Player *p , int playerIndex , Square *squares,GameState *game){
    
    if(isNationalEffectActive(game,CARD_LABOUR_STRIKE,playerIndex)){
        printf("%s cannot build due to Labour Strike.\n",p->name);
        return;
    }
    for(int group = Group_Brown; group <= Group_DarkBlue; group ++){
        if(hasMonopoly(playerIndex , squares , (PropertyGroup)group)){
            p -> hasMonopoly = 1;
            int buildHouses = shouldBuild(p, 0); //decide once per group
            int buildHotels = shouldBuild(p, 1);
            for(int i = 0; i<BOARD_SIZE; i++){
                if(squares[i].group == (PropertyGroup)group && squares[i].owner == playerIndex && squares[i].propertytype == PropertyType_Regular){
                    if(squares[i].numHouses == 4 && !squares[i].hasHotel){
                        if(buildHotels){
                            buildHotel(p, playerIndex, &squares[i], game,squares);
                        }
                    } else if(squares[i].numHouses < 4){
                        if(buildHouses){
                            buildHouse(p, playerIndex, squares, &squares[i],game);
                        }
                    }
                }
            }
        }
    }
}



//500 Round
void runGame(Player *players,int turnOrder[],Square *squares,GameState *game){
    
    int currentRound = 0;
    int gameOver = 0; //if 1 game ends

    //This is for Dynamic property Market
    game -> boomedGroup = GROUP_NONE;
    game -> declinedGroup = GROUP_NONE;
    game -> currentRound = currentRound;
    game -> currentLoanInterest = 0.08;
    game -> baseLoanInterest = 0.08;    
    game -> activeRegulation = -1;
    game -> regulationRoundsRemaining = 0;
    game -> activeRegionalCard = -1;
    game -> regionalCardRoundsRemaining = 0;

    initEventDeck(game);

    while(!gameOver && currentRound < MAX_ROUND ){

        for(int i = 0;i < MAX_PLAYERS; i++){
            int currentPlayerIndex = turnOrder[i];
            players[currentPlayerIndex].isRoundCompleted = 1;


            if(resolveJailTurn(&players[currentPlayerIndex],currentPlayerIndex,squares,game)==1){
                continue; //Still in jail , skip rest of this turn
            }

            if(players[currentPlayerIndex].isBankrupt == 1){
                continue;
            }
            //Rule-LK 27/29: maintain or renovate buildings at the beginning of the turn
            for(int j = 0; j < BOARD_SIZE; j++){
                if(squares[j].owner == currentPlayerIndex && (squares[j].numHouses > 0 || squares[j].hasHotel == 1)){
                    if(squares[j].isStructurallyDamaged){
                        renovateDamagedBuilding(&players[currentPlayerIndex], currentPlayerIndex, squares, &squares[j]);
                    }else if(squares[j].buildingCondition < 100 &&
                        shouldMaintain(&players[currentPlayerIndex], squares[j].buildingCondition)){
                        maintainProperty(&players[currentPlayerIndex], currentPlayerIndex, squares, &squares[j]);
                    }
                }   
            }

            int oldPosition = players[currentPlayerIndex].position;
            int diceRoll = rollDice();
            printf("%s rolled %d\n",players[currentPlayerIndex].name,diceRoll);
            printf("%s moves from Square %d to Square %d\n\n",players[currentPlayerIndex].name,oldPosition,(oldPosition+diceRoll)%BOARD_SIZE);

            movePlayer(players,currentPlayerIndex,diceRoll);

            resolveLanding(players,currentPlayerIndex,squares,diceRoll,game);

            //Call the build function if can build houses and hotel
            if(squares[players[currentPlayerIndex].position].propertytype == PropertyType_Regular){
                tryBuildMonopolies(&players[currentPlayerIndex],currentPlayerIndex,squares,game);
            }


        }

        
        //Loan Handling and Bankruptcy Check
        for(int i = 0;i < MAX_PLAYERS; i++){
            if(players[i].hasActiveLoan == 1){
                float effectiveInterest = game -> currentLoanInterest;
                if(isEconomicEventActive(game, ECON_ECONOMIC_RECESSION)){
                    effectiveInterest *= 1.15; //Rule-LK 18: loan interest +15%
                }
                if(isEconomicEventActive(game, ECON_STOCK_MARKET_BOOM)){
                    effectiveInterest *= 0.90; //Rule-LK 18: loan interest -10%
                }
                players[i].loanAmount += (int)(players[i].loanAmount * effectiveInterest);
                players[i].loanTurnsRemaining--;

                if(players[i].loanTurnsRemaining <= 0){
                    printf("%s has defaulted.\n",players[i].name);

                    int stillHasProperties = 0;
                    for(int j = 0;j<BOARD_SIZE;j++){
                        if(squares[j].owner == i && squares[j].isLoanLocked == 1){
                            players[i].numOwnedProperties--;
                            squares[j].owner = -1; //Return to bank

                            squares[j].propertyAge = 0;//Reset property age
                            squares[j].depreciationPct = 0;//Reset depreciation percentage

                            squares[j].isLoanLocked = 0;
                            squares[j].isMortgaged = 0; //clear mortgage when returned to the bank
                            squares[j].numHouses = 0;//Houses demolished
                            squares[j].hasHotel = 0;//Hotel demolished
                            printf("%s's property %s has been returned to the bank.\n",players[i].name,squares[j].name);
                            squares[j].insurancePolicyType = None_Insurance;
                            squares[j].insuranceRoundsRemaining = 0;

                            squares[j].propertyAge = 0;//Reset property age
                            squares[j].depreciationPct = 0;//Reset depreciation percentage
                            squares[j].buildingCondition = 100;           //Rule-LK 25: reset condition
                            squares[j].roundsWithoutMaintenance = 0;      //Rule-LK 28: reset ignored-rounds
                            squares[j].isStructurallyDamaged = 0;         //Rule-LK 28: clear damage flag
                        }
                        if(squares[j].owner == i){
                            stillHasProperties = 1;
                        }
                    }
                    players[i].hasActiveLoan = 0;
                    players[i].loanAmount = 0;
                    
                    printf("Collateral has been foreclosed\n");
                    printf("Outstanding debt cleared\n");

                    if(!stillHasProperties && players[i].cash <= 0){
                        checkBankruptcy(&players[i],i,squares);//finance.c RULE LK 7
                        

                        printf("%s has no properties and is bankrupt.\n",players[i].name);
                    }
                }
            }
        }

        for(int i = 0;i < BOARD_SIZE; i++){
            if(squares[i].insurancePolicyType != None_Insurance){
                squares[i].insuranceRoundsRemaining--;
                if(squares[i].insuranceRoundsRemaining == 3){
                    printf("Insurance policy for %s is about to expire in 3 rounds.\n",squares[i].name);
                }
                if(squares[i].insuranceRoundsRemaining <= 0){
                    printf("Insurance policy for %s has expired.\n",squares[i].name);
                    squares[i].insurancePolicyType = None_Insurance;
                    squares[i].insuranceRoundsRemaining = 0;
                }
            }
        }
        tickNationalEffects(game);

        //Rule-LK 25: building condition decreases by 2% at the end of every round
        //Rule-LK 28: structural damage if maintenance ignored for more than 20 rounds
        for(int i = 0; i < BOARD_SIZE; i++){
            if(squares[i].owner != -1 && (squares[i].numHouses > 0 || squares[i].hasHotel == 1)){
                squares[i].buildingCondition -= 2;
                if(squares[i].buildingCondition < 0) squares[i].buildingCondition = 0;
                squares[i].roundsWithoutMaintenance++;
                if(squares[i].roundsWithoutMaintenance > 20 && !squares[i].isStructurallyDamaged){
                    squares[i].isStructurallyDamaged = 1;
                    printf("STRUCTURAL DAMAGE: %s ignored maintenance too long!\n", squares[i].name);
                    printf("Property value -15%%, max rent -25%%, maintenance costs +50%%.\n");
                }
            }
        }

        //Rule-LK 15: property age increases every complete round
        //Rule-LK 16: older than 50 rounds, -1% value per 5 rounds, max 30%
        for(int i = 0; i < BOARD_SIZE; i++){
            if(squares[i].owner != -1){
                squares[i].propertyAge++;
                if(squares[i].propertyAge > 50 && squares[i].propertyAge % 5 == 0){
                    if(squares[i].depreciationPct < 30){
                        squares[i].depreciationPct++;
                        printf("%s has depreciated by %d%% (age %d rounds).\n",
                            squares[i].name, squares[i].depreciationPct, squares[i].propertyAge);
                    }
                }
            }
        }

        //Luxury Property Tax
        if(isRegulationActive(game, REG_LUXURY_PROPERTY_TAX)){
            for(int i = 0; i < BOARD_SIZE; i++){
                if(squares[i].hasHotel && squares[i].owner != -1){
                    int hotelTax = (int)((currentMarketValue(&squares[i]) * 25) / 100);
                    players[squares[i].owner].cash -= hotelTax;
                    printf("%s paid LKR %d luxury property tax for %s (Hotel).\n",
                        players[squares[i].owner].name, hotelTax, squares[i].name);
                    checkBankruptcy(&players[squares[i].owner], squares[i].owner, squares);
                }
            }
        }
        
        //Automatic Repair when owner has sufficient cash
        for(int i = 0;i<BOARD_SIZE;i++){
            if(squares[i].isDamaged){
                int ownerIndex = squares[i].owner;
                if(ownerIndex != -1){
                    int repairCost = squares[i].marketPrice;
                    if(squares[i].hasHotel){
                        repairCost += squares[i].baseHotelCost;
                    }else if(squares[i].numHouses > 0){
                        repairCost += squares[i].numHouses * squares[i].baseHouseCost;
                    }
                    if(players[ownerIndex].cash >= repairCost){
                        players[ownerIndex].cash -= repairCost;
                        squares[i].isDamaged = 0;
                        printf("%s repaired %s, Repair Cost: LKR %d.\n", players[ownerIndex].name, squares[i].name, repairCost);
                    }
                }
            }
        }

       

        //if only one player is solvent, game ends
        int solventCount = 0;
        for(int i = 0;i<MAX_PLAYERS;i++){
            if(!players[i].isBankrupt){
                solventCount++;
            }
        }
            if(solventCount <= 1){
                gameOver = 1;
            }

            currentRound ++;
            game -> currentRound = currentRound;
            updateMarketBoomDecline(game); //Call the function to do market Boom or Decline

            //Check if a government regulation is active and decrement the remaining rounds
            if(game -> regulationRoundsRemaining > 0){
                game -> regulationRoundsRemaining--;
            }
            if(currentRound % 20 == 0){
                triggerGovernmentRegulation(game, squares);
            }

            //Check if a regional development card is active and decrement the remaining rounds
            if(game -> regionalCardRoundsRemaining > 0){
                game -> regionalCardRoundsRemaining--;
            }
            if(currentRound % 15 == 0){
                triggerRegionalDevelopment(game, squares);
            }

            tickEconomicEvents(game, squares); //Rule-LK 18: expire finished events
            
            if(currentRound % 15 == 0){
                triggerEconomicEvent(game, squares); //Rule-LK 18: new event
            }

            //Call Inflaction function in finance.c
            if(currentRound %10 == 0){
                calculateInflation(squares,currentRound,game);
                triggerDisaster(players,squares,game); //Call the function to trigger a disaster event
            }
            printf("Current Round: %d\n\n",currentRound);

            calculateNetWorth(players,squares);
            printf("\n==========================================================\n");
            printf("                        ROUND %d SUMMARY\n", currentRound);
            printf("==========================================================\n");

            for(int i = 0;i<MAX_PLAYERS;i++){
                printf("%s\n",players[i].name);
                printf("Cash : LKR %d\n",players[i].cash);
                printf("Net Worth : LKR %d\n",players[i].netWorth);
                printf("Properties : %d\n",players[i].numOwnedProperties);
                printf("Hotels : %d\n",players[i].numHotelCount);
                printf("Outstanding Loan : LKR %d\n",players[i].loanAmount);
                printf("\n----------------------------------------------------------\n\n");
            }

            printf("\n=========================\n\n");
            printf("Rule-LK 36 output messages\n\n");
            printf("==========================\n");
            printf("Current Market Condition\n");
            printf("==========================\n\n");
            printf("Market Boom\n----------------------------------------------------------\n");
            printf("Rounds Remaining : %d\n\n",game -> boomRoundsRemaining);

            printf("Market Decline\n----------------------------------------------------------\n");
            printf("Rounds Remainig : %d\n\n",game -> declineRoundsRemaining);

            printf("Regional Development\n----------------------------------------------------------\n");

            printActiveRegionalCard(game); //Call the function to print the active regional development card            

            printf("\nInflation\n----------------------------------------------------------\n%.2f\n",game -> currentInflactionRate);

            printf("Current Loan Interest\n----------------------------------------------------------\n%.2f\n",game -> currentLoanInterest);

            printf("==========================================================\n");

            //Reset for next round
            for(int i = 0;i < MAX_PLAYERS; i++){
                players[i].isRoundCompleted = 0;
            }
      //  }

   
        
    }

    
    printf("=========================================\n\n");
    printf("            End of Game\n\n");
    printf("=========================================\n\n");
    printf("GAME OVER!\n\n");
    printf("Winner:\n");

int winnerIndex = -1;
for(int i = 0; i < MAX_PLAYERS; i++){
    if(!players[i].isBankrupt){
        if(winnerIndex == -1 || players[i].netWorth > players[winnerIndex].netWorth){
            winnerIndex = i;
        }
    }
}
if(winnerIndex == -1){
    printf("All players are bankrupt. No solvent winner.\n");
    // decide a tie-break rule here, e.g. least negative net worth
}else{
    
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
}

