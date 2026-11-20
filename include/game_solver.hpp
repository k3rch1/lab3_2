#pragma once

#include <limits>
#include <functional>
#include "game_state.hpp"
#include "hash_table.hpp"

class game_solver
{
private:
    size_t depth_ = 3;

    array_sequence<game_move> generate_moves(const game_state& state) const;
    long long evaluate(const game_state& state) const;
    long long minimax(const game_state& state, size_t depth, long long alpha, long long beta);
public:
    game_move get_best_move(const game_state& state);
    void depth(size_t value);
    size_t depth();
};