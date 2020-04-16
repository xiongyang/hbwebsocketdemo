QT -= gui
QT += network websockets
CONFIG += c++14 console
CONFIG -= app_bundle

win32 {

INCLUDEPATH +="E:\boost_1_65_1"
LIBS += -LE:\boost_1_65_1\lib64-msvc-14.1

INCLUDEPATH +="C:\work\lib\boost_1_66_0"
LIBS += -LC:\work\lib\boost_1_66_0/stage/lib

INCLUDEPATH +="D:\boost_1_67_0"
LIBS += -LD:\boost_1_67_0\stage\lib
#DEFINES += BOOST_LOG_DYN_LINK
DEFINES +=_WIN32_WINNT=0x0601

#QMAKE_CXXFLAGS += /MP
}


unix
{

INCLUDEPATH +=$$(BOOST_PATH)
DEFINES += BOOST_LOG_DYN_LINK linux
LIBS += -L$$(BOOST_PATH)/stage/lib -lboost_system -lboost_iostreams
LIBS += -lrt -lz 
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        hbwebsocketauth.cpp \
        main.cpp \
        mxcwebsocket.cpp \
        zbwebsocket.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    hbwebsocketauth.h \
    mxcwebsocket.h \
    zbwebsocket.h
