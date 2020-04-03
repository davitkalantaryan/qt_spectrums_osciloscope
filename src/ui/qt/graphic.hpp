//
// file:			src/ui/qt/graphic.hpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_GRAPHIC_HPP
#define SRC_UI_QT_GRAPHIC_HPP

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <stdint.h>
#include <QPointF>
#include <QVector>
#include <QVariant>
#include <QValueAxis>
#include <stdint.h>

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class Graphic final : public ::QtCharts::QChartView
//class Graphic final : public ::QWidget
{
public:
	Graphic();

private:
	qreal  zmqToChart(int32_t singleDataSize, char* zmqData)const;
	void PlotGui(const QVector<QVariant>& a_readedData);

private:
	//::QtCharts::QChart			m_chart;
	//::QtCharts::QLineSeries		m_lineSeries;
	//int32_t						m_pointsCount;
	//qreal						m_lfXkoef, m_lfYkoef;
	//QVector<QPointF>			m_buffer;
	//::QtCharts::QValueAxis		m_axisX;
	//::QtCharts::QValueAxis		m_axisY;

	::QtCharts::QChart			m_chart;
	::QtCharts::QLineSeries		m_series ;
	::QtCharts::QValueAxis		m_axisX;
	::QtCharts::QValueAxis		m_axisY;
	int32_t						m_pointsCount;
	qreal						m_lfXkoef, m_lfYkoef;
	qreal						m_lfYmin, m_lfYmax;
	QVector<QPointF>			m_buffer;

	union{
		struct{
			uint64_t	isMaxMinNotInited : 1;
		}bits;
		uint64_t allBits;
	}m_statuses;

};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_GRAPHIC_HPP
