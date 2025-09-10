#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chessboardwidget.h"
#include <QMainWindow>

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
    void setBoard(ChessBoardWidget *board);

private slots:
    void on_actionSinglePLayer_triggered();

    void on_actionTwoPlayers_triggered();

private:
    Ui::MainWindow *ui;
    ChessBoardWidget *board;
};
#endif // MAINWINDOW_H
