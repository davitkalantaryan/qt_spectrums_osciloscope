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

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class Graphic final : public ::QtCharts::QChartView
{
public:
	Graphic();

private:
	qreal  zmqToChart(int32_t singleDataSize, char* zmqData)const;
	void PlotGui(const QVector<QVariant>& a_readedData);

private:
	::QtCharts::QChart			m_chart;
	::QtCharts::QLineSeries		m_lineSeries;
	int32_t						m_pointsCount;
	qreal						m_lfXkoef, m_lfYkoef;
	QVector<QPointF>			m_buffer;
	::QtCharts::QValueAxis		m_axisX;
	::QtCharts::QValueAxis		m_axisY;

};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_GRAPHIC_HPP
