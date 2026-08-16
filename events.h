#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

void updateMarketBoomDecline(GameState *game);                 //events.c:7
void triggerDisaster(Player *players, Square *squares, GameState *game); //events.c:54

//National Card Functions
void initEventDeck(GameState *game);                           //events.c:111
NationalCardId drawEventCard(GameState *game);                 //events.c:124
int isNationalEffectActive(GameState *game,int cardID,int playerIndex); //events.c:172
int isPropertyClosed(GameState *game,int propertyIndex);       //events.c:180
void tickNationalEffects(GameState *game);                     //events.c:188

//Government Regulation Functions
int isRegulationActive(GameState *game, GovernmentRegulation reg); //events.c:200
void triggerGovernmentRegulation(GameState *game, Square *squares); //events.c:215

//Regional Card Functions
int isRegionalCardActive(GameState *game, RegionalCardId card);//events.c:333
void revertRegionalCard(GameState *game, Square *squares);     //events.c:337
void triggerRegionalDevelopment(GameState *game, Square *squares); //events.c:354
void printActiveRegionalCard(GameState *game);                 //events.c:375

int canPurchaseProperty(GameState *game, int playerIndex, Square *squares); //events.c:399
void applyEventCard(NationalCardId card,Player *players,int playerIndex, Square *squares,GameState *game); //events.c:409

//Economic Event Functions (Rule-LK 18)
int isEconomicEventActive(GameState *game, EconomicEvent event); //events.c:601
void triggerEconomicEvent(GameState *game, Square *squares);   //events.c:611
void tickEconomicEvents(GameState *game, Square *squares);     //events.c:637
void printActiveEconomicEvents(GameState *game);               //events.c:658

#endif