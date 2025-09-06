#include "mainwindow.h"
#include "ChessBoardWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    auto* board = new ChessBoardWidget;

    board->setOrientation(ChessBoardWidget::Orientation::WhiteBottom);
    //Import images
    board->loadPieceImage("bp", ":/resources/bp.svg");
    board->loadPieceImage("bk", ":/resources/bk.svg");
    board->loadPieceImage("bq", ":/resources/bq.svg");
    board->loadPieceImage("bn", ":/resources/bn.svg");
    board->loadPieceImage("br", ":/resources/br.svg");
    board->loadPieceImage("bb", ":/resources/bb.svg");
    board->loadPieceImage("wp", ":/resources/wp.svg");
    board->loadPieceImage("wk", ":/resources/wk.svg");
    board->loadPieceImage("wq", ":/resources/wq.svg");
    board->loadPieceImage("wn", ":/resources/wn.svg");
    board->loadPieceImage("wr", ":/resources/wr.svg");
    board->loadPieceImage("wb", ":/resources/wb.svg");

    //Place rooks
    board->setPieceAt(7, 0, "wr");
    board->setPieceAt(0, 0, "wr");
    board->setPieceAt(0, 7, "br");
    board->setPieceAt(7, 7, "br");

    //Place knights
    board->setPieceAt(6, 0, "wn");
    board->setPieceAt(1, 0, "wn");
    board->setPieceAt(1, 7, "bn");
    board->setPieceAt(6, 7, "bn");

    //Place bishops
    board->setPieceAt(5, 0, "wb");
    board->setPieceAt(2, 0, "wb");
    board->setPieceAt(2, 7, "bb");
    board->setPieceAt(5, 7, "bb");

    //Place queens/kings
    board->setPieceAt(3, 0, "wq");
    board->setPieceAt(4, 0, "wk");
    board->setPieceAt(3, 7, "bq");
    board->setPieceAt(4, 7, "bk");

    //Place pawns
    for (int f = 0; f < 8; ++f) {
        board->setPieceAt(f, 6, "bp");
        board->setPieceAt(f, 1, "wp");
    }

    QObject::connect(board, &ChessBoardWidget::squareClicked, [&](int f,int r){
        qDebug() << "Clicked:" << char('a' + f) << r+1;
        board->setLegalMoves(f,r);
    });
    QObject::connect(board, &ChessBoardWidget::moveRequested, [&](int f1,int r1,int f2,int r2){
        qDebug("Move: %c%d -> %c%d", 'a'+f1, r1+1, 'a'+f2, r2+1);
        auto sym = board->pieceAt(f1,r1);
        //En passant
        if(sym.endsWith("p") && (f2==f1-1 || f2==f1+1) && board->pieceAt(f2,r2)==""){
            if(sym.startsWith("w")){
                board->clearSquare(f2,r2-1);
            } else {
                board->clearSquare(f2,r2+1);
            }
        }
        board->clearSquare(f1,r1);
        board->setPieceAt(f2,r2,sym);
        board->setLastMove({f1,r1}, {f2,r2});

        board->changeTurn();
    });

    w.setCentralWidget(board);
    w.resize(560, 560);
    w.show();
    return a.exec();
}
