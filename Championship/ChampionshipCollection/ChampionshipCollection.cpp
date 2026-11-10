#include "ChampionshipCollection.h"
#include "FirstSeason.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <utility>

ChampionshipCollection::ChampionshipCollection()
    :championshipsCount(0), championshipsCapacity(2), activeSeasonIndex(-1)
{
    championships = new Championship[2]{};
}

ChampionshipCollection::ChampionshipCollection(const ChampionshipCollection& other)
    : championshipsCount(other.championshipsCount), championshipsCapacity(other.championshipsCapacity)
    , activeSeasonIndex(other.activeSeasonIndex)
{
    championships = new Championship[other.championshipsCapacity]{};
    unsigned limit = other.championshipsCapacity >= other.championshipsCount
        ? other.championshipsCount : other.championshipsCapacity;
    for (int i = 0; i < limit; i++) {
        championships[i] = other.championships[i];
    }
}

ChampionshipCollection& ChampionshipCollection::operator=(const ChampionshipCollection& other) {
    if (this != &other) {
        ChampionshipCollection temp(other);
        std::swap(championships, temp.championships);
        std::swap(championshipsCount, temp.championshipsCount);
        std::swap(championshipsCapacity, temp.championshipsCapacity);
        std::swap(activeSeasonIndex, temp.activeSeasonIndex);
    }
    return *this;
}

ChampionshipCollection::~ChampionshipCollection() {
    delete[] championships;
}

unsigned ChampionshipCollection::nextYear() const {
    int year = 2026;
    for (int i = 0; i < championshipsCount; i++) {
        if (championships[i].getYear() >= year) {
            year = championships[i].getYear() + 1;
        }
    }
    return year;
}

void ChampionshipCollection::addChampionship(const Championship& championship) {
    if (findByYear(championship.getYear()))
        throw std::runtime_error("A championship with this year already exists.");

    ensureCapacity(championshipsCount + 1);
    championships[championshipsCount] = championship;
    activeSeasonIndex = static_cast<int>(championshipsCount);
    championshipsCount++;
}

unsigned ChampionshipCollection::startNewSeason(ChampionshipCollection& collection) {
    unsigned year = collection.nextYear();
    Championship* current = collection.getCurrentSeason();
    Championship newSeason;

    if (current == nullptr) {
        newSeason = createFirstSeason(year);
    }
    else {
        Championship* activeSeason = collection.getActiveSeason();
        if (activeSeason == nullptr) {
            throw std::runtime_error("No active season selected.");
        }
        newSeason = activeSeason->createNextSeason(year);

    }

    collection.addChampionship(newSeason);
    return year;
}

void ChampionshipCollection::listSeasons() const {
    if (championshipsCount == 0) {
        std::cout << "No seasons loaded.\n";
        return;
    }
    for (unsigned i = 0; i < championshipsCount; i++) {
        std::cout << championships[i].getYear()
            << (championships[i].isFinished() ? " finished" : " active")
            << (static_cast<int>(i) == activeSeasonIndex ? " [selected]" : "")
            << "\n";
        championships[i].printMedalists();
    }
}

Championship* ChampionshipCollection::getActiveSeason() {
    if (championshipsCount == 0 || activeSeasonIndex < 0 ||
        activeSeasonIndex >= championshipsCount)
        return nullptr;
    return &championships[championshipsCount - 1];
}

Championship* ChampionshipCollection::getCurrentSeason()
{
    return getActiveSeason();
}

bool ChampionshipCollection::selectSeason(unsigned year) {
    for (unsigned i = 0; i < championshipsCount; i++) {
        if (championships[i].getYear() == year) {
            activeSeasonIndex = i;
            return true;
        }
    }
    return false;
}

Championship* ChampionshipCollection::selectSeasonPtr(unsigned year) {
    for (unsigned i = 0; i < championshipsCount; i++) {
        if (championships[i].getYear() == year) {
            activeSeasonIndex = i;
            return &championships[i];
        }
    }
    return nullptr;
}

unsigned ChampionshipCollection::getCurrentSeasonYear() const
{
    return getActiveSeasonYear();
}

unsigned ChampionshipCollection::getActiveSeasonYear() const {
    if (championshipsCount == 0 || activeSeasonIndex < 0 ||
        activeSeasonIndex >= championshipsCount)
        return 0;
    return championships[activeSeasonIndex].getYear();
}

void ChampionshipCollection::saveAll(const char* fileName) const {
    std::ofstream out(fileName);
    if (!out) throw std::runtime_error("Cannot open file for saving.");
    out << championshipsCount << "\n";
    for (int i = 0; i < championshipsCount; i++) {
        championships[i].save(out);
    }
    out.close();
}

void ChampionshipCollection::loadAll(const char* fileName) {
    std::ifstream in(fileName);
    if (!in) throw std::runtime_error("Cannot open file for loading.");
    int count;
    in >> count;

    ChampionshipCollection temp;
    temp.ensureCapacity(count);

    for (int i = 0; i < count; i++) {
        temp.championships[i].load(in);
    }
    temp.championshipsCount = count;

    temp.activeSeasonIndex = count > 0 ? count - 1 : -1;
    *this = temp;
    in.close();
}

Championship* ChampionshipCollection::findByYear(unsigned year) {
    for (unsigned i = 0; i < championshipsCount; i++) {
        if (championships[i].getYear() == year)
            return &championships[i];
    }
    return nullptr;
}

void ChampionshipCollection::ensureCapacity(unsigned needed) {
    if (needed <= championshipsCapacity)
        return;
    unsigned newCapacity = championshipsCapacity == 0 ? 2 : championshipsCapacity * 2;
    while (newCapacity < needed)  newCapacity *= 2;

    Championship* newChampionships = new Championship[newCapacity];
    unsigned limit = newCapacity >= championshipsCount ? championshipsCount : newCapacity;

    for (int i = 0; i < limit; i++) {
        newChampionships[i] = championships[i];
    }
    delete[] championships;
    championships = newChampionships;
    championshipsCapacity = newCapacity;
}