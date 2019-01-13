TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt

SOURCES += main.cpp \
    LoadLadyBugPgr.cpp

HEADERS += \
    LoadLadyBugPgr.h

INCLUDEPATH += "D:\Program Files\Point Grey Research\Ladybug\include"
INCLUDEPATH += "D:\Users\jxl11\Downloads\opencv\build\include"
LIBS += -L"D:\Program Files\Point Grey Research\Ladybug\lib64" -lladybug
LIBS += -L"D:/Program Files/Point Grey Research/Ladybug/bin64"
LIBS += -L"D:\Program Files\Point Grey Research\Ladybug\bin64"  -lladybug



