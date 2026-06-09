#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>
#include "GameController.h"

// что есть для управление экранами игры
enum class GameState {
    MainMenu,
    GamePlay,
    GameOver
};

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    //для обработки нажатий
    void onStartGameClicked();
    void onExitClicked();
    void onResetGameClicked();

private:
    GameController m_controller;

    //состояние игры
    GameState m_gameState = GameState::MainMenu;

    //логика выделения шашек
    bool m_hasSelected = false;
    Position m_selectedPos;

    //размеры игры
    static const int CELL_SIZE = 90;
    static const int BOARD_SIZE = 720;
    static const int PANEL_WUI = 280;

    // кнопки
    QPushButton* m_btnPlay = nullptr;      // Кнопка "Играть" в меню
    QPushButton* m_btnExitMenu = nullptr;  // Кнопка "Выход" в меню

    QPushButton* m_btnReset = nullptr;     // Кнопка "Заново" на боковой панели
    QPushButton* m_btnExitInGame = nullptr;// Кнопка "Выход" на боковой панели

    // Вспомогательный метод для создания и настройки кнопок
    void initInterfaceButtons();
    // Метод для обновления видимости кнопок в зависимости от состояния игры
    void updateButtonsVisibility();

#endif // MAINWINDOW_H
};
