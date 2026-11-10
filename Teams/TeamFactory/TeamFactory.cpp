#include "TeamFactory.h"
#include "AttackingTeam.h"
#include "DefensiveTeam.h"
#include "BalancedTeam.h"

Team* createTeamByType(const char* type) {
    if (strcmp(type, "Attacking") == 0) {
        return new AttackingTeam();
    }
    if (strcmp(type, "Defensive") == 0) {
        return new DefensiveTeam();
    }
    if (strcmp(type, "Balanced") == 0) {
        return new BalancedTeam();
    }
    return nullptr;
}