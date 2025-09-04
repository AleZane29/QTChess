#include "mainwindow.h"
#include "ChessBoardWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    auto* board = new ChessBoardWidget;

    board->loadPieceImage("bq", ":/resources/bb.svg");

    board->setPieceAt(4, 0, "bq");
    board->setPieceAt(4, 1, "bq");    // re nero
    /*board->setPieceAt(4, 7, "wk"); // re bianco
    for (int f = 0; f < 8; ++f) {
        board->setPieceAt(f, 1, "bp");
        board->setPieceAt(f, 6, "wp");
    }*/

    QObject::connect(board, &ChessBoardWidget::squareClicked, [&](int f,int r){
        qDebug("Clicked: %c%d", 'a'+f, r+1);
    });
    QObject::connect(board, &ChessBoardWidget::moveRequested, [&](int f1,int r1,int f2,int r2){
        qDebug("Move: %c%d -> %c%d", 'a'+f1, r1+1, 'a'+f2, r2+1);
        // esempio: aggiorna ultima mossa e muovi simbolo
        auto sym = board->pieceAt(f1,r1);
        board->clearSquare(f1,r1);
        board->setPieceAt(f2,r2,sym);
        board->setLastMove({f1,r1}, {f2,r2});
    });

    w.setCentralWidget(board);
    w.resize(560, 560);
    w.show();
    return a.exec();
}
