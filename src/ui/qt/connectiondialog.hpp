//
// file:			src/ui/qt/connectiondialog.hpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_CONNECTIONDIALOG_HPP
#define SRC_UI_QT_CONNECTIONDIALOG_HPP

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class ConnectionDialog final : public ::QDialog
{
public:
	ConnectionDialog();

	bool MyExec(QString* pServerAddr);

private:
	QGridLayout		m_mainLayout;
	//QLineEdit		m_ensHostsEdit;
	QLabel			m_ensHeader;
	QLabel			m_ensHosts;
	QComboBox		m_facility;
	QComboBox		m_device;
	QComboBox		m_location;
	QComboBox		m_property;
	QLineEdit		m_fullAddress;
	QPushButton		m_ok;
	QPushButton		m_cancel;
	QString*		m_pServerAddr;
	bool			m_bReturn;
};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_MAINWINDOW_HPP
