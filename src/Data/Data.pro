include($$PWD/../3rdParty/QxOrm/QxOrm.pri)

QT += network widgets xml

TARGET      = Data
TEMPLATE    = lib
DEFINES    += DATA_LIBRARY
CONFIG     += c++11
unix:!macx: PRECOMPILED_HEADER = stable.h

unix:!macx: CONFIG   += precompile_header

DESTDIR = $$OUT_PWD/..

win32 {
    LIBS += -L$$OUT_PWD/../../libs/win32/ -lQxOrm
}
unix:!macx: LIBS += -L$$PWD/../../libs/linux/QxOrm/ -lQxOrm

win32: QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES    += \
    debugAsserts.cpp \
    GoogleContacts.cpp \
    Auth/AuthServer.cpp \
    Auth/AuthSettings.cpp \
    Auth/AuthManager.cpp \
    Model/ContactEntry.cpp \
    Model/User.cpp \
    Database.cpp \
    Model/ContactProperty.cpp \
    Model/RelValue.cpp \
    Model/ContactGroup.cpp

HEADERS    += \
    debugAsserts.h \
    debugBreak.h \
    GoogleContacts.h \
    Data_global.h \
    Auth/AuthServer.h \
    Auth/AuthSettings.h \
    Auth/AuthManager.h \
    Model/ContactEntry.h \
    Model/User.h \
    Database.h \
    Model/ContactProperty.h \
    stable.h \
    Model/RelValue.h \
    Model/ContactGroup.h

RESOURCES += \
    ../../resources/resources.qrc

INCLUDEPATH += $$PWD/../ \
    ../3rdParty/QxOrm/include/ \
    ../3rdParty/
