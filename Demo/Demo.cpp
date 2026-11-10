#include "Demo.h"
#include "ChampionshipCollection.h"
#include "AttackingTeam.h"
#include "DefensiveTeam.h"
#include "BalancedTeam.h"
#include "TextUtil.h"
#include <stdexcept>

const double Demo::initialTeamBudget = 75000.0;

void Demo::addPlayersForBalanced(Team* team, const char* prefix) {
    char name[64];
    makeIndexedName(name, 64, prefix, "GK", 1);
    team->addPlayer(Player(name, 1, Position::Goalkeeper, 1000, 2000));
    makeIndexedName(name, 64, prefix, "GK", 2);
    team->addPlayer(Player(name, 30, Position::Goalkeeper, 1000, 2000));
    for (int i = 0; i < 4; i++) {
        makeIndexedName(name, 64, prefix, "D", i + 1);
        team->addPlayer(Player(name, 2 + i, Position::Defender, 1200, 2500));
        makeIndexedName(name, 64, prefix, "M", i + 1);
        team->addPlayer(Player(name, 6 + i, Position::Midfielder, 1300, 2600));
        makeIndexedName(name, 64, prefix, "W", i + 1);
        team->addPlayer(Player(name, 16 + i, Position::Winger, 1400, 2700));
        makeIndexedName(name, 64, prefix, "F", i + 1);
        team->addPlayer(Player(name, 20 + i, Position::Forward, 1500, 3000, 0, 0));
    }
}

void Demo::addPlayersForAttacking(Team* team, const char* prefix) {
    char name[64];
    makeIndexedName(name, 64, prefix, "GK", 1);
    team->addPlayer(Player(name, 1, Position::Goalkeeper, 1000, 2000));
    makeIndexedName(name, 64, prefix, "GK", 2);
    team->addPlayer(Player(name, 12, Position::Goalkeeper, 1000, 2000));
    for (int i = 0; i < 2; i++) {
        makeIndexedName(name, 64, prefix, "D", i + 1);
        team->addPlayer(Player(name, 2 + i, Position::Defender, 1200, 2500));
        makeIndexedName(name, 64, prefix, "M", i + 1);
        team->addPlayer(Player(name, 6 + i, Position::Midfielder, 1300, 2600));
        makeIndexedName(name, 64, prefix, "W", i + 1);
        team->addPlayer(Player(name, 10 + i, Position::Winger, 1400, 2700));
    }
    for (int i = 0; i < 8; i++) {
        makeIndexedName(name, 64, prefix, "F", i + 1);
        team->addPlayer(Player(name, 20 + i, Position::Forward, 1500, 3000, 0, 0));
    }
}

void Demo::addPlayersForDefensive(Team* team, const char* prefix) {
    char name[64];
    makeIndexedName(name, 64, prefix, "GK", 1);
    team->addPlayer(Player(name, 1, Position::Goalkeeper, 1000, 2000));
    makeIndexedName(name, 64, prefix, "GK", 2);
    team->addPlayer(Player(name, 30, Position::Goalkeeper, 1000, 2000));
    for (int i = 0; i < 6; i++) {
        makeIndexedName(name, 64, prefix, "D", i + 1);
        team->addPlayer(Player(name, 2 + i, Position::Defender, 1200, 2500));
        makeIndexedName(name, 64, prefix, "M", i + 1);
        team->addPlayer(Player(name, 8 + i, Position::Midfielder, 1300, 2600));
    }
    for (int i = 0; i < 2; i++) {
        makeIndexedName(name, 64, prefix, "W", i + 1);
        team->addPlayer(Player(name, 16 + i, Position::Winger, 1400, 2700));
        makeIndexedName(name, 64, prefix, "F", i + 1);
        team->addPlayer(Player(name, 20 + i, Position::Forward, 1500, 3000, 0, 0));
    }
}

Championship Demo::createInitialChampionship(int year) {
    Championship championship(year);
    Team* teams[6] = { 0, 0, 0, 0, 0, 0 };

    try {
        /* Names of teams and players were generated from AI (ChatGPT) */
        teams[0] = new AttackingTeam("Lions", "Ivanov", "Arena_Lions", initialTeamBudget);
        teams[1] = new DefensiveTeam("Walls", "Petrov", "Stone_Park", initialTeamBudget);
        teams[2] = new BalancedTeam("Stars", "Dimitrov", "Star_Field", initialTeamBudget);
        teams[3] = new BalancedTeam("Rivers", "Georgiev", "River_Ground", initialTeamBudget);
        teams[4] = new AttackingTeam("Falcons", "Nikolov", "Falcon_Arena", initialTeamBudget);
        teams[5] = new DefensiveTeam("Mountains", "Stoyanov", "Mountain_Park", initialTeamBudget);

        addPlayersForAttacking(teams[0], "L");
        addPlayersForDefensive(teams[1], "W");
        addPlayersForBalanced(teams[2], "S");
        addPlayersForBalanced(teams[3], "R");
        addPlayersForAttacking(teams[4], "F");
        addPlayersForDefensive(teams[5], "M");

        for (int i = 0; i < 6; i++) {
            if (!championship.addTeam(teams[i])) {
                throw std::runtime_error("Cannot add demo team.");
            }
            teams[i] = nullptr;
        }
        championship.generateSchedule();
    }
    catch (...) {
        for (int i = 0; i < 6; i++) {
            delete teams[i];
            teams[i] = 0;
        }
        throw;
    }

    return championship;
}

Championship Demo::createDemoChampionship(int year) {
    return createInitialChampionship(year);
}

unsigned Demo::addDemoSeason(ChampionshipCollection& collection) {
    unsigned year = collection.nextYear();
    Championship championship = createDemoChampionship(year);
    collection.addChampionship(championship);
    return year;
}

double Demo::getInitialTeamBudget() {
    return initialTeamBudget;
}

