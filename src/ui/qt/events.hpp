//
// file:			src/ui/qt/events.hpp
// created on:		2020 Apr 04
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_EVENTS_HPP
#define SRC_UI_QT_EVENTS_HPP

#include <QEvent>
#include <QWidget>
#include <stdint.h>

class EqData;

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class SingSeries;
struct SSingleEntry;
struct SSingleEntryBase{
	SSingleEntry*	pParent;
	void*			pSocket;
	uint32_t		secondHeaderLength;
	uint32_t		singleItemSize;
	int32_t			type;
	int32_t			expectedDataLength;
	union{
		struct{
			uint64_t	isRemoved : 1;
			uint64_t	isLockedByZmqReceiver : 1;
			uint64_t	isLockedByZmqWorker : 1;
			uint64_t	isNetworkError : 1;
			uint64_t	bitwisePadding : 61;
		}bits;
		uint64_t allBits;
	}statues;
	QString			ensPlusDoocsAdr;
};

namespace events{

enum class Type{
	NewData = QEvent::User + 1,
	NamesReady = QEvent::User + 2,
	PropAddingDone = QEvent::User + 3
};

class NewData final : public QEvent
{
public:
	NewData(void* pSecondHeader, void* pData, int32_t numberOfPoints, int32_t singleItemSize );
	~NewData() override;

	const void* secondHeader()const;
	const void* data()const;
	int32_t		numberOfPoints()const;
	int32_t		singleItemSize()const;

private:
	void*		m_pSecondHeader;
	void*		m_pData;
	int32_t		m_numberOfPoints;
	int32_t		m_singleItemSize;
};


class NamesReady final : public QEvent
{
public:
	NamesReady(EqData* pData);
	~NamesReady() override;

	const EqData* data()const;

private:
	EqData*		m_pData;
};


class PropertyAddingDone final : public ::QEvent
{
public:
	PropertyAddingDone(const QString& doocsAddress, SingSeries* pSeries,const SSingleEntryBase& entryDetails);
	PropertyAddingDone(SingSeries* pSeries, const QString& errorString);

	const QString& errorStringOrDoocsAddress()const;
	bool isAddingOk()const;
	SingSeries*	pSeries();
	const SSingleEntryBase& entryDetails()const;

private:
	SingSeries*			m_pSeries;
	const QString		m_errorStringOrDoocsAddress;
	SSingleEntryBase	m_entryDetails;
	bool				m_isAddingOk;
};


}}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{ namespace events{


#endif  // #ifndef SRC_UI_QT_EVENTS_HPP
