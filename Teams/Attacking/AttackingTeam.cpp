#include "AttackingTeam.h"

AttackingTeam::AttackingTeam(const char* name, const char* coachName, const char* stadiumName, double budget)
	:Team(name, coachName, stadiumName, budget)
{}

const char* AttackingTeam::getType() const
{
	return "Attacking";
}

Team* AttackingTeam::clone() const
{
	return new AttackingTeam(*this);
}

bool AttackingTeam::isValidTeamForType() const
{
	return countPosition(Position::Goalkeeper) == 2 &&
		countPosition(Position::Forward) >= 8 &&
		countPosition(Position::Defender) >= 2 &&
		countPosition(Position::Midfielder) >= 2 &&
		countPosition(Position::Winger) >= 2 &&
		playersCount <= MAX_TEAM_PLAYERS;
}

bool AttackingTeam::canBuySpecificPlayer(const Player& player) const
{
	if (player.getPosition() == Position::Forward && player.getGoalsScored() < 6)
		return false;
	return true;
}
