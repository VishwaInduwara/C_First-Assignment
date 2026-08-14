#include <stdlib.h>
#include <stdio.h>
#include "types.h"


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
        printf("\nMarket Decline Group : %d\n",boomPick);
    
}