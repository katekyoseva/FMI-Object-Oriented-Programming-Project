#pragma once

/* Allocates and returns an owned copy of str. The caller must delete[] it. */
char* copyDynamic(const char* str);
/* Appends text into a fixed char buffer without overflowing it. */
void appendText(char* buffer, int bufferSize, const char* text);
/* Builds deterministic generated names such as prefix_roleNumber. 
 * Used for creating the demonstration championship.
 */
void makeIndexedName(char* buffer, int bufferSize, const char* prefix, const char* role, int index);
