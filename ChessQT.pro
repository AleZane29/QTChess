QT       += core gui
QT += svg


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chessboardwidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    chessboardwidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources/bp.svg \
    resources/bk.svg \
    resources/bq.svg \
    resources/bn.svg \
    resources/br.svg \
    resources/bb.svg \
    resources/wp.svg \
    resources/wk.svg \
    resources/wq.svg \
    resources/wn.svg \
    resources/wr.svg \
    resources/wb.svg

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


