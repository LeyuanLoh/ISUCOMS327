#include <stdio.h>
#include <stdlib.h>

#include "dungeon.h"
#include "dijkstra.h"
#include "../Helpers/helpers.h"
#include "../Helpers/tree.h"
#include "../Settings/dungeon-parameters.h"
#include "../Settings/print-settings.h"
#include "../Settings/exit-codes.h"

// Private struct to store partitions. Same as rooms (for now), but separating them makes it easier to extend Room_T
// in the future, and keeps code more readable.
typedef struct Partition_ {
    int y, x, height, width;
} Partition_T;

// Returns a pointer to a new partition. Disparate "initializer" functions are preferred but they can cause overhead
// in cases where heap space is already allocated to them. In this case, partitions are stored in a tree, so a pointer
// works great (see tree.h for details).
static Partition_T *new_partition(int y, int x, int height, int width) {
    Partition_T *partition = safe_malloc(sizeof(Partition_T));
    partition->y = y;
    partition->x = x;
    partition->height = height;
    partition->width = width;
    return partition;
}

// Determines if room parameters are valid in the given dungeon. Since rooms are contained in a partition, only the
// exterior walls need to be checked if they are within one tile of any other room.
static bool is_valid_room(const Dungeon_T *d, int y, int x, int height, int width) {
    int i;

    // Check left and right
    for (i = y - 1; i < y + height + 1; i++) {
        if (d->MAP(i, x - 1).type != ROCK || d->MAP(i, x + width).type != ROCK) {
            return false;
        }
    }

    // Check top and bottom
    for (i = x; i < x + width; i++) {
        if (d->MAP(y - 1, i).type != ROCK || d->MAP(y + height, i).type != ROCK) {
            return false;
        }
    }

    return true;
}

// Heavy lifting function that paints corridors. Given the source pair and destination pair, it will call the
// dijkstra functions to create a new cost map from the source. It then starts at the destination and "rolls downhill".
// By starting at the destination and always visiting the cheapest neighbor node, we can generate a shortest load_path.
// There may be multiple, but it will always go in order of UP, DOWN, LEFT, and then RIGHT, if costs are equal.
// As it builds the load_path, it will paint any rock tiles to be corridors and set the hardness of those to be equal to
// the default hardness in setting.h
static void paint_corridor(Dungeon_T *d, int src_y, int src_x, int dst_y, int dst_x) {
    int *cost;
    int i, j, neighbor_cost;
    Direction_T direction;

    // Generate our cost map. See dijkstra.c
    cost = generate_dijkstra_corridor_map(d, src_y, src_x);

    // Starting at the destination, roll downhill to the cheapest node
    i = dst_y;
    j = dst_x;

    while (i != src_y || j != src_x) {
        // Start with up
        direction = UP;
        neighbor_cost = COST(i - 1, j);

        if (neighbor_cost > COST(i + 1, j)) {
            neighbor_cost = COST(i + 1, j);
            direction = DOWN;
        }

        if (neighbor_cost > COST(i, j - 1)) {
            neighbor_cost = COST(i, j - 1);
            direction = LEFT;
        }

        // We don't care about setting cost, since this is the last neighbor to check
        if (neighbor_cost > COST(i, j + 1)) {
            direction = RIGHT;
        }

        // Travel to the chosen cell
        switch (direction) { // NOLINT(hicpp-multiway-paths-covered)
            case UP:
                i -= 1;
                break;
            case DOWN:
                i += 1;
                break;
            case LEFT:
                j -= 1;
                break;
            case RIGHT:
                j += 1;
                break;
        }

        // Actually paint the corridor into the dungeon map
        if (d->MAP(i, j).type == ROCK) {
            d->MAP(i, j).type = CORRIDOR;
            d->MAP(i, j).hardness = 0;
        }
    }
    free(cost);
}

// Helper function that iterates through the array, counts all cells that are rooms, and returns if it is more than
// the provided percentage.
static bool is_room_percentage_covered(const Dungeon_T *d, float percentage_covered) {
    int i, j, count;

    count = 0;
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            if (d->MAP(i, j).type == ROOM) {
                count++;
            }
        }
    }

    return ((count / (double) (d->height * d->width)) > percentage_covered);
}

// Heavy lifting function that partitions the dungeon. If the dimensions of a given partition are outside of
// PERCENTAGE_SPLIT_FORCE of each other, or one of the sides is in range for the allowed sizes for partitions
// it will split along the long side... otherwise it will randomly choose a direction to split along. It splits along
// a valid range, making sure both resultant partitions will be valid according to the minimums in setting.h.
// It then sets the new partitions to be children of itself, and if the children partitions can be split further it
// will recurse into them, splitting them as necessary.
static void partition(Binary_Tree_T *t) { // NOLINT(misc-no-recursion)
    int split;
    bool split_horizontal;
    Partition_T *p, *left, *right;

    // Get our partition so we don't have to do hideous casts
    p = t->data;

    // Ugly ternary... checks if the dimension is within valid range, since we don't want to split that direction
    // then. If it's not, we split on if height or width is within PERCENTAGE_SPLIT_FORCE of each other. If all of
    // these are false, we pick a random direction. Cast to bool for safety... even though it will always be 0 or 1.
    split_horizontal = (bool)
            (MIN_PARTITION_HEIGHT <= p->height && p->height <= MAX_PARTITION_HEIGHT ? false : // check height range
             MIN_PARTITION_WIDTH <= p->width && p->width <= MAX_PARTITION_WIDTH ? true : // check width range
             p->height * (1 + PERCENTAGE_SPLIT_FORCE) < (float) p->width ? false : // check if height is much smaller
             p->width * (1 + PERCENTAGE_SPLIT_FORCE) < (float) p->height ? true : // check if width is much smaller
             rand() % 2); // NOLINT(cert-msc50-cpp)

    // Do the split
    if (split_horizontal) {
        split = rand_int_in_range(MIN_PARTITION_HEIGHT, p->height - MIN_PARTITION_HEIGHT);
        left = new_partition(p->y, p->x, split, p->width);
        right = new_partition(p->y + split, p->x, p->height - split, p->width);
    } else {
        split = rand_int_in_range(MIN_PARTITION_WIDTH, p->width - MIN_PARTITION_WIDTH);
        left = new_partition(p->y, p->x, p->height, split);
        right = new_partition(p->y, p->x + split, p->height, p->width - split);
    }

    // Set our child nodes
    t->left = new_binary_tree(left);
    t->right = new_binary_tree(right);

    // Check if the children nodes are valid to be split... if they are... split them
    if (!(MIN_PARTITION_HEIGHT <= left->height && left->height <= MAX_PARTITION_HEIGHT) ||
        !(MIN_PARTITION_WIDTH <= left->width && left->width <= MAX_PARTITION_WIDTH)) {
        partition(t->left);
    }
    if (!(MIN_PARTITION_HEIGHT <= right->height && right->height <= MAX_PARTITION_HEIGHT) ||
        !(MIN_PARTITION_WIDTH <= right->width && right->width <= MAX_PARTITION_WIDTH)) {
        partition(t->right);
    }
}

// Heavy lifting function that fills partitions with rooms. It only fills partitions that are at the lowest level,
// meaning they have no children. It will try FAILED_ROOM_PLACEMENT times to fill a room with a valid room, randomly
// generating values that are within range for a valid room, and fit in the partition. If it can't generation a room
// it will return false, and the failure will bubble up back to the caller. In the case of new_dungeon() this will
// trigger a dungeon generation failure and it will try and again from scratch. When it does generate a valid room
// it will then allocate space on the dungeons room array, set the parameters for the array, and paint room tiles
// into the dungeon map.
static bool generate_rooms(Dungeon_T *d, Binary_Tree_T *t) { // NOLINT(misc-no-recursion)
    int tries, y, x, height, width, i, j;
    Partition_T *p;

    // Only fill partitions with no children. Return true only if both children did.
    if (t->left != NULL && t->right != NULL) {
        return generate_rooms(d, t->left) && generate_rooms(d, t->right);
    } else {
        tries = 0;
        p = t->data;

        // Try placing room randomly within valid range... we're just making sure rooms aren't touching here.
        do {
            if (tries > FAILED_ROOM_PLACEMENT) {
                return false;
            }
            y = rand_int_in_range(p->y, p->y + p->height - MIN_ROOM_HEIGHT);
            x = rand_int_in_range(p->x, p->x + p->height - MIN_ROOM_WIDTH);
            height = rand_int_in_range(MIN_ROOM_HEIGHT, p->y + p->height - y);
            width = rand_int_in_range(MIN_ROOM_WIDTH, p->x + p->height - x);
            tries++;
        } while (!is_valid_room(d, y, x, height, width));

        // Add the room to the dungeon.
        d->num_rooms++;
        d->rooms[d->num_rooms - 1].y = y;
        d->rooms[d->num_rooms - 1].x = x;
        d->rooms[d->num_rooms - 1].height = height;
        d->rooms[d->num_rooms - 1].width = width;

        // Paint the room into the dungeon
        for (i = y; i < y + height; i++) {
            for (j = x; j < x + width; j++) {
                d->MAP(i, j).type = ROOM;
                d->MAP(i, j).hardness = 0;
            }
        }

        return true;
    }
}

// Randomizes the hardness across the dungeon, within the range provided in setting.h. Any immutable cells are set to
// be maximum hardness: otherwise the hardness is added to the base, which is controlled by DEFAULT_HARDNESS.
static void randomize_hardness(Dungeon_T *d) {
    int i, j;

    for (i = 1; i < d->height - 1; i++) {
        for (j = 1; j < d->width - 1; j++) {
            if (d->MAP(i, j).type == ROCK) {
                d->MAP(i, j).hardness += rand_int_in_range(MIN_ROCK_HARDNESS, MAX_ROCK_HARDNESS);
            }
        }
    }
}

// Function that iterates through a random permutation of the rooms. Starting at the first room in the permutation
// it will connect it to the next, and so on, until it's connected all of them. The source and destination coordinates
// for paint_corridor() are set to be random points within the room itself for added variability.
static void generate_corridors(Dungeon_T *d) {
    int *shuffle;
    int r, i, j, y, x;

    // Generate a random permutation of the rooms
    shuffle = safe_malloc(d->num_rooms * sizeof(int));
    for (r = 0; r < d->num_rooms; r++) {
        shuffle[r] = r;
    }
    shuffle_int_array(shuffle, d->num_rooms);

    // Connect the rooms one to the next. i, j are the source, x, y are the destination. Always generate in range so
    // we don't have to bounds check.
    for (r = 0; r < d->num_rooms - 1; r++) {
        i = rand_int_in_range(d->rooms[shuffle[r]].y, d->rooms[shuffle[r]].y + d->rooms[shuffle[r]].height - 1);
        j = rand_int_in_range(d->rooms[shuffle[r]].x, d->rooms[shuffle[r]].x + d->rooms[shuffle[r]].width - 1);
        y = rand_int_in_range(d->rooms[shuffle[r + 1]].y,
                              d->rooms[shuffle[r + 1]].y + d->rooms[shuffle[r + 1]].height - 1);
        x = rand_int_in_range(d->rooms[shuffle[r + 1]].x,
                              d->rooms[shuffle[r + 1]].x + d->rooms[shuffle[r + 1]].width - 1);
        paint_corridor(d, i, j, y, x);
    }

    // Cleanup our permutation array
    free(shuffle);
}

// Simply places an upward stair and a downwards stair within rooms in the dungeon, and not in the same room.
static void place_stairs(Dungeon_T *d) {
    int r1, r2, i, j;

    // Pick a room, then pick coordinates in that room, and paint
    r1 = rand_int_in_range(0, d->num_rooms - 1);
    i = rand_int_in_range(d->rooms[r1].y + 1, d->rooms[r1].y + d->rooms[r1].height - 2);
    j = rand_int_in_range(d->rooms[r1].x + 1, d->rooms[r1].x + d->rooms[r1].width - 2);
    d->MAP(i, j).type = STAIR_UP;

    // Pick a different room, then pick coordinates in the new room and paint
    do {
        r2 = rand_int_in_range(0, d->num_rooms - 1);
    } while (r1 == r2);
    i = rand_int_in_range(d->rooms[r2].y + 1, d->rooms[r2].y + d->rooms[r2].height - 2);
    j = rand_int_in_range(d->rooms[r2].x + 1, d->rooms[r2].x + d->rooms[r2].width - 2);
    d->MAP(i, j).type = STAIR_DOWN;
}

// Generates a new dungeon. It sets up the dungeon struct itself so it can call init_dungeon() and sets up the binary
// tree for the partition() function, seeding the initial partition with one that is set to be a cell smaller on every
// side, since the outer border of the dungeon is immutable (this guarantees rooms will always generate inside the map
// meaning we can be lazier with our checking). It then tries generating rooms according to the given parameters...
// It it makes too little, too many, fails to place a room in a partition, or doesn't cover enough of the map, it will
// retry, until it has failed too many times. When it generates a workable dungeon, it randomizes the hardness across
// the array, connects rooms, places stairs, cleans up after itself, and returns a pointer to the full dungeon to the
// caller.
Dungeon_T *
new_dungeon(int height, int width, int min_rooms, int max_rooms, float percentage_covered) {
    int tries;
    bool success_room;
    Dungeon_T *d;

    // Allocate our dungeon
    d = safe_malloc(sizeof(Dungeon_T));

    // Initialize all dungeon variable and draw the border
    init_dungeon(d, height, width);
    generate_dungeon_border(d);

    // Try generating our dungeon... if a partition can't be filled, or the dungeon isn't covered enough, or the
    // number of rooms isn't in range... try again.
    tries = 0;
    do {
        int num_partitions;
        Binary_Tree_T *t;

        // Only need to free rooms if we failed to generate a dungeon and need to try again
        if (tries > 0) {
            free(d->rooms);
        }

        // Check to see if we failed after too many tries to generate a dungeon
        if (tries > FAILED_DUNGEON_GENERATION) {
            kill(DUNGEON_GENERATION_FAILURE,
                 "FATAL ERROR! FAILED TO GENERATE WORKABLE DUNGEON AFTER %i TRIES! TRY NEW PARAMETERS!\n",
                 FAILED_DUNGEON_GENERATION);
        }
        // Allocate our binary tree, and seed it with a partition without the immutable cells
        t = new_binary_tree(new_partition(1, 1, height - 2, width - 2));

        // Partition the dungeon, count the number of children, allocate the room array, and generate rooms
        partition(t);
        num_partitions = count_leaf_nodes(t);
        d->rooms = safe_malloc(num_partitions * sizeof(Room_T));
        success_room = generate_rooms(d, t);

        // Cleanup
        cleanup_binary_tree(t);
        tries++;
    } while (!success_room || !is_room_percentage_covered(d, percentage_covered) ||
             min_rooms > d->num_rooms || d->num_rooms > max_rooms);

    // Finish up our generation
    randomize_hardness(d);
    generate_corridors(d);
    place_stairs(d);
    place_pc(d);

    return d;
}

// Initializes a dungeon's variables. Sets num_rooms to be zero, and rooms to NULL. This is the function to be sure to
// update if Cell_T is extended.
void init_dungeon(Dungeon_T *d, int height, int width) {
    int i, j;

    // Static values
    d->height = height;
    d->width = width;

    // These will be set later as part of generation
    d->num_rooms = 0;
    d->rooms = NULL;
    d->player = NULL;

    // Allocate our map array
    d->map = safe_malloc(height * width * sizeof(Cell_T));
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            d->MAP(i, j).type = DEFAULT_CELL_TYPE;
            d->MAP(i, j).hardness = DEFAULT_HARDNESS;
            d->MAP(i, j).character = NULL;
        }
    }
}

// See dungeon.h
void generate_dungeon_border(Dungeon_T *d) {
    int i;

    // Left and right
    for (i = 1; i < d->height - 1; i++) {
        d->MAP(i, 0).hardness = IMMUTABLE_ROCK_HARDNESS;
        d->MAP(i, d->width - 1).hardness = IMMUTABLE_ROCK_HARDNESS;
    }

    // Top and bottom
    for (i = 0; i < d->width; i++) {
        d->MAP(0, i).hardness = IMMUTABLE_ROCK_HARDNESS;
        d->MAP(d->height - 1, i).hardness = IMMUTABLE_ROCK_HARDNESS;
    }
}

// See dungeon.h
void place_pc(Dungeon_T *d) {
    int room, y, x;

    // Pick room randomly, then pick random coordinate in that room and create the PC.
    room = rand_int_in_range(0, d->num_rooms - 1);
    y = rand_int_in_range(d->rooms[room].y, d->rooms[room].y + d->rooms[room].height - 1);
    x = rand_int_in_range(d->rooms[room].x, d->rooms[room].x + d->rooms[room].width - 1);
    d->player = new_character(y, x, PC_SYMBOL, PC_COLOR, true);
    d->MAP(y, x).character = d->player;
}

// See dungeon.h
void cleanup_dungeon(Dungeon_T *d) {
    int i, j;

    // Free any characters that aren't the player
    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {
            if (d->MAP(i, j).character != NULL && !d->MAP(i, j).character->player) {
                cleanup_character(d->MAP(i, j).character);
            }
        }
    }

    // Free the rest of our pointers
    free(d->player);
    free(d->map);
    free(d->rooms);
    free(d);
}

// See dungeon.h
char cell_type_char(Cell_Type_T c) {
    switch (c) {
        case ROCK: // NOLINT(bugprone-branch-clone)
            return ROCK_CHAR;
        case ROOM:
            return ROOM_CHAR;
        case CORRIDOR:
            return CORRIDOR_CHAR;
        case STAIR_UP:
            return STAIR_UP_CHAR;
        case STAIR_DOWN:
            return STAIR_DOWN_CHAR;
        default:
            return '?';
    }
}

// See dungeon.h
char *cell_type_color(Cell_Type_T c) {
    switch (c) {
        case ROCK:
            return ROCK_COLOR;
        case ROOM:
            return ROOM_COLOR;
        case CORRIDOR:
            return CORRIDOR_COLOR;
        case STAIR_UP: // NOLINT(bugprone-branch-clone)
            return STAIR_UP_COLOR;
        case STAIR_DOWN:
            return STAIR_DOWN_COLOR;
        default:
            return CONSOLE_RESET;
    }
}

// See dungeon.h
void print_dungeon(const Dungeon_T *d) {
    int i, j;

    for (i = 0; i < d->height; i++) {
        for (j = 0; j < d->width; j++) {

            // Check if there's a character
            if (d->MAP(i, j).character != NULL) {
                printf("%s%c", d->MAP(i, j).character->color, d->MAP(i, j).character->symbol);
            } else {
                printf("%s%c", cell_type_color(d->MAP(i, j).type), cell_type_char(d->MAP(i, j).type));
            }
        }

        // Print a new line after every row
        printf("%s\n", CONSOLE_RESET);
    }
}



