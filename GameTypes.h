#ifndef GAMETYPES_H
#define GAMETYPES_H

// Структура координат клетки
struct Position {
    int row;
    int col;

    // Оператор сравнения (чтобы можно было писать pos1 == pos2)
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

// Состояния клетки на доске
enum class CellState {
    Empty,
    WhitePiece,
    WhiteKing,
    BlackPiece,
    BlackKing
};

// Кто сейчас ходит
enum class Player {
    White,
    Black
};

#endif // GAMETYPES_H