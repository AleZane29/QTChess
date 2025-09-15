#include "mainwindow.h"
#include "ChessBoardWidget.h"
#include <QApplication>
#include <QInputDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto* board = new ChessBoardWidget;
    MainWindow w;
    w.show();
    w.setBoard(board);

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

    board->resetBoard();

    QObject::connect(board, &ChessBoardWidget::squareClicked, [&](int f,int r){
        qDebug() << "Clicked:" << char('a' + f) << r+1;
        board->setLegalMoves(f,r);
    });
    QObject::connect(board, &ChessBoardWidget::moveRequested, [&](int f1,int r1,int f2,int r2){
        //qDebug("Move: %c%d -> %c%d", 'a'+f1, r1+1, 'a'+f2, r2+1);
        auto sym = board->pieceAt(f1,r1);

        if(board->pieceAt(f2,r2)!="" || sym.endsWith("p")){
            board->setHalfMoves(0);
        }else{board->addHalfMoves();}

        //En passant
        if(sym.endsWith("p") && (f2==f1-1 || f2==f1+1) && board->pieceAt(f2,r2)==""){
            if(sym.startsWith("w")){
                board->clearSquare(f2,r2-1);
            } else {
                board->clearSquare(f2,r2+1);
            }
            board->setHalfMoves(0);
        }

        //Castling
        if(sym.endsWith("k") && f1-f2==2){
            board->setPieceAt(3, r2, board->clearSquare(0,r2));
        }
        if(sym.endsWith("k") && f1-f2==-2){
            board->setPieceAt(5, r2, board->clearSquare(7,r2));
        }

        //Remove castling from future moves
        if(sym.endsWith("k")){
            if(sym.startsWith("w")){
                board->setWCastLong(false);
                board->setWCastShort(false);
            } else {
                board->setBCastLong(false);
                board->setBCastShort(false);
            }
        }
        if(sym.endsWith("r")){
            if(sym.startsWith("w")){
                if(f1==0){
                    board->setWCastLong(false);
                }else{
                    board->setWCastShort(false);
                }
            } else {
                if(f1==0){
                    board->setBCastLong(false);
                }else{
                    board->setBCastShort(false);
                }
            }
        }

        board->clearSquare(f1,r1);
        //Promotion
        if((sym=="wp" && r2==7)||(sym=="bp" && r2==0)){
            QDialog dialog(board);
            dialog.setAttribute(Qt::WA_DeleteOnClose, false);
            dialog.setWindowTitle("Promotion");

            QHBoxLayout layout(&dialog);
            bool pieceDialogSelected=false;

            auto addButton = [&](const QString& id){
                QPushButton* b = new QPushButton;
                b->setIcon(QIcon(":/resources/" + id + ".svg"));
                b->setIconSize(QSize(64,64));
                layout.addWidget(b);
                qDebug()<<id;
                ChessBoardWidget::connect(b, &QPushButton::clicked, [board, id, f2, r2, &pieceDialogSelected, &dialog]() {
                    board->setPieceAt(f2, r2, id);
                    pieceDialogSelected=true;
                    dialog.accept();
                });

            };

            if (board->getTurn()==ChessBoardWidget::Turn::WhiteTurn ) {
                addButton("wq");
                addButton("wr");
                addButton("wb");
                addButton("wn");
            } else {
                addButton("bq");
                addButton("br");
                addButton("bb");
                addButton("bn");
            }
            //Keep open dialog until promotion completed
            while(!pieceDialogSelected){
                dialog.exec();
            }
        } else {
            //Normal move
            board->setPieceAt(f2,r2,sym);
        }

        board->setLastMove({f1,r1}, {f2,r2});
        if(board->getTurn()==ChessBoardWidget::Turn::BlackTurn){board->addFullMoves();}
        board->changeTurn();
        board->addPosition(board->getFEN());
        qDebug()<<board->getFEN();
        if(board->isGameFinished()!=ChessBoardWidget::EndGameCondition::NoFinished){
            QDialog dialog(&w);
            dialog.setWindowTitle("Game Over");
            QVBoxLayout layout(&dialog);
            QString mes ="";

            if(board->isGameFinished()==ChessBoardWidget::EndGameCondition::WhiteWin){
                mes="WHITE WIN";
            } else if(board->isGameFinished()==ChessBoardWidget::EndGameCondition::BlackWin){
                mes="BLACK WIN";
            } else if(board->isGameFinished()==ChessBoardWidget::EndGameCondition::Draw){
                mes="DRAW";
            }
            QLabel *label = new QLabel(mes);
            label->setAlignment(Qt::AlignCenter);
            layout.addWidget(label);

            QPushButton *newGameBtn = new QPushButton("Nuova partita");
            layout.addWidget(newGameBtn);

            QObject::connect(newGameBtn, &QPushButton::clicked, [&](){
                board->resetBoard();
                dialog.accept();
            });

            dialog.exec();
        }

    });

    w.setCentralWidget(board);
    w.resize(560, 560);
    w.show();
    return a.exec();
}
