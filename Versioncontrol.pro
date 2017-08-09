#-------------------------------------------------
#
# Project created by QtCreator 2014-03-13T14:15:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Versioncontrol
TEMPLATE = app

win32 {
    win32:RC_ICONS += icon/versioncontrol.ico
}

SOURCES += main.cpp\
        mainwindow.cpp \
    filedownloader.cpp

HEADERS  += mainwindow.h \
    filedownloader.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    icon/versioncontrol.ico \
