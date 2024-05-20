CXX     = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++20 
LDFLAGS = -lboost_program_options
TARGETS = main kierki-klient kierki-serwer

all: $(TARGETS)

kierki-serwer: kierki-serwer.o common.o err.o game_classes.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

kierki-klient: kierki-klient.o common.o err.o game_classes.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

main: main.o common.o err.o game_classes.o
	$(CXX) $(CXXFLAGS) -o $@ $^

common.o: common.cpp err.h common.h
err.o: err.cpp err.h
game_classes.o: game_classes.cpp game_classes.h card_classes.h \
 constants.h game_enum_types.h
# kierki-klient.o: kierki-klient.cpp
# kierki-serwer.o: kierki-serwer.cpp
main.o: main.cpp card_classes.h constants.h game_enum_types.h

clean:
	rm -f $(TARGETS) *.o