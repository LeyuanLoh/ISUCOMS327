#ifndef DESCRIPTIONS_H
#define DESCRIPTIONS_H

#include <stdint.h>
#undef swap
#include <vector>
#include <string>
#include "dice.h"
#include "npc.h"

typedef struct dungeon dungeon_t;

uint32_t parse_descriptions(dungeon_t *d);
uint32_t print_descriptions(dungeon_t *d);
uint32_t destroy_descriptions(dungeon_t *d);

typedef enum object_type
{
  objtype_no_type,
  objtype_WEAPON,
  objtype_OFFHAND,
  objtype_RANGED,
  objtype_LIGHT,
  objtype_ARMOR,
  objtype_HELMET,
  objtype_CLOAK,
  objtype_GLOVES,
  objtype_BOOTS,
  objtype_AMULET,
  objtype_RING,
  objtype_SCROLL,
  objtype_BOOK,
  objtype_FLASK,
  objtype_GOLD,
  objtype_AMMUNITION,
  objtype_FOOD,
  objtype_WAND,
  objtype_CONTAINER
} object_type_t;

extern const char object_symbol[];

class monster_description
{
private:
  std::string name, description;
  char symbol;
  std::vector<uint32_t> color;
  uint32_t abilities;
  dice speed, hitpoints, damage;
  uint32_t rarity;

public:
  monster_description() : name(), description(), symbol(0), color(0),
                          abilities(0), speed(), hitpoints(), damage(),
                          rarity(0)
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const char symbol,
           const std::vector<uint32_t> &color,
           const dice &speed,
           const uint32_t abilities,
           const dice &hitpoints,
           const dice &damage,
           const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  char get_symbol() { return symbol; }
  std::string get_name() { return name; }

  //Lee's
  npc *generate_npc()
  {

    npc *gen_monster;

    gen_monster = new npc;

    gen_monster->name = name;
    gen_monster->description = description;
    gen_monster->symbol = symbol;
    gen_monster->color = color;
    gen_monster->abilities = abilities;
    gen_monster->speed = speed.roll();
    gen_monster->hitpoints = hitpoints.roll();
    gen_monster->damage = damage;
    gen_monster->rarity = rarity;

    return gen_monster;
  }
};

//Leyuan
class real_object
{
public:
  std::string name;
  std::string description;
  object_type_t type;
  uint32_t color;
  int32_t hit;
  dice damage;
  int32_t dodge;
  int32_t defence;
  int32_t weight;
  int32_t speed;
  int32_t attribute;
  int32_t value;
  bool artifact;
  uint32_t rarity;
  pair_t position;

  real_object()
  {
    name = "";
    description = "";
    type = objtype_no_type;
  }
};

class object_description
{
private:
  std::string name, description;
  object_type_t type;
  uint32_t color;
  dice hit, damage, dodge, defence, weight, speed, attribute, value;
  bool artifact;
  uint32_t rarity;

public:
  object_description() : name(), description(), type(objtype_no_type),
                         color(0), hit(), damage(),
                         dodge(), defence(), weight(),
                         speed(), attribute(), value(),
                         artifact(false), rarity(0)
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const object_type_t type,
           const uint32_t color,
           const dice &hit,
           const dice &damage,
           const dice &dodge,
           const dice &defence,
           const dice &weight,
           const dice &speed,
           const dice &attrubute,
           const dice &value,
           const bool artifact,
           const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  /* Need all these accessors because otherwise there is a *
   * circular dependancy that is difficult to get around.  */
  inline const std::string &get_name() const { return name; }
  inline const std::string &get_description() const { return description; }
  inline const object_type_t get_type() const { return type; }
  inline const uint32_t get_color() const { return color; }
  inline const dice &get_hit() const { return hit; }
  inline const dice &get_damage() const { return damage; }
  inline const dice &get_dodge() const { return dodge; }
  inline const dice &get_defence() const { return defence; }
  inline const dice &get_weight() const { return weight; }
  inline const dice &get_speed() const { return speed; }
  inline const dice &get_attribute() const { return attribute; }
  inline const dice &get_value() const { return value; }

  //Leyuan
  real_object *generate_obj()
  {
    real_object *gen_obj;
    gen_obj = new real_object;
    gen_obj->name = name;
    gen_obj->description = description;
    gen_obj->type = type;
    gen_obj->color = color;
    gen_obj->hit = hit.roll();
    gen_obj->damage = damage;
    gen_obj->dodge = dodge.roll();
    gen_obj->defence = defence.roll();
    gen_obj->weight = weight.roll();
    gen_obj->speed = speed.roll();
    gen_obj->attribute = attribute.roll();
    gen_obj->value = value.roll();
    gen_obj->rarity = rarity;
    gen_obj->artifact = artifact;
    return gen_obj;
  }
};

void gen_object(dungeon *d);
std::ostream &operator<<(std::ostream &o, monster_description &m);
std::ostream &operator<<(std::ostream &o, object_description &od);

#endif
