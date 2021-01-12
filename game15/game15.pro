TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC

SOURCES += \
        main.cpp \
    board.cpp

HEADERS += \
    board.hh
