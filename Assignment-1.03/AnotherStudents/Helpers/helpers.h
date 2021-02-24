#ifndef ROGUE_HELPERS_H
#define ROGUE_HELPERS_H

#include <stddef.h>

// Defines colors for printing to console
#define BACKGROUND_BLUE "\x1b[48;2;70;130;180m"
#define BACKGROUND_WHITE "\x1b[48;2;255;255;255m"
#define BACKGROUND_GREY "\x1b[48;2;127;127;127m"
#define BACKGROUND_BLACK "\x1b[48;2;0;0;0m"

// Defines the proper key combo to reset the console
#define CONSOLE_RESET "\x1b[0m"

// Wraps malloc() with an additional check to make sure the pointer is real. If it's not the program kills.
void *safe_malloc(size_t size);

// Same as safe_malloc()
void *safe_calloc(size_t num, size_t size);

// Same as safe_malloc()
void *safe_realloc(void *ptr, size_t size);

// Returns a random integer in the range [lower, upper] (inclusive). LOWER MUST BE < UPPER OR AN ARITHMETIC FAULT
// WILL BE GENERATED
int rand_int_in_range(int lower, int upper);

// Shuffles the given int array with a Fisher-Yates algorithm. Modifies the array in memory.
void shuffle_int_array(int *arr, int n);

// Returns the number of digits in a given integer (used for printing)
int count_digits(int n);

// Wrapper to kill the program while informing the user as to why. MSG MUST BE PROPERLY FORMATTED FOR FPRINT OR WE WILL
// CAUSE A SEGFAULT AND CRASH
_Noreturn void kill(int code, char *msg, ...);

#endif //ROGUE_HELPERS_H
