#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <cstdint>
#include <vector>

constexpr int SUCCESS = 0;
constexpr int FAILURE = 1;
constexpr int ERROR = -1;
constexpr int TIMEOUT = -2;

// TCP SERVER CONSTANTS
constexpr uint8_t QUEUE_LENGTH = 5;
constexpr int BUFFER_SIZE = 1024;


// COMMUNICATION WRAPPERS CONSTANTS
constexpr size_t INIT_PACKET_NAME_SIZE = 4;
constexpr size_t INGAME_PACKET_NAME_SIZE = 5;
constexpr size_t MAX_PACKET_NAME_SIZE = 5;
constexpr size_t IAM_BUFF_SIZE = 6;
constexpr size_t BUSY_BUFF_SIZE = 10;
constexpr size_t DEAL_BUFF_SIZE = 50;
constexpr size_t TRICK_BUFF_SIZE = 16;
constexpr size_t WRONG_BUFF_SIZE = 8;
constexpr size_t TAKEN_BUFF_SIZE = 17;
constexpr size_t SCORE_BUFF_SIZE = 15;
constexpr size_t TOTAL_BUFF_SIZE = 19;
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