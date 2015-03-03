TEMPLATE = lib

QT += core gui widgets webkitwidgets
CONFIG += link_pkgconfig c++11
PKGCONFIG += libdballe
TARGET = provami
DEPENDPATH += .
INCLUDEPATH += ..

# Input
HEADERS += types.h \
           highlight.h \
           datagridmodel.h \
           datagridview.h \
           filtercombobox.h \
           mapview.h \
           model.h \
           provamimainwindow.h \
           recordlineedit.h \
           stationgridmodel.h \
           attrgridmodel.h \
           dateedit.h \
           rawquerymodel.h \
           refreshthread.h \
           progressindicator.h \
           filters.h \
           config.h \
           mapcontroller.h
FORMS += provamimainwindow.ui
SOURCES += types.cpp \
           highlight.cpp \
           datagridmodel.cpp \
           datagridview.cpp \
           filtercombobox.cpp \
           mapview.cpp \
           model.cpp \
           provamimainwindow.cpp \
           recordlineedit.cpp \
           stationgridmodel.cpp \
           attrgridmodel.cpp \
           dateedit.cpp \
           rawquerymodel.cpp \
           refreshthread.cpp \
           progressindicator.cpp \
           filters.cpp \
           config.cpp \
           mapcontroller.cpp

INSTALLS += target
target.path = $${PREFIX}/lib

include(../defines.inc)
