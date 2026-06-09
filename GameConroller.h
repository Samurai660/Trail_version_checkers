#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Board.h"
#include "GameTypes.h"
#include <vector>

class GameController {
public:
    GameController();

    // Начать игру заново
    void startNewGame();

    // Получить текущую доску (нужно будет для интерфейса)
    const Board& getBoard() const;

    // Чей сейчас ход?
    Player getCurrentPlayer() const;

    // Главная функция: попытка сделать ход из точки 'from' в точку 'to'
    // Возвращает true, если ход успешный и правила не нарушены
    bool makeMove(Position from, Position to);

private:
    Board m_board;            // Наша доска, с которой мы работаем
    Player m_currentPlayer;   // Кто сейчас ходит (White или Black)

    // Внутренние функции-помощники для проверки правил (чтобы не раздувать одну функцию)
    bool isValidSimpleMove(Position from, Position to) const;
    void switchPlayer();       // Передать ход другому игроку
};

#endif // GAMECONTROLLER_H