// ЛОГИКА ХОДОВ

#include "GameController.h"
#include <cmath>
#include <QMessageBox>
// при создании контроллера сразу запускаем новую игру
GameController::GameController(){
    startNewGame();
}
// сброс игры в начальное состояние
void GameController::startNewGame(){
    m_board.reset(); // чистим доску и расставляем шашки в массиве
    m_currentPlayer = Player::White; // по правилам белые всегда стартуют
}
// геттер для получения доски
const Board& GameController::getBoard() const {
    return m_board;
}
// геттер для получения текущего ходящего игрока
Player GameController::getCurrentPlayer() const {
    return m_currentPlayer;
}
// попытка соверщить ход
bool GameController::makeMove(Position from, Position to) {
    bool isSimple = isValidSimpleMove(from, to);
    bool isEat = isValidEatMove(from, to);

    if (!isSimple && !isEat) return false;

    // Реализация правила обязательного взятия (если есть возможность бить, простой ход блокируется)
    if (hasForcedEats() && isSimple) return false;

    // Применение изменений к модели данных (перемещение фигуры)
    CellState movingPiece = m_board.getCell(from);
    m_board.setCell(to, movingPiece);
    m_board.setCell(from, CellState::Empty);

    // Если это был бой — зачищаем поле
    if (isEat) {
        removeEatenPiece(from, to);
    }

    // Смотрим, не стала ли шашка дамкой
    checkKingPromotion(to);

    // Если был бой и можно бить дальше этой же фигурой — продолжаем серию
    if (isEat && canPieceEatOneMore(to)) {
        return true;
    }

    // Смена хода
    switchPlayer();
    return true;
}
// самая главная функция - проверка правил обычного хода
bool GameController::isValidSimpleMove(Position from, Position to) const {
    if(!m_board.isValidPosition(from) || !m_board.isValidPosition(to)) return false;

    CellState fromState = m_board.getCell(from);
    if(m_currentPlayer == Player::White && fromState != CellState::WhitePiece && fromState != CellState::WhiteKing) return false;
    if(m_currentPlayer == Player::Black && fromState != CellState::BlackPiece && fromState != CellState::BlackKing) return false;

    if(m_board.getCell(to) != CellState::Empty) return false;

    int rowDiff = to.row - from.row;
    int colDiff = to.col - from.col;

    // Любой ход обязан быть строго по диагонали
    if (std::abs(rowDiff) != std::abs(colDiff)) return false;

    bool isKing = (fromState == CellState::WhiteKing || fromState == CellState::BlackKing);

    if(!isKing) {
        // Обычная пешка: только на 1 клетку и только вперёд
        if (std::abs(rowDiff) != 1) return false;
        if (m_currentPlayer == Player::White && rowDiff != -1) return false;
        if (m_currentPlayer == Player::Black && rowDiff != 1) return false;
    } else {
        // Реализация правил "длинной" (летящей) дамки согласно правилам русских шашек.
        // Вычисляем единичный вектор направления движения (шаг по строке и столбцу).
        int stepRow = (rowDiff > 0) ? 1 : -1;
        int stepCol = (colDiff > 0) ? 1 : -1;

        int currentRow = from.row + stepRow;
        int currentCol = from.col + stepCol;

        // Линейное сканирование диагонали: проверяем, что все клетки между
        // стартовой и конечной позициями строго свободны.
        while(currentRow != to.row && currentCol != to.col) {
            if (m_board.getCell({currentRow, currentCol}) != CellState::Empty) {
                return false; // дамку останавливает фигура
            }
            currentRow += stepRow;
            currentCol += stepCol;
        }
    }

    return true;
}
// Переключение хода
void GameController::switchPlayer(){
    // Тернарный оператор: если ходили белые — ставим черных, и наоборот
    m_currentPlayer = (m_currentPlayer == Player::White) ? Player::Black : Player::White;
}
//функция взятия фигур
bool GameController::isValidEatMove(Position from, Position to) const {
    if (!m_board.isValidPosition(from) || !m_board.isValidPosition(to)) return false;

    CellState fromState = m_board.getCell(from);
    if (m_board.getCell(to) != CellState::Empty) return false;

    if(m_currentPlayer == Player::White) {

        if(fromState != CellState::WhitePiece && fromState != CellState::WhiteKing) return false;
    } else {

        if (fromState != CellState::BlackPiece && fromState != CellState::BlackKing) return false;
    }

    int rowDiff = to.row - from.row;
    int colDiff = to.col - from.col;
    if (std::abs(rowDiff) != std::abs(colDiff)) return false;

    bool isKing = (fromState == CellState::WhiteKing || fromState == CellState::BlackKing);

    if (!isKing) {
        // Обычная пешка: бьёт строго на 2 клетки во все стороны
        if (std::abs(rowDiff) != 2) return false;

        int middleRow = from.row + (rowDiff / 2);
        int middleCol = from.col + (colDiff / 2);
        CellState middleState = m_board.getCell({middleRow, middleCol});

        if(m_currentPlayer == Player::White) {
            return (middleState == CellState::BlackPiece || middleState == CellState::BlackKing);
        } else {
            return (middleState == CellState::WhitePiece || middleState == CellState::WhiteKing);
        }
    } else {
        // дамка ищет врага на всей линии
        int stepRow = (rowDiff > 0) ? 1 : -1;
        int stepCol = (colDiff > 0) ? 1 : -1;

        int currentRow = from.row + stepRow;
        int currentCol = from.col + stepCol;

        int piecesFound = 0;
        CellState enemyState = CellState::Empty;

        while (currentRow != to.row && currentCol != to.col) {
            CellState currentState = m_board.getCell({currentRow, currentCol});

            if (currentState != CellState::Empty) {
                piecesFound++;
                enemyState = currentState; // запоминаем, кого нашли
            }
            currentRow += stepRow;
            currentCol += stepCol;
        }

        // Дамка может съесть, только если на пути встретилась фигура
        if (piecesFound != 1) return false;

        // Проверяем, что эта фигура была вражеской
        if(m_currentPlayer == Player::White) {
            return (enemyState == CellState::BlackPiece || enemyState == CellState::BlackKing);
        } else {
            return (enemyState == CellState::WhitePiece || enemyState == CellState::WhiteKing);
        }
    }
}

//реализация конца игры

int GameController::checkGameOver() const {
    int countWhite = 0;
    int countBlack = 0;

    //считаем оставшихся
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c){
            CellState state = m_board.getCell({r, c});
            if (state == CellState::WhitePiece || state == CellState::WhiteKing) countWhite++;
            if (state == CellState::BlackPiece || state == CellState::BlackKing) countBlack++;
        }
    }

    if (countWhite == 0){
        return 2;
    }
    if (countBlack == 0){
        return 1;
    }
    return 0;
}
// попытка дробления кода

bool GameController::canPieceEatOneMore(Position pos) const {
    CellState state = m_board.getCell(pos);
    bool isKing = (state == CellState::WhiteKing || state == CellState::BlackKing);
    // Векторы направлений для пошагового сканирования 4-х смежных диагоналей
    int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (int i = 0; i < 4; ++i) {
        int stepRow = directions[i][0];
        int stepCol = directions[i][1];

        if (!isKing) {
            // Обычная шашка: проверяет строго фиксированный прыжок на 2 клетки во всех направлениях
            Position targetPos{pos.row + stepRow * 2, pos.col + stepCol * 2};
            if(isValidEatMove(pos, targetPos)) return true;
        } else {
            // просмотр дамкой всей длинны для потенциального взятия фигуры
            int r = pos.row + stepRow;
            int c = pos.col + stepCol;

            while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                if (isValidEatMove(pos, {r, c})) {
                    return true;
                }
                r += stepRow;
                c += stepCol;
            }
        }
    }
    return false;
}
// Проверяем, принадлежит ли фигура текущему игроку
bool GameController::isOwnPiece(CellState piece) const {
    if (m_currentPlayer == Player::White) {
        return (piece == CellState::WhitePiece || piece == CellState::WhiteKing);
    } else {
        return (piece == CellState::BlackPiece || piece == CellState::BlackKing);
    }
}

// Удаляем врага, которого взяли
void GameController::removeEatenPiece(Position from, Position to) {
    int rowDiff = to.row - from.row;
    int colDiff = to.col - from.col;
    int stepRow = (rowDiff > 0) ? 1 : -1;
    int stepCol = (colDiff > 0) ? 1 : -1;

    int currentRow = from.row + stepRow;
    int currentCol = from.col + stepCol;

    // Сканируем линию и убираем первую встречную фигуру
    while (currentRow != to.row && currentCol != to.col) {
        if (m_board.getCell({currentRow, currentCol}) != CellState::Empty) {
            m_board.setCell({currentRow, currentCol}, CellState::Empty);
            break;
        }
        currentRow += stepRow;
        currentCol += stepCol;
    }
}

// Проверяем достижение края доски
void GameController::checkKingPromotion(Position pos) {
    CellState piece = m_board.getCell(pos);
    if (m_currentPlayer == Player::White && pos.row == 0 && piece == CellState::WhitePiece) {
        m_board.setCell(pos, CellState::WhiteKing);
    }
    if (m_currentPlayer == Player::Black && pos.row == 7 && piece == CellState::BlackPiece) {
        m_board.setCell(pos, CellState::BlackKing);
    }
}

// Проверка всей доски на наличие обязательных ходов
bool GameController::hasForcedEats() const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos{r, c};
            if (isOwnPiece(m_board.getCell(pos)) && canPieceEatOneMore(pos)) {
                return true;
            }
        }
    }
    return false;
}