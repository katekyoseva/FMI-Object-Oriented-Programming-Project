#include "FirstSeason.h"
#include "AttackingTeam.h"
#include "DefensiveTeam.h"
#include "BalancedTeam.h"
#include "Demo.h"
#include <iostream>

Position readPosition() {
    while (true) {
        char positionText[32];
        std::cout << "Position (Goalkeeper/Defender/Midfielder/Winger/Forward): ";
        if (!(std::cin >> positionText))
            throw std::runtime_error("Cannot read position.");

        Position position = positionFromString(positionText);
        if (position != Position::Unknown_Position)
            return position;

        std::cout << "Unknown position. Try again.\n";
    }
}

Team* createTeamByChoice(int typeChoice, const char* name,
    const char* coach, const char* stadium, double budget) {
    if (typeChoice == 1) {
        return new AttackingTeam(name, coach, stadium, budget);
    }
    if (typeChoice == 2) {
        return new DefensiveTeam(name, coach, stadium, budget);
    }
    if (typeChoice == 3) {
        return new BalancedTeam(name, coach, stadium, budget);
    }
    return nullptr;
}

void addPlayerFromConsole(Team* team, int playerIndex) {
    while (true) {
        char playerName[128];
        unsigned number;
        double salary;
        double transferFee;

        std::cout << "\nPlayer " << playerIndex << "\n";
        std::cout << "Name: ";
        if (!(std::cin >> playerName)) {
            throw std::runtime_error("Cannot read player name.");
        }
        std::cout << "Number: ";
        if (!(std::cin >> number)) {
            throw std::runtime_error("Cannot read player number.");
        }
        Position position = readPosition();
        std::cout << "Salary: ";
        if (!(std::cin >> salary)) {
            throw std::runtime_error("Cannot read salary.");
        }
        std::cout << "Transfer fee: ";
        if (!(std::cin >> transferFee)) {
            throw std::runtime_error("Cannot read transfer fee.");
        }

        Player player(playerName, number, position, salary, transferFee);
        if (team->addPlayer(player)) {
            return;
        }

        std::cout << "Cannot add player. Check duplicate name/number, goalkeeper limit, or team size.\n";
    }
}

Team* createTeamFromConsole(int teamIndex, double initialBudget) {
    while (true) {
        char teamName[128];
        char coachName[128];
        char stadiumName[128];
        int typeChoice;

        std::cout << "\nTeam " << teamIndex << "\n";
        std::cout << "Type: 1-Attacking, 2-Defensive, 3-Balanced: ";
        if (!(std::cin >> typeChoice)) {
            throw std::runtime_error("Cannot read team type.");
        }
        std::cout << "Team name: ";
        if (!(std::cin >> teamName)) {
            throw std::runtime_error("Cannot read team name.");
        }
        std::cout << "Coach name: ";
        if (!(std::cin >> coachName)) {
            throw std::runtime_error("Cannot read coach name.");
        }
        std::cout << "Stadium name: ";
        if (!(std::cin >> stadiumName)) {
            throw std::runtime_error("Cannot read stadium name.");
        }

        Team* team = createTeamByChoice(typeChoice, teamName, coachName,
            stadiumName, initialBudget);
        if (team == nullptr) {
            std::cout << "Invalid team type. Try again.\n";
            continue;
        }

        int playersCount;
        std::cout << "Players count (1-20): ";
        if (!(std::cin >> playersCount)) {
            delete team;
            throw std::runtime_error("Cannot read players count.");
        }
        if (playersCount < 1 || playersCount > MAX_TEAM_PLAYERS) {
            delete team;
            std::cout << "Invalid players count. Try the team again.\n";
            continue;
        }

        for (int i = 0; i < playersCount; i++) {
            addPlayerFromConsole(team, i + 1);
        }

        if (team->isValidTeamForType()) {
            return team;
        }

        delete team;
        std::cout << "The team does not satisfy its type composition rules. Re-enter this team.\n";
    }
}

Championship createFirstSeasonFromConsole(unsigned year) {
    int teamsCount;
    do {
        std::cout << "Teams count (even number, at least 4): ";
        if (!(std::cin >> teamsCount)) {
            throw std::runtime_error("Cannot read teams count.");
        }
    } while (teamsCount < 4 || teamsCount % 2 != 0);

    double initialBudget;
    do {
        std::cout << "Initial budget for every team: ";
        if (!(std::cin >> initialBudget)) {
            throw std::runtime_error("Cannot read initial budget.");
        }
    } while (initialBudget <= 0);

    Championship championship(year);

    for (int i = 0; i < teamsCount; i++) {
        bool added = false;
        while (!added) {
            Team* team = createTeamFromConsole(i + 1, initialBudget);
            if (championship.addTeam(team)) {
                added = true;
            }
            else {
                delete team;
                std::cout << "Cannot add team. The team name may already exist. Re-enter this team.\n";
            }
        }
    }

    if (!championship.hasValidTeamCount() ||
        !championship.allTeamsHaveValidComposition()) {
        throw std::runtime_error("Invalid championship data.");
    }

    championship.generateSchedule();
    return championship;
}

Championship createFirstSeason(unsigned year) {
    while (true) {
        int choice;
        std::cout << "Create first season:\n";
        std::cout << "1. Enter teams manually\n";
        std::cout << "2. Use default teams\n";
        std::cout << "Choice: ";
        if (!(std::cin >> choice)) {
            throw std::runtime_error("Cannot read season creation choice.");
        }

        if (choice == 1) {
            return createFirstSeasonFromConsole(year);
        }
        if (choice == 2) {
            return Demo::createInitialChampionship(year);
        }

        std::cout << "Invalid choice. Try again.\n";
    }
}
