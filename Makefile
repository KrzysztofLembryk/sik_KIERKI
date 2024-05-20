CC     = g++
CFLAGS = -Wall -Wextra -O2 -std=c++20
TARGETS = main

all: $(TARGETS)

common.o: common.cpp err.h common.h
err.o: err.cpp err.h
game_classes.o: game_classes.cpp game_classes.h card_classes.h \
 constants.h game_enum_types.h
main.o: main.cpp card_classes.h constants.h game_enum_types.h


clean:
	rm -f $(TARGETS) *.o