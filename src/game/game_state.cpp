#include "game_state.hpp"

game_state::game_state(size_t board_size, size_t win_length) : board_(board_size * board_size), board_size_(board_size), win_length_(win_length), result_(game_result::in_progress) {
    validate();
}

game_state::game_state(const array_sequence<cell>& board, size_t board_size, size_t win_length) : board_(board), board_size_(board_size), win_length_(win_length) {
    validate();
    update_result();
}

void game_state::validate() const {
    if (board_size_ == 0) throw std::invalid_argument("Board size must be positive");
    if (win_length_ == 0 || win_length_ > board_size_) throw std::invalid_argument("Invalid win length");
    if (board_.size() != board_size_ * board_size_) throw std::invalid_argument("Invalid board size");

    size_t x_count = 0;
    size_t o_count = 0;

    for (size_t i = 0; i < board_.size(); ++i) {
        if (board_[i] == cell::x) ++x_count;
        else if (board_[i] == cell::o) ++o_count;
        else if (board_[i] != cell::empty) throw std::invalid_argument("Invalid cell value");
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

    size_t x_count = 0;
    size_t o_count = 0;

    for (size_t row = 0; row < board_size_; ++row) {
        for (size_t col = 0; col < board_size_; ++col) {
            cell value = board_[row * board_size_ + col];

            if (value == cell::empty) {
                has_empty = true;
                continue;
            }

            if (value == cell::x) ++x_count;
            else ++o_count;

            bool wins = false;

            for (const auto& direction : directions) {
                int dir_row = direction[0];
                int dir_col = direction[1];

                bool fits = (dir_row == 0 || row <= board_size_ - win_length_) && (dir_col == 0 || (dir_col == 1 && col <= board_size_ - win_length_) || (dir_col == -1 && col >= win_length_ - 1));
                if (fits) {
                    bool line = true;

                    size_t r = row;
                    size_t c = col;

                    for (size_t k = 1; k < win_length_; ++k) {
                        if (dir_row == 1) ++r;

                        if (dir_col == 1) ++c;
                        else if (dir_col == -1) --c;

                        if (board_[r * board_size_ + c] != value) {
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

    if (x_wins) result_ = game_result::x_wins;
    else if (o_wins) result_ = game_result::o_wins;
    else if (!has_empty) result_ = game_result::draw;
    else result_ = game_result::in_progress;
}

cell game_state::get_cell(size_t row, size_t column) const {
    if (row >= board_size_ || column >= board_size_) throw std::out_of_range("Cell coordinates out of range");

    return board_[row * board_size_ + column];
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

    for (size_t i = 0; i < board_.size(); ++i) {
        if (board_[i] == cell::x) ++x_count;
        else if (board_[i] == cell::o) ++o_count;
    }

    return x_count == o_count ? player::x : player::o;
}

array_sequence<game_move> game_state::get_available_moves() const
{
    array_sequence<game_move> moves;

    if (is_terminal()) return moves;

    for (size_t row = 0; row < board_size_; ++row) {
        for (size_t col = 0; col < board_size_; ++col) {
            if (board_[row * board_size_ + col] == cell::empty) moves.append(game_move{row, col});
        }
    }

    return moves;
}

game_state game_state::make_move(const game_move& move) const {
    if (is_terminal()) throw std::logic_error("Game is already finished");
    if (move.row >= board_size_ || move.column >= board_size_) throw std::out_of_range("Move coordinates out of range");

    size_t index = move.row * board_size_ + move.column;
    if (board_[index] != cell::empty) throw std::invalid_argument("Cell is not empty");

    game_state next_state(*this);
    next_state.board_[index] = get_current_player() == player::x ? cell::x : cell::o;
    next_state.update_result();

    return next_state;
}

bool game_state::operator==(const game_state& other) const {
    return board_size_ == other.board_size_ && win_length_ == other.win_length_ && board_ == other.board_;
}

bool game_state::operator!=(const game_state& other) const {
    return !(*this == other);
}