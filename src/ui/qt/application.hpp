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

#define INDEX_FOR_TYPE				0
#define INDEX_FOR_SINGLE_ITEM_SIZE	1
#define INDEX_FOR_ITEMS_COUNT		2
#define INDEX_FOR_SECOND_HEADER		3
#define INDEX_FOR_DATA				4

class EqData;

namespace doocs_zmq_reader{ namespace ui { namespace qt{

class Application;

struct EarlyJob{
	enum class Type{None,Names,Connect}type;
	QVariant	arg;
	QWidget*	pCaller;
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

	void ConnectToServerAnyThread(const QString& serverAddress);
	void GetDoocsNamesAnyThread(const QString& serverAddress,QWidget* pCaller);

private:
	void ConnectToServerWorkerThread(const QString& serverAddress);
	void GetDoocsNamesWorkerThread(const QVector<QVariant>& nmGetter);

	// requests
private:
signals:
	void ConnectToServerToWorkerSignal(const QString& serverAddress); // no direct send
	void GetDoocsNamesToWorkerSignal(const QVector<QVariant>& nmGetter); // no direct send

	// other
	void ZmqReadDoneSignal( const QVector<QVariant>& readedData );

	// replies
private:
signals:
	void ConnectionDoneToGuiSignal();
	void ConnectionFailedToGuiSignal(const QString& reasonStr);
	void GetNamesDoneToGuiSignal(const QVector<QVariant>& nmGetter);
	void GetNamesFailedToGuiSignal(const QString& reasonStr);

private:
	USettings					m_appSettings;
	WorkerThread				m_workerThread;
	WorkerObject				m_livesOnWorkerThread;
	::std::queue< EarlyJob >	m_earlyJobs;
	::std::mutex				m_mutexForQueue;
	::std::thread				m_zmqThread;
	void*						m_pContext;
	union{
		struct{
			uint64_t  worker01Started : 1;
			uint64_t  zmqThreadRunning : 1;
			uint64_t  zmqThreadShouldRun : 1;
		}bits;
		uint64_t	allBits;
	}m_statuses;
};

}}}  // namespace doocs_zmq_reader{ namespace ui { namespace qt{


#define thisApp()						static_cast< ::doocs_zmq_reader::ui::qt::Application* >( QCoreApplication::instance() )
#define	PointerToQvariant(_ptr)			static_cast<qulonglong>(reinterpret_cast<size_t>((_ptr)))
#define VariantToPtr(_type,_variant)	reinterpret_cast<_type*>( static_cast<size_t>((_variant).toULongLong()) )

#endif  // #ifndef SRC_UI_QT_APPLICATION_HPP
