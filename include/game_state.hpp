#pragma once

#include <stdexcept>
#include "hash_table.hpp"

enum class cell {
    empty,
    x,
    o
};

enum class player {
    x,
    o
};

enum class game_result {
    in_progress,
    x_wins,
    o_wins,
    draw
};

struct game_move {
    size_t row;
    size_t column;
};

class game_state
{
private:
    hash_table<size_t, cell> board_;
    size_t board_size_;
    size_t win_length_;
    game_result result_;

    void validate() const;
    void update_result();

public:
    explicit game_state(size_t board_size, size_t win_length);

    game_state(const hash_table<size_t, cell>& board, size_t board_size, size_t win_length);

    game_state(const game_state& other) = default;
    game_state(game_state&& other) noexcept = default;

    game_state& operator=(const game_state& other) = default;
    game_state& operator=(game_state&& other) noexcept = default;

    cell get_cell(size_t row, size_t column) const;
    player get_current_player() const;
    game_result get_result() const;

    size_t get_board_size() const;
    size_t get_win_length() const;

    bool is_terminal() const;

    array_sequence<game_move> get_available_moves() const;

    game_state make_move(const game_move& move) const;

    bool operator==(const game_state& other) const;
    bool operator!=(const game_state& other) const;
};