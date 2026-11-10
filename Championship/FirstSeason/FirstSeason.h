#pragma once
#include "Player.h"
#include "Team.h"
#include "Championship.h"

Position readPosition();
Team* createTeamByChoice(int typeChoice, const char* name, const char* coach, const char* stadium, double budget);
void addPlayerFromConsole(Team* team, int playerIndex);
Team* createTeamFromConsole(int teamIndex, double initialBudget);
Championship createFirstSeasonFromConsole(unsigned year);
Championship createFirstSeason(unsigned year);