#pragma once

#include <limits>
#include <functional>
#include "game_state.hpp"
#include "hash_table.hpp"

struct game_state_hash {
    size_t operator()(const game_state& state) const {
        size_t seed = std::hash<size_t>{}(state.get_board_size());

        seed ^= std::hash<size_t>{}(state.get_win_length()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        for (size_t row = 0; row < state.get_board_size(); ++row) {
            for (size_t col = 0; col < state.get_board_size(); ++col) {
                size_t value = static_cast<size_t>(state.get_cell(row, col));
                seed ^= std::hash<size_t>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
        }

        return seed;
    }
};

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
    hash_table<game_state, transposition_entry, game_state_hash> table_;

    array_sequence<game_move> generate_moves(const game_state& state) const;
    long long evaluate(const game_state& state) const;
    long long minimax(const game_state& state, size_t depth, long long alpha, long long beta);
public:
    game_move get_best_move(const game_state& state);
};