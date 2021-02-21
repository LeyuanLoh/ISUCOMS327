#ifndef ROGUE_CHARACTER_H
#define ROGUE_CHARACTER_H

#include <stdbool.h>

// Struct for a character. Will be used for player and monster.
typedef struct Character_S {
    int y, x;
    char symbol;
    char *color;
    bool player;
} Character_T;

// Returns a pointer to a new character. May be made static later. Simply initializes the above. Make sure to update
// this function when extending the above struct
Character_T *new_character(int y, int x, char symbol, char *color, bool player);

// Cleans up a character. Currently just wraps free(), but exists for extending later.
void cleanup_character(Character_T *c);

#endif //ROGUE_CHARACTER_H
