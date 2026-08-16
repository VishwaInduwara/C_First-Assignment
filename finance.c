#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#include "types.h"
#include "events.h"
#include "finance.h"

// Rent Calculation
int calculateRent(Square *landedSquare, Square *squares, int diceRoll,GameState *game) {
    int rent = 0;

    if (landedSquare->type == SQ_PROPERTY) {
        if(landedSquare -> hasHotel){
            rent = 10*(landedSquare -> baseRent);
        }else{
            switch (landedSquare -> numHouses){
                case 1:
                    rent = 2*(landedSquare -> baseRent);
                    break;
                case 2:
                    rent = 3*(landedSquare -> baseRent);
                    break;
                case 3:
                    rent = 5*(landedSquare -> baseRent);
                    break;
                case 4:
                    rent = 7*(landedSquare -> baseRent);
                    break;
                default:
                    rent = landedSquare -> baseRent;
            }
        }
        

    } else if (landedSquare->type == SQ_RAILWAY) {
        int railwayCount = 0;
        int ownerIndex = landedSquare->owner;

        for (int j = 0; j < BOARD_SIZE; j++) {
            if (squares[j].type == SQ_RAILWAY && squares[j].owner == ownerIndex) {
                railwayCount++;
            }
        }
        if (railwayCount == 1){
            rent = 250;
        }else if (railwayCount == 2){
            rent = 500;
        }else if (railwayCount == 3){
            rent = 1000;
        }else if (railwayCount == 4){
            rent = 2000;
        }

    } else if (landedSquare->type == SQ_UTILITY) {
        int utilityCount = 0;
        int ownerIndex = landedSquare->owner;
        for (int k = 0; k < BOARD_SIZE; k++) {
            if (squares[k].type == SQ_UTILITY && squares[k].owner == ownerIndex) {
                utilityCount++;
            }
        }
        if (utilityCount == 1){
            rent = diceRoll * 4;
        }
        else if (utilityCount == 2){
            rent = diceRoll * 10;
        }
    }
    if(landedSquare -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        rent = (rent *125)/100; //Rule LK 31
    }else if(landedSquare -> group == game -> declinedGroup && game -> declineRoundsRemaining > 0){
        rent = (rent *80)/100; //Rule LK 32
    }

    //Check for government regulations affecting rent
    if(landedSquare -> type == SQ_RAILWAY && isRegulationActive(game, REG_RAILWAY_MODERNIZATION)){
        rent = (rent * 125) / 100;
    }else if(landedSquare -> type == SQ_UTILITY && isRegulationActive(game, REG_ELECTRICITY_TARIFF)){
        rent = (rent * 120) / 100;
    }

    //Check for regional cards affecting rent
    if(isRegionalCardActive(game, REGIONAL_SOUTHERN_TOURISM) &&
       (landedSquare->index == 26 || landedSquare->index == 27 || landedSquare->index == 29)){
        rent = (rent * 140) / 100;
    }
    if(isRegionalCardActive(game, REGIONAL_BEACH_POLLUTION) &&
       (landedSquare->index == 26 || landedSquare->index == 27 || landedSquare->index == 29)){
        rent = (rent * 70) / 100;
    }
    if(isRegionalCardActive(game, REGIONAL_AIRPORT_EXPANSION) &&
       (landedSquare->index == 16 || landedSquare->index == 18 || landedSquare->index == 19)){
        rent = (rent * 130) / 100;
    }
    if(landedSquare -> type == SQ_RAILWAY && isRegionalCardActive(game, REGIONAL_TRANSPORT_STRIKE)){
        rent = (rent * 60) / 100;
    }
    if(landedSquare -> type == SQ_UTILITY && isRegionalCardActive(game, REGIONAL_ELECTRICITY_TARIFF)){
        rent = (rent * 125) / 100;
    }
    if(landedSquare -> index == 28 && isRegionalCardActive(game, REGIONAL_WATER_SHORTAGE)){
        rent = (rent * 120) / 100;
    }

    if(isPropertyClosed(game,landedSquare->index)){
        printf("Property is closed due to Political Rally\n");
        return 0; //Rule LK 33
    }
    if(landedSquare -> owner != -1){
        if(landedSquare -> hasHotel){
            if(isNationalEffectActive(game,CARD_TOURISM_HYPE,landedSquare->owner)){
                rent *= 2; //Double rent for hotels
            }
            if(isNationalEffectActive(game,CARD_FESTIVAL_SEASON,landedSquare -> owner)){
                rent += rent / 2; //50% extra rent for hotels
            }
            if(isEconomicEventActive(game, ECON_TOURISM_BOOM)){
                rent *= 2; //Rule-LK 18: hotels earn double rent
            }
            if(isEconomicEventActive(game, ECON_POLITICAL_UNREST)){
                rent /= 2; //Rule-LK 18: hotel rent drops by 50%
            }
        }
        if(landedSquare -> type == SQ_RAILWAY){
            if(isNationalEffectActive(game,CARD_FUEL_SHORTAGE,landedSquare -> owner)){
                rent *= 2; //Double rent for railways
            }
            if(isEconomicEventActive(game, ECON_FUEL_CRISIS)){
                rent *= 2; //Rule-LK 18: railway rent doubles
            }

        }
        if(landedSquare -> type == SQ_UTILITY){
            if(isNationalEffectActive(game,CARD_POWER_FAILURE,landedSquare -> owner)){
                rent /= 2; //Halve rent for utilities
            }
        }
        //Rule-LK 16: depreciation reduces rental income
        if(landedSquare -> depreciationPct > 0){
            rent = (rent * (100 - landedSquare -> depreciationPct)) / 100;
        }
        //Rule-LK 17: each renovation gives a permanent +5% rent bonus (cap +50%)
        if(landedSquare -> renovationLevel > 0){
            int bonus = landedSquare -> renovationLevel * 5;
            if(bonus > 50) bonus = 50;
            rent = (rent * (100 + bonus)) / 100;
        }

        //Rule-LK 26: building condition affects rent (Table 3); closed building collects nothing
        if(landedSquare -> numHouses > 0 || landedSquare -> hasHotel == 1){
            int condFactor = buildingConditionRentFactor(landedSquare -> buildingCondition);
        if(condFactor == 0){
            printf("%s is closed for maintenance. No rent collected.\n", landedSquare -> name);
            return 0;
        }

        rent = (rent * condFactor) / 100;
    }
    //Rule-LK 28: structural damage reduces maximum rent by 25%
    if(landedSquare -> isStructurallyDamaged){
        rent = (rent * 75) / 100;
    }
    }

    if(isEconomicEventActive(game, ECON_ECONOMIC_RECESSION)){
        rent = (rent * 90) / 100; //Rule-LK 18: rent -10%
    }

    return rent;
}


int checkBankruptcy(Player *p,int playerIndex,Square *squares){
    if (p->cash < 0) {
        p->isBankrupt = 1;
        printf("%s has been declared bankrupt.\n", p->name);
        
        //Rule 14 - remove buildings, expire insurance, loans due immediately, transfer remaining assets
        for(int i = 0; i < BOARD_SIZE; i++) {
            if(squares[i].owner == playerIndex) {
                squares[i].owner = -1; // Return to bank
                squares[i].numHouses = 0; // Remove houses
                squares[i].hasHotel = 0; // Remove hotel
                squares[i].insurancePolicyType = None_Insurance; // Expire insurance
                squares[i].insuranceRoundsRemaining = 0; // Reset insurance rounds
                squares[i].isLoanLocked = 0; // Unlock property from loan
                squares[i].isMortgaged = 0; //clear mortgage when returned to the bank
                
                squares[i].propertyAge = 0;
                squares[i].depreciationPct = 0;

                squares[i].buildingCondition = 100;
                squares[i].roundsWithoutMaintenance = 0;
                squares[i].isStructurallyDamaged = 0;

            }
        }

        //Loan become void
        p->hasActiveLoan = 0;
        p->loanAmount = 0;
        p->loanTurnsRemaining = 0;
        p->numOwnedProperties = 0;

        printf("All buildings removed, insurance expired, properties transferred to bank.\n");
        return 1; // Player is bankrupt
    }
    return 0;
}



void payTax(Player *p, int taxAmount,int currentPlayerIndex,Square *squares,GameState *game) {
    printf("%s must pay LKR %d in tax.\n", p->name, taxAmount);

    if (p->cash < taxAmount) {
        raiseCashByMortgage(p, currentPlayerIndex, squares, game, taxAmount);//mortgage to cover tax
    }    

    if(p->cash >= taxAmount){
        p->cash -= taxAmount;

        printf("Paid Income Tax: LKR %d.\n", taxAmount);
        printf("Remaining Balance : LKR %d.\n", p->cash);

    } else {
        p->cash -= taxAmount; // This will make cash negative
        printf("%s cannot afford Income Tax!\n", p->name);
    }

    checkBankruptcy(p,currentPlayerIndex,squares);
}

int calculateMaxLoan(int playerIndex,Square *squares,GameState *game){
    int totalMortgageValue = 0;
//Must be owned by player and not currently mortgaged 
    for (int i=0 ; i<BOARD_SIZE ; i++){
         if(squares[i].owner == playerIndex && squares[i].isLoanLocked == 0 && squares[i].isMortgaged == 0){
            if(squares[i].group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
                totalMortgageValue += (squares[i].baseMortgageValue * 115)/100; //Rule LK 31
            }else if(squares[i].group == game -> declinedGroup && game -> declineRoundsRemaining > 0){
                totalMortgageValue += (squares[i].baseMortgageValue * 90)/100; //Rule LK 32
            }else{
                totalMortgageValue += squares[i].baseMortgageValue;
            }
        }
    }

    //75% of eligible property collateral
    return (totalMortgageValue * 75)/100;
}

int mortgageValue(Square *square, GameState *game){
    int value = square -> baseMortgageValue;
    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        value = (value * 115) / 100; //Rule LK 31
    }else if(square -> group == game -> declinedGroup && game -> declineRoundsRemaining > 0){
        value = (value * 90) / 100; //Rule LK 32
    }
    return value;
}

int findMortgageableProperty(int playerIndex, Square *squares, GameState *game){
    int best = -1;
    int bestValue = -1;
    for(int i = 0; i < BOARD_SIZE; i++){
        if(squares[i].owner != playerIndex) continue;
        if(squares[i].isMortgaged) continue;
        if(squares[i].isLoanLocked) continue;                              //Rule-LK 3
        if(squares[i].numHouses > 0 || squares[i].hasHotel == 1) continue; //must be undeveloped
        int v = mortgageValue(&squares[i], game);
        if(v > bestValue){ bestValue = v; best = i; }
    }
    return best;
}

void raiseCashByMortgage(Player *p, int playerIndex, Square *squares, GameState *game, int amount){
    while(p -> cash < amount){
        int idx = findMortgageableProperty(playerIndex, squares, game);
        if(idx == -1) break;
        int value = mortgageValue(&squares[idx], game);
        p -> cash += value;
        squares[idx].isMortgaged = 1;
        printf("%s mortgaged %s for LKR %d to cover the payment.\n", p -> name, squares[idx].name, value);
    }
    if(p -> cash < amount){
        printf("%s could not raise enough cash by mortgaging.\n", p -> name);
    }
}

void repayLoan(Player *p, int repaymentAmount,Square *squares,int playerIndex){
    //repaymentAmoount comes from players.c strategy function
    if(!p -> hasActiveLoan){
        printf("%s has no active loan to repay.\n",p -> name);
        return;
    }

    if(repaymentAmount > p -> cash){
        repaymentAmount = p -> cash; //Repay as much as possible
    }
    p -> cash -= repaymentAmount;
    p -> loanAmount -= repaymentAmount;

    printf("%s repaid LKR %d towards the loan.\n",p -> name,repaymentAmount);
    printf("Remaining Loan Amount: LKR %d\n",p -> loanAmount);

    if(p -> loanAmount <= 0){
        p -> loanAmount = 0;
        p -> hasActiveLoan = 0;
        p -> loanTurnsRemaining = 0;

        //Unlock all properties that were locked for the loan
        for(int i=0 ; i<BOARD_SIZE ; i++){
            if(squares[i].owner == playerIndex){
                squares[i].isLoanLocked = 0;
            }
        }
        printf("%s has fully repaid the loan and all properties are unlocked.\n",p -> name);
    }
}

//This will call in handelBankSquare in game.c when player decide to do it in players.c
void extendLoanTurn(Player *p){
    p -> loanTurnsRemaining = 20; //Reset to 20 turns
    printf("%s has extended the loan repayment period to 20 turns.\n",p -> name);
}

//This will call in handelBankSquare in game.c when player decide to do it in players.c
void increaseLoanAmount(Player *p, int playerIndex, Square *squares,GameState *game){
    int maxLoan = calculateMaxLoan(playerIndex,squares,game);
    if(maxLoan > p->loanAmount){
        int increaseAmount = maxLoan - p->loanAmount;
        p->cash += increaseAmount;
        p->loanAmount = maxLoan;
        printf("%s has increased the loan amount by LKR %d. New loan amount: LKR %d\n", p->name, increaseAmount, p->loanAmount);
    }else{
        printf("%s has no additional collateral to increase the loan amount.\n", p->name);
    }
}

//Helper function to build house to check can build houses
//Rule 9
int canBuildEvenly(Square *squares , PropertyGroup group ,  int propertyIndex){
    for(int i = 0; i<BOARD_SIZE; i++){
        if(squares[i].group == group && squares[i].propertytype == PropertyType_Regular){
            if(squares[i].numHouses < squares[propertyIndex].numHouses){
                return 0;//another property in the group has fewer , so can't build
            }
        }
    }
    return 1;//This property has fewest house , Can build
}

void buildHouse(Player *p, int playerIndex, Square *squares, Square *square, GameState *game){
    printf("%s is attempting to build a house on %s.\n", p->name, square->name);
    if(square -> isMortgaged == 1){
        printf("%s cannot build on %s - property is mortgaged.\n", p->name, square->name);
        return;
    }
    if(square -> numHouses >= 4 || square -> hasHotel == 1){
        return;
    }

    int actualCost = square -> baseHouseCost;
    if(isNationalEffectActive(game,CARD_HOUSING_SUBSIDY,playerIndex)){    
    actualCost = (actualCost * 70) / 100;
    }
    if(isRegulationActive(game, REG_HOUSING_SUBSIDY)){
        actualCost = (actualCost * 70) / 100;
    }

    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        actualCost = (square -> baseHouseCost * 110) / 100;
    }

    if(p -> cash < actualCost){
        return;
    }
    if(!canBuildEvenly(squares, square->group, square->index)){
        return;
    }

    if(isEconomicEventActive(game, ECON_FUEL_CRISIS)){
        actualCost = (actualCost * 120) / 100; //Rule-LK 18: development costs +20%
    }
    if(isEconomicEventActive(game, ECON_HOUSING_PROGRAMME)){
        actualCost = (actualCost * 75) / 100; //Rule-LK 18: house construction -25%
    }

    p -> cash -= actualCost;
    square -> numHouses++;

    square -> buildingCondition = 100;      //Rule-LK 25: new building starts at 100%
    square -> roundsWithoutMaintenance = 0;
    square -> isStructurallyDamaged = 0;

    checkBankruptcy(p, playerIndex, squares);   // ADD THIS

    printf("%s construct one house on %s.\n", p->name, square->name);
    printf("Construction Cost : LKR %d\n", actualCost);
}

void buildHotel(Player *p, int playerIndex, Square *square, GameState *game,Square *squares){
    if(square -> isMortgaged == 1){
        printf("%s cannot build on %s - property is mortgaged.\n", p->name, square->name);
        return;
    }
    if(square -> numHouses != 4 || square -> hasHotel == 1){
        return;
    }

    int actualCost = square -> baseHotelCost;
    if(isNationalEffectActive(game,CARD_CURRENCY_DEPRECIATION,playerIndex)){
    actualCost = (actualCost * 110) / 100;
    }
    if(isRegulationActive(game, REG_HOUSING_SUBSIDY)){
        actualCost = (actualCost * 70) / 100;
    }

    if(isEconomicEventActive(game, ECON_FUEL_CRISIS)){
        actualCost = (actualCost * 120) / 100; //Rule-LK 18: development costs +20%
    }

    if(square -> group == game -> boomedGroup && game -> boomRoundsRemaining > 0){
        actualCost = (square -> baseHotelCost * 110) / 100;
    }

    if(p -> cash < actualCost){
        return;
    }

    p -> cash -= actualCost;
    square -> numHouses = 0;
    square -> hasHotel = 1;
    p -> numHotelCount++;

    square -> buildingCondition = 100;      //Rule-LK 25: new building starts at 100%
    square -> roundsWithoutMaintenance = 0;
    square -> isStructurallyDamaged = 0;

    checkBankruptcy(p, playerIndex, squares);  

    printf("%s upgraded %s to a Hotel", p->name, square->name);
}

//Property Depreciation and Renovation Functions
int buildingConditionRentFactor(int condition){
    if(condition >= 90) return 100;
    if(condition >= 75) return 90;
    if(condition >= 50) return 75;
    if(condition >= 25) return 50;
    return 0; //below 25%: building closed
}

int maintenanceCost(Square *square){
    int cost = 0;
    if(square -> hasHotel == 1){
        cost = (square -> baseHotelCost * 8) / 100; //Rule-LK 27: hotel = 8% of construction cost
    }else if(square -> numHouses > 0){
        cost = ((square -> baseHouseCost * 5) / 100) * square -> numHouses; //house = 5% each
    }
    if(square -> isStructurallyDamaged){
        cost = (cost * 150) / 100; //Rule-LK 28: future maintenance costs +50%
    }
    return cost;
}

void maintainProperty(Player *p, int playerIndex, Square *squares, Square *square){
    if(square -> buildingCondition >= 100) return;
    int cost = maintenanceCost(square);
    if(p -> cash < cost){
        printf("%s cannot afford maintenance on %s (cost LKR %d).\n", p -> name, square -> name, cost);
        return;
    }
    p -> cash -= cost;
    square -> buildingCondition = 100;        //Rule-LK 27: restore condition
    square -> roundsWithoutMaintenance = 0;   //reset ignored-rounds counter
    
    checkBankruptcy(p, playerIndex, squares);
    printf("%s maintained %s. Condition restored to 100%%. Cost: LKR %d\n", p -> name, square -> name, cost);
}

void renovateDamagedBuilding(Player *p, int playerIndex, Square *squares, Square *square){
    if(!square -> isStructurallyDamaged){
        return;
    }
    int replacementValue;

    if (square->hasHotel == 1) {
        replacementValue = square->baseHotelCost;
    } else {
        replacementValue = square->numHouses * square->baseHouseCost;
    }

    int cost = (replacementValue * 25) / 100; //Rule-LK 29: 25% of replacement value
    
    if(p -> cash < cost){
        printf("%s cannot afford to renovate the damaged building on %s (cost LKR %d).\n",
            p -> name, square -> name, cost);
        return;
    }

    p -> cash -= cost;
    square -> isStructurallyDamaged = 0;   //Rule-LK 29: restores value, rent, condition
    square -> buildingCondition = 100;
    square -> roundsWithoutMaintenance = 0;
    checkBankruptcy(p, playerIndex, squares);
    printf("%s renovated the damaged building on %s for LKR %d.\n", p -> name, square -> name, cost);
}


int currentMarketValue(Square *square){
    int value = square -> marketPrice;
    value = (value * (100 - square -> depreciationPct)) / 100; //Rule-LK 16 depreciation
    if(square -> isStructurallyDamaged){
        value = (value * 85) / 100; //Rule-LK 28: structural damage reduces value 15%
    }
    return value;
}

//Calculate total property assets for a player (excluding buildings)
int totalPropertyAssets(int playerIndex, Square *squares){
    int total = 0;
    for(int i = 0; i < BOARD_SIZE; i++){
        if(squares[i].owner == playerIndex){
            total += currentMarketValue(&squares[i]); //current market value of the property only; buildings excluded
        }
    }
    return total;
}

int marketTaxRate(GameState *game, int baseRate){
    int rate = baseRate;
    if(game -> boomRoundsRemaining > 0){
        rate = (rate * 115) / 100; //market boom increases the rate
    }else if(game -> declineRoundsRemaining > 0){
        rate = (rate * 90) / 100; //market decline reduces the rate
    }
    rate = (int)(rate * (1 + game -> currentInflactionRate)); //inflation also affects the rate
    return rate;
}

void renovateProperty(Player *p, int playerIndex, Square *squares, Square *square){
    if(square -> depreciationPct <= 0 && square -> propertyAge == 0){
        printf("%s's %s is already in good condition.\n", p -> name, square -> name);
        return;
    }
    int cost = (currentMarketValue(square) * 10) / 100; //Rule-LK 17: 10% of current market value
    if(p -> cash < cost){
        printf("%s cannot afford to renovate %s (cost LKR %d).\n", p -> name, square -> name, cost);
        return;
    }
    p -> cash -= cost;
    square -> depreciationPct = 0; //restore value and rent
    square -> propertyAge = 0;     //reset age
    square -> renovationLevel++;   //permanent rent increase

    checkBankruptcy(p, playerIndex, squares);
    printf("%s renovated %s for LKR %d. Rental income increased.\n", p -> name, square -> name, cost);
}

//get random inflation
void calculateInflation(Square *squares,int currentRound,GameState *game){
    float inflation[6]={-0.03,00.0,0.02,0.05,0.08,0.12};

    float InflationRate = inflation[rand()%6];
    if(InflationRate>=0.08){
        game -> currentLoanInterest = 0.12;
    }else{
        game -> currentLoanInterest = 0.1;
    }
    game -> currentInflactionRate = InflationRate;
    
    for(int i=0 ; i<BOARD_SIZE;i++){
        squares[i].marketPrice = (int)((squares[i].marketPrice)*(1+InflationRate));
    }
}

void purchaseInsurance(Player *p, Square *square, int policyChoice, GameState *game){
    if(policyChoice == 0){
        printf("%s chose not to purchase insurance\n", p->name);
        return;
    }
    
    int premium = 0;
    PolicyType type;
    
    if(policyChoice == 1){
        premium = (int)(currentMarketValue(square) * 0.05);
        type = Basic_Insurance;
    }else if(policyChoice == 2){
        premium = (int)(currentMarketValue(square) * 0.1);
        type = Comprehensive_Insurance;
    }else{
        type = Business_Interruption_Insurance;
        premium = (int)(currentMarketValue(square) * 0.15);
    }

    if(isRegulationActive(game, REG_INSURANCE_REGULATION)){
        premium = (premium * 85) / 100; //10% discount
    }

    if(isNationalEffectActive(game,CARD_INSURANCE_DISCOUNT,p->index)){
        premium = (premium * 80) / 100;
    }

    if(isEconomicEventActive(game, ECON_HEAVY_MONSOON)){
        premium = (premium * 125) / 100; //Rule-LK 18: premiums increase (assumed +25%)
    }
    
    if(p -> cash < premium){
        printf("%s cannot afford the insurance premium\n", p->name);
        return;
    }

    p -> cash -= premium;
    square -> insurancePolicyType = type;
    square -> insuranceRoundsRemaining = 20; //Insurance lasts for 20 rounds

    printf("%s purchased insurance for %s. Premium paid: LKR %d\n", p->name, square->name, premium);
}
