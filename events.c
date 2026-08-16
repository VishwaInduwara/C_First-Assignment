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

void triggerDisaster(Player *players, Square *squares,GameState *game){
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

    int repairCost = currentMarketValue(affectedSquare); // Use current market value for repair cost
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
        
        printf("%s must pay the full repair cost of LKR %d due to lack of insurance.\n", players[ownerIndex].name, repairCost);
        
    }
}


// events.c
void initEventDeck(GameState *game) {
     for (int i = 0; i < NUM_NATIONAL_CARDS; i++) {
        game->nationalCardDeck[i] = i;
    }
    for (int i = NUM_NATIONAL_CARDS - 1; i > 0; i--) {   // Fisher-Yates
        int j = rand() % (i + 1);
        int temp = game->nationalCardDeck[i];
        game->nationalCardDeck[i] = game->nationalCardDeck[j];
        game->nationalCardDeck[j] = temp;
    }
    game->nationalCardTop = 0;
}

NationalCardId drawEventCard(GameState *game) {
    NationalCardId drawn = game->nationalCardDeck[game->nationalCardTop];

    // move this card to the back of the deck
    for (int i = game->nationalCardTop; i < NUM_NATIONAL_CARDS - 1; i++) {
        game->nationalCardDeck[i] = game->nationalCardDeck[i + 1];
    }
    game->nationalCardDeck[NUM_NATIONAL_CARDS - 1] = drawn;

    return drawn;
}

static void registerNationalEffect(GameState *game ,int cardID,int targetPlayer,int targetProperty,int rounds){
    for(int i=0; i<MAX_ACTIVE_NATIONAL_EFFECTS;i++){
        NationalEffect *effect = &game -> nationalEffects[i];
        if(effect -> isActive && effect -> cardID == cardID && effect -> targetPlayer == targetPlayer && effect -> targetProperty == targetProperty){
            effect -> roundsRemaining = rounds; //Refresh duration
            return;
        } 
    }

    for(int i=0; i<MAX_ACTIVE_NATIONAL_EFFECTS; i++){
        if(!game -> nationalEffects[i].isActive){
            game -> nationalEffects[i].cardID = cardID;
            game -> nationalEffects[i].targetPlayer = targetPlayer;
            game -> nationalEffects[i].targetProperty = targetProperty;
            game -> nationalEffects[i].roundsRemaining = rounds;
            game -> nationalEffects[i].isActive = 1;
            return;
        }
    }
}

static int isCostalProperty(int squareIndex){
    int costal[] = {1,3,6,8,9,16,18,19,26,27,29};
    int n = sizeof(costal) / sizeof(costal[0]); //Find how many elements are in the array
    for(int i=0; i<n; i++){
        if(costal[i] == squareIndex){
            return 1;
        }
    }
    return 0;
}

static int isCommercialProperty(Square *square){
    return(square->type == SQ_RAILWAY || square->type == SQ_UTILITY);
}

int isNationalEffectActive(GameState *game,int cardID,int playerIndex){
    for(int i=0;i<MAX_ACTIVE_NATIONAL_EFFECTS;i++){
        NationalEffect *e = &game->nationalEffects[i];
        if(e->isActive && e->roundsRemaining>0 && e->cardID==cardID && e->targetPlayer==playerIndex) return 1;
    }
    return 0;
}

int isPropertyClosed(GameState *game,int propertyIndex){
    for(int i=0;i<MAX_ACTIVE_NATIONAL_EFFECTS;i++){
        NationalEffect *e = &game->nationalEffects[i];
        if(e->isActive && e->roundsRemaining>0 && e->cardID==CARD_POLITICAL_RALLY && e->targetProperty==propertyIndex) return 1;
    }
    return 0;
}

void tickNationalEffects(GameState *game){
    for(int i=0;i<MAX_ACTIVE_NATIONAL_EFFECTS;i++){
        NationalEffect *e = &game->nationalEffects[i];
        if(!e->isActive) continue;
        e->roundsRemaining--;
        if(e->roundsRemaining <= 0){
            e->isActive = 0;
            printf("A National Event Card effect has expired.\n");
        }
    }
}

int isRegulationActive(GameState *game, GovernmentRegulation reg){
    return (game -> activeRegulation == (int)reg && game -> regulationRoundsRemaining > 0);
}

static const char *regulationNames[] = {
    "Increase Property Tax",
    "Reduce Loan Interest",
    "Housing Subsidy",
    "Luxury Property Tax",
    "Railway Modernization",
    "Electricity Tariff Revision",
    "Insurance Regulation",
    "Anti-Speculation Act"
};

void triggerGovernmentRegulation(GameState *game, Square *squares){
    int reg = rand() % NUM_GOVERNMENT_REGULATIONS;
    game -> activeRegulation = reg;
    game -> regulationRoundsRemaining = 20;

    printf("\n==========================\n");
    printf("Government Regulation (Rule-LK 24)\n");
    printf("==========================\n");
    printf("%s\n", regulationNames[reg]);

    switch(reg){

        case REG_INCREASE_PROPERTY_TAX:
           // squares[4].baseRent = (squares[4].baseRent * 150) / 100;
            printf("\nIncome Tax increased by 50%%. while this regulation is active.\n");
            break;

        case REG_REDUCE_LOAN_INTEREST:
            game -> currentLoanInterest -= 0.02;
            if(game -> currentLoanInterest < 0.0){
                game -> currentLoanInterest = 0.0;
            }
            printf("Loan interest reduced by 2%% (now %.2f)\n", game -> currentLoanInterest);
            break;
        case REG_HOUSING_SUBSIDY:
            printf("House construction costs reduced by 30%% for 20 rounds\n");
            break;
        case REG_LUXURY_PROPERTY_TAX:
            printf("Hotels pay 25%% maintenance tax each round while active\n");
            break;
        case REG_RAILWAY_MODERNIZATION:
            printf("Railway rents increased by 25%% for 20 rounds\n");
            break;
        case REG_ELECTRICITY_TARIFF:
            printf("Utility rents increased by 20%% for 20 rounds\n");
            break;
        case REG_INSURANCE_REGULATION:
            printf("Insurance premiums decreased by 15%% for 20 rounds\n");
            break;
        case REG_ANTI_SPECULATION:
            printf("Players may own at most 3 undeveloped properties\n");
            break;
        default:
            break;
    }
    printf("\n");
}


static int regionalValuePct(RegionalCardId card){
    switch(card){
        case REGIONAL_PORT_CITY_EXPANSION:  return 125;
        case REGIONAL_IT_GROWTH:            return 120;
        case REGIONAL_NORTHERN_DEVELOPMENT: return 130;
        case REGIONAL_TEA_EXPORT:           return 135;
        case REGIONAL_UNIVERSITY_GROWTH:    return 120;
        case REGIONAL_FLOOD_DAMAGE:         return 80;
        case REGIONAL_WATER_SHORTAGE:       return 90;
        default:                            return 100;
    }
}

static void regionalValueSquares(RegionalCardId card, int idx[], int *n){
    static const int coastal[] = {1,3,6,8,9,16,18,19,26,27,29};
    int size = sizeof(coastal)/sizeof(coastal[0]);
    switch(card){
        case REGIONAL_PORT_CITY_EXPANSION:  
            idx[0]=1; 
            idx[1]=3; 
            idx[2]=5; 
            *n=3; 
            break;

        case REGIONAL_IT_GROWTH:            
            idx[0]=11; 
            idx[1]=13; 
            idx[2]=14;
            *n=3; 
            break;

        case REGIONAL_NORTHERN_DEVELOPMENT: 
            idx[0]=31; idx[1]=32; idx[2]=34; 
            *n=3; 
            break;

        case REGIONAL_TEA_EXPORT:           
            idx[0]=37; 
            *n=1; 
            break;

        case REGIONAL_UNIVERSITY_GROWTH:    
            idx[0]=21; idx[1]=23; *n=2; 
            break;

        case REGIONAL_FLOOD_DAMAGE:
            for(int i=0;i<size;i++) {
                idx[i]=coastal[i];
            }
            *n=size; 
                break;
        case REGIONAL_WATER_SHORTAGE:      
            idx[0]=26; 
            idx[1]=27; 
            idx[2]=29; 
            *n=3; break;
        default:                            
            *n=0; break;
    }
}

//This array is used to print the name of the regional card when drawn
static const char *regionalCardNames[] = {
    "Southern Tourism Boom", "Port City Expansion", "IT Industry Growth",
    "Northern Development Programme", "Tea Export Boom", "Airport Expansion",
    "University City Growth", "Beach Pollution", "Flood Damage",
    "Transport Strike", "Electricity Tariff Increase", "Water Shortage"
};

int isRegionalCardActive(GameState *game, RegionalCardId card){
    return (game -> activeRegionalCard == (int)card && game -> regionalCardRoundsRemaining > 0);
}

void revertRegionalCard(GameState *game, Square *squares){
    if(game -> activeRegionalCard == -1){
       return;
    }
    int pct = regionalValuePct((RegionalCardId)game -> activeRegionalCard);
    int idx[BOARD_SIZE], n;

    regionalValueSquares((RegionalCardId)game -> activeRegionalCard, idx, &n);
    if(pct == 100 || n == 0){
        return;
    }

    for(int i = 0; i < n; i++){
        squares[idx[i]].marketPrice = (squares[idx[i]].marketPrice * 100) / pct;
    }
}

void triggerRegionalDevelopment(GameState *game, Square *squares){
    revertRegionalCard(game, squares); // undo previous value changes (Rule-LK 35)

    int card = rand() % NUM_REGIONAL_CARDS;
    game -> activeRegionalCard = card;
    game -> regionalCardRoundsRemaining = 15;

    printf("\n==========================================================\n");
    printf("Regional Development Card (Rule-LK 35)\n");
    printf("==========================================================\n");
    printf("%s\n", regionalCardNames[card]);

    int pct = regionalValuePct((RegionalCardId)card);
    int idx[BOARD_SIZE], n;
    regionalValueSquares((RegionalCardId)card, idx, &n);
    for(int i = 0; i < n; i++){
        squares[idx[i]].marketPrice = (squares[idx[i]].marketPrice * pct) / 100;
    }
    printf("\n");
}

void printActiveRegionalCard(GameState *game){
    if(game -> activeRegionalCard != -1 && game -> regionalCardRoundsRemaining > 0){
        printf("%s\n", regionalCardNames[game -> activeRegionalCard]);
        printf("Rounds Remaining : %d\n\n", game -> regionalCardRoundsRemaining);
    }else{
        printf("No active regional development card\n\n");
    }
}





int countUndevelopedProperties(int playerIndex, Square *squares){
    int count = 0;
    for(int i = 0; i < BOARD_SIZE; i++){
        if(squares[i].owner == playerIndex &&
           squares[i].numHouses == 0 && squares[i].hasHotel == 0){
            count++;
        }
    }
    return count;
}

int canPurchaseProperty(GameState *game, int playerIndex, Square *squares){
    if(isRegulationActive(game, REG_ANTI_SPECULATION)){
        return countUndevelopedProperties(playerIndex, squares) < 3;
    }
    return 1;
}



//The effect application function
void applyEventCard(NationalCardId card,Player *players,int playerIndex, Square *squares,GameState *game){
    printf("\n%s drew the card\n", players[playerIndex].name);
    switch(card){
        case CARD_TOURISM_HYPE:
            registerNationalEffect(game,card,playerIndex,-1,5);
            printf("Tourism Hype: %s's hotels earn double rent for 5 rounds\n", players[playerIndex].name);
            break;
        
        case CARD_FUEL_SHORTAGE:
            registerNationalEffect(game,card,playerIndex,-1,5);
            printf("Fuel Shortage: %s's railways earn double rent for 5 rounds\n", players[playerIndex].name);
            break;

        case CARD_HEAVY_FLOODS:{
            int coastal[BOARD_SIZE], count = 0;
            for(int i=0;i<BOARD_SIZE;i++) if(isCostalProperty(i)) coastal[count++] = i;
                if(count > 0){
                    int r = coastal[rand() % count];
                    squares[r].isDamaged = 1;
                    printf("Heavy Floods: %s is damaged\n", squares[r].name);
                }
                break;
            }

        case CARD_POLITICAL_RALLY:{
            int candidates[BOARD_SIZE], count = 0;
            for(int i=0;i<BOARD_SIZE;i++){
                if(squares[i].owner != -1 && squares[i].propertytype == PropertyType_Regular){
                    candidates[count++] = i;
                }
            }
            if(count > 0){
                int r = candidates[rand() % count];
                registerNationalEffect(game,card,-1,r,2);
                printf("Political Rally: %s is closed for 2 rounds\n", squares[r].name);
            }else{
                printf("Political Rally: No owned property to close\n");
            }
            break;
        }
            

        case CARD_STOCK_MARKET_RISE:{
            for(int i=0; i<BOARD_SIZE; i++){
                squares[i].marketPrice = (int)(squares[i].marketPrice * 1.1);
            }
            printf("Stock Market Rise: All property prices increase by 10%%\n");
            break;
        }

        case CARD_ECONOMIC_DOWNTURN:
            for(int i=0; i<BOARD_SIZE; i++){
                squares[i].marketPrice = (int)(squares[i].marketPrice * 0.85);
            }
            printf("Economic Downturn: All property prices decrease by 15%%\n");
            break;

        case CARD_HOUSING_SUBSIDY:
            registerNationalEffect(game,card,playerIndex,-1,15);
            printf("Housing Subsidy: %s's house construction costs reduced by 30%% for 15 rounds\n", players[playerIndex].name);
            break;

        case CARD_INTEREST_RATE_CUT:
            game -> currentLoanInterest -= 0.02;
            printf("Interest Rate Cut: Current loan interest rate decreased by 2%%\n");
            break;

        case CARD_INTEREST_RATE_INCREASE:
            game -> currentLoanInterest += 0.02;
            printf("Interest Rate Increase: Current loan interest rate increased by 2%%\n");
            break;

        case CARD_TAX_AMNESTY:
            for(int i=0; i<MAX_PLAYERS; i++){
                players[i].cash += 2000;
            }
            printf("Tax Amnesties: Each player receives LKR: 2,000\n");
            break;

        case CARD_POWER_FAILURE:
            registerNationalEffect(game,card,playerIndex,-1,3);
            printf("Power Failure: %s's utility income halved for 3 rounds\n", players[playerIndex].name);
            break;

        case CARD_FOREIGN_FUNDING:
            for(int i=0;i<BOARD_SIZE;i++){
                if(squares[i].type == SQ_RAILWAY || squares[i].type == SQ_UTILITY){
                     squares[i].marketPrice = (int)(squares[i].marketPrice * 1.15);
                }
            }
            printf("Foreign Funding: Railway and utility values increased by 15%%\n");
             break;

        case CARD_PORT_EXPANSION:
            for(int i=0; i<BOARD_SIZE; i++){
                if(squares[i].type == SQ_RAILWAY){
                    squares[i].marketPrice = (int)(squares[i].marketPrice * 1.2);
                }
            }
            printf("Port Expansion: Railway property values increase by 20%%\n");
            break;

        case CARD_FESTIVAL_SEASON:
            registerNationalEffect(game,card,playerIndex,-1,15);
            printf("Festival Season: %s's hotels earn 50%% extra rent for 15 rounds\n", players[playerIndex].name);
            break;

        case CARD_LABOUR_STRIKE:
            registerNationalEffect(game,card,playerIndex,-1,2);
            printf("Labour Strike: %s cannot construct buildings for 2 rounds\n", players[playerIndex].name);
            break;

        case CARD_INSURANCE_DISCOUNT:
            registerNationalEffect(game,card,playerIndex,-1,15);
            printf("Insurance Discount: %s's insurance premiums reduced by 20%% for 15 rounds\n", players[playerIndex].name);
            break;

        case CARD_PROPERTY_REVALUATION:{
            int group = rand() % 8;
            for(int i=0; i<BOARD_SIZE; i++){
                if(squares[i].group == (PropertyGroup)group){
                     squares[i].marketPrice =(int)(squares[i].marketPrice *1.15);
                }
            }
            printf("Property Revaluation: Properties in group %d increase by 15%%\n", group);
            break;
        }

        case CARD_CURRENCY_DEPRECIATION:
            registerNationalEffect(game,card,playerIndex,-1,15);
            printf("Currency Depreciation: %s's construction costs increased by 10%% for 15 rounds\n", players[playerIndex].name);
            break;

        case CARD_GOVERNMENT_GRANT:{
            int randomPlayer = rand() % MAX_PLAYERS;
            players[randomPlayer].cash += 5000;
            printf("Government Grant: %s receives $5,000\n", players[randomPlayer].name);
            break;
        }

        case CARD_NATIONAL_DISASTER:
            printf("National Disaster: Random developed property damaged\n");
            triggerDisaster(players,squares,game);
            break;

        default:
            printf("Unknown card drawn.\n");
            break;
    }
}

//Rule LK 18: Economic Events
static const char *economicEventNames[] = {
    "Tourism Boom", "Fuel Crisis", "Heavy Monsoon", "Economic Recession",
    "Stock Market Boom", "Government Housing Programme",
    "Foreign Investment", "Political Unrest"
};

static int economicValuePct(EconomicEvent event){
    switch(event){
        case ECON_TOURISM_BOOM:       return 115; //Southern Coastal +15%
        case ECON_HEAVY_MONSOON:      return 90;  //Coastal -10%
        case ECON_ECONOMIC_RECESSION: return 85;  //All -15%
        case ECON_STOCK_MARKET_BOOM:  return 110; //All +10%
        case ECON_FOREIGN_INVESTMENT: return 120; //Railways + Utilities +20%
        default:                      return 100;
    }
}

static void economicValueSquares(EconomicEvent event, int idx[], int *n){
    static const int southernCoastal[] = {26, 27, 29};
    static const int coastal[] = {1,3,6,8,9,16,18,19,26,27,29};
    static const int commercial[] = {5, 15, 25, 35, 12, 28}; //railways + utilities
    switch(event){
        case ECON_TOURISM_BOOM:
            for(int i = 0; i < 3; i++) idx[i] = southernCoastal[i];
            *n = 3; break;
        case ECON_HEAVY_MONSOON:
            for(int i = 0; i < 11; i++) idx[i] = coastal[i];
            *n = 11; break;
        case ECON_FOREIGN_INVESTMENT:
            for(int i = 0; i < 6; i++) idx[i] = commercial[i];
            *n = 6; break;
        case ECON_ECONOMIC_RECESSION:
        case ECON_STOCK_MARKET_BOOM:
            for(int i = 0; i < BOARD_SIZE; i++) idx[i] = i;
            *n = BOARD_SIZE; break;
        default:
            *n = 0; break;
    }
}

int isEconomicEventActive(GameState *game, EconomicEvent event){
    for(int i = 0; i < MAX_ACTIVE_ECONOMIC_EVENTS; i++){
        if(game -> economicEffects[i].eventID == (int)event &&
           game -> economicEffects[i].roundsRemaining > 0){
            return 1;
        }
    }
    return 0;
}

void triggerEconomicEvent(GameState *game, Square *squares){
    int slot = -1;
    for(int i = 0; i < MAX_ACTIVE_ECONOMIC_EVENTS; i++){
        if(game -> economicEffects[i].eventID == -1){ slot = i; break; }
    }
    if(slot == -1){
        printf("Too many economic events active; none triggered.\n");
        return;
    }
    int event = rand() % NUM_ECONOMIC_EVENTS;
    game -> economicEffects[slot].eventID = event;
    game -> economicEffects[slot].roundsRemaining = ECONOMIC_EVENT_DURATION;

    printf("Economic Event: %s (active for %d rounds)\n",
           economicEventNames[event], ECONOMIC_EVENT_DURATION);

    int pct = economicValuePct((EconomicEvent)event);
    if(pct != 100){
        int idx[BOARD_SIZE], n;
        economicValueSquares((EconomicEvent)event, idx, &n);
        for(int i = 0; i < n; i++){
            squares[idx[i]].marketPrice = (squares[idx[i]].marketPrice * pct) / 100;
        }
    }
}

void tickEconomicEvents(GameState *game, Square *squares){
    for(int i = 0; i < MAX_ACTIVE_ECONOMIC_EVENTS; i++){
        if(game -> economicEffects[i].eventID == -1) continue;
        game -> economicEffects[i].roundsRemaining--;
        if(game -> economicEffects[i].roundsRemaining <= 0){
            int event = game -> economicEffects[i].eventID;
            printf("Economic Event ended: %s\n", economicEventNames[event]);
            int pct = economicValuePct((EconomicEvent)event);
            if(pct != 100){ //Rule-LK 34: revert this event's changes (order-safe)
                int idx[BOARD_SIZE], n;
                economicValueSquares((EconomicEvent)event, idx, &n);
                for(int j = 0; j < n; j++){
                    squares[idx[j]].marketPrice = (squares[idx[j]].marketPrice * 100) / pct;
                }
            }
            game -> economicEffects[i].eventID = -1;
            game -> economicEffects[i].roundsRemaining = 0;
        }
    }
}

void printActiveEconomicEvents(GameState *game){
    printf("\nActive Economic Events\n----------------------\n");
    int any = 0;
    for(int i = 0; i < MAX_ACTIVE_ECONOMIC_EVENTS; i++){
        if(game -> economicEffects[i].eventID != -1){
            any = 1;
            printf("%s (%d rounds remaining)\n",
                   economicEventNames[game -> economicEffects[i].eventID],
                   game -> economicEffects[i].roundsRemaining);
        }
    }
    if(!any) printf("None\n");
    printf("\n");
}
