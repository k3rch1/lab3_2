#include "game_solver.hpp"

array_sequence<game_move> game_solver::generate_moves(const game_state& state) const {
    array_sequence<game_move> moves;
    array_sequence<game_move> available_moves = state.get_available_moves();

    if (available_moves.empty()) return moves;

    size_t board_size = state.get_board_size();
    size_t radius = state.get_win_length() / 2;

    if (available_moves.size() == board_size * board_size) {
        size_t center_start = (board_size - 1) / 2;
        size_t center_end = board_size / 2;

        for (size_t i = 0; i < available_moves.size(); ++i) {
            game_move move = available_moves[i];

            if (move.row >= center_start && move.row <= center_end &&
                move.column >= center_start && move.column <= center_end) {
                moves.append(move);
            }
        }

        return moves;
    }

    for (size_t i = 0; i < available_moves.size(); ++i) {
        game_move move = available_moves[i];

        size_t row_start = move.row > radius ? move.row - radius : 0;
        size_t row_end = radius > board_size - 1 - move.row ? board_size - 1 : move.row + radius;

        size_t col_start = move.column > radius ? move.column - radius : 0;
        size_t col_end = radius > board_size - 1 - move.column ? board_size - 1 : move.column + radius;

        bool has_neighbor = false;

        for (size_t row = row_start; row <= row_end && !has_neighbor; ++row) {
            for (size_t col = col_start; col <= col_end; ++col) {
                if (state.get_cell(row, col) != cell::empty) {
                    has_neighbor = true;
                    break;
                }
            }
        }

        if (has_neighbor) moves.append(move);
    }

    return moves.empty() ? available_moves : moves;
}

long long game_solver::evaluate(const game_state& state) const {
    long long win_score = 1000000;

    if (state.get_result() == game_result::o_wins) return win_score;
    if (state.get_result() == game_result::x_wins) return -win_score;
    if (state.get_result() == game_result::draw) return 0;

    size_t board_size = state.get_board_size();
    size_t win_length = state.get_win_length();

    long long score = 0;

    int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    for (size_t row = 0; row < board_size; ++row) {
        for (size_t col = 0; col < board_size; ++col) {
            for (const auto& direction : directions) {
                int dir_row = direction[0];
                int dir_col = direction[1];

                if ((dir_row == 0 || row <= board_size - win_length) && (dir_col == 0 || (dir_col == 1 && col <= board_size - win_length) || (dir_col == -1 && col >= win_length - 1))) {
                    size_t x_count = 0;
                    size_t o_count = 0;

                    for (size_t i = 0; i < win_length; ++i) {
                        size_t r = row + (dir_row == 1 ? i : 0);
                        size_t c = col;

                        if (dir_col == 1) c += i;
                        else if (dir_col == -1) c -= i;

                        cell value = state.get_cell(r, c);

                        if (value == cell::x) ++x_count;
                        else if (value == cell::o) ++o_count;
                    }

                    if (x_count > 0 && o_count > 0) continue;

                    size_t count = x_count + o_count;
                    long long weight = 1;

                    for (size_t i = 0; i < count; ++i) weight *= win_length;

                    if (o_count > 0) score += weight;
                    else if (x_count > 0) score -= weight;
                }
            }
        }
    }

    return score;
}

long long game_solver::minimax(const game_state& state, size_t depth, long long alpha, long long beta){
    if (state.is_terminal()) {
        long long score = evaluate(state);
        long long bonus = static_cast<long long>(depth);

        if (score > 0) return score + bonus;
        if (score < 0) return score - bonus;

        return score;
    }

    if (depth == 0) return evaluate(state);

    array_sequence<game_move> moves = generate_moves(state);

    if (moves.empty()) return evaluate(state);

    bool maximizing = state.get_current_player() == player::o;
    long long best_score = maximizing ? std::numeric_limits<long long>::lowest() : std::numeric_limits<long long>::max();

    for (size_t i = 0; i < moves.size(); ++i) {
        game_state next_state = state.make_move(moves[i]);
        long long score = minimax(next_state, depth - 1, alpha, beta);

        if (maximizing) {
            best_score = std::max(best_score, score);
            alpha = std::max(alpha, best_score);
        }
        else {
            best_score = std::min(best_score, score);
            beta = std::min(beta, best_score);
        }

        if (beta <= alpha) break;
    }

    return best_score;
}

game_move game_solver::get_best_move(const game_state& state) {
    if (state.is_terminal()) throw std::logic_error("Game is already finished");

    array_sequence<game_move> moves = generate_moves(state);
    if (moves.empty()) throw std::logic_error("No available moves");

    long long alpha = std::numeric_limits<long long>::lowest();
    long long beta = std::numeric_limits<long long>::max();

    size_t search_depth = 3;
    bool maximizing = state.get_current_player() == player::o;

    long long best_score = maximizing ? std::numeric_limits<long long>::lowest() : std::numeric_limits<long long>::max();

    game_move best_move = moves[0];

    for (size_t i = 0; i < moves.size(); ++i) {
        game_state next_state = state.make_move(moves[i]);
        long long score = minimax(next_state, search_depth - 1, alpha, beta);

        if ((maximizing && score > best_score) || (!maximizing && score < best_score)) {
            best_score = score;
            best_move = moves[i];
        }

        if (maximizing) alpha = std::max(alpha, best_score);
        else beta = std::min(beta, best_score);
    }

    return best_move;
}