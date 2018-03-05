SOURCES_COMMON = \
    $$PWD/utility.cpp

HEADERS_COMMON += \
    $$PWD/utility.h 
#不填下面这句，会出现找不到头文件的问题
INCLUDEPATH += $$PWD
