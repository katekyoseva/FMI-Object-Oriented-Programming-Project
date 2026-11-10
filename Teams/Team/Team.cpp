#include "Team.h"
#include "TextUtil.h"
#include <cstring>
#include <iostream>

#pragma warning(disable:4996)

Team::Team()
	:Team("Unknown name", "Unknown coach", "Unknown stadium", 1000)
{}
Team::Team(const char* name, const char* coach, const char* stadium, double budget)
	:budget(budget), playersCount(0), totalGames(0), wins(0), draws(0), losses(0),
	points(0), goalsFor(0), goalsAgainst(0)
{
	try {
		setName(name);
		setCoach(coach);
		setStadium(stadium);
	}
	catch (...)
	{
		delete[] name;
		delete[] coach;
		delete[] stadium;
		throw;
	}
}

Team::Team(const Team& other)
{
	try {
		setName(other.name);
		setCoach(other.coach);
		setStadium(other.stadium);
	}
	catch (...)
	{
		delete[] name;
		delete[] coach;
		delete[] stadium;
		throw;
	}
	copyFrom(other);
}

Team& Team::operator=(const Team& other)
{
	if (this != &other)
	{
		char* tempName = nullptr;
		char* tempCoach = nullptr;
		char* tempStadium = nullptr;
		try {
			tempName = copyDynamic(other.name);
			tempCoach = copyDynamic(other.coach);
			tempStadium = copyDynamic(other.stadium);
		}
		catch (...)
		{
			delete[] tempName;
			delete[] tempCoach;
			delete[] tempStadium;
			throw;
		}
		delete[] name;
		delete[] coach;
		delete[] stadium;

		name = tempName;
		coach = tempCoach;
		stadium = tempStadium;

		copyFrom(other);
	}
	return *this;
}

Team::~Team()
{
	free();
}

Player* Team::findPlayer(const char* playerName) {
	for (unsigned i = 0; i < playersCount; i++) {
		if (strcmp(players[i]->getName(), playerName) == 0) {
			return players[i];
		}
	}
	return nullptr;
}

const Player* Team::findPlayer(const char* playerName) const {
	for (unsigned i = 0; i < playersCount; i++) {
		if (strcmp(players[i]->getName(), playerName) == 0) {
			return players[i];
		}
	}
	return nullptr;
}

Player* Team::getPlayerAt(int index) {
	if (index < 0 || index >= playersCount) {
		return nullptr;
	}
	return players[index];
}

const Player* Team::getPlayerAt(int index) const {
	if (index < 0 || index >= playersCount) {
		return nullptr;
	}
	return players[index];
}

bool Team::addPlayer(const Player& player) {
	if (!isValidTeam(player.getPosition()))
		return false;
	if (hasPlayerName(player.getName()) || hasPlayerNumber(player.getNumber()))
		return false;

	Player* newPlayer = new Player(player);
	newPlayer->setTeam(this);
	players[playersCount] = newPlayer;
	playersCount++;
	return true;
}

bool Team::removePlayer(const char* playerName, Player& removed) {
	for (unsigned i = 0; i < playersCount; i++) {
		if (strcmp(players[i]->getName(), playerName) == 0) {
			removed = *players[i];
			removed.setTeam(nullptr);
			delete players[i];
			for (unsigned j = i; j < playersCount - 1; j++) {
				players[j] = players[j + 1];
			}
			playersCount--;
			players[playersCount] = nullptr;
			return true;
		}
	}
	return false;
}

bool Team::buyPlayer(const Player& player) {
	if (!canBuyPlayer(player))
		return false;
	if (!addPlayer(player))
		return false;

	budget -= player.getTransferFee();
	return true;
}

void Team::addTransferIncome(double amount)
{
	if (amount > 0) budget += amount;
}

bool Team::exchangeWith(Team& other, const char* ownPlayerName, const char* otherPlayerName) {
	if (!canExchangeWith(other, ownPlayerName, otherPlayerName))
		return false;

	Player ownRemoved;
	Player otherRemoved;
	removePlayer(ownPlayerName, ownRemoved);
	other.removePlayer(otherPlayerName, otherRemoved);

	bool ok = addPlayer(otherRemoved) && other.addPlayer(ownRemoved) && isValidTeamForType() && other.isValidTeamForType();

	if (!ok) {
		Player rollback;
		removePlayer(otherRemoved.getName(), rollback);
		other.removePlayer(ownRemoved.getName(), rollback);
		addPlayer(ownRemoved);
		other.addPlayer(otherRemoved);
		return false;
	}
	return true;
}

double Team::allSalaries() const
{
	double sum = 0;
	for (unsigned i = 0; i < playersCount; i++)
	{
		sum += players[i]->getSalary();
	}
	return sum;
}

bool Team::canBuyPlayer(const Player& player) const {
	if (allSalaries() + player.getSalary() + player.getTransferFee() > budget)
		return false;
	if (budget < player.getTransferFee())
		return false;
	if (!isValidTeam(player.getPosition()))
		return false;
	if (hasPlayerName(player.getName()) || hasPlayerNumber(player.getNumber()))
		return false;

	return canBuySpecificPlayer(player);
}

bool Team::canExchangeWith(Team& other, const char* ownPlayerName, const char* otherPlayerName) const {
	const Player* own = findPlayer(ownPlayerName);
	const Player* others = other.findPlayer(otherPlayerName);
	if (own == nullptr || others == nullptr)
		return false;
	if (findPlayer(others->getName()) != nullptr || other.findPlayer(own->getName()) != nullptr)
		return false;

	return true;
}

void Team::updateAfterMatch(int scored, int conceded) {
	goalsFor += scored;
	goalsAgainst += conceded;
	if (scored > conceded) {
		wins++;
		points += 3;
	}
	else if (scored == conceded) {
		draws++;
		points += 1;
	}
	else {
		losses++;
	}
	totalGames++;
}

void Team::startNewSeason() {
	clearStats();
	for (unsigned i = 0; i < playersCount; i++) {
		players[i]->resetSeasonStats();
	}
}

void Team::regulateSalaries() {
	for (unsigned i = 0; i < playersCount; i++) {
		players[i]->regulateSalary(totalGames);
	}
}

void Team::print() const {
	std::cout << name << " (" << getType() << "), coach: " << coach
		<< ", stadium: " << stadium << ", budget: " << budget << '\n';
	std::cout << "Total games: " << totalGames << '\n';
	std::cout << "\tW/D/L: " << wins << "/" << draws << "/" << losses
		<< ", points: " << points << ", goals: " << goalsFor
		<< ":" << goalsAgainst << "\n";
	std::cout << "Players: \n";
	printPlayers();
}

void Team::printForStandings() const {
	std::cout << name << " (" << getType() << "), coach: " << coach
		<< ", stadium: " << stadium << ", budget: " << budget << '\n';
	std::cout << "Total games: " << totalGames << '\n';
	std::cout << "\tW/D/L: " << wins << "/" << draws << "/" << losses
		<< ", points: " << points << ", goals: " << goalsFor
		<< ":" << goalsAgainst << "\n";
}

void Team::printPlayers() const {
	for (unsigned i = 0; i < playersCount; i++) {
		players[i]->print();
	}
}

void Team::saveBase(std::ofstream& out) const {
	out << getType() << " " << name << " " << coach << " "
		<< stadium << " " << budget << " " << wins << " " << draws
		<< " " << losses << " " << points << " " << goalsFor << " "
		<< goalsAgainst << " " << playersCount << "\n";
	for (unsigned i = 0; i < playersCount; i++) {
		players[i]->save(out);
	}
}

void Team::loadBase(std::ifstream& in) {
	char nameBuffer[128]{};
	char coachBuffer[128]{};
	char stadiumBuffer[128]{};
	double newBudget;
	int newWins;
	int newDraws;
	int newLosses;
	int newPoints;
	int newGoalsFor;
	int newGoalsAgainst;
	int newPlayersCount;

	in >> nameBuffer >> coachBuffer >> stadiumBuffer >> newBudget >> newWins
		>> newDraws >> newLosses >> newPoints >> newGoalsFor
		>> newGoalsAgainst >> newPlayersCount;

	if (newPlayersCount < 0 || newPlayersCount > MAX_TEAM_PLAYERS)
		throw std::runtime_error("Invalid players count in file.");

	try {
		setName(nameBuffer);
		setCoach(coachBuffer);
		setStadium(stadiumBuffer);

		for (int i = 0; i < newPlayersCount; i++) {
			Player player;
			player.load(in);
			if (!addPlayer(player)) throw std::runtime_error("Invalid player data in file.");
		}
	}
	catch (...) {
		free();
		throw;
	}
	budget = newBudget;
	wins = newWins;
	draws = newDraws;
	losses = newLosses;
	points = newPoints;
	goalsFor = newGoalsFor;
	goalsAgainst = newGoalsAgainst;
	totalGames = wins + draws + losses;
}

int Team::countPosition(Position position) const
{
	int count = 0;
	for (unsigned i = 0; i < playersCount; i++) {
		if (players[i]->getPosition() == position) {
			count++;
		}
	}
	return count;
}

bool Team::hasPlayerName(const char* playerName) const {
	return findPlayer(playerName) != nullptr;
}

bool Team::hasPlayerNumber(int number) const {
	for (unsigned i = 0; i < playersCount; i++) {
		if (players[i]->getNumber() == number) {
			return true;
		}
	}
	return false;
}

bool Team::isValidTeam(Position position) const {
	if (playersCount >= MAX_TEAM_PLAYERS) {
		return false;
	}
	if (position == Position::Goalkeeper && countPosition(Position::Goalkeeper) >= 2) {
		return false;
	}
	return true;
}

void Team::setName(const char* newName)
{
	char* temp = copyDynamic(newName);
	delete[] name;
	name = temp;
}

void Team::setStadium(const char* newStadium)
{
	char* temp = copyDynamic(newStadium);
	delete[] stadium;
	stadium = temp;
}

void Team::setCoach(const char* newCoach)
{
	char* temp = copyDynamic(newCoach);
	delete[] coach;
	coach = temp;
}

void Team::clearStats()
{
	totalGames = 0;
	wins = 0;
	draws = 0;
	losses = 0;
	points = 0;
	goalsFor = 0;
	goalsAgainst = 0;
}

void Team::copyFrom(const Team& other)
{
	for (unsigned i = 0; i < other.playersCount; i++) {
		players[i] = new Player(*other.players[i]);
		players[i]->setTeam(this);
	}
	budget = other.budget;
	playersCount = other.playersCount;
	totalGames = other.totalGames;
	wins = other.wins;
	draws = other.draws;
	losses = other.losses;
	points = other.points;
	goalsFor = other.goalsFor;
	goalsAgainst = other.goalsAgainst;
}

void Team::free()
{
	delete[] name;
	delete[] coach;
	delete[] stadium;
	for (unsigned i = 0; i < playersCount; i++) {
		delete players[i];
		players[i] = 0;
	}
}
