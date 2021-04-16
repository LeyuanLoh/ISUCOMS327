#ifndef PC_H
#define PC_H

#include <stdint.h>

#include "dims.h"
#include "character.h"
#include "dungeon.h"

//Lee's: an enum for object type
//Follow the order of the object_type_t
typedef enum equip_slot
{
  weapon_slot,
  offhand_slot,
  ranged_slot,
  light_slot,
  armor_slot,
  helmet_slot,
  cloak_slot,
  gloves_slot,
  boots_slot,
  amulet_slot,
  lring_slot,
  rring_slot,
  num_eq_slots
} equip_slot_t;

extern const char *eq_slot_type[num_eq_slots];

class pc : public character
{

public:
  pc();
  ~pc();
  terrain_type known_terrain[DUNGEON_Y][DUNGEON_X];
  uint8_t visible[DUNGEON_Y][DUNGEON_X];

  //Lee's
  object *equipment[10];
  object *inventory[10];
  void pick_up(dungeon *d);
  void update_pc_speed();
  uint32_t drop_item_on_slot(dungeon *d, uint32_t slot);
  uint32_t expunge_item_on_slot(dungeon *d, uint32_t slot);
  uint32_t has_open_inventory_slot();
  uint32_t get_open_inventory_slot();
  uint32_t wear_item_on_slot(dungeon *d, uint32_t slot);
  uint32_t take_off_item_on_slot(dungeon *d, uint32_t slot);

};

void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, uint32_t room);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter);
terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon *d);
int32_t is_illuminated(pc *p, int16_t y, int16_t x);
void pc_reset_visibility(pc *p);

#endif
