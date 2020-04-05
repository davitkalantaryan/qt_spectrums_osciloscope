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
#include <list>
#include <QLegendMarker>
#include <QLegend>
#include <QAction>
#include <common/ui/qt/sizeabledialog.hpp>
#include <QPushButton>
#include <QVBoxLayout>
#include <ui/qt/events.hpp>

namespace doocs_zmq_reader{ namespace ui { namespace qt{


class SingSeries;
class HandlerDlg;

class Graphic final : public ::QtCharts::QChartView
{
	friend class SingSeries;
public:
	Graphic();
	~Graphic()override;

	QAction* ActionAddProperty();
	void  RemoveSeries(SingSeries* pSeries);

private:
	bool event(QEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void AddNewSeries( SingSeries* newSeries, const QString& legendStr );

private:
	::QtCharts::QChart			m_chart;
	::std::list< SingSeries* >	m_listSeries;
	QAction						m_actionPlus;
	::QtCharts::QValueAxis		m_axisX;
	::QtCharts::QValueAxis		m_axisY;
	qreal						m_lfXmin, m_lfXmax;
	qreal						m_lfYmin, m_lfYmax;

	union{
		struct{
			//uint64_t	isXmaxMinNotInited : 1;
			//uint64_t	isYmaxMinNotInited : 1;
			uint64_t	isMaxMinNotInited : 1;
		}bits;
		uint64_t allBits;
	}m_statuses;

};


class SingSeries final : public ::QtCharts::QLineSeries
{
	friend class HandlerDlg;
	friend class Graphic;
private:
	SingSeries(Graphic* a_pParentGraphic);
	~SingSeries() override;
	qreal  zmqToChart(int32_t singleDataSize, const char* zmqData)const;
	void PlotGui(int32_t numberOfPoint, int32_t singleItemSize, const void* secondHeaderBuffer, const void* pData);
	bool event(QEvent* event) override;

public:
	void SetNotAlive();

private:
	Graphic*					m_pParentGraphic;
	::QtCharts::QLegendMarker*	m_pLegendMarker;
	int32_t						m_pointsCount;
	qreal						m_lfXkoef;
	qreal						m_lfYkoef;
	QVector<QPointF>			m_buffer;
	SSingleEntryBase			m_details;
	::std::list<SingSeries*>::iterator	m_iter;

	union{
		struct{
			uint64_t	isAlive : 1;
			uint64_t	isProblemHidden : 1;
		}bits;
		uint64_t allBits;
	}m_statuses;
};


class HandlerDlg final : public ::common::ui::qt::SizeableDialog
{
public:
	HandlerDlg(Graphic* pGraph, SingSeries* pSeries);

private:
	void mouseReleaseEvent(QMouseEvent * event ) override;

private:
	QVBoxLayout		m_mainLayout;
	QPushButton		m_hideUnhideProblem;
	QPushButton		m_deleteBtn;
	QPushButton		m_cancel;
	Graphic*		m_pGraph;
	SingSeries*		m_pSeries;
	Qt::WindowFlags     m_defaultWindowFlags;

};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_GRAPHIC_HPP
