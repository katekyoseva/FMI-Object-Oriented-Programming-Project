#include "Position.h"
#include <cstring>

Position positionFromString(const char* text) {
    if (strcmp(text, "Goalkeeper") == 0) {
        return Position::Goalkeeper;
    }
    if (strcmp(text, "Defender") == 0) {
        return Position::Defender;
    }
    if (strcmp(text, "Midfielder") == 0) {
        return Position::Midfielder;
    }
    if (strcmp(text, "Winger") == 0) {
        return Position::Winger;
    }
    if (strcmp(text, "Forward") == 0) {
        return Position::Forward;
    }
    return Position::Unknown_Position;
}

const char* positionToString(Position position) {
    switch (position)
    {
    case Position::Goalkeeper: return "Goalkeeper";
    case Position::Defender: return "Defender";
    case Position::Midfielder: return "Midfielder";
    case Position::Winger: return "Winger";
    case Position::Forward: return "Forward";
    default: return "Unknown";
    }
}

