include($$PWD/../3rdParty/QxOrm/QxOrm.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += core network

TARGET      = gcontacts
TEMPLATE    = app
CONFIG     += c++11 console

win32 {
    DESTDIR = $$OUT_PWD/..
}

win32: QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES    += \
    MainConsole.cpp \
    ConsoleOperator.cpp

HEADERS    += \
    MainAppConsole_global.h \
    ConsoleOperator.h

win32 {
    LIBS += -L$$OUT_PWD/../../libs/win32/ -llibeay32 -lssleay32
}
unix:!macx: LIBS += -L$$PWD/../../libs/linux/QxOrm/ -lQxOrm

LIBS += -L$$OUT_PWD/../ -lData

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

INCLUDEPATH += $$PWD/../ \
    ../3rdParty/QxOrm/include/ \
    ../3rdParty/
