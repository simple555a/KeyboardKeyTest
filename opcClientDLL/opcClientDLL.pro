#-------------------------------------------------
#
# Project created by QtCreator 2017-06-28T14:20:35
#
#-------------------------------------------------

QT       += gui  core xml
QT       += axcontainer

TARGET = opcClientDLL
TEMPLATE = lib

DEFINES += OPCCLIENTDLL_LIBRARY

SOURCES += opcclientdll.cpp \
    tagmap/tagmap.cpp \
    opclib/copcdatacallback.cpp \
    opclib/copctransaction.cpp \
    opclib/OPCClient.cpp \
    opclib/opccomn_i.c \
    opclib/Opcda_i.c \
    opclib/OpcEnum_i.c

HEADERS += opcclientdll.h \
    tagmap/tagmap.h \
    opclib/copcdatacallback.h \
    opclib/copctransaction.h \
    opclib/OPCClient.h \
    opclib/opccomn.h \
    opclib/Opcda.h \
    opclib/OpcEnum.h \
    opclib/opcerror.h \
    opclib/unknownbase.h \
    opclib/wldef.h

