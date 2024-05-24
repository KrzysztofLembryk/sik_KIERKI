#include "read_file.h"
#include <iostream>
#include <fstream>
#include <regex>
#include "exception_wrappers.h"
#include "constants.h"

using std::cout;

void determine_suit(const char suit_str, Suit &suit)
{
    if (suit_str == 'H')
        suit = HEARTS;
    else if (suit_str == 'D')
        suit = DIAMONDS;
    else if (suit_str == 'C')
        suit = CLUBS;
    else if (suit_str == 'S')
        suit = SPADES;
    else
        exception_wrappers::invalid_arg_wrapper("Invalid suit");
}

void determine_value(const char value_str, uint8_t &value)
{
    if (value_str >= '2' && value_str <= '9')
    {
        value = value_str - '0';
    }
    else
    {
        switch (value_str)
        {
        case 'J':
            value = J;
            break;
        case 'Q':
            value = Q;
            break;
        case 'K':
            value = K;
            break;
        case 'A':
            value = A;
            break;
        default:
            exception_wrappers::invalid_arg_wrapper("Invalid card value");
        }
    }
}

cardCls::CardClassWrapper create_card(const std::string &card_str)
{
    Suit suit;
    uint8_t value;

    if (card_str.length() == 2)
    {
        determine_value(card_str[0], value);
        determine_suit(card_str[1], suit);
    }
    else
    {
        value = 10;
        determine_suit(card_str[2], suit);
    }
    return cardCls::CardClassWrapper(suit, value);
}

void create_player_cards(PlayerPosition player_pos, const std::string &line, gameCls::Round &round)
{
    std::regex regx(CARD_REGEX);
    std::smatch match;
    std::string::const_iterator search_start(line.cbegin());
    std::string card_str;
    cardCls::DeckOfCards player_cards;

    cout << "CARDS FOR PLAYER: " << player_pos << "\n";
    while (std::regex_search(search_start, line.cend(), match, regx))
    {
        card_str = match.str();
        cout << "card str: " << card_str << ", len: " << card_str.length() << "\n";
        cardCls::CardClassWrapper card = create_card(card_str);

        player_cards.add_card(card);
        search_start = match.suffix().first;
    }
    round.set_player_cards(player_pos, player_cards);
}

std::vector<gameCls::Round> fHandler::read_rounds_from_file(const std::string &file_path)
{
    std::ifstream input_file(file_path);

    if (!input_file.is_open())
        exception_wrappers::invalid_arg_wrapper("File not found");

    std::string line;
    int curr_line = 0;
    std::vector<gameCls::Round> rounds;
    gameCls::Round round;
    GameType game_type;
    PlayerPosition first_player;

    while (getline(input_file, line))
    {
        if (curr_line == 0)
        {
            game_type = (GameType)(line[0] - '0');

            if (line[1] == 'N')
                first_player = N;
            else if (line[1] == 'E')
                first_player = E;
            else if (line[1] == 'S')
                first_player = S;
            else if (line[1] == 'W')
                first_player = W;
            else
                exception_wrappers::invalid_arg_wrapper("Invalid place of first player");

            round.set_first_player(first_player);
            round.set_game_type(game_type);
        }
        else if (curr_line - 1 == N)
            create_player_cards(N, line, round);
        else if (curr_line - 1 == E)
            create_player_cards(E, line, round);
        else if (curr_line - 1 == S)
            create_player_cards(S, line, round);
        else if (curr_line - 1 == W)
        {
            create_player_cards(W, line, round);
            rounds.push_back(round);
            round.clear_round();
        }
        curr_line++;
        curr_line = curr_line % 5;
    }

    input_file.close();
    return rounds;
}
