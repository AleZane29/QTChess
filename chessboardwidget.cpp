#include "chessboardwidget.h"

void ChessBoardWidget::changeTurn() {playerTurn = playerTurn==ChessBoardWidget::Turn::WhiteTurn ? ChessBoardWidget::Turn::BlackTurn : ChessBoardWidget::Turn::WhiteTurn;};
ChessBoardWidget::Turn ChessBoardWidget::getTurn() const {return playerTurn;}

void ChessBoardWidget::setOrientation(ChessBoardWidget::Orientation o) { boardOrientation = o; update(); }
ChessBoardWidget::Orientation ChessBoardWidget::orientation() const { return boardOrientation; }

// colori personalizzabili
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
void ChessBoardWidget::setFullMoves(int num){FullMoves=num;};

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

QPair<int,int> ChessBoardWidget::getPiecePosition(const QString& piece){
    for(int x=7; x>=0; x--){
        for(int y=0; y<8; y++){
            if(pieceAt(y,x)==piece){
                return {y,x};
            }
        }
    }
    return {-1,-1};
}

bool ChessBoardWidget::isSquareAttacked(int file, int rank){
    for(int x=7; x>=0; x--){
        for(int y=0; y<8; y++){
            if(pieceAt(y,x).startsWith("b") && this->getTurn()==ChessBoardWidget::Turn::WhiteTurn){
                if(pieceLegalMoves(y,x, "w", "b").contains({file,rank})){
                    return true;
                }
            } else if(pieceAt(y,x).startsWith("w") && this->getTurn()==ChessBoardWidget::Turn::BlackTurn){
                if(pieceLegalMoves(y,x,"b", "w").contains({file,rank})){
                    return true;
                }
            }
        }
    }
    return false;
}

QVector<QPair<int,int>> ChessBoardWidget::pieceLegalMoves(int f, int r, QString col1, QString col2, bool underAttack){
    QVector<QPair<int,int>> res;
    QString a = pieceAt(f,r);

    //WhitePawns moves
    if(a=="wp"){
        if(pieceAt(f,r+1)==""){
            res={{f,r+1}};
            if(r==1 && pieceAt(f,r+2)==""){
                res+={{f,r+2}};
            }
        }
        if(pieceAt(f-1,r+1).startsWith("b")){
            res+={{f-1,r+1}};
        }
        if(pieceAt(f+1,r+1).startsWith("b")){
            res+={{f+1,r+1}};
        }
        if(r==4 && lastMove.second.second==4){
            if(lastMove.second.first==f-1 && pieceAt(f-1,r)=="bp"){
                res+={{f-1,r+1}};
            } else if(lastMove.second.first==f+1 && pieceAt(f+1,r)=="bp"){
                res+={{f+1,r+1}};
            }
        }
    }
    //BlackPawns moves
    if(a=="bp"){
        if(pieceAt(f,r-1)==""){
            res={{f,r-1}};
            if(r==6 && pieceAt(f,r-2)==""){
                res+={{f,r-2}};
            }
        }
        if(pieceAt(f-1,r-1).startsWith("w")){
            res+={{f-1,r-1}};
        }
        if(pieceAt(f+1,r-1).startsWith("w")){
            res+={{f+1,r-1}};
        }
        if(r==3 && lastMove.second.second==3){
            if(lastMove.second.first==f-1 && pieceAt(f-1,r)=="wp"){
                res+={{f-1,r-1}};
            } else if(lastMove.second.first==f+1 && pieceAt(f+1,r)=="wp"){
                res+={{f+1,r-1}};
            }
        }
    }
    //Rooks-Queen moves
    else if(a.endsWith("r") || a.endsWith("q")){
        int i=1;
        while(pieceAt(f,r+i)=="" && inBounds(f,r+i)){
            res+={{f,r+i}};
            i++;
        }
        if(pieceAt(f,r+i).startsWith(col1)){res+={{f,r+i}};}
        i=1;
        while(pieceAt(f,r-i)=="" && inBounds(f,r-i)){
            res+={{f,r-i}};
            i++;
        }
        if(pieceAt(f,r-i).startsWith(col1)){res+={{f,r-i}};}
        i=1;
        while(pieceAt(f+i,r)=="" && inBounds(f+i,r)){
            res+={{f+i,r}};
            i++;
        }
        if(pieceAt(f+i,r).startsWith(col1)){res+={{f+i,r}};}
        i=1;
        while(pieceAt(f-i,r)=="" && inBounds(f-i,r)){
            res+={{f-i,r}};
            i++;
        }
        if(pieceAt(f-i,r).startsWith(col1)){res+={{f-i,r}};}
    }
    //Bishops-Queen moves
    if(a.endsWith("b") || a.endsWith("q")){
        int i=1;
        while(pieceAt(f+i,r+i)=="" && inBounds(f+i,r+i)){
            res+={{f+i,r+i}};
            i++;
        }
        if(pieceAt(f+i,r+i).startsWith(col1)){res+={{f+i,r+i}};}
        i=1;
        while(pieceAt(f+i,r-i)=="" && inBounds(f+i,r-i)){
            res+={{f+i,r-i}};
            i++;
        }
        if(pieceAt(f+i,r-i).startsWith(col1)){res+={{f+i,r-i}};}
        i=1;
        while(pieceAt(f-i,r+i)=="" && inBounds(f-i,r+i)){
            res+={{f-i,r+i}};
            i++;
        }
        if(pieceAt(f-i,r+i).startsWith(col1)){res+={{f-i,r+i}};}
        i=1;
        while(pieceAt(f-i,r-i)=="" && inBounds(f-i,r-i)){
            res+={{f-i,r-i}};
            i++;
        }
        if(pieceAt(f-i,r-i).startsWith(col1)){res+={{f-i,r-i}};}
    }
    //Knights moves
    else if(a.endsWith("n")){
        if(inBounds(f+1,r+2) && !pieceAt(f+1,r+2).startsWith(col2)){res+={{f+1,r+2}};}
        if(inBounds(f-1,r+2) && !pieceAt(f-1,r+2).startsWith(col2)){res+={{f-1,r+2}};}
        if(inBounds(f+1,r-2) && !pieceAt(f+1,r-2).startsWith(col2)){res+={{f+1,r-2}};}
        if(inBounds(f-1,r-2) && !pieceAt(f-1,r-2).startsWith(col2)){res+={{f-1,r-2}};}
        if(inBounds(f+2,r+1) && !pieceAt(f+2,r+1).startsWith(col2)){res+={{f+2,r+1}};}
        if(inBounds(f-2,r+1) && !pieceAt(f-2,r+1).startsWith(col2)){res+={{f-2,r+1}};}
        if(inBounds(f+2,r-1) && !pieceAt(f+2,r-1).startsWith(col2)){res+={{f+2,r-1}};}
        if(inBounds(f-2,r-1) && !pieceAt(f-2,r-1).startsWith(col2)){res+={{f-2,r-1}};}
    }
    //King moves
    else if(a.endsWith("k")){
        if(inBounds(f+1,r) && !pieceAt(f+1,r).startsWith(col2)){res+={{f+1,r}};}
        if(inBounds(f-1,r) && !pieceAt(f-1,r).startsWith(col2)){res+={{f-1,r}};}
        if(inBounds(f+1,r-1) && !pieceAt(f+1,r-1).startsWith(col2)){res+={{f+1,r-1}};}
        if(inBounds(f-1,r-1) && !pieceAt(f-1,r-1).startsWith(col2)){res+={{f-1,r-1}};}
        if(inBounds(f+1,r+1) && !pieceAt(f+1,r+1).startsWith(col2)){res+={{f+1,r+1}};}
        if(inBounds(f-1,r+1) && !pieceAt(f-1,r+1).startsWith(col2)){res+={{f-1,r+1}};}
        if(inBounds(f,r-1) && !pieceAt(f,r-1).startsWith(col2)){res+={{f,r-1}};}
        if(inBounds(f,r+1) && !pieceAt(f,r+1).startsWith(col2)){res+={{f,r+1}};}
        if(!underAttack){
            if(a.startsWith("w")){
                if(wCastShort && pieceAt(5,0)=="" && pieceAt(6,0)==""){res+={{6,0}};}
                if(wCastLong && pieceAt(3,0)=="" && pieceAt(2,0)=="" && pieceAt(1,0)==""){res+={{2,0}};}
            } else {
                if(bCastShort && pieceAt(5,7)=="" && pieceAt(6,7)==""){res+={{6,7}};}
                if(bCastLong && pieceAt(3,7)=="" && pieceAt(2,7)=="" && pieceAt(1,7)==""){res+={{2,7}};}
            }
        }
    }
    return res;
}
void ChessBoardWidget::setLegalMoves(int f, int r) {
    QString a = pieceAt(f,r);
    if(this->getTurn()==ChessBoardWidget::Turn::WhiteTurn && a.startsWith("w")){
        legalMoves=pieceLegalMoves(f,r,"b","w", isSquareAttacked(f,r));
    } else if (this->getTurn()==ChessBoardWidget::Turn::BlackTurn && a.startsWith("b")){
        legalMoves=pieceLegalMoves(f,r, "w","b", isSquareAttacked(f,r));
    }
    //King can't move on square attacked
    if(a.endsWith("k")){
        for (int i = 0; i < legalMoves.length(); i++) {
            QString rem=pieceAt(legalMoves[i].first, legalMoves[i].second);
            setPieceAt(legalMoves[i].first, legalMoves[i].second,a);
            clearSquare(f,r);
            if(isSquareAttacked(legalMoves[i].first, legalMoves[i].second)){
                setPieceAt(legalMoves[i].first, legalMoves[i].second,rem);
                legalMoves.remove(i);
                i--;
            } else{
                setPieceAt(legalMoves[i].first, legalMoves[i].second,rem);
            }
            setPieceAt(f,r,a);
        }
    }
    QString col =this->getTurn()==ChessBoardWidget::Turn::WhiteTurn ? "w" : "b";
    //White/Black-King attacked
    for (int i = 0; i < legalMoves.length(); i++) {
        QString rem=pieceAt(legalMoves[i].first, legalMoves[i].second);
        setPieceAt(legalMoves[i].first, legalMoves[i].second,a);
        clearSquare(f,r);
        if(isSquareAttacked(getPiecePosition(col+"k").first, getPiecePosition(col+"k").second)){
            setPieceAt(legalMoves[i].first, legalMoves[i].second,rem);
            legalMoves.remove(i);
            i--;
        } else{
            setPieceAt(legalMoves[i].first, legalMoves[i].second,rem);
        }
        setPieceAt(f,r,a);
    }
    update();
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

void ChessBoardWidget::loadFEN(QString fen){
    clearBoard();
    int i=0;
    for(int x=7; x>=0; x--){
        for(int y=0; y<8; y++){
            if(fen[i]=="/"){
                y--;
            }
            else if(fen[i].isDigit()){
                y+=fen[i].digitValue()-1;
            } else if(fen[i].isLower()){
                setPieceAt(y,x, "b"+QString(fen[i]));
            } else{
                setPieceAt(y,x, "w"+QString(fen[i].toLower()));
            }
            i++;
        }
    }
    i++;
    playerTurn=fen[i]=="b" ? Turn::BlackTurn : Turn::WhiteTurn;
    i+=2;
    wCastShort = false;
    wCastLong = false;
    bCastShort = false;
    bCastLong = false;
    while(fen[i]!="-" && fen[i]!=" "){
        wCastShort = fen[i]=="K" ? true : wCastShort;
        wCastLong = fen[i]=="Q" ? true : wCastLong;
        bCastShort = fen[i]== "k" ? true : bCastShort;
        bCastLong = fen[i]=="q" ? true : bCastLong;
        i++;
    }
    if(fen[i]==" "){i++;}
    else if(fen[i]=="-"){i+=2;}
    if(fen[i+1]=="3"){setLastMove({fen[i].toLatin1() - 'a',fen[i+1].digitValue()-2},{fen[i].toLatin1() - 'a',fen[i+1].digitValue()});}
    else if(fen[i+1]=="6"){setLastMove({fen[i].toLatin1() - 'a',fen[i+1].digitValue()},{fen[i].toLatin1() - 'a',fen[i+1].digitValue()-2});}
    else{i--;}
    i+=3;
    setHalfMoves(fen[i].digitValue());
    i+=2;
    setFullMoves(fen[i].digitValue());
    update();
}
