#include "Match.h"
#include "TextUtil.h"
#include <iostream>

#pragma warning (disable:4996)

Match::Match()
    : homeTeam(nullptr), awayTeam(nullptr),
    homeLineup(nullptr), awayLineup(nullptr),
    homeGoals(0), awayGoals(0), roundNumber(0), played(false),
    scorersCount(0), scorersCapacity(2) 
{
    setHomeName("Unknown");
    try
    {
        setAwayName("Unknown");
    }
    catch (...)
    {
        delete[] homeTeamName;
        throw;
    }
    scorers = new char* [2];
}

Match::Match(Team* homeTeam, Team* awayTeam, int roundNumber)
    : homeTeam(homeTeam), awayTeam(awayTeam),
    homeLineup(homeTeam), awayLineup(awayTeam),
    homeGoals(0), awayGoals(0), roundNumber(roundNumber), played(false)
    , scorersCount(0), scorersCapacity(2) 
{
    if (homeTeam == nullptr || awayTeam == nullptr)
        throw std::invalid_argument("Match teams cannot be null.");

    setHomeName(homeTeam->getName());
    try
    {
        setAwayName(awayTeam->getName());
    }
    catch(...)
    {
        delete[] homeTeamName;
        throw;
    }
    scorers = new char* [2];
}

Match::Match(const Match& other) 
    :homeTeam(other.homeTeam), awayTeam(other.awayTeam),
    homeLineup(other.homeLineup), awayLineup(other.awayLineup),
    homeGoals(other.homeGoals), awayGoals(other.awayGoals),
    roundNumber(other.roundNumber), played(other.played)
{
    if (!other.homeTeam || !other.awayTeam) throw std::invalid_argument("Team nullptr!");
    setHomeName(other.homeTeam->getName());

    char** newScorers = nullptr;
    unsigned allocatedCount = 0;
    try
    {
        setAwayName(other.awayTeam->getName());
        if (other.scorersCapacity > 0) {
            newScorers = new char*[other.scorersCapacity] {};
            unsigned limit = other.scorersCapacity >= other.scorersCount ? other.scorersCount : other.scorersCapacity;
            for (allocatedCount = 0; allocatedCount < limit; allocatedCount++) {
                newScorers[allocatedCount] = copyDynamic(other.scorers[allocatedCount]);
            }
        }
    }
    catch (...)
    {
        delete[] homeTeamName;
        delete[] awayTeamName;
        if (newScorers) {
            for (unsigned i = 0; i < allocatedCount; i++) {
                delete[] newScorers[i];
            }
            delete[] newScorers;
        }
        throw;
    }

    scorers = newScorers;
    scorersCount = other.scorersCount;
    scorersCapacity = other.scorersCapacity;
    reconnectTeams(other.homeTeam, other.awayTeam);
}

Match& Match::operator=(const Match& other) {
    if (this != &other) {
        char* newHomeTeamName = nullptr;
        char* newAwayTeamName = nullptr;
        char** newScorers = nullptr;

        try {
            newHomeTeamName = copyDynamic(other.homeTeamName);
            newAwayTeamName = copyDynamic(other.awayTeamName);
            if (other.scorersCapacity > 0) {
                newScorers = new char* [other.scorersCapacity] {};
                unsigned limit = other.scorersCapacity >= other.scorersCount ? other.scorersCount : other.scorersCapacity;
                for (unsigned i = 0; i < limit; i++) {
                    newScorers[i] = copyDynamic(other.scorers[i]);
                }
            }
        }
        catch (...) {
            delete[] newHomeTeamName;
            delete[] newAwayTeamName;
            if (newScorers) {
                for (unsigned i = 0; i < other.scorersCapacity; i++) {
                    delete[] newScorers[i];
                }
                delete[] newScorers;
            }
            throw;
        }

        free();
        homeTeamName = newHomeTeamName;
        awayTeamName = newAwayTeamName;
        scorers = newScorers;
        homeLineup = other.homeLineup;
        awayLineup = other.awayLineup;
        homeGoals = other.homeGoals;
        awayGoals = other.awayGoals;
        roundNumber = other.roundNumber;
        played = other.played;
        scorersCount = other.scorersCount;
        scorersCapacity = other.scorersCapacity;

        reconnectTeams(other.homeTeam, other.awayTeam);
    }
    return *this;
}

Match::~Match() {
    free();
}

void Match::reconnectTeams(Team* homeTeam, Team* awayTeam) {
    if (homeTeam != nullptr && strcmp(homeTeam->getName(), homeTeamName) != 0)
        throw std::logic_error("Home team pointer does not match home team name.");

    if (awayTeam != nullptr && strcmp(awayTeam->getName(), awayTeamName) != 0)
        throw std::logic_error("Away team pointer does not match away team name.");

    this->homeTeam = homeTeam;
    this->awayTeam = awayTeam;
    homeLineup = Lineup(homeTeam);
    awayLineup = Lineup(awayTeam);
}

void Match::playFromConsole(int totalRounds) {
    if (played) {
        std::cout << "Match already played.\n";
        return;
    }
    if (homeTeam == nullptr || awayTeam == nullptr) {
        std::cout << "Match teams are not connected. Match cannot start.\n";
        return;
    }

    bool lateSeasonRule = roundNumber > (totalRounds * 2) / 3;
    if (totalRounds - roundNumber <= 3) lateSeasonRule = true;

    std::cout << "\nMatch: " << homeTeamName << " vs " << awayTeamName << "\n";
    homeLineup.chooseFromConsole(lateSeasonRule);
    awayLineup.chooseFromConsole(lateSeasonRule);

    if (!homeLineup.isValid() || !awayLineup.isValid()) {
        std::cout << "Invalid lineup. Match cannot start.\n";
        return;
    }

    std::cout << "Home goals: ";
    std::cin >> homeGoals;
    std::cout << "Away goals: ";
    std::cin >> awayGoals;
    if (homeGoals < 0 || awayGoals < 0) {
        std::cout << "Goals cannot be negative.\n";
        return;
    }

    for (int i = 0; i < homeGoals + awayGoals; i++) {
        std::cout << "Goal " << i + 1 << " team (H/A): ";
        char side[8];
        std::cin >> side;
        std::cout << "Scorer name: ";
        char scorerName[128];
        std::cin >> scorerName;
        Player* scorer = nullptr;
        if (strcmp(side, "H")== 0 || strcmp(side, "h") == 0) {
            scorer = homeTeam->findPlayer(scorerName);
        }
        else {
            scorer = awayTeam->findPlayer(scorerName);
        }
        if (!scorer) {
            std::cout << "Scorer not found. Goal is saved only in match result.\n";
            addScorerName("Unknown");
        }
        else {
            addScorerName(scorer->getName());
            scorer->addGoal();
        }
    }

    homeLineup.markPlayed();
    awayLineup.markPlayed();
    homeTeam->updateAfterMatch(homeGoals, awayGoals);
    awayTeam->updateAfterMatch(awayGoals, homeGoals);
    played = true;
}

void Match::print() const {
    std::cout << "Round " << roundNumber << ": " << homeTeamName << " - "
        << awayTeamName;
    if (played) {
        std::cout << " " << homeGoals << ":" << awayGoals;
    }
    else {
        std::cout << " (not played)";
    }
    std::cout << "\n";
}

void Match::save(std::ofstream& out) const {
    out << roundNumber << " " << homeTeamName << " " << awayTeamName << " "
        << homeGoals << " " << awayGoals << " " << played << " "
        << scorersCount << "\n";
    for (unsigned i = 0; i < scorersCount; i++) {
        out << scorers[i] << "\n";
    }
}

void Match::load(std::ifstream& in) {
    int newRoundNumber;
    int newHomeGoals;
    int newAwayGoals;
    bool newPlayed;
    int newScorersCount;
    char homeBuffer[128];
    char awayBuffer[128];
    in >> newRoundNumber >> homeBuffer >> awayBuffer >> newHomeGoals
        >> newAwayGoals >> newPlayed >> newScorersCount;

    Match temp;
    delete[] temp.homeTeamName;
    delete[] temp.awayTeamName;
    temp.homeTeamName = copyDynamic(homeBuffer);
    temp.awayTeamName = copyDynamic(awayBuffer);
    temp.roundNumber = newRoundNumber;
    temp.homeGoals = newHomeGoals;
    temp.awayGoals = newAwayGoals;
    temp.played = newPlayed;
    temp.homeTeam = 0;
    temp.awayTeam = 0;

    for (int i = 0; i < newScorersCount; i++) {
        char scorer[128];
        in >> scorer;
        temp.addScorerName(scorer);
    }

    *this = temp;
}

void Match::free() {
    delete[] homeTeamName;
    delete[] awayTeamName;

    for (unsigned i = 0; i < scorersCount; i++) {
        delete[] scorers[i];
    }
    delete[] scorers;
}

void Match::resize()
{
    int newCapacity = scorersCapacity * 2;
    char** newScorers = new char* [newCapacity] {};
    for (unsigned i = 0; i < scorersCount; i++) {
        newScorers[i] = scorers[i];
    }

    delete[] scorers;
    scorers = newScorers;
    scorersCapacity = newCapacity;
}

void Match::addScorerName(const char* scorer) {
    if (scorersCount >= scorersCapacity)
        resize();

    char* newName = copyDynamic(scorer);
    scorers[scorersCount] = newName;
    scorersCount++;
}

void Match::setHomeName(const char* newName)
{
    char* temp = copyDynamic(newName);
    delete[] homeTeamName;
    homeTeamName = temp;
}

void Match::setAwayName(const char* newName)
{
    char* temp = copyDynamic(newName);
    delete[] awayTeamName;
    awayTeamName = temp;
}