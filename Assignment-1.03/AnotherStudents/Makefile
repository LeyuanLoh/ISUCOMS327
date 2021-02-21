OBJS	= rogue.o Character/character.o Dungeon/dungeon.o Dungeon/dungeon-disk.o Dungeon/dijkstra.o Helpers/helpers.o Helpers/program-init.o Helpers/tree.o
SOURCE	= rogue.c Character/character.c Dungeon/dungeon.c Dungeon/dungeon-disk.c Dungeon/dijkstra.c Helpers/helpers.c Helpers/program-init.c Helpers/tree.c
HEADER	= Character/character.h Dungeon/dungeon.h Dungeon/dijkstra.h Dungeon/dungeon-disk.h Helpers/helpers.h Helpers/program-init.h Helpers/tree.h Settings/dungeon-parameters.h Settings/print-settings.h Settings/exit-codes.h Settings/misc-settings.h
OUT	= rogue
LFLAGS	 = -std=c11 -g3 -Wall -pedantic -Werror

all: rogue

rogue: $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADER)
	$(CC) -c -o $@ $< $(LFLAGS)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# run the program
run: $(OUT)
	./$(OUT)

# compile program with debugging information
debug: $(OUT)
	valgrind ./$(OUT)

# run program with valgrind for errors
valgrind: $(OUT)
	valgrind ./$(OUT)

# run program with valgrind for leak checks
valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full ./$(OUT)

# run program with valgrind for leak checks (extreme)
valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes ./$(OUT)