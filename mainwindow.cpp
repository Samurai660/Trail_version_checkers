#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
MainWindow:: MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(CELL_SIZE * 8, CELL_SIZE * 8); //задаем фиксированный размер окна, чтобы доска $8 \times 8$(по 60 пикселей) влезала идеально
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event){
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //Включаем сглаживание, чтобы круги шашек были ровными

    //проходим циклом по всей доске и рисуем клетки

    for(int r = 0; r < 8; ++r){
        for(int c = 0; c < 8; ++c){
            //определяем цвет клетки

            if((r + c) % 2 == 0){
                painter.setBrush(QColor(240,217,181)); //светло - бежевая клетка
            }else{
                painter.setBrush(QColor(181, 136, 99)); // коричневая клетка
            }
            // рисуем квадрат клетки
            painter.drawRect(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            //Теперь проверяем, есть ли на этой клетке шашка и рисуем ее
            Position pos{r, c};
            CellState state = m_controller.getBoard().getCell(pos);

            //подсветка выбранной шашки
            if (m_hasSelected){
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen(QColor(46, 204, 113), 4));

                painter.drawRect(m_selectedPos.col * CELL_SIZE, m_selectedPos.row * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
            if (state != CellState::Empty){
                bool isWhite = (state == CellState::WhitePiece || state == CellState::WhiteKing);
                bool isKing = (state == CellState::WhiteKing || state == CellState::BlackKing);

                if (isWhite) {
                    painter.setBrush(Qt::white);
                    painter.setPen (QPen(Qt::black, 2));
                }else {
                    painter.setBrush(QColor(40, 40, 40));
                    painter.setPen(QPen(Qt::white, 2));
                }
                //рисуем круг шашки чуть меньше размера клетки, чтобы были отсутпы
                int padding = 8;
                painter.drawEllipse(c * CELL_SIZE + padding, r * CELL_SIZE + padding, CELL_SIZE - padding * 2, CELL_SIZE - padding * 2);

                //кто нибудь, дайте этой пешке корону (Отрисовка короны у дамки)

                if (isKing){
                    painter.setBrush(QColor(255, 215, 0));
                    painter.setPen(QPen(Qt::black, 1));

                    int kingPadding = CELL_SIZE / 3;
                    painter.drawEllipse (c * CELL_SIZE + kingPadding, r * CELL_SIZE + kingPadding, CELL_SIZE - kingPadding * 2, CELL_SIZE - kingPadding * 2);
                }
            }
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int col = event->position().x() / CELL_SIZE;
        int row = event->position().y() / CELL_SIZE;
        Position clickedPos{row, col};

        if (row < 0 || row >= 8 || col < 0 || col >= 8) return;

        if (!m_hasSelected) {
            // ПЕРВЫЙ КЛИК: Выбираем шашку
            CellState piece = m_controller.getBoard().getCell(clickedPos);
            Player currentPlayer = m_controller.getCurrentPlayer();

            bool isCorrectSelection = false;
            if (currentPlayer == Player::White && (piece == CellState::WhitePiece || piece == CellState::WhiteKing)) isCorrectSelection = true;
            if (currentPlayer == Player::Black && (piece == CellState::BlackPiece || piece == CellState::BlackKing)) isCorrectSelection = true;

            if (isCorrectSelection) {
                // Если нужно обязательно бить, проверяем конкретно эту выбранную шашку
                if (m_controller.hasForcedEats()) {
                    if (!m_controller.canPieceEatOneMore(clickedPos)) {
                        return; // Мимо, этой шашкой ходить нельзя, нужно бить другой
                    }
                }

                m_hasSelected = true;
                m_selectedPos = clickedPos;
                update();
            }
        } else {
            // ВТОРОЙ КЛИК: Делаем ход
            if (clickedPos.row == m_selectedPos.row && clickedPos.col == m_selectedPos.col) {
                m_hasSelected = false;
                update();
                return;
            }

            if (m_controller.makeMove(m_selectedPos, clickedPos)) {
                if (m_controller.canPieceEatOneMore(clickedPos) && m_controller.getBoard().getCell(clickedPos) != CellState::Empty) {
                    m_selectedPos = clickedPos;
                    m_hasSelected = true;
                } else {
                    m_hasSelected = false;
                }
                update();

                int winner = m_controller.checkGameOver();
                if (winner != 0) {
                    QString message = (winner == 1) ? "Белые победили!" : "Черные победили!";
                    QMessageBox::information(this, "Конец игры", message);
                    m_controller.startNewGame();
                    m_hasSelected = false;
                    update();
                }
            } else {
                m_hasSelected = false;
                update();
            }
        }
    } // Конец проверки LeftButton
}

