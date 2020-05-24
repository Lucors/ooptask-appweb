TEMPLATE = app
CONFIG += console c++11
CONFIG += link_pkgconfig
PKGCONFIG += sqlite3
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L/usr/local/lib -lwt -lwthttp -lboost_regex -lboost_signals
LIBS += -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time

SOURCES += \
        main.cpp \
    bones.cpp \
    main_consolemode.cpp \
    appweb.cpp

HEADERS += \
    bones.h \
    appweb.h
