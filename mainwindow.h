#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include "GameController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    GameController m_controller;// создаем объект доски прямо внутри окна
    const int CELL_SIZE = 60; //Размер одной клетки в пикселях (60x60)
    // выбор шашки мышкой
    bool m_hasSelected = false; //флаг: выбрал ли игрок шашку первым кликом?
    Position m_selectedPos; // координаты выбранной шашки
    void mousePressEvent (QMouseEvent* event) override; //объявляем метод клика мышкой
};
#endif // MAINWINDOW_H
