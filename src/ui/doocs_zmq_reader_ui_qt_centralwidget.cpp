//
// file:			src/doocs_zmq_reader_ui_qt_centralwidget.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/centralwidget.hpp>
#include <QMenu>
#include <QMessageBox>
#include <ui/qt/application.hpp>

using namespace doocs_zmq_reader;

ui::qt::CentralWidget::CentralWidget()
	:
	  m_actionExit(QIcon(":/img/quit_pik.png"),"&Exit")
{
	// make left side menu
	QMenu* pMenu;

	m_pFileMenu = m_menuBarLeft.addMenu("&File");

	//m_pFileMenu->addAction("&Exit",[](){ ::QCoreApplication::quit(); });
	m_pFileMenu->addAction(&m_actionExit); connect(&m_actionExit,&QAction::triggered,[](){ ::QCoreApplication::quit(); });
	m_pFileMenu->insertAction(&m_actionExit,m_graph.ActionAddProperty());

	// now right side menu
	pMenu = m_menuBarRight.addMenu("&Help");
	pMenu->addAction("About",[this](){
		QMessageBox::about(this,"doocs_zmq_reader","To be done");
	});

	// toolbar
	m_mainToolBar.addAction(&m_actionExit);
	m_mainToolBar.insertAction(&m_actionExit,m_graph.ActionAddProperty());

	// layouts
	m_menuLayout.addWidget(&m_menuBarLeft);
	m_menuLayout.addWidget(&m_menuBarRight);
	m_menuLayout.setAlignment(&m_menuBarLeft, Qt::AlignLeft);
	m_menuLayout.setAlignment(&m_menuBarRight, Qt::AlignRight);

	m_menuAndToolbarLayout.addLayout(&m_menuLayout);
	m_menuAndToolbarLayout.addWidget(&m_mainToolBar);

	m_mainLayout.addLayout(&m_menuAndToolbarLayout);
	//m_mainLayout.addStretch(1);

	m_mainLayout.addWidget(&m_graph);

	setLayout(&m_mainLayout);
}
