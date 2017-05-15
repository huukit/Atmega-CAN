#-------------------------------------------------
#
# Project created by QtCreator 2014-10-10T14:29:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CanBridgeTester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#Include libusb. Do this or copy libusb.dll to your bould directory.
win32:CONFIG(release, debug|release): win32: LIBS += -L$$PWD/../libusb-1.0.19-rc1-win/MS/dll/ -llibusb-1.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-libCanBridge-Desktop_Qt_5_8_0_MinGW_32bit-Debug/release/ -llibCanBridge
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-libCanBridge-Desktop_Qt_5_8_0_MinGW_32bit-Debug/debug/ -llibCanBridge

INCLUDEPATH += $$PWD/../libCanBridge
DEPENDPATH += $$PWD/../libCanBridge
