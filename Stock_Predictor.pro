TEMPLATE = app
TARGET = "Stock Predictor"

QT += core gui

QT += charts
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11

SOURCES += \
    datamining.cpp \
    get_data.cpp \
    listmanager.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    datamining.h \
    get_data.h \
    listmanager.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
