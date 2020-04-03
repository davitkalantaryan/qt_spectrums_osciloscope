//
// file:			src/doocs_zmq_reader_ui_qt_mainwindow.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/mainwindow.hpp>

using namespace doocs_zmq_reader;

ui::qt::MainWindow::MainWindow()
{
	setCentralWidget(&m_centralWidget);
}
