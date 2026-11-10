#pragma once
#include "Team.h"

const unsigned LINEUP_SIZE = 11;

/* Represents the starting eleven for one match.
 *
 * Lineup does not own the Team or Player objects. It only stores pointers to
 * players that already belong to the connected team.
 */

class Lineup {
public:
    Lineup();
    Lineup(Team* team);
    Lineup(const Lineup& other);
    Lineup& operator=(const Lineup& other);
    ~Lineup();

    Team* getTeam() const { return team; }
    unsigned getSize() const { return startersCount; }

    /* Adds a starter by name only if he exists in the team and is not duplicated. */
    bool addPlayerByName(const char* playerName);

    void chooseFromConsole(bool lastThreeGames);

    /* Late in the season, tries to include players with fewer than 3 matches
     * without breaking the required position structure.
     * If the added player breaks the required position structure they get removed
     * from the lineup.
     */
    void tryIncludeLowMatchPlayers();

    /* Checks 11 starters, exactly one goalkeeper, and minimum outfield positions. */
    bool isValid() const;

    void markPlayed();

    void print() const;

    /* Files store player names, not addresses.Loading reconnects those names
     * to real Player objects in the provided team.
     */
    void save(std::ofstream& os) const;
    void load(std::ifstream& is, Team* team);

private:
    bool contains(Player* player) const;
    unsigned countPosition(Position position) const;
    bool isPlayerInTeam(const char* playerName) const;
private:
    Team* team = nullptr;
    Player* starters[LINEUP_SIZE]{};
    unsigned startersCount;
};