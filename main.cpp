#include "mainwindow.h"
#include <QApplication>
#include <iostream>  // Подключаем стандартный вывод C++
#include "Board.h"   // Подключаем нашу доску

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ---- НАШ ТЕСТОВЫЙ БЛОК ----
    Board testBoard; // Создаем объект доски. Внутри сразу сработает расстановка!

    std::cout << "--- TEST BOARD CHESSBOARD ---" << std::endl;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos{r, c};
            CellState state = testBoard.getCell(pos);

            // Смотрим, что лежит в клетке, и выводим символ
            if (state == CellState::Empty) {
                std::cout << ". "; // Пустая клетка
            } else if (state == CellState::BlackPiece) {
                std::cout << "B "; // Черная шашка (Black)
            } else if (state == CellState::WhitePiece) {
                std::cout << "W "; // Белая шашка (White)
            }
        }
        std::cout << std::endl; // Перенос строки в конце каждого ряда
    }
    std::cout << "-----------------------------" << std::endl;
    // ----------------------------

    MainWindow w;
    w.show();
    return a.exec();
}