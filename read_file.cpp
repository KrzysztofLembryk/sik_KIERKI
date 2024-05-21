#include "read_file.h"
#include <iostream>
#include <regex>
using namespace fHandler;

deck::CardClassWrapper create_card(const std::string &card_str)
{
    Suit suit;
    uint8_t value;

    if (card_str.length() == 2)
    {
        switch (card_str[1])
        {
        case 'H':
            suit = HEARTS;
            break;
        case 'D':
            suit = DIAMONDS;
            break;
        case 'C':
            suit = CLUBS;
            break;
        case 'S':
            suit = SPADES;
            break;
        default:
            throw std::invalid_argument("Invalid suit");
        }

        if (card_str[0] >= '2' && card_str[0] <= '9')
        {
            value = card_str[0] - '0';
        }
        else
        {
            switch (card_str[0])
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
                throw std::invalid_argument("Invalid card value");
            }
        }
    }
    else
    {
        switch (card_str[2])
        {
        case 'H':
            suit = HEARTS;
            break;
        case 'D':
            suit = DIAMONDS;
            break;
        case 'C':
            suit = CLUBS;
            break;
        case 'S':
            suit = SPADES;
            break;
        default:
            throw std::invalid_argument("Invalid suit");
        }
        value = 10;
    }

    return deck::CardClassWrapper(deck::Card(suit, value));
}


void create_player_cards(PlayerPosition player_pos, const std::string &line, game::Round &round)
{
    std::regex regx("([2-9]|10|[JQKA])([HDCS])");
    std::smatch match;
    std::string::const_iterator search_start(line.cbegin());
    std::string card_str;
    deck::DeckOfCards player_cards;

    while (std::regex_search(search_start, line.cend(), match, regx))
    {
        card_str = match.str();
        deck::CardClassWrapper card = create_card(card_str);

        player_cards.add_card(card);

        std::cout << "card: " << card_str << "\n";

        search_start = match.suffix().first;
    }
    round.set_player_cards(player_pos, player_cards);
}

std::vector<game::Round> read_rounds_from_file(const std::string &file_path)
{
    std::ifstream input_file(file_path);

    if (!input_file.is_open())
    {
        throw std::invalid_argument("File not found");
    }

    std::string line;
    int curr_line = 0;
    std::vector<game::Round> rounds;
    game::Round round;
    GameType game_type;
    PlayerPosition first_player;

    while(getline(input_file, line))
    {
        if (curr_line == 0)
        {
            game_type = (GameType)line[0];
            std::cout << "game type: " << game_type << "\n";
            first_player = (PlayerPosition)line[1];

            round.set_first_player(first_player);
            round.set_game_type(game_type);
        }
        else if (curr_line - 1 == N)
        {
           create_player_cards(N, line, round); 
        }
        else if (curr_line - 1 == E)
        {
            create_player_cards(E, line, round);
        }
        else if (curr_line - 1 == S)
        {
            create_player_cards(S, line, round);
        }
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

