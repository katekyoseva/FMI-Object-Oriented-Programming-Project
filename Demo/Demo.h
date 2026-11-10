#pragma once
#include "Championship.h"
class ChampionshipCollection;

/* Builds valid sample data for the first season and for demonstrations. */
class Demo {
public:
    static Championship createDemoChampionship(int year);
    static unsigned addDemoSeason(ChampionshipCollection& collection);
    static double getInitialTeamBudget();
    /* Creates the first season with valid teams, equal budget, squads, and schedule. */
    static Championship createInitialChampionship(int year);
private:
    static void addPlayersForBalanced(Team* team, const char* prefix);
    static void addPlayersForAttacking(Team* team, const char* prefix);
    static void addPlayersForDefensive(Team* team, const char* prefix);
private:
    static const double initialTeamBudget;
};

