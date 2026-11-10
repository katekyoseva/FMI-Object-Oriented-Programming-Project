#pragma once
#include "Championship.h"

/* Owns the history of seasons.
 *
 * Championship years are unique. Starting a season either creates the initial
 * prepared season or clones the latest season with reset statistics.
 */

class ChampionshipCollection {
public:
    ChampionshipCollection();
    ChampionshipCollection(const ChampionshipCollection& other);
    ChampionshipCollection& operator=(const ChampionshipCollection& other);
    ~ChampionshipCollection();

    unsigned nextYear() const;
    /* Starts a new automatically numbered season and returns its year. */
    static unsigned startNewSeason(ChampionshipCollection& collection);
    /* Adds an existing season only when its year is not already used. */
    void addChampionship(const Championship& championship);

    void listSeasons() const;
    Championship* getCurrentSeason();
    Championship* getActiveSeason();
    bool selectSeason(unsigned year);
    Championship* selectSeasonPtr(unsigned year);
    unsigned getActiveSeasonYear() const;
    unsigned getCurrentSeasonYear() const;

    /* saveAll / loadAll persist the full history. Loading uses a temporary
     * collection so existing data survives failed loads.
     */
    void saveAll(const char* fileName) const;
    void loadAll(const char* fileName);
private:
    Championship* findByYear(unsigned year);
    void ensureCapacity(unsigned needed);
private:
    Championship* championships = nullptr;
    unsigned championshipsCount;
    unsigned championshipsCapacity;
    int activeSeasonIndex;
};

