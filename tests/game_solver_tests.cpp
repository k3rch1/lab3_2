#include <gtest/gtest.h>
#include <stdexcept>
#include "game_solver.hpp"

TEST(game_solver, chooses_center_on_empty_board) {
    game_state state(3, 3);
    game_solver solver;

    game_move move = solver.get_best_move(state);

    EXPECT_EQ(move.row, 1u);
    EXPECT_EQ(move.column, 1u);
}

TEST(game_solver, chooses_winning_move) {
    game_state state(3, 3);

    state = state.make_move({1, 0});
    state = state.make_move({0, 0});
    state = state.make_move({1, 1});
    state = state.make_move({0, 1});
    state = state.make_move({2, 2});

    game_solver solver;
    game_move move = solver.get_best_move(state);

    EXPECT_EQ(move.row, 0u);
    EXPECT_EQ(move.column, 2u);
}

TEST(game_solver, blocks_opponent_winning_move) {
    game_state state(3, 3);

    state = state.make_move({0, 0});
    state = state.make_move({1, 0});
    state = state.make_move({0, 1});

    game_solver solver;
    game_move move = solver.get_best_move(state);

    EXPECT_EQ(move.row, 0u);
    EXPECT_EQ(move.column, 2u);
}

TEST(game_solver, does_not_modify_state) {
    game_state state(3, 3);
    state = state.make_move({0, 0});
    state = state.make_move({2, 2});

    game_state original(state);
    game_solver solver;

    solver.get_best_move(state);

    EXPECT_EQ(state, original);
}

TEST(game_solver, throws_on_finished_game) {
    game_state state(3, 3);

    state = state.make_move({0, 0});
    state = state.make_move({1, 0});
    state = state.make_move({0, 1});
    state = state.make_move({1, 1});
    state = state.make_move({0, 2});

    game_solver solver;

    EXPECT_THROW(solver.get_best_move(state), std::logic_error);
}

TEST(game_solver, same_positions_produce_same_move) {
    game_state first(3, 3);
    first = first.make_move({0, 0});
    first = first.make_move({1, 1});
    first = first.make_move({0, 1});
    first = first.make_move({2, 2});

    game_state second(3, 3);
    second = second.make_move({0, 1});
    second = second.make_move({2, 2});
    second = second.make_move({0, 0});
    second = second.make_move({1, 1});

    ASSERT_EQ(first, second);

    game_solver solver;
    game_move first_move = solver.get_best_move(first);
    game_move second_move = solver.get_best_move(second);

    EXPECT_EQ(first_move.row, second_move.row);
    EXPECT_EQ(first_move.column, second_move.column);
}
