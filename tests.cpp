#include "card_classes.h"


int main()
{
    // FROM SERVER:
    // ingame_comm_wrappers::WRONG_Wrapper wrong_wrapper;
    // wrong_wrapper.write(client_fd_sp->to_int(), cardCls::Lewa(1));
    // ingame_comm_wrappers::TAKEN_Wrapper taken_wrapper;
    // cardCls::Lewa lewa(1);
    // lewa.add_card(cardCls::CardClassWrapper(HEARTS, A));
    // lewa.add_card(cardCls::CardClassWrapper(SPADES, 10));
    // lewa.add_card(cardCls::CardClassWrapper(DIAMONDS, 2));
    // lewa.add_card(cardCls::CardClassWrapper(CLUBS, 3));
    // taken_wrapper.write(client_fd_sp->to_int(), lewa, N);
    // ingame_comm_wrappers::TRICK_Wrapper trick_wrapper;
    // cardCls::Lewa lewa(1);
    // lewa.add_card(cardCls::CardClassWrapper(HEARTS, A));
    // lewa.add_card(cardCls::CardClassWrapper(SPADES, 10));
    // lewa.add_card(cardCls::CardClassWrapper(DIAMONDS, 2));
    // trick_wrapper.write(client_fd_sp->to_int(), lewa);

    // ingame_comm_wrappers::TOTAL_Wrapper total_wrapper;
    // total_wrapper.write(client_fd_sp->to_int(), std::map<PlayerPosition, uint32_t> {
    //     {N, 33333}, {E, 255}, {S, 1}, {W, 2137}});
    // ingame_comm_wrappers::SCORE_Wrapper score_wrapper;
    // score_wrapper.write(client_fd_sp->to_int(), std::map<PlayerPosition, uint8_t>{
    //     {N, 1}, {E, 27}, {S, 0}, {W, 69}
    // });
}