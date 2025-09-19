#include "engine.h"

Engine::Engine() {}

void Engine::setBoard(ChessBoardWidget *b){
    this->board=b;
}

float Engine::evalutePosition(QString position){
    /*QString originPosition = board->getFEN();
    board->loadFEN(position);
    float value = 0;

    int whiteMob = 0, blackMob = 0;

    for(int r=0; r<8; r++){
        for(int f=0; f<8; f++){
            QString p = board->pieceAt(f,r);
            if (p.isEmpty()) continue;

            bool isWhite = p.startsWith("w");
            float v = this->piecesValue.value(p[1]);

            // materiale
            value += isWhite ? v : -v;

            // mobilità (numero di mosse disponibili)
            board->setLegalMoves(f,r);
            int mob = board->getLegalMoves().length();
            if (isWhite) whiteMob += mob;
            else blackMob += mob;

            // sicurezza del re: penalizza re non arroccato e con pochi pedoni davanti
            if (p == "wk" && r <= 4) value -= 0.2;
            if (p == "bk" && r >= 3) value += 0.2;
        }
    }

    // aggiungi mobilità pesata
    value += 0.1f * (whiteMob - blackMob);

    board->loadFEN(originPosition);
    return value;*/

   QString originPosition = board->getFEN();
    board->loadFEN(position);
    float value= 0;
    //Pieces evaluation
    for(int x=7; x>=0; x--){
        for(int y=0; y<8; y++){
            QString p = board->pieceAt(y,x);
            if (p.isEmpty()) continue;

            //base pieces values
            bool isWhite = p.startsWith("w");
            float v = this->piecesValue.value(p[1]);
            value += isWhite ? v : -v;

            //center pawns
            if(x>=3 && x<=4 && y>=3 && y<=4 && p.endsWith("p")){
                value += isWhite ? 0.2 : -0.2;
            }

        }
    }
    board->loadFEN(originPosition);
    return value;
}

float Engine::minimax(QString position, int depth, float alpha, float beta, bool whiteTurn){
    QString originPosition = board->getFEN();
    board->loadFEN(position);
    if(depth==0 || board->isGameFinished()!=ChessBoardWidget::EndGameCondition::NoFinished){
        float eval=this->evalutePosition(position);
        qDebug()<<position<<eval;
        board->loadFEN(originPosition);
        return eval;
    } else {
        //Check evaluation of all possible moves
        if(whiteTurn){
            float maxEval = -INFINITY;
            for(int x=7; x>=0; x--){
                for(int y=0; y<8; y++){
                    if(board->pieceAt(y,x).startsWith("w")){
                        QVector<QPair<int,int>> legalMoves = board->setLegalMoves(y,x);
                        qDebug()<<board->pieceAt(y,x)<<legalMoves;
                        for (int i = 0; i < legalMoves.length(); i++) {
                            QString piece = board->pieceAt(y,x);
                            int cy = legalMoves[i].first;
                            int cx = legalMoves[i].second;
                            QString captured = board->pieceAt(cy,cx);

                            board->setPieceAt(cy,cx,piece);
                            board->clearSquare(y,x);
                            board->changeTurn();

                            float eval=minimax(board->getFEN(), depth-1, alpha, beta, false);

                            board->setPieceAt(y,x,piece);
                            board->setPieceAt(cy,cx,captured);
                            board->changeTurn();

                            maxEval=std::max(maxEval, eval);
                            alpha=std::max(alpha, eval);

                            if (beta <= alpha){
                                qDebug()<<depth<<"Best move:"<<board->getFEN()<<eval;
                                board->loadFEN(originPosition);
                                return maxEval;
                            }
                        }
                    }
                }
            }
            qDebug()<<depth<<"Best move..:"<<board->getFEN()<<maxEval;
            board->loadFEN(originPosition);
            return maxEval;
        }
        else {
            float minEval = +INFINITY;
            for(int x=7; x>=0; x--){
                for(int y=0; y<8; y++){
                    if(board->pieceAt(y,x).startsWith("b")){
                        board->setLegalMoves(y,x);
                        QVector<QPair<int,int>> legalMoves = board->setLegalMoves(y,x);
                        for (int i = 0; i < legalMoves.length(); i++) {
                            //emit board->moveRequested(y,x, board->getLegalMoves()[i].first, board->getLegalMoves()[i].second);
                            QString piece = board->pieceAt(y,x);
                            int cy = legalMoves[i].first;
                            int cx = legalMoves[i].second;
                            QString captured = board->pieceAt(cy,cx);

                            board->setPieceAt(cy,cx,piece);
                            board->clearSquare(y,x);
                            board->changeTurn();

                            float eval = minimax(board->getFEN(), depth-1, alpha, beta, true);

                            board->setPieceAt(y,x,piece);
                            board->setPieceAt(cy,cx,captured);
                            board->changeTurn();

                            minEval=std::min(minEval, eval);
                            beta=std::min(beta, eval);

                            if (beta <= alpha){
                                qDebug()<<depth<<"Best move:"<<board->getFEN()<<eval;
                                board->loadFEN(originPosition);
                                return minEval;
                            }
                        }
                    }
                }
            }
            qDebug()<<depth<<"Best move.:"<<board->getFEN()<<minEval;
            board->loadFEN(originPosition);
            return minEval;
        }
    }
}


