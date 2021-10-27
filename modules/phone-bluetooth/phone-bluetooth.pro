TEMPLATE = lib
CONFIG += c++11 plugin link_pkgconfig
QT += qml quick bluetooth dbus BluezQt
TARGET = $$qtLibraryTarget(phone-bluetooth-plugin)
DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += $${PWD}/../../includes
DESTDIR = $${OUT_PWD}/../../plugins
QMAKE_LIBS += -lKF5BluezQt


SOURCES += \
    ofono_dbus_types.cpp \
    ofonomanager.cpp \
    telephonymanager.cpp \
    bluezagent.cpp

include("../../config.pri")

target.path = $${PREFIX}/plugins

INSTALLS += target

HEADERS += \
    ofono_dbus_types.h \
    ofonomanager.h \
    telephonymanager.h \
    bluezagent.h

DISTFILES += \
    config.json

RESOURCES += \
    phonebluetooth.qrc

DBUS_INTERFACES += dbus_interfaces/org.bluez.obex.PhonebookAccess1.xml
DBUS_INTERFACES += dbus_interfaces/org.ofono.Modem.xml
DBUS_INTERFACES += dbus_interfaces/org.ofono.Handsfree.xml
DBUS_INTERFACES += dbus_interfaces/org.ofono.VoiceCall.xml

ofon_manager.files = dbus_interfaces/org.ofono.Manager.xml
ofon_manager.header_flags = -i ofono_dbus_types.h
DBUS_INTERFACES += ofon_manager

ofon_voicecallmanager.files = dbus_interfaces/org.ofono.VoiceCallManager.xml
ofon_voicecallmanager.header_flags = -i ofono_dbus_types.h
DBUS_INTERFACES += ofon_voicecallmanager

ofon_networkregistartion.files = dbus_interfaces/org.ofono.NetworkRegistration.xml
ofon_networkregistartion.header_flags = -i ofono_dbus_types.h
DBUS_INTERFACES += ofon_networkregistartion
