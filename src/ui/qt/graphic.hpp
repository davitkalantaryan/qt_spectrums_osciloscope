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
#include <common/ui/qt/sizeablewidget.hpp>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <ui/qt/events.hpp>
#include <pitz_daq_data_handling_types.h>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSettings>

namespace doocs_zmq_reader{ namespace ui { namespace qt{


class SingSeries;
class HandlerDlg;
class SeriesSettings;

class Graphic final : public ::QtCharts::QChartView
{
	friend class SingSeries;
	friend class HandlerDlg;
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
	::QSettings					m_settings;
	qreal						m_lfXmin, m_lfXmax;
	qreal						m_lfYmin, m_lfYmax;

	union{
		struct{
			uint64_t	fitGraphNumber : 10;
			uint64_t	isXmaxMinInited : 1;
			uint64_t	isYmaxMinInited : 1;
		}bits;
		uint64_t allBits;
	}m_statuses;

};


class SingSeries final : public ::QtCharts::QLineSeries
{
	friend class HandlerDlg;
	friend class Graphic;
	friend class SeriesSettings;
private:
	SingSeries(Graphic* a_pParentGraphic);
	~SingSeries() override;
	//qreal  zmqToChart(int32_t singleDataSize, const char* zmqData)const;
	void PlotGui(int32_t numberOfPoint, const void* secondHeaderBuffer, const void* pData);
	bool event(QEvent* event) override;
	void SetEntryDetails(const SSingleEntryBase& entryDetails);

public:
	void SetNotAlive();

private:
	Graphic*					m_pParentGraphic;
	::QtCharts::QLegendMarker*	m_pLegendMarker;
	qreal						m_lfXkoef;
	QVector<QPointF>			m_buffer;
	SSingleEntryBase			m_details2;
	::std::list<SingSeries*>::iterator	m_iter;
	PrepareDaqEntryInputs		m_input;
	PrepareDaqEntryOutputs		m_outputs;

	union{
		struct{
			uint64_t	fitGraphNumber : 10;
			uint64_t	isAlive : 1;
		}bits;
		uint64_t allBits;
	}m_statuses;
};


class HandlerDlg final : public ::common::ui::qt::SizeableWidget< ::QDialog >
{
public:
	HandlerDlg(Graphic* pGraph, SingSeries* pSeries);

private:
	void mouseReleaseEvent(QMouseEvent * event ) override;

private:
	QVBoxLayout		m_mainLayout;
	QPushButton		m_toggleSign;
	QPushButton		m_deleteBtn;
	QPushButton		m_settingsBtn;
	QPushButton		m_cancel;
	Graphic*		m_pGraph;
	SingSeries*		m_pSeries;
	Qt::WindowFlags     m_defaultWindowFlags;

};

class SeriesSettings final : private ::common::ui::qt::SizeableWidget< ::QDialog >
{
	friend class HandlerDlg;
public:
	SeriesSettings(Graphic* pGraph,SingSeries* pSeries);

	bool MyExec();

private:
	SingSeries*		m_pSeries;
	QGridLayout		m_mainLayout;
	QLabel			m_lblXkoef;
	QDoubleSpinBox	m_XkoefSpin;
	QLabel			m_lblYkoef;
	QDoubleSpinBox	m_YkoefSpin;
	QPushButton		m_btnSet;
	QPushButton		m_btnCancel;
	bool			m_bSetPushed;

};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_GRAPHIC_HPP
