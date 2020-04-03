//
// file:			src/doocs_zmq_reader_ui_qt_connectiondialog.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/connectiondialog.hpp>
#include <stdlib.h>
#include <eq_client.h>
#include <ui/qt/application.hpp>

#define REQUEST_VECT_SIZE	2

using namespace doocs_zmq_reader;

ui::qt::ConnectionDialog::ConnectionDialog()
	:
	  m_ensHeader("ENSHOST"),
	  m_pServerAddr(nullptr)
{
	const char* cpcEnsHostValue = getenv("ENSHOST");

	m_bReturn = false;

	cpcEnsHostValue = cpcEnsHostValue?cpcEnsHostValue:"Unknown";
	m_ensHosts.setText(cpcEnsHostValue);

	m_ok.setText("OK");
	m_cancel.setText("Cancel");

	// let's start fill facility
	thisApp()->GetDoocsNamesAnyThread("*",&m_facility);

	m_mainLayout.addWidget(&m_ensHeader,  0,0);
	m_mainLayout.addWidget(&m_ensHosts,   0,1,1,3);
	m_mainLayout.setAlignment(&m_ensHosts, Qt::AlignRight);
	m_mainLayout.addWidget(&m_facility,   1,0);
	m_mainLayout.addWidget(&m_device,     1,1);
	m_mainLayout.addWidget(&m_location,   1,2);
	m_mainLayout.addWidget(&m_property,   1,3);
	m_mainLayout.addWidget(&m_fullAddress,2,0,1,4);
	m_mainLayout.addWidget(&m_ok,         3,0);
	m_mainLayout.addWidget(&m_cancel,     3,3);
	setLayout(&m_mainLayout);

	// connections
	::QObject::connect(&m_facility,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		m_device.clear();
		m_location.clear();
		m_property.clear();
		if((a_curText.size()>0)&&(a_curText!="*")){
			thisApp()->GetDoocsNamesAnyThread(a_curText+"/*",&m_device);
		}
	});

	::QObject::connect(&m_device,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		m_location.clear();
		m_property.clear();
		if((a_curText.size()>0)&&(a_curText!="*")){
			thisApp()->GetDoocsNamesAnyThread(m_facility.currentText() + "/" + a_curText+"/*",&m_location);
		}
	});

	::QObject::connect(&m_location,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		m_property.clear();
		if((a_curText.size()>0)&&(a_curText!="*")){
			thisApp()->GetDoocsNamesAnyThread(m_facility.currentText() + "/" + m_device.currentText() + "/" + a_curText+"/*",&m_property);
		}
	});

	::QObject::connect(&m_property,&QComboBox::currentTextChanged,[this](const QString& a_curText){
		if((a_curText.size()>0)&&(a_curText!="*")){
			QString& aDoocsAdr = *m_pServerAddr;
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

	::QObject::connect(thisApp(),&Application::GetNamesDoneToGuiSignal,this,[](const QVector<QVariant>& a_getter){
		USTR* pUSTR;
		QComboBox* pBox = VariantToPtr(QComboBox,a_getter[0]);
		EqData* pData = VariantToPtr(EqData,a_getter[REQUEST_VECT_SIZE]);
		int length = pData->length();
		pBox->clear();
		pBox->addItem("*");
		for(int i(0);i<length;++i){
			pUSTR = pData->get_ustr(i);
			if(pUSTR && pUSTR->str_data.str_data_val){
				pBox->addItem(pUSTR->str_data.str_data_val);
			}
		}
		pBox->model()->sort(0);
		delete pData;
	});
}


bool ui::qt::ConnectionDialog::MyExec(QString* a_pServerAddr)
{
	m_pServerAddr = a_pServerAddr;
	QDialog::exec();
	m_pServerAddr = nullptr;
	return m_bReturn;
}
