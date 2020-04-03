//
// file:			src/ui/qt/mainwindow.hpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_MAINWINDOW_HPP
#define SRC_UI_QT_MAINWINDOW_HPP

#include <QMainWindow>
#include <ui/qt/centralwidget.hpp>

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class MainWindow final : public ::QMainWindow
{
public:
	MainWindow();

private:
	CentralWidget	m_centralWidget;
};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_MAINWINDOW_HPP
