QT += core
QT += gui #不然QImage用不了

TARGET = Algo3
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH+=$$PWD/library/win32/opencv/include
             $$PWD/library/win32/opencv/include/opencv
             $$PWD/library/win32/opencv/include/opencv2
include($$PWD/common/common.pri)

SOURCES += main.cpp \
    algoc.cpp
SOURCES += $${SOURCES_COMMON}

HEADERS += \
    algoc.h \
    watersshedsegmenter.h
HEADERS += $${HEADERS_COMMON}

LIBS+=-L $$PWD/library/win32/opencv/lib/libopencv_*.a
         $$PWD/library/win32/opencv/libopencv_*.dll

