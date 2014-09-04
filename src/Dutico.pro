#-------------------------------------------------
#
# Project created by QtCreator 2014-03-30T15:54:54
#
#-------------------------------------------------

QT       += core gui \
        sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dutico
TEMPLATE = app

CONFIG += static

SOURCES += main.cpp\
        mainwindow.cpp \
    qfichier.cpp \
    editlayer.cpp \
    preferences.cpp \
    dufsqlquery.cpp \
    about.cpp \
    addclip.cpp \
    delclip.cpp \
    timecodemanager.cpp

HEADERS  += mainwindow.h \
    qfichier.h \
    editlayer.h \
    preferences.h \
    dufsqlquery.h \
    about.h \
    addclip.h \
    delclip.h \
    timecodemanager.h

FORMS    += mainwindow.ui \
    editlayer.ui \
    preferences.ui \
    about.ui \
    addclip.ui \
    delclip.ui

RESOURCES += \
    ressources.qrc

OTHER_FILES += \
    ico.rc

RC_FILE = ico.rc
