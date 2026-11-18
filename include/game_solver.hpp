#pragma once

#include <limits>
#include <functional>
#include "game_state.hpp"
#include "hash_table.hpp"

enum class bound_type {
    exact,
    lower_bound,
    upper_bound
};

struct transposition_entry {
    long long score = 0;
    size_t depth = 0;
    bound_type bound = bound_type::exact;
};

class game_solver
{
private:
    array_sequence<game_move> generate_moves(const game_state& state) const;
    long long evaluate(const game_state& state) const;
    long long minimax(const game_state& state, size_t depth, long long alpha, long long beta);
public:
    game_move get_best_move(const game_state& state);
};