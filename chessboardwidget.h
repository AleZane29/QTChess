#ifndef CHESSBOARDWIDGET_H
#define CHESSBOARDWIDGET_H

#pragma once
#include <QWidget>
#include <QObject>
#include <QPainter>
#include <QtSvg/QSvgRenderer>
#include <QMouseEvent>
#include <QVector>
#include <QPair>
#include <QFontMetrics>
#include <array>

class ChessBoardWidget : public QWidget {
    Q_OBJECT
public:
    enum class Orientation { WhiteBottom, BlackBottom };
    enum class Turn { WhiteTurn, BlackTurn };
    enum class EndGameCondition { WhiteWin, BlackWin, Draw, NoFinished };

private:
    Orientation boardOrientation = Orientation::WhiteBottom;
    Turn playerTurn = Turn::WhiteTurn;
    QColor light = QColor(240, 217, 181); // stile legno chiaro
    QColor dark  = QColor(181, 136, 99);  // stile legno scuro
    QColor highlight = QColor(30, 144, 255); // selezione
    QColor moveColor = QColor(0, 0, 0, 160); // pallino mosse
    QColor lastMoveColor = QColor(255, 215, 0); // giallo trasparente

    std::array<std::array<QString, 8>, 8> board{};
    QHash<QString, QSvgRenderer*> pieceImages;
    QPair<int,int> selected{-1, -1};
    QVector<QPair<int,int>> legalMoves;
    QPair<QPair<int,int>, QPair<int,int>> lastMove{{-1,-1},{-1,-1}};
    QVector<QString> previousPositions;
    bool wCastShort=true;
    bool wCastLong=true;
    bool bCastShort=true;
    bool bCastLong=true;
    int FullMoves=1;
    int HalfMoves=0;

public:
    explicit ChessBoardWidget(QWidget* parent = nullptr)
        : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(240, 240);
        clearBoard();
    }

    // Imposta orientamento della scacchiera
    void setOrientation(Orientation o);
    Orientation orientation() const;

    void setBoard(std::array<std::array<QString, 8>, 8>);

    void changeTurn();
    Turn getTurn() const;

    // Colori personalizzabili
    void setLightColor(const QColor& c);
    void setDarkColor(const QColor& c);
    void setHighlightColor(const QColor& c);
    void setMoveColor(const QColor& c);
    void setLastMoveColor(const QColor& c);

    void setWCastShort(bool b);
    void setWCastLong(bool b);
    void setBCastShort(bool b);
    void setBCastLong(bool b);

    int getFullMoves();
    void addFullMoves();
    int getHalfMoves();
    void addHalfMoves();
    void setHalfMoves(int num);
    void setFullMoves(int num);

    // Carica immagini (una volta sola all’avvio)
    void loadPieceImage(const QString& id, const QString& filePath);

    // Usa id per piazzare pezzo
    void setPieceAt(int file, int rank, const QString& id);

    QString pieceAt(int file, int rank) const;

    // Rimuovi pezzo
    QString clearSquare(int file, int rank);

    // Pulisci tutta la scacchiera
    void clearBoard();
    void resetBoard();

    // Selezione/Highlight manuali (puoi collegarle alla tua logica di mosse)
    void setSelectedSquare(int file, int rank);

    QVector<QPair<int,int>> setLegalMoves(int file, int rank);
    QVector<QPair<int,int>> pieceLegalMoves(int file, int rank, QString col1, QString col2, bool underAttack = false) const;
    QVector<QPair<int,int>> getLegalMoves() const;

    void setLastMove(QPair<int,int> from, QPair<int,int> to);

    QVector<QString> getPreviousPositions() const;
    void addPosition(QString position);

    bool isSquareAttacked(int file, int rank) const;

    QPair<int,int> getPiecePosition(const QString& id) const;

    //Check win condition of check mate / draw by stalemate / draw by 75 moves / draw by repetition
    EndGameCondition isGameFinished();

    //Get or Load pieces positions trough FEN annotation
    QString getFEN() const;
    void loadFEN(QString FEN);

    void checkKingMoves(QString a, int r, int f);

signals:
    // Emette quando l'utente clicca una casella (file: 0..7 = a..h, rank: 0..7 = 1..8 dalla prospettiva del bianco)
    void squareClicked(int file, int rank);

    // Esempio di segnale di richiesta mossa (opzionale): primo click = selezione, secondo = destinazione
    void moveRequested(int fromFile, int fromRank, int toFile, int toRank);

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        // Board area (quadrata, centrata nel widget)
        const int size = qMin(width(), height());
        const int marginX = (width() - size) / 2;
        const int marginY = (height() - size) / 2;
        const int square = size / 8;

        // Disegna caselle
        for (int r = 0; r < 8; ++r) {
            for (int f = 0; f < 8; ++f) {
                const int vFile = viewFile(f);
                const int vRank = viewRank(r);
                const QRect rect(marginX + vFile * square, marginY + (7 - vRank) * square, square, square);
                const bool isLight = ((f + r) % 2 == 1);
                p.fillRect(rect, isLight ? light : dark);
            }
        }

        // Evidenzia ultima mossa
        auto drawSquareOverlay = [&](QPair<int,int> sq, const QColor& c, int alpha){
            if (!inBounds(sq.first, sq.second)) return;
            const int vFile = viewFile(sq.first);
            const int vRank = viewRank(sq.second);
            QRect rect(marginX + vFile * square, marginY + (7 - vRank) * square, square, square);
            QColor cc = c; cc.setAlpha(alpha);
            p.fillRect(rect, cc);
        };
        if (inBounds(lastMove.first.first, lastMove.first.second)) {
            drawSquareOverlay(lastMove.first, lastMoveColor, 120);
            drawSquareOverlay(lastMove.second, lastMoveColor, 120);
        }

        // Evidenzia selezione
        if (inBounds(selected.first, selected.second)) {
            drawSquareOverlay(selected, highlight, 140);
        }

        // Evidenzia mosse legali (pallino al centro)
        for (auto mv : legalMoves) {
            if (!inBounds(mv.first, mv.second)) continue;
            const int vFile = viewFile(mv.first);
            const int vRank = viewRank(mv.second);
            QRect rect(marginX + vFile * square, marginY + (7 - vRank) * square, square, square);
            int d;
            if(this->pieceAt(mv.first, mv.second)!=""){
                p.setPen(QPen(moveColor, 3));
                p.setBrush(Qt::NoBrush);
                d = square / 3;
            } else {
                p.setPen(Qt::NoPen);
                p.setBrush(moveColor);
                d = square / 5;
            }
            const QPoint center = rect.center();
            p.drawEllipse(center, d, d);
        }

        // Disegna pezzi
        for (int r = 0; r < 8; ++r) {
            for (int f = 0; f < 8; ++f) {
                const QString& id = board[r][f];
                if (id.isEmpty() || !pieceImages.contains(id))
                    continue;

                const int vFile = viewFile(f);
                const int vRank = viewRank(r);
                QRect rect(marginX + vFile * square,
                           marginY + (7 - vRank) * square,
                           square, square);

                pieceImages[id]->render(&p, rect);
            }
        }


        // Coordinate bordo (opzionali)
        p.setPen(QPen(QColor(0,0,0,120)));
        QFont tiny = p.font();
        tiny.setPointSizeF(qMax(8.0, square * 0.15));
        p.setFont(tiny);
        for (int i = 0; i < 8; ++i) {
            const int fileIdx = (boardOrientation == Orientation::WhiteBottom) ? i : (7 - i);
            const QChar letter('a' + fileIdx);
            QRect bottom(marginX + i * square, marginY + 7 * square, square, square);
            QRect top(marginX + i * square, marginY, square, square);
            p.drawText(bottom.adjusted(2, 0, -2, -2), Qt::AlignRight | Qt::AlignBottom, QString(letter));

            const int rankIdx = (boardOrientation == Orientation::WhiteBottom) ? i : (7 - i);
            const int number = rankIdx + 1;
            QRect left(marginX, marginY + (7 - i) * square, square, square);
            QRect right(marginX + 7 * square, marginY + (7 - i) * square, square, square);
            p.drawText(left.adjusted(2, 2, 0, 0), Qt::AlignLeft | Qt::AlignTop, QString::number(number));
        }
    }

    void mousePressEvent(QMouseEvent* e) override {
        const int size = qMin(width(), height());
        const int marginX = (width() - size) / 2;
        const int marginY = (height() - size) / 2;
        const int square = size / 8;

        if (e->button() != Qt::LeftButton) return;
        if (e->pos().x() < marginX || e->pos().y() < marginY ||
            e->pos().x() >= marginX + size || e->pos().y() >= marginY + size) return;

        const int vFile = (e->pos().x() - marginX) / square;
        const int vRankFromTop = (e->pos().y() - marginY) / square; // 0 in alto
        const int rank = viewRankInv(7 - vRankFromTop);
        const int file = viewFileInv(vFile);

        if((this->pieceAt(file, rank).startsWith("w") && this->getTurn()==Turn::WhiteTurn)
            || (this->pieceAt(file, rank).startsWith("b") && this->getTurn()==Turn::BlackTurn)
            || selected.first != -1){
            // Comportamento base: primo click seleziona, secondo propone mossa
            if (selected.first == -1) {

                emit squareClicked(file, rank);
                selected = {file, rank};
            } else if(this->legalMoves.contains({file, rank})){
                emit moveRequested(selected.first, selected.second, file, rank);
                selected = {-1, -1};
                legalMoves.clear();
            } else if((this->pieceAt(file, rank).startsWith("w") && this->getTurn()==Turn::WhiteTurn)
                       || (this->pieceAt(file, rank).startsWith("b") && this->getTurn()==Turn::BlackTurn))
            {
                selected = {file, rank};
                legalMoves.clear();
                emit squareClicked(file, rank);

            } else {
                selected = {-1, -1};
                legalMoves.clear();
            }
             update();
        }
    }

private:
    // mapping per orientamento (file/rank logici -> vista)
    int viewFile(int file) const {
        return (boardOrientation == Orientation::WhiteBottom) ? file : (7 - file);
    }
    int viewRank(int rank) const {
        return (boardOrientation == Orientation::WhiteBottom) ? rank : (7 - rank);
    }
    int viewFileInv(int vFile) const { // inversa
        return (boardOrientation == Orientation::WhiteBottom) ? vFile : (7 - vFile);
    }
    int viewRankInv(int vRank) const { // inversa
        return (boardOrientation == Orientation::WhiteBottom) ? vRank : (7 - vRank);
    }

    bool inBounds(int file, int rank) const {
        return file >= 0 && file < 8 && rank >= 0 && rank < 8;
    }


};
#endif // CHESSBOARDWIDGET_H
