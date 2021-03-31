#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <cstdlib>
#include <ctime>



#include "utils.h"

int makedirectory(char *dir)
{
  char *slash;

  for (slash = dir + strlen(dir); slash > dir && *slash != '/'; slash--)
    ;

  if (slash == dir) {
    return 0;
  }

  if (mkdir(dir, 0700)) {
    if (errno != ENOENT && errno != EEXIST) {
      fprintf(stderr, "mkdir(%s): %s\n", dir, strerror(errno));
      return 1;
    }
    if (*slash != '/') {
      return 1;
    }
    *slash = '\0';
    if (makedirectory(dir)) {
      *slash = '/';
      return 1;
    }

    *slash = '/';
    if (mkdir(dir, 0700) && errno != EEXIST) {
      fprintf(stderr, "mkdir(%s): %s\n", dir, strerror(errno));
      return 1;
    }
  }

  return 0;
}

//Lee's
int roll_dice(int base, int dice, int sides)
{

  srand(time(NULL));

  if(dice ==0){
      return base;
  }

  int final_value = 0;

  int i;
  for(i =0; i<dice; i++){
    int dice_value = rand()%sides +1;
    final_value += dice_value;
  }

  return base + final_value;
}
