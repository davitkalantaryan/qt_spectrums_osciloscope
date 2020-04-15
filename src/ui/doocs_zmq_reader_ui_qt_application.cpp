//
// file:			src/doocs_zmq_reader_ui_qt_application.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/application.hpp>
#include <QDebug>
#include <signal.h>
#include <utility>
#include <string>
#include <algorithm>
#include <zmq.h>
#include <eq_dmsg.h>
#include <pitz_daq_data_handling_types.h>
#include <new>
#include <doocsclient_dynamic.h>
#include <ui/qt/events.hpp>
#include <ui/qt/graphic.hpp>
#ifdef _WIN32
#define sleep(_x)	SleepEx(1000*(_x),TRUE)
#endif

#ifdef __USE_POSIX199309
#undef sa_handler
#define sa_handler2	__sigaction_handler.sa_handler
#endif

static const int s_cnDoocsTerminatorStringLength = static_cast<int>(strlen(DOOCS_TERMINATOR));


namespace doocs_zmq_reader{ namespace ui { namespace qt{

struct SSingleEntry{
	SSingleEntryBase			b;
	SingSeries*				pOwner;
	const ::std::string			zmqEndpoint;
	StringSet::Iterator			setIter;
	::std::list< SSingleEntry* >::iterator	listIter;
	SSingleEntry( ::std::string&& otherStr, SSingleEntryBase&& a_otherBase, SingSeries* a_pOwner );
	~SSingleEntry();
};

}}} // namespace doocs_zmq_reader{ namespace ui { namespace qt{

using namespace doocs_zmq_reader;

template  <typename PtrType>
class PtrDeleter
{
public:
	typedef void (*TypeDeleter)(PtrType*);
	PtrDeleter():m_ptr(nullptr),m_fpDeleter(nullptr){}
	PtrDeleter(const PtrDeleter&)=delete;
	PtrDeleter(PtrDeleter&&)=delete;
	~PtrDeleter(){if(m_fpDeleter){(*m_fpDeleter)(m_ptr);}}
	void set(PtrType* a_ptr, TypeDeleter a_fpDeleter){m_ptr=a_ptr;m_fpDeleter=a_fpDeleter;}

private:
	PtrType* m_ptr;
	TypeDeleter	m_fpDeleter;
};


ui::qt::Application::Application(int& a_argc, char** a_argv)
	:
	  ::QApplication (a_argc,a_argv)
{
	QSettings& aSettings = *m_appSettings.m_pSettings;
	QString aEnsHostValue;
	//QSettings aSettings;
	////::std::list<QVariant> aListStd{"","2"};
	////::QList<QVariant> aListQt = ::QList<QVariant>::fromStdList(aListStd );
	//
	////
	////aSettings.setValue("TEST1/TEST2/TEST3/TEST4/TEST5/VAL1",1);
	////aSettings.setValue("TEST1/TEST2/TEST3/TEST4/TEST5/VAL2",2);
	//aSettings.beginGroup("TEST1");
	//aSettings.beginGroup("TEST2");
	//aSettings.beginGroup("TEST3");
	//aSettings.beginGroup("TEST4");
	//aSettings.beginGroup("TEST5");
	//aSettings.setValue("VAL1",1);
	//aSettings.setValue("VAL2",2);
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.setValue("GVAL1",3);
	//aSettings.setValue("GVAL2",4);
	//aSettings.beginGroup("TEST1/AAA");
	//aSettings.beginGroup("TEST2");
	//aSettings.beginGroup("TEST3");
	//aSettings.beginGroup("TEST4");
	//aSettings.beginGroup("TEST6");
	//aSettings.setValue("VAL3",5);
	//aSettings.setValue("VAL4",6);
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.setValue("GVAL3",7);
	//aSettings.setValue("GVAL4",8);
	//aSettings.beginGroup("TEST1");
	//aSettings.beginGroup("TEST2");
	//aSettings.beginGroup("TEST3");
	//aSettings.beginGroup("TEST4");
	//aSettings.beginGroup("TEST5");
	//aSettings.setValue("VAL5",9);
	//aSettings.setValue("VAL6",10);
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	//aSettings.endGroup();
	////
	//
	//exit(0);

	m_statuses.allBits = 0;
	m_pContext = zmq_ctx_new();

	aEnsHostValue=aSettings.value("ENSHOST",getenv("ENSHOST")).toString();
	SetEnsHostValueAnyThread(aEnsHostValue);
	if(aSettings.contains(RECENT_PAST_ENTRIES_SETTINGS_KEY)){
		QList<QVariant> recentPastEntries = aSettings.value(RECENT_PAST_ENTRIES_SETTINGS_KEY).toList();
		m_recentPastEntriesAccessedOnlyByWorker.AddListContent(recentPastEntries);
	}

	// connections
	m_livesOnWorkerThread.moveToThread(&m_workerThread);
	m_statuses.bits.zmqReceiverThreadShouldRun = 1;
	m_zmqReceiverThread = ::std::thread(&Application::ZmqReceiverThread,this);
	m_workerThread.start();

}


ui::qt::Application::~Application()
{
	StopZmqReceiverThread();
	m_workerThread.quit();
	m_workerThread.wait();
}


void ui::qt::Application::StopZmqReceiverThread()
{
	if(m_statuses.bits.zmqReceiverThreadShouldRun){
		m_statuses.bits.zmqReceiverThreadShouldRun = 0;
		m_semaForZmq.post();
#ifdef _WIN32
		QueueUserAPC([](ULONG_PTR){},m_zmqReceiverThread.native_handle(),0);
#else
		pthread_kill(m_zmqReceiverThread.native_handle(),SIGPIPE);
#endif
		m_zmqReceiverThread.join();
	}
}


const QString& ui::qt::Application::ensHostValue()const
{
	return m_ensHostValue2;
}


int ui::qt::Application::SetEnsHostValueWorkerThreadRaw(const QString& a_ensHostValue)
{
	DynCleanDoocsCClient();
	return DynInitDoocsCClient(a_ensHostValue.toStdString().c_str());
}


void ui::qt::Application::SetEnsHostValueWorkerThread(const QString& a_ensHostValue)
{
	if(a_ensHostValue!=m_ensHostValue2){
		QSettings& aSettings = *m_appSettings.m_pSettings;
		SetEnsHostValueWorkerThreadRaw(a_ensHostValue);
		m_ensHostValue2 = a_ensHostValue.toStdString().c_str();
		aSettings.setValue("ENSHOST",m_ensHostValue2);
		emit EnsHostChangedToGuiSignal();
	}
}


void ui::qt::Application::SetEnsHostValueAnyThread(const QString& a_ensHostValue)
{
	QMetaObject::invokeMethod(&m_livesOnWorkerThread,[this,a_ensHostValue](){
		SetEnsHostValueWorkerThread(a_ensHostValue);
	});
}


void ui::qt::Application::RemoveExistingPropertyWorkerThread(SSingleEntry* a_pExisting)
{
	a_pExisting->b.statues.bits.isLockedByZmqWorker=1;
	a_pExisting->b.statues.bits.isRemoved = 1;

	m_currentEntriesAccessedOnlyByWorker.erase(a_pExisting->setIter);
	::QMetaObject::invokeMethod(this,[this](){
		QList<QVariant> listCurrent = m_currentEntriesAccessedOnlyByWorker.list();
		m_appSettings.m_pSettings->setValue(CURRENT_ENTRIES_SETTINGS_KEY,listCurrent);
	},Qt::BlockingQueuedConnection);

	{
		::std::lock_guard< ::std::mutex > aGuard(m_mutexListEntries);
		m_listEntries.erase(a_pExisting->listIter);
	}
	if(!a_pExisting->b.statues.bits.isLockedByZmqReceiver){
		a_pExisting->pOwner->SetNotAlive();
		delete a_pExisting;
	}
	else{
		a_pExisting->b.statues.bits.isLockedByZmqWorker=0;
	}
}


void ui::qt::Application::AddNewPropertyWorkerThread(QWidget* a_pCaller,const QString& a_ensPlusDoocsAddress,SingSeries* a_pSeries)
{
	int nReturn, nType, nPort;
	PrepareDaqEntryInputs prInps;
	PrepareDaqEntryOutputs prOuts;
	events::PropertyAddingDone* pEvent;
	QString doocsAddress;
	QString ensHost;
	bool bEnsChanged=false;
	QString ensPlusDoocsAddress;
	SSingleEntryBase aEntry;
	EqData *pDataIn=nullptr, *pDataOut=nullptr;
	EqAdr* pEqAddr=nullptr;
	EqCall* pEqCall=nullptr;
	//DEC_OUT_PD(TypeAndCount) branchInfo;
	::std::string doocsUrl ;
	::std::string hostName;
	::std::string propToSubscribe ;
	::std::string zmqEndpoint;
	PtrDeleter<EqData> aDataInDeleter;
	PtrDeleter<EqData> aDataOutDeleter;
	PtrDeleter<EqAdr> eqAdrDeleter;
	PtrDeleter<EqCall> eqCallDeleter;

	GetEnsAndDoocsAddressFromSavedString(a_ensPlusDoocsAddress,&ensHost,&doocsAddress);
	ensPlusDoocsAddress = ensHost + DOOCS_TERMINATOR + doocsAddress;
	doocsUrl = doocsAddress.toStdString();

	if( m_currentEntriesAccessedOnlyByWorker.contains(ensPlusDoocsAddress) ){
		QString reportStr = ensPlusDoocsAddress + " is already handled";
		qCritical()<<reportStr;
		pEvent = new events::PropertyAddingDone(a_pSeries,reportStr);
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		return;
	}

	if(ensHost!=m_ensHostValue2){
		bEnsChanged = true;
		if(SetEnsHostValueWorkerThreadRaw(ensHost)){
			qCritical()<<"Unable to initialize DOOCS library";
			pEvent = new events::PropertyAddingDone(a_pSeries,"Unable to initialize DOOCS library");
			::QCoreApplication::postEvent(a_pCaller,pEvent);
			goto returnPoint;
		}
	}

	pDataIn = DynGetNewEqData(); aDataInDeleter.set(pDataIn,DynDeleteEqData);
	pDataOut = DynGetNewEqData();aDataOutDeleter.set(pDataOut,DynDeleteEqData);
	pEqAddr = DynGetNewEqAdr();  eqAdrDeleter.set(pEqAddr,DynDeleteEqAdr);
	pEqCall = DynGetNewEqCall(); eqCallDeleter.set(pEqCall,DynDeleteEqCall);

	//eqAddr.adr(doocsUrl);
	DynEqAdrAdr(pEqAddr,doocsUrl);
	//nReturn = eqCall.get(&eqAddr,&dataIn,&dataOut);
	nReturn = DynEqCallGet(pEqCall,pEqAddr,pDataIn,pDataOut);
	if(nReturn){
		qCritical()<<"Unable to connect to DOOCS server";
		pEvent = new events::PropertyAddingDone(a_pSeries,"Unable to connect to DOOCS server");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}
	//branchInfo.type = dataOut.type();
	prInps.dataType=DynEqDataType(pDataOut);
	//branchInfo.itemsCountPerEntry = dataOut.length();
	prInps.countIn = DynEqDataLength(pDataOut);

	//propToSubscribe = eqAddr.property();
	propToSubscribe = DynEqAdrProperty(pEqAddr);
	//eqAddr.set_property("SPN");
	DynEqAdrSet_property(pEqAddr,"SPN");

	//pDataIn->set (1, 0.0f, 0.0f, static_cast<time_t>(0), propToSubscribe.c_str(), 0);
	DynEqDataSet1(pDataIn,1, 0.0f, 0.0f, static_cast<time_t>(0), propToSubscribe, 0);
	//nReturn=eqCall.set(&eqAddr,&dataIn,&dataOut);
	nReturn = DynEqCallSet(pEqCall,pEqAddr,pDataIn,pDataOut);

	if(nReturn){
		qCritical()<<"Unable to set data";
		pEvent = new events::PropertyAddingDone(a_pSeries,"Unable to set data");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}

	//nType=dataOut.type();
	nType = DynEqDataType(pDataOut);

	switch(nType){
	case DATA_INT:{
		//nPort = dataOut.get_int();
		nPort = DynEqDataGet_int1(pDataOut);
	}break;
	case DATA_A_USTR:{
		float f1, f2;
		time_t tm;
		char         *sp;
		//pDataOut->get_ustr (&nPort, &f1, &f2, &tm, &sp, 0);
		DynEqDataGet_ustr(pDataOut,&nPort, &f1, &f2, &tm, &sp, 0);
	}break;
	default:
		qCritical()<<"Wrong protocol";
		pEvent = new events::PropertyAddingDone(a_pSeries,"Wrong protocol");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint ;
	}  // switch(nType){

	//nReturn=pEqCall->get_option(&eqAddr,&dataIn,&dataOut,EQ_HOSTNAME);
	nReturn = DynEqCallGet_option(pEqCall,pEqAddr,pDataIn,pDataOut,EQ_HOSTNAME);
	if(nReturn<0){
		qCritical()<<"Unable to get hostname";
		pEvent = new events::PropertyAddingDone(a_pSeries,"Unable to get hostname");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}

	//hostName = pDataOut->get_string();
	hostName = DynEqDataGet_string2(pDataOut);

	zmqEndpoint = ::std::string("tcp://") + hostName + ":" + ::std::to_string(nPort);

	prInps.shouldDupString = 0;

	memset(&prOuts,0,sizeof(prOuts));
	//if(!PrepareDaqEntryBasedOnType(0,branchInfo.type,nullptr,&branchInfo,&aEntry.singleItemSize,&aEntry.secondHeaderLength,NEWNULLPTR2,NEWNULLPTR2,NEWNULLPTR2)){
	if(!PrepareDaqEntryBasedOnType(&prInps,&prOuts)){
		qCritical()<<"No information on this type";
		pEvent = new events::PropertyAddingDone(a_pSeries,"No information on this type");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}
	aEntry.type = prInps.dataType;
	aEntry.singleItemSize = prOuts.oneItemSize;
	aEntry.expectedDataLength = static_cast<int32_t>(prOuts.itemsCountPerEntry) * static_cast<int32_t>(aEntry.singleItemSize);
	aEntry.secondHeaderLength = prOuts.zmqSecondHeaderSize;

	if(!m_pContext){
		qCritical()<<"No zmq context";
		pEvent = new events::PropertyAddingDone(a_pSeries,"No zmq context");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}
	aEntry.pSocket = zmq_socket(m_pContext,ZMQ_SUB);
	if(!aEntry.pSocket){
		qCritical()<<"Unable to create zmq socket";
		pEvent = new events::PropertyAddingDone(a_pSeries,"Unable to create zmq socket");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}
	nReturn = zmq_setsockopt (aEntry.pSocket, ZMQ_SUBSCRIBE,nullptr, 0);
	if(nReturn){
		zmq_close(aEntry.pSocket);
		qCritical()<<"Unable to set zmq socket option";
		pEvent = new events::PropertyAddingDone(a_pSeries,"Unable to set zmq socket option");
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}
	nReturn = zmq_connect (aEntry.pSocket, zmqEndpoint.c_str());
	if(nReturn){
		QString errorString = QString("Unable to connect to endpoint ") + zmqEndpoint.c_str();
		zmq_close(aEntry.pSocket);
		qCritical()<<errorString;
		pEvent = new events::PropertyAddingDone(a_pSeries,errorString);
		::QCoreApplication::postEvent(a_pCaller,pEvent);
		goto returnPoint;
	}

	aEntry.pParent = new SSingleEntry( ::std::move(zmqEndpoint), ::std::move(aEntry),a_pSeries);
	aEntry.pParent->b.ensPlusDoocsAdr = ensPlusDoocsAddress;

	if(!m_recentPastEntriesAccessedOnlyByWorker.contains(ensPlusDoocsAddress)){
		m_recentPastEntriesAccessedOnlyByWorker.insert(ensPlusDoocsAddress);
		::QMetaObject::invokeMethod(this,[this](){
			QList<QVariant> listRecent = m_recentPastEntriesAccessedOnlyByWorker.list();
			m_appSettings.m_pSettings->setValue(RECENT_PAST_ENTRIES_SETTINGS_KEY,listRecent);
		},Qt::BlockingQueuedConnection);
	}

	aEntry.pParent->setIter = m_currentEntriesAccessedOnlyByWorker.insert(ensPlusDoocsAddress);

	::QMetaObject::invokeMethod(this,[this](){
		QList<QVariant> listCurrent = m_currentEntriesAccessedOnlyByWorker.list();
		m_appSettings.m_pSettings->setValue(CURRENT_ENTRIES_SETTINGS_KEY,listCurrent);
	},Qt::BlockingQueuedConnection);

	//pEntry->
	m_mutexListEntries.lock();
	m_listEntries.push_front(aEntry.pParent);
	aEntry.pParent->listIter = m_listEntries.begin();
	m_mutexListEntries.unlock();
	m_semaForZmq.post();

	//emit PropertyAddedToGuiSignal();
	pEvent = new events::PropertyAddingDone(doocsAddress,a_pSeries,aEntry.pParent->b);
	::QCoreApplication::postEvent(a_pCaller,pEvent);

returnPoint:
	if(bEnsChanged){
		if(SetEnsHostValueWorkerThreadRaw(m_ensHostValue2)){
			m_ensHostValue2="";
		}
	}
}


void ui::qt::Application::GetDoocsNamesWorkerThread(QWidget* a_pCaller,const QString& a_serverAddress)
{
	int nReturn;
	events::NamesReady* pEvent;
	//QVector<QVariant> vectReturn ( a_nmGetter );
	//QString serverAddress = a_nmGetter[1].toString();
	EqData* pData = nullptr;
	EqAdr* pEqAddr=nullptr;
	EqCall* pEqCall=nullptr;
	::std::string adrString = a_serverAddress.toStdString();
	PtrDeleter<EqAdr> eqAdrDeleter;
	PtrDeleter<EqCall> eqCallDeleter;

	if(DynInitDoocsCClient(m_ensHostValue2.toStdString().c_str())){
		qCritical()<<"Unable to initialize DOOCS library";
		emit GetNamesFailedToGuiSignal("Unable to initialize DOOCS library");
		return;
	}

	pEqAddr = DynGetNewEqAdr();eqAdrDeleter.set(pEqAddr,DynDeleteEqAdr);
	pEqCall = DynGetNewEqCall();eqCallDeleter.set(pEqCall,DynDeleteEqCall);

	//pData = new EqData;
	pData = DynGetNewEqData();
	//eqAddr.adr(adrString);
	DynEqAdrAdr(pEqAddr,adrString);
	//nReturn=pEqCall->names(&eqAddr,pData);
	nReturn=DynEqCallNames(pEqCall,pEqAddr,pData);
	//if(nReturn || (pData->type()!=DATA_A_USTR)){
	if(nReturn || (DynEqDataType(pData)!=DATA_A_USTR)){
		//delete pData;
		DynDeleteEqData(pData);
		qCritical()<<"Unable to get names";
		emit GetNamesFailedToGuiSignal("Unable to get names");
		return;
	}

	pEvent = new events::NamesReady(pData);
	::QCoreApplication::postEvent(a_pCaller,pEvent);

	//vectReturn.push_back(PointerToQvariant(pData));
	//emit GetNamesDoneToGuiSignal(vectReturn);
}


void ui::qt::Application::AddNewPropertyAnyThread(QWidget* a_pCaller,const QString& a_serverAddress,SingSeries* a_pSeries)
{
	if(m_statuses.bits.worker01Started){
		//emit ConnectToServerToWorkerSignal(a_serverAddress);
		QMetaObject::invokeMethod(&m_livesOnWorkerThread,[this,a_pCaller,a_serverAddress,a_pSeries](){
			AddNewPropertyWorkerThread(a_pCaller,a_serverAddress,a_pSeries);
		});
	}
	else{
		::std::lock_guard< ::std::mutex > aGuard(m_mutexForQueue);
		m_earlyJobs.push({ EarlyJob::Type::AddProperty,a_pCaller,a_serverAddress,PointerToQvariant(a_pSeries)});
	}
}


void ui::qt::Application::GetDoocsNamesAnyThread(QWidget* a_pCaller,const QString& a_serverAddress)
{
	if(m_statuses.bits.worker01Started){
		//emit GetDoocsNamesToWorkerSignal({PointerToQvariant(a_pCaller),a_serverAddress});
		QMetaObject::invokeMethod(&m_livesOnWorkerThread,[this,a_serverAddress,a_pCaller](){
			GetDoocsNamesWorkerThread(a_pCaller,a_serverAddress);
		});
	}
	else{
		::std::lock_guard< ::std::mutex > aGuard(m_mutexForQueue);
		m_earlyJobs.push({ EarlyJob::Type::Names,a_pCaller,a_serverAddress,QVariant()});
	}
}


void ui::qt::Application::RemoveExistingPropertyAnyThread(SSingleEntry* a_pExisting)
{
	if(m_statuses.bits.worker01Started){
		//emit GetDoocsNamesToWorkerSignal({PointerToQvariant(a_pCaller),a_serverAddress});
		QMetaObject::invokeMethod(&m_livesOnWorkerThread,[this,a_pExisting](){
			RemoveExistingPropertyWorkerThread(a_pExisting);
		});
	}
	else{
		::std::lock_guard< ::std::mutex > aGuard(m_mutexForQueue);
		m_earlyJobs.push({ EarlyJob::Type::RemEntry,nullptr,PointerToQvariant(a_pExisting),QVariant()});
	}
}


void ui::qt::Application::ReadAndNotifySingleEntry(SSingleEntry* a_pEntry)
{
	int nReturn;
	events::NewData* pEvent;
	dmsg_hdr_t aDcsHeader;
	bool isBadAlloc = false;
	void* pBufferForSecondHeader = nullptr;
	void* pBufferForData = nullptr;

	if(a_pEntry->b.expectedDataLength<1){return;}

	if(a_pEntry->b.secondHeaderLength>0){
		pBufferForSecondHeader = malloc(a_pEntry->b.secondHeaderLength);
		if(!pBufferForSecondHeader){isBadAlloc=true;goto returnPoint;}
	}
	pBufferForData = malloc(static_cast<size_t>(a_pEntry->b.expectedDataLength));
	if(!pBufferForData){isBadAlloc=true;goto returnPoint;}

	nReturn=zmq_recv(a_pEntry->b.pSocket,&aDcsHeader,sizeof(dmsg_hdr_t),0);
	if(nReturn<4){
		goto returnPoint;
	}

	switch(aDcsHeader.vers){
	case 1:{
		struct dmsg_header_v1* pHeaderV1 = reinterpret_cast<struct dmsg_header_v1*>(&aDcsHeader);
		short exthd = static_cast<short>(DMSG_HDR_EXT);
		uint64_t ullnSec = static_cast<uint64_t>(pHeaderV1->sec) & 0x0fffffffful;
		if (pHeaderV1->size & exthd) {
			// extended header with 64 bit seconds
			// sechi contains seconds' high 32 bit word
			ullnSec |= ( static_cast<uint64_t>(pHeaderV1->sechi) & 0x0fffffffful) << 32;
			pHeaderV1->size &= ~exthd;
		}
		if(nReturn != pHeaderV1->size){
			// this is not header give chance for header
			goto returnPoint;
		}
	}break;
	default:
		goto returnPoint;
	}

	if(a_pEntry->b.secondHeaderLength>0){
		nReturn=zmq_recv(a_pEntry->b.pSocket,pBufferForSecondHeader,a_pEntry->b.secondHeaderLength,0);
		if(nReturn!=static_cast<int>(a_pEntry->b.secondHeaderLength)){
			goto returnPoint;
		}
		//vectGraph[INDEX_FOR_SECOND_HEADER]=PointerToQvariant(pBufferForSecondHeader);
	}

	nReturn=zmq_recv(a_pEntry->b.pSocket,pBufferForData,static_cast<size_t>(a_pEntry->b.expectedDataLength),0);
	if(nReturn<1){
		a_pEntry->b.statues.bits.isNetworkError = 1;
		goto returnPoint;
	}
	else if(nReturn<static_cast<int>(a_pEntry->b.expectedDataLength)){
		a_pEntry->b.expectedDataLength = nReturn;
	}
	else if(nReturn>a_pEntry->b.expectedDataLength) {
		// this one we will not handle, lets delete this buffer and wait for next
		a_pEntry->b.expectedDataLength = nReturn;
		goto returnPoint;
	}

	pEvent = new events::NewData(pBufferForSecondHeader,pBufferForData,
								 a_pEntry->b.expectedDataLength/static_cast<int32_t>(a_pEntry->b.singleItemSize),
								 static_cast<int32_t>(a_pEntry->b.singleItemSize));
	::QCoreApplication::postEvent(a_pEntry->pOwner,pEvent);

	pBufferForSecondHeader=nullptr;
	pBufferForData=nullptr;

returnPoint:
	if(pBufferForSecondHeader){free(pBufferForSecondHeader);pBufferForSecondHeader=nullptr;}
	if(pBufferForData){free(pBufferForData);pBufferForData=nullptr;}
	if(isBadAlloc){throw ::std::bad_alloc();}
}


bool ui::qt::Application::GetEnsAndDoocsAddressFromSavedString(const QString& a_inputString, QString* a_pEns, QString* a_pDoocsAddress)
{
	bool bRet(false);
	int nCount = a_inputString.count(DOOCS_TERMINATOR);
	if(nCount>3){  // ens hosts is embedded here
		int nIndex = a_inputString.indexOf(DOOCS_TERMINATOR);
		*a_pEns = a_inputString.mid(0,nIndex);
		*a_pDoocsAddress = a_inputString.mid(nIndex+s_cnDoocsTerminatorStringLength);
		if((*a_pEns)!=m_ensHostValue2){
			bRet = true;
		}
	}
	else{
		*a_pEns = m_ensHostValue2;
		*a_pDoocsAddress = a_inputString;
	}

	return bRet;
}


void ui::qt::Application::ZmqReceiverThread()
{
	typedef SSingleEntry* SSingleEntryPtr;
	m_statuses.bits.zmqReceiverThreadStarted = 1;

	zmq_pollitem_t *pItems = nullptr, *pItemTmp;
	bool bShouldSleep=false;
	int nIndex;
	int nCurrentItems = 0;
	int nMaxCreatedItems = 0;
	int nReturn;
	::std::list< SSingleEntry* >::iterator siIter;
	//::std::vector< SSingleEntry* >	vectOnPoll;
	SSingleEntry*	pEntry;
	SSingleEntryPtr *ppOnPoll = nullptr, *ppOnPollTmp;

#ifdef _WIN32
#else
	struct sigaction oldSigaction, newSigAction;

	sigemptyset(&newSigAction.sa_mask);
	newSigAction.sa_flags = 0;
	newSigAction.sa_restorer = nullptr;
	newSigAction.sa_handler2 = [](int){};
	sigaction(SIGPIPE,&newSigAction,&oldSigaction);
#endif

	while(m_statuses.bits.zmqReceiverThreadShouldRun){

		if(nCurrentItems<1){
			m_semaForZmq.wait();
		}

		if(bShouldSleep){
			sleep(1);
			bShouldSleep = false;
		}

		m_mutexListEntries.lock();
		nCurrentItems = static_cast<int>( m_listEntries.size() );

		if(nCurrentItems<1){
			m_mutexListEntries.unlock();
			continue;
		}

		if(nCurrentItems>nMaxCreatedItems){
			pItemTmp = static_cast<zmq_pollitem_t*>(realloc(pItems,static_cast<size_t>(nCurrentItems)*sizeof(zmq_pollitem_t)));
			if(!pItemTmp){
				m_mutexListEntries.unlock();
				throw ::std::bad_alloc();
			}
			pItems = pItemTmp;
			ppOnPollTmp = static_cast<SSingleEntryPtr*>(realloc(ppOnPoll,static_cast<size_t>(nCurrentItems)*sizeof(SSingleEntryPtr)));
			if(!ppOnPollTmp){
				m_mutexListEntries.unlock();
				throw ::std::bad_alloc();
			}
			ppOnPoll = ppOnPollTmp;
			nMaxCreatedItems = nCurrentItems;
		}

		siIter = m_listEntries.begin();
		for(nIndex=0;nIndex<nCurrentItems;++nIndex,++siIter){
			pItems[nIndex].fd = 0;
			pItems[nIndex].revents = 0;
			pItems[nIndex].socket = (*siIter)->b.pSocket;
			pItems[nIndex].events = ZMQ_POLLIN;
			ppOnPoll[nIndex]=(*siIter);
			ppOnPoll[nIndex]->b.statues.bits.isLockedByZmqReceiver = 1;
		}

		m_mutexListEntries.unlock();

		nReturn=zmq_poll(pItems,nCurrentItems,-1);
		if(nReturn<0){
			bShouldSleep = true;
			continue;
		}

		for(nIndex=0;nIndex<nCurrentItems;++nIndex){
			pEntry=ppOnPoll[nIndex];

			if(pEntry->b.statues.bits.isRemoved){
				pEntry->pOwner->SetNotAlive();
				pEntry->b.statues.bits.isLockedByZmqReceiver = 0;
				if(!pEntry->b.statues.bits.isLockedByZmqWorker){
					delete pEntry;
				}
				continue;
			}

			if(pItems[nIndex].revents & ZMQ_POLLIN){
				ReadAndNotifySingleEntry(pEntry);
			}
		}

	}  // while(m_statuses.bits.zmqReceiverThreadShouldRun){

#ifdef _WIN32
#else
	sigaction(SIGPIPE,&oldSigaction,nullptr);
#endif

	m_statuses.bits.zmqReceiverThreadFinished = 1;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/
ui::qt::USettings::USettings()
{
	QCoreApplication::setOrganizationName("DESY");
	QCoreApplication::setApplicationName("doocs_zmq_reader");
	QSettings::setDefaultFormat(QSettings::IniFormat); // use ini files on all platforms
	m_pSettings = new QSettings();

#ifdef NewSettings_redefined
	QSettings::Format fmt = m_pSettings->m_settings.format();
	QSettings::Scope scp = m_pSettings->m_settings.scope();
	QString filePath = m_pSettings->m_settings.fileName();
#else
	QSettings::Format fmt = m_pSettings->format();
	QSettings::Scope scp = m_pSettings->scope();
	QString filePath = m_pSettings->fileName();
#endif
	qDebug()<<fmt<<scp<<filePath;
}


ui::qt::USettings::~USettings()
{
	delete m_pSettings;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/
void ui::qt::WorkerThread::run()
{
	EarlyJob aJob;

#ifdef _WIN32
#else
	struct sigaction oldSigaction, newSigAction;

	sigemptyset(&newSigAction.sa_mask);
	newSigAction.sa_flags = 0;
	newSigAction.sa_restorer = nullptr;
	newSigAction.sa_handler2 = [](int){};
	sigaction(SIGPIPE,&newSigAction,&oldSigaction);
#endif

	thisApp()->m_statuses.bits.worker01Started = 1;

	{
		::std::lock_guard< ::std::mutex > aGuard( thisApp()->m_mutexForQueue );
		while( thisApp()->m_earlyJobs.size() ){
			aJob = thisApp()->m_earlyJobs.front();
			switch(aJob.type){
			case EarlyJob::Type::Names:
				thisApp()->GetDoocsNamesWorkerThread(aJob.pCaller,aJob.arg1.toString());
				break;
			case EarlyJob::Type::AddProperty:
				thisApp()->AddNewPropertyWorkerThread(aJob.pCaller,aJob.arg1.toString(),VariantToPtr(SingSeries,aJob.arg2));
				break;

			case EarlyJob::Type::RemEntry:
				thisApp()->RemoveExistingPropertyWorkerThread(VariantToPtr(SSingleEntry,aJob.arg1));
				break;
			default:
				break;
			} // switch(aJob.type){
			thisApp()->m_earlyJobs.pop();
		}  // while( thisApp()->m_earlyJobs.size() ){
	}

	QThread::exec();

#ifdef _WIN32
#else
	sigaction(SIGPIPE,&oldSigaction,nullptr);
#endif
}


/*/////////////////////////////////////////////////////////////////////////////////////*/
#if 0
	struct SSingleEntry{
		void*		pSocket;
		SingSeries* pOwner;
		uint32_t secondHeaderLength;
		uint32_t singleItemSize;
		DEC_OUT_PD(TypeAndCount) branchInfo;
		const ::std::string zmqEndpoint;
		union{
			struct{
				uint64_t	isDeleted : 1;
				uint64_t	isLockedByZmqReceiver : 1;
				uint64_t	bitwisePadding : 62;
			}bits;
			uint64_t allBits;
		}statues;
		SSingleEntry(){this->statues.allBits=0;}
	};
#endif
ui::qt::SSingleEntry::SSingleEntry( ::std::string&& a_otherStr, SSingleEntryBase&& a_otherBase, SingSeries* a_pOwner )
	:
	  b( ::std::move(a_otherBase) ),
	  pOwner(a_pOwner),
	  zmqEndpoint( ::std::move(a_otherStr) )
{
	this->b.pParent = this;
}


ui::qt::SSingleEntry::~SSingleEntry()
{
	if(this->b.pSocket){
		zmq_close(this->b.pSocket);
	}
}


/*/////////////////////////////////////////////////////////////////////////////////////*/

bool ui::qt::StringSet::contains(const QVariant& a_other)const
{
	return m_set.count(a_other.toString());
}


::QList< ::QVariant > ui::qt::StringSet::list()const
{
	return ::QList<QVariant>::fromStdList(m_list);
}


void ui::qt::StringSet::AddListContent(const QList<QVariant>& a_list)
{
	for(auto newEntry : a_list){
		if(!m_set.count(newEntry.toString())){
			m_list.push_back(newEntry);
			m_set.insert(newEntry.toString());
		}
	}
}


void ui::qt::StringSet::erase (Iterator a_iter)
{
	m_list.erase(a_iter.listIter);
	m_set.erase(a_iter.setIter);
}


ui::qt::StringSet::Iterator ui::qt::StringSet::insert( const QString& a_newItem)
{
	Iterator retIter;
	std::pair<std::set<QString>::iterator,bool> setRet;
	m_list.push_back(a_newItem);
	retIter.listIter = m_list.end();
	--(retIter.listIter);
	setRet = m_set.insert(a_newItem);
	retIter.setIter = setRet.first;
	return retIter;
}


/*/////////////////////////////////////////////////////////////////////////////////////*/


namespace doocs_zmq_reader{ namespace ui { namespace qt{

//bool GetEnsAndDoocsAddressFromSavedString(const QString& a_savedString, QString* a_pEns, QString* a_pDoocsAddress)
//{
//	bool bRet=false;
//	int nIndex = a_savedString.indexOf(ENS_TERMINATOR2);
//	if(nIndex>=1){
//		bRet = true;
//		*a_pEns = a_savedString.mid(0,nIndex);
//		*a_pDoocsAddress = a_savedString.mid(nIndex+s_cnEnsTerminatorStringLength);
//	}
//	else{
//		*a_pDoocsAddress = a_savedString;
//	}
//
//	return bRet;
//}

}}} // namespace doocs_zmq_reader{ namespace ui { namespace qt{
