#include <cstdlib>
#include <cstring>
#include <ncurses.h>

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "io.h"
#include "object.h"

//Lee's
//array of object type
const char *eq_slot_type[num_eq_slots] = {
    "weapon",
    "offhand",
    "ranged",
    "light",
    "armor",
    "helmet",
    "cloak",
    "gloves",
    "boots",
    "amulet",
    "lh ring",
    "rh ring"};

//initialize
pc::pc()
{
  uint32_t i;

  for (i = 0; i < 10; i++)
  {
    inventory[i] = 0 ;
  }

  for (i = 0; i < num_eq_slots; i++)
  {
    equipment[i] = 0;
  }
}

pc::~pc()
{
  uint32_t i;

  for (i = 0; i < 10; i++)
  {
    if (inventory[i])
    {
      delete inventory[i];
      inventory[i] = NULL;
    }
  }

  for (i = 0; i < num_eq_slots; i++)
  {
    if (equipment[i])
    {
      delete equipment[i];
      equipment[i] = NULL;
    }
  }
}

uint32_t pc_is_alive(dungeon *d)
{
  return d->PC->alive;
}

void place_pc(dungeon *d)
{
  d->PC->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                      (d->rooms->position[dim_y] +
                                       d->rooms->size[dim_y] - 1));
  d->PC->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                      (d->rooms->position[dim_x] +
                                       d->rooms->size[dim_x] - 1));

  pc_init_known_terrain(d->PC);
  pc_observe_terrain(d->PC, d);
}

void config_pc(dungeon *d)
{
  static dice pc_damage_dice(10000, 1, 4);

  //Leyuan
  dice pc_hp(10000, 0, 1);

  d->PC = new pc;

  d->PC->symbol = '@';

  place_pc(d);

  d->PC->speed = PC_SPEED;
  d->PC->alive = 1;
  d->PC->sequence_number = 0;
  d->PC->kills[kill_direct] = d->PC->kills[kill_avenged] = 0;
  d->PC->color.push_back(COLOR_WHITE);
  d->PC->damage = &pc_damage_dice;
  d->PC->name = "Isabella Garcia-Shapiro";
  
   //Leyuan
  d->PC->hp = pc_hp.roll();

  d->character_map[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, d->PC))
  {
    if (!count)
    {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y] - 1))
  {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  }
  else if (charxy(d->PC->position[dim_x], d->PC->position[dim_y] - 1))
  {
    dir[dim_y] = -1;
  }
  else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y] - 1))
  {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  }
  else if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y]))
  {
    dir[dim_x] = -1;
  }
  else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y]))
  {
    dir[dim_x] = 1;
  }
  else if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y] + 1))
  {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  }
  else if (charxy(d->PC->position[dim_x], d->PC->position[dim_y] + 1))
  {
    dir[dim_y] = 1;
  }
  else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y] + 1))
  {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  }
  else if (!have_seen_corner || count < 250)
  {
    /* Head to a corner and let most of the NPCs kill each other off */
    if (count)
    {
      count++;
    }
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111))
    {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    }
    else
    {
      dir_nearest_wall(d, d->PC, dir);
    }
  }
  else
  {
    /* And after we've been there, let's head toward the center of the map. */
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111))
    {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    }
    else
    {
      dir[dim_x] = ((d->PC->position[dim_x] > DUNGEON_X / 2) ? -1 : 1);
      dir[dim_y] = ((d->PC->position[dim_y] > DUNGEON_Y / 2) ? -1 : 1);
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(d->PC->position[dim_x] + dir[dim_x],
              d->PC->position[dim_y] + dir[dim_y]) &&
      ((charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y]) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y]) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x],
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x],
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x],
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x],
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y]) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y]) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC))))
  {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t pc_in_room(dungeon *d, uint32_t room)
{
  if ((room < d->num_rooms) &&
      (d->PC->position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->PC->position[dim_x] < (d->rooms[room].position[dim_x] +
                                 d->rooms[room].size[dim_x])) &&
      (d->PC->position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->PC->position[dim_y] < (d->rooms[room].position[dim_y] +
                                 d->rooms[room].size[dim_y])))
  {
    return 1;
  }

  return 0;
}

void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter)
{
  p->known_terrain[pos[dim_y]][pos[dim_x]] = ter;
  p->visible[pos[dim_y]][pos[dim_x]] = 1;
}

void pc_reset_visibility(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      p->visible[y][x] = 0;
    }
  }
}

terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x)
{
  if (y < 0 || y >= DUNGEON_Y || x < 0 || x >= DUNGEON_X)
  {
    io_queue_message("Invalid value to %s: %d, %d", __FUNCTION__, y, x);
  }

  return p->known_terrain[y][x];
}

void pc_init_known_terrain(pc *p)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      p->known_terrain[y][x] = ter_unknown;
      p->visible[y][x] = 0;
    }
  }
}

void pc_observe_terrain(pc *p, dungeon *d)
{
  pair_t where;
  int16_t y_min, y_max, x_min, x_max;

  y_min = p->position[dim_y] - PC_VISUAL_RANGE;
  if (y_min < 0)
  {
    y_min = 0;
  }
  y_max = p->position[dim_y] + PC_VISUAL_RANGE;
  if (y_max > DUNGEON_Y - 1)
  {
    y_max = DUNGEON_Y - 1;
  }
  x_min = p->position[dim_x] - PC_VISUAL_RANGE;
  if (x_min < 0)
  {
    x_min = 0;
  }
  x_max = p->position[dim_x] + PC_VISUAL_RANGE;
  if (x_max > DUNGEON_X - 1)
  {
    x_max = DUNGEON_X - 1;
  }

  for (where[dim_y] = y_min; where[dim_y] <= y_max; where[dim_y]++)
  {
    where[dim_x] = x_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_x] = x_max;
    can_see(d, p->position, where, 1, 1);
  }
  /* Take one off the x range because we alreay hit the corners above. */
  for (where[dim_x] = x_min - 1; where[dim_x] <= x_max - 1; where[dim_x]++)
  {
    where[dim_y] = y_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_y] = y_max;
    can_see(d, p->position, where, 1, 1);
  }
}

int32_t is_illuminated(pc *p, int16_t y, int16_t x)
{
  return p->visible[y][x];
}

void pc_see_object(character *the_pc, object *o)
{
  if (o)
  {
    o->has_been_seen();
  }
}

//Lee's
uint32_t pc::has_open_inventory_slot()
{
  int i;

  for (i = 0; i < 10; i++)
  {
    if (!inventory[i])
    {
      return 1;
    }
  }
  return 0;
}

uint32_t pc::get_open_inventory_slot()
{
  int i;

  for (i = 0; i < 10; i++)
  {
    if (!inventory[i])
    {
      return i;
    }
  }
  return -1;
}

void pc::pick_up(dungeon *d)
{

  while (d->objmap[position[dim_y]][position[dim_x]] && has_open_inventory_slot())
  {
    object *temp;

    if ((temp = d->objmap[position[dim_y]][position[dim_x]]))
    {
      io_queue_message("You pick up %s.", temp->get_name());
      d->objmap[position[dim_y]][position[dim_x]] = temp->get_next_obj();
      temp->set_next_obj(0);
      inventory[get_open_inventory_slot()] = temp;
    }
  }

  if (d->objmap[position[dim_y]][position[dim_x]] && !has_open_inventory_slot())
  {
    io_queue_message("No more slot to pick up item");
  }
}

uint32_t pc::drop_item_on_slot(dungeon *d, uint32_t slot)
{
  if (!inventory[slot])
  {
    return 0;
  }
  else
  {
    io_queue_message("You drop %s.", inventory[slot]->get_name());
    inventory[slot]->set_next_obj(d->objmap[position[dim_y]][position[dim_x]]);
    d->objmap[position[dim_y]][position[dim_x]] = inventory[slot];
    inventory[slot] = NULL;
    return 1;
  }
}

uint32_t pc::expunge_item_on_slot(dungeon *d, uint32_t slot)
{
  if (!inventory[slot])
  {
    return 0;
  }
  else
  {
    io_queue_message("You expunge %s.", inventory[slot]->get_name());
    uint32_t i;
    for (i = 0; i < d->object_descriptions.size(); i++)
    {
      if (strcmp(d->object_descriptions[i].get_name().c_str(), inventory[slot]->get_name()) == 0)
      {
        d->object_descriptions[i].expunged();
      }
    }
    delete inventory[slot];
    inventory[slot] = NULL;
    return 1;
  }
}

void pc::update_pc_speed()
{
  uint32_t i;
  int32_t default_speed = PC_SPEED;

  for (i = 0; i < num_eq_slots; i++)
  {
    if (equipment[i])
    {
      default_speed += equipment[i]->get_speed();
    }
  }

  //try what will hppen if go to zero; it will get floating point execption lul
  if (default_speed <= 0)
  {
    default_speed = 1;
  }

  speed = default_speed;
}

uint32_t pc::wear_item_on_slot(dungeon *d, uint32_t slot)
{
  int32_t eq_slot;
  object *temp;

  if (!inventory[slot] && !inventory[slot]->can_equip())
  {
    return 0;
  }
  eq_slot = inventory[slot]->get_equipment_slot();

  if (equipment[eq_slot])
  {
    if (equipment[eq_slot]->get_type() == objtype_RING)
    { //always swap the left ring first
      if (!equipment[eq_slot + 1])
      {
        eq_slot++;
      }
      else
      {
        io_queue_message("You are switching %s.", equipment[eq_slot]->get_name());
      }
    }
    else
    {
      io_queue_message("You are switching %s.", equipment[eq_slot]->get_name());
    }
  }

  temp = equipment[eq_slot];
  equipment[eq_slot] = inventory[slot];
  inventory[slot] = temp;

  io_queue_message("You are wearing %s.", equipment[eq_slot]->get_name());

  update_pc_speed();

  return 1;
}

uint32_t pc::take_off_item_on_slot(dungeon *d, uint32_t slot)
{
  if (!equipment[slot])
  {
    return 0;
  }

  if (!has_open_inventory_slot())
  {
    io_queue_message("Your inventory is full, the item is drop on floor.");
    equipment[slot]->set_next_obj(d->objmap[position[dim_y]][position[dim_x]]);
    d->objmap[position[dim_y]][position[dim_x]] = equipment[slot];
    equipment[slot] = NULL;
  }
  else
  {
    io_queue_message("Your are taking off %s", equipment[slot]->get_name());
    inventory[get_open_inventory_slot()] = equipment[slot];
    equipment[slot] = NULL;
  }

  update_pc_speed();

  return 1;
}
