#pragma once
#include "Team.h"

/* Team type focused on forwards.
 * It requires many forwards and buys a new forward only if he already has enough goals.
 */

class AttackingTeam : public Team {
public:
    AttackingTeam() = default;
    AttackingTeam(const char* name, const char* coachName, const char* stadiumName, double budget);
    const char* getType() const override;
    Team* clone() const override;
    bool isValidTeamForType() const override;
    bool canBuySpecificPlayer(const Player& player) const;
};
