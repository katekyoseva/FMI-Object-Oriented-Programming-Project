#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>

#include "Player.h"
#include "BalancedTeam.h"
#include "AttackingTeam.h"
#include "DefensiveTeam.h"
#include "Lineup.h"
#include "ChampionshipCollection.h"
#include "Demo.h"
#include "TextUtil.h"

#pragma warning(disable:4996)

int passedChecks = 0;
int failedChecks = 0;

bool almostEqual(double first, double second) {
    return std::fabs(first - second) < 0.001;
}

void expect(bool condition, const char* message) {
    if (condition) {
        passedChecks++;
        std::cout << "[OK] " << message << "\n";
    }
    else {
        failedChecks++;
        std::cout << "[FAIL] " << message << "\n";
    }
}

void addGeneratedPlayer(Team& team, const char* prefix,
    const char* role, int index, unsigned number, Position position,
    unsigned goals = 0) {
    char name[64];
    makeIndexedName(name, 64, prefix, role, index);
    Player player(name, number, position, 1000 + number, 2000 + number, 0, goals);
    if (!team.addPlayer(player)) {
        throw std::runtime_error("Cannot create test team.");
    }
}

void addBalancedPlayers(Team& team, const char* prefix) {
    addGeneratedPlayer(team, prefix, "GK", 1, 1, Position::Goalkeeper);
    addGeneratedPlayer(team, prefix, "GK", 2, 30, Position::Goalkeeper);

    for (int i = 0; i < 4; i++) {
        addGeneratedPlayer(team, prefix, "D", i + 1, 2 + i, Position::Defender);
        addGeneratedPlayer(team, prefix, "M", i + 1, 6 + i, Position::Midfielder);
        addGeneratedPlayer(team, prefix, "W", i + 1, 10 + i, Position::Winger);
        addGeneratedPlayer(team, prefix, "F", i + 1, 14 + i, Position::Forward, 6 + i);
    }
}

void addAttackingPlayers(Team& team, const char* prefix) {
    addGeneratedPlayer(team, prefix, "GK", 1, 1, Position::Goalkeeper);
    addGeneratedPlayer(team, prefix, "GK", 2, 30, Position::Goalkeeper);

    for (int i = 0; i < 2; i++) {
        addGeneratedPlayer(team, prefix, "D", i + 1, 2 + i, Position::Defender);
        addGeneratedPlayer(team, prefix, "M", i + 1, 6 + i, Position::Midfielder);
        addGeneratedPlayer(team, prefix, "W", i + 1, 10 + i, Position::Winger);
    }
    for (int i = 0; i < 8; i++) {
        addGeneratedPlayer(team, prefix, "F", i + 1, 14 + i, Position::Forward, 6 + i);
    }
}

void addDefensivePlayers(Team& team, const char* prefix) {
    addGeneratedPlayer(team, prefix, "GK", 1, 1, Position::Goalkeeper);
    addGeneratedPlayer(team, prefix, "GK", 2, 30, Position::Goalkeeper);

    for (int i = 0; i < 6; i++) {
        addGeneratedPlayer(team, prefix, "D", i + 1, 2 + i, Position::Defender);
        addGeneratedPlayer(team, prefix, "M", i + 1, 8 + i, Position::Midfielder);
    }
    for (int i = 0; i < 2; i++) {
        addGeneratedPlayer(team, prefix, "W", i + 1, 16 + i, Position::Winger);
        addGeneratedPlayer(team, prefix, "F", i + 1, 20 + i, Position::Forward, 6 + i);
    }
}

bool addLineupByGeneratedName(Lineup& lineup, const char* prefix,
    const char* role, int index) {
    char name[64];
    makeIndexedName(name, 64, prefix, role, index);
    return lineup.addPlayerByName(name);
}

void fillValidLineup(Lineup& lineup, const char* prefix) {
    expect(addLineupByGeneratedName(lineup, prefix, "GK", 1),
        "lineup adds goalkeeper");
    expect(addLineupByGeneratedName(lineup, prefix, "D", 1),
        "lineup adds defender 1");
    expect(addLineupByGeneratedName(lineup, prefix, "D", 2),
        "lineup adds defender 2");
    expect(addLineupByGeneratedName(lineup, prefix, "M", 1),
        "lineup adds midfielder 1");
    expect(addLineupByGeneratedName(lineup, prefix, "M", 2),
        "lineup adds midfielder 2");
    expect(addLineupByGeneratedName(lineup, prefix, "W", 1),
        "lineup adds winger 1");
    expect(addLineupByGeneratedName(lineup, prefix, "W", 2),
        "lineup adds winger 2");
    expect(addLineupByGeneratedName(lineup, prefix, "F", 1),
        "lineup adds forward 1");
    expect(addLineupByGeneratedName(lineup, prefix, "F", 2),
        "lineup adds forward 2");
    expect(addLineupByGeneratedName(lineup, prefix, "F", 3),
        "lineup adds forward 3");
    expect(addLineupByGeneratedName(lineup, prefix, "F", 4),
        "lineup adds forward 4");
}

bool readAndCheckTeamFromSavedChampionship(std::ifstream& in,
    double expectedBudget, bool checkBudget, bool checkResetStats) {
    char type[64];
    char name[128];
    char coach[128];
    char stadium[128];
    double budget;
    int wins;
    int draws;
    int losses;
    int points;
    int goalsFor;
    int goalsAgainst;
    int playersCount;

    in >> type >> name >> coach >> stadium >> budget >> wins >> draws
        >> losses >> points >> goalsFor >> goalsAgainst >> playersCount;

    if (!in || playersCount < 0 || playersCount > MAX_TEAM_PLAYERS) {
        return false;
    }
    if (checkBudget && !almostEqual(budget, expectedBudget)) {
        return false;
    }
    if (checkResetStats &&
        (wins != 0 || draws != 0 || losses != 0 || points != 0 ||
            goalsFor != 0 || goalsAgainst != 0)) {
        return false;
    }

    for (int i = 0; i < playersCount; i++) {
        char playerName[128];
        char position[32];
        unsigned number;
        double salary;
        double transferFee;
        unsigned matchesPlayed;
        unsigned goalsScored;

        in >> playerName >> number >> position >> salary >> transferFee
            >> matchesPlayed >> goalsScored;
        if (!in) {
            return false;
        }
        if (checkResetStats && (matchesPlayed != 0 || goalsScored != 0)) {
            return false;
        }
    }

    return true;
}

bool savedChampionshipTeamsPassChecks(const Championship& championship,
    double expectedBudget, bool checkBudget, bool checkResetStats) {
    const char* fileName = "test_championship_state.txt";
    {
        std::ofstream out(fileName);
        if (!out) {
            return false;
        }
        championship.save(out);
    }

    std::ifstream in(fileName);
    if (!in) {
        return false;
    }

    unsigned year;
    unsigned currentRound;
    bool finished;
    unsigned teamsCount;
    in >> year >> currentRound >> finished >> teamsCount;
    if (!in || teamsCount != championship.getTeamsCount()) {
        return false;
    }

    for (unsigned i = 0; i < teamsCount; i++) {
        if (!readAndCheckTeamFromSavedChampionship(in, expectedBudget,
            checkBudget, checkResetStats)) {
            return false;
        }
    }

    return true;
}

void testPlayerSalary() {
    Player scorer("Scorer", 9, Position::Forward, 1000, 2000, 8, 11);
    scorer.regulateSalary(12);
    expect(almostEqual(scorer.getSalary(), 1584.0),
        "salary increases for active high-scoring forward");

    Player weakForward("Weak", 10, Position::Forward, 1000, 2000, 2, 1);
    weakForward.regulateSalary(12);
    expect(almostEqual(weakForward.getSalary(), 765.0),
        "salary decreases for inactive low-scoring forward");
}

void testTeamValidation() {
    BalancedTeam team("Test", "Coach", "Stadium", 75000);

    expect(team.addPlayer(Player("GK1", 1, Position::Goalkeeper, 1000, 2000)),
        "team adds first goalkeeper");
    expect(!team.addPlayer(Player("GK1", 2, Position::Defender, 1000, 2000)),
        "team rejects duplicate player name");
    expect(!team.addPlayer(Player("Other", 1, Position::Defender, 1000, 2000)),
        "team rejects duplicate player number");
    expect(team.addPlayer(Player("GK2", 30, Position::Goalkeeper, 1000, 2000)),
        "team adds second goalkeeper");
    expect(!team.addPlayer(Player("GK3", 31, Position::Goalkeeper, 1000, 2000)),
        "team rejects third goalkeeper");

    Player* stored = team.findPlayer("GK1");
    expect(stored != nullptr && stored->getTeam() == &team,
        "stored player points back to owning team");
}

void testCompositionRules() {
    BalancedTeam balanced("Balanced", "Coach", "Stadium", 75000);
    AttackingTeam attacking("Attacking", "Coach", "Stadium", 75000);
    DefensiveTeam defensive("Defensive", "Coach", "Stadium", 75000);

    addBalancedPlayers(balanced, "B");
    addAttackingPlayers(attacking, "A");
    addDefensivePlayers(defensive, "D");

    expect(balanced.isValidTeamForType(), "balanced team composition is valid");
    expect(attacking.isValidTeamForType(), "attacking team composition is valid");
    expect(defensive.isValidTeamForType(), "defensive team composition is valid");

    AttackingTeam buyer("Buyer", "Coach", "Stadium", 75000);
    expect(!buyer.canBuyPlayer(Player("WeakForward", 9, Position::Forward, 1000, 1000, 0, 5)),
        "attacking team rejects forward with fewer than 6 goals");
    expect(buyer.canBuyPlayer(Player("StrongForward", 10, Position::Forward, 1000, 1000, 0, 6)),
        "attacking team can buy forward with at least 6 goals");
}

void testLineup() {
    BalancedTeam team("LineupTeam", "Coach", "Stadium", 75000);
    addBalancedPlayers(team, "L");

    Lineup lineup(&team);
    fillValidLineup(lineup, "L");

    expect(!addLineupByGeneratedName(lineup, "L", "GK", 1),
        "lineup rejects duplicate or twelfth player");
    expect(lineup.isValid(), "lineup with 11 valid players is valid");

    Lineup copied(lineup);
    expect(copied.isValid() && copied.getSize() == LINEUP_SIZE,
        "lineup copy constructor preserves starters");

    Lineup assigned;
    assigned = lineup;
    expect(assigned.isValid() && assigned.getSize() == LINEUP_SIZE,
        "lineup operator= preserves starters");

    lineup.markPlayed();
    Player* goalkeeper = team.findPlayer("L_GK1");
    expect(goalkeeper != nullptr && goalkeeper->getMatchesPlayed() == 1,
        "lineup markPlayed updates player matches");
}

void testTransfersAndCopying() {
    BalancedTeam first("First", "Coach", "Stadium", 75000);
    BalancedTeam second("Second", "Coach", "Stadium", 75000);
    addBalancedPlayers(first, "FST");
    addBalancedPlayers(second, "SND");

    expect(first.exchangeWith(second, "FST_F1", "SND_F1"),
        "balanced teams can exchange equivalent players");
    expect(first.findPlayer("SND_F1") != nullptr &&
        first.findPlayer("SND_F1")->getTeam() == &first,
        "exchanged player points to new first team");
    expect(second.findPlayer("FST_F1") != nullptr &&
        second.findPlayer("FST_F1")->getTeam() == &second,
        "exchanged player points to new second team");

    BalancedTeam copied(first);
    expect(copied.findPlayer("SND_F1") != nullptr &&
        copied.findPlayer("SND_F1") != first.findPlayer("SND_F1") &&
        copied.findPlayer("SND_F1")->getTeam() == &copied,
        "team copy constructor deep-copies players");

    BalancedTeam assigned("Assigned", "Coach", "Stadium", 1000);
    assigned = copied;
    expect(assigned.findPlayer("SND_F1") != nullptr &&
        assigned.findPlayer("SND_F1")->getTeam() == &assigned,
        "team operator= deep-copies players and reconnects team pointers");
}

void testChampionshipAndSeasons() {
    Championship first = Demo::createInitialChampionship(2026);

    expect(first.getYear() == 2026, "initial championship uses requested year");
    expect(first.getTeamsCount() == 6, "initial championship has 6 teams");
    expect(first.getTotalRounds() == 10, "6-team championship has 10 rounds");
    expect(first.hasValidTeamCount(), "initial championship has valid team count");
    expect(first.allTeamsHaveValidComposition(),
        "initial championship teams satisfy composition rules");

    double initialBudget = Demo::getInitialTeamBudget();
    expect(savedChampionshipTeamsPassChecks(first, initialBudget, true, false),
        "all first-season teams have the same budget in saved data");

    Championship copied(first);
    expect(copied.getTeamsCount() == first.getTeamsCount() &&
        copied.getTotalRounds() == first.getTotalRounds(),
        "championship copy constructor preserves teams and schedule");

    Championship assigned;
    assigned = first;
    expect(assigned.getTeamsCount() == first.getTeamsCount() &&
        assigned.getTotalRounds() == first.getTotalRounds(),
        "championship operator= preserves teams and schedule");

    Championship second = first.createNextSeason(2027);
    expect(second.getYear() == 2027, "next season uses requested year");
    expect(second.getTeamsCount() == first.getTeamsCount(),
        "next season keeps the teams");
    expect(savedChampionshipTeamsPassChecks(second, initialBudget, true, true),
        "next season saves reset team and player statistics");
}

void testCollectionSaveLoad() {
    ChampionshipCollection collection;
    Championship first = Demo::createInitialChampionship(2026);
    collection.addChampionship(first);

    Championship* current = collection.getCurrentSeason();
    if (current == nullptr) {
        throw std::runtime_error("No current season after add.");
    }
    collection.addChampionship(current->createNextSeason(collection.nextYear()));

    expect(collection.nextYear() == 2028,
        "collection computes next unique year");
    expect(collection.getCurrentSeason() != nullptr &&
        collection.getCurrentSeason()->getYear() == 2027,
        "current season is the last added season");

    collection.saveAll("test_championships.txt");

    ChampionshipCollection loaded;
    loaded.loadAll("test_championships.txt");
    expect(loaded.nextYear() == 2028,
        "loaded collection preserves years");
    expect(loaded.getCurrentSeason() != nullptr &&
        loaded.getCurrentSeason()->getTeamsCount() == 6,
        "loaded collection restores current season teams");
}

void runTest(const char* name, void (*testFunction)()) {
    std::cout << "\n=== " << name << " ===\n";
    try {
        testFunction();
    }
    catch (const std::exception& ex) {
        failedChecks++;
        std::cout << "[EXCEPTION] " << name << ": " << ex.what() << "\n";
    }
    catch (...) {
        failedChecks++;
        std::cout << "[EXCEPTION] " << name << ": unknown exception\n";
    }
}

//int main() {
//    runTest("Player salary", testPlayerSalary);
//    runTest("Team validation", testTeamValidation);
//    runTest("Composition rules", testCompositionRules);
//    runTest("Lineup", testLineup);
//    runTest("Transfers and copying", testTransfersAndCopying);
//    runTest("Championship and seasons", testChampionshipAndSeasons);
//    runTest("Collection save/load", testCollectionSaveLoad);
//
//    std::cout << "\nPassed checks: " << passedChecks << "\n";
//    std::cout << "Failed checks: " << failedChecks << "\n";
//
//    return failedChecks == 0 ? 0 : 1;
//}
