#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QString>
#include "GameTypes.h"
#include "Board.h"
//класс GameController - судья матча. Следит за правилами, проверяет очередность ходов для игроков
class GameController {
public:
    GameController();// Конструктор: подготваливает все для игры
    void startNewGame(); //Метод для полного сброса игры (расставляет все шашки заново)
    const Board& getBoard() const;//возвращает ссылку на доску (нужно окну, чтобы знать, что и где рисовать)
    Player getCurrentPlayer() const; // возвращает игрока, который должен ходить сейчас (white or black)
    bool makeMove (Position from, Position to); // главный метод:пытается передвинуть шашку из from в to. Если ход по правилам - двигает, меняет игрока, возвращает true
    int checkGameOver () const;
    bool hasForcedEats() const; //есть ли обязательные взятия на доске?
    //проверка, можно ли съесть еще кого то или нет
    bool canPieceEatOneMore(Position pos) const;
private:
    Board m_board; // экземпляр доски с массивом клеток
    Player m_currentPlayer; // перменная, хранящая цвет текущего игровка
    // внутренний метод - помощник: проверяет корректен ли обычный шаг по диагонали
    bool isValidSimpleMove(Position from, Position to) const;
    bool isValidEatMove (Position from, Position to) const;
    // внутренний метод: передает ход от белых к черным и наоборот
    void switchPlayer();
    //разделение кода
    bool isOwnPiece(CellState piece) const;
    //проверка свой - чужой
    void removeEatenPiece(Position from, Position to);
    //проверка на дамку
    void checkKingPromotion(Position pos);

};

#endif // GAMECONTROLLER_H
