#pragma once
#include "Team.h"

/* Team type focused on defenders and midfielders.
 * It uses the base transfer checks without an additional scoring requirement.
 */

class DefensiveTeam : public Team {
public:
    DefensiveTeam() = default;
    DefensiveTeam(const char* name, const char* coachName, const char* stadiumName, double budget);
    const char* getType() const override;
    Team* clone() const override;
    bool isValidTeamForType() const override;
    bool canBuySpecificPlayer(const Player& player) const override;
};