TEMPLATE = subdirs
SUBDIRS += provami src tests

src.depends = provami
tests.depends = provami

DISTFILES += mapview

include(defines.inc)

data.path = $$PREFIX/share/provami
data.files = mapview

INSTALLS += target data

