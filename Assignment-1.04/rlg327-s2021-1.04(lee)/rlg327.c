#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#include "utils.h"
#include "dungeon.h"
#include "path.h"

#define BIT_SMART 0x1
#define BIT_TELE 0x2
#define BIT_TUN 0x4
#define BIT_ERR 0x8

static int32_t monster_cmp(const void *key, const void *with) {
  if( ((character_t *)key)->next_turn == ((character_t *)with)->next_turn){
    return (int32_t)((character_t *) key)->sequence_next_turn - ((character_t *) with)->sequence_next_turn;
  }
  else{
    return (int32_t)((character_t *) key)->next_turn - ((character_t *) with)->next_turn;
  }
}

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <number_of_monster>]",
          name);

  exit(-1);
}

int generate_monster_code()
{
  return rand() & 0xf;
}



void generate_character(dungeon_t *d,heap_t *h){

  int next_turn = 0;
  int sequence_next_turn =0;

  character_t *c = malloc(sizeof(character_t));
  c->pc = malloc(sizeof(pc_t));
  c->npc = malloc(sizeof(npc_t));

  c->pc->position[dim_x] = d->pc.position[dim_x];
  c->pc->position[dim_y] = d->pc.position[dim_y];
  c->npc = NULL;
  c->x_pos = d->pc.position[dim_x];
  c->y_pos = d->pc.position[dim_y];
  c->next_turn = next_turn;
  c->sequence_next_turn = sequence_next_turn++;
  c->speed = 10;
  c->is_alive = 1;

  d->characters[d->pc.position[dim_y]][d->pc.position[dim_x]] = c;
  heap_insert(h,&d->characters[d->pc.position[dim_y]][d->pc.position[dim_x]]);
  //add to queue

  int i;
  for( i=0; i<d->num_monster;i++){

    character_t *m = malloc(sizeof(character_t));
    m->pc = malloc(sizeof(pc_t));
    m->npc = malloc(sizeof(npc_t));

    m->npc->monster_code = generate_monster_code();
    // printf("%d \n",m->npc->monster_code);
    int random_x;
    int random_y;
    int attempt = 0;
    do{
      random_x = rand_range(1,79);
      random_y = rand_range(1,20);

      attempt ++;
      if(attempt>=2000){
        printf("%s","Maximum can only be 1481");
        break;
      }
    }while(d->map[random_y][random_x]==ter_wall_immutable ||
          (random_x == d->pc.position[dim_x]&&random_y==d->pc.position[dim_y] )
          || (d->characters[random_y][random_x]!=NULL));
    if(attempt>=2000){
        exit(0);
    }
    // printf("%d %d\n",random_x,random_y);
    m->pc = NULL;
    m->x_pos = random_x;
    m->y_pos = random_y;
    m->next_turn = next_turn;
    m->sequence_next_turn = sequence_next_turn++;
    m->speed = rand_range(5,20);
    m->is_alive = 1;

    d->characters[random_y][random_x] = m;
    //add to quue  
    printf("%d ",d->characters[random_y][random_x]->sequence_next_turn);
    heap_insert(h,d->characters[random_y][random_x]);
  
  } 

}

void init_monster(dungeon_t *d)
{
  heap_t h;
  heap_init(&h,monster_cmp,NULL);

  generate_character(d,&h);

  character_t *temp;
  while((temp = heap_remove_min(&h))){
    printf("%d ",temp->sequence_next_turn);
  }

}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image, do_monster;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = do_monster = 0;
  do_seed = 1;
  save_file = load_file = NULL;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
          case 'n':
             if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%d", &d.num_monster) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_monster = 1;
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  if (!do_load) {
    /* Set a valid position for the PC */
    d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  if(!do_monster){
    d.num_monster = 10;
  }

  // generate_character(&d);
  // for(int j =0; j<DUNGEON_Y;j++){
  //   for(int i =0;i<DUNGEON_X;i++){
  //     if(d.characters[j][i] != NULL)
  //     {
  //       if(d.characters[j][i]!= NULL){
  //         printf("%d",d.characters[j][i]->sequence_next_turn);
  //       }
        
  //     }
  //     else{
  //       printf(".");
  //     }
  //   }
  //   printf("\n");
  // }
  init_monster(&d);
  render_dungeon(&d);

  // dijkstra(&d);
  // dijkstra_tunnel(&d);
  // render_distance_map(&d);
  // render_tunnel_distance_map(&d);
  //render_hardness_map(&d);
  // render_movement_cost_map(&d);

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}
