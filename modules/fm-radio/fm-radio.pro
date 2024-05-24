TEMPLATE = lib
CONFIG += c++11 plugin link_pkgconfig
QT += quick
TARGET = $$qtLibraryTarget(fm-radio-plugin)
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Wno-unused-parameter
INCLUDEPATH += $${PWD}/../../includes
DESTDIR = $${OUT_PWD}/../../plugins

include("../../config.pri")

target.path = $${PREFIX}/plugins

INSTALLS += target

LIBS += -li2c

SOURCES += \
    backends/rda5807m/RDA5807M.cpp \
    fmradioplugin.cpp

HEADERS += \
    backends/radiointerface.h \
    backends/rda5807m/RDA5807M.h \
    fmradioplugin.h

DISTFILES += \
    config.json

RESOURCES += \
    qml.qrc

