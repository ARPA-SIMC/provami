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
    datagridmodel.cpp \
    mapscene.cpp \
    mapview.cpp \
    recordlineedit.cpp \
    datagridview.cpp \
    filtercombobox.cpp

HEADERS  += provamimainwindow.h \
    model.h \
    datagridmodel.h \
    mapview.h \
    mapscene.h \
    mapview.h \
    recordlineedit.h \
    datagridview.h \
    filtercombobox.h

FORMS    += provamimainwindow.ui
