//
// file:			src/doocs_zmq_reader_ui_qt_events.cpp
// created on:		2020 Apr 04
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/events.hpp>
#include <QDebug>
#include <doocsclient_dynamic.h>
#include <stdlib.h>

using namespace doocs_zmq_reader;

ui::qt::events::NewData::NewData(void* a_pSecondHeader, void* a_pData, int32_t a_numberOfPoints, int32_t a_singleItemSize)
	:
	  QEvent( static_cast< ::QEvent::Type >(ui::qt::events::Type::NewData) ),
	  m_pSecondHeader(a_pSecondHeader),
	  m_pData(a_pData),
	  m_numberOfPoints(a_numberOfPoints),
	  m_singleItemSize(a_singleItemSize)
{
}


ui::qt::events::NewData::~NewData()
{
	free(m_pData);
	free(m_pSecondHeader);
	//qDebug()<<__FUNCTION__;
}


const void* ui::qt::events::NewData::secondHeader()const
{
	return m_pSecondHeader;
}


const void* ui::qt::events::NewData::data()const
{
	return m_pData;
}


int32_t ui::qt::events::NewData::numberOfPoints()const
{
	return m_numberOfPoints;
}


int32_t	ui::qt::events::NewData::singleItemSize()const
{
	return m_singleItemSize;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/

ui::qt::events::NamesReady::NamesReady(EqData* a_pData)
	:
	  ::QEvent( static_cast< ::QEvent::Type >(ui::qt::events::Type::NamesReady) ),
	  m_pData(a_pData)
{
	//
}


ui::qt::events::NamesReady::~NamesReady()
{
	DynDeleteEqData(m_pData);
	qDebug()<<__FUNCTION__;
}


const EqData* ui::qt::events::NamesReady::data()const
{
	return m_pData;
}

/*/////////////////////////////////////////////////////////////////////////////////////*/

ui::qt::events::PropertyAddingDone::PropertyAddingDone(const QString& a_doocsAddress,SingSeries* a_pSeries,const SSingleEntryBase& a_entryDetails)
	:
	  ::QEvent( static_cast< ::QEvent::Type >(ui::qt::events::Type::PropAddingDone) ),
	  m_pSeries(a_pSeries),
	  m_errorStringOrDoocsAddress(a_doocsAddress),
	  m_entryDetails(a_entryDetails),
	  m_isAddingOk(true)
{
}


ui::qt::events::PropertyAddingDone::PropertyAddingDone(SingSeries* a_pSeries, const QString& a_errorString)
	:
	  ::QEvent( static_cast< ::QEvent::Type >(ui::qt::events::Type::PropAddingDone) ),
	  m_pSeries(a_pSeries),
	  m_errorStringOrDoocsAddress(a_errorString),
	  m_isAddingOk(false)
{
}


const QString& ui::qt::events::PropertyAddingDone::errorStringOrDoocsAddress()const
{
	return m_errorStringOrDoocsAddress;
}


bool ui::qt::events::PropertyAddingDone::isAddingOk()const
{
	return m_isAddingOk;
}


ui::qt::SingSeries*	ui::qt::events::PropertyAddingDone::pSeries()
{
	return m_pSeries;
}


const ui::qt::SSingleEntryBase& ui::qt::events::PropertyAddingDone::entryDetails()const
{
	return m_entryDetails;
}
