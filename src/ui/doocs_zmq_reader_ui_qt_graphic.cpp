//
// file:			src/doocs_zmq_reader_ui_qt_graphic.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/graphic.hpp>
#include <ui/qt/application.hpp>
#include <QVBoxLayout>
#include <QDebug>
#include <stdlib.h>

using namespace doocs_zmq_reader;


ui::qt::Graphic::Graphic()
	:
	  m_chart(new ::QtCharts::QChart),
		  m_series(new ::QtCharts::QLineSeries)
{

	m_statuses.allBits = 0;
	m_statuses.bits.isMaxMinNotInited = 1;

	::QtCharts::QChartView *chartView = new ::QtCharts::QChartView(m_chart);
		chartView->setMinimumSize(800, 600);
		m_chart->addSeries(m_series);
		m_axisX = new ::QtCharts::QValueAxis;
		m_axisX->setLabelFormat("%g");
		m_axisX->setTitleText("Samples");
		m_axisY = new ::QtCharts::QValueAxis;
		//m_axisX->setRange(0, 10000);
		//m_axisY->setRange(0,35000);
		m_axisY->setTitleText("Audio level");
		m_chart->setAxisX(m_axisX, m_series);
		m_chart->setAxisY(m_axisY, m_series);
		m_chart->legend()->hide();
		m_chart->setTitle("Data from the microphone (" + QString("TestName") + ')');

		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(chartView);


#if 0
	//m_chart.legend()->hide();
	//m_chart.addSeries(&m_lineSeries);
	//setChart(&m_chart);
	//m_chart.addSeries(&m_lineSeries);

	//QChartView *chartView = new QChartView(m_chart);
	setMinimumSize(800, 600);
	m_chart.addSeries(&m_lineSeries);
	//QValueAxis *axisX = new QValueAxis;
	//m_axisX.setRange(0, 0); // !!!
	m_axisX.setRange(0, 1000);
	m_axisX.setLabelFormat("%g");
	m_axisX.setTitleText("Samples");
	//QValueAxis *axisY = new QValueAxis;
	m_axisY.setRange(0,32799);
	m_axisY.setTitleText("Audio level");

	//m_chart.setAxisX(&m_axisX, m_lineSeries);
	//m_chart->setAxisY(axisY, m_series);

	m_chart.legend()->hide();

	m_chart.setTitle("Data from the microphone (" + QString("TestName") + ')');

#endif

	m_lfXkoef = m_lfYkoef = 1.0;
	m_pointsCount = 0;

	// connections
	::QObject::connect(thisApp(),&Application::ZmqReadDoneSignal,this,[this](const QVector<QVariant>& a_readedData){
		PlotGui(a_readedData);
	});
}


void ui::qt::Graphic::PlotGui(const QVector<QVariant>& a_readedData)
{
	//QLine
//#define INDEX_FOR_TYPE				0
//#define INDEX_FOR_SINGLE_ITEM_SIZE	1
//#define INDEX_FOR_ITEMS_COUNT		2
//#define INDEX_FOR_SECOND_HEADER		3
//#define INDEX_FOR_DATA				4
	bool bYmaxOrMinChanged=false;
	qreal lfCurrentValue;
	int32_t unIter=0;
	int32_t newPointsCount = a_readedData[INDEX_FOR_ITEMS_COUNT].toInt();
	int32_t nSingleItemSize = a_readedData[INDEX_FOR_SINGLE_ITEM_SIZE].toInt();
	char* pcSecondHeaderInit = VariantToPtr(char,a_readedData[INDEX_FOR_SECOND_HEADER]);
	char* pcDataInit = VariantToPtr(char,a_readedData[INDEX_FOR_DATA]);
	char* pcData = pcDataInit;
	if(newPointsCount!=m_pointsCount){
		m_buffer.resize(newPointsCount);
		m_pointsCount = newPointsCount;
		//if(newPointsCount<m_lastPointsCount){
		//	for(unIter=newPointsCount;unIter<m_lastPointsCount;++unIter){
		//		m_lineSeries.remove(unIter);
		//	}
		//}
		//else{
		//	for(unIter=m_lastPointsCount;unIter<newPointsCount;++unIter){
		//		m_lineSeries.append(static_cast<qreal>(unIter),zmqToChart(nSingleItemSize,pcData));
		//		pcData += nSingleItemSize;
		//	}
		//}
		//m_lastPointsCount = newPointsCount;

		m_lfXmin = 0.;
		m_lfXmax = static_cast<qreal>(m_pointsCount)*m_lfXkoef;

		m_axisX->setRange(m_lfXmin,m_lfXmax);

		for(;unIter<m_pointsCount;++unIter){
			m_buffer[unIter].setX(static_cast<qreal>(unIter)*m_lfXkoef);
		}

		if(m_statuses.bits.isMaxMinNotInited){
			m_lfYmin = m_lfYmax = zmqToChart(nSingleItemSize,pcData);
			pcData += nSingleItemSize;
			m_buffer[0].setY(m_lfYmin);
			unIter = 1;
			m_statuses.bits.isMaxMinNotInited=0;
		}

	}  // if(newPointsCount!=m_lastPointsCount){

	for(;unIter<m_pointsCount;++unIter){
		lfCurrentValue = zmqToChart(nSingleItemSize,pcData);
		pcData += nSingleItemSize;
		if(lfCurrentValue>m_lfYmax){m_lfYmax=lfCurrentValue; bYmaxOrMinChanged=true;}
		if(lfCurrentValue<m_lfYmin){m_lfYmin=lfCurrentValue; bYmaxOrMinChanged=true;}
		m_buffer[unIter].setY(lfCurrentValue);
	}
	free(pcDataInit);
	free(pcSecondHeaderInit);
	if(bYmaxOrMinChanged){
		m_axisY->setRange(m_lfYmin,m_lfYmax);
	}
	m_series->replace(m_buffer);
}


qreal ui::qt::Graphic::zmqToChart(int32_t a_singleDataSize, char* a_zmqData)const
{
	qreal lfReturn;
	switch(a_singleDataSize){
	case 2:
		lfReturn = static_cast<qreal>(*reinterpret_cast<uint16_t*>(a_zmqData))*m_lfYkoef;
		break;
	default:
		lfReturn=.0;
		break;
	}

	//static qreal minValue = 100000.;
	//static qreal maxValue = -100000.;
	//if(lfReturn>maxValue){
	//	maxValue=lfReturn;
	//	qDebug()<<"[min,max]=["<<minValue<<","<<maxValue<<"]";
	//}
	//if(lfReturn<minValue){
	//	minValue=lfReturn;
	//	qDebug()<<"[min,max]=["<<minValue<<","<<maxValue<<"]";
	//}
	return lfReturn;
}
