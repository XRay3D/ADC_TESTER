QT       += core gui serialport multimedia
#CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = $$_PRO_FILE_PWD_/bin

include(devices/CommonInterfaces/CommonInterfaces.pri)
include(devices/MyProtokol/myprotokol.pri)
include(devices/ElemerDevice/ElemerDevice.pri)

#CONFIG += c++17
QMAKE_CXXFLAGS += /std:c++latest
QMAKE_CXXFLAGS += /await
DEFINES += __cpp_lib_coroutine

TARGET = TestADC
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adcdatamodel.cpp \
    devices/devices.cpp \
    devices/irt5501.cpp \
    devices/irt5920.cpp \
    devices/tester.cpp \
    main.cpp \
    mainwindow.cpp \
    myled.cpp \
    testerth.cpp \

HEADERS += \
    adcdatamodel.h \
    devices/devices.h \
    devices/irt5501.h \
    devices/irt5920.h \
    devices/tester.h \
    mainwindow.h \
    myled.h \
    testerth.h \


FORMS += \
    mainwindow.ui

DEFINES += \
#    EL_ALWAYS_OPEN \
    EL_LOG
