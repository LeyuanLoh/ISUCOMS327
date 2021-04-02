#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/stdc++.h>

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "utils.h"
#include "io.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
const char *victory =
    "\n                                       o\n"
    "                                      $\"\"$o\n"
    "                                     $\"  $$\n"
    "                                      $$$$\n"
    "                                      o \"$o\n"
    "                                     o\"  \"$\n"
    "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
    "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
    "\"oo   o o o o\n"
    "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
    "   \"o$$\"    o$$\n"
    "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
    "     o\"\"\n"
    "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
    "   o\"\n"
    "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
    "\"$$$  $\n"
    "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
    "\"\"      \"\"\" \"\n"
    "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
    "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
    "\"$$$$\n"
    "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
    "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
    "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
    "              $\"                                                 \"$\n"
    "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
    "$\"$\"$\"$\"$\"$\"$\"$\n"
    "                                   You win!\n\n";

const char *tombstone =
    "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
    "               /     /         \\             __\n"
    "              /     /           \\            ||\n"
    "             /____ /   Rest in   \\           ||\n"
    "            |     |    Pieces     |          ||\n"
    "            |     |               |          ||\n"
    "            |     |   A. Luser    |          ||\n"
    "            |     |               |          ||\n"
    "            |     |     * *   * * |         _||_\n"
    "            |     |     *\\/* *\\/* |        | TT |\n"
    "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
    "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
    "            |     |         \\/..\"\"\"\"\"...\"\"\""
    "\\ || /.\"\"\".......\"\"\"\"...\n"
    "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
    "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
    "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
    "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
    "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <count>]\n",
          name);

  exit(-1);
}

// int main(int argc, char *argv[])
// {
//   dungeon d;
//   time_t seed;
//   struct timeval tv;
//   int32_t i;
//   uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
//   uint32_t long_arg;
//   char *save_file;
//   char *load_file;
//   char *pgm_file;

//   /* Quiet a false positive from valgrind. */
//   memset(&d, 0, sizeof(d));

//   /* Default behavior: Seed with the time, generate a new dungeon, *
//    * and don't write to disk.                                      */
//   do_load = do_save = do_image = do_save_seed = do_save_image = 0;
//   do_seed = 1;
//   save_file = load_file = NULL;
//   d.max_monsters = MAX_MONSTERS;

//   /* The project spec requires '--load' and '--save'.  It's common  *
//    * to have short and long forms of most switches (assuming you    *
//    * don't run out of letters).  For now, we've got plenty.  Long   *
//    * forms use whole words and take two dashes.  Short forms use an *
//     * abbreviation after a single dash.  We'll add '--rand' (to     *
//    * specify a random seed), which will take an argument of it's    *
//    * own, and we'll add short forms for all three commands, '-l',   *
//    * '-s', and '-r', respectively.  We're also going to allow an    *
//    * optional argument to load to allow us to load non-default save *
//    * files.  No means to save to non-default locations, however.    *
//    * And the final switch, '--image', allows me to create a dungeon *
//    * from a PGM image, so that I was able to create those more      *
//    * interesting test dungeons for you.                             */

//   if (argc > 1)
//   {
//     for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0)
//     {
//       if (argv[i][0] == '-')
//       { /* All switches start with a dash */
//         if (argv[i][1] == '-')
//         {
//           argv[i]++;    /* Make the argument have a single dash so we can */
//           long_arg = 1; /* handle long and short args at the same place.  */
//         }
//         switch (argv[i][1])
//         {
//         case 'n':
//           if ((!long_arg && argv[i][2]) ||
//               (long_arg && strcmp(argv[i], "-nummon")) ||
//               argc < ++i + 1 /* No more arguments */ ||
//               !sscanf(argv[i], "%hu", &d.max_monsters))
//           {
//             usage(argv[0]);
//           }
//           break;
//         case 'r':
//           if ((!long_arg && argv[i][2]) ||
//               (long_arg && strcmp(argv[i], "-rand")) ||
//               argc < ++i + 1 /* No more arguments */ ||
//               !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */)
//           {
//             usage(argv[0]);
//           }
//           do_seed = 0;
//           break;
//         case 'l':
//           if ((!long_arg && argv[i][2]) ||
//               (long_arg && strcmp(argv[i], "-load")))
//           {
//             usage(argv[0]);
//           }
//           do_load = 1;
//           if ((argc > i + 1) && argv[i + 1][0] != '-')
//           {
//             /* There is another argument, and it's not a switch, so *
//              * we'll treat it as a save file and try to load it.    */
//             load_file = argv[++i];
//           }
//           break;
//         case 's':
//           if ((!long_arg && argv[i][2]) ||
//               (long_arg && strcmp(argv[i], "-save")))
//           {
//             usage(argv[0]);
//           }
//           do_save = 1;
//           if ((argc > i + 1) && argv[i + 1][0] != '-')
//           {
//             /* There is another argument, and it's not a switch, so *
//              * we'll save to it.  If it is "seed", we'll save to    *
// 	     * <the current seed>.rlg327.  If it is "image", we'll  *
// 	     * save to <the current image>.rlg327.                  */
//             if (!strcmp(argv[++i], "seed"))
//             {
//               do_save_seed = 1;
//               do_save_image = 0;
//             }
//             else if (!strcmp(argv[i], "image"))
//             {
//               do_save_image = 1;
//               do_save_seed = 0;
//             }
//             else
//             {
//               save_file = argv[i];
//             }
//           }
//           break;
//         case 'i':
//           if ((!long_arg && argv[i][2]) ||
//               (long_arg && strcmp(argv[i], "-image")))
//           {
//             usage(argv[0]);
//           }
//           do_image = 1;
//           if ((argc > i + 1) && argv[i + 1][0] != '-')
//           {
//             /* There is another argument, and it's not a switch, so *
//              * we'll treat it as a save file and try to load it.    */
//             pgm_file = argv[++i];
//           }
//           break;
//         default:
//           usage(argv[0]);
//         }
//       }
//       else
//       { /* No dash */
//         usage(argv[0]);
//       }
//     }
//   }

//   if (do_seed)
//   {
//     /* Allows me to generate more than one dungeon *
//      * per second, as opposed to time().           */
//     gettimeofday(&tv, NULL);
//     seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
//   }

//   srand(seed);

//
//   // io_init_terminal();
//   // init_dungeon(&d);

//   // if (do_load) {
//   //read_dungeon(&d, load_file);
//   // } else if (do_image) {
//   //   read_pgm(&d, pgm_file);
//   // } else {
//   //   gen_dungeon(&d);
//   // }

//   // /* Ignoring PC position in saved dungeons.  Not a bug. */
//   // config_pc(&d);
//   // gen_monsters(&d);

//   // io_display(&d);
//   // if (!do_load && !do_image) {
//   //   io_queue_message("Seed is %u.", seed);
//   // }
//   // while (pc_is_alive(&d) && dungeon_has_npcs(&d) && !d.quit) {
//   //   do_moves(&d);
//   // }
//   // io_display(&d);

//   // io_reset_terminal();

//   // if (do_save) {
//   //   if (do_save_seed) {
//   //      /* 10 bytes for number, plus dot, extention and null terminator. */
//   //     save_file = (char *) malloc(18);
//   //     sprintf(save_file, "%ld.rlg327", seed);
//   //   }
//   //   if (do_save_image) {
//   //     if (!pgm_file) {
//   // fprintf(stderr, "No image file was loaded.  Using default.\n");
//   // do_save_image = 0;
//   //     } else {
//   // /* Extension of 3 characters longer than image extension + null. */
//   // save_file = (char *) malloc(strlen(pgm_file) + 4);
//   // strcpy(save_file, pgm_file);
//   // strcpy(strchr(save_file, '.') + 1, "rlg327");
//   //     }
//   //   }
//   //   write_dungeon(&d, save_file);

//   //   if (do_save_seed || do_save_image) {
//   //     free(save_file);
//   //   }
//   // }

//   // printf("%s", pc_is_alive(&d) ? victory : tombstone);
//   // printf("You defended your life in the face of %u deadly beasts.\n"
//   //        "You avenged the cruel and untimely murders of %u "
//   //        "peaceful dungeon residents.\n",
//   //        d.PC->kills[kill_direct], d.PC->kills[kill_avenged]);

//   // if (pc_is_alive(&d)) {
//   //   /* If the PC is dead, it's in the move heap and will get automatically *
//   //    * deleted when the heap destructs.  In that case, we can't call       *
//   //    * delete_pc(), because it will lead to a double delete.               */
//   //   character_delete(d.PC);
//   // }

//   // delete_dungeon(&d);

//   return 0;
// }

class new_monster
{
public:
  string name;
  string desc;
  vector<int> color;
  bitset<9> abil;
  dice hp;
  dice speed;
  dice dam;
  string symb;
  int rrty;

  new_monster()
  {
    name = "";
    desc = "";
    symb = "";
    rrty = 0;
  }

void printColor(){

    for(size_t i = 0; i < color.size();i++)
    {
      if (color[i] == 1)
      {
        cout << "RED";
      }
      else if(color[i]== 2)
      {
        cout << "GREEN";
      }
      else if( color[i] ==3)
      {
        cout << "YELLOW";
      }
      else if( color[i]==4)
      {
        cout << "BLUE";
      }
      else if(color[i] ==5)
      {
        cout << "MAGENTA";
      }
      else if(color[i]== 6)
      {
        cout << "CYAN";
      }
      else if(color[i] == 7)
      {
        cout << "WHITE";
      }
      else
      {
        cout << "BLACK";
      }

      cout << " ";
    }
    cout << endl;
}

void printAbil()
{
  bitset<9> temp(NPC_SMART);
  if( (abil & temp) == NPC_SMART){
    cout << "SMART ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_TELEPATH){
    cout << "TELE ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_TUNNEL){
    cout << "TUNNEL ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_ERRATIC){
    cout << "ERRATIC ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_PASS){
    cout << "PASS ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_PICKUP){
    cout << "PICKUP ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_DESTROY){
    cout << "DESTROY ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_UNIQ){
    cout << "UNIQ ";
  }
  temp <<= 1;
  if( (abil & temp) == NPC_BOSS){
    cout << "BOSS ";
  }
  cout << "\n";
  
}

};

//Lee's
//We are using the method from https://www.geeksforgeeks.org/split-string-substrings-using-delimiter/. 
//The method simple split a string into a vector.
vector<string> splitStrings(string str, char dl)
{
    string word = "";
  
    // adding delimiter character at the end
    // of 'str'
    str = str + dl;
  
    // length of 'str'
    int l = str.size();
  
    // traversing 'str' from left to right
    vector<string> substr_list;
    for (int i = 0; i < l; i++) {
  
        // if str[i] is not equal to the delimiter
        // character then accumulate it to 'word'
        if (str[i] != dl)
            word = word + str[i];
  
        else {
  
            // if 'word' is not an empty string,
            // then add this 'word' to the array
            // 'substr_list[]'
            if ((int)word.size() != 0)
                substr_list.push_back(word);
  
            // reset 'word'
            word = "";
        }
    }
  
    // return the splitted strings
    return substr_list;
}


//@author: Leyuan Loh
//its better to create a new main method.
int main(int argv, char *argc[])
{
  const char *home;
  if (!(home = getenv("HOME")))
  {
    fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
    home = ".";
  }

  char *filename;
  size_t len = (strlen(home) + strlen(".rlg327") + strlen("monster_desc.txt") +
                1 /* The NULL terminator */ +
                2 /* The slashes */);

  filename = (char *)malloc(len * sizeof(*filename));
  sprintf(filename, "%s/%s/%s", home, SAVE_DIR, "monster_desc.txt");
  string line;
  ifstream f(filename);
  vector<new_monster> monsList;
  if (f.is_open())
  {

    //Metadata for versioning
    //If it fails to match, you may terminate the program.
    if (!(line.compare("RLG327 MONSTER DESCRIPTION 1")))
    {
      cout << "Wrong meta" << endl;
      exit(1);
    }

    //Start really reading.
    while (getline(f, line))
    {
      if (line.compare("BEGIN MONSTER") == 0)
      {
        bool wrong = false;
        bool nameFoo = false;
        bool descFoo = false;
        bool colorFoo = false;
        bool speedFoo = false;
        bool abilFoo = false;
        bool hpFoo = false;
        bool damFoo = false;
        bool symbFoo = false;
        bool rrtyFoo = false;

        new_monster monster;

        getline(f, line);
        //read until end.
        while ((line.compare("END") != 0))
        {
          if (wrong)
          {
            getline(f, line);
            continue;
          }
          int pos = line.find(" ");
          string keyword = line.substr(0, pos);
          if (keyword.compare("DESC") != 0 && pos == -1)
          {
            wrong = true;
            continue;
          }
          if (keyword.compare("NAME") == 0)
          {
            if (nameFoo)
            {
              wrong = true;
              continue;
            }
            monster.name = line.substr(pos + 1, line.length());
            nameFoo = true;
          }
          else if (keyword.compare("COLOR") == 0)
          {
            if (colorFoo)
            {
              wrong = true;
              continue;
            }
            string colorStr = line.substr(pos + 1, line.length());
            vector<string> colorVector = splitStrings(colorStr,' ');
            
            for(size_t i = 0; i < colorVector.size();i++)
            {
                if (colorVector[i].compare("RED") == 0)
                {
                  monster.color.push_back(1);
                }
                else if (colorVector[i].compare("GREEN") == 0)
                {
                  monster.color.push_back(2);
                }
                else if (colorVector[i].compare("BLUE") == 0)
                {
                  monster.color.push_back(4);
                }
                else if (colorVector[i].compare("CYAN") == 0)
                {
                  monster.color.push_back(6);
                }
                else if (colorVector[i].compare("YELLOW") == 0)
                {
                  monster.color.push_back(3);
                }
                else if (colorVector[i].compare("MAGENTA") == 0)
                {
                  monster.color.push_back(5);
                }
                else if (colorVector[i].compare("WHITE") == 0)
                {
                  monster.color.push_back(7);
                }
                else
                {
                  monster.color.push_back(0);
                }
            }

            
            colorFoo = true;
          }
          else if (keyword.compare("DESC") == 0)
          {

            if (descFoo)
            {
              wrong = true;
              continue;
            }
            getline(f, line);
            string cand = "";
            while (line.compare(".") != 0 && wrong != true)
            {
              if(line.length() > 77){
                wrong = true;
                continue;
              }
              monster.desc += line + "\n";
              getline(f, line);
            }
            //remove last \n from cand
            monster.desc = monster.desc.substr(0, cand.length() - 1);
            descFoo = true;
          }
          else if (keyword.compare("SPEED") == 0)
          {
            if (speedFoo)
            {
              wrong = true;
              continue;
            }
            int temp = line.find("+");
            monster.speed.base = stoi(line.substr(pos + 1, temp));
            int temp1 = line.find("d");
            monster.speed.roll = stoi(line.substr(temp + 1, temp1));
            monster.speed.sides = stoi(line.substr(temp1 + 1, line.length()));
            speedFoo = true;
          }
          else if (keyword.compare("ABIL") == 0)
          {
            if (abilFoo)
            {
              wrong = true;
              continue;
            }
            // monster.abil = line.substr(pos + 1, line.length());
            string abilStr = line.substr(pos + 1, line.length());
            vector<string> abilVector = splitStrings(abilStr,' ');
            
            for(size_t i = 0; i < abilVector.size();i++)
            {

                if (abilVector[i].compare("SMART") == 0)
                {
                  monster.abil |= NPC_SMART;
                }
                else if (abilVector[i].compare("TELE") == 0)
                {
                  monster.abil |= NPC_TELEPATH;
                }
                else if (abilVector[i].compare("TUNNEL") == 0)
                {
                  monster.abil |= NPC_TUNNEL;
                }
                else if (abilVector[i].compare("ERRATIC") == 0)
                {
                  monster.abil |= NPC_ERRATIC;
                }
                else if (abilVector[i].compare("PASS") == 0)
                {
                  monster.abil |= NPC_PASS;
                }
                else if (abilVector[i].compare("PICKUP") == 0)
                {
                  monster.abil |= NPC_PICKUP;
                }
                else if (abilVector[i].compare("DESTROY") == 0)
                {
                  monster.abil |= NPC_DESTROY;
                }
                else if (abilVector[i].compare("UNIQ") == 0)
                {
                  monster.abil |= NPC_UNIQ;
                }
                else
                {
                  monster.abil |= NPC_BOSS;
                }
            }
            abilFoo = true;
          }
          else if (keyword.compare("HP") == 0)
          {
            if (hpFoo)
            {
              wrong = true;
              continue;
            }
            int temp = line.find("+");
            monster.hp.base = stoi(line.substr(pos + 1, temp));
            int temp1 = line.find("d");
            monster.hp.roll = stoi(line.substr(temp + 1, temp1));
            monster.hp.sides = stoi(line.substr(temp1 + 1, line.length()));
            hpFoo = true;
          }
          else if (keyword.compare("DAM") == 0)
          {
            if (damFoo)
            {
              wrong = true;
              continue;
            }
            int temp = line.find("+");
            monster.dam.base = stoi(line.substr(pos + 1, temp));
            int temp1 = line.find("d");
            monster.dam.roll = stoi(line.substr(temp + 1, temp1));
            monster.dam.sides = stoi(line.substr(temp1 + 1, line.length()));
            damFoo = true;
          }
          else if (keyword.compare("SYMB") == 0)
          {
            if (symbFoo)
            {
              wrong = true;
              continue;
            }
            monster.symb = line.substr(pos + 1, line.length());
            symbFoo = true;
          }
          else if (keyword.compare("RRTY") == 0)
          {
            if (rrtyFoo)
            {
              wrong = true;
              continue;
            }
            monster.rrty = stoi(line.substr(pos + 1, line.length()));
            rrtyFoo = true;
          }
          else
          {
            //unknown field.
            wrong = true;
          }
          getline(f, line);
        }
        if (!wrong && nameFoo && descFoo && colorFoo && speedFoo && abilFoo && hpFoo && damFoo && symbFoo && rrtyFoo)
        {
          if (monster.desc.at(monster.desc.length() - 1) == '\n')
          {
            monster.desc = monster.desc.substr(0, monster.desc.length() - 1);
          }
          monsList.push_back(monster);
        }
      }
    }
  }
  else
  {
    cout << "Ohh ohh. Something wrong.";
  }
  f.close();

  //Now output

  for (size_t i = 0; i < monsList.size(); i++)
  {
    new_monster mon = monsList[i];
    cout << mon.name << endl;

    //change here
    //not sure need to output the orignal description or the actual description.
    cout << mon.desc << endl;
    mon.printColor(); //Lee's
    cout << mon.speed.toString() << endl;
    mon.printAbil();
    cout << mon.hp.toString() << endl;
    cout << mon.dam.toString() << endl;
    cout << mon.symb << endl;
    cout << mon.rrty << endl
         << endl;
  }
}