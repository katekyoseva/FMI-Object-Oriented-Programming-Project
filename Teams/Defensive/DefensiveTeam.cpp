#include "DefensiveTeam.h"

DefensiveTeam::DefensiveTeam(const char* name, const char* coachName, const char* stadiumName, double budget)
	:Team(name, coachName, stadiumName, budget)
{}

const char* DefensiveTeam::getType() const
{
	return "Defensive";
}

Team* DefensiveTeam::clone() const
{
	return new DefensiveTeam(*this);
}

bool DefensiveTeam::isValidTeamForType() const
{
	return countPosition(Position::Goalkeeper) == 2 &&
		countPosition(Position::Defender) >= 6 &&
		countPosition(Position::Midfielder) >= 6 &&
		countPosition(Position::Winger) >= 2 &&
		countPosition(Position::Forward) >= 2 &&
		playersCount <= MAX_TEAM_PLAYERS;
}

bool DefensiveTeam::canBuySpecificPlayer(const Player& player) const
{
	return player.getPosition() != Position::Unknown_Position;
}
