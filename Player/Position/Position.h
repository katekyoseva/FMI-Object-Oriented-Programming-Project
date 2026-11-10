#pragma once

enum class Position
{
	Unknown_Position = -1,
	Goalkeeper,
	Defender,
	Midfielder,
	Winger,
	Forward,
	Count_Position
};

/* Converts saved or typed text to a Position value for loading and validation. */
Position positionFromString(const char* text);
/* Converts a Position value to text for printing and saving. */
const char* positionToString(Position position);