include($$PWD/../3rdParty/QxOrm/QxOrm.pri)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core gui network webkit webkitwidgets

TARGET      = MainApp
TEMPLATE    = app
CONFIG     += c++11

win32 {
    DESTDIR = $$OUT_PWD/..
}

SOURCES    += \
    MainWindow.cpp \
    Main.cpp \
    LoginDialog.cpp

HEADERS    += \
    MainWindow.h \
    LoginDialog.h

FORMS      += \
    MainWindow.ui \
    LoginDialog.ui

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
