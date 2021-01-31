#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void topple(int grid[23][23], int y, int x);
int min(int y, int x);
int max(int y, int x);

void topple(int grid[23][23], int y, int x)
{
    if (grid[y][x] <= 8)
    {
        return;
    }

    grid[y][x] = grid[y][x] - 8;

    for (int k = max(y - 1, 0); k <= min(y + 1, 22); k++)
    {
        for (int l = max(x - 1, 0); l <= min(x + 1, 22); l++)
        {
            if (k != y || l != x)
            {
                if (grid[k][l] != -1)
                {
                    grid[k][l] = grid[k][l] + 1;
                    if (grid[k][l] > 8)
                    {
                        topple(grid, k, l);
                    }
                }
            }
        }

        // for (int l = max(x - 1, 0); l <= min(x + 1, 22); l++)
        // {

        // }
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