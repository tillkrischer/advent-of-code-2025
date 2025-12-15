CC = clang
CFLAGS = -Wall -Wextra -std=c23

DAYS = day1 day2 day3 day4 day5 day6 day7 day8 day9 day10 day11 day12

.PHONY: all clean $(DAYS)

all: $(DAYS)

# Compile union_find.o separately
union_find.o: union_find.c union_find.h
	$(CC) $(CFLAGS) -c union_find.c -o union_find.o

# Link each day with union_find.o
$(DAYS): %: %.c union_find.o
	$(CC) $(CFLAGS) -o $@ $< union_find.o

clean:
	rm -f $(DAYS) union_find.o
