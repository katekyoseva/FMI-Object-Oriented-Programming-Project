#pragma once
#include "Team.h"
/* Creates the correct derived Team object from a saved type name. */
Team* createTeamByType(const char* type);