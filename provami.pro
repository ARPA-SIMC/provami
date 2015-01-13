#-------------------------------------------------
#
# Project created by QtCreator 2013-08-27T16:38:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = provami
TEMPLATE = app


SOURCES += main.cpp\
        provamimainwindow.cpp \
    model.cpp \
    datagridmodel.cpp

HEADERS  += provamimainwindow.h \
    model.h \
    datagridmodel.h

FORMS    += provamimainwindow.ui
