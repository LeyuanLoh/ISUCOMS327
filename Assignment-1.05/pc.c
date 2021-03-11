#include <stdlib.h>
#include <ncurses.h>

#include "string.h"
#include "math.h"
#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "npc.h"

#define KEY_ESC 27



void pc_delete(pc_t *pc)
{
  if (pc)
  {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  memset(&d->pc, 0, sizeof(d->pc));
  d->pc.symbol = '@';

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = calloc(1, sizeof(*d->pc.pc));
  d->pc.npc = NULL;
  d->pc.kills[kill_direct] = d->pc.kills[kill_avenged] = 0;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

void print_monster_list(dungeon_t *d, int limit)
{
  int i = 0;
  int j = 0;
  int line = 4;
  mvprintw(2,17,"                                    ");
  mvprintw(3,17,"            Monster List            ");
  for(i = limit, j=0; i<d->num_monsters&&j<13;i++,j++){
      char *y_dir;
      char *x_dir;
      if(d->monster_list[i].dis_from_pc[dim_y]>0){
        y_dir = "North";
      }
      else{
        y_dir = "South";
      }
      if(d->monster_list[i].dis_from_pc[dim_x]>0){
        x_dir = "East";
      }
      else{
        x_dir = "West";
      }
      mvprintw(line,17, "   %2d  %c, %3d %s and %3d %s    ",
                i+1,
                d->monster_list[i].characteristics,
                abs(d->monster_list[i].dis_from_pc[dim_y]), 
                y_dir,
                abs(d->monster_list[i].dis_from_pc[dim_x]), 
                x_dir);
      line++;                
  }
  mvprintw(j+4,17,"                                  ");

}

void create_monster_list(dungeon_t *d)
{

    d->monster_list = malloc(d->num_monsters * sizeof(npc_t));

    int x_pc = d->pc.position[dim_x];
    int y_pc = d->pc.position[dim_y];

    int y,x,mon_counter =0;

    for(y=0; y<DUNGEON_Y;y++){
      for(x=0;x<DUNGEON_X;x++){
        if(charxy(x,y) != NULL){
          if(charxy(x,y)->pc ==NULL && mon_counter<d->num_monsters){
            int y_monster,x_monster =0;
            y_monster = charxy(x,y)->position[dim_y];
            x_monster = charxy(x,y)->position[dim_x];
            d->monster_list[mon_counter].characteristics = charxy(x,y)->symbol;
            d->monster_list[mon_counter].dis_from_pc[dim_y] = y_monster - y_pc;
            d->monster_list[mon_counter].dis_from_pc[dim_x] = x_monster - x_pc;

            mon_counter++;
          }
        }
      }
    }
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{

  int key;

  dir[dim_y] = dir[dim_x] = 0;

  /* First, eat anybody standing next to us. */
  // if (charxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y] - 1)) {
  //   dir[dim_y] = -1;
  //   dir[dim_x] = -1;
  // } else if (charxy(d->pc.position[dim_x], d->pc.position[dim_y] - 1)) {
  //   dir[dim_y] = -1;
  // } else if (charxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y] - 1)) {
  //   dir[dim_y] = -1;
  //   dir[dim_x] = 1;
  // } else if (charxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y])) {
  //   dir[dim_x] = -1;
  // } else if (charxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y])) {
  //   dir[dim_x] = 1;
  // } else if (charxy(d->pc.position[dim_x] - 1, d->pc.position[dim_y] + 1)) {
  //   dir[dim_y] = 1;
  //   dir[dim_x] = -1;
  // } else if (charxy(d->pc.position[dim_x], d->pc.position[dim_y] + 1)) {
  //   dir[dim_y] = 1;
  // } else if (charxy(d->pc.position[dim_x] + 1, d->pc.position[dim_y] + 1)) {
  //   dir[dim_y] = 1;
  //   dir[dim_x] = 1;
  // } else
  //   {
  int valid = 0;
  do
  {
    render_dungeon(d);
    key = getch();
    int limit;
    switch (key)
    {
    case '>':
      // go_down_stairs();
      dir[dim_y] = 0;
      dir[dim_x] = 0;
      valid = 0;
      mvprintw(23, 1, "down Stairs");
      refresh();
      if (mappair(d->pc.position) == ter_stairs_down)
      {
        stairs_move(d);
      }
      break;
    case '<':
      // go_up_stairs();
      dir[dim_y] = 0;
      dir[dim_x] = 0;
      valid = 0;
      mvprintw(23, 1, "up Stairs");
      refresh();
      if (mappair(d->pc.position) == ter_stairs_up)
      {
        stairs_move(d);
      }

      break;
    case 'm':
      limit =0;
      create_monster_list(d);
      print_monster_list(d,limit);
      refresh();
      do
      {
        key = getch();

        if (key == KEY_UP &&  limit >0){
          limit--;
           mvprintw(23,1, "                                 ");
        }
        else if(key == KEY_DOWN && limit < d->num_monsters - 13){
          limit++;
          mvprintw(23,1, "                                 ");

        }
        else{
          if(limit == 0 && key == KEY_UP){
            mvprintw(23,1,"You are at the top!              ");
          }
          else if(limit >= d->num_monsters - 13 && key == KEY_DOWN){
            mvprintw(23,1,"You are at the bottom!           ");
          }
          else{
            mvprintw(23,1,"    Invalid Input                ");
          }
          
        }
        print_monster_list(d,limit);
        refresh();

      }while(key != KEY_ESC);

      clear();
      render_dungeon(d);      
      valid = 1;
      
      break;
    case '7':
    case 'y':
      dir[dim_y] = -1;
      dir[dim_x] = -1;
      valid = 0;
      mvprintw(23, 1, "move upper left");
      refresh();
      break;
    case '8':
    case 'k':
      dir[dim_y] = -1;
      dir[dim_x] = 0;
      valid = 0;
      mvprintw(23, 1, "move up");
      refresh();
      break;
    case '9':
    case 'u':
      dir[dim_y] = -1;
      dir[dim_x] = 1;
      valid = 0;
      mvprintw(23, 1, "move upper right");
      refresh();
      break;
    case '6':
    case 'l':
      dir[dim_y] = 0;
      dir[dim_x] = 1;
      valid = 0;
      mvprintw(23, 1, "move right");
      refresh();
      break;
    case '3':
    case 'n':
      dir[dim_y] = 1;
      dir[dim_x] = 1;
      valid = 0;
      mvprintw(23, 1, "move lower right");
      refresh();
      break;
    case '2':
    case 'j':
      dir[dim_y] = 1;
      dir[dim_x] = 0;
      valid = 0;
      mvprintw(23, 1, "move down");
      refresh();
      break;
    case '1':
    case 'b':
      dir[dim_y] = 1;
      dir[dim_x] = -1;
      valid = 0;
      mvprintw(23, 1, "move lower left");
      refresh();
      break;
    case '4':
    case 'h':
      dir[dim_y] = 0;
      dir[dim_x] = -1;
      valid = 0;
      mvprintw(23, 1, "move left");
      refresh();
      break;
    case '5':
    case ' ':
    case '.':
      dir[dim_y] = 0;
      dir[dim_x] = 0;
      valid = 0;
      mvprintw(23, 1, "Stay");
      refresh();
      break;
    case 'Q':
      d->pc.alive = 0;
      valid = 0;
      mvprintw(23, 1, "Quit");
      refresh();
      break;
    default:
      valid = 1;
      render_dungeon(d);
      mvprintw(23, 1, "Unknown key is called");
      refresh();
      break;
    }
  } while (valid == 1);
  // }

  // static uint32_t have_seen_corner = 0;
  // static uint32_t count = 0;
  // static int target_room = -1;
  // static int target_is_valid = 0;

  // if (target_is_valid &&
  //     (d->pc.position[dim_x] == d->rooms[target_room].position[dim_x]) &&
  //     (d->pc.position[dim_y] == d->rooms[target_room].position[dim_y])) {
  //   target_is_valid = 0;
  // }

  // dir[dim_y] = dir[dim_x] = 0;

  // if (in_corner(d, &d->pc)) {
  //   if (!count) {
  //     count = 1;
  //   }
  //   have_seen_corner = 1;
  // }

  //    else if (!have_seen_corner || count < 250) {
  //   /* Head to a corner and let most of the NPCs kill each other off */
  //   if (count) {
  //     count++;
  //   }
  //   if (!against_wall(d, &d->pc) && ((rand() & 0x111) == 0x111)) {
  //     dir[dim_x] = (rand() % 3) - 1;
  //     dir[dim_y] = (rand() % 3) - 1;
  //   } else {
  //     dir_nearest_wall(d, &d->pc, dir);
  //   }
  // } else {
  //   /* And after we've been there, let's cycle through the rooms, *
  //    * one-by-one, until the game ends                            */
  //   if (target_room == -1) {
  //     target_room = 0;
  //     target_is_valid = 1;
  //   }
  //   if (!target_is_valid) {
  //     target_is_valid = 1;
  //     target_room = (target_room + 1) % d->num_rooms;
  //   }
  //   /* When against the dungeon border, always head toward the target; *
  //    * otherwise, head toward the target with 1/3 probability.         */
  //   if (against_wall(d, &d->pc) || rand_under(1, 3)) {
  //     dir[dim_x] = ((d->pc.position[dim_x] >
  // 	     d->rooms[target_room].position[dim_x]) ? -1 : 1);
  //     dir[dim_y] = ((d->pc.position[dim_y] >
  // 	     d->rooms[target_room].position[dim_y]) ? -1 : 1);
  //   } else {
  //     /* Else we'll choose a random direction */
  //     dir[dim_x] = (rand() % 3) - 1;
  //     dir[dim_y] = (rand() % 3) - 1;
  //   }
  // }

  /* Don't move to an unoccupied location if that places us next to a monster */
  // if (!charxy(d->pc.position[dim_x] + dir[dim_x],
  //             d->pc.position[dim_y] + dir[dim_y]) &&
  //     ((charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
  //              d->pc.position[dim_y] + dir[dim_y] - 1) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
  //               d->pc.position[dim_y] + dir[dim_y] - 1) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
  //              d->pc.position[dim_y] + dir[dim_y]) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
  //               d->pc.position[dim_y] + dir[dim_y]) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
  //              d->pc.position[dim_y] + dir[dim_y] + 1) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x] - 1,
  //               d->pc.position[dim_y] + dir[dim_y] + 1) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x],
  //              d->pc.position[dim_y] + dir[dim_y] - 1) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x],
  //               d->pc.position[dim_y] + dir[dim_y] - 1) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x],
  //              d->pc.position[dim_y] + dir[dim_y] + 1) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x],
  //               d->pc.position[dim_y] + dir[dim_y] + 1) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
  //              d->pc.position[dim_y] + dir[dim_y] - 1) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
  //               d->pc.position[dim_y] + dir[dim_y] - 1) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
  //              d->pc.position[dim_y] + dir[dim_y]) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
  //               d->pc.position[dim_y] + dir[dim_y]) != &d->pc)) ||
  //      (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
  //              d->pc.position[dim_y] + dir[dim_y] + 1) &&
  //       (charxy(d->pc.position[dim_x] + dir[dim_x] + 1,
  //               d->pc.position[dim_y] + dir[dim_y] + 1) != &d->pc)))) {
  //   dir[dim_x] = dir[dim_y] = 0;
  // }

  return 0;
}

uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms) &&
      (d->pc.position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc.position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x])) &&
      (d->pc.position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc.position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y])))
  {
    return 1;
  }

  return 0;
}
