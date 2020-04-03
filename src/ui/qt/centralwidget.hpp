//
// file:			src/ui/qt/centralwidget.hpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_CENTRALWIDGET_HPP
#define SRC_UI_QT_CENTRALWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <ui/qt/graphic.hpp>

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class CentralWidget final : public ::QWidget
{
public:
	CentralWidget();

private:
	QVBoxLayout		m_mainLayout;
	QVBoxLayout		m_menuAndToolbarLayout;
	QHBoxLayout		m_menuLayout;
	QMenuBar		m_menuBarLeft ;
	QMenuBar		m_menuBarRight;
	QToolBar		m_mainToolBar;
	QAction			m_actionConnect;
	QAction			m_actionDisconnect;
	QAction			m_actionExit;
	QMenu*			m_pFileMenu;
	Graphic			m_graph;
};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_CENTRALWIDGET_HPP
