//
// file:			src/doocs_zmq_reader_ui_qt_application.cpp
// created on:		2020 Apr 02
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <ui/qt/application.hpp>
#include <QDebug>
#include <signal.h>
#include <utility>
#include <eq_client.h>
#include <string>
#include <algorithm>
#include <zmq.h>
#include <eq_dmsg.h>
#include <pitz_daq_data_handling_types.h>
#include <new>

#ifdef __USE_POSIX199309
#undef sa_handler
#define sa_handler2	__sigaction_handler.sa_handler
#endif

using namespace doocs_zmq_reader;


ui::qt::Application::Application(int& a_argc, char** a_argv)
	:
	  ::QApplication (a_argc,a_argv)
{
	m_statuses.allBits = 0;
	m_pContext = zmq_ctx_new();

	// connections
	::QObject::connect(this,&Application::GetDoocsNamesToWorkerSignal,&m_livesOnWorkerThread,[this](const QVector<QVariant>& a_nmGetter){
		GetDoocsNamesWorkerThread(a_nmGetter);
	});

	::QObject::connect(this,&Application::ConnectToServerToWorkerSignal,&m_livesOnWorkerThread,[this](const QString& a_serverAddress){
		ConnectToServerWorkerThread(a_serverAddress);
	});

	m_livesOnWorkerThread.moveToThread(&m_workerThread);
	m_workerThread.start();
}


ui::qt::Application::~Application()
{
	if(m_statuses.bits.zmqThreadShouldRun){
		m_statuses.bits.zmqThreadShouldRun = 0;
		m_zmqThread.join();
	}
	m_workerThread.quit();
	m_workerThread.wait();
}


void ui::qt::Application::ConnectToServerWorkerThread(const QString& a_serverAddress)
{
	int nReturn, nType, nPort;
	EqData dataIn, dataOut;
	EqAdr eqAddr;
	EqCall eqCall;
	::std::string doocsUrl = a_serverAddress.toStdString();
	::std::string hostName;
	::std::string propToSubscribe ;
	uint32_t singleItemSize,secondHeaderLength;
	DEC_OUT_PD(TypeAndCount) branchInfo;

	eqAddr.adr(doocsUrl);
	nReturn = eqCall.get(&eqAddr,&dataIn,&dataOut);
	if(nReturn){
		qCritical()<<"Unable to connect to DOOCS server";
		emit ConnectionFailedToGuiSignal("Unable to connect to DOOCS server");
		return;
	}
	branchInfo.type = dataOut.type();
	branchInfo.itemsCountPerEntry = dataOut.length();

	//eqAddr.adr(doocsUrl);
	propToSubscribe = eqAddr.property();
	eqAddr.set_property("SPN");

	dataIn.set (1, 0.0f, 0.0f, static_cast<time_t>(0), propToSubscribe.c_str(), 0);
	nReturn=eqCall.set(&eqAddr,&dataIn,&dataOut);

	if(nReturn){
		qCritical()<<"Unable to connect to DOOCS server";
		emit ConnectionFailedToGuiSignal("Unable to connect to DOOCS server");
		return;
	}

	nType=dataOut.type();

	switch(nType){
	case DATA_INT:{
		nPort = dataOut.get_int();
	}break;
	case DATA_A_USTR:{
		float f1, f2;
		time_t tm;
		char         *sp;
		dataOut.get_ustr (&nPort, &f1, &f2, &tm, &sp, 0);
	}break;
	default:
		qCritical()<<"Wrong protocol";
		emit ConnectionFailedToGuiSignal("Wrong protocol");
		return ;
	}  // switch(nType){

	nReturn=eqCall.get_option(&eqAddr,&dataIn,&dataOut,EQ_HOSTNAME);
	if(nReturn<0){
		qCritical()<<"Unable to get hostname";
		emit ConnectionFailedToGuiSignal("Unable to get hostname");
		return;
	}

	hostName = dataOut.get_string();

	if(!PrepareDaqEntryBasedOnType2(0,branchInfo.type,nullptr,&branchInfo,&singleItemSize,&secondHeaderLength,NEWNULLPTR2,NEWNULLPTR2)){
		qCritical()<<"No information on this type";
		emit ConnectionFailedToGuiSignal("No information on this type");
		return;
	}

	m_statuses.bits.zmqThreadShouldRun = 1;
	m_zmqThread = ::std::thread([this](const ::std::string& a_zmqEndpoint,uint32_t a_secondHeaderLength, DEC_OUT_PD(TypeAndCount) a_branchInfo, uint32_t a_singleItemSize){
		uint32_t expectedDataLength = a_singleItemSize * static_cast<uint32_t>(a_branchInfo.itemsCountPerEntry);
		dmsg_hdr_t aDcsHeader;
		QVector<QVariant> vectGraph(10);
		int nReturn;
		void* pSocket;
		void* pBufferForSecondHeader=nullptr;
		void* pBufferForData=nullptr;

		m_statuses.bits.zmqThreadRunning = 1;

		vectGraph[INDEX_FOR_TYPE]=a_branchInfo.type;
		vectGraph[INDEX_FOR_SINGLE_ITEM_SIZE]=static_cast<int32_t>(a_singleItemSize);
		vectGraph[INDEX_FOR_ITEMS_COUNT]=a_branchInfo.itemsCountPerEntry;

		if(!m_pContext){
			qCritical()<<"No zmq context";
			goto returnPoint;
		}
		pSocket = zmq_socket(m_pContext,ZMQ_SUB);
		if(!pSocket){
			qCritical()<<"Unable to create zmq socket";
			goto returnPoint;
		}
		nReturn = zmq_setsockopt (pSocket, ZMQ_SUBSCRIBE,nullptr, 0);
		if(nReturn){
			qCritical()<<"Unable to set zmq socket option";
			goto returnPoint ;
		}
		nReturn = zmq_connect (pSocket, a_zmqEndpoint.c_str());
		if(nReturn){
			qCritical()<<"Unable to connect to endpoint "<<a_zmqEndpoint.c_str();
			goto returnPoint;
		}
		//if(!PrepareDaqEntryBasedOnType2())
		while(m_statuses.bits.zmqThreadShouldRun){

			if(pBufferForSecondHeader){free(pBufferForSecondHeader);pBufferForSecondHeader=nullptr;}
			if(pBufferForData){free(pBufferForData);pBufferForData=nullptr;}

			if(a_secondHeaderLength>0){
				pBufferForSecondHeader = malloc(a_secondHeaderLength);
				if(!pBufferForSecondHeader){throw ::std::bad_alloc();}
			}
			pBufferForData = malloc(expectedDataLength);
			if(!pBufferForData){throw ::std::bad_alloc();}

			nReturn=zmq_recv(pSocket,&aDcsHeader,sizeof(dmsg_hdr_t),0);
			if(nReturn<0){
				goto returnPoint;
			}
			if(nReturn<4){
				continue;
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
					continue;
				}
			}break;
			default:
				continue;
			}

			if(a_secondHeaderLength>0){
				nReturn=zmq_recv(pSocket,pBufferForSecondHeader,a_secondHeaderLength,0);
				if(nReturn!=static_cast<int>(a_secondHeaderLength)){
					continue;
				}
				vectGraph[INDEX_FOR_SECOND_HEADER]=PointerToQvariant(pBufferForSecondHeader);
			}

			nReturn=zmq_recv(pSocket,pBufferForData,expectedDataLength,0);
			if(nReturn!=static_cast<int>(expectedDataLength)){
				continue;
			}
			vectGraph[INDEX_FOR_DATA]=PointerToQvariant(pBufferForData);
			pBufferForSecondHeader=nullptr;
			pBufferForData=nullptr;
			emit ZmqReadDoneSignal(vectGraph);
		}
		returnPoint:
		if(pBufferForSecondHeader){free(pBufferForSecondHeader);pBufferForSecondHeader=nullptr;}
		if(pBufferForData){free(pBufferForData);pBufferForData=nullptr;}
		m_statuses.bits.zmqThreadRunning = 0;
	},::std::string("tcp://") + hostName + ":" + ::std::to_string(nPort),secondHeaderLength,branchInfo,singleItemSize);

	emit ConnectionDoneToGuiSignal();
}


void ui::qt::Application::GetDoocsNamesWorkerThread(const QVector<QVariant>& a_nmGetter)
{
	int nReturn;
	QVector<QVariant> vectReturn ( a_nmGetter );
	QString serverAddress = a_nmGetter[1].toString();
	EqData* pData = nullptr;
	EqAdr eqAddr;
	EqCall eqCall;
	::std::string adrString = serverAddress.toStdString();

	//size_t nFields = static_cast<size_t>( ::std::count(adrString.begin(),adrString.end(),'/') );
	//switch(nFields){
	//case 0:
	//	adrString += "*";
	//	break;
	//case 1:
	//	adrString += "/*/*";
	//	break;
	//case 2:
	//	adrString += "/*";
	//	break;
	//default:{
	//	QString retString = QString("Wrong address: ") + serverAddress;
	//	qWarning()<<retString;
	//	emit GetNamesFailedToGuiSignal(retString);
	//	return;
	//}  // default:{
	//}

	pData = new EqData;
	eqAddr.adr(adrString);
	nReturn=eqCall.names(&eqAddr,pData);
	if(nReturn || (pData->type()!=DATA_A_USTR)){
		delete pData;
		qCritical()<<"Unable to get names";
		emit GetNamesFailedToGuiSignal("Unable to get names");
		return;
	}

	vectReturn.push_back(PointerToQvariant(pData));
	emit GetNamesDoneToGuiSignal(vectReturn);
}


void ui::qt::Application::ConnectToServerAnyThread(const QString& a_serverAddress)
{
	if(m_statuses.bits.worker01Started){
		emit ConnectToServerToWorkerSignal(a_serverAddress);
	}
	else{
		::std::lock_guard< ::std::mutex > aGuard(m_mutexForQueue);
		m_earlyJobs.push({ EarlyJob::Type::Connect,a_serverAddress,nullptr});
	}
}


void ui::qt::Application::GetDoocsNamesAnyThread(const QString& a_serverAddress,QWidget* a_pCaller)
{
	if(m_statuses.bits.worker01Started){
		emit GetDoocsNamesToWorkerSignal({PointerToQvariant(a_pCaller),a_serverAddress});
	}
	else{
		::std::lock_guard< ::std::mutex > aGuard(m_mutexForQueue);
		m_earlyJobs.push({ EarlyJob::Type::Names,a_serverAddress,a_pCaller});
	}
}


/*/////////////////////////////////////////////////////////////////////////////////////*/
ui::qt::USettings::USettings()
{
	QCoreApplication::setOrganizationName("DESY");
	QCoreApplication::setApplicationName("doocs_zmq_reader");
	QSettings::setDefaultFormat(QSettings::IniFormat); // use ini files on all platforms
	m_pSettings = new QSettings();

	QSettings::Format fmt = m_pSettings->format();
	QSettings::Scope scp = m_pSettings->scope();
	QString filePath = m_pSettings->fileName();
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
				thisApp()->GetDoocsNamesWorkerThread({PointerToQvariant(aJob.pCaller),aJob.arg.toString()});
				break;
			case EarlyJob::Type::Connect:
				thisApp()->ConnectToServerWorkerThread(aJob.arg.toString());
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
