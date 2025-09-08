#include "chessboardwidget.h"

void ChessBoardWidget::changeTurn() {playerTurn = playerTurn==ChessBoardWidget::Turn::WhiteTurn ? ChessBoardWidget::Turn::BlackTurn : ChessBoardWidget::Turn::WhiteTurn;};
ChessBoardWidget::Turn ChessBoardWidget::getTurn() const {return playerTurn;}

void ChessBoardWidget::setOrientation(ChessBoardWidget::Orientation o) { boardOrientation = o; update(); }
ChessBoardWidget::Orientation ChessBoardWidget::orientation() const { return boardOrientation; }

// Colori personalizzabili
void ChessBoardWidget::setLightColor(const QColor& c) { light = c; update(); }
void ChessBoardWidget::setDarkColor(const QColor& c) { dark = c; update(); }
void ChessBoardWidget::setHighlightColor(const QColor& c) { highlight = c; update(); }
void ChessBoardWidget::setMoveColor(const QColor& c) { moveColor = c; update(); }
void ChessBoardWidget::setLastMoveColor(const QColor& c) { lastMoveColor = c; update(); }

//Castling permission
void ChessBoardWidget::setWCastShort(bool b){wCastShort=b;};
void ChessBoardWidget::setWCastLong(bool b){wCastLong=b;};
void ChessBoardWidget::setBCastShort(bool b){bCastShort=b;};
void ChessBoardWidget::setBCastLong(bool b){bCastLong=b;};

int ChessBoardWidget::getFullMoves(){return FullMoves;};
void ChessBoardWidget::addFullMoves(){FullMoves++;};
int ChessBoardWidget::getHalfMoves(){return HalfMoves;};
void ChessBoardWidget::addHalfMoves(){HalfMoves++;};
void ChessBoardWidget::setHalfMoves(int num){HalfMoves=num;};

// Carica immagini (una volta sola all’avvio)
void ChessBoardWidget::loadPieceImage(const QString& id, const QString& filePath) {
    auto* renderer = new QSvgRenderer(filePath, this);
    if (renderer->isValid()) {
        pieceImages[id] = renderer;
    } else {
        delete renderer;
    }
}

// Usa id per piazzare pezzo
void ChessBoardWidget::setPieceAt(int file, int rank, const QString& id) {
    if (!inBounds(file, rank)) return;
    board[rank][file] = id;
    update();
}


QString ChessBoardWidget::pieceAt(int file, int rank) const {
    if (!inBounds(file, rank)) return {};
    return board[rank][file];
}

// Rimuovi pezzo
QString ChessBoardWidget::clearSquare(int file, int rank) {
    if (!inBounds(file, rank)) return "";
    QString res = board[rank][file];
    board[rank][file].clear();
    update();
    return res;
}

// Pulisci tutta la scacchiera
void ChessBoardWidget::clearBoard() {
    for (auto& row : board) row.fill("");
    selected = {-1, -1};
    legalMoves.clear();
    lastMove = {{-1, -1}, {-1, -1}};
    update();
}

// Selezione/Highlight manuali (puoi collegarle alla tua logica di mosse)
void ChessBoardWidget::setSelectedSquare(int file, int rank) {
    if (inBounds(file, rank)) selected = {file, rank};
    else selected = {-1, -1};
    update();
}

void ChessBoardWidget::setLegalMoves(int f, int r) {
    QString a = pieceAt(f,r);
    if(this->getTurn()==ChessBoardWidget::Turn::WhiteTurn && a.startsWith("w")){
        //WhitePawns moves check
        if(a.endsWith("p")){
            if(pieceAt(f,r+1)==""){
                legalMoves={{f,r+1}};
                if(r==1 && pieceAt(f,r+2)==""){
                    legalMoves+={{f,r+2}};
                }
            }
            if(pieceAt(f-1,r+1).startsWith("b")){
                legalMoves+={{f-1,r+1}};
            }
            if(pieceAt(f+1,r+1).startsWith("b")){
                legalMoves+={{f+1,r+1}};
            }
            if(r==4 && lastMove.second.second==4){
                if(lastMove.second.first==f-1 && pieceAt(f-1,r)=="bp"){
                    legalMoves+={{f-1,r+1}};
                } else if(lastMove.second.first==f+1 && pieceAt(f+1,r)=="bp"){
                    legalMoves+={{f+1,r+1}};
                }
            }
        }
        //WhiteRooks-Queen moves check
        else if(a.endsWith("r") || a.endsWith("q")){
            int i=1;
            while(pieceAt(f,r+i)=="" && inBounds(f,r+i)){
                legalMoves+={{f,r+i}};
                i++;
            }
            if(pieceAt(f,r+i).startsWith("b")){legalMoves+={{f,r+i}};}
            i=1;
            while(pieceAt(f,r-i)=="" && inBounds(f,r-i)){
                legalMoves+={{f,r-i}};
                i++;
            }
            if(pieceAt(f,r-i).startsWith("b")){legalMoves+={{f,r-i}};}
            i=1;
            while(pieceAt(f+i,r)=="" && inBounds(f+i,r)){
                legalMoves+={{f+i,r}};
                i++;
            }
            if(pieceAt(f+i,r).startsWith("b")){legalMoves+={{f+i,r}};}
            i=1;
            while(pieceAt(f-i,r)=="" && inBounds(f-i,r)){
                legalMoves+={{f-i,r}};
                i++;
            }
            if(pieceAt(f-i,r).startsWith("b")){legalMoves+={{f-i,r}};}
        }
        //WhiteBishops-Queen moves check
        if(a.endsWith("b") || a.endsWith("q")){
            int i=1;
            while(pieceAt(f+i,r+i)=="" && inBounds(f+i,r+i)){
                legalMoves+={{f+i,r+i}};
                i++;
            }
            if(pieceAt(f+i,r+i).startsWith("b")){legalMoves+={{f+i,r+i}};}
            i=1;
            while(pieceAt(f+i,r-i)=="" && inBounds(f+i,r-i)){
                legalMoves+={{f+i,r-i}};
                i++;
            }
            if(pieceAt(f+i,r-i).startsWith("b")){legalMoves+={{f+i,r-i}};}
            i=1;
            while(pieceAt(f-i,r+i)=="" && inBounds(f-i,r+i)){
                legalMoves+={{f-i,r+i}};
                i++;
            }
            if(pieceAt(f-i,r+i).startsWith("b")){legalMoves+={{f-i,r+i}};}
            i=1;
            while(pieceAt(f-i,r-i)=="" && inBounds(f-i,r-i)){
                legalMoves+={{f-i,r-i}};
                i++;
            }
            if(pieceAt(f-i,r-i).startsWith("b")){legalMoves+={{f-i,r-i}};}
        }
        //WhiteKnights moves check
        else if(a.endsWith("n")){
            if(inBounds(f+1,r+2) && !pieceAt(f+1,r+2).startsWith("w")){legalMoves+={{f+1,r+2}};}
            if(inBounds(f-1,r+2) && !pieceAt(f-1,r+2).startsWith("w")){legalMoves+={{f-1,r+2}};}
            if(inBounds(f+1,r-2) && !pieceAt(f+1,r-2).startsWith("w")){legalMoves+={{f+1,r-2}};}
            if(inBounds(f-1,r-2) && !pieceAt(f-1,r-2).startsWith("w")){legalMoves+={{f-1,r-2}};}
            if(inBounds(f+2,r+1) && !pieceAt(f+2,r+1).startsWith("w")){legalMoves+={{f+2,r+1}};}
            if(inBounds(f-2,r+1) && !pieceAt(f-2,r+1).startsWith("w")){legalMoves+={{f-2,r+1}};}
            if(inBounds(f+2,r-1) && !pieceAt(f+2,r-1).startsWith("w")){legalMoves+={{f+2,r-1}};}
            if(inBounds(f-2,r-1) && !pieceAt(f-2,r-1).startsWith("w")){legalMoves+={{f-2,r-1}};}
        }
        //WhiteKing moves check
        else if(a.endsWith("k")){
            //VIETARE MOVIMENTO SE RE VA IN SCACCO
            if(inBounds(f+1,r) && !pieceAt(f+1,r).startsWith("w")){legalMoves+={{f+1,r}};}
            if(inBounds(f-1,r) && !pieceAt(f-1,r).startsWith("w")){legalMoves+={{f-1,r}};}
            if(inBounds(f+1,r-1) && !pieceAt(f+1,r-1).startsWith("w")){legalMoves+={{f+1,r-1}};}
            if(inBounds(f-1,r-1) && !pieceAt(f-1,r-1).startsWith("w")){legalMoves+={{f-1,r-1}};}
            if(inBounds(f+1,r+1) && !pieceAt(f+1,r+1).startsWith("w")){legalMoves+={{f+1,r+1}};}
            if(inBounds(f-1,r+1) && !pieceAt(f-1,r+1).startsWith("w")){legalMoves+={{f-1,r+1}};}
            if(inBounds(f,r-1) && !pieceAt(f,r-1).startsWith("w")){legalMoves+={{f,r-1}};}
            if(inBounds(f,r+1) && !pieceAt(f,r+1).startsWith("w")){legalMoves+={{f,r+1}};}
            if(wCastShort && pieceAt(5,0)=="" && pieceAt(6,0)==""){legalMoves+={{6,0}};}
            if(wCastLong && pieceAt(3,0)=="" && pieceAt(2,0)=="" && pieceAt(1,0)==""){legalMoves+={{2,0}};}
        }
        update();
    } else if (this->getTurn()==ChessBoardWidget::Turn::BlackTurn && a.startsWith("b")){
        //BlackPawns moves check
        if(a.endsWith("p")){
            if(pieceAt(f,r-1)==""){
                legalMoves={{f,r-1}};
                if(r==6 && pieceAt(f,r-2)==""){
                    legalMoves+={{f,r-2}};
                }
            }
            if(pieceAt(f-1,r-1).startsWith("w")){
                legalMoves+={{f-1,r-1}};
            }
            if(pieceAt(f+1,r-1).startsWith("w")){
                legalMoves+={{f+1,r-1}};
            }
            if(r==3 && lastMove.second.second==3){
                if(lastMove.second.first==f-1 && pieceAt(f-1,r)=="wp"){
                    legalMoves+={{f-1,r-1}};
                } else if(lastMove.second.first==f+1 && pieceAt(f+1,r)=="wp"){
                    legalMoves+={{f+1,r-1}};
                }
            }
        }
        //BlackRooks-Queen moves check
        else if(a.endsWith("r") || a.endsWith("q")){
            int i=1;
            while(pieceAt(f,r+i)=="" && inBounds(f,r+i)){
                legalMoves+={{f,r+i}};
                i++;
            }
            if(pieceAt(f,r+i).startsWith("w")){legalMoves+={{f,r+i}};}
            i=1;
            while(pieceAt(f,r-i)=="" && inBounds(f,r-i)){
                legalMoves+={{f,r-i}};
                i++;
            }
            if(pieceAt(f,r-i).startsWith("w")){legalMoves+={{f,r-i}};}
            i=1;
            while(pieceAt(f+i,r)=="" && inBounds(f+i,r)){
                legalMoves+={{f+i,r}};
                i++;
            }
            if(pieceAt(f+i,r).startsWith("w")){legalMoves+={{f+i,r}};}
            i=1;
            while(pieceAt(f-i,r)=="" && inBounds(f-i,r)){
                legalMoves+={{f-i,r}};
                i++;
            }
            if(pieceAt(f-i,r).startsWith("w")){legalMoves+={{f-i,r}};}
        }
        //BlackBishops-Queen moves check
        if(a.endsWith("b") || a.endsWith("q")){
            int i=1;
            while(pieceAt(f+i,r+i)=="" && inBounds(f+i,r+i)){
                legalMoves+={{f+i,r+i}};
                i++;
            }
            if(pieceAt(f+i,r+i).startsWith("w")){legalMoves+={{f+i,r+i}};}
            i=1;
            while(pieceAt(f+i,r-i)=="" && inBounds(f+i,r-i)){
                legalMoves+={{f+i,r-i}};
                i++;
            }
            if(pieceAt(f+i,r-i).startsWith("w")){legalMoves+={{f+i,r-i}};}
            i=1;
            while(pieceAt(f-i,r+i)=="" && inBounds(f-i,r+i)){
                legalMoves+={{f-i,r+i}};
                i++;
            }
            if(pieceAt(f-i,r+i).startsWith("w")){legalMoves+={{f-i,r+i}};}
            i=1;
            while(pieceAt(f-i,r-i)=="" && inBounds(f-i,r-i)){
                legalMoves+={{f-i,r-i}};
                i++;
            }
            if(pieceAt(f-i,r-i).startsWith("w")){legalMoves+={{f-i,r-i}};}
        }
        //BlackKnights moves check
        else if(a.endsWith("n")){
            if(inBounds(f+1,r+2) && !pieceAt(f+1,r+2).startsWith("b")){legalMoves+={{f+1,r+2}};}
            if(inBounds(f-1,r+2) && !pieceAt(f-1,r+2).startsWith("b")){legalMoves+={{f-1,r+2}};}
            if(inBounds(f+1,r-2) && !pieceAt(f+1,r-2).startsWith("b")){legalMoves+={{f+1,r-2}};}
            if(inBounds(f-1,r-2) && !pieceAt(f-1,r-2).startsWith("b")){legalMoves+={{f-1,r-2}};}
            if(inBounds(f+2,r+1) && !pieceAt(f+2,r+1).startsWith("b")){legalMoves+={{f+2,r+1}};}
            if(inBounds(f-2,r+1) && !pieceAt(f-2,r+1).startsWith("b")){legalMoves+={{f-2,r+1}};}
            if(inBounds(f+2,r-1) && !pieceAt(f+2,r-1).startsWith("b")){legalMoves+={{f+2,r-1}};}
            if(inBounds(f-2,r-1) && !pieceAt(f-2,r-1).startsWith("b")){legalMoves+={{f-2,r-1}};}
        }
        //BlackKing moves check
        else if(a.endsWith("k")){
            if(inBounds(f+1,r) && !pieceAt(f+1,r).startsWith("b")){legalMoves+={{f+1,r}};}
            if(inBounds(f-1,r) && !pieceAt(f-1,r).startsWith("b")){legalMoves+={{f-1,r}};}
            if(inBounds(f+1,r-1) && !pieceAt(f+1,r-1).startsWith("b")){legalMoves+={{f+1,r-1}};}
            if(inBounds(f-1,r-1) && !pieceAt(f-1,r-1).startsWith("b")){legalMoves+={{f-1,r-1}};}
            if(inBounds(f+1,r+1) && !pieceAt(f+1,r+1).startsWith("b")){legalMoves+={{f+1,r+1}};}
            if(inBounds(f-1,r+1) && !pieceAt(f-1,r+1).startsWith("b")){legalMoves+={{f-1,r+1}};}
            if(inBounds(f,r-1) && !pieceAt(f,r-1).startsWith("b")){legalMoves+={{f,r-1}};}
            if(inBounds(f,r+1) && !pieceAt(f,r+1).startsWith("b")){legalMoves+={{f,r+1}};}
            if(bCastShort && pieceAt(5,7)=="" && pieceAt(6,7)==""){legalMoves+={{6,7}};}
            if(bCastLong && pieceAt(3,7)=="" && pieceAt(2,7)=="" && pieceAt(1,7)==""){legalMoves+={{2,7}};}
        }
        update();
    }
}

void ChessBoardWidget::setLastMove(QPair<int,int> from, QPair<int,int> to) {
    lastMove = {from, to};
    update();
}

QString ChessBoardWidget::getFEN(){
    QString fen="";
    int emptyCells=0;
    for(int x=7; x>=0; x--){
        for(int y=0; y<8; y++){
            if(pieceAt(y,x)==""){emptyCells++;}
            else{
                if(emptyCells!=0){fen+=std::to_string(emptyCells); emptyCells=0;}
                fen+=pieceAt(y,x).startsWith("b") ? pieceAt(y,x)[1]: pieceAt(y,x)[1].toUpper();
            }
        }
        if(emptyCells!=0){fen+=std::to_string(emptyCells); emptyCells=0;}
        if(x!=0){fen+="/";}
    }
    fen+=playerTurn==Turn::WhiteTurn ? " w ":" b ";
    fen+=wCastShort ? "K" : "";
    fen+=wCastLong ? "Q" : "";
    fen+=bCastShort ? "k" : "";
    fen+=bCastLong ? "q" : "";
    fen+=fen.endsWith(" ") ? "- " : " ";
    if(lastMove.second.second==4 && pieceAt(lastMove.second.first,lastMove.second.second)=="bp"){
        fen+=char('a' + lastMove.second.first) + std::to_string(lastMove.second.second+2);
    }
    else if(lastMove.second.second==3 && pieceAt(lastMove.second.first,lastMove.second.second)=="wp"){
        fen+=char('a' + lastMove.second.first) + std::to_string(lastMove.second.second);
    } else {
        fen+="-";
    }
    fen+= " "+std::to_string(HalfMoves)+" "+std::to_string(FullMoves);
    return fen;
}
/*
void ChessBoardWidget::loadFEN(QString fen){
    int fenChPos=0;
    for(int x=7; x>=0; x--){
        for(int y=0; y<8; y++){
            if(pieceAt(y,x)==""){emptyCells++;}
            else{
                if(emptyCells!=0){fen+=std::to_string(emptyCells); emptyCells=0;}
                fen+=pieceAt(y,x).startsWith("b") ? pieceAt(y,x)[1]: pieceAt(y,x)[1].toUpper();
            }
        }
        if(emptyCells!=0){fen+=std::to_string(emptyCells); emptyCells=0;}
        if(x!=0){fen+="/";}
    }
}
*/
