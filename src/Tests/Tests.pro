include($$PWD/../3rdParty/QxOrm/QxOrm.pri)

QT += network widgets xml testlib

TARGET      = Tests
TEMPLATE = lib
CONFIG     += c++11
unix:!macx: CONFIG   += precompile_header

DESTDIR = $$OUT_PWD/..

win32 {
    LIBS += -L$$OUT_PWD/../../libs/win32/ -lQxOrm
}
unix:!macx: LIBS += -L$$PWD/../../libs/linux/QxOrm/ -lQxOrm

win32: QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES    += \
    Test_GoogleContacts.cpp \
    Test_Database.cpp

HEADERS    += \
    Test_GoogleContacts.h \
    Test_Database.h

RESOURCES += \
    ../../resources/resources.qrc

LIBS += -L$$OUT_PWD/../ -lData

INCLUDEPATH += $$PWD/../ \
    ../3rdParty/QxOrm/include/ \
    ../3rdParty/
