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

void MainWindow::mousePressEvent (QMouseEvent* event){
    if (event->button() == Qt::LeftButton) {
        int col = event->position().x() / CELL_SIZE;
        int row = event->position().y() / CELL_SIZE;
        Position clickedPos{row, col};

        if (row < 0 || row >= 8 || col < 0 || col >= 8) return;

        if (!m_hasSelected){
            // ПЕРВЫЙ КЛИК: Выбор шашки
            CellState piece = m_controller.getBoard().getCell(clickedPos);
            Player currentPlayer = m_controller.getCurrentPlayer();

            bool isCorrectSelection = false;
            if (currentPlayer == Player::White && (piece == CellState::WhitePiece || piece == CellState::WhiteKing)) isCorrectSelection = true;
            if (currentPlayer == Player::Black && (piece == CellState::BlackPiece || piece == CellState::BlackKing)) isCorrectSelection = true;

            if (isCorrectSelection) {
                m_hasSelected = true;
                m_selectedPos = clickedPos;
                update();
            }
        } else {
            // ВТОРОЙ КЛИК: Попытка совершить ход
            Position oldPos = m_selectedPos; // запоминаем откуда ходили
            Player playerBeforeMove = m_controller.getCurrentPlayer(); // запоминаем чей был ход

            if (m_controller.makeMove(m_selectedPos, clickedPos)) {
                update();

                int status = m_controller.checkGameOver();
                if (status != 0){
                    QString mesForWinner = (status == 1) ? "Hell Yeah! White is winner." : "Fatality. Black is winner!";
                    QMessageBox::information(this, "GAMEOVER", mesForWinner);
                    m_controller.startNewGame();
                    m_hasSelected = false;
                    update();
                    return;
                }

                // Проверяем: переключил ли этот хуй игрока
                // Если игрок НЕ изменился, значит началась резняяяя!
                if (m_controller.getCurrentPlayer() == playerBeforeMove) {
                    // Оставляем выделение активным, но ПЕРЕНОСИМ его на новую клетку, куда прыгнула шашка
                    m_hasSelected = true;
                    m_selectedPos = clickedPos;
                } else {
                    // Ход завершён, передали другому игроку
                    m_hasSelected = false;
                }
                update();
            } else {
                // Если это обычный ход и он не получился — сбрасываем выделение
                // Но если это было комбо (игрок обязан бить дальше), мы не разрешаем сбрасывать выделение кликом в пустоту

                // Проверяем, находится ли игрок посреди комбо-удара:
                // Если шашка на старом месте уже пустая (она передвинулась), значит мы в процессе серии
                if (m_controller.getBoard().getCell(oldPos) == CellState::Empty) {
                    // Игнорируем неверный клик, заставляя игрока бить дальше правильной шашкой
                } else {
                    m_hasSelected = false;
                    update();
                }
            }
        }
    }
}

