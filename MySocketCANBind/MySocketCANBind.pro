#-------------------------------------------------
#
# Project created by QtCreator 2017-01-06T11:39:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySocketCANBind
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    receivecanframes.cpp \
    processcanmessage.c

HEADERS  += mainwindow.h \
    receivecanframes.h \
    processcanmessage.h

FORMS    += mainwindow.ui
