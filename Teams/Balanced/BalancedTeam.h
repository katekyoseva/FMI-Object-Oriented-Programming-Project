#pragma once
#include "Team.h"

/* Team type with an even distribution between defenders, midfielders, wingers,
 * and forwards. It has no extra buying rule beyond the base checks.
 */

class BalancedTeam : public Team {
public:
    BalancedTeam() = default;
    BalancedTeam(const char* name, const char* coachName, const char* stadiumName, double budget);
    const char* getType() const override;
    Team* clone() const override;
    bool isValidTeamForType() const override;
    bool canBuySpecificPlayer(const Player& player) const override;
};