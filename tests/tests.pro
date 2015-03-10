QT += core gui widgets webkitwidgets
CONFIG += testcase c++11 link_pkgconfig
PKGCONFIG += libwibble libdballe
INCLUDEPATH += ..

TARGET = test-provami

LIBS += -L../provami -lprovami

SOURCES += tests.cpp tut-main.cpp \
    model-tut.cpp

HEADERS += tests.h

include(../defines.inc)
