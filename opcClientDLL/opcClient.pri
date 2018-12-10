INCLUDEPATH+=$$PWD
DEPENDPATH+=$$PWD
SOURCES += $$PWD/opcclientdll.cpp \
    $$PWD/tagmap/tagmap.cpp \
   $$PWD/opclib/copcdatacallback.cpp \
    $$PWD/opclib/copctransaction.cpp \
    $$PWD/opclib/OPCClient.cpp \
   $$PWD/opclib/opccomn_i.c \
    $$PWD/opclib/Opcda_i.c \
    $$PWD/opclib/OpcEnum_i.c

HEADERS += $$PWD/opcclientdll.h \
    $$PWD/tagmap/tagmap.h \
    $$PWD/opclib/copcdatacallback.h \
    $$PWD/opclib/copctransaction.h \
    $$PWD/opclib/OPCClient.h \
    $$PWD/opclib/opccomn.h \
    $$PWD/opclib/Opcda.h \
    $$PWD/opclib/OpcEnum.h \
    $$PWD/opclib/opcerror.h \
    $$PWD/opclib/unknownbase.h \
    $$PWD/opclib/wldef.h
