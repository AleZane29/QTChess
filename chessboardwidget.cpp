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
            legalMoves={{f,r+1}};
            if(pieceAt(f-1,r+1).startsWith("b")){
                legalMoves+={{f-1,r+1}};
            }
            if(pieceAt(f+1,r+1).startsWith("b")){
                legalMoves+={{f+1,r+1}};
            }
            if(r==1){
                legalMoves+={{f,r+2}};
            }
        }
        update();
    } else if (this->getTurn()==ChessBoardWidget::Turn::BlackTurn && a.startsWith("b")){
        //BlackPawns moves check
        if(a.endsWith("p")){
            legalMoves={{f,r-1}};
            if(pieceAt(f-1,r-1).startsWith("w")){
                legalMoves+={{f-1,r-1}};
            }
            if(pieceAt(f+1,r-1).startsWith("w")){
                legalMoves+={{f+1,r-1}};
            }
            if(r==6){
                legalMoves+={{f,r-2}};
            }
        }
        update();
    }
}

void ChessBoardWidget::setLastMove(QPair<int,int> from, QPair<int,int> to) {
    lastMove = {from, to};
    update();
}
