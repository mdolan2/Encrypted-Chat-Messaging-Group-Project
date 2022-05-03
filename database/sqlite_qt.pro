QT       += core sql
QT       -= gui

TARGET = sqlite_qt.out
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp dbmanager.cpp

HEADERS += dbmanager.h