QT       += core gui serialport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = $$_PRO_FILE_PWD_/bin

include(CommonInterfaces/CommonInterfaces.pri)
include(MyProtokol/xrprotokol.pri)
include(ElemerDevice/ElemerDevice.pri)

#CONFIG += c++17
QMAKE_CXXFLAGS += /std:c++latest # c++20

TARGET = TestADC
TEMPLATE = app

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_DISABLE_DEPRECATED_BEFORE=0x060000 \ # disables all the APIs deprecated before Qt 6.0.0
    EL_LOG

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

