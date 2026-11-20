#pragma once

#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QSlider>
#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>

#include "game_state.hpp"
#include "game_solver.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override = default;

private:
    game_state cur_state_{3, 3};
    game_solver solver_;

    QWidget* workspace;
    QHBoxLayout* elements_layout;

    QWidget* menu_controls;
    QWidget* game_controls;

    QSlider* board_size_slider;
    QSlider* win_length_slider;

    QLabel* board_size_label;
    QLabel* win_length_label;

    QPushButton* start_button;
    QPushButton* exit_button;
    QPushButton* mode_button;

    QWidget* board_widget;
    QGridLayout* board_layout;

    game_move last_move_{0, 0};
    bool has_last_move_ = false;

    void redraw_workspace();
};