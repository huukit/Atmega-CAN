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



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-libCanBridge-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Debug/release/ -llibCanBridge
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-libCanBridge-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Debug/debug/ -llibCanBridge

INCLUDEPATH += $$PWD/../libCanBridge
DEPENDPATH += $$PWD/../libCanBridge
