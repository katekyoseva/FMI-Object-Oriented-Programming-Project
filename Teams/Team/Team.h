#pragma once
#include "Player.h"
#include <fstream>

const int MAX_TEAM_PLAYERS = 20;
/* Base class for all team types.
 *
 * Team owns the Player pointers stored in the fixed squad array. 
 * Derived classes define only the concrete squad and transfer rules; 
 * the base class handles common identity, statistics, transfers, and serialization.
 */

class Team {
public:
    Team();
    Team(const char* name, const char* coach, const char* stadium, double budget);
    Team(const Team& other);
    Team& operator=(const Team& other);
    virtual ~Team();

    /* Polymorphic interface implemented by Attacking / Defensive / Balanced teams. */
    virtual const char* getType() const = 0;
    virtual Team* clone() const = 0;
    virtual bool isValidTeamForType() const = 0;
    virtual bool canBuySpecificPlayer(const Player& player) const = 0;

    const char* getName() const { return name; }
    const char* getCoach() const { return coach; }
    const char* getStadium() const { return stadium; }
    unsigned getPoints() const { return points; }
    unsigned getGoalsFor() const { return goalsFor; }
    unsigned getMatchesPlayedByTeam() const { return totalGames; }
    unsigned getPlayersCount() const { return playersCount; }
    double getBudget() const { return budget; }
    int getGoalDifference() const { return (goalsFor - goalsAgainst); }

    Player* findPlayer(const char* playerName);
    const Player* findPlayer(const char* playerName) const;
    Player* getPlayerAt(int index);
    const Player* getPlayerAt(int index) const;

    /* Adds a copy of a player after checking unique name, unique number,
     * maximum squad size, and goalkeeper limit.
     */
    bool addPlayer(const Player& player);
    /* Removes an owned player and returns a value copy, which is used by
     * transfers and rollback logic.
     */
    bool removePlayer(const char* playerName, Player& removed);
    /* Buys a player only after budget, base squad rules, and type rules pass. */
    bool buyPlayer(const Player& player);
    /* Adds money received from selling a player to another team. */
    void addTransferIncome(double amount);
    /* Exchanges players and rolls back if either final squad becomes invalid. */
    bool exchangeWith(Team& other, const char* ownPlayerName, const char* otherPlayerName);

    double allSalaries() const;
    bool canBuyPlayer(const Player& player) const;
    bool canExchangeWith(Team& other, const char* ownPlayerName, const char* otherPlayerName) const;

    void updateAfterMatch(int scored, int conceded);
    void startNewSeason();
    void regulateSalaries();

    void print() const;
    void printForStandings() const;
    void printPlayers() const;

    void saveBase(std::ofstream& out) const;
    void loadBase(std::ifstream& in);

protected:
    /* These setters replace owned dynamic text fields with deep copies. */
    void setName(const char* newName);
    void setStadium(const char* newStadium);
    void setCoach(const char* newCoach);

    /* Validation helpers used before mutating the squad. */
    int countPosition(Position position) const;
    bool hasPlayerName(const char* playerName) const;
    bool hasPlayerNumber(int number) const;
    bool isValidTeam(Position position) const;

    void clearStats();
    /* Teams deep-copy and delete their players, so copied teams never share Player pointers. */
    void copyFrom(const Team& other);
    void free();
protected:
    char* name = nullptr;
    char* coach = nullptr;
    char* stadium = nullptr;
    Player* players[MAX_TEAM_PLAYERS]{};
    unsigned playersCount;
    double budget;

    unsigned totalGames;
    unsigned wins;
    unsigned draws;
    unsigned losses;
    unsigned points;
    unsigned goalsFor;
    unsigned goalsAgainst;
};
