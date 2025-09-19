#ifndef ENGINE_H
#define ENGINE_H
#include "chessboardwidget.h"
#include <QPair>
#include <QString>
#include <QVector>

class Engine
{
private:
    ChessBoardWidget *board;
    //Parameters for evaluation
    const QMap<QString, int>piecesValue = {{"k", 200}, {"q",9}, {"r",5}, {"b",3}, {"k",3}, {"p",1}};

public:
    Engine();
    void setBoard(ChessBoardWidget *board);
    float evalutePosition(QString position);
    float minimax(QString position, int depth, float alpha, float beta, bool whiteTurn);
};

#endif // ENGINE_H
