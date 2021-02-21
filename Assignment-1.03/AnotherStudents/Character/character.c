#include <stdlib.h>

#include "character.h"
#include "../Helpers/helpers.h"

// See character.h
Character_T *new_character(int y, int x, char symbol, char *color, bool player) {
    Character_T *c = safe_malloc(sizeof(Character_T));
    c->y = y;
    c->x = x;
    c->symbol = symbol;
    c->color = color;
    c->player = player;
    return c;
}

// See character.h
void cleanup_character(Character_T *c) {
    free(c);
}
