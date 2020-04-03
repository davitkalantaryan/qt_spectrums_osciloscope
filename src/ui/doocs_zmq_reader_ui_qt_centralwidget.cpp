//
// file:			src/doocs_zmq_reader_ui_qt_centralwidget.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/centralwidget.hpp>
#include <QMenu>
#include <ui/qt/application.hpp>
#include <ui/qt/connectiondialog.hpp>
#include <QMessageBox>

using namespace doocs_zmq_reader;

ui::qt::CentralWidget::CentralWidget()
	:
	  m_actionConnect(QIcon(":/img/connect_btn.png"),tr("Connect"),this),
	  m_actionDisconnect(QIcon(":/img/disconnect_btn.png"),tr("Disconnect"),this),
	  m_actionExit(QIcon(":/img/quit_pik.png"),"&Exit")
{
	// make left side menu
	QMenu* pMenu;

	m_pFileMenu = m_menuBarLeft.addMenu("&File");

	//m_pFileMenu->addAction("&Exit",[](){ ::QCoreApplication::quit(); });
	m_pFileMenu->addAction(&m_actionExit); connect(&m_actionExit,&QAction::triggered,[](){ ::QCoreApplication::quit(); });
	m_pFileMenu->insertAction(&m_actionExit,&m_actionConnect);

	// now right side menu
	pMenu = m_menuBarRight.addMenu("&Help");
	pMenu->addAction("About",[this](){
		QMessageBox::about(this,"doocs_zmq_reader","To be done");
	});

	// toolbar
	m_mainToolBar.addAction(&m_actionExit);
	m_mainToolBar.insertAction(&m_actionExit,&m_actionConnect);

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

	// signal slot connections
	::QObject::connect(&m_actionConnect,&QAction::triggered,this,[this](){
		QString serverName;
		ConnectionDialog aConDlg;
		if(aConDlg.MyExec(&serverName)){
			m_actionConnect.setDisabled(true);
			thisApp()->ConnectToServerAnyThread(serverName);
		}
	});

	::QObject::connect(&m_actionDisconnect,&QAction::triggered,this,[](){
		// todo:
	});


	::QObject::connect(thisApp(),&Application::ConnectionFailedToGuiSignal,this,[this](const QString& a_reason){
		QMessageBox::critical(this,"Unable to connect",a_reason);
		m_actionConnect.setEnabled(true);
	});


	::QObject::connect(thisApp(),&Application::ConnectionDoneToGuiSignal,this,[this](){
		m_actionDisconnect.setEnabled(true);
		m_pFileMenu->removeAction(&m_actionConnect);
		m_pFileMenu->insertAction(&m_actionExit,&m_actionDisconnect);
		m_mainToolBar.removeAction(&m_actionConnect);
		m_mainToolBar.insertAction(&m_actionExit,&m_actionDisconnect);
	});
}
