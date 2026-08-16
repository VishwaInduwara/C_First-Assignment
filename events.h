#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

void updateMarketBoomDecline(GameState *game);

void triggerDisaster(Player *players, Square *squares, GameState *game);

//National Card Functions
void initEventDeck(GameState *game);
void applyEventCard(NationalCardId card,Player *players,int playerIndex, Square *squares,GameState *game);
NationalCardId drawEventCard(GameState *game);
int isNationalEffectActive(GameState *game,int cardID,int playerIndex);
int isPropertyClosed(GameState *game,int propertyIndex);
void tickNationalEffects(GameState *game);

//Government Regulation Functions
void triggerGovernmentRegulation(GameState *game, Square *squares);
int isRegulationActive(GameState *game, GovernmentRegulation reg);
int canPurchaseProperty(GameState *game, int playerIndex, Square *squares);

//Regional Card Functions
void triggerRegionalDevelopment(GameState *game, Square *squares);
int isRegionalCardActive(GameState *game, RegionalCardId card);
void revertRegionalCard(GameState *game, Square *squares);

//Economic Event Functions (Rule-LK 18)
void triggerEconomicEvent(GameState *game, Square *squares);
void tickEconomicEvents(GameState *game, Square *squares);
int isEconomicEventActive(GameState *game, EconomicEvent event);
void printActiveEconomicEvents(GameState *game);

void printActiveRegionalCard(GameState *game);

#endif