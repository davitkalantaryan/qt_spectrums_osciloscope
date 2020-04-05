#
# File daq_browser_bin_for_matlab.pro
# File created : 01 Jan 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
#

TEMPLATE = lib

DEFINES += DEBUG_APPLICATION

message("!!! daq_browser_bin_for_matlab.pro")
QMAKE_CXXFLAGS += -std=c++0x

QMAKE_CXXFLAGS_WARN_ON += -Wno-implicit-fallthrough

win32{
    SOURCES += \

} else {
    GCCPATH = $$system(which gcc)
    message("!!!!!!!!!!! GCCPATH=$$GCCPATH")
    #QMAKE_CXXFLAGS += -std=c++11
    SOURCES += \

}

include ( $${PWD}/../../common/common_qt/sys_common.pri)

win32{
    MYDOOCS =  = D:/davit/dev/udoocs
    SYSTEM_LIB = $$MYDOOCS/sys/win_x64/lib
    INCLUDEPATH += $$MYDOOCS/contrib/doocs/clientlib/include
    INCLUDEPATH += $$MYDOOCS/contrib/doocs/serverlib/include
    INCLUDEPATH += $$MYDOOCS/contrib/zmq/include

} else {
    GCCPATH = $$system(which gcc)
    MYDOOCS = /afs/ifh.de/group/pitz/doocs
    SYSTEM_LIB = $$MYDOOCS/system_arch/$$CODENAME/lib
    LIBS += -lldap
    LIBS += -lrt

}
INCLUDEPATH += $$MYDOOCS/include/doocs

# message ("!!!!! No cpp 11 used") # todo: calculate in the sys_common.pri
#QMAKE_CXXFLAGS += -std=c++0x

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

DEFINES += DCS_CLNT_DYN_CREATING_LIB

CONFIG += c++14

INCLUDEPATH += $${PWD}/../../../include
#INCLUDEPATH += $${PWD}/../../../src/include_p
#INCLUDEPATH += /afs/ifh.de/group/pitz/doocs/include/doocs

SOURCES += \
	$${PWD}/../../../src/libs/entry_doocsclient_dynamic.cpp

HEADERS += \
	$${PWD}/../../../include/doocsclient_dynamic.h

OTHER_FILES += \
