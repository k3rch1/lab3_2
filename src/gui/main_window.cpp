#include "main_window.hpp"

MainWindow::MainWindow() {
    workspace = new QWidget(this);
    elements_layout = new QHBoxLayout(workspace);

    menu_controls = new QWidget(workspace);
    menu_controls->setFixedWidth(220);

    auto* menu_layout = new QVBoxLayout(menu_controls);

    board_size_label = new QLabel("Board size: 3", menu_controls);
    board_size_slider = new QSlider(Qt::Horizontal, menu_controls);
    board_size_slider->setRange(3, 10);
    board_size_slider->setValue(3);

    win_length_label = new QLabel("Winline: 3", menu_controls);
    win_length_slider = new QSlider(Qt::Horizontal, menu_controls);
    win_length_slider->setRange(3, 3);
    win_length_slider->setValue(3);

    start_button = new QPushButton("Start", menu_controls);
    mode_button = new QPushButton("Mode: Classic", menu_controls);

    menu_layout->addWidget(board_size_label);
    menu_layout->addWidget(board_size_slider);
    menu_layout->addWidget(win_length_label);
    menu_layout->addWidget(win_length_slider);
    menu_layout->addWidget(start_button);
    menu_layout->addWidget(mode_button);
    menu_layout->addStretch();

    game_controls = new QWidget(workspace);
    game_controls->setFixedWidth(220);

    auto* game_layout = new QVBoxLayout(game_controls);

    exit_button = new QPushButton("Exit", game_controls);

    game_layout->addWidget(exit_button);
    game_layout->addStretch();

    game_controls->hide();

    board_widget = new QWidget(workspace);

    board_layout = new QGridLayout(board_widget);
    board_layout->setSpacing(4);
    board_layout->setContentsMargins(0, 0, 0, 0);

    elements_layout->addWidget(menu_controls);
    elements_layout->addWidget(game_controls);
    elements_layout->addWidget(board_widget, 1);

    setCentralWidget(workspace);
    redraw_workspace();

    connect(board_size_slider, &QSlider::valueChanged, this, [this](int value) {
        board_size_label->setText(QString("Board size: %1").arg(value));

        win_length_slider->setMaximum(value <= 5 ? value : 5);
    });

    connect(win_length_slider, &QSlider::valueChanged, this, [this](int value) {
        win_length_label->setText(QString("Winline: %1").arg(value));
    });

    connect(start_button, &QPushButton::clicked, this, [this]() {
        cur_state_ = game_state(
            board_size_slider->value(),
            win_length_slider->value()
        );

        menu_controls->hide();
        game_controls->show();

        has_last_move_ = false;

        redraw_workspace();
    });

    connect(mode_button, &QPushButton::clicked, this, [this]() {
        if (solver_.depth() == 3) {
            solver_.depth(size_t(2));
            mode_button->setText("Mode: Blitz");
        } else if (solver_.depth() == 2) {
            solver_.depth(size_t(3));
            mode_button->setText("Mode: Classic");
        }

        redraw_workspace();
    });

    connect(exit_button, &QPushButton::clicked, this, &QWidget::close);
}

void MainWindow::redraw_workspace() {
    while (QLayoutItem* item = board_layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->hide();
            widget->deleteLater();
        }

        delete item;
    }

    const int size = static_cast<int>(cur_state_.get_board_size());

    for (int i = 0; i < 10; ++i) {
        board_layout->setRowStretch(i, 0);
        board_layout->setColumnStretch(i, 0);
    }

    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            auto* button = new QPushButton(board_widget);

            cell current_cell = cur_state_.get_cell(row, column);

            bool is_last_move = has_last_move_ && last_move_.row == row && last_move_.column == column;
            switch (current_cell) {
                case cell::x:
                    button->setText("X");
                    button->setStyleSheet("QPushButton { color: blue; }");
                    break;
                case cell::o:
                    button->setText("O");
                    if (is_last_move) button->setStyleSheet("QPushButton { color: green; }");
                    else button->setStyleSheet("QPushButton { color: red; }");
                    break;
                case cell::empty:
                    break;
            }

            QFont font = button->font();
            font.setPointSize(24);
            font.setBold(true);
            button->setFont(font);

            button->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Expanding
            );

            if (current_cell == cell::empty && !game_controls->isHidden()) {
                connect(button, &QPushButton::clicked, this, [this, row, column]() {
                    cur_state_ = cur_state_.make_move({
                        static_cast<size_t>(row),
                        static_cast<size_t>(column)
                    });

                    if (!cur_state_.is_terminal()) {
                        game_move move = solver_.get_best_move(cur_state_);
                        cur_state_ = cur_state_.make_move(move);
                        has_last_move_ = true;
                        last_move_ = move;
                    }

                    if (cur_state_.is_terminal()) {
                        game_controls->hide();
                        menu_controls->show();
                    }

                    redraw_workspace();
                });
            } else {
                button->setEnabled(false);
            }

            board_layout->addWidget(button, row, column);
        }

        board_layout->setRowStretch(row, 1);
        board_layout->setColumnStretch(row, 1);
    }
}