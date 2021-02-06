#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
    Requirements:
    -[] Printable dungeon is 80 x 21
    -[] 6 Rooms per dungeon
    -[] Minimum room is 4 x 3
    -[] At least 1 cell of spacing between room
    -[x] Drawn with .
*/

const char roomCell = '.';

void generateRooms() {
    srand((unsigned) time(NULL));

    int COLS = 80;
    int ROWS = 21;
    int minRooms = 6;
    int minRoomCOLS = 4;
    int minRoomROWS = 3;
    int MAXROOMS = 12;

    
}

int generateRandom(int max, int min) {
  return (rand() % (max - min)) + min;
}