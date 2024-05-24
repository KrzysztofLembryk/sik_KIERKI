CXX     = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++20 
LDFLAGS = -lboost_program_options
TARGETS = main kierki-klient kierki-serwer

all: $(TARGETS)

kierki-serwer: kierki-serwer.o common.o err.o  game_classes.o \
exception_wrappers.o card_classes.o read_file.o parameters_handling.o \
game_master.o player_class.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

kierki-klient: kierki-klient.o common.o err.o game_classes.o \
exception_wrappers.o card_classes.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

main: main.o read_file.o game_classes.o exception_wrappers.o card_classes.o
	$(CXX) $(CXXFLAGS) -o $@ $^

card_classes.o: card_classes.cpp card_classes.h enum_types.h constants.h \
 exception_wrappers.h
common.o: common.cpp err.h common.h
err.o: err.cpp err.h
exception_wrappers.o: exception_wrappers.cpp exception_wrappers.h
game_classes.o: game_classes.cpp game_classes.h card_classes.h \
 enum_types.h constants.h exception_wrappers.h
game_master.o: game_master.cpp game_master.h player_class.h \
 game_classes.h card_classes.h enum_types.h
kierki-klient.o: kierki-klient.cpp
kierki-serwer.o: kierki-serwer.cpp parameters_handling.h constants.h \
 exception_wrappers.h read_file.h game_classes.h card_classes.h \
 enum_types.h game_master.h player_class.h
main.o: main.cpp card_classes.h enum_types.h read_file.h game_classes.h
parameters_handling.o: parameters_handling.cpp parameters_handling.h \
 exception_wrappers.h
player_class.o: player_class.cpp player_class.h game_classes.h \
 card_classes.h enum_types.h
read_file.o: read_file.cpp read_file.h game_classes.h card_classes.h \
 enum_types.h exception_wrappers.h constants.h

clean:
	rm -f $(TARGETS) *.o