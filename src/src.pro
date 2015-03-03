TEMPLATE = app
QT += core gui widgets webkitwidgets
CONFIG += link_pkgconfig c++11
PKGCONFIG += libdballe
TARGET = provami-qt
INCLUDEPATH += ..
LIBS += -L../provami -lprovami

SOURCES += main.cpp

INSTALLS += target
target.path = $${PREFIX}/bin

include(../defines.inc)
