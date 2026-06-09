#ifndef BOARD_H
#define BOARD_H

#include <array>
#include "GameTypes.h"

class Board {
public:
    Board();

    void reset();

    CellState getCell(Position pos) const;
    void setCell(Position pos, CellState state);

    bool isValidPosition(Position pos) const;
    bool isDarkCell(Position pos) const;

private:
    std::array<std::array<CellState, 8>, 8> m_grid;
};

#endif