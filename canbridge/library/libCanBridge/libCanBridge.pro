#-------------------------------------------------
#
# Project created by QtCreator 2014-10-10T14:15:30
#
#-------------------------------------------------

QT       -= gui

TARGET = libCanBridge
TEMPLATE = lib

DEFINES += LIBCANBRIDGE_LIBRARY

SOURCES += libcanbridge.cpp

HEADERS += libcanbridge.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32: LIBS += -L$$PWD/../libusb-1.0.19-rc1-win/MS32/dll/ -llibusb-1.0

INCLUDEPATH += $$PWD/../libusb-1.0.19-rc1-win/include/libusb-1.0
DEPENDPATH += $$PWD/../libusb-1.0.19-rc1-win/include/libusb-1.0
