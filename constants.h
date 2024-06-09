#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint>
#include <vector>

constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;
constexpr int CONTINUE = 3;
constexpr int ERROR = -1;
constexpr int TIMEOUT = -2;
constexpr int DISCONNECTED = -3;

// TCP SERVER CONSTANTS
constexpr uint8_t QUEUE_LENGTH = 5;
constexpr int BUFFER_SIZE = 1024;
constexpr int PIPE_READ_DSCR = 0;
constexpr int PIPE_WRITE_DSCR = 1;
constexpr int PIPE_BUFF_SIZE = 3;
constexpr int POLLS_NBR_OF_DSCR = 2;
constexpr int TCP_SOCKET_POLLS_ID = 0;
constexpr int STDIN_POLLS_ID = 0;
constexpr int PIPE_POLLS_ID = 1;

// CLIENT CONSTANTS
constexpr int IP6_OPT = 6;
constexpr int IP4_OPT = 4;
constexpr int NO_IP_OPT = 0;
constexpr bool NOT_INVOKED_BY_SERVER = false;


// COMMUNICATION WRAPPERS CONSTANTS
constexpr size_t INIT_PACKET_NAME_SIZE = 4;
constexpr size_t INGAME_PACKET_NAME_SIZE = 5;
constexpr size_t MAX_PACKET_NAME_SIZE = 5;

// Buffor sizes for each available packet, we need to know max and min sizes of
// every packet to be able to discern when we got to little/many bytes.
constexpr size_t MAX_IAM_BUFF_SIZE = 6;
constexpr size_t MIN_IAM_BUFF_SIZE = 6;

constexpr size_t MAX_BUSY_BUFF_SIZE = 6;
constexpr size_t MIN_BUSY_BUFF_SIZE = 3;

constexpr size_t MAX_DEAL_BUFF_SIZE = 34;
constexpr size_t MIN_DEAL_BUFF_SIZE = 30;

constexpr size_t MAX_TRICK_BUFF_SIZE = 13;
constexpr size_t MIN_TRICK_BUFF_SIZE = 3;

constexpr size_t MAX_WRONG_BUFF_SIZE = 4;
constexpr size_t MIN_WRONG_BUFF_SIZE = 3;

constexpr size_t MAX_TAKEN_BUFF_SIZE = 17;
constexpr size_t MIN_TAKEN_BUFF_SIZE = 12;

constexpr size_t MAX_SCORE_BUFF_SIZE = 18;
constexpr size_t MIN_SCORE_BUFF_SIZE = 10;

constexpr size_t MAX_TOTAL_BUFF_SIZE = 95;
constexpr size_t MIN_TOTAL_BUFF_SIZE = 10;

const std::vector<char> end_chars{'\r', '\n'};


// GAME_CLASSES CONSTANTS
constexpr uint8_t MAX_PLAYERS = 4;
constexpr uint8_t MAX_LEWA_SIZE = 4;
constexpr uint8_t MAX_CARDS_IN_DECK = 13;
constexpr uint8_t MAX_NBR_OF_LEWAS = 13;
constexpr uint8_t MAX_ONE_COLOR_SIZE = 13;
constexpr uint8_t MAX_ONE_FIGURE_SIZE = 4;
// ---------------------------------------
constexpr uint8_t INIT_POINTS = 0;
constexpr uint8_t NO_LEWA_POINTS = 1;
constexpr uint8_t NO_HEART_POINTS = 1;
constexpr uint8_t NO_QUEEN_POINTS = 5;
constexpr uint8_t NO_MISTER_POINTS = 2;
constexpr uint8_t NO_KING_HEARTS_POINTS = 18;
constexpr uint8_t NO_SEVEN_AND_LAST_POINTS = 10;

// READ_FILE CONSTANTS
const std::string CARD_REGEX = "([2-9]|10|[JQKA])([HDCS])";

#endif // CONSTANTS_H