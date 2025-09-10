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
    //this->board->loadFEN("r1b1k1nr/ppp3pp/8/3PNpP1/1nB4q/8/PPPP1b1P/RNBQ1RK1 w kq f6 0 9");
}


void MainWindow::on_actionTwoPlayers_triggered()
{

}

