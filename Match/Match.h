#pragma once
#include "Lineup.h"
#include <fstream>

/* Represents one scheduled football match.
 *
 * Match does not own homeTeam and awayTeam. It keeps owned copies of team names
 * and scorer names so match history can still be saved after copying/loading.
 */

class Match {
public:
    Match();
    Match(Team* homeTeam, Team* awayTeam, int roundNumber);
    Match(const Match& other);
    Match& operator=(const Match& other);
    ~Match();

    bool isPlayed() const { return played; }
    int getRoundNumber() const { return roundNumber; }
    const char* getHomeTeamName() const { return homeTeamName; }
    const char* getAwayTeamName() const { return awayTeamName; }

    /* Reconnects non-owning team pointers after loading/copying from saved names. */
    void reconnectTeams(Team* homeTeam, Team* awayTeam);

    /* Runs the full console flow: choose lineups, enter result, enter scorers. 
     * The late - season rule starts after two thirds of the season, and is also
     * forced in the last three rounds.
     */
    void playFromConsole(int totalRounds);

    void print() const;

    /* Serialization stores team/scorer names, not raw pointers. Championship
     * reconnects teams after all teams and matches are loaded.
     */
    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

private:
    /* Frees only owned text and scorer arrays; teams are non-owning links*/
    void free();
    void resize();
    void addScorerName(const char* scorer);
    void setHomeName(const char* name);
    void setAwayName(const char* name);
private:
    Team* homeTeam = nullptr;
    Team* awayTeam = nullptr;
    char* homeTeamName = nullptr;
    char* awayTeamName = nullptr;
    Lineup homeLineup;
    Lineup awayLineup;
    int homeGoals;
    int awayGoals;
    int roundNumber;
    bool played;
    char** scorers = nullptr;
    unsigned scorersCount;
    unsigned scorersCapacity;
};
