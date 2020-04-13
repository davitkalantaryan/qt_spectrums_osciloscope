#
# File docs_zmq_reader.pro
# File created : 01 Apr 2020
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used ...
# for PITZ
#

DEFINES += DEBUG_APPLICATION

message("!!! docs_zmq_reader.pro")
#QMAKE_CXXFLAGS += -std=c++0x
#QMAKE_CXXFLAGS_WARN_ON += -Wno-implicit-fallthrough


#include ( $${PWD}/../../../contrib/matlab/prj/common/common_qt/matlab_matrix_common.pri )
include ( $${PWD}/../../common/common_qt/sys_common.pri)

# DOOCS part is here
win32{
    MYDOOCS =  = D:/davit/dev/udoocs
    SYSTEM_LIB = $$MYDOOCS/sys/win_x64/lib
    INCLUDEPATH += $$MYDOOCS/contrib/doocs/clientlib/include
    INCLUDEPATH += $$MYDOOCS/contrib/doocs/serverlib/include
    INCLUDEPATH += $$MYDOOCS/contrib/zmq/include

} else {
	GCCPATH = $$system(which gcc)
	message("!!! GCCPATH="$$GCCPATH)
	QMAKE_CXXFLAGS += -std=c++14
	MYDOOCS = /afs/ifh.de/group/pitz/doocs
	SYSTEM_LIB = $$MYDOOCS/system_arch/$$CODENAME/lib
	LIBS += -lldap
	LIBS += -lrt
}

CONFIG += c++14

INCLUDEPATH += $$MYDOOCS/include/doocs
INCLUDEPATH += $$MYDOOCS/include/zmq
LIBS += -L$$SYSTEM_LIB
LIBS += -lzmq
LIBS += -ldl

QT += widgets
QT += charts
DEFINES += USE_NEW_MATLB_NAMES
DEFINES += PITZ_DAQ_DATA_HANDLING_TYPES_USING_STATIC_LIB_OR_SOURCES

INCLUDEPATH += $${PWD}/../../../include
INCLUDEPATH += $${PWD}/../../../src
INCLUDEPATH += $${PWD}/../../../src/include_p
INCLUDEPATH += $${PWD}/../../../contrib/pitz_daq_data_handling/include
INCLUDEPATH += $${PWD}/../../../contrib/pitz_daq_data_handling/contrib/matlab/include

RESOURCES += \
    $${PWD}/../../../src/resources/doocs_zmq_reader.qrc

SOURCES +=	\
	$${PWD}/../../../src/ui/doocs_zmq_reader_ui_qt_mainwindow.cpp						\
	$${PWD}/../../../src/ui/main_doocs_zmq_reader_ui_qt.cpp							\
	$${PWD}/../../../src/ui/doocs_zmq_reader_ui_qt_application.cpp						\
	$${PWD}/../../../src/ui/doocs_zmq_reader_ui_qt_centralwidget.cpp					\
	$${PWD}/../../../src/ui/doocs_zmq_reader_ui_qt_connectiondialog.cpp					\
	$${PWD}/../../../src/ui/doocs_zmq_reader_ui_qt_graphic.cpp						\
	$${PWD}/../../../src/ui/doocs_zmq_reader_ui_qt_events.cpp						\
	$${PWD}/../../../contrib/pitz_daq_data_handling/src/libs/pitz_daq_data_handling_types.cpp		\
	$${PWD}/../../../contrib/pitz_daq_data_handling/src/libs/common_libs_matlab_independent_functions.cpp	\
	$${PWD}/../../../src/libs/entry_doocsclient_dynamic.cpp

HEADERS +=	\
	$${PWD}/../../../include/common/ui/qt/sizeablewidget.hpp							\
	$${PWD}/../../../include/common/ui/qt/sizeablewidget.impl.hpp		\
	$${PWD}/../../../src/ui/qt/application.hpp								\
	$${PWD}/../../../src/ui/qt/mainwindow.hpp								\
	$${PWD}/../../../src/ui/qt/centralwidget.hpp								\
	$${PWD}/../../../src/ui/qt/connectiondialog.hpp								\
	$${PWD}/../../../src/ui/qt/graphic.hpp									\
	$${PWD}/../../../src/ui/qt/events.hpp									\
	$${PWD}/../../../include/doocsclient_dynamic.h								\
	$${PWD}/../../../include/common/unnamedsemaphorelite.hpp

OTHER_FILES +=	\
	$${PWD}/../../../../common_ui_qt_sizeabledialog.cpp
