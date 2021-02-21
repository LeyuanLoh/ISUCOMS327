#ifndef ROGUE_DIJKSTRA_H
#define ROGUE_DIJKSTRA_H

#include "dungeon.h"

// See dijkstra.c for several helper functions.

// Define a macro to help obfuscate bare pointer arithmetic
#define COST(a, b) cost[(a) * d->width + (b)]

// Generates a Dijkstra cost map to map corridors. By then "rolling" downhill from the goal to the source, we can
// generate the shortest load_path and paint a new corridor. See paint_corridor in dungeon.c for details.
int *generate_dijkstra_corridor_map(const Dungeon_T *d, int y, int x);

#endif //ROGUE_DIJKSTRA_H
