#pragma once
#include "Position.h"
#include <fstream>

class Team;
/* Represents one football player and his season statistics.
 * 
 * Player owns only the dynamically allocated name. The Team pointer is
 * non-owning and is set by Team when the player is added to a squad.
 */
class Player
{
public:
    Player();
    Player(const char* name, unsigned number, Position position,
        double salary, double transferFee, unsigned matchesPlayed = 0,
        unsigned goalsScored = 0);
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    const char* getName() const { return name; }
    unsigned getNumber() const { return number; }
    double getSalary() const { return salary; }
    double getTransferFee() const { return transferFee; }
    unsigned getMatchesPlayed() const { return matchesPlayed; }
    unsigned getGoalsScored() const { return goalsScored; }
    Position getPosition() const { return position; }
    Team* getTeam() const { return team; }

    void setTeam(Team* team);
    void setNumber(int number);
    void addMatch();
    void addGoal();
    void resetSeasonStats();

     /* Applies the salary regulation rules in the order from the assignment.
      * It is called after a season/period because it depends on accumulated
      * appearances and goals.
      */
    void regulateSalary(unsigned teamMatches);

    void print() const;

    /* Serialization stores the player data, not the team pointer. The team
     * pointer is restored when the player is added back to a Team.
     */
    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

private:
    /* Replaces the owned name buffer with a deep copy of newName. */
    void setName(const char* newName);
private:
    char* name = nullptr;
    unsigned number;
    Position position;
    double salary;
    double transferFee;
    unsigned matchesPlayed;
    unsigned goalsScored;

    Team* team = nullptr;
};