#include "Championship.h"
#include "TeamFactory.h"
#include <iostream>
#include <stdexcept>

Championship::Championship()
    : Championship(1960)
{}

Championship::Championship(int year)
    : year(year), teamsCount(0), teamsCapacity(2),
    matchesCount(0), matchesCapacity(2), currentRound(1), finished(false) 
{
    teams = new Team * [2] {};
    try
    {
        matches = new Match[2]{};
    }
    catch (...)
    {
        for (size_t i = 0; i < matchesCapacity; i++)
            delete teams[i];
        delete[] teams;
        throw;
    }
}

Championship::Championship(const Championship& other)
    : year(other.year), teamsCount(other.teamsCount), 
    teamsCapacity(other.teamsCapacity),
    matchesCount(other.matchesCount), matchesCapacity(other.matchesCapacity), 
    currentRound(other.currentRound), finished(other.finished) 
{
    teams = new Team * [other.teamsCapacity] {};
    try
    {
        unsigned limit = other.teamsCapacity >= other.teamsCount ? other.teamsCount : other.teamsCapacity;
        for (unsigned i = 0; i < limit; i++) {
            teams[i] = other.teams[i]->clone();
        }
        matches = new Match[other.matchesCapacity]{};
        unsigned limitMatches = other.matchesCapacity >= other.matchesCount ? other.matchesCount : other.matchesCapacity;
        for (unsigned i = 0; i < limitMatches; i++) {
            matches[i] = other.matches[i];
        }

        reconnectMatches();
    }
    catch(...)
    {
        clearTeams();
        clearMatches();
        throw;
    }
}

Championship& Championship::operator=(const Championship& other) {
    if (this != &other) {
    Team** tempTeams = new Team * [other.teamsCapacity] {};
    Match* tempMatches = nullptr;
    try
    {
        unsigned limit = other.teamsCapacity >= other.teamsCount ? other.teamsCount : other.teamsCapacity;
        for (unsigned i = 0; i < limit; i++) {
            tempTeams[i] = other.teams[i]->clone();
        }
        tempMatches = new Match[other.matchesCapacity]{};
        unsigned limitMatches = other.matchesCapacity >= other.matchesCount ? other.matchesCount : other.matchesCapacity;
        for (unsigned i = 0; i < limitMatches; i++) {
            tempMatches[i] = other.matches[i];
        }
    }
    catch (...)
    {
        unsigned limit = other.teamsCapacity >= other.teamsCount ? other.teamsCount : other.teamsCapacity;
        for (unsigned i = 0; i < limit; i++) {
            delete tempTeams[i];
        }
        delete[] tempTeams;
        delete[] tempMatches;
        throw;
    }

    clearTeams();
    clearMatches();
    teams = tempTeams;
    matches = tempMatches;
    year = other.year;
    teamsCount = other.teamsCount;
    teamsCapacity = other.teamsCapacity;
    matchesCount = other.matchesCount;
    matchesCapacity = other.matchesCapacity;
    currentRound = other.currentRound;
    finished = other.finished;
    reconnectMatches();
    }
    return *this;
}

Championship::~Championship() {
    clearTeams();
    clearMatches();
}

unsigned Championship::getTotalRounds() const {
    if (teamsCount < 2) return 0;
    return (teamsCount - 1) * 2;
}

bool Championship::addTeam(Team* team) {
    if (team == nullptr || findTeam(team->getName()))
        return false;
    addTeamPointer(team);
    return true;
}

void Championship::generateSchedule() {
    clearMatches();
    if (!hasValidTeamCount()) {
        std::cout << "Invalid team count. Need even count and at least 4 teams.\n";
        return;
    }
    /* Used logic for double round-robin schedule using the circle method 
     * from https://en.wikipedia.org/wiki/Round-robin_tournament#Circle_method
     * for lines 184 - 210 but implemented by me
     */
    int n = teamsCount;
    int* order = new int[n];
    for (int i = 0; i < n; i++) {
        order[i] = i;
    }

    ensureMatchesCapacity(n * (n - 1));
    for (int round = 1; round <= n - 1; round++) {
        for (int i = 0; i < n / 2; i++) {
            Team* home = teams[order[i]];
            Team* away = teams[order[n - 1 - i]];
            if (round % 2 == 0) {
                Team* temp = home;
                home = away;
                away = temp;
            }
            addMatch(Match(home, away, round));
            addMatch(Match(away, home, round + n - 1));
        }
        int last = order[n - 1];
        for (int i = n - 1; i > 1; i--) {
            order[i] = order[i - 1];
        }
        order[1] = last;
    }
    delete[] order;
    currentRound = 1;
    finished = false;
}

void Championship::playRoundFromConsole() {
    if (finished) {
        std::cout << "Championship already finished.\n";
        return;
    }
    if (matchesCount == 0) {
        generateSchedule();
    }
    std::cout << "\nPlaying round " << currentRound << "\n";
    bool any = false;
    for (unsigned i = 0; i < matchesCount; i++) {
        if (matches[i].getRoundNumber() == currentRound && !matches[i].isPlayed()) {
            matches[i].playFromConsole(getTotalRounds());
            any = true;
        }
    }
    if (!any) {
        std::cout << "No unplayed matches in this round.\n";
    }

    updateProgressAfterPlaying();
}

void Championship::playAllRemainingManual() {
    if (matchesCount == 0)
        generateSchedule();

    for (unsigned round = currentRound; round <= getTotalRounds(); round++) {
        for (unsigned i = 0; i < matchesCount; i++) {
            if (matches[i].getRoundNumber() == round && !matches[i].isPlayed()) {
                std::cout << "Round " << currentRound << ": ";
                matches[i].playFromConsole(getTotalRounds());
            }
        }
    }
    updateProgressAfterPlaying();
    if (!finished) {
        std::cout << "Some matches are still not played.\n";
    }
}

bool Championship::playMatchFromRound(unsigned roundNumber, unsigned matchNumberInRound)
{
    if (finished) {
        std::cout << "Championship already finished.\n";
        return false;
    }
    if (matchesCount == 0) {
        generateSchedule();
    }
    if (roundNumber == 0 || roundNumber > getTotalRounds() ||
        matchNumberInRound == 0) {
        std::cout << "Invalid round or match number.\n";
        return false;
    }

    unsigned currentMatchInRound = 0;
    for (unsigned i = 0; i < matchesCount; i++) {
        if (matches[i].getRoundNumber() == roundNumber) {
            currentMatchInRound++;
            if (currentMatchInRound == matchNumberInRound) {
                if (matches[i].isPlayed()) {
                    std::cout << "Selected match is already played.\n";
                    return false;
                }

                matches[i].playFromConsole(getTotalRounds());
                if (matches[i].isPlayed()) {
                    updateProgressAfterPlaying();
                    return true;
                }
                return false;
            }
        }
    }

    std::cout << "No such match in this round.\n";
    return false;
}

void Championship::printSchedule() const {
    for (unsigned round = 1; round <= getTotalRounds(); round++) {
        std::cout << "\nRound " << round << "\n";
        for (unsigned i = 0; i < matchesCount; i++) {
            if (matches[i].getRoundNumber() == round) {
                matches[i].print();
            }
        }
    }
}

void Championship::printRoundSchedule(unsigned roundNumber) const
{
    if (roundNumber == 0 || roundNumber > getTotalRounds()) {
        std::cout << "Invalid round number.\n";
        return;
    }

    std::cout << "\nRound " << roundNumber << "\n";
    unsigned matchNumber = 0;
    for (unsigned i = 0; i < matchesCount; i++) {
        if (matches[i].getRoundNumber() == roundNumber) {
            matchNumber++;
            std::cout << matchNumber << ". ";
            matches[i].print();
        }
    }
    if (matchNumber == 0) {
        std::cout << "No matches in this round.\n";
    }
}

void Championship::printStandings() const {
    Team** ordered = new Team * [teamsCount];
    sortTeamPointers(ordered, teamsCount);
    std::cout << "\nStandings " << year << "\n";
    for (unsigned i = 0; i < teamsCount; i++) {
        std::cout << i + 1 << ". ";
        ordered[i]->printForStandings();
    }
    delete[] ordered;
}

void Championship::printMedalists() const {
    if (!finished) {
        std::cout << "Medalists are not decided yet.\n";
        return;
    }
    Team** ordered = new Team * [teamsCount];
    sortTeamPointers(ordered, teamsCount);
    if (teamsCount >= 3) {
        std::cout << "Champion: " << ordered[0]->getName() << "\n";
        std::cout << "Vice champion: " << ordered[1]->getName() << "\n";
        std::cout << "Bronze: " << ordered[2]->getName() << "\n";
    }
    delete[] ordered;
}

void Championship::printTopScorers() const {
    int bestGoals = -1;
    unsigned bestMatches = 0;
    for (unsigned i = 0; i < teamsCount; i++) {
        for (unsigned j = 0; j < teams[i]->getPlayersCount(); j++) {
            const Player* player = teams[i]->getPlayerAt(j);
            if (player != nullptr && player->getMatchesPlayed() >= 7) {
                if (player->getGoalsScored() > bestGoals ||
                    (player->getGoalsScored() == bestGoals &&
                        player->getMatchesPlayed() < bestMatches)) {
                    bestGoals = player->getGoalsScored();
                    bestMatches = player->getMatchesPlayed();
                }
            }
        }
    }
    if (bestGoals < 0) {
        std::cout << "No eligible top scorer yet.\n";
        return;
    }
    std::cout << "Top scorer(s):\n";
    for (unsigned i = 0; i < teamsCount; i++) {
        for (unsigned j = 0; j < teams[i]->getPlayersCount(); j++) {
            const Player* player = teams[i]->getPlayerAt(j);
            if (player != 0 && player->getMatchesPlayed() >= 7 &&
                player->getGoalsScored() == bestGoals &&
                player->getMatchesPlayed() == bestMatches) {
                std::cout << player->getName() << " from " << teams[i]->getName()
                    << " - " << bestGoals << " goals in "
                    << bestMatches << " matches\n";
            }
        }
    }
}

void Championship::printTeams() const
{
    for (unsigned i = 0; i < teamsCount; i++) {
        std::cout << i + 1 << ". ";
        teams[i]->print();
    }
}

void Championship::regulateAllSalaries() {
    for (unsigned i = 0; i < teamsCount; i++) {
        teams[i]->regulateSalaries();
    }
}

bool Championship::exchangePlayers(const char* firstTeamName,
    const char* firstPlayerName, const char* secondTeamName,
    const char* secondPlayerName) {
    int firstIndex = findTeamIndex(firstTeamName);
    int secondIndex = findTeamIndex(secondTeamName);
    if (firstIndex < 0 || secondIndex < 0 || firstIndex == secondIndex)
        return false;

    Team* first = teams[firstIndex];
    Team* second = teams[secondIndex];
    Team* newFirst = nullptr;
    Team* newSecond = nullptr;

    try {
        newFirst = first->clone();
        newSecond = second->clone();

        if (!newFirst->exchangeWith(*newSecond, firstPlayerName, secondPlayerName)) {
            delete newFirst;
            delete newSecond;
            return false;
        }
    }
    catch (...) {
        delete newFirst;
        delete newSecond;
        throw;
    }

    teams[firstIndex] = newFirst;
    teams[secondIndex] = newSecond;
    delete first;
    delete second;
    reconnectMatches();
    return true;
}

Championship Championship::createNextSeason(unsigned newYear) const {
    Championship next(newYear);
    next.ensureTeamsCapacity(teamsCount);

    for (unsigned i = 0; i < teamsCount; i++) {
        Team* cloned = teams[i]->clone();
        try {
            cloned->startNewSeason();
            next.addTeamPointer(cloned);
        }
        catch (...) {
            delete cloned;
            throw;
        }
    }
    next.generateSchedule();
    return next;
}

void Championship::save(std::ofstream& out) const {
    out << year << " " << currentRound << " " << finished << " "
        << teamsCount << "\n";
    for (unsigned i = 0; i < teamsCount; i++) {
        teams[i]->saveBase(out);
    }
    out << matchesCount << "\n";
    for (unsigned i = 0; i < matchesCount; i++) {
        matches[i].save(out);
    }
}

void Championship::load(std::ifstream& in) {
    Championship temp;
    unsigned teamsToRead;
    unsigned matchesToRead;
    in >> temp.year >> temp.currentRound >> temp.finished >> teamsToRead;
    temp.ensureTeamsCapacity(teamsToRead);

    for (unsigned i = 0; i < teamsToRead; i++) {
        char type[64];
        in >> type;
        Team* team = createTeamByType(type);
        if (team == 0) {
            throw std::runtime_error("Unknown team type in file.");
        }
        try {
            team->loadBase(in);
            temp.addTeamPointer(team);
        }
        catch (...) {
            delete team;
            throw;
        }
    }

    in >> matchesToRead;
    temp.ensureMatchesCapacity(matchesToRead);
    for (unsigned i = 0; i < matchesToRead; i++) {
        Match match;
        match.load(in);
        temp.addMatch(match);
    }
    temp.reconnectMatches();

    *this = temp;
}

Team* Championship::findTeam(const char* teamName) const {
    for (unsigned i = 0; i < teamsCount; i++) {
        if (strcmp(teams[i]->getName(), teamName) == 0) {
            return teams[i];
        }
    }
    return nullptr;
}

int Championship::findTeamIndex(const char* teamName) const {
    for (unsigned i = 0; i < teamsCount; i++) {
        if (strcmp(teams[i]->getName(), teamName) == 0) {
            return i;
        }
    }
    return -1;
}

bool Championship::hasValidTeamCount() const {
    return teamsCount >= 4 && teamsCount % 2 == 0;
}

bool Championship::allTeamsHaveValidComposition() const {
    for (unsigned i = 0; i < teamsCount; i++) {
        if (!teams[i]->isValidTeamForType())
            return false;
    }
    return true;
}
void Championship::clearTeams() {
    for (unsigned i = 0; i < teamsCount; i++)
        delete teams[i];
    delete[] teams;
    teams = 0;
    teamsCount = 0;
    teamsCapacity = 0;
}
void Championship::clearMatches() {
    delete[] matches;
    matches = 0;
    matchesCount = 0;
    matchesCapacity = 0;
}
void Championship::reconnectMatches() {
    for (unsigned i = 0; i < matchesCount; i++) {
        matches[i].reconnectTeams(findTeam(matches[i].getHomeTeamName()),
            findTeam(matches[i].getAwayTeamName()));
    }
}

void Championship::sortTeamPointers(Team** ordered, int count) const {
    for (int i = 0; i < count; i++) {
        ordered[i] = teams[i];
    }
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            bool swapNeeded = false;
            if (ordered[j]->getPoints() > ordered[i]->getPoints()) {
                swapNeeded = true;
            }
            else if (ordered[j]->getPoints() == ordered[i]->getPoints() &&
                ordered[j]->getGoalDifference() > ordered[i]->getGoalDifference()) {
                swapNeeded = true;
            }
            else if (ordered[j]->getPoints() == ordered[i]->getPoints() &&
                ordered[j]->getGoalDifference() == ordered[i]->getGoalDifference() &&
                ordered[j]->getGoalsFor() > ordered[i]->getGoalsFor()) {
                swapNeeded = true;
            }
            if (swapNeeded) {
                Team* temp = ordered[i];
                ordered[i] = ordered[j];
                ordered[j] = temp;
            }
        }
    }
}
void Championship::ensureTeamsCapacity(unsigned needed) {
    if (needed <= teamsCapacity) return;
    unsigned newCapacity = teamsCapacity == 0 ? 4 : teamsCapacity * 2;
    while (newCapacity < needed) newCapacity *= 2;

    Team** newTeams = new Team * [newCapacity] {};
    unsigned limit = teamsCapacity >= teamsCount ? teamsCount : teamsCapacity;
    for (unsigned i = 0; i < limit; i++) {
        newTeams[i] = teams[i];
    }
    delete[] teams;
    teams = newTeams;
    teamsCapacity = newCapacity;
}

void Championship::ensureMatchesCapacity(unsigned needed) {
    if (needed <= matchesCapacity) return;
    unsigned newCapacity = matchesCapacity == 0 ? 8 : matchesCapacity * 2;
    while (newCapacity < needed) newCapacity *= 2;

    Match* newMatches = new Match[newCapacity]{};
    unsigned limit = newCapacity >= matchesCount ? matchesCount : newCapacity;
    for (unsigned i = 0; i < limit; i++) {
        newMatches[i] = matches[i];
    }
    delete[] matches;
    matches = newMatches;
    matchesCapacity = newCapacity;
}

bool Championship::isRoundFinished(unsigned roundNumber) const {
    bool foundRound = false;
    for (unsigned i = 0; i < matchesCount; i++) {
        if (matches[i].getRoundNumber() == roundNumber) {
            foundRound = true;
            if (!matches[i].isPlayed()) {
                return false;
            }
        }
    }
    return foundRound;
}

bool Championship::allMatchesPlayed() const {
    if (matchesCount == 0) {
        return false;
    }
    for (unsigned i = 0; i < matchesCount; i++) {
        if (!matches[i].isPlayed()) {
            return false;
        }
    }
    return true;
}

void Championship::updateProgressAfterPlaying() {
    while (currentRound <= getTotalRounds() && isRoundFinished(currentRound)) {
        currentRound++;
    }
    if (!finished && currentRound > getTotalRounds() && allMatchesPlayed()) {
        finishSeason();
    }
}

void Championship::finishSeason() {
    if (finished) {
        return;
    }
    finished = true;
    regulateAllSalaries();
}

void Championship::addTeamPointer(Team* team) {
    ensureTeamsCapacity(teamsCount + 1);
    teams[teamsCount] = team;
    teamsCount++;
}

void Championship::addMatch(const Match& match) {
    ensureMatchesCapacity(matchesCount + 1);
    matches[matchesCount] = match;
    matchesCount++;
}