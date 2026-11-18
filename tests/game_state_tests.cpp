#include <gtest/gtest.h>
#include <stdexcept>

#include "game_state.hpp"
#include "hash_table.hpp"

TEST(game_state, empty_constructor) {
    game_state state(3, 3);

    EXPECT_EQ(state.get_board_size(), 3);
    EXPECT_EQ(state.get_win_length(), 3);
    EXPECT_EQ(state.get_result(), game_result::in_progress);
    EXPECT_EQ(state.get_current_player(), player::x);
    EXPECT_FALSE(state.is_terminal());

    for (size_t row = 0; row < 3; ++row)
        for (size_t col = 0; col < 3; ++col)
            EXPECT_EQ(state.get_cell(row, col), cell::empty);
}

TEST(game_state, zero_board_size) {
    EXPECT_THROW(game_state(0, 1), std::invalid_argument);
}

TEST(game_state, zero_win_length) {
    EXPECT_THROW(game_state(3, 0), std::invalid_argument);
}

TEST(game_state, win_length_exceeds_board_size) {
    EXPECT_THROW(game_state(3, 4), std::invalid_argument);
}

TEST(game_state, incorrect_board_size) {
    hash_table<size_t, cell> board;

    board[0] = cell::x;
    board[1] = cell::o;
    board[2] = cell::x;
    board[3] = cell::o;
    board[4] = cell::x;
    board[5] = cell::o;
    board[6] = cell::x;
    board[7] = cell::o;
    board[8] = cell::x;
    board[9] = cell::o;

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}

TEST(game_state, invalid_cell_value) {
    hash_table<size_t, cell> board;
    board[0] = static_cast<cell>(42);

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}

TEST(game_state, more_o_than_x) {
    hash_table<size_t, cell> board;
    board[0] = cell::o;

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}

TEST(game_state, too_many_x) {
    hash_table<size_t, cell> board;
    board[0] = cell::x;
    board[1] = cell::x;

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}

TEST(game_state, x_wins_horizontal) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({1, 0}); // O
    state = state.make_move({0, 1}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({0, 2}); // X wins

    EXPECT_EQ(state.get_result(), game_result::x_wins);
}

TEST(game_state, x_wins_vertical) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({1, 0}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({2, 0}); // X wins

    EXPECT_EQ(state.get_result(), game_result::x_wins);
}

TEST(game_state, x_wins_main_diagonal) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({1, 1}); // X
    state = state.make_move({1, 2}); // O
    state = state.make_move({2, 2}); // X wins

    EXPECT_EQ(state.get_result(), game_result::x_wins);
}

TEST(game_state, x_wins_anti_diagonal) {
    game_state state(3, 3);

    state = state.make_move({0, 2}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({1, 1}); // X
    state = state.make_move({1, 2}); // O
    state = state.make_move({2, 0}); // X wins

    EXPECT_EQ(state.get_result(), game_result::x_wins);
}

TEST(game_state, o_wins_horizontal) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({1, 0}); // O
    state = state.make_move({0, 1}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({2, 0}); // X
    state = state.make_move({1, 2}); // O wins

    EXPECT_EQ(state.get_result(), game_result::o_wins);
}

TEST(game_state, o_wins_vertical) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({0, 2}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({2, 0}); // X
    state = state.make_move({2, 1}); // O wins

    EXPECT_EQ(state.get_result(), game_result::o_wins);
}

TEST(game_state, o_wins_main_diagonal) {
    game_state state(3, 3);

    state = state.make_move({0, 1}); // X
    state = state.make_move({0, 0}); // O
    state = state.make_move({0, 2}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({1, 0}); // X
    state = state.make_move({2, 2}); // O wins

    EXPECT_EQ(state.get_result(), game_result::o_wins);
}

TEST(game_state, o_wins_anti_diagonal) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 2}); // O
    state = state.make_move({0, 1}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({1, 0}); // X
    state = state.make_move({2, 0}); // O wins

    EXPECT_EQ(state.get_result(), game_result::o_wins);
}

TEST(game_state, draw) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({0, 2}); // X
    state = state.make_move({1, 2}); // O
    state = state.make_move({1, 0}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({2, 1}); // X
    state = state.make_move({2, 0}); // O
    state = state.make_move({2, 2}); // X; draw

    EXPECT_EQ(state.get_result(), game_result::draw);
    EXPECT_TRUE(state.is_terminal());
}

TEST(game_state, game_in_progress) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({1, 1}); // X
    state = state.make_move({2, 2}); // O

    EXPECT_EQ(state.get_result(), game_result::in_progress);
    EXPECT_FALSE(state.is_terminal());
}

TEST(game_state, both_players_cannot_win) {
    hash_table<size_t, cell> board;

    board[0] = cell::x;
    board[1] = cell::x;
    board[2] = cell::x;
    board[3] = cell::o;
    board[4] = cell::o;
    board[5] = cell::o;

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}

TEST(game_state, current_player_changes_after_move) {
    game_state state(3, 3);

    auto after_x = state.make_move({0, 0});
    EXPECT_EQ(after_x.get_current_player(), player::o);

    auto after_o = after_x.make_move({1, 1});
    EXPECT_EQ(after_o.get_current_player(), player::x);
}

TEST(game_state, available_moves) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({0, 1}); // O
    state = state.make_move({1, 1}); // X
    state = state.make_move({2, 0}); // O

    auto moves = state.get_available_moves();

    ASSERT_EQ(moves.size(), 5);

    EXPECT_EQ(moves[0].row, 0);
    EXPECT_EQ(moves[0].column, 2);

    EXPECT_EQ(moves[1].row, 1);
    EXPECT_EQ(moves[1].column, 0);

    EXPECT_EQ(moves[2].row, 1);
    EXPECT_EQ(moves[2].column, 2);

    EXPECT_EQ(moves[3].row, 2);
    EXPECT_EQ(moves[3].column, 1);

    EXPECT_EQ(moves[4].row, 2);
    EXPECT_EQ(moves[4].column, 2);
}

TEST(game_state, no_available_moves_after_win) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({1, 0}); // O
    state = state.make_move({0, 1}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({0, 2}); // X wins

    EXPECT_TRUE(state.get_available_moves().empty());
}

TEST(game_state, make_move_does_not_modify_original) {
    game_state state(3, 3);

    auto next = state.make_move({1, 1});

    EXPECT_EQ(state.get_cell(1, 1), cell::empty);
    EXPECT_EQ(next.get_cell(1, 1), cell::x);

    EXPECT_EQ(state.get_current_player(), player::x);
    EXPECT_EQ(next.get_current_player(), player::o);
}

TEST(game_state, winning_move_finishes_game) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({1, 0}); // O
    state = state.make_move({0, 1}); // X
    state = state.make_move({1, 1}); // O

    auto next = state.make_move({0, 2}); // X wins

    EXPECT_EQ(next.get_result(), game_result::x_wins);
    EXPECT_TRUE(next.is_terminal());
    EXPECT_FALSE(state.is_terminal());
}

TEST(game_state, get_cell_out_of_range) {
    game_state state(3, 3);

    EXPECT_THROW(state.get_cell(3, 0), std::out_of_range);
    EXPECT_THROW(state.get_cell(0, 3), std::out_of_range);
}

TEST(game_state, make_move_out_of_range) {
    game_state state(3, 3);

    EXPECT_THROW(state.make_move({3, 0}), std::out_of_range);
    EXPECT_THROW(state.make_move({0, 3}), std::out_of_range);
}

TEST(game_state, make_move_occupied_cell) {
    game_state state(3, 3);
    state = state.make_move({0, 0}); // X

    EXPECT_THROW(state.make_move({0, 0}), std::invalid_argument);
}

TEST(game_state, make_move_after_game_finished) {
    game_state state(3, 3);

    state = state.make_move({0, 0}); // X
    state = state.make_move({1, 0}); // O
    state = state.make_move({0, 1}); // X
    state = state.make_move({1, 1}); // O
    state = state.make_move({0, 2}); // X wins

    EXPECT_THROW(state.make_move({2, 2}), std::logic_error);
}

TEST(game_state, equality) {
    game_state first(3, 3);
    game_state second(3, 3);

    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);

    auto third = first.make_move({0, 0});

    EXPECT_FALSE(first == third);
    EXPECT_TRUE(first != third);
}

TEST(game_state, different_win_length_means_different_state) {
    game_state first(3, 3);
    game_state second(3, 2);

    EXPECT_FALSE(first == second);
}

TEST(game_state, copy_constructor) {
    game_state first(3, 3);
    auto next = first.make_move({1, 1});

    game_state copy(next);

    EXPECT_TRUE(copy == next);
}

TEST(game_state, x_cannot_win_when_counts_are_equal) {
    hash_table<size_t, cell> board;

    board[0] = cell::x;
    board[1] = cell::x;
    board[2] = cell::x;

    board[3] = cell::o;
    board[4] = cell::o;
    board[6] = cell::o;

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}

TEST(game_state, o_cannot_win_when_x_count_is_greater) {
    hash_table<size_t, cell> board;

    board[0] = cell::x;
    board[2] = cell::x;

    board[3] = cell::o;
    board[4] = cell::o;
    board[5] = cell::o;

    board[6] = cell::x;
    board[8] = cell::x;

    EXPECT_THROW(game_state(board, 3, 3), std::invalid_argument);
}
