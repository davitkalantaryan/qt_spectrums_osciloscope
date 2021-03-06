//
// file:			src/ui/qt/application.hpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef SRC_UI_QT_APPLICATION_HPP
#define SRC_UI_QT_APPLICATION_HPP

#include <QApplication>
#include <QThread>
#include <QSettings>
#include <stdint.h>
#include <queue>
#include <mutex>
#include <QVariant>
#include <thread>
#include <QVector>
#include <stddef.h>
#include <common/unnamedsemaphorelite.hpp>
#include <list>
#include <QSet> // accessed only by worker
#include <QHash> // accessed only by worker
#include <set>
#include <QDebug>

#define thisApp()							static_cast< ::doocs_zmq_reader::ui::qt::Application* >( QCoreApplication::instance() )
#define	PointerToQvariant(_ptr)				static_cast<qulonglong>(reinterpret_cast<size_t>((_ptr)))
#define	FPointerToQvariant(_fptr)			PointerToQvariant( *reinterpret_cast<void**>(&(_fptr)) )
#define VariantToPtrRaw(_typePtr,_variant)	reinterpret_cast<_typePtr>( static_cast<size_t>((_variant).toULongLong()) )
#define VariantToPtr(_type,_variant)		VariantToPtrRaw(_type*,(_variant))
#define RECENT_PAST_ENTRIES_SETTINGS_KEY	"RECENT_PAST_ENTRIES"
#define CURRENT_ENTRIES_SETTINGS_KEY		"CURRENT_ENTRIES"
//#define ENS_TERMINATOR						"=>"
#define DOOCS_TERMINATOR						"/"

//#define INDEX_FOR_TYPE				0
//#define INDEX_FOR_SINGLE_ITEM_SIZE	1
//#define INDEX_FOR_ITEMS_COUNT		2
//#define INDEX_FOR_SECOND_HEADER		3
//#define INDEX_FOR_DATA				4

class EqData;

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class Application;
struct SSingleEntry;
class SingSeries;


class StringSet
{
public:
	class Iterator{
		friend class StringSet;
		::std::set<QString>::iterator	setIter;
		::std::list<QVariant>::iterator	listIter;
	};
public:
	bool contains(const QVariant& a_other)const;
	operator QVariant ()const {return list();}
	QList<QVariant> list()const;
	void AddListContent(const QList<QVariant>& list);
	void erase (Iterator a_iter);
	Iterator insert( const QString& newItem);

private:
	::std::set<QString>		m_set;
	::std::list<QVariant>	m_list;
};

//bool GetEnsAndDoocsAddressFromSavedString(const QString& savedString, QString* pEns, QString* pDoocsAddress);

struct EarlyJob{
	enum class Type{None,Names,AddProperty,RemEntry}type;
	QWidget*	pCaller;
	QVariant	arg1;
	QVariant	arg2;
};

class WorkerThread final : public QThread
{
	void run() override;
};

class USettings
{
	friend class Application;
	USettings();
	~USettings();
	QSettings* m_pSettings;
};

class WorkerObject final : public QObject {
public:
	QObject *sender() const{ return QObject::sender();}
};


class Application final : public ::QApplication
{
	Q_OBJECT
	friend class WorkerThread;
public:
	Application(int& argc, char** argv);
	~Application() override;

	void AddNewPropertyAnyThread(QWidget* pCaller,const QString& serverAddress,SingSeries* a_pSeries);
	void GetDoocsNamesAnyThread(QWidget* pCaller,const QString& serverAddress);
	void RemoveExistingPropertyAnyThread(SSingleEntry* a_pExisting);
	void StopZmqReceiverThread();
	const QString& ensHostValue()const;
	bool GetEnsAndDoocsAddressFromSavedString(const QString& savedString, QString* pEns, QString* pDoocsAddress);
	void SetEnsHostValueAnyThread(const QString&);

private:
	void AddNewPropertyWorkerThread(QWidget* pCaller,const QString& serverAddress, SingSeries* a_pSeries);
	void GetDoocsNamesWorkerThread(QWidget* pCaller,const QString& serverAddress);
	void RemoveExistingPropertyWorkerThread(SSingleEntry* a_pExisting);

	void ZmqReceiverThread();
	void ReadAndNotifySingleEntry(SSingleEntry* pEntry);

	//
	void SetEnsHostValueWorkerThread(const QString&);
	static int SetEnsHostValueWorkerThreadRaw(const QString&);

	// replies
private:
signals:
	//void PropertyAddedToGuiSignal();
	//void PropertyAddingFailedToGuiSignal(const QString& reasonStr);
	//void GetNamesDoneToGuiSignal(const QVector<QVariant>& nmGetter);
	void GetNamesFailedToGuiSignal(const QString& reasonStr);

	// other
	void EnsHostChangedToGuiSignal();

private:
	USettings						m_appSettings;
	WorkerThread					m_workerThread;
	WorkerObject					m_livesOnWorkerThread;
	::std::queue< EarlyJob >		m_earlyJobs;
	::std::mutex					m_mutexForQueue;
	::std::thread					m_zmqReceiverThread;
	void*							m_pContext;
	QString							m_ensHostValue2;
	::common::UnnamedSemaphoreLite	m_semaForZmq;
	::std::list< SSingleEntry* >	m_listEntries;
	::std::mutex					m_mutexListEntries;
	//QHash<QString,QVariant>			m_recentPropertiesAccessedOnlyByWorker;
	StringSet						m_recentPastEntriesAccessedOnlyByWorker;
	StringSet						m_currentEntriesAccessedOnlyByWorker;
	union{
		struct{
			uint64_t	worker01Started : 1;
			uint64_t	zmqReceiverThreadShouldRun : 1;
			uint64_t	zmqReceiverThreadStarted : 1;
			uint64_t	zmqReceiverThreadFinished : 1;
		}bits;
		uint64_t	allBits;
	}m_statuses;
};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#endif  // #ifndef SRC_UI_QT_APPLICATION_HPP
