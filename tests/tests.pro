CONFIG += testcase c++11 link_pkgconfig
PKGCONFIG += libwibble

TARGET = test-provami

LIBS += -L../provami -lprovami

SOURCES += tests.cpp tut-main.cpp

HEADERS += tests.h

include(../defines.inc)
