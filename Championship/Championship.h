#pragma once
#include "Match.h"
#include "Team.h"
#include <fstream>

/* Represents one season of the championship.
 *
 * Championship owns its teams and matches. 
 * Teams are stored polymorphically through Team pointers
 * matches keep non-owning links back to those teams.
 */

class Championship {
public:
    Championship();
    Championship(int year);
    Championship(const Championship& other);
    Championship& operator=(const Championship& other);
    ~Championship();

    unsigned getYear() const { return year; }
    bool isFinished() const { return finished; }
    unsigned getTeamsCount() const { return teamsCount; }
    unsigned getTotalRounds() const;

    bool addTeam(Team* team);

    /* Generates a schedule where every team plays every other twice. 
     * Teams are paired from opposite ends of a rotating order array.
     * For each pairing, both the original match and the reverse fixture are added,
     * so every team plays every other team twice: once at home and once away.
     * The first team remains fixed while the others rotate after each round.
     */
    void generateSchedule();
    void playRoundFromConsole();
    void playAllRemainingManual();
    bool playMatchFromRound(unsigned roundNumber, unsigned matchNumberInRound);

    void printSchedule() const;
    void printRoundSchedule(unsigned roundNumber) const;
    void printStandings() const;
    void printMedalists() const;
    /* Finds top scorer(s) by goals, then fewer matches as the tie-breaker. */
    void printTopScorers() const;
    void printTeams() const;

    void regulateAllSalaries();

    bool exchangePlayers(const char* firstTeamName, const char* firstPlayerName,
        const char* secondTeamName, const char* secondPlayerName);

    /* Builds the next season by cloning teams and resetting season statistics. */
    Championship createNextSeason(unsigned newYear) const;

    /* save/load persist the complete season. Loading rebuilds temporary data
     * first, so invalid files do not destroy the current object.
     */
    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

    bool hasValidTeamCount() const;
    bool allTeamsHaveValidComposition() const;
    Team* findTeam(const char* teamName) const;
    int findTeamIndex(const char* teamName) const;

private:
    void clearTeams();
    void clearMatches();

    /* Matches store team names in files, so after copy/load their pointers must
     * be reconnected to this season's Team objects.
     */
    void reconnectMatches();
    /* After end of season team are sorted by points. */
    void sortTeamPointers(Team** ordered, int count) const;
    void ensureTeamsCapacity(unsigned needed);
    void ensureMatchesCapacity(unsigned needed);

    bool isRoundFinished(unsigned roundNumber) const;
    bool allMatchesPlayed() const;
    void updateProgressAfterPlaying();
    void finishSeason();

    void addTeamPointer(Team* team);
    void addMatch(const Match& match);
private:
    unsigned year;
    Team** teams = nullptr;
    unsigned teamsCount;
    unsigned teamsCapacity;
    Match* matches = nullptr;
    unsigned matchesCount;
    unsigned matchesCapacity;
    unsigned currentRound;
    bool finished;
};