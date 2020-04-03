#
# File docs_zmq_reader.pro
# File created : 01 Apr 2020
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used ...
# for PITZ
#

DEFINES += DEBUG_APPLICATION

message("!!! daq_browser_bin_for_matlab.pro")
#QMAKE_CXXFLAGS += -std=c++0x
#QMAKE_CXXFLAGS_WARN_ON += -Wno-implicit-fallthrough

win32{
    SOURCES += \

} else {
    GCCPATH = $$system(which gcc)
    message("!!!!!!!!!!! GCCPATH=$$GCCPATH")
    #QMAKE_CXXFLAGS += -std=c++11
    SOURCES += \

}

#include ( $${PWD}/../../../contrib/matlab/prj/common/common_qt/matlab_matrix_common.pri )
include ( $${PWD}/../../common/common_qt/doocs_client_common.pri)

LIBS += -lzmq
LIBS += -ldl

CONFIG += c++14

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
	$${PWD}/../../../contrib/pitz_daq_data_handling/src/libs/pitz_daq_data_handling_types.cpp		\
	$${PWD}/../../../contrib/pitz_daq_data_handling/src/libs/common_libs_matlab_independent_functions.cpp

HEADERS +=	\
	$${PWD}/../../../src/ui/qt/application.hpp								\
	$${PWD}/../../../src/ui/qt/mainwindow.hpp								\
	$${PWD}/../../../src/ui/qt/centralwidget.hpp								\
	$${PWD}/../../../src/ui/qt/connectiondialog.hpp								\
	$${PWD}/../../../src/ui/qt/graphic.hpp

OTHER_FILES +=
