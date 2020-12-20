QT -= gui

CONFIG += console no_keywords
CONFIG -= app_bundle
QT += websockets

QMAKE_CXXFLAGS += -std=c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 liblog4cxx nvmr-json dbus-cxx-qt-2.0

SOURCES += \
        main.cpp \
    videosender.cpp \
    configurationserver.cpp \
    avahi-dbus/EntryGroupProxy.cpp \
    avahi-dbus/ServerProxy.cpp \
    avahi-dbus/org_freedesktop_Avahi_ServerProxy.cpp \
    avahi-dbus/org_freedesktop_Avahi_Server2Proxy.cpp \
    avahi-dbus/org_freedesktop_Avahi_EntryGroupProxy.cpp \
    avahi.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    videosender.h \
    configurationserver.h \
    avahi-dbus/EntryGroupProxy.h \
    avahi-dbus/ServerProxy.h \
    avahi-dbus/org_freedesktop_Avahi_ServerProxy.h \
    avahi-dbus/org_freedesktop_Avahi_Server2Proxy.h \
    avahi-dbus/org_freedesktop_Avahi_EntryGroupProxy.h \
    avahi.h
