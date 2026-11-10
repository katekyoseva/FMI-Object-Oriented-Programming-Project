#include <iostream>
#include <exception>
#include "ChampionshipCollection.h"

static void printMenu() {
    std::cout << "\nFootball Championship\n";

    std::cout << "1. Start new season\n";
    std::cout << "2. Open old season\n";
    std::cout << "3. List seasons\n";
    std::cout << "4. Show schedule\n";
    std::cout << "5. Play next round from console\n";
    std::cout << "6. Play selected match from round\n";
    std::cout << "7. Finish season by entering results\n";
    std::cout << "8. Show standings\n";
    std::cout << "9. Show top scorer\n";
    std::cout << "10. Save to file\n";
    std::cout << "11. Load from file\n";
    std::cout << "12. Exchange players\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

static void playSelectedMatchFromConsole(Championship* championship);
static void selectSeasonFromConsole(ChampionshipCollection& collection);
static void exchangePlayersFromConsole(Championship* championship);

static Championship* getSelectedSeasonOrWarn(ChampionshipCollection& collection) {
    Championship* current = collection.getActiveSeason();
    if (current == 0) {
        std::cout << "Create or load a season first.\n";
    }
    return current;
}

int main() {
    ChampionshipCollection collection;
    bool running = true;

    while (running) {
        try {
            printMenu();
            int choice;
            if (!(std::cin >> choice)) {
                running = false;
                continue;
            }

            if (choice == 1) {
                unsigned year = ChampionshipCollection::startNewSeason(collection);
                std::cout << "Season " << year << " started.\n";
            }
            else if (choice == 2) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                unsigned year;

                collection.listSeasons();
                std::cout << "Year to select: ";
                std::cin >> year;
                if (collection.selectSeason(year)) {
                    std::cout << "Selected season " << year << ".\n";

                }
                else {
                    std::cout << "Season not found.\n";
                }
            }
            else if (choice == 3) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                collection.listSeasons();
            }
            else if (choice == 4) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    current->printSchedule();
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 5) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    current->playRoundFromConsole();
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 6) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    playSelectedMatchFromConsole(current);
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 7) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    current->playAllRemainingManual();
                    std::cout << "Season finished.\n";
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 8) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    current->printStandings();
                    current->printMedalists();
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 9) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    current->printTopScorers();
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 10) {
                collection.saveAll("championships.txt");
                std::cout << "Saved to championships.txt\n";
            }
            else if (choice == 11) {
                collection.loadAll("championships.txt");
                std::cout << "Loaded from championships.txt\n";
            }
            else if (choice == 12) {
                Championship* current = getSelectedSeasonOrWarn(collection);
                if (current) {
                    exchangePlayersFromConsole(current);
                }
                else {
                    std::cout << "Create or load a season first.\n";
                }
            }
            else if (choice == 0) {
                running = false;
            }
            else {
                std::cout << "Unknown option.\n";
            }
        }
        catch (const std::exception& ex) {
            std::cout << "Error: " << ex.what() << "\n";
        }
    }

    return 0;
}

static void selectSeasonFromConsole(ChampionshipCollection& collection) {
    unsigned year;

    collection.listSeasons();
    std::cout << "Year to select: ";
    std::cin >> year;
    if (collection.selectSeason(year)) {
        std::cout << "Selected season " << year << ".\n";

    }
    else {
        std::cout << "Season not found.\n";
    }
}

static void playSelectedMatchFromConsole(Championship* championship) {
    unsigned roundNumber;
    unsigned matchNumber;

    std::cout << "Round number: ";
    std::cin >> roundNumber;
    championship->printRoundSchedule(roundNumber);

    std::cout << "Match number in round: ";
    std::cin >> matchNumber;
    if (championship->playMatchFromRound(roundNumber, matchNumber)) {
        std::cout << "Selected match played.\n";
    }
}

static void exchangePlayersFromConsole(Championship* championship) {
    char firstTeam[128];
    char firstPlayer[128];
    char secondTeam[128];
    char secondPlayer[128];

    championship->printTeams();
    std::cout << "First team: ";
    std::cin >> firstTeam;
    std::cout << "First player: ";
    std::cin >> firstPlayer;
    std::cout << "Second team: ";
    std::cin >> secondTeam;
    std::cout << "Second player: ";
    std::cin >> secondPlayer;

    if (championship->exchangePlayers(firstTeam, firstPlayer, secondTeam, secondPlayer)) {
        std::cout << "Exchange completed.\n";
    }
    else {
        std::cout << "Exchange rejected.\n";
    }
}