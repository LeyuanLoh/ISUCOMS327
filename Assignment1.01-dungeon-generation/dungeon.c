#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define GRIDY (24)
#define GRIDX (80)
static char grid[GRIDY][GRIDX];

void output();
void initialize();
void createNewRooms();
void placeRooms(int **arraySize, int numRooms);
int canPlace(int insertY, int insertX, int sizeY, int sizeX);

int main(int argc, char *argv[])
{
    initialize();
    createNewRooms();
    output();
}

//Creating rooms with unique size. 
void createNewRooms()
{
    const int MAX_ROOM_NUMBER = 12;
    const int MIN_ROOM_NUMBER = 6;
    //Seeder for random using current time
    srand(time(NULL));

    //Math stuff
    const int numRoom = (rand() % (MAX_ROOM_NUMBER - MIN_ROOM_NUMBER + 1)) + MIN_ROOM_NUMBER;

    const int MAX_ROOMY_SIZE = 24 / (numRoom - 3);
    const int MAX_ROOMX_SIZE = 80 / (numRoom);

    const int MIN_ROOMY_SIZE = 3;
    const int MIN_ROOMX_SIZE = 4;

    //2D array. Storing information of the size of each random rooms.
    //The reason I do this instead of array[][] because you can't do array[var][var]. You only can do array[2][2]
    int **arraySize = (int **)malloc(numRoom * sizeof(int *));
    for (int i = 0; i < numRoom; i++)
    {
        arraySize[i] = (int *)malloc(2 * sizeof(int));
        arraySize[i][0] = (rand() % (MAX_ROOMY_SIZE - MIN_ROOMY_SIZE + 1)) + MIN_ROOMY_SIZE;
        arraySize[i][1] = (rand() % (MAX_ROOMX_SIZE - MIN_ROOMX_SIZE + 1)) + MIN_ROOMX_SIZE;
    }

    placeRooms(arraySize, numRoom);
}

//Place the rooms. Generate a random point and check surrounding already has a room or not. If not, then place it.
void placeRooms(int **arraySize, int numRooms)
{
    srand(time(NULL));
    int availableCell = (80 * 24);
    int i = 0;
    while (i < numRooms)
    {
        int sizeY = arraySize[i][0];
        int sizeX = arraySize[i][1];

        //The point where we start to insert the top left of the room

        int insertY = (rand() % (24 - sizeY - 0 + 1)) + 0;
        int insertX = (rand() % (80 - sizeX - 0 + 1)) + 0;

        //We meed minus one to check whether another room is nearby
        if (insertY > 0)
        {
            insertY--;
        }
        if (insertX > 0)
        {
            insertX--;
        }
        //Now check if we can place or not
        int placeFoo = canPlace(insertY, insertX, sizeY, sizeX);
        if (placeFoo == 0)
        {
            for (int j = 1; j <= sizeY; j++)
            {
                for (int k = 1; k <= sizeX; k++)
                {
                    // if (sizeX > 4 && (k == 1 || k == sizeX))
                    // {
                    //     int noise = (rand() % (1 - 0 + 1));
                    //     if (noise == 0)
                    //     {
                    //         grid[insertY + j][insertX + k] = '.';
                    //     }
                    // }
                    // else if (sizeY > 3 && (j == 1 || j == sizeY))
                    // {
                    //     int noise = (rand() % (1 - 0 + 1));
                    //     if (noise == 0)
                    //     {
                    //         grid[insertY + j][insertX + k] = '.';
                    //     }
                    // }
                    // else
                    // {
                    grid[insertY + j][insertX + k] = '.';
                    // }
                }
            }
            i++;
        }
    }
}

//0 is true and 1 is false
//Check if can place the room or not.
int canPlace(int insertY, int insertX, int sizeY, int sizeX)
{
    //Checking the surrounding. 
    for (int i = 0; i <= sizeY + 1; i++)
    {
        for (int j = 0; j <= sizeX + 1; j++)
        {
            if (j >= 79)
            {
                continue;
            }
            if (grid[insertY + i][insertX + j] == '.')
            {
                return 1;
            }
        }
        if (i >= 23)
        {
            continue;
        }
    }
    return 0;
}

void output()
{
    printf("----------------------------------------------------------------------------------");
    for (int i = 0; i < GRIDY; i++)
    {
        printf("|");
        for (int j = 0; j < GRIDX; j++)
        {
            printf("%c", grid[i][j]);
        }
        printf("|");
        printf("\n");
    }
    printf("----------------------------------------------------------------------------------");
    printf("\n");
}

void initialize()
{
    for (int i = 0; i < GRIDY; i++)
    {
        for (int j = 0; j < GRIDX; j++)
        {
            grid[i][j] = ' ';
        }
    }
}