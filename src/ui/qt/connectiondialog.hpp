//
// file:			src/ui/qt/connectiondialog.hpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_CONNECTIONDIALOG_HPP
#define SRC_UI_QT_CONNECTIONDIALOG_HPP

#include <common/ui/qt/sizeabledialog.hpp>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class ConnectionDialog;

class NewComboBox final : public ::QComboBox
{
	bool event( QEvent* event) override;
};

class Enslabel final : public QLabel
{
	friend class ConnectionDialog;
	Enslabel();
	//void mouseDoubleClickEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
};


class ConnectionDialog final : private ::common::ui::qt::SizeableDialog
{
public:
	ConnectionDialog(QWidget* a_pParent);
	~ConnectionDialog() override;

	bool MyExec();
	QString doocsAddress()const;

private:
	QGridLayout		m_mainLayout;
	//QLineEdit		m_ensHostsEdit;
	QLabel			m_ensHeader;		
	Enslabel		m_ensHosts;
	NewComboBox		m_facility;
	NewComboBox		m_device;
	NewComboBox		m_location;
	NewComboBox		m_property;
	QLabel			m_comment;
	QLineEdit		m_fullAddress;
	QListWidget		m_recents;
	QPushButton		m_ok;
	QPushButton		m_cancel;
	QString			m_ensHostName;
	QMetaObject::Connection m_connectionNewEns;
	bool			m_bReturn;
};


class EnsChangerDialog final : private ::common::ui::qt::SizeableDialog
{
public:
	EnsChangerDialog(QWidget* a_pParent);

	bool MyExec();
	QString ensHost()const;

private:
	QGridLayout		m_mainLayout;
	QLineEdit		m_ensHost;
	QPushButton		m_ok;
	QPushButton		m_cancel;
	bool			m_bReturn;
};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_MAINWINDOW_HPP
