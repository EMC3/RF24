#-------------------------------------------------
#
# Project created by QtCreator 2018-08-29T21:15:16
#
#-------------------------------------------------

QT       -= core gui

TARGET = rfio
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += rfio.cpp

HEADERS += 

INCLUDEPATH += ../
win32{
INCLUDEPATH += D:\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\install_root\include
}
unix {
    target.path = /usr/lib
    INSTALLS += target
INCLUDEPATH += /mnt/hdd/sources/mlibs/
LIBS += -L.. -lRF24
}
