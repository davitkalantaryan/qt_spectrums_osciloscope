//
// file:			main_doocs_zmq_reader_ui_qt.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//


#include <ui/qt/application.hpp>
#include <ui/qt/mainwindow.hpp>
#include <eq_client.h>

int main(int a_argc, char* a_argv[])
{
	doocs_zmq_reader::ui::qt::Application aApp(a_argc,a_argv);
	doocs_zmq_reader::ui::qt::MainWindow aMainWnd;

	qRegisterMetaType<EqData*>( "EqData*" );
	qRegisterMetaType< QVector<QVariant> >( "QVector<QVariant>" );

	aMainWnd.show();

	aApp.exec();
	return 0;
}
