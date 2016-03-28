include($$PWD/../../../QxOrm/QxOrm.pri)

QT += network widgets xml

TARGET      = Data
TEMPLATE    = lib
DEFINES    += DATA_LIBRARY
CONFIG     += c++11

DESTDIR = $$OUT_PWD/..

win32 {
    LIBS += -L$$OUT_PWD/../../libs/win32/ -lQxOrm
}

SOURCES    += \
    debugAsserts.cpp \
    GoogleContacts.cpp \
    Auth/AuthServer.cpp \
    Auth/AuthSettings.cpp \
    Auth/AuthManager.cpp \
    ContactEntry.cpp \
    Model/User.cpp \
    Model/Database.cpp

HEADERS    += \
    debugAsserts.h \
    debugBreak.h \
    GoogleContacts.h \
    Data_global.h \
    Auth/AuthServer.h \
    Auth/AuthSettings.h \
    Auth/AuthManager.h \
    ContactEntry.h \
    Model/User.h \
    Model/Database.h

RESOURCES += \
    ../../resources/resources.qrc

INCLUDEPATH += $$PWD/../ \
    ../3rdparty/QxOrm/include/ \
    ../3rdparty/


