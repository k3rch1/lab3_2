#include "game_state.hpp"

game_state::game_state(size_t board_size, size_t win_length) : board_(), board_size_(board_size), win_length_(win_length), result_(game_result::in_progress) {
    validate();
}

game_state::game_state(const hash_table<size_t, cell>& board, size_t board_size, size_t win_length) : board_(board), board_size_(board_size), win_length_(win_length) {
    validate();
    update_result();
}

void game_state::validate() const {
    if (board_size_ == 0) throw std::invalid_argument("Board size must be positive");
    if (win_length_ == 0 || win_length_ > board_size_) throw std::invalid_argument("Invalid win length");

    size_t x_count = 0;
    size_t o_count = 0;

    for (size_t i = 0; i < board_size_ * board_size_; ++i) {
        if (board_.contains(i)) {
            cell value = board_.at(i);

            if (value == cell::x) ++x_count;
            else if (value == cell::o) ++o_count;
            else if (value != cell::empty) throw std::invalid_argument("Invalid cell value");
        }
    }

    if (o_count > x_count || x_count > o_count + 1) throw std::invalid_argument("Invalid game state");
}

void game_state::update_result() {
    bool x_wins = false;
    bool o_wins = false;
    bool has_empty = false;

    int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    size_t valid_entry_count = 0;
    size_t x_count = 0;
    size_t o_count = 0;

    for (size_t row = 0; row < board_size_; ++row) {
        for (size_t col = 0; col < board_size_; ++col) {
            cell value = get_cell(row, col);
            if (value == cell::empty) {
                has_empty = true;
                continue;
            }

            ++valid_entry_count;
            if (value == cell::x) ++x_count;
            else ++o_count;

            bool wins = false;

            for (const auto& direction : directions) {
                int dir_row = direction[0];
                int dir_col = direction[1];

                if ((dir_row == 0 || row <= board_size_ - win_length_) && (dir_col == 0 || (dir_col == 1 && col <= board_size_ - win_length_) || (dir_col == -1 && col >= win_length_ - 1))) {
                    bool line = true;

                    size_t r = row;
                    size_t c = col;

                    for (size_t k = 1; k < win_length_; ++k) {
                        if (dir_row == 1) ++r;

                        if (dir_col == 1) ++c;
                        else if (dir_col == -1) --c;

                        if (get_cell(r, c) != value) {
                            line = false;
                            break;
                        }
                    }

                    if (line) {
                        wins = true;
                        break;
                    }
                }
            }

            if (wins) {
                if (value == cell::x) x_wins = true;
                else o_wins = true;
            }
        }
    }

    if (x_wins && o_wins) throw std::invalid_argument("Both players cannot win");
    if (x_wins && x_count != o_count + 1) throw std::invalid_argument("Invalid X winning state");
    if (o_wins && x_count != o_count) throw std::invalid_argument("Invalid O winning state");
    if (valid_entry_count != board_.size()) throw std::invalid_argument("Invalid board index");

    if (x_wins) result_ = game_result::x_wins;
    else if (o_wins) result_ = game_result::o_wins;
    else if (!has_empty) result_ = game_result::draw;
    else result_ = game_result::in_progress;
}

cell game_state::get_cell(size_t row, size_t column) const {
    if (row >= board_size_ || column >= board_size_) throw std::out_of_range("Cell coordinates out of range");

    size_t index = row * board_size_ + column;
    if (!board_.contains(index)) return cell::empty;

    return board_.at(index);
}

game_result game_state::get_result() const {
    return result_;
}

size_t game_state::get_board_size() const {
    return board_size_;
}

size_t game_state::get_win_length() const {
    return win_length_;
}

bool game_state::is_terminal() const {
    return result_ != game_result::in_progress;
}

player game_state::get_current_player() const {
    size_t x_count = 0;
    size_t o_count = 0;

    for (size_t i = 0; i < board_size_ * board_size_; ++i) {
        if (!board_.contains(i)) continue;

        if (board_.at(i) == cell::x) ++x_count;
        else if (board_.at(i) == cell::o) ++o_count;
    }

    return x_count == o_count ? player::x : player::o;
}

array_sequence<game_move> game_state::get_available_moves() const {
    array_sequence<game_move> moves;

    if (is_terminal()) return moves;

    for (size_t row = 0; row < board_size_; ++row)
        for (size_t col = 0; col < board_size_; ++col)
            if (get_cell(row, col) == cell::empty) moves.append(game_move{row, col});

    return moves;
}

game_state game_state::make_move(const game_move& move) const {
    if (is_terminal()) throw std::logic_error("Game is already finished");
    if (move.row >= board_size_ || move.column >= board_size_) throw std::out_of_range("Move coordinates out of range");

    size_t index = move.row * board_size_ + move.column;
    if (get_cell(move.row, move.column) != cell::empty)  throw std::invalid_argument("Cell is not empty");

    game_state next_state(*this);
    next_state.board_[index] = get_current_player() == player::x ? cell::x : cell::o;
    next_state.update_result();

    return next_state;
}

bool game_state::operator==(const game_state& other) const {
    if (board_size_ != other.board_size_ || win_length_ != other.win_length_)  return false;

    for (size_t row = 0; row < board_size_; ++row)
        for (size_t col = 0; col < board_size_; ++col)
            if (get_cell(row, col) != other.get_cell(row, col)) return false;
    return true;
}

bool game_state::operator!=(const game_state& other) const {
    return !(*this == other);
}