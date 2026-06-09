#include "mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setFixedSize(800, 800);
    setWindowTitle("Checkers");
    setStatusBar(nullptr);
    initInterfaceButtons();
    updateButtonsVisibility();
}

MainWindow::~MainWindow() {

}

// --- КНОПКИ ---
void MainWindow::initInterfaceButtons() {
    QString btnStyle = "QPushButton { background-color: #4a4a4a; color: white; border: 2px solid #2a2a2a; border-radius: 5px; font-weight: bold; }";

    m_btnPlay = new QPushButton("ИГРАТЬ", this);
    m_btnPlay->setGeometry(300, 320, 200, 50); // Кнопка "Играть" на Y = 320
    m_btnPlay->setStyleSheet(btnStyle);
    connect(m_btnPlay, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);

    m_btnExitMenu = new QPushButton("ВЫХОД", this);
    m_btnExitMenu->setGeometry(300, 390, 200, 50); // Кнопка "Выход" чуть ниже на Y = 390
    m_btnExitMenu->setStyleSheet(btnStyle);
    connect(m_btnExitMenu, &QPushButton::clicked, this, &MainWindow::onExitClicked);
}

void MainWindow::updateButtonsVisibility() {
    // Кнопки должны быть видны и в Главном меню, и на экране Конец Игры
    bool showButtons = (m_gameState == GameState::MainMenu || m_gameState == GameState::GameOver);
    m_btnPlay->setVisible(showButtons);
    m_btnExitMenu->setVisible(showButtons);
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
    // 2. РИСУЕМ ОВЕРЛЕЙ
    if (m_gameState != GameState::GamePlay) {
        // Используем width() и height(), чтобы оверлей идеально закрывал ЛЮБОЙ размер окна
        painter.setBrush(QColor(0, 0, 0, 220)); // Сделали чуть темнее (220 вместо 180) для солидности
        painter.drawRect(0, 0, width(), height());

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 42, QFont::Bold)); // Немного увеличили шрифт

        // Определяем текст
        QString text;
        if (m_gameState == GameState::MainMenu) {
            text = "CHECKERS";
        } else if (m_gameState == GameState::GameOver) {
            int winner = m_controller.checkGameOver();
            text = (winner == 1) ? "БЕЛЫЕ ПОБЕДИЛИ!" : "ЧЕРНЫЕ ПОБЕДИЛИ!";
        }

        // СДВИГАЕМ ТЕКСТ ВЫШЕ: вместо 0 по Y ставим 120, а высоту ограничиваем в 100 пикселей.
        // Теперь заголовок будет гордо висеть вверху, не мешая кнопкам.
        painter.drawText(0, 120, width(), 100, Qt::AlignCenter, text);
    }
}

// --- КЛИКИ ---
void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (m_gameState != GameState::GamePlay) return;

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
                    m_gameState = GameState::GameOver;
                    updateButtonsVisibility();

                    update();
                }
            } else {
                m_hasSelected = false;
                update();
            }
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        // Если идет игра, выходим в главное меню
        if (m_gameState == GameState::GamePlay) {
            m_gameState = GameState::MainMenu;
            m_hasSelected = false;
            updateButtonsVisibility();
            update();
        }
    }
    if (event->key() == Qt::Key_R) {
        // Быстрый перезапуск игры в любой момент
        if (m_gameState == GameState::GamePlay) {
            m_controller.startNewGame();
            m_hasSelected = false;
            update();
        }
    }
}