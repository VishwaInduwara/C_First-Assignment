#ifndef TYPES_H
#define TYPES_H

#define BOARD_SIZE 40
#define MAX_PLAYERS 4
#define MAX_ROUND 500
#define MAX_ACTIVE_NATIONAL_EFFECTS 20

// Loan Trigger Thresholds (based on LKR 30,000 starting capital)
#define LOAN_THRESHOLD_AGGRESSIVE     15000  // 50% of starting cash
#define LOAN_THRESHOLD_OPPORTUNISTIC   5000  // Safe liquidity buffer
#define LOAN_THRESHOLD_CONSERVATIVE    2000  // Imminent bankruptcy safety line
#define MAX_ACTIVE_ECONOMIC_EVENTS 8 //Rule-LK 34: multiple events can stack
#define ECONOMIC_EVENT_DURATION 20    //rounds each event stays active

//Enums

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
    PropertyType_NONE ,
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

typedef enum{
    CARD_TOURISM_HYPE,             //0 Hotels earn double rent for 5 rounds
    CARD_FUEL_SHORTAGE,            //1 Railway rent doubles for 5 rounds
    CARD_HEAVY_FLOODS,             //2 Random coastal property damaged
    CARD_POLITICAL_RALLY,          //3 One random property closed for 2 rounds
    CARD_STOCK_MARKET_RISE,        //4 All property values increase by 10%
    CARD_ECONOMIC_DOWNTURN,        //5 Property values decrease by 15%
    CARD_HOUSING_SUBSIDY,          //6 House construction cost reduced by 30%
    CARD_INTEREST_RATE_CUT,        //7 Loan interest reduced by 2%
    CARD_INTEREST_RATE_INCREASE,   //8 Loan interest increased by 2%
    CARD_TAX_AMNESTY,              //9 Each player receives LKR 2,000
    CARD_POWER_FAILURE,            //10 Utility income halved for 3 rounds
    CARD_FOREIGN_FUNDING,          //11 Commercial property values increase by 15%
   
    
    CARD_PORT_EXPANSION,           //12 Railway station values increase by 20%
    CARD_FESTIVAL_SEASON,          //13 Hotels receive 50% additional rent
    CARD_LABOUR_STRIKE,            //14 Construction suspended for 2 rounds
    CARD_INSURANCE_DISCOUNT,       //15 Premiums reduced by 20%
    CARD_PROPERTY_REVALUATION,     //16 Random property group appreciates by 15%
    
    
    CARD_CURRENCY_DEPRECIATION,    //17 Construction costs increase by 10%
    CARD_GOVERNMENT_GRANT,         //18 Random player receives LKR 5,000
    CARD_NATIONAL_DISASTER,        //19 Random developed property damaged
    NUM_NATIONAL_CARDS             //20
} NationalCardId;
    


typedef enum{
    STRATEGY_AGGRESSIVE_INVESTOR ,
    STRATEGY_CONSERVATIVE_BANKER ,
    STRATEGY_RISK_TAKER ,
    STRATEGY_OPPORTUNISTIC_TRADER ,
} PlayerStrategy;

typedef enum{
    REG_INCREASE_PROPERTY_TAX,   //0 Income Tax +50%
    REG_REDUCE_LOAN_INTEREST,    //1 Loan interest -2%
    REG_HOUSING_SUBSIDY,         //2 House construction -30%
    REG_LUXURY_PROPERTY_TAX,     //3 Hotels pay 25% maintenance tax
    REG_RAILWAY_MODERNIZATION,   //4 Railway rent +25%
    REG_ELECTRICITY_TARIFF,      //5 Utility rent +20%
    REG_INSURANCE_REGULATION,    //6 Insurance premiums -15%
    REG_ANTI_SPECULATION,        //7 Max 3 undeveloped properties
    NUM_GOVERNMENT_REGULATIONS   //8
} GovernmentRegulation;

typedef enum{
    REGIONAL_SOUTHERN_TOURISM,      //0 rents +40% {26,27,29}
    REGIONAL_PORT_CITY_EXPANSION,   //1 value +25% {1,3,5}
    REGIONAL_IT_GROWTH,             //2 value +20% {11,13,14}
    REGIONAL_NORTHERN_DEVELOPMENT,  //3 value +30% {31,32,34}
    REGIONAL_TEA_EXPORT,            //4 value +35% {37}
    REGIONAL_AIRPORT_EXPANSION,     //5 rents +30% {16,18,19}
    REGIONAL_UNIVERSITY_GROWTH,     //6 value +20% {21,23}
    REGIONAL_BEACH_POLLUTION,       //7 rents -30% {26,27,29}
    REGIONAL_FLOOD_DAMAGE,          //8 value -20% coastal
    REGIONAL_TRANSPORT_STRIKE,      //9 railway rent -40%
    REGIONAL_ELECTRICITY_TARIFF,    //10 utility rent +25%
    REGIONAL_WATER_SHORTAGE,        //11 utility{28} rent +20%; {26,27,29} value -10%
    NUM_REGIONAL_CARDS              //12
} RegionalCardId;

//Rule-LK 18: Economic Events
typedef enum{
    ECON_TOURISM_BOOM,        //0 Hotels double rent; Southern Coastal +15%
    ECON_FUEL_CRISIS,         //1 Railway rent doubles; development costs +20%
    ECON_HEAVY_MONSOON,       //2 Flood risk up; insurance premiums up; Coastal -10%
    ECON_ECONOMIC_RECESSION,  //3 Values -15%; rent -10%; loan interest +15%
    ECON_STOCK_MARKET_BOOM,   //4 Values +10%; loan interest -10%
    ECON_HOUSING_PROGRAMME,   //5 House construction -25%
    ECON_FOREIGN_INVESTMENT,  //6 Railway+Utility values +20%
    ECON_POLITICAL_UNREST,    //7 Riot probability x2; hotel rent -50%
    NUM_ECONOMIC_EVENTS       //8
} EconomicEvent;



typedef struct{
    //Identity
    int index;
    char name[50];

    int propertyIndex; //-1 for non-property squares
    SquareType type;
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
    int marketPrice; //After inflation
    int numHouses; //0-4 , only hasHotel == 0
    int hasHotel; //0 for no hotel, 1 for hotel
    int isMortgaged; //0 or 1
    int propertyAge; //Rule 15
    int buildingCondition; //Rule 25
    int isLoanLocked; //Rule 3
    int isDamaged; //0 or 1, for disaster events

    int depreciationPct;   //Rule-LK 16: 0-30, % of value/rent lost (0 = not depreciated)
    int renovationLevel;   //Rule-LK 17: number of renovations (permanent +5% rent each)

    int roundsWithoutMaintenance; //Rule-LK 28: consecutive rounds since last maintenance
    int isStructurallyDamaged;    //Rule-LK 28: 0/1 structural damage flag

    //Insurance Policy
    PolicyType insurancePolicyType;
    int insuranceRoundsRemaining; //Number of rounds remaining for the insurance policy
} Square;


typedef struct{
    int index;
    char name[50];
    PlayerStrategy strategy;
    int position; //0-39,current square index
    int cash; //Current cash balance

    int playerrolls; //Dice value rolled by the player to determine turn order

    int ownedProperties[28]; //Array of property indices owned by the player
    int numOwnedProperties; //Number of properties owned by the player
    int numHotelCount;

    int inJail; //0 or 1
    int jailTurnsRemaining;//after 3 turns, player is released from jail

    int hasActiveLoan; //0 or 1
    int hasMonopoly;//0 or 1

    int loanAmount;
    int loanTurnsRemaining;//number of turns left to repay the loan(20)

    int isBankrupt; //0 or 1

    int isRoundCompleted; //0 or 1

    int totalPropertyValue;
    int netWorth;
} Player;

typedef struct{           
    int targetPlayer;     //Player the effect applies to (-1 = all players)
    int targetProperty;   //Property index (Political Rally only, else -1)
    int roundsRemaining;  //Rounds left for the effect
    int isActive;  //0 or 1
    int cardID;   //NationalCardId    
} NationalEffect;

typedef struct{
    int eventID;         //EconomicEvent value, -1 = empty slot
    int roundsRemaining; //rounds left before expiry
} EconomicEventSlot;


typedef struct{
    int currentRound;
    float currentInflactionRate;
    float currentLoanInterest;
    PropertyGroup boomedGroup; //The property group that is currently booming
    int boomRoundsRemaining; //Rule-LK 30/31

    PropertyGroup declinedGroup; //The property group that is currently declining
    int declineRoundsRemaining; //Rule-LK 30/31

    PropertyGroup lastBoomedGroup;
    PropertyGroup lastDeclinedGroup;
    int boomCooldown[8];//array is used to remember how many rounds each property group must wait before it can boom again(30 rounds)
    int declineCooldown[8];

    EconomicEventSlot economicEffects[MAX_ACTIVE_ECONOMIC_EVENTS]; //Rule-LK 18

    int activeRegionalCard; //-1=none
    int regionalCardRoundsRemaining; //Number of rounds remaining for the active regional card effect

    int activeRegulation;          // -1 = none, else GovernmentRegulation
    int regulationRoundsRemaining; // rounds the regulation stays active

    int nationalCardDeck[NUM_NATIONAL_CARDS]; //Array to hold the national card deck
    int nationalCardTop; //index of the next card to draw
    NationalEffect nationalEffects[MAX_ACTIVE_NATIONAL_EFFECTS]; //Array to hold active national card effects
    
    int gameOver; //0 or 1
} GameState;



#endif // TYPES_H

