#ifndef TYPES_H
#define TYPES_H

typedef enum{
    SQ_START ,
    SQ_PROPERTY ,
    SQ_EVENT ,
    SQ_TAX ,
    SQ_RAILWAY ,
    SQ_SPECIAL ,
    SQ_UTILITY ,
    SQ_INSURANCE ,
    SQ_BANK ,
} SquareType;

typedef struct{
    int index;
    SquareType type;
    char name[50];
    int propertyIndex;
} Square;

typedef enum{
    Group_Brown ,
    Group_LightBlue ,
    Group_Pink ,
    Group_Orange ,
    Group_Red ,
    Group_Yellow ,
    Group_Green ,
    Group_DarkBlue , 
    GROUP_NONE , //Railways and Utilities
}PropertyGroup;

typedef enum{
    PropertyType_Regular ,
    PropertyType_Railway ,
    PropertyType_Utility ,
}PropertyType;

//Insurance Policy Types
typedef enum{
    Basic_Insurance ,
    Comprehensive_Insurance ,
    Business_Interruption_Insurance ,
    None_Insurance ,
}PolicyType;

typedef struct{
    //Identity
    char name[50];
    PropertyGroup group;
    PropertyType propertytype;

    //Base Values
    int basePurchasePrice;
    int baseMortgageValue;
    int baseRent;
    int baseHouseCost; //0 for railways and utilities
    int baseHotelCost; //0 for railways and utilities

    //Current
    int owner; //-1 for unowned or Bank
    int numHouses; //0-4 , only hasHotel == 0
    int hasHotel; //0 for no hotel, 1 for hotel
    int isMortgaged; //0 or 1
    int propertyAge; //Rule 15
    int buildingCondition; //Rule 25
    int isLoanLocked; //Rule 3

    //Insurance Policy
    PolicyType insurancePolicyType;
    int insuranceRoundsRemaining; //Number of rounds remaining for the insurance policy
} Property;


typedef enum{
    STRATEGY_AGGRESSIVE_INVESTOR ,
    STRATEGY_CONSERVATIVE_BANKER ,
    STRATEGY_RISK_TAKER ,
    STRATEGY_OPPORTUNISTIC_TRADER ,
} PlayerStrategy;

typedef struct{
    char name[50];
    PlayerStrategy strategy;
    int position; //0-39,current square index
    int cash; //Current cash balance

    int ownedProperties[28]; //Array of property indices owned by the player
    int numOwnedProperties; //Number of properties owned by the player

    int inJail; //0 or 1
    int jailTurnsRemaining;//after 3 turns, player is released from jail

    int hasActiveLoan; //0 or 1

    int isBankrupt; //0 or 1
} Player;

typedef struct{
    int amount;
    int interestRate; //Percentage
    int remainingTurns; //Number of turns left to repay the loan
    int collateralProperty[28];
    int numCollateralProperties; //Number of properties used as collateral
    int ownerPlayerIndex; //Index of the player who took the loan
    int isActive; //0 or 1
} Loan;

typedef struct{
    Square squares[40]; //Array of 40 squares on the board
    Property properties[28]; //Array of 28 properties on the board
    Player players[4]; //Array of 4 players in the game
    Loan loans[4]; //One loan for one player at a time

    int currentRound;
    int currentInflationRate; //Percentage
    int currentLoanInterestRate; //Percentage

    PropertyGroup boomedGroup; //The property group that is currently booming
    int boomRoundsRemaining; //Rule-LK 30/31
    PropertyGroup declinedGroup; //The property group that is currently declining
    int declineRoundsRemaining; //Rule-LK 30/31

    int activeRegionalCard; //-1=none
    int regionalCardRoundsRemaining; //Number of rounds remaining for the active regional card effect

    int gameOver; //0 or 1
} GameBoard;

#endif // TYPES_H
