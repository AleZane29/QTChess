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
void ChessBoardWidget::clearSquare(int file, int rank) {
    if (!inBounds(file, rank)) return;
    board[rank][file].clear();
    update();
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
    //AGGIUNGERE EN PASSANT
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
                if(lastMove.second.first==f-1){
                    legalMoves+={{f-1,r+1}};
                } else if(lastMove.second.first==f+1){
                    legalMoves+={{f+1,r+1}};
                }
            }
        }
        //WhiteRooks moves check
        else if(a.endsWith("r")){
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
                if(lastMove.second.first==f-1){
                    legalMoves+={{f-1,r-1}};
                } else if(lastMove.second.first==f+1){
                    legalMoves+={{f+1,r-1}};
                }
            }
        }
        //BlackRooks moves check
        else if(a.endsWith("r")){
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
        update();
    }
}

void ChessBoardWidget::setLastMove(QPair<int,int> from, QPair<int,int> to) {
    lastMove = {from, to};
    update();
}
