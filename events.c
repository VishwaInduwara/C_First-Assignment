#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "events.h"
#include "finance.h"

void updateMarketBoomDecline(GameState *game){
    //Tick down cooldown and active effects
    for(int i = 0; i<8; i++){
        if(game -> boomCooldown[i] >0){//array to remember how many rounds remain each group
            game -> boomCooldown[i]--;
        }
        if(game -> declineCooldown[i] > 0){
            game -> declineCooldown[i]--;
        }
    }
        if(game -> boomRoundsRemaining >0){//This is for count boom rounds(10 rounds)
            game -> boomRoundsRemaining--;
        }
        if(game -> declineRoundsRemaining >0){
            game -> declineRoundsRemaining--;
        }

        if(game -> currentRound % 10 !=0){
            return; //Only after 10 rounds pick boom or decline group
        }
    

        //Pick a random group for boom , must not be on cooldown(30 rounds)
        int boomPick;
        do{
            boomPick = rand() % 8;
        }while (game -> boomCooldown[boomPick] > 0);

        game -> boomedGroup = (PropertyGroup)boomPick;
        game -> boomRoundsRemaining = 10;
        game -> boomCooldown[boomPick] = 30; //boomPick is basically group number

        //Pick a random group for Decline , must not be in 
        int declinePick;
        do{
            declinePick = rand() %8;
        }while (declinePick == boomPick || game -> declineCooldown[declinePick] > 0);

        game -> declinedGroup = (PropertyGroup)declinePick; //Get what group in enum
        game -> declineRoundsRemaining = 10;
        game -> declineCooldown[declinePick] = 30;

        printf("\nMarket Boom Group : %d\n",boomPick);
        printf("\nMarket Decline Group : %d\n",declinePick);
    
}

void triggerDisaster(Player *players, Square *squares){
    int developedProperties[BOARD_SIZE];
    int count = 0;

    for(int i = 0; i < BOARD_SIZE; i++){
        if(squares[i].owner != -1 && (squares[i].numHouses > 0 || squares[i].hasHotel == 1)){
            developedProperties[count] = i;
            count++;
        }
    } 
    if (count == 0){
        return; // No developed properties to affect
    }

    int pick =developedProperties[rand() % count];
    Square *affectedSquare = &squares[pick];
    int ownerIndex = affectedSquare->owner;

    const char *disasterTypes[] = {"Fire", "Flood","Riot","Building Collapse","Electric Failure"};
    int disasterIndex = rand() % 5;

    int repairCost = affectedSquare->marketPrice;
    if(affectedSquare->hasHotel == 1){
        repairCost += affectedSquare->baseHotelCost;
    }else if(affectedSquare->numHouses > 0){
        repairCost += affectedSquare->numHouses * affectedSquare->baseHouseCost;
    }

    printf("\n%s occurred.\n", disasterTypes[disasterIndex]);
    printf("Affected Property: %s owned by %s\n", affectedSquare->name, players[ownerIndex].name);
    
    //This is for damage building cannot be used for rent or mortgage until repaired
    affectedSquare->isDamaged = 1; // Mark the square as damaged

    if(affectedSquare -> insurancePolicyType != None_Insurance){
        int compensationPercent = 0;

        if(affectedSquare -> insurancePolicyType == Basic_Insurance){
            compensationPercent = 80;
        }else{
            compensationPercent = 100;
        }

        int compensationAmount = (repairCost * compensationPercent) / 100;
        players[ownerIndex].cash += compensationAmount;

        printf("Insurance Claim Approved\n");
        printf("Compensation Received: LKR %d\n", compensationAmount);
    }else{
        players[ownerIndex].cash -= repairCost;
        printf("%s paid full repair cost of LKR %d due to lack of insurance.\n", players[ownerIndex].name, repairCost);
        checkBankruptcy(&players[ownerIndex], ownerIndex, squares); // Check for bankruptcy after paying repair cost
    }
}