### 4/6/21
* Comment with //Lee's
* BoSheng - Started working on the assignment.\
          - Add new method to generate_monster (desciptions.h)\
          - Add new member in npc (npc.h)\
          - Modify the io_display to print color symbol (io.cpp)\
          - Modify generate_monster function (npc.cpp)\
          - Modify do_combat function to add killed unique monster into the list (move.cpp)\
          - Modify new_dungeon function to reset unique monster list (dungeon.cpp)\
          - Add more member in deungeon class (dunegon.h)\
### 4/8/21
* Comment with //Leyuan
* Leyuan - Working on placing the object.
         - Added new class real_object and added a new function generate_obj() for object_description in descriptions.h.
         - Added function gen_object in descriptions.cpp
         - Modified io_display_no_fog in io.cpp
         - Modified io_display in io.cpp
         - Fixed bug in teleportation mode where the color is incorrect and object dissapeared (io_teleport_pc io.cpp ). 
         - Modified delete_dungeon() and new_dungeon() in dungeon.cpp