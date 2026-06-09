#include "Board.h"

Board::Board() {
    reset(); // Просто вызываем сброс при создании объекта
}

void Board::reset() {
    // 1. Сначала полностью очищаем доску (делаем все клетки пустыми)
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            m_grid[r][c] = CellState::Empty;
        }
    }

    // 2. Расставляем шашки по правилам
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos{r, c};

            // Шашки стоят только на черных клетках!
            if (isDarkCell(pos)) {
                if (r < 3) {
                    // Первые 3 ряда (0, 1, 2) — черные шашки
                    m_grid[r][c] = CellState::BlackPiece;
                }
                else if (r > 4) {
                    // Последние 3 ряда (5, 6, 7) — белые шашки
                    m_grid[r][c] = CellState::WhitePiece;
                }
            }
        }
    }
}

CellState Board::getCell(Position pos) const {
    if (!isValidPosition(pos)) return CellState::Empty;
    return m_grid[pos.row][pos.col];
}

void Board::setCell(Position pos, CellState state) {
    if (isValidPosition(pos)) {
        m_grid[pos.row][pos.col] = state;
    }
}

bool Board::isValidPosition(Position pos) const {
    // Проверка, что индекс не вылетает за границы массива от 0 до 7
    return (pos.row >= 0 && pos.row < 8 && pos.col >= 0 && pos.col < 8);
}

bool Board::isDarkCell(Position pos) const {
    // Математическое правило шахматной доски: клетка темная,
    // если сумма её координат (строка + столбец) — нечетное число.
    return (pos.row + pos.col) % 2 != 0;
}