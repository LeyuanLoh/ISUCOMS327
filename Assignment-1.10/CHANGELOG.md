### 4/6/21
* Comment with //Lee's
* BoSheng - Started working on the assignment.
          - Added one more sttributes for pc gold.
          - Modify pc() and ~pc().
          - Add gold object in object_desc.txt.
          - Modify io_display() to print pc gold value.
          - Modify object.h & object.cpp to add function get_attribute
          - Modify pick_up method to handle pick up gold
          - Modify dungeon.h ,dungeon.cpp and io.cpp to hanle addding new ter_type : ter_store which symbol is '^'
          - Adding case ter_store in every switch in io.cpp
          - Adding inpout '^' to enter store
          - Create new object constructor
          - Add mehtod get_gold_worth for getting the value of every item
          - Done, left with how to decide the gold value of item
          - Add rarity for object to calculate gold value
          - Done with the store and gold new feature. 

BEGIN OBJECT
NAME Ringgit Malaysia
TYPE GOLD
WEIGHT 5+3d6
COLOR YELLOW
DODGE 0+0d1
VAL 0+0d1
DAM 0+0d1
DEF 0+0d1
HIT 0+0d1
SPEED 0+0d1
DESC
I am from Malaysia so ringgi Malaysia must be here, but it jsut worth 
lesser than US dolalr :(
.
ATTR 2+2d6
RRTY 80
ART FALSE
END

BEGIN OBJECT
NAME US dollar
TYPE GOLD
WEIGHT 5+3d6
COLOR CYAN
DODGE 0+0d1
VAL 0+0d1
DAM 0+0d1
DEF 0+0d1
HIT 0+0d1
SPEED 0+0d1
DESC
Most human beings strongly believe that money is way less important than
the life of a human being, but in reality five hundred, fifty, or even 
five dollars are way more important to the lives of most human beings 
than the lives of most human beings
.
ATTR 2+3d6
RRTY 80
ART FALSE
END

### 4/28/2021
* Leyuan - Started to work with different stair level.
         - If you got up (>), then the monster is stronger.
         - If you got down (<), then the monster is weaker.
         - Changed new_dungeon. 
         - Created new method to generate weak monster and stronger monster.
         - Fixed message bug.