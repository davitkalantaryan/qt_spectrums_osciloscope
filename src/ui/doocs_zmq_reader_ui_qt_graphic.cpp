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
	NewSettings& aSettings = thisApp()->settings();

	m_statuses.allBits = 0;
	m_statuses.bits.isMaxMinNotInited = 1;

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

	if(aSettings.contains(CURRENT_ENTRIES_SETTINGS_KEY)){
		SingSeries* pSeries;
		QString curEnsValue = thisApp()->ensHostValue();
		QString ensHostName,doocsAddress;
		QList<QVariant> lastEntries = aSettings.value(CURRENT_ENTRIES_SETTINGS_KEY).toList();

		for(auto ensPlusDoocs : lastEntries){
			if(GetEnsAndDoocsAddressFromSavedString(ensPlusDoocs.toString(),&ensHostName,&doocsAddress)){
				thisApp()->SetEnsHostValue(ensHostName);
			}
			pSeries = new SingSeries(this);
			m_actionPlus.setDisabled(true);
			thisApp()->AddNewPropertyAnyThread(this,doocsAddress,pSeries);
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
	thisApp()->RemoveExistingPropertyAnyThread(a_pSeries->m_details.pParent);
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
			NewSettings& aSettings = thisApp()->settings();
			bool bProblemHidden;
			pSeries->m_details = pEvent->entryDetails();
			bProblemHidden = aSettings.value("ui::qt::SingSeries"+pSeries->m_details.ensPlusDoocsAdr).toBool();
			pSeries->m_statuses.bits.isProblemHidden = bProblemHidden?1:0;
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
				 m_statuses.bits.isMaxMinNotInited = 1;
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

ui::qt::SingSeries::SingSeries( Graphic* a_pParentGraphic )
	:
	  m_pParentGraphic(a_pParentGraphic)
{
	m_statuses.allBits = 0;
	m_pLegendMarker = nullptr;
	m_lfYkoef = m_lfXkoef = 1.;
	m_statuses.bits.isAlive = 1;
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

void ui::qt::SingSeries::PlotGui(int32_t a_numberOfPoint, int32_t a_singleItemSize, const void* /*a_secondHeaderBuffer*/, const void* a_pData)
{
	bool bXalreadyScanned = false;
	bool bXmaxOrMinChanged=false;
	bool bYmaxOrMinChanged=false;
	qreal lfCurrX;
	qreal lfCurrentValue;
	int32_t unIterY=0;
	int32_t newPointsCount = static_cast<int32_t>(a_numberOfPoint);
	int32_t nSingleItemSize = a_singleItemSize;
	const char* pcData = static_cast<const char*>(a_pData);
	if(newPointsCount!=m_pointsCount){
		int32_t unIterX=0;
		m_buffer.resize(newPointsCount);
		m_pointsCount = newPointsCount;

		if(m_pParentGraphic->m_statuses.bits.isMaxMinNotInited){
			m_pParentGraphic->m_lfXmin = m_pParentGraphic->m_lfXmax = FORMULA_FOR_X(0);
			m_buffer[0].setX(m_pParentGraphic->m_lfXmin);
			unIterX = 1;
		}

		for(;unIterX<m_pointsCount;++unIterX){
			lfCurrX = FORMULA_FOR_X(unIterX);
			if(lfCurrX>m_pParentGraphic->m_lfXmax){m_pParentGraphic->m_lfXmax=lfCurrX; bXmaxOrMinChanged=true;}
			if(lfCurrX<m_pParentGraphic->m_lfXmin){m_pParentGraphic->m_lfXmin=lfCurrX; bXmaxOrMinChanged=true;}
			m_buffer[unIterX].setX(lfCurrX);
		}
		bXalreadyScanned = true;

	}  // if(newPointsCount!=m_lastPointsCount){

	if(m_pParentGraphic->m_statuses.bits.isMaxMinNotInited){
		m_pParentGraphic->m_lfYmin = m_pParentGraphic->m_lfYmax = zmqToChart(nSingleItemSize,pcData);
		pcData += nSingleItemSize;
		m_buffer[0].setY(m_pParentGraphic->m_lfYmin);
		if(!bXalreadyScanned){
			for(int32_t unIterX(1);unIterX<m_pointsCount;++unIterX){
				m_pParentGraphic->m_lfXmin = m_pParentGraphic->m_lfXmax = FORMULA_FOR_X(0);
				m_buffer[0].setX(m_pParentGraphic->m_lfXmin);
				lfCurrX = FORMULA_FOR_X(unIterX);
				if(lfCurrX>m_pParentGraphic->m_lfXmax){m_pParentGraphic->m_lfXmax=lfCurrX; bXmaxOrMinChanged=true;}
				if(lfCurrX<m_pParentGraphic->m_lfXmin){m_pParentGraphic->m_lfXmin=lfCurrX; bXmaxOrMinChanged=true;}
				m_buffer[unIterX].setX(lfCurrX);
			}
		}
		unIterY = 1;
		m_pParentGraphic->m_statuses.bits.isMaxMinNotInited=0;
	}  // if(m_pParentGraphic->m_statuses.bits.isMaxMinNotInited){

	if(bXmaxOrMinChanged){
		m_pParentGraphic->m_axisX.setRange(m_pParentGraphic->m_lfXmin,m_pParentGraphic->m_lfXmax);
	}

	for(;unIterY<m_pointsCount;++unIterY){
		lfCurrentValue = zmqToChart(nSingleItemSize,pcData);
		pcData += nSingleItemSize;
		if(lfCurrentValue>m_pParentGraphic->m_lfYmax){m_pParentGraphic->m_lfYmax=lfCurrentValue; bYmaxOrMinChanged=true;}
		if(lfCurrentValue<m_pParentGraphic->m_lfYmin){m_pParentGraphic->m_lfYmin=lfCurrentValue; bYmaxOrMinChanged=true;}
		m_buffer[unIterY].setY(lfCurrentValue);
	}
	if(bYmaxOrMinChanged){
		m_pParentGraphic->m_axisY.setRange(m_pParentGraphic->m_lfYmin,m_pParentGraphic->m_lfYmax);
	}
	replace(m_buffer);
}


qreal ui::qt::SingSeries::zmqToChart(int32_t a_singleDataSize, const char* a_zmqData)const
{
	qreal lfReturn;
	switch(a_singleDataSize){
	case 2:
		if(m_statuses.bits.isProblemHidden){
			lfReturn = static_cast<qreal>(*reinterpret_cast<const uint16_t*>(a_zmqData))*m_lfYkoef;
		}
		else{
			lfReturn = static_cast<qreal>(*reinterpret_cast<const int16_t*>(a_zmqData))*m_lfYkoef;
		}
		break;
	case 4:
		lfReturn = static_cast<qreal>(*reinterpret_cast<const uint32_t*>(a_zmqData))*m_lfYkoef;
		break;
	case 8:
		lfReturn = static_cast<qreal>(*reinterpret_cast<const uint64_t*>(a_zmqData))*m_lfYkoef;
		break;
	default:
		lfReturn=.0;
		break;
	}
	return lfReturn;
}


bool ui::qt::SingSeries::event(QEvent* a_event)
{
	events::NewData* pEvent = dynamic_cast< events::NewData* >(a_event);

	if(pEvent){
		PlotGui(pEvent->numberOfPoints(),pEvent->singleItemSize(),pEvent->secondHeader(),pEvent->data());
	}
	else{
		return ::QtCharts::QLineSeries::event(a_event);
	}

	return true;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/

ui::qt::HandlerDlg::HandlerDlg(Graphic* a_pGraph, SingSeries* a_pSeries)
	:
	  ::common::ui::qt::SizeableDialog(a_pGraph)
{
	NewSettings& aSettings = thisApp()->settings();
	bool bFrameloss = aSettings.value("ui::qt::HandlerDlg::isFrameloss",false).toBool();

	m_defaultWindowFlags = windowFlags();
	m_pGraph = a_pGraph;
	m_pSeries = a_pSeries;

	m_hideUnhideProblem.setText(a_pSeries->m_statuses.bits.isProblemHidden?"Unhide overflow problem":"Hide overflow problem");
	m_deleteBtn.setText("Delete");
	m_cancel.setText("Cancel");

	m_mainLayout.addWidget(&m_hideUnhideProblem);
	m_mainLayout.addWidget(&m_deleteBtn);
	m_mainLayout.addWidget(&m_cancel);

	setLayout(&m_mainLayout);

	if(bFrameloss){
		setWindowFlags(m_defaultWindowFlags|Qt::FramelessWindowHint);
	}

	::QObject::connect(&m_deleteBtn,&QPushButton::clicked,this,[this](){
		m_pGraph->RemoveSeries(m_pSeries);
		hide();
	});

	::QObject::connect(&m_hideUnhideProblem,&QPushButton::clicked,this,[this](){
		NewSettings& aSettings = thisApp()->settings();
		bool bProblemHidden = m_pSeries->m_statuses.bits.isProblemHidden ? true : false;

		m_pSeries->m_statuses.bits.isProblemHidden = ~m_pSeries->m_statuses.bits.isProblemHidden;
		aSettings.setValue("ui::qt::SingSeries"+m_pSeries->m_details.ensPlusDoocsAdr,bProblemHidden);
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
				NewSettings& aSettings = thisApp()->settings();
				setWindowFlags(m_defaultWindowFlags);
				aSettings.setValue("ui::qt::HandlerDlg::isFrameloss",false);
				show();
			});
		}
		else{
			aMenu.addAction(tr("Hide Frame"),[this](){
				NewSettings& aSettings = thisApp()->settings();
				setWindowFlags(m_defaultWindowFlags|Qt::FramelessWindowHint);
				aSettings.setValue("ui::qt::HandlerDlg::isFrameloss",true);
				show();
			});
		}
		aMenu.exec(mapToGlobal(a_event->pos()));
	}break;
	default:
		::common::ui::qt::SizeableDialog::mousePressEvent(a_event);
		break;
	}
}
