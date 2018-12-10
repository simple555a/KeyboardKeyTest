#-------------------------------------------------
#
# Project created by QtCreator 2018-08-01T14:19:36
#
#-------------------------------------------------

QT       += core gui axcontainer xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KeyboardKeyTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./opcClientDLL/opcClient.pri)

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dataandinfofile.cpp \
    findusbreciver.cpp \
    usbconrol.cpp \
    matchcode.cpp \
    maincontrol.cpp \
    keyboardevent.cpp \
    countresult.cpp \
    keyboardpushbutton.cpp \
    plccontrol.cpp \
    keyboardshow.cpp \
    keyboardhook.cpp \
    keyboardvirtualkey.cpp \
    filtervkcode.cpp

HEADERS += \
        mainwindow.h \
    dataandinfofile.h \
    findusbreciver.h \
    golab_define.h \
    usbconrol.h \
    matchcode.h \
    maincontrol.h \
    keyboardevent.h \
    countresult.h \
    keyboardpushbutton.h \
    plccontrol.h \
    keyboardshow.h \
    keyboardhook.h \
    keyboardvirtualkey.h \
    filtervkcode.h

FORMS += \
        mainwindow.ui

LIBS += -lSetupapi -lhid

SUBDIRS += \
    opcClientDLL/opcClientDLL.pro


