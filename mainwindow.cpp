#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setBoard(ChessBoardWidget *b){
    this->board=b;
}

void MainWindow::on_actionSinglePLayer_triggered()
{
    //this->board->loadFEN("8/8/k2b4/8/8/1P6/P2P1PPP/RNBQKBNR b KQ - 1 8");
}


void MainWindow::on_actionTwoPlayers_triggered()
{
    board->resetBoard();
}

