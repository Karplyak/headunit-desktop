TEMPLATE = lib
CONFIG += c++11 plugin link_pkgconfig
QT += quick
TARGET = $$qtLibraryTarget(pult-plugin)
DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -li2c /usr/lib/libwiringx.so
INCLUDEPATH += $${PWD}/../../includes
DESTDIR = $${OUT_PWD}/../../plugins

include("../../config.pri")

target.path = $${PREFIX}/plugins

INSTALLS += target

SOURCES += \
    pultplugin.cpp

HEADERS += \
    pultplugin.h

DISTFILES += \
    config.json

RESOURCES += \
    qml.qrc
