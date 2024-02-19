#-------------------------------------------------
#
# Project created by QtCreator 2015-06-13T23:45:28
#
#-------------------------------------------------

QT       += core gui xml widgets

Debug:CONFIG += console

include(../qtpropertybrowser/src/qtpropertybrowser.pri)
TARGET = qdraw
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    drawobj.cpp \
    drawscene.cpp \
    drawtool.cpp \
    sizehandle.cpp \
    rulebar.cpp \
    drawview.cpp \
    commands.cpp \
    materials.cpp \
    context.cpp \
    commandwidget.cpp \
    document.cpp

HEADERS  += mainwindow.h \
    drawobj.h \
    drawscene.h \
    drawtool.h \
    sizehandle.h \
    rulebar.h \
    drawview.h \
    commands.h \
    materials.h \
    context.h \
    commandwidget.h \
    document.h

RESOURCES += \
    app.qrc
