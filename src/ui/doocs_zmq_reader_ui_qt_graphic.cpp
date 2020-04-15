//
// file:			src/doocs_zmq_reader_ui_qt_graphic.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/graphic.hpp>
#include <QVBoxLayout>
#include <QDebug>
#include <stdlib.h>
#include <ui/qt/connectiondialog.hpp>
#include <ui/qt/events.hpp>
#include <QMessageBox>
#include <QMenu>
#include <ui/qt/application.hpp>

#ifdef _WIN32
#else
#include <unistd.h>
#define Sleep(_x)  usleep(1000*(_x))
#endif

using namespace doocs_zmq_reader;


ui::qt::Graphic::Graphic()
	:
	  m_actionPlus(QIcon(":/img/green_plus.png"),tr("Add property"),this)
{
	m_statuses.allBits = 0;

	setChart(&m_chart);

	m_chart.addAxis(&m_axisX,Qt::AlignBottom);
	m_chart.addAxis(&m_axisY,Qt::AlignLeft);

	m_axisX.setLabelFormat("%g");
	m_axisX.setTitleText("Samples");

	m_axisY.setTitleText("Measuerement");

	setMinimumSize(800, 600);

	//m_chart.legend()->setReverseMarkers(true);
	//m_chart.legend()->setShowToolTips(true);

	// signal slot connections
	::QObject::connect(&m_actionPlus,&QAction::triggered,this,[this](){
		ConnectionDialog aConDlg(this);
		if(aConDlg.MyExec()){
			QString doocsAddress = aConDlg.doocsAddress();
			SingSeries* pSeries = new SingSeries(this);
			m_actionPlus.setDisabled(true);
			thisApp()->AddNewPropertyAnyThread(this,doocsAddress,pSeries);
		}
	});

	if(m_settings.contains(CURRENT_ENTRIES_SETTINGS_KEY)){
		SingSeries* pSeries;
		//QString curEnsValue = thisApp()->ensHostValue();
		//QString ensHostName,doocsAddress;
		QList<QVariant> lastEntries = m_settings.value(CURRENT_ENTRIES_SETTINGS_KEY).toList();

		for(auto ensPlusDoocs : lastEntries){
			//if(GetEnsAndDoocsAddressFromSavedString(ensPlusDoocs.toString(),&ensHostName,&doocsAddress)){
			//	thisApp()->SetEnsHostValue(ensHostName);
			//}
			pSeries = new SingSeries(this);
			m_actionPlus.setDisabled(true);
			//thisApp()->AddNewPropertyAnyThread(this,doocsAddress,pSeries);
			thisApp()->AddNewPropertyAnyThread(this,ensPlusDoocs.toString(),pSeries);
		}  // for(auto ensPlusDoocs : lastEntries){
	}  // if(aSettings.contains(RECENT_ENTRIES_SETTINGS_KEY)){

}


ui::qt::Graphic::~Graphic()
{
	thisApp()->StopZmqReceiverThread();
	for(auto pSeries : m_listSeries){
		delete pSeries;
	}
}


QAction* ui::qt::Graphic::ActionAddProperty()
{
	return &m_actionPlus;
}


void ui::qt::Graphic::RemoveSeries(SingSeries* a_pSeries)
{
	m_chart.removeSeries(a_pSeries);
	thisApp()->RemoveExistingPropertyAnyThread(a_pSeries->m_details2.pParent);
	m_listSeries.erase(a_pSeries->m_iter);
	while(a_pSeries->m_statuses.bits.isAlive){
		Sleep(1);
	}
	a_pSeries->deleteLater();
}


bool ui::qt::Graphic::event(QEvent* a_event)
{
	events::PropertyAddingDone* pEvent = dynamic_cast< events::PropertyAddingDone* >(a_event);

	if(pEvent){
		SingSeries* pSeries = pEvent->pSeries();
		if(pEvent->isAddingOk()){
			pSeries->SetEntryDetails(pEvent->entryDetails());
			AddNewSeries(pSeries,pEvent->errorStringOrDoocsAddress());
		}
		else{
			delete pSeries;
			::QMessageBox::critical(this,"Unable to connect",pEvent->errorStringOrDoocsAddress());
		}
		m_actionPlus.setEnabled(true);
	}
	else{
		return ::QtCharts::QChartView::event(a_event);
	}

	return true;
}


void ui::qt::Graphic::mouseReleaseEvent(QMouseEvent* a_event)
{
	 switch(a_event->button()){
	 case Qt::RightButton:{

		 if(m_listSeries.size()>0){
			 QMenu aMenu;

			 aMenu.addAction("FitGraph",[this](){
				 m_statuses.bits.isXmaxMinInited = 0;
				 m_statuses.bits.isYmaxMinInited = 0;
				 ++m_statuses.bits.fitGraphNumber;
			 });

			 aMenu.exec(mapToGlobal(a_event->pos()));
		 }

	 }break;
	 default:
		 break;
	 }

	 ::QtCharts::QChartView::mouseReleaseEvent(a_event);
}


void ui::qt::Graphic::AddNewSeries( SingSeries* a_newSeries,const QString& a_legendStr )
{
	m_listSeries.push_front(a_newSeries );
	a_newSeries->m_iter = m_listSeries.begin();

	m_chart.addSeries(a_newSeries);
	a_newSeries->attachAxis(&m_axisX);
	a_newSeries->attachAxis(&m_axisY);

	QList< ::QtCharts::QLegendMarker * > aList = m_chart.legend()->markers(a_newSeries);
	if(aList.size()>0){
		a_newSeries->m_pLegendMarker = aList.at(0);
		a_newSeries->m_pLegendMarker->setLabel(a_legendStr);
		//a_newSeries->m_pLegendMarker->in
		//update(rect());
		m_chart.legend()->hide();
		m_chart.legend()->show();

		a_newSeries->m_pLegendMarker->setParent(a_newSeries);

		::QObject::connect(a_newSeries->m_pLegendMarker,&QtCharts::QLegendMarker::clicked,this,[this](){
			char vcBuffer[1024];
			::QtCharts::QLegendMarker* pLegendMarker = static_cast< ::QtCharts::QLegendMarker* >(sender());
			SingSeries* pSeries = static_cast<SingSeries*>(pLegendMarker->parent());
			QColor aColor = pLegendMarker->brush().color();
			HandlerDlg aDlg(this,pSeries);
			snprintf(vcBuffer,1023,"background-color:rgb(%d,%d,%d);" "color:white;",aColor.red(),aColor.green(),aColor.blue());
			aDlg.setStyleSheet(vcBuffer);
			aDlg.exec();
		});
	}
}


/*/////////////////////////////////////////////////////////////////////////////////////*/

static void TypeDataGetterStatic(double a_inData, void* a_outData, void** a_outDataNext)
{
	QPointF* pOutData = static_cast<QPointF*>(a_outData);
	pOutData->setY(a_inData);
	*a_outDataNext = (++pOutData);
}

ui::qt::SingSeries::SingSeries( Graphic* a_pParentGraphic )
	:
	  m_pParentGraphic(a_pParentGraphic)
{
	m_statuses.allBits = 0;
	m_pLegendMarker = nullptr;
	m_lfXkoef = 1.;
	m_statuses.bits.isAlive = 1;
	memset(&m_input,0,sizeof(m_input));
	memset(&m_outputs,0,sizeof(m_outputs));
	m_input.yKoef = 1.0;
	m_input.fpDataGetter = &TypeDataGetterStatic;
}


ui::qt::SingSeries::~SingSeries()
{
	//thisApp()->StopZmqReceiverThread();
}


void ui::qt::SingSeries::SetNotAlive()
{
	m_statuses.bits.isAlive = 0;
}


#define FORMULA_FOR_X(_x)	( static_cast<qreal>(_x)*m_lfXkoef )

void ui::qt::SingSeries::PlotGui(int32_t a_numberOfPoint, const void* /*a_secondHeaderBuffer*/, const void* a_pData)
{
	int32_t newPointsCount = static_cast<int32_t>(a_numberOfPoint);

	if(
			(newPointsCount!=m_input.countIn)||(!m_pParentGraphic->m_statuses.bits.isXmaxMinInited) ||
			(m_statuses.bits.fitGraphNumber!=m_pParentGraphic->m_statuses.bits.fitGraphNumber)){
		int32_t unIterX=0;
		double lfCurrX;
		bool bXmaxMinChanged = false;
		m_buffer.resize(newPointsCount);
		m_statuses.bits.fitGraphNumber = m_pParentGraphic->m_statuses.bits.fitGraphNumber;
		m_input.countIn = newPointsCount;
		if (!m_pParentGraphic->m_statuses.bits.isXmaxMinInited){
			m_pParentGraphic->m_lfXmin = m_pParentGraphic->m_lfXmax = FORMULA_FOR_X(0);
			unIterX=1;
			m_pParentGraphic->m_statuses.bits.isXmaxMinInited = 1;
		}
		for( ;unIterX<newPointsCount;++unIterX){
			lfCurrX = FORMULA_FOR_X(unIterX);
			if(lfCurrX>m_pParentGraphic->m_lfXmax){m_pParentGraphic->m_lfXmax=lfCurrX;bXmaxMinChanged=true; }
			if(lfCurrX<m_pParentGraphic->m_lfXmin){m_pParentGraphic->m_lfXmin=lfCurrX;bXmaxMinChanged=true; }
			m_buffer[unIterX].setX(lfCurrX);
		}

		if(bXmaxMinChanged){
			m_pParentGraphic->m_axisX.setRange(m_pParentGraphic->m_lfXmin,m_pParentGraphic->m_lfXmax);
		}

	}

	m_input.maxOrMinInited = m_pParentGraphic->m_statuses.bits.isYmaxMinInited;
	m_input.inpData = a_pData;
	m_outputs.outData = m_buffer.data();
	m_outputs.yMinInOut = m_pParentGraphic->m_lfYmin;
	m_outputs.yMaxInOut = m_pParentGraphic->m_lfYmax;

	PrepareDaqEntryBasedOnType(&m_input,&m_outputs);
	m_pParentGraphic->m_lfYmin = m_outputs.yMinInOut;
	m_pParentGraphic->m_lfYmax = m_outputs.yMaxInOut;
	m_pParentGraphic->m_statuses.bits.isYmaxMinInited = 1;

	if(m_outputs.maxOrMinChanged){
		m_pParentGraphic->m_axisY.setRange(m_pParentGraphic->m_lfYmin,m_pParentGraphic->m_lfYmax);
	}

	replace(m_buffer);
}


void ui::qt::SingSeries::SetEntryDetails(const SSingleEntryBase& a_entryDetails)
{
	QSettings& aSettings = m_pParentGraphic->m_settings;
	bool bIsSignChanged;

	m_details2 = a_entryDetails;
	m_input.dataType = a_entryDetails.type;

	//m_lfXkoef = aSettings.value("ui::qt::SingSeries::lfXkoef::"+m_details2.ensPlusDoocsAdr,m_lfXkoef).toDouble();
	//m_input.yKoef = aSettings.value("ui::qt::SingSeries::lfYkoef::"+m_details2.ensPlusDoocsAdr,m_input.yKoef).toDouble();
	GET_PROP_FROM_SETT(aSettings,m_details2.ensPlusDoocsAdr,this,m_lfXkoef,1.0,toDouble);
	GET_PROP_FROM_SETT(aSettings,m_details2.ensPlusDoocsAdr,this,m_input.yKoef,1.0,toDouble);
	bIsSignChanged = aSettings.value(  PROP_TO_SET_KEY(m_details2.ensPlusDoocsAdr,isSignChanged),false).toBool();
	m_input.isSignChanged = bIsSignChanged?1:0;
}


bool ui::qt::SingSeries::event(QEvent* a_event)
{
	events::NewData* pEvent = dynamic_cast< events::NewData* >(a_event);

	if(pEvent){
		PlotGui(pEvent->numberOfPoints(),pEvent->secondHeader(),pEvent->data());
	}
	else{
		return ::QtCharts::QLineSeries::event(a_event);
	}

	return true;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/

ui::qt::HandlerDlg::HandlerDlg(Graphic* a_pGraph, SingSeries* a_pSeries)
	:
	  ::common::ui::qt::SizeableWidget< ::QDialog >(a_pSeries->m_details2.ensPlusDoocsAdr.toStdString().c_str(), a_pGraph)
{
	QSettings& aSettings = a_pGraph->m_settings;
	bool bFrameloss = aSettings.value( m_settingsKey + "/isFrameloss",false).toBool();

	m_defaultWindowFlags = windowFlags();
	m_pGraph = a_pGraph;
	m_pSeries = a_pSeries;

	m_toggleSign.setText(a_pSeries->m_input.isSignChanged?"Show correct sign":"Show togled sign");
	m_deleteBtn.setText("Delete");
	m_settingsBtn.setText("Settings");
	m_cancel.setText("Cancel");

	m_mainLayout.addWidget(&m_toggleSign);
	m_mainLayout.addWidget(&m_deleteBtn);
	m_mainLayout.addWidget(&m_settingsBtn);
	m_mainLayout.addWidget(&m_cancel);

	setLayout(&m_mainLayout);

	if(bFrameloss){
		setWindowFlags(m_defaultWindowFlags|Qt::FramelessWindowHint);
	}

	::QObject::connect(&m_deleteBtn,&QPushButton::clicked,this,[this](){
		m_pGraph->RemoveSeries(m_pSeries);
		hide();
	});

	::QObject::connect(&m_toggleSign,&QPushButton::clicked,this,[this](){
		QSettings& aSettings = m_pGraph->m_settings;
		bool bIsSignChangedWillBe = m_pSeries->m_input.isSignChanged ? false : true;

		aSettings.setValue(PROP_TO_SET_KEY(m_pSeries->m_details2.ensPlusDoocsAdr,isSignChanged),bIsSignChangedWillBe);
		m_pSeries->m_input.isSignChanged = ~m_pSeries->m_input.isSignChanged;
		hide();
	});

	::QObject::connect(&m_settingsBtn,&QPushButton::clicked,this,[this](){
		SeriesSettings settDlg(m_pGraph,m_pSeries);
		if(settDlg.MyExec()){
			QSettings& aSettings = m_pGraph->m_settings;
			m_pSeries->m_lfXkoef = settDlg.m_XkoefSpin.value();
			m_pSeries->m_input.yKoef = settDlg.m_YkoefSpin.value();
			qDebug()<<"m_lfXkoef="<<m_pSeries->m_lfXkoef;
			//aSettings.setValue( m_pSeries->m_details2.ensPlusDoocsAdr + "/lfXkoef",m_pSeries->m_lfXkoef);
			//aSettings.setValue( m_pSeries->m_details2.ensPlusDoocsAdr + "/lfXkoef",m_pSeries->m_input.yKoef);
			WRITE_PROP_TO_SETT(aSettings,m_pSeries->m_details2.ensPlusDoocsAdr,m_pSeries,m_lfXkoef);
			WRITE_PROP_TO_SETT(aSettings,m_pSeries->m_details2.ensPlusDoocsAdr,m_pSeries,m_input.yKoef);
			++m_pSeries->m_statuses.bits.fitGraphNumber;
		}
		hide();
	});

	::QObject::connect(&m_cancel,&QPushButton::clicked,this,[this](){
		hide();
	});
}


void ui::qt::HandlerDlg::mouseReleaseEvent(QMouseEvent * a_event )
{
	switch(a_event->button()){
	case Qt::RightButton:{
		QMenu aMenu;
		Qt::WindowFlags currentFlags = windowFlags();
		aMenu.addAction("Exit",[this](){
			hide();
		});
		if(currentFlags&Qt::FramelessWindowHint){  // we have frameloss
			aMenu.addAction(QIcon(":/img/window_frame.png"),tr("Show Frame"),[this](){
				QSettings& aSettings = m_pGraph->m_settings;
				setWindowFlags(m_defaultWindowFlags);
				aSettings.setValue(m_settingsKey + "/isFrameloss",false);
				show();
			});
		}
		else{
			aMenu.addAction(tr("Hide Frame"),[this](){
				QSettings& aSettings = m_pGraph->m_settings;
				setWindowFlags(m_defaultWindowFlags|Qt::FramelessWindowHint);
				aSettings.setValue(m_settingsKey + "/isFrameloss",true);
				show();
			});
		}
		aMenu.exec(mapToGlobal(a_event->pos()));
	}break;
	default:
		::common::ui::qt::SizeableWidget< ::QDialog >::mousePressEvent(a_event);
		break;
	}
}


/*/////////////////////////////////////////////////////////////////////////////////////*/
ui::qt::SeriesSettings::SeriesSettings(Graphic* a_pGraph,SingSeries* a_pSeries)
	:
	  ::common::ui::qt::SizeableWidget< ::QDialog >(a_pSeries->m_details2.ensPlusDoocsAdr.toStdString().c_str(), a_pGraph),
	  m_pSeries(a_pSeries)
{
#if 0
	QLabel			m_lblXkoef;
	QDoubleSpinBox	m_XkoefSpin;
	QLabel			m_lblYkoef;
	QDoubleSpinBox	m_YkoefSpin;
	QPushButton		m_btnSet;
	QPushButton		m_btnCancel;
#endif

	m_bSetPushed = false;

	m_lblXkoef.setText("Koef. X");
	m_XkoefSpin.setValue(a_pSeries->m_lfXkoef);
	m_lblYkoef.setText("Koef. Y");
	m_YkoefSpin.setValue(a_pSeries->m_input.yKoef);
	m_btnSet.setText("SET");
	m_btnCancel.setText("CANCEL");

	m_mainLayout.addWidget(&m_lblXkoef,0,0);
	m_mainLayout.addWidget(&m_XkoefSpin,0,1);
	m_mainLayout.addWidget(&m_lblYkoef,1,0);
	m_mainLayout.addWidget(&m_YkoefSpin,1,1);

	m_mainLayout.addWidget(&m_btnSet,2,0);
	m_mainLayout.addWidget(&m_btnCancel,2,1);

	setLayout(&m_mainLayout);

	connect(&m_btnSet,&QPushButton::clicked,this,[this](){
		m_bSetPushed = true;
		hide();
	});

	connect(&m_btnCancel,&QPushButton::clicked,this,[this](){
		hide();
	});
}

bool ui::qt::SeriesSettings::MyExec()
{
	::common::ui::qt::SizeableWidget< ::QDialog >::exec();
	return m_bSetPushed;
}
