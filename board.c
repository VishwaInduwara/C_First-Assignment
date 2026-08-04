#include <string.h>

#include "board.h"
#include "types.h"


//Square squares[40]; //Array of 40 squares on the board

void initBoard(Square *squares){

    for(int i = 0; i < BOARD_SIZE; i++){
        
        //Identity
        squares[i].index = i;
        squares[i].propertyIndex = -1; //Unowned
        squares[i].group = GROUP_NONE; //Default group
        squares[i].propertytype = PropertyType_Regular; //Default property type

        //Base Values
        squares[i].basePurchasePrice = 0;
        squares[i].baseMortgageValue = 0;
        squares[i].baseRent = 0;
        squares[i].baseHouseCost = 0;
        squares[i].baseHotelCost = 0;

        //Current
        squares[i].owner = -1;
        squares[i].numHouses = 0;
        squares[i].hasHotel = 0;
        squares[i].isMortgaged = 0;
        squares[i].propertyAge = 0;
        squares[i].buildingCondition = 100;
        squares[i].isLoanLocked = 0;

        //Insurance Policy
        squares[i].insurancePolicyType = None_Insurance;
        squares[i].insuranceRoundsRemaining = 0;
    }
    strcpy(squares[0].name, "GO");
    squares[0].type = SQ_START;
    squares[0].propertytype = PropertyType_Regular;

    strcpy(squares[1].name, "Pettah");
    squares[1].type = SQ_PROPERTY;
    squares[1].group = Group_Brown;
    squares[1].propertyIndex = 0; //Index of the associated property
    squares[1].basePurchasePrice = 1500;
    squares[1].baseMortgageValue = 750;
    squares[1].baseHouseCost = 500;
    squares[1].baseHotelCost = 2000;
    squares[1].baseRent = 100;

    strcpy(squares[2].name, "Community Development Fund");
    squares[2].type = SQ_EVENT;

    strcpy(squares[3].name, "Maradana");
    squares[3].type = SQ_PROPERTY;
    squares[3].group = Group_Brown;
    squares[3].basePurchasePrice = 1800;
    squares[3].baseMortgageValue = 750;
    squares[3].baseHouseCost = 500;
    squares[3].baseHotelCost = 2000;
    squares[3].baseRent = 120;

    strcpy(squares[4].name, "Income Tax");
    squares[4].type = SQ_TAX;

    strcpy(squares[5].name, "Colombo Fort Railway Station");
    squares[5].type = SQ_RAILWAY;
    squares[5].basePurchasePrice = 2000;
    squares[5].baseMortgageValue = 1000;
    squares[5].baseRent = 250;

    strcpy(squares[6].name, "Bambalapitiya");
    squares[6].type = SQ_PROPERTY;
    squares[6].group = Group_LightBlue;
    squares[6].basePurchasePrice = 2500;
    squares[6].baseMortgageValue = 1250;
    squares[6].baseHouseCost = 750;
    squares[6].baseHotelCost = 3000;
    squares[6].baseRent = 180;

    strcpy(squares[7].name, "National Event Card");
    squares[7].type = SQ_EVENT; 

    strcpy(squares[8].name, "Wellawatte");
    squares[8].type = SQ_PROPERTY;
    squares[8].group = Group_LightBlue;
    squares[8].basePurchasePrice = 2700;
    squares[8].baseMortgageValue = 1250;
    squares[8].baseHouseCost = 750;
    squares[8].baseHotelCost = 3000;
    squares[8].baseRent = 200;

    strcpy(squares[9].name, "Mount Lavinia");
    squares[9].type = SQ_PROPERTY;
    squares[9].group = Group_LightBlue;
    squares[9].basePurchasePrice = 3000;
    squares[9].baseMortgageValue = 1250;
    squares[9].baseHouseCost = 750;
    squares[9].baseHotelCost = 3000;
    squares[9].baseRent = 220;

    strcpy(squares[10].name, "Jail / Just Visiting");
    squares[10].type = SQ_SPECIAL;

    strcpy(squares[11].name, "Nugegoda");
    squares[11].type = SQ_PROPERTY;
    squares[11].group = Group_Pink;
    squares[11].basePurchasePrice = 3500;
    squares[11].baseMortgageValue = 1750;
    squares[11].baseHouseCost = 1000;
    squares[11].baseHotelCost = 4000;
    squares[11].baseRent = 260;

    strcpy(squares[12].name, "Ceylon Electricity Board");
    squares[12].type = SQ_UTILITY;
    squares[12].basePurchasePrice = 1500;
    squares[12].baseMortgageValue = 750;

    strcpy(squares[13].name, "Maharagama");
    squares[13].type = SQ_PROPERTY;
    squares[13].group = Group_Pink;
    squares[13].basePurchasePrice = 3800;
    squares[13].baseMortgageValue = 1750;
    squares[13].baseHouseCost = 1000;
    squares[13].baseHotelCost = 4000;
    squares[13].baseRent = 280;

    strcpy(squares[14].name, "Kottawa");
    squares[14].type = SQ_PROPERTY;
    squares[14].group = Group_Pink;
    squares[14].basePurchasePrice = 4000;
    squares[14].baseMortgageValue = 1750;
    squares[14].baseHouseCost = 1000;
    squares[14].baseHotelCost = 4000;
    squares[14].baseRent = 300;

    strcpy(squares[15].name, "Kandy Railway Station");
    squares[15].type = SQ_RAILWAY;
    squares[15].basePurchasePrice = 2000;
    squares[15].baseMortgageValue = 1000;
    squares[15].baseRent = 250;

    strcpy(squares[16].name, "Negombo");
    squares[16].type = SQ_PROPERTY;
    squares[16].group = Group_Orange;
    squares[16].basePurchasePrice = 4500;
    squares[16].baseMortgageValue = 2250;
    squares[16].baseHouseCost = 1250;
    squares[16].baseHotelCost = 5000;
    squares[16].baseRent = 350;

    strcpy(squares[17].name, "Sri Lanka Insurance");
    squares[17].type = SQ_INSURANCE;

    strcpy(squares[18].name, "Katunayake");
    squares[18].type = SQ_PROPERTY;
    squares[18].group = Group_Orange;
    squares[18].basePurchasePrice = 4700;
    squares[18].baseMortgageValue = 2250;
    squares[18].baseHouseCost = 1250;
    squares[18].baseHotelCost = 5000;
    squares[18].baseRent = 370;

    strcpy(squares[19].name, "Ja-Ela");
    squares[19].type = SQ_PROPERTY;
    squares[19].group = Group_Orange;
    squares[19].basePurchasePrice = 5000;
    squares[19].baseMortgageValue = 2250;
    squares[19].baseHouseCost = 1250;
    squares[19].baseHotelCost = 5000;
    squares[19].baseRent = 400;

    strcpy(squares[20].name, "Free Parking");
    squares[20].type = SQ_SPECIAL;      

    strcpy(squares[21].name, "Kandy City");
    squares[21].type = SQ_PROPERTY;
    squares[21].group = Group_Red;
    squares[21].basePurchasePrice = 5500;
    squares[21].baseMortgageValue = 2750;
    squares[21].baseHouseCost = 1500;
    squares[21].baseHotelCost = 6000;
    squares[21].baseRent = 450;

    strcpy(squares[22].name, "National Event Card");
    squares[22].type = SQ_EVENT;

    strcpy(squares[23].name, "Peradeniya");
    squares[23].type = SQ_PROPERTY;
    squares[23].group = Group_Red;
    squares[23].basePurchasePrice = 5800;
    squares[23].baseMortgageValue = 2750;
    squares[23].baseHouseCost = 1500;
    squares[23].baseHotelCost = 6000;
    squares[23].baseRent = 480;

    strcpy(squares[24].name, "Katugastota");
    squares[24].type = SQ_PROPERTY;
    squares[24].group = Group_Red;
    squares[24].basePurchasePrice = 6000;
    squares[24].baseMortgageValue = 2750;
    squares[24].baseHouseCost = 1500;
    squares[24].baseHotelCost = 6000;
    squares[24].baseRent = 500;

    strcpy(squares[25].name, "Galle Railway Station");
    squares[25].type = SQ_RAILWAY;
    squares[25].basePurchasePrice = 2000;
    squares[25].baseMortgageValue = 1000;
    squares[25].baseRent = 250;

    strcpy(squares[26].name, "Galle Fort");
    squares[26].type = SQ_PROPERTY;
    squares[26].group = Group_Yellow;
    squares[26].basePurchasePrice = 6500;
    squares[26].baseMortgageValue = 3250;
    squares[26].baseHouseCost = 2000;
    squares[26].baseHotelCost = 8000;
    squares[26].baseRent = 600;

    strcpy(squares[27].name, "Unawatuna");
    squares[27].type = SQ_PROPERTY;
    squares[27].group = Group_Yellow;
    squares[27].basePurchasePrice = 6800;
    squares[27].baseMortgageValue = 3250;
    squares[27].baseHouseCost = 2000;
    squares[27].baseHotelCost = 8000;
    squares[27].baseRent = 620;

    strcpy(squares[28].name, "National Water Supply and Drainage Board");
    squares[28].type = SQ_UTILITY;
    squares[28].basePurchasePrice = 1500;
    squares[28].baseMortgageValue = 750;

    strcpy(squares[29].name, "Hikkaduwa");
    squares[29].type = SQ_PROPERTY;
    squares[29].group = Group_Yellow;
    squares[29].basePurchasePrice = 7000;
    squares[29].baseMortgageValue = 3250;
    squares[29].baseHouseCost = 2000;
    squares[29].baseHotelCost = 8000;
    squares[29].baseRent = 650;

    strcpy(squares[30].name, "Go To Jail");
    squares[30].type = SQ_SPECIAL;

    strcpy(squares[31].name, "Jaffna Town");
    squares[31].type = SQ_PROPERTY;
    squares[31].group = Group_Green;
    squares[31].basePurchasePrice = 8000;
    squares[31].baseMortgageValue = 4000;
    squares[31].baseHouseCost = 2500;
    squares[31].baseHotelCost = 10000;
    squares[31].baseRent = 750;

    strcpy(squares[32].name, "Nallur");
    squares[32].type = SQ_PROPERTY;
    squares[32].group = Group_Green;
    squares[32].basePurchasePrice = 8300;
    squares[32].baseMortgageValue = 4000;
    squares[32].baseHouseCost = 2500;
    squares[32].baseHotelCost = 10000;
    squares[32].baseRent = 780;

    strcpy(squares[33].name, "Ceylinco Insurance");
    squares[33].type = SQ_INSURANCE;

    strcpy(squares[34].name, "Trincomalee");
    squares[34].type = SQ_PROPERTY;
    squares[34].group = Group_Green;
    squares[34].basePurchasePrice = 8500;
    squares[34].baseMortgageValue = 4000;
    squares[34].baseHouseCost = 2500;
    squares[34].baseHotelCost = 10000;
    squares[34].baseRent = 800;

    strcpy(squares[35].name, "Jaffna Railway Station");
    squares[35].type = SQ_RAILWAY;
    squares[35].basePurchasePrice = 2000;
    squares[35].baseMortgageValue = 1000;
    squares[35].baseRent = 250;

    strcpy(squares[36].name, "National Event Card");
    squares[36].type = SQ_EVENT;

    strcpy(squares[37].name, "Nuwara Eliya");
    squares[37].type = SQ_PROPERTY;
    squares[37].group = Group_DarkBlue;
    squares[37].basePurchasePrice = 10000;
    squares[37].baseMortgageValue = 5000;
    squares[37].baseHouseCost = 3000;
    squares[37].baseHotelCost = 12000;
    squares[37].baseRent = 1000;

    strcpy(squares[38].name, "Bank of Ceylon");
    squares[38].type = SQ_BANK;

    strcpy(squares[39].name, "Galle Face");
    squares[39].type = SQ_PROPERTY;
    squares[39].group = Group_DarkBlue;
    squares[39].basePurchasePrice = 12000;
    squares[39].baseMortgageValue = 5000;
    squares[39].baseHouseCost = 3000;
    squares[39].baseHotelCost = 12000;
    squares[39].baseRent = 1200;
}









