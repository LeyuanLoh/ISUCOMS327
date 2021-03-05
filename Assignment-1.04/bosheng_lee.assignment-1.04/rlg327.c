#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "dungeon.h"
#include "path.h"
#include <limits.h>

#define BIT_SMART 0x1
#define BIT_TELE 0x2
#define BIT_TUN 0x4
#define BIT_ERR 0x8

//Leyuan
//To loop through its adjacent cell
#define MAX(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
  })

#define MIN(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
  })

static int32_t monster_cmp(const void *key, const void *with)
{
  if (((character_t *)key)->next_turn == ((character_t *)with)->next_turn)
  {
    return ((int32_t)((character_t *)key)->sequence_next_turn - ((character_t *)with)->sequence_next_turn);
  }
  else
  {
    return ((int32_t)((character_t *)key)->next_turn - ((character_t *)with)->next_turn);
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

void generate_character(dungeon_t *d, heap_t *h)
{

  int next_turn = 0;
  int sequence_next_turn = 0;

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

  heap_insert(h, d->characters[d->pc.position[dim_y]][d->pc.position[dim_x]]);
  //add to queue

  int i;
  //remember to change it
  for (i = 0; i < d->num_monster; i++)
  {

    character_t *m = malloc(sizeof(character_t));
    m->pc = malloc(sizeof(pc_t));
    m->npc = malloc(sizeof(npc_t));

    m->npc->monster_code = generate_monster_code();
    // printf("%d \n",m->npc->monster_code);
    int random_x;
    int random_y;
    int attempt = 0;
    do
    {
      int random_room = rand_range(1, d->num_rooms - 1);
      random_x = (d->rooms[random_room].position[dim_x] +
                  (rand() % d->rooms[random_room].size[dim_x]));
      random_y = (d->rooms[random_room].position[dim_y] +
                  (rand() % d->rooms[random_room].size[dim_y]));

      attempt++;
      if (attempt >= 2000)
      {
        printf("%s", "Enter a smaller value! \n");
        break;
      }
    } while (d->map[random_y][random_x] == ter_wall_immutable || (random_x == d->pc.position[dim_x] && random_y == d->pc.position[dim_y]) || (d->characters[random_y][random_x] != NULL));

    if (attempt >= 2000)
    {
      exit(0);
    }
    // printf("%d %d\n",random_x,random_y);
    m->pc = NULL;
    m->x_pos = random_x;
    m->y_pos = random_y;
    m->next_turn = next_turn;
    m->sequence_next_turn = sequence_next_turn++;
    m->speed = rand_range(5, 20);
    m->is_alive = 1;

    d->characters[random_y][random_x] = m;
    //add to quue
    heap_insert(h, d->characters[random_y][random_x]);
  }
}

//Leyuan
//Initiliaze character map
void characters_init(dungeon_t *d)
{
  for (int i = 0; i < DUNGEON_Y; i++)
  {
    for (int j = 0; j < DUNGEON_X; j++)
    {
      d->characters[i][j] = NULL;
    }
  }
}

void init_monster(dungeon_t *d, heap_t *h)
{
  characters_init(d);

  generate_character(d, h);

  // character_t *temp;
  // while((temp = heap_remove_min(&h))){
  //   printf("%d ",temp->sequence_next_turn);
  // }
}

//Leyuan
//To check whether they are in the same room.
int same_room(pair_t pc, pair_t mons, dungeon_t *d)
{
  uint32_t i;
  room_t *r;

  for (i = 0; i < d->num_rooms; i++)
  {
    r = d->rooms + i;

    //check pc is in the room
    if (pc[dim_x] >= r->position[dim_x] && pc[dim_x] < (r->position[dim_x] + r->size[dim_x]) &&
        pc[dim_y] >= r->position[dim_y] && pc[dim_y] < (r->position[dim_y] + r->size[dim_y]))
    {
      //check if monster is in the room
      if (mons[dim_x] >= r->position[dim_x] && mons[dim_x] < (r->position[dim_x] + r->size[dim_x]) &&
          mons[dim_y] >= r->position[dim_y] && mons[dim_y] < (r->position[dim_y] + r->size[dim_y]))
      {
        return 1;
      }
    }
  }

  //not in the same room
  return 0;
}

//Helper method to help moving a to b
void move(character_t *a, int x, int y, dungeon_t *d)
{

  //set isAlive to 0.
  if (d->characters[y][x] != NULL)
  {
    if(d->characters[y][x]!= a){
      d->characters[y][x]->is_alive = 0;
    }
  }

  //create a new connection let char[y][x] to connect to.
  character_t *b = malloc(sizeof(character_t));
  b->pc = malloc(sizeof(pc_t));
  b->npc = malloc(sizeof(npc_t));

  if (a->pc != NULL)
  {
    b->pc->position[dim_x] = x;
    b->pc->position[dim_y] = y;
  }
  else
  {
    b->pc = NULL;
  }

  if (a->npc != NULL)
  {
    b->npc->monster_code = a->npc->monster_code;
  }
  else
  {
    b->npc = NULL;
  }

  b->is_alive = a->is_alive;
  b->x_pos = x;
  b->y_pos = y;
  b->next_turn = a->next_turn;
  b->sequence_next_turn = a->sequence_next_turn;
  b->speed = a->speed;

  //break the connection and create a new connection.
  d->characters[y][x] = b;
  
  
  

  //Safe
  if (!(x == a->x_pos && y == a->y_pos))
  {
    d->characters[a->y_pos][a->x_pos] = NULL;
  }
}

//Leyuan
//Movement of monster and pc.
void movement(dungeon_t *d, heap_t *h)
{
  dijkstra(d);
  dijkstra_tunnel(d);
  static character_t *c;

  while ((c = heap_remove_min(h)))
  {
    if (h->size == 0)
    {
      if(c->pc != NULL){
        printf("PC WIN  !\n");
      }
      break;
    }

    if (c->is_alive == 0)
    {
      continue;
    }

    c->next_turn = c->next_turn + (1000 / c->speed);
    //First, check if they are alive or not. If not alive, ignore it and DON'T push it back to heap.
    if (d->characters[c->y_pos][c->x_pos] != NULL && c->is_alive == 1)
    {
      //check if its pc.
      if (c->pc != NULL)
      { //check surrounding cell if that is a monster. If yes, kill it.
        for (int i = MAX(c->y_pos - 1, 1); i <= MIN(c->y_pos + 1, 19); i++)
        {
          for (int j = MAX(c->x_pos - 1, 1); j <= MIN(c->x_pos + 1, 78); j++)
          {
            //Skipping it current cell
            if (i == c->y_pos && j == c->x_pos)
            {
              continue;
            }
            if (d->characters[i][j] != NULL)
            {
              if (d->characters[i][j]->npc != NULL)
              {
                move(c, j, i, d);
                c = d->characters[i][j];
                //First time using this, its amazing.
                goto pcHere;
              }
            }
          }
        }

      //place pc back to heap
      //update pc location in dungeon
      pcHere:
        d->pc.position[dim_x] = c->pc->position[dim_x];
        d->pc.position[dim_y] = c->pc->position[dim_y];

        //update the map
        dijkstra(d);
        dijkstra_tunnel(d);

        //1000000/fps
        usleep(1000000 / 3);
        render_dungeon(d);
      }
      //monster cases
      else
      {
        if ((c->npc->monster_code & 4))
        {
          int smallestX = 0, smallestY = 0;
          int smallestDis = INT_MAX;
          //finding the neigbhors with the lowess hardness
          if ((c->npc->monster_code & BIT_SMART))
          {
            for (int i = MAX(c->y_pos - 1, 1); i <= MIN(c->y_pos + 1, 19); i++)
            {
              for (int j = MAX(c->x_pos - 1, 1); j <= MIN(c->x_pos + 1, 78); j++)
              {
                if (i == c->y_pos && j == c->x_pos)
                {
                  continue;
                }
                if (d->pc_tunnel[i][j] < smallestDis)
               {
                  smallestDis = d->pc_tunnel[i][j];
                  smallestX = j;
                  smallestY = i;
                }
              }
            }
          }
          else{
            if ((c->npc->monster_code & BIT_TELE)){
              if(d->pc.position[dim_x]>c->x_pos){
                smallestX = c->x_pos+1;
                smallestY = c->y_pos;
              }
              else if(d->pc.position[dim_x]<c->x_pos){
                smallestX = c->x_pos-1; 
                smallestY = c->y_pos;
             }
             else{
               smallestX = c->x_pos;
               if(d->pc.position[dim_y]>c->y_pos){
                 smallestY = c->y_pos+1;
               } 
               else if(d->pc.position[dim_y]<c->y_pos){
                 smallestY = c->y_pos-1;  
               }
               else{
                 smallestY = c->y_pos;
               }
              }
            }
            else{
              int x_random;
              int y_random ;
              do{
                 x_random = rand_range(-1,1);
                 y_random = rand_range(-1,1);
                
              }while(d->map[c->y_pos+y_random][c->x_pos+x_random]==ter_wall_immutable);
              smallestX = c->x_pos +x_random;
              smallestY = c->y_pos+ y_random;
            }
          }

          //check if hardness is equal to 0
          if (d->hardness[smallestY][smallestX] == 0)
          {
            //check if that is the border, I know this is not going to happen.
            if (d->map[c->y_pos][c->x_pos] != ter_wall_immutable)
            {
              if (d->map[c->y_pos][c->x_pos] == ter_wall)
              {
                d->map[c->y_pos][c->x_pos] = ter_floor_hall;
              }
              if ((d->characters[smallestY][smallestX]) != NULL)
              {
                //monster kill pc
                if (d->characters[smallestY][smallestX]->pc != NULL)
                {
                  move(c, smallestX, smallestY, d);
                  c = d->characters[smallestY][smallestX];
                  render_dungeon(d);
                  //Game end
                  break;
                }
              }
              move(c, smallestX, smallestY, d);
              c = d->characters[smallestY][smallestX];
              render_dungeon(d);
            }
          }
          //hardness not zero, minus 85.
          else
          {
            //printf("Dig\n");
            d->hardness[smallestY][smallestX] = MAX(0, d->hardness[smallestY][smallestX] - 85);
          }
        }

        else{
          int x_min,y_min = 0;
          int min_dis = INT_MAX;

          //find lowest distance
          if ((c->npc->monster_code & BIT_SMART)){
            for (int i = MAX(c->y_pos - 1, 1); i <= MIN(c->y_pos + 1, 19); i++)
            {
             for (int j = MAX(c->x_pos - 1, 1); j <= MIN(c->x_pos + 1, 78); j++)
             {
               if (i == c->y_pos && j == c->x_pos)
               {
                 continue;
               }
               if(d->map[i][j] != ter_wall&&d->map[i][j] != ter_wall_immutable){
                 if (d->pc_distance[i][j] < min_dis)
                 {
                   min_dis = d->pc_distance[i][j];
                   x_min= j;
                   y_min = i;
                 }
               } 
             }
            }
          }
          else{
            if(d->pc.position[dim_x]>c->x_pos){
              x_min = c->x_pos+1;
              y_min = c->y_pos;
              if(d->map[y_min][x_min]==ter_wall||d->map[y_min][x_min]==ter_wall_immutable){
                x_min = c->x_pos;
              }
            }
            else if(d->pc.position[dim_x]<c->x_pos){
              x_min = c->x_pos-1; 
              y_min = c->y_pos;
              if(d->map[y_min][x_min]==ter_wall||d->map[y_min][x_min]==ter_wall_immutable){
                x_min = c->x_pos;
              }
            }
            else{
              x_min = c->x_pos;
              if(d->pc.position[dim_y]>c->y_pos){
                y_min = c->y_pos+1;
                if(d->map[y_min][x_min]==ter_wall||d->map[y_min][x_min]==ter_wall_immutable){
                  y_min = c->y_pos;
                }
              } 
              else if(d->pc.position[dim_y]<c->y_pos){
                y_min = c->y_pos-1;
                if(d->map[y_min][x_min]==ter_wall||d->map[y_min][x_min]==ter_wall_immutable){
                  y_min = c->y_pos;
                }  
              }
              else{
                y_min = c->y_pos;
              }
            }
          }

          //check if next place is pc
          if ((d->characters[y_min][x_min]) != NULL)
          {
              //monster kill pc
              if (d->characters[y_min][x_min]->pc != NULL)
              {
                move(c, x_min, y_min, d);
                c = d->characters[y_min][x_min];
                render_dungeon(d);
                //Game end
                break;
              }
          }
          move(c,x_min,y_min,d);
          c = d->characters[y_min][x_min];
          render_dungeon(d);
        }
      }
      heap_insert(h, c);
    }
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
  memset(&d, 0, sizeof(d));

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

  if (argc > 1)
  {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0)
    {
      if (argv[i][0] == '-')
      { /* All switches start with a dash */
        if (argv[i][1] == '-')
        {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1])
        {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */)
          {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load")))
          {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save")))
          {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
            if (!strcmp(argv[++i], "seed"))
            {
              do_save_seed = 1;
              do_save_image = 0;
            }
            else if (!strcmp(argv[i], "image"))
            {
              do_save_image = 1;
              do_save_seed = 0;
            }
            else
            {
              save_file = argv[i];
            }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image")))
          {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-')
          {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%d", &d.num_monster) /* Argument is not an integer */)
          {
            usage(argv[0]);
          }
          do_monster = 1;
          break;
        default:
          usage(argv[0]);
        }
      }
      else
      { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed)
  {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);

  if (do_load)
  {
    read_dungeon(&d, load_file);
  }
  else if (do_image)
  {
    read_pgm(&d, pgm_file);
  }
  else
  {
    gen_dungeon(&d);
  }

  if (!do_load)
  {
    /* Set a valid position for the PC */
    d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);

  if (!do_monster)
  {
    d.num_monster = 10;
  }

  //Leyuan
  heap_t h;
  heap_init(&h, monster_cmp, NULL);

  //Lee
  init_monster(&d, &h);

  render_dungeon(&d);

  //Leyuan
  movement(&d, &h);

  if (do_save)
  {
    if (do_save_seed)
    {
      /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image)
    {
      if (!pgm_file)
      {
        fprintf(stderr, "No image file was loaded.  Using default.\n");
        do_save_image = 0;
      }
      else
      {
        /* Extension of 3 characters longer than image extension + null. */
        save_file = malloc(strlen(pgm_file) + 4);
        strcpy(save_file, pgm_file);
        strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image)
    {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}
