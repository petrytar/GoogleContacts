QT += network widgets xml

TARGET      = Data
TEMPLATE    = lib
DEFINES    += DATA_LIBRARY
CONFIG     += c++11

DESTDIR = $$OUT_PWD/..

SOURCES    += \
    debugAsserts.cpp \
    GoogleContacts.cpp \
    Auth/AuthServer.cpp \
    Auth/AuthSettings.cpp \
    Auth/AuthManager.cpp \
    ContactEntry.cpp

HEADERS    += \
    debugAsserts.h \
    debugBreak.h \
    GoogleContacts.h \
    Data_global.h \
    Auth/AuthServer.h \
    Auth/AuthSettings.h \
    Auth/AuthManager.h \
    ContactEntry.h

RESOURCES += \
    ../../resources/resources.qrc

INCLUDEPATH += $$PWD/../
