#include "Lineup.h"
#include <iostream>

Lineup::Lineup() 
    : Lineup(nullptr)
{}

Lineup::Lineup(Team* team) 
    : team(team), startersCount(0) 
{}

Lineup::Lineup(const Lineup& other) 
{   
    for (unsigned i = 0; i < LINEUP_SIZE; i++) {
        starters[i] = other.starters[i];
    }
    team = other.team;
    startersCount = other.startersCount;
}

Lineup& Lineup::operator=(const Lineup& other) {
    if (this != &other) 
    {
        for (unsigned i = 0; i < LINEUP_SIZE; i++) {
            starters[i] = other.starters[i];
        }
        team = other.team;
        startersCount = other.startersCount;
    }
    return *this;
}

Lineup::~Lineup()
{
    for (unsigned i = 0; i < LINEUP_SIZE; i++) {
        starters[i] = nullptr;
    }
}

bool Lineup::addPlayerByName(const char* playerName) {
    if (team == nullptr || startersCount >= LINEUP_SIZE) return false;
    if (!isPlayerInTeam(playerName)) return false;
    Player* player = team->findPlayer(playerName);
    if (contains(player)) return false;
    
    starters[startersCount] = player;
    startersCount++;
    return true;
}

void Lineup::chooseFromConsole(bool lastThreeGames) {
    if (team == nullptr) {
        std::cout << "Lineup has no connected team.\n";
        return;
    }
    startersCount = 0;
    for (unsigned i = 0; i < LINEUP_SIZE; i++) {
        starters[i] = nullptr;
    }

    std::cout << "Players in " << team->getName() << ":\n";
    team->printPlayers();
    if (lastThreeGames) 
    {
        tryIncludeLowMatchPlayers();
        if (startersCount > 0) {
            std::cout << "Players with less then 3 matches played included automatically where possible:\n";
            print();
        }
    }
    while (startersCount < LINEUP_SIZE) {
        std::cout << "Enter player name for lineup " << startersCount + 1
            << "/11: ";
        char playerNameBuffer[128];
        std::cin >> playerNameBuffer;
        if (!addPlayerByName(playerNameBuffer)) {
            std::cout << "Invalid or duplicate player. Try again.\n";
        }
    }
}

void Lineup::tryIncludeLowMatchPlayers() {
    if (team == nullptr) return;

    for (unsigned i = 0; i < team->getPlayersCount() && startersCount < LINEUP_SIZE; i++) {
        Player* player = team->getPlayerAt(i);
        if (player != nullptr && player->getMatchesPlayed() < 3 && !contains(player)) {
            starters[startersCount] = player;
            startersCount++;
            if (!isValid() && startersCount == LINEUP_SIZE) {
                startersCount--;
                starters[startersCount] = nullptr;
            }
        }
    }
}

bool Lineup::isValid() const {
    return startersCount == LINEUP_SIZE &&
        countPosition(Position::Goalkeeper) == 1 &&
        countPosition(Position::Defender) >= 2 &&
        countPosition(Position::Midfielder) >= 2 &&
        countPosition(Position::Winger) >= 2 &&
        countPosition(Position::Forward) >= 2;
}

void Lineup::markPlayed() {
    for (unsigned i = 0; i < startersCount; i++) {
        starters[i]->addMatch();
    }
}

void Lineup::print() const {
    std::cout << team->getName() << " lineup: \n";
    for (unsigned i = 0; i < startersCount; i++) {
        std::cout << starters[i]->getName() << " ("
            << positionToString(starters[i]->getPosition()) << ")\n";
    }
}

void Lineup::save(std::ofstream& os) const {
    os << startersCount << "\n";
    for (unsigned i = 0; i < startersCount; i++) {
        os << starters[i]->getName() << "\n";
    }
}

void Lineup::load(std::ifstream& is, Team* team) {
    unsigned newStartersCount;
    is >> newStartersCount;
    if (newStartersCount < 0 || newStartersCount > LINEUP_SIZE)
        throw std::runtime_error("Invalid lineup size in file.");

    if (team == nullptr && newStartersCount > 0)
        throw std::runtime_error("Cannot load lineup without team.");

    Lineup temp(team);
    for (unsigned i = 0; i < newStartersCount; i++) {
        char playerName[128];
        is >> playerName;
        if (!temp.addPlayerByName(playerName))
            throw std::runtime_error("Invalid player in lineup file.");
    }
    *this = temp;
}

bool Lineup::contains(Player* player) const {
    for (unsigned i = 0; i < startersCount; i++) {
        if (starters[i] == player) {
            return true;
        }
    }
    return false;
}

unsigned Lineup::countPosition(Position position) const {
    int count = 0;
    for (unsigned i = 0; i < startersCount; i++) {
        if (starters[i]->getPosition() == position) {
            count++;
        }
    }
    return count;
}

bool Lineup::isPlayerInTeam(const char* playerName) const
{
    return team->findPlayer(playerName);
}