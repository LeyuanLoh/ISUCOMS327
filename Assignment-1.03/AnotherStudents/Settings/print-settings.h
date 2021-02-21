#ifndef ROGUE_PRINT_SETTINGS_H
#define ROGUE_PRINT_SETTINGS_H

// This file sets compile time setting for how monsters and the dungeon is printed out.

// Settings to control how characters are printed
#define PC_COLOR BACKGROUND_BLUE
#define PC_SYMBOL '@'

// Settings to control how the dungeon is printed
#define ROCK_COLOR BACKGROUND_WHITE
#define ROCK_CHAR ' '

#define CORRIDOR_COLOR BACKGROUND_GREY
#define CORRIDOR_CHAR ' '

#define ROOM_COLOR BACKGROUND_BLACK
#define ROOM_CHAR ' '

#define STAIR_UP_COLOR ROOM_COLOR
#define STAIR_UP_CHAR '<'

#define STAIR_DOWN_COLOR ROOM_COLOR
#define STAIR_DOWN_CHAR '>'

#endif //ROGUE_PRINT_SETTINGS_H
