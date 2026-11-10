#include "BalancedTeam.h"

BalancedTeam::BalancedTeam(const char* name, const char* coachName, const char* stadiumName, double budget)
	:Team(name, coachName, stadiumName, budget)
{}

const char* BalancedTeam::getType() const
{
	return "Balanced";
}

Team* BalancedTeam::clone() const
{
	return new BalancedTeam(*this);
}

bool BalancedTeam::isValidTeamForType() const
{
	return countPosition(Position::Goalkeeper) == 2 &&
        countPosition(Position::Defender) >= 4 &&
        countPosition(Position::Midfielder) >= 4 &&
        countPosition(Position::Winger) >= 4 &&
        countPosition(Position::Forward) >= 4 &&
        playersCount <= MAX_TEAM_PLAYERS;
}

bool BalancedTeam::canBuySpecificPlayer(const Player& player) const {
    return player.getPosition() != Position::Unknown_Position;
}
