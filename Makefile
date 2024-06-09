CXX     = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++20 -pthread
LDFLAGS = -lboost_program_options
TARGETS = kierki-klient kierki-serwer

all: $(TARGETS)

kierki-serwer: kierki-serwer.o common.o err.o  game_classes.o \
exception_wrappers.o card_classes.o read_file.o parameters_handling.o \
game_master.o player_class.o init_comm_wrappers.o \
TCP_handler.o ingame_comm_wrappers.o polls_func.o \
socket_fd_handler.o TCP_threads.o player_threads.o btwn_thread_comm.o \
address_wrapper_cls.o resend_lib.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

kierki-klient: kierki-klient.o common.o err.o game_classes.o \
exception_wrappers.o card_classes.o init_comm_wrappers.o read_file.o \
socket_fd_handler.o TCP_handler.o ingame_comm_wrappers.o \
parameters_handling.o polls_func.o btwn_thread_comm.o TCP_threads.o \
player_class.o player_threads.o game_master.o klient_auto_lib.o \
address_wrapper_cls.o resend_lib.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


TCP_handler.o: TCP_handler.cpp TCP_handler.h err.h exception_wrappers.h \
 common.h enum_types.h game_master.h player_class.h game_classes.h \
 card_classes.h address_wrapper_cls.h socket_fd_handler.h constants.h
TCP_threads.o: TCP_threads.cpp TCP_threads.h game_master.h player_class.h \
 game_classes.h card_classes.h enum_types.h exception_wrappers.h \
 address_wrapper_cls.h socket_fd_handler.h constants.h polls_func.h \
 ingame_comm_wrappers.h init_comm_wrappers.h TCP_handler.h err.h common.h \
 resend_lib.h
address_wrapper_cls.o: address_wrapper_cls.cpp address_wrapper_cls.h \
 constants.h exception_wrappers.h
btwn_thread_comm.o: btwn_thread_comm.cpp btwn_thread_comm.h constants.h \
 exception_wrappers.h
card_classes.o: card_classes.cpp card_classes.h enum_types.h \
 exception_wrappers.h constants.h
common.o: common.cpp err.h common.h enum_types.h game_master.h \
 player_class.h game_classes.h card_classes.h exception_wrappers.h \
 address_wrapper_cls.h socket_fd_handler.h constants.h
err.o: err.cpp err.h
exception_wrappers.o: exception_wrappers.cpp exception_wrappers.h
game_classes.o: game_classes.cpp game_classes.h card_classes.h \
 enum_types.h exception_wrappers.h constants.h
game_master.o: game_master.cpp game_master.h player_class.h \
 game_classes.h card_classes.h enum_types.h exception_wrappers.h \
 address_wrapper_cls.h socket_fd_handler.h constants.h
ingame_comm_wrappers.o: ingame_comm_wrappers.cpp ingame_comm_wrappers.h \
 card_classes.h enum_types.h exception_wrappers.h common.h game_master.h \
 player_class.h game_classes.h address_wrapper_cls.h socket_fd_handler.h \
 TCP_handler.h constants.h err.h
init_comm_wrappers.o: init_comm_wrappers.cpp init_comm_wrappers.h \
 card_classes.h enum_types.h exception_wrappers.h constants.h common.h \
 game_master.h player_class.h game_classes.h address_wrapper_cls.h \
 socket_fd_handler.h err.h TCP_handler.h
kierki-klient.o: kierki-klient.cpp common.h enum_types.h game_master.h \
 player_class.h game_classes.h card_classes.h exception_wrappers.h \
 address_wrapper_cls.h socket_fd_handler.h init_comm_wrappers.h \
 constants.h ingame_comm_wrappers.h TCP_handler.h err.h \
 parameters_handling.h klient_auto_lib.h
kierki-serwer.o: kierki-serwer.cpp parameters_handling.h enum_types.h \
 constants.h exception_wrappers.h read_file.h game_classes.h \
 card_classes.h game_master.h player_class.h address_wrapper_cls.h \
 socket_fd_handler.h init_comm_wrappers.h ingame_comm_wrappers.h \
 player_threads.h polls_func.h common.h err.h
klient_auto_lib.o: klient_auto_lib.cpp klient_auto_lib.h enum_types.h \
 address_wrapper_cls.h init_comm_wrappers.h card_classes.h \
 exception_wrappers.h constants.h ingame_comm_wrappers.h TCP_handler.h \
 common.h game_master.h player_class.h game_classes.h socket_fd_handler.h
parameters_handling.o: parameters_handling.cpp parameters_handling.h \
 enum_types.h exception_wrappers.h err.h constants.h common.h \
 game_master.h player_class.h game_classes.h card_classes.h \
 address_wrapper_cls.h socket_fd_handler.h
player_class.o: player_class.cpp player_class.h game_classes.h \
 card_classes.h enum_types.h exception_wrappers.h address_wrapper_cls.h \
 socket_fd_handler.h constants.h
player_threads.o: player_threads.cpp player_threads.h socket_fd_handler.h \
 game_master.h player_class.h game_classes.h card_classes.h enum_types.h \
 exception_wrappers.h address_wrapper_cls.h ingame_comm_wrappers.h \
 constants.h btwn_thread_comm.h TCP_threads.h
polls_func.o: polls_func.cpp polls_func.h exception_wrappers.h \
 constants.h err.h
read_file.o: read_file.cpp read_file.h game_classes.h card_classes.h \
 enum_types.h exception_wrappers.h constants.h common.h game_master.h \
 player_class.h address_wrapper_cls.h socket_fd_handler.h
resend_lib.o: resend_lib.cpp resend_lib.h game_master.h player_class.h \
 game_classes.h card_classes.h enum_types.h exception_wrappers.h \
 address_wrapper_cls.h socket_fd_handler.h constants.h \
 ingame_comm_wrappers.h init_comm_wrappers.h common.h TCP_handler.h
socket_fd_handler.o: socket_fd_handler.cpp socket_fd_handler.h \
 exception_wrappers.h err.h constants.h

clean:
	rm -f $(TARGETS) *.o