#include "Player.h"
#include "TextUtil.h"
#include <fstream>
#include <iostream>

#pragma warning (disable:4996)

Player::Player()
	:Player("Unknown name", 1, Position::Unknown_Position, 0.0, 0.0)
{}

Player::Player(const char* name, unsigned number, Position position, double salary, 
	double transferFee, unsigned matchesPlayed, unsigned goalsScored)
	: number(number), position(position), salary(salary), transferFee(transferFee),
	matchesPlayed(matchesPlayed), goalsScored(goalsScored)
{
	setName(name);
}

Player::Player(const Player& other)
	:Player(other.name, other.number, other.position,
		other.salary, other.transferFee)
{
	matchesPlayed = other.matchesPlayed;
	goalsScored = other.goalsScored;
}

Player& Player::operator=(const Player& other)
{
	if (this != &other) {
		char* tempName = copyDynamic(other.name);
		delete[] name;
		name = tempName;
		number = other.number;
		position = other.position;
		salary = other.salary;
		transferFee = other.transferFee;
		matchesPlayed = other.matchesPlayed;
		goalsScored = other.goalsScored;
	}
	return *this;
}

Player::~Player()
{
	delete[] name;
}

void Player::setName(const char* newName)
{
	char* temp = copyDynamic(newName);
	delete[] name;
	name = temp;
}

void Player::setTeam(Team* team)
{
	this->team = team;
}

void Player::setNumber(int number)
{
	this->number = number;
}

void Player::addMatch()
{
	matchesPlayed++;
}

void Player::addGoal()
{
	goalsScored++;
}

void Player::resetSeasonStats()
{
	matchesPlayed = 0;
	goalsScored = 0;
}

void Player::regulateSalary(unsigned teamMatches)
{
	if (teamMatches > 0 && matchesPlayed * 2 > teamMatches) {
		salary *= 1.20;
	}
	if (matchesPlayed < 7) {
		salary *= 0.90;
	}
	if (goalsScored > 5) {
		salary *= 1.10;
	}
	if (goalsScored > 10) {
		salary *= 1.20;
	}
	if (goalsScored < 5 && position == Position::Forward) {
		salary *= 0.85;
	}
}

void Player::print() const
{
	std::cout << "#" << number << " " << name << " - " << positionToString(position) << '\n';
	std::cout << "\tSalary: " << salary << '\n';
	std::cout << "\tFee: " << transferFee << '\n';
	std::cout << "\tMatches played this season: " << matchesPlayed << '\n';
	std::cout << "\tGoals scored this season: " << goalsScored << '\n';
}

void Player::save(std::ofstream& out) const
{
	out << name << " " << number << " " << positionToString(position) << " "
		<< salary << " " << transferFee << " " << matchesPlayed << " "
		<< goalsScored << "\n";
}

void Player::load(std::ifstream& in)
{
	char nameBuffer[128]{};
	char positionText[32]{};
	int newNumber;
	double newSalary;
	double newTransferFee;
	int newMatchesPlayed;
	int newGoalsScored;

	in >> nameBuffer >> newNumber >> positionText >> newSalary >> newTransferFee
		>> newMatchesPlayed >> newGoalsScored;

	Position newPosition = positionFromString(positionText);
	setName(nameBuffer);
	number = newNumber;
	position = newPosition;
	salary = newSalary;
	transferFee = newTransferFee;
	matchesPlayed = newMatchesPlayed;
	goalsScored = newGoalsScored;
}