#include <limits.h>

#include "dijkstra.h"
#include "../Settings/dungeon-parameters.h"
#include "../Helpers/helpers.h"

// Cost function for the below function. Use parameters in setting.h if you want to tune this.
static int corridor_cost(const Dungeon_T *d, int y, int x) {
    // Only needs to be initialized once... its a compile time constant.
    static int difference = (MAX_ROCK_HARDNESS - MIN_ROCK_HARDNESS) / NUM_HARDNESS_LEVELS;

    // Infinite cost if we can't go here.
    if (d->MAP(y, x).hardness == IMMUTABLE_ROCK_HARDNESS) {
        return INT_MAX;
    }

    switch (d->MAP(y, x).type) {
        case ROCK:
            // Checks if we are using hardness... if we are it divides the hardness by a difference to split it into
            // multiple ranges. If there is remainder, we just add it onto the top.
            return USE_HARDNESS_FOR_CORRIDORS ? d->MAP(y, x).hardness / difference > NUM_HARDNESS_LEVELS ?
                                                NUM_HARDNESS_LEVELS + 1 :
                                                d->MAP(y, x).hardness / difference + 1 : 1 + ROCK_WEIGHT;
        case ROOM:
            return 1 + ROOM_WEIGHT;
        case CORRIDOR:
            return 1 + CORRIDOR_WEIGHT;
        default:
            return INT_MAX;
    }
}

// Recursive function that does the heavy lifting for generating the corridor cost map. It keeps track of what
// neighbor called it, not entering into that cell to avoid an infinite recursion loop and blowing the stack. It then
// adjusts the cost if it found a cheaper load_path, and will recurse into its neighbor, otherwise it skips those cells,
// knowing that its load_path is a dead end.
static void
corridor_cost_recurse(const Dungeon_T *d, int *cost, int y, int x, Direction_T caller) { // NOLINT(misc-no-recursion)
    int neighbor_cost;
    bool up, down, left, right;

    // Store if we need to recurse left or right. We want to calculate costs before recursing into those cells.
    up = down = left = right = false;

    // Check up if it wasn't the caller
    if (caller != UP) {
        if (y + 1 < d->height - 1) {
            neighbor_cost = corridor_cost(d, y + 1, x);
            if (COST(y + 1, x) > COST(y, x) + neighbor_cost) {
                COST(y + 1, x) = COST(y, x) + neighbor_cost;
                down = true;
            }
        }
    }

    // Check down if it wasn't the caller
    if (caller != DOWN) {
        if (y - 1 > 0) {
            neighbor_cost = corridor_cost(d, y - 1, x);
            if (COST(y - 1, x) > COST(y, x) + neighbor_cost) {
                COST(y - 1, x) = COST(y, x) + neighbor_cost;
                up = true;
            }
        }
    }

    // Check left if it wasn't the caller
    if (caller != LEFT) {
        if (x + 1 < d->width - 1) {
            neighbor_cost = corridor_cost(d, y, x + 1);
            if (COST(y, x + 1) > COST(y, x) + neighbor_cost) {
                COST(y, x + 1) = COST(y, x) + neighbor_cost;
                right = true;
            }
        }
    }

    // Check right if it wasn't the caller
    if (caller != RIGHT) {
        if (x - 1 > 0) {
            neighbor_cost = corridor_cost(d, y, x - 1);
            if (COST(y, x - 1) > COST(y, x) + neighbor_cost) {
                COST(y, x - 1) = COST(y, x) + neighbor_cost;
                left = true;
            }
        }
    }

    // Begin entering into neighboring cells.
    if (up) {
        corridor_cost_recurse(d, cost, y - 1, x, UP);
    }
    if (down) {
        corridor_cost_recurse(d, cost, y + 1, x, DOWN);
    }
    if (left) {
        corridor_cost_recurse(d, cost, y, x - 1, LEFT);
    }
    if (right) {
        corridor_cost_recurse(d, cost, y, x + 1, RIGHT);
    }
}

// First it initializes the cost map, setting all values to INT_MAX (infinite effectively). Then it sets the source to
// be zero. It then initializes the recursive function, by figuring out if it can go up, down, left, and right from
// itself, based on if the cells are still in range. To do so, it sets the costs of all the cells it can visit next,
// then recurses into them.
int *generate_dijkstra_corridor_map(const Dungeon_T *d, int y, int x) {
    int *cost;
    int i, j;
    bool up, down, left, right;

    // Initialize our cost map
    cost = safe_malloc(d->height * d->width * sizeof(int));
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            COST(i, j) = INT_MAX;
        }
    }

    // Set source to be zero
    COST(y, x) = 0;

    // Set directions to be true if they are valid cells to enter
    up = y - 1 > 0;
    down = y + 1 < d->height - 1;
    left = x - 1 > 0;
    right = x + 1 < d->width - 1;

    // Set cost for neighbor cells from source
    if (up) {
        COST(y - 1, x) = corridor_cost(d, y - 1, x);
    }
    if (down) {
        COST(y + 1, x) = corridor_cost(d, y + 1, x);
    }
    if (left) {
        COST(y, x - 1) = corridor_cost(d, y, x - 1);
    }
    if (right) {
        COST(y, x + 1) = corridor_cost(d, y, x + 1);
    }

    // Start the recursion chain into neighbor cells
    if (up) {
        corridor_cost_recurse(d, cost, y - 1, x, UP);
    }
    if (down) {
        corridor_cost_recurse(d, cost, y + 1, x, DOWN);
    }
    if (left) {
        corridor_cost_recurse(d, cost, y, x - 1, LEFT);
    }
    if (right) {
        corridor_cost_recurse(d, cost, y, x + 1, RIGHT);
    }

    return cost;
}
