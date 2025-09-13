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
    this->board->loadFEN("rnb2bnr/pp1Pkppp/2p5/2q1p3/2P5/1P6/P2P1PPP/RNBQKBNR w KQ - 1 8");
}


void MainWindow::on_actionTwoPlayers_triggered()
{

}

