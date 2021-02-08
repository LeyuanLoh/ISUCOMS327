#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define GRIDY (21)
#define GRIDX (80)
static char grid[GRIDY][GRIDX];

void output();
void initialize();
int** createNewRooms();
int placeRooms(int **arraySize, int numRooms, int **roomMidPoint);
int canPlace(int insertY, int insertX, int sizeY, int sizeX);

int main(int argc, char *argv[])
{
    int **rooms; //2D array of room with 0 is y midpoitn and 1 is x midpoint
    initialize();
    rooms = createNewRooms();
    output();
}

//Creating rooms with unique size.
int** createNewRooms()
{
    const int MAX_ROOM_NUMBER = 12;
    const int MIN_ROOM_NUMBER = 6;
    //Seeder for random using current time
    srand(time(NULL));
    int success;

    //Math stuff
    do{
    const int numRoom = (rand() % (MAX_ROOM_NUMBER - MIN_ROOM_NUMBER + 1)) + MIN_ROOM_NUMBER;

    const int MAX_ROOMY_SIZE = 17 /numRoom +5;
    const int MAX_ROOMX_SIZE = 76 /numRoom;

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


    int **roomMidPoint = (int **)malloc(numRoom * sizeof(int *));

    success = placeRooms(arraySize, numRoom,roomMidPoint);

    if(success==0){
        return roomMidPoint;
    }

    }while(success==1);

    
}

//Place the rooms. Generate a random point and check surrounding already has a room or not. If not, then place it.
int placeRooms(int **arraySize, int numRooms, int**roomMidPoint)
{
    srand(time(NULL));
    int availableCell = (76 * 17);
    int i = 0;
    int trial =0;
    while (i < numRooms)
    {
        int sizeY = arraySize[i][0];
        int sizeX = arraySize[i][1];

        //The point where we start to insert the top left of the room

        int insertY = (rand() % (17 - 2 + 1)) + 2;
        int insertX = (rand() % (76 - 2 + 1)) + 2;


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
                    //0 for y midpoint and 1 for x midpoint
                    roomMidPoint[i] = (int *)malloc(2 * sizeof(int));
                    roomMidPoint[i][0] = insertY + (sizeY/2);
                    roomMidPoint[i][1] = insertX + (sizeX/2);
                    // }
                }
            }
            i++;
            trial =0;
        }
        trial++;
        if(trial>= 2000){
            return 1;
        }
    }
    return 0;
}

//0 is true and 1 is false
//Check if can place the room or not.
int canPlace(int insertY, int insertX, int sizeY, int sizeX)
{
    int posY,posX,surroundY,surroundX;

    if (insertY > 2)
    {
         posY = insertY-1;
         surroundY = sizeY +1;
    }
    else{
        surroundY = sizeY;
    }
    if (insertX > 2)
    {
        posX = insertX-1;
        surroundX = sizeX+1;
    }
    else{
        surroundX = sizeX;
    }
    //Checking the surrounding.

    int i,j;
    for (i = 0; i <= surroundY; i++){
        for (j = 0; j <= surroundX; j++){
            if (insertX+j >= 79) { return 1;}
            if (grid[insertY + i][insertX + j] == '.') {return 1;}
        }
        if (insertY+i >= 20){return 1;}
    }
    return 0;

    
}

void output()
{
    int y,x;

    for(y=0;y<GRIDY;y++){
        for(x=0;x<GRIDX;x++){
            printf("%c",grid[y][x]);
        }
        printf("\n");
    }
}

void initialize()
{
    int y,x;

    for(y=0;y<GRIDY;y++){
        for(x=0;x<GRIDX;x++){
            if (y==0||y == 20)
                grid[y][x] = '-';
            else if(x==0||x==79)
                grid[y][x] = '|';
            else
                grid[y][x] =' ';
        }
    }
}