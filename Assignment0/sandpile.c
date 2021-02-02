#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void initializeSandPile(int sandPile[23][23]);
void outputSandPile(int sandPile[23][23]);
void topple(int sandPile[23][23], int y, int x);
int min(int y, int x);
int max(int y, int x);

int main(int argc, char *argv[])
{
    //variable
    int sandPile[23][23];
    int fps;

    //initialize sandPile
    initializeSandPile(sandPile);

    //if there is argument
    if (argc > 1)
    {

        //variable
        int i;
        int coorY;
        int coorX;
        int h;

        //if there is fps in argument
        if (!strcmp("--fps", argv[1]))
        {
            fps = atoi(argv[2]);
            for (i = 1; i < argc; i++)
            {
                if (i % 3 == 0)
                {
                    coorY = atoi(argv[i - 2 + 2]);
                    coorX = atoi(argv[i - 1 + 2]);
                    h = atoi(argv[i + 2]);
                    sandPile[coorY][coorX] = h;
                }

                //Check inputs if a number is greater than 8 but not count fps
                //If input is greater than 8, topple at the location coorY and coorX
                if (h > 8)
                {
                    topple(sandPile, coorY, coorX);
                }
            }
        }

        //if there is no fps in argument
        else
        {
            fps = 60; //default 60 fps
            for (i = 1; i < argc; i++)
            {
                if (i % 3 == 0)
                {
                    coorY = atoi(argv[i - 2]);
                    coorX = atoi(argv[i - 1]);
                    h = atoi(argv[i]);
                    sandPile[coorY][coorX] = h;
                }

                //Check inputs if a number is greater than 8 but not count fps
                //If input is greater than 8, topple at the location coorY and coorX
                if (h > 8)
                {
                    topple(sandPile, coorY, coorX);
                }
            }
        }
    }

    while (1)
    {
        usleep((float)1000000 / fps);
        sandPile[11][11] += 1;
        topple(sandPile, 11, 11);
        //output sandPile
        outputSandPile(sandPile);
    }
}

void initializeSandPile(int sandPile[23][23])
{
    int i, j;

    for (j = 0; j < 23; j++)
    {
        for (i = 0; i < 23; i++)
        {
            sandPile[j][i] = 0;
        }
    }
}

void outputSandPile(int sandPile[23][23])
{
    int i, j;

    for (j = 0; j < 23; j++)
    {
        for (i = 0; i < 23; i++)
        {
            if (sandPile[j][i] != -1)
            {
                printf("%3d", sandPile[j][i]);
            }
            else
            {
                printf("%3c", '#');
            }
        }
        printf("\n");
    }
    printf("\n");
}

void topple(int sandPile[23][23], int y, int x)
{
    if (sandPile[y][x] <= 8)
    {
        return;
    }

    sandPile[y][x] = sandPile[y][x] - 8;

    for (int k = max(y - 1, 0); k <= min(y + 1, 22); k++)
    {
        for (int l = max(x - 1, 0); l <= min(x + 1, 22); l++)
        {
            if (k != y || l != x)
            {
                if (sandPile[k][l] != -1)
                {
                    sandPile[k][l] = sandPile[k][l] + 1;
                    if (sandPile[k][l] > 8)
                    {
                        topple(sandPile, k, l);
                    }
                }
            }
        }
    }
}

int max(int y, int x)
{
    if (y > x)
    {
        return y;
    }
    else
        return x;
}

int min(int y, int x)
{
    if (y > x)
    {
        return x;
    }
    return y;
}