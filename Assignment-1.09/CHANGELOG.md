### 4/6/21
* Comment with //Lee's
* BoSheng - Started working on the assignment.
          - Add two array (equipment and inventory) and enum (equip_slot for future use)) in pc.h 
          - Cretae construcotr for pc to initialize the two array in pc.cpp
          - Create function get_next and set_next to set and get the next object on a floor object.h
          - To implement auto pick up add some line of code in move.cpp
          - Add pickup,expunge,drop function for pc in pc.cpp
          - Add more assisting function like getSlot(), haveSlot in pc.cpp
          - Add one properties in descriptions.h
          - Add more cases in io.cpp
          - Add more function for corresponding cases in io.cpp
          - Add cases 'w' to print equipment
          - Left wtih case 't', and 'e'
          - Last edit was on 1262 io.cpp which is to wear item
          - For wear and take off most probably u need to add functions in pc.cpp where wear only for object weapon tp ring, you can use the object_type in descriptions.h to help differenttiate.
* Leyuan Loh - modified io_handle_input() in io.cpp
             - wear_item() in pc.cpp
### 4/15/21
* Leyuan Loh - Working on combat system.
             - Implemented the combat system. (move.cpp)

