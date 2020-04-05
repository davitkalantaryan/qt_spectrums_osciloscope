//
// file:			src/doocs_zmq_reader_ui_qt_connectiondialog.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/connectiondialog.hpp>
#include <stdlib.h>
#include <doocsclient_dynamic.h>
#include <ui/qt/events.hpp>
#include <QMouseEvent>
#include <ui/qt/application.hpp>

#define REQUEST_VECT_SIZE	2

using namespace doocs_zmq_reader;

ui::qt::ConnectionDialog::ConnectionDialog(QWidget* a_pParent)
	:
	  ::common::ui::qt::SizeableDialog(a_pParent),
	  m_ensHeader("ENSHOST")
{
	NewSettings& aSettings = thisApp()->settings();
	QString ensHost = thisApp()->ensHostValue();
	if(ensHost.size()<2){ensHost="Unknown";}

	m_bReturn = false;


	m_ensHosts.setText(ensHost);
	m_ok.setText("Add");
	m_cancel.setText("Cancel");

	if(aSettings.contains(RECENT_PAST_ENTRIES_SETTINGS_KEY)){
		QList <QVariant> recentProps = aSettings.value(RECENT_PAST_ENTRIES_SETTINGS_KEY).toList();
		for(auto pPropName : recentProps){
			m_recents.addItem(pPropName.toString());
		}
	}

	m_mainLayout.addWidget(&m_ensHeader,  0,0);
	m_mainLayout.addWidget(&m_ensHosts,   0,1,1,3);
	m_mainLayout.setAlignment(&m_ensHosts, Qt::AlignRight);
	m_mainLayout.addWidget(&m_facility,   1,0);
	m_mainLayout.addWidget(&m_device,     1,1);
	m_mainLayout.addWidget(&m_location,   1,2);
	m_mainLayout.addWidget(&m_property,   1,3);
	m_mainLayout.addWidget(&m_comment,    2,0,1,4);
	m_mainLayout.addWidget(&m_fullAddress,3,0,1,4);
	m_mainLayout.addWidget(&m_recents,    4,0,2,4);
	m_mainLayout.addWidget(&m_ok,         6,0);
	m_mainLayout.addWidget(&m_cancel,     6,3);
	setLayout(&m_mainLayout);

	// connections
	m_connectionNewEns = ::QObject::connect(thisApp(),&Application::EnsHostChangedToGuiSignal,[this](){
		m_comment.setText("");
		m_device.clear();
		m_location.clear();
		m_property.clear();
		m_facility.clear();
		thisApp()->GetDoocsNamesAnyThread(&m_facility,"*");
	});


	::QObject::connect(&m_facility,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		m_comment.setText("");
		m_device.clear();
		m_location.clear();
		m_property.clear();
		if((a_curText.size()>0)&&(a_curText!="*")){
			thisApp()->GetDoocsNamesAnyThread(&m_device,a_curText+"/*");
		}
	});

	::QObject::connect(&m_device,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		m_comment.setText("");
		m_location.clear();
		m_property.clear();
		if((a_curText.size()>0)&&(a_curText!="*")){
			thisApp()->GetDoocsNamesAnyThread(&m_location,m_facility.currentText() + "/" + a_curText+"/*");
		}
	});

	::QObject::connect(&m_location,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		m_comment.setText("");
		m_property.clear();
		if((a_curText.size()>0)&&(a_curText!="*")){
			thisApp()->GetDoocsNamesAnyThread(&m_property,m_facility.currentText() + "/" + m_device.currentText() + "/" + a_curText+"/*");
		}
	});

	::QObject::connect(&m_property,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		if((a_curText.size()>0)&&(a_curText!="*")){
			QString aDoocsAdr;
			QString propAddr;
			::std::string curText = a_curText.toStdString();
			size_t unPos = curText.find_first_of(" \t\n");
			if(unPos== ::std::string::npos){
				propAddr = a_curText;
			}
			else{
				propAddr = curText.substr(0,unPos).c_str();
			}
			aDoocsAdr = m_facility.currentText() + "/" + m_device.currentText() + "/" +  m_location.currentText() + "/" +  propAddr;
			m_fullAddress.setText(aDoocsAdr);
		}
	});

	::QObject::connect(&m_ok,&QPushButton::clicked,this,[this](){
		m_bReturn = true;
		hide();
	});

	::QObject::connect(&m_cancel,&QPushButton::clicked,this,[this](){
		m_bReturn = false;
		hide();
	});

	::QObject::connect(&m_recents,&QListWidget::itemClicked,this,[this](QListWidgetItem * a_item){
		QString itemText = a_item->text();
		QString doocsAddress;

		if(GetEnsAndDoocsAddressFromSavedString(itemText,&m_ensHostName,&doocsAddress)){
			if(m_ensHostName!=thisApp()->ensHostValue()){
				m_comment.setText("In the case of pressing Add enshost environment will be changed");
			}
		}
		m_fullAddress.setText(doocsAddress);
	});

	// let's start fill facility
	thisApp()->GetDoocsNamesAnyThread(&m_facility,"*");
}


ui::qt::ConnectionDialog::~ConnectionDialog()
{
}


bool ui::qt::ConnectionDialog::MyExec()
{
	::common::ui::qt::SizeableDialog::exec();
	::QObject::disconnect(m_connectionNewEns);
	if((m_ensHostName.size()>1)&&(m_ensHostName!=thisApp()->ensHostValue())){
		thisApp()->SetEnsHostValue(m_ensHostName);
	}
	return m_bReturn;
}


QString ui::qt::ConnectionDialog::doocsAddress()const
{
	return m_fullAddress.text();
}


/*/////////////////////////////////////////////////////////////////////////////////////*/


bool ui::qt::NewComboBox::event( QEvent* a_event)
{
	events::NamesReady* pEvent = dynamic_cast<events::NamesReady*>(a_event);

	if(pEvent){
		USTR* pUSTR;
		const EqData* pData = pEvent->data();
		//int length = pData->length();
		int length = DynEqDataLength(pData);
		clear();
		addItem("*");
		for(int i(0);i<length;++i){
			//pUSTR = pData->get_ustr(i);
			pUSTR = DynEqDataGet_ustr2(pData,i);
			if(pUSTR && pUSTR->str_data.str_data_val){
				addItem(pUSTR->str_data.str_data_val);
			}
		}
		model()->sort(0);
		//delete pData;
		//DynDeleteEqData(pData);
	}
	else{
		return ::QComboBox::event(a_event);
	}

	return true;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/
ui::qt::EnsChangerDialog::EnsChangerDialog(QWidget* a_pParent)
	:
	  ::common::ui::qt::SizeableDialog(a_pParent)
{
	m_bReturn = false;
	m_ensHost.setText(thisApp()->ensHostValue());
	m_ok.setText("Change");
	m_cancel.setText("Cancel");

	m_mainLayout.addWidget(&m_ensHost,0,0,1,2);
	m_mainLayout.addWidget(&m_ok,1,0);
	m_mainLayout.addWidget(&m_cancel,1,1);

	setLayout(&m_mainLayout);

	// connections
	::QObject::connect(&m_ok,&QPushButton::clicked,this,[this](){
		m_bReturn = true;
		hide();
	});

	::QObject::connect(&m_cancel,&QPushButton::clicked,this,[this](){
		m_bReturn = false;
		hide();
	});
}


bool ui::qt::EnsChangerDialog::MyExec()
{
	::common::ui::qt::SizeableDialog::exec();
	return  m_bReturn;
}


QString ui::qt::EnsChangerDialog::ensHost()const
{
	return m_ensHost.text();
}


/*/////////////////////////////////////////////////////////////////////////////////////*/

ui::qt::Enslabel::Enslabel()
{
	setStyleSheet("background-color:rgb(0,0,255);" "color:white;");
}


void ui::qt::Enslabel::mouseReleaseEvent(QMouseEvent *a_event)
{
	switch (a_event->button()) {
	case Qt::LeftButton:{
		EnsChangerDialog aEnsDlg(this);
		if(aEnsDlg.MyExec()){
			QString newVal = aEnsDlg.ensHost();
			thisApp()->SetEnsHostValue(newVal);
			setText(newVal);
		}
	}break;
	default:
		QLabel::mouseDoubleClickEvent(a_event);
		break;
	}
}
