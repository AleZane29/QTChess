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

// ------------------------------------------------------------
// ChessBoardWidget
// ------------------------------------------------------------
// • Widget responsive che disegna una scacchiera 8x8.
// • Supporta orientamento (bianco in basso / nero in basso).
// • Evidenzia selezione, mosse possibili e ultima mossa.
// • Gestisce click sulle caselle ed emette i segnali opportuni.
// • Permette di impostare simboli per i pezzi (es. Unicode ♟, ♞, ... o semplici lettere).
// • Nessuna dipendenza extra: solo QtWidgets.
// ------------------------------------------------------------

class ChessBoardWidget : public QWidget {
    Q_OBJECT
public:
    enum class Orientation { WhiteBottom, BlackBottom };

    explicit ChessBoardWidget(QWidget* parent = nullptr)
        : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(240, 240);
        clearBoard();
    }

    // Imposta orientamento della scacchiera
    void setOrientation(Orientation o) { orientation_ = o; update(); }
    Orientation orientation() const { return orientation_; }

    // Colori personalizzabili
    void setLightColor(const QColor& c) { light_ = c; update(); }
    void setDarkColor(const QColor& c) { dark_ = c; update(); }
    void setHighlightColor(const QColor& c) { highlight_ = c; update(); }
    void setMoveColor(const QColor& c) { moveColor_ = c; update(); }
    void setLastMoveColor(const QColor& c) { lastMoveColor_ = c; update(); }

    // Imposta un pezzo (stringa disegnata al centro casella). Esempi: "♙", "♞", "K", "q" ecc.
    // Carica immagini (una volta sola all’avvio)
    void loadPieceImage(const QString& id, const QString& filePath) {
        auto* renderer = new QSvgRenderer(filePath, this);
        if (renderer->isValid()) {
            pieceImages_[id] = renderer;
        } else {
            delete renderer;
        }
    }

    // Usa id per piazzare pezzo
    void setPieceAt(int file, int rank, const QString& id) {
        if (!inBounds(file, rank)) return;
        board_[rank][file] = id;
        update();
    }


    QString pieceAt(int file, int rank) const {
        if (!inBounds(file, rank)) return {};
        return board_[rank][file];
    }

    // Rimuovi pezzo
    void clearSquare(int file, int rank) {
        if (!inBounds(file, rank)) return;
        board_[rank][file].clear();
        update();
    }

    // Pulisci tutta la scacchiera
    void clearBoard() {
        for (auto& row : board_) row.fill("");
        selected_ = {-1, -1};
        legalMoves_.clear();
        lastMove_ = {{-1, -1}, {-1, -1}};
        update();
    }

    // Selezione/Highlight manuali (puoi collegarle alla tua logica di mosse)
    void setSelectedSquare(int file, int rank) {
        if (inBounds(file, rank)) selected_ = {file, rank};
        else selected_ = {-1, -1};
        update();
    }

    void setLegalMoves(const QVector<QPair<int,int>>& moves) { // vettore di (file, rank)
        legalMoves_ = moves;
        update();
    }

    void setLastMove(QPair<int,int> from, QPair<int,int> to) {
        lastMove_ = {from, to};
        update();
    }

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
                const bool isLight = ((f + r) % 2 == 0);
                p.fillRect(rect, isLight ? light_ : dark_);
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
        if (inBounds(lastMove_.first.first, lastMove_.first.second)) {
            drawSquareOverlay(lastMove_.first, lastMoveColor_, 120);
            drawSquareOverlay(lastMove_.second, lastMoveColor_, 120);
        }

        // Evidenzia selezione
        if (inBounds(selected_.first, selected_.second)) {
            drawSquareOverlay(selected_, highlight_, 140);
        }

        // Evidenzia mosse legali (pallino al centro)
        p.setPen(Qt::NoPen);
        for (auto mv : legalMoves_) {
            if (!inBounds(mv.first, mv.second)) continue;
            const int vFile = viewFile(mv.first);
            const int vRank = viewRank(mv.second);
            QRect rect(marginX + vFile * square, marginY + (7 - vRank) * square, square, square);
            p.setBrush(moveColor_);
            const int d = square / 5;
            const QPoint center = rect.center();
            p.drawEllipse(center, d, d);
        }

        // Disegna pezzi
        for (int r = 0; r < 8; ++r) {
            for (int f = 0; f < 8; ++f) {
                const QString& id = board_[r][f];
                if (id.isEmpty() || !pieceImages_.contains(id))
                    continue;

                const int vFile = viewFile(f);
                const int vRank = viewRank(r);
                QRect rect(marginX + vFile * square,
                           marginY + (7 - vRank) * square,
                           square, square);

                pieceImages_[id]->render(&p, rect);
            }
        }


        // Coordinate bordo (opzionali)
        p.setPen(QPen(QColor(0,0,0,120)));
        QFont tiny = p.font();
        tiny.setPointSizeF(qMax(8.0, square * 0.15));
        p.setFont(tiny);
        for (int i = 0; i < 8; ++i) {
            // File letters a..h dal punto di vista del bianco
            const int fileIdx = (orientation_ == Orientation::WhiteBottom) ? i : (7 - i);
            const QChar letter('a' + fileIdx);
            QRect bottom(marginX + i * square, marginY + 7 * square, square, square);
            QRect top(marginX + i * square, marginY, square, square);
            p.drawText(bottom.adjusted(2, 0, -2, -2), Qt::AlignRight | Qt::AlignBottom, QString(letter));

            // Rank numbers 1..8 dal punto di vista del bianco
            const int rankIdx = (orientation_ == Orientation::WhiteBottom) ? i : (7 - i);
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

        emit squareClicked(file, rank);

        // Comportamento base: primo click seleziona, secondo propone mossa
        if (selected_.first == -1) {
            selected_ = {file, rank};
            update();
        } else {
            emit moveRequested(selected_.first, selected_.second, file, rank);
            selected_ = {-1, -1};
            legalMoves_.clear();
            update();
        }
    }

private:
    // mapping per orientamento (file/rank logici -> vista)
    int viewFile(int file) const {
        return (orientation_ == Orientation::WhiteBottom) ? file : (7 - file);
    }
    int viewRank(int rank) const {
        return (orientation_ == Orientation::WhiteBottom) ? rank : (7 - rank);
    }
    int viewFileInv(int vFile) const { // inversa
        return (orientation_ == Orientation::WhiteBottom) ? vFile : (7 - vFile);
    }
    int viewRankInv(int vRank) const { // inversa
        return (orientation_ == Orientation::WhiteBottom) ? vRank : (7 - vRank);
    }

    bool inBounds(int file, int rank) const {
        return file >= 0 && file < 8 && rank >= 0 && rank < 8;
    }

private:
    Orientation orientation_ = Orientation::WhiteBottom;
    QColor light_ = QColor(240, 217, 181); // stile legno chiaro
    QColor dark_  = QColor(181, 136, 99);  // stile legno scuro
    QColor highlight_ = QColor(30, 144, 255); // selezione
    QColor moveColor_ = QColor(0, 0, 0, 160); // pallino mosse
    QColor lastMoveColor_ = QColor(255, 215, 0); // giallo trasparente

    // board_[rank][file] -> simbolo pezzo (string)
    std::array<std::array<QString, 8>, 8> board_{};
    QHash<QString, QSvgRenderer*> pieceImages_;
    QPair<int,int> selected_{-1, -1};
    QVector<QPair<int,int>> legalMoves_;
    QPair<QPair<int,int>, QPair<int,int>> lastMove_{{-1,-1},{-1,-1}};
};

/* ------------------------------------------------------------
USO RAPIDO (esempio):

#include <QApplication>
#include <QMainWindow>

int main(int argc, char** argv){
    QApplication app(argc, argv);
    QMainWindow w;
    auto* board = new ChessBoardWidget;

    // piazza qualche pezzo con unicode
    board->setPieceAt(4, 0, "♚"); // re nero e8
    board->setPieceAt(4, 7, "♔"); // re bianco e1
    for (int f=0; f<8; ++f) {
        board->setPieceAt(f, 1, "♟"); // pedoni neri 2a traversa
        board->setPieceAt(f, 6, "♙"); // pedoni bianchi 7a
    }

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
    return app.exec();
}

NOTE:
- I simboli Unicode potrebbero variare come font. Per resa migliore puoi impostare un QFont dedicato (es. "Noto Sans Symbols2") o usare pixmap SVG/PNG.
- Collega la tua logica di generazione mosse: quando selezioni un pezzo, chiama setLegalMoves([...]) per mostrare i target possibili.
- setOrientation(ChessBoardWidget::Orientation::BlackBottom) per ruotare la scacchiera.
------------------------------------------------------------ */


#endif // CHESSBOARDWIDGET_H
