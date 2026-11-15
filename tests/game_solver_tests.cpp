#include <gtest/gtest.h>
#include "game_solver.hpp"

TEST(game_solver, chooses_center_on_empty_board) {
    game_state state(3, 3);
    game_solver solver;

    game_move move = solver.get_best_move(state);

    EXPECT_EQ(move.row, 1);
    EXPECT_EQ(move.column, 1);
}

TEST(game_solver, chooses_winning_move) {
    const array_sequence<cell> board{
        cell::o, cell::o, cell::empty,
        cell::x, cell::x, cell::empty,
        cell::empty, cell::empty, cell::x
    };

    game_state state(board, 3, 3);
    game_solver solver;

    game_move move = solver.get_best_move(state);

    EXPECT_EQ(move.row, 0);
    EXPECT_EQ(move.column, 2);
}

TEST(game_solver, blocks_opponent_winning_move) {
    const array_sequence<cell> board{
        cell::x, cell::x, cell::empty,
        cell::o, cell::empty, cell::empty,
        cell::empty, cell::empty, cell::empty
    };

    game_state state(board, 3, 3);
    game_solver solver;

    game_move move = solver.get_best_move(state);

    EXPECT_EQ(move.row, 0);
    EXPECT_EQ(move.column, 2);
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
    const array_sequence<cell> board{
        cell::x, cell::x, cell::x,
        cell::o, cell::o, cell::empty,
        cell::empty, cell::empty, cell::empty
    };

    game_state state(board, 3, 3);
    game_solver solver;

    EXPECT_THROW(solver.get_best_move(state), std::logic_error);
}

TEST(game_solver, same_positions_have_same_hash) {
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

    game_state_hash hasher;

    EXPECT_EQ(first, second);
    EXPECT_EQ(hasher(first), hasher(second));
}

TEST(game_solver, hash_table_finds_equivalent_positions) {
    game_state first(3, 3);

    first = first.make_move({0, 0});
    first = first.make_move({0, 2});
    first = first.make_move({2, 2});
    first = first.make_move({2, 0});

    game_state second(3, 3);

    second = second.make_move({2, 2});
    second = second.make_move({2, 0});
    second = second.make_move({0, 0});
    second = second.make_move({0, 2});

    ASSERT_EQ(first, second);

    hash_table<game_state, int, game_state_hash> table;

    table.insert({first, 42});

    EXPECT_TRUE(table.contains(second));
    EXPECT_EQ(table.at(second), 42);
}