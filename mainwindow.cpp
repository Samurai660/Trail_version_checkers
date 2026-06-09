#include "mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setFixedSize(800, 800);
    setWindowTitle("Checkers");
    initInterfaceButtons();
    updateButtonsVisibility();
}

MainWindow::~MainWindow() {
    // Деструктор пуст, так как кнопки управляются Qt автоматически
}

// --- КНОПКИ ---
void MainWindow::initInterfaceButtons() {
    QString btnStyle = "QPushButton { background-color: #4a4a4a; color: white; border: 2px solid #2a2a2a; border-radius: 5px; font-weight: bold; }";

    // Центр доски: 400 (половина 800)
    // Ставим кнопки в центр: x = 800/2 - 100 (ширина кнопки 200 / 2) = 300
    m_btnPlay = new QPushButton("ИГРАТЬ", this);
    m_btnPlay->setGeometry(300, 300, 200, 50);
    m_btnPlay->setStyleSheet(btnStyle);
    connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);

    m_btnExitMenu = new QPushButton("ВЫХОД", this);
    m_btnExitMenu->setGeometry(300, 370, 200, 50);
    m_btnExitMenu->setStyleSheet(btnStyle);
    connect(m_btnExitMenu, &QPushButton::clicked, this, &MainWindow::onExitClicked);
}

void MainWindow::updateButtonsVisibility() {
    bool inMenu = (m_gameState == GameState::MainMenu);
    m_btnPlay->setVisible(inMenu);
    m_btnExitMenu->setVisible(inMenu);
}

// --- СЛОТЫ ---
void MainWindow::onStartGameClicked() {
    m_gameState = GameState::GamePlay; // Исправлено на Gameplay
    m_controller.startNewGame();
    updateButtonsVisibility();
    update();
}

void MainWindow::onExitClicked() {
    close();
}

void MainWindow::onResetGameClicked() {
    m_controller.startNewGame();
    m_hasSelected = false; // Сбрасываем выбор, если он был
    update();              // Перерисовываем экран
}
// --- ОТРИСОВКА ---
void MainWindow::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Доска 8x8, каждая клетка по 100px (800 / 8 = 100)
    int cellSize = 100;

    for(int r = 0; r < 8; ++r){
        for(int c = 0; c < 8; ++c){
            painter.setRenderHint(QPainter::Antialiasing, false);
            // Отключаем обводку в 1 пиксель
            painter.setPen(Qt::NoPen);

            painter.setBrush((r + c) % 2 == 0 ? QColor(240,217,181) : QColor(181, 136, 99));
            painter.drawRect(c * cellSize, r * cellSize, cellSize, cellSize);


            // --- 2. РИСУЕМ ШАШКУ И ВЫДЕЛЕНИЕ ---
            // Включаем сглаживание обратно, чтобы шашки были круглыми и гладкими
            painter.setRenderHint(QPainter::Antialiasing, true);

            Position pos{r, c};
            CellState state = m_controller.getBoard().getCell(pos);
            // Подсветка выбранной шашки
            if (m_hasSelected && m_selectedPos.row == r && m_selectedPos.col == c){
                painter.setBrush(QColor(46, 204, 113, 100)); // Полупрозрачный зеленый
                painter.drawRect(c * cellSize, r * cellSize, cellSize, cellSize);
            }

            // Рисование шашки
            if (state != CellState::Empty){
                bool isWhite = (state == CellState::WhitePiece || state == CellState::WhiteKing);
                bool isKing = (state == CellState::WhiteKing || state == CellState::BlackKing);

                painter.setBrush(isWhite ? Qt::white : QColor(40, 40, 40));
                painter.setPen(isWhite ? QPen(Qt::black, 2) : QPen(Qt::white, 2));

                int padding = 12;
                painter.drawEllipse(c * cellSize + padding, r * cellSize + padding, cellSize - padding * 2, cellSize - padding * 2);

                if (isKing){
                    painter.setBrush(QColor(255, 215, 0)); // Золотой цвет короны
                    int kingPadding = cellSize / 3;
                    painter.drawEllipse(c * cellSize + kingPadding, r * cellSize + kingPadding, cellSize - kingPadding * 2, cellSize - kingPadding * 2);
                }
            }
        }
    }
}

// --- КЛИКИ ---
void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (m_gameState != GameState::GamePlay) return; // Исправлено на Gameplay

    if (event->button() == Qt::LeftButton) {
        // Учитываем cellSize = 100
        int col = event->position().x() / 100;
        int row = event->position().y() / 100;

        if (row < 0 || row >= 8 || col < 0 || col >= 8) return;

        Position clickedPos{row, col};

        if (!m_hasSelected) {
            CellState piece = m_controller.getBoard().getCell(clickedPos);
            Player currentPlayer = m_controller.getCurrentPlayer();

            bool isCorrectSelection = (currentPlayer == Player::White && (piece == CellState::WhitePiece || piece == CellState::WhiteKing)) ||
                                      (currentPlayer == Player::Black && (piece == CellState::BlackPiece || piece == CellState::BlackKing));

            if (isCorrectSelection) {
                if (m_controller.hasForcedEats() && !m_controller.canPieceEatOneMore(clickedPos)) return;
                m_hasSelected = true;
                m_selectedPos = clickedPos;
                update();
            }
        } else {
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
                    QMessageBox::information(this, "Конец игры", (winner == 1) ? "Белые победили!" : "Черные победили!");
                    m_controller.startNewGame();
                    m_hasSelected = false;
                    update();
                }
            } else {
                m_hasSelected = false;
                update();
            }
        }
    }
}