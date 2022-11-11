QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGL_1
TEMPLATE = app

CONFIG += c++14

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mainview.cpp \
    user_input.cpp \
    model.cpp

HEADERS += \
    mainwindow.h \
    mainview.h \
    model.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc
