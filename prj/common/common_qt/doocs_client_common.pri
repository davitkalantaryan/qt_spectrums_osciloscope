#
# File doocs_client_common.pri
# File created : 12 Feb 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
#

message("!!! doocs_client_common.pri:")

#QMAKE_CXXFLAGS_WARN_ON += -Wno-attributes

include(../../common/common_qt/sys_common.pri)


win32{
	MYDOOCS =  = D:/davit/dev/udoocs
	SYSTEM_LIB = $$MYDOOCS/sys/win_x64/lib
	INCLUDEPATH += $$MYDOOCS/contrib/doocs/clientlib/include
	INCLUDEPATH += $$MYDOOCS/contrib/doocs/serverlib/include
	INCLUDEPATH += $$MYDOOCS/contrib/zmq/include
} else {
	GCCPATH = $$system(which gcc)
	message("!!! GCCPATH="$$GCCPATH)
	MYDOOCS = /afs/ifh.de/group/pitz/doocs
	SYSTEM_LIB = $$MYDOOCS/system_arch/$$CODENAME/lib
	LIBS += -lldap
	LIBS += -lrt
}
INCLUDEPATH += $$MYDOOCS/include/doocs

# message ("!!!!! No cpp 11 used") # todo: calculate in the sys_common.pri
#QMAKE_CXXFLAGS += -std=c++0x

CONFIG += c++14

DEFINES += LINUX

#LIBS += -L/doocs/lib
LIBS += -L$$SYSTEM_LIB
#LIBS += -L/doocs/develop/kalantar/programs/cpp/works/sys/$$CODENAME/lib
#LIBS += -L/doocs/develop/bagrat/doocs.git/amd64_rhel60/lib
LIBS += -lDOOCSapi
LIBS +=	-lgul14
