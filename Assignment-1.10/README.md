Leyuan Loh:  leyuan <br />
Bo Sheng Lee: lee0717 <br /> <br />

For this assignment, we decide to add the feature of the gold object and the store. To able to implement this feature, we add two gold object in the object description list, 

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
I am from Malaysia so ringgit Malaysia must be here, but it just worth 
lesser than US dolar :(
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

So, now they will be created and randomly places on the map just like the other object. At the beginning of the game, the player will have 0 gold. The player need to pick upo the gold the be able to buy item from the store. Every dungeon level will or will not have a store. The store is randomly place on the mpa and only in a room. The symbol of a store is '^'. To enter the store, the player need to step on the store and input '^'. Then , the store will randomly generate 3-6 item from the desctiion list. every store will have different item. And the player only allow to buy on item from the store. The store will also disappear after the player purchase one item or leave the store without pruchasing. The price of each item is set according to their rarity, so the price will be higher if their rarity is higher.