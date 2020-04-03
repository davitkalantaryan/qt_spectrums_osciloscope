//
// file:			src/doocs_zmq_reader_ui_qt_graphic.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/graphic.hpp>
#include <ui/qt/application.hpp>

using namespace doocs_zmq_reader;


ui::qt::Graphic::Graphic()
{
	m_lfXkoef = m_lfYkoef = 1.0;
	m_pointsCount = 0;
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
	int32_t unIter;
	int32_t newPointsCount = a_readedData[INDEX_FOR_ITEMS_COUNT].toInt();
	int32_t nSingleItemSize = a_readedData[INDEX_FOR_SINGLE_ITEM_SIZE].toInt();
	char* pcData = VariantToPtr(char,a_readedData[INDEX_FOR_DATA]);
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

		for(unIter=0;unIter<m_pointsCount;++unIter){
			m_buffer[unIter].setX(static_cast<qreal>(unIter)*m_lfXkoef);
		}

	}  // if(newPointsCount!=m_lastPointsCount){

	for(unIter=0;unIter<m_pointsCount;++unIter){
		m_buffer[unIter].setY(zmqToChart(nSingleItemSize,pcData));
		pcData += nSingleItemSize;
	}
	m_lineSeries.replace(m_buffer);
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

	return lfReturn;
}
