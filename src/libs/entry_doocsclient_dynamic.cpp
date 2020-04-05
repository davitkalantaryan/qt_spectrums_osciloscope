//
// file:			entry_doocsclient_dynamic.cpp
// created on:		2020 Apr 03
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <doocsclient_dynamic.h>
#include <string.h>

#ifdef _WIN32
#else
#include <dlfcn.h>
typedef void* HMODULE;
#define correct_cast	reinterpret_cast
#define C_LIBRARY_NAME	"libdoocsclient_c_interface.so"
#endif

typedef EqData* (*TypeGetNewEqData)(void);
typedef void (*TypeDeleteEqData)(EqData* a_pData);
typedef void (*TypeEqDataInit)(EqData* a_pData);
typedef int (*TypeEqDataType)(const EqData* a_pData);
typedef int (*TypeEqDataLength)(const EqData* a_pData);
typedef int (*TypeEqDataSet1) (EqData* a_pData,int a_i1, float a_f1, float a_f2, time_t a_tm, const std::string &a_str, int a_index);
typedef int (*TypeEqDataGet_int1) (const EqData* a_pData);
typedef int (*TypeEqDataGet_ustr) (const EqData* a_pData,int *a_i1, float *a_f1, float *a_f2, time_t *a_tm, char **a_com, int a_index);
typedef char* (*TypeEqDataGet_string1) (const EqData* a_pData, char* a_pBuffer, size_t a_unBuffLen);
typedef USTR* (*TypeEqDataGet_ustr2) (const EqData* a_pData, int a_index);

typedef EqAdr* (*TypeGetNewEqAdr)(void);
typedef void (*TypeDeleteEqAdr)(EqAdr* a_pEqAdr);
typedef void (*TypeEqAdrAdr)(EqAdr* a_pEqAdr, const ::std::string& a_adr);
typedef const char* (*TypeEqAdrProperty)(const EqAdr* a_pEqAdr);
typedef void (*TypeEqAdrSet_property)(EqAdr* a_pEqAdr, const ::std::string& a_prop);

typedef EqCall* (*TypeGetNewEqCall)(void);
typedef void (*TypeDeleteEqCall)(EqCall* a_pEqCall);
typedef int (*TypeEqCallGet)(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out);
typedef int (*TypeEqCallSet)(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out);
typedef int (*TypeEqCallGet_option)(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out, EqOption a_option);
typedef int (*TypeEqCallNames)(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data);


static HMODULE				s_libModule = correct_cast<HMODULE>(0);

static TypeGetNewEqData		s_getNewEqData;
static TypeDeleteEqData		s_deleteEqData;
static TypeEqDataInit		s_eqDataInit;
static TypeEqDataType		s_eqDataType;
static TypeEqDataLength		s_eqDataLength;
static TypeEqDataSet1		s_eqDataSet1;
static TypeEqDataGet_int1	s_eqDataGet_int1;
static TypeEqDataGet_ustr	s_eqDataGet_ustr;
static TypeEqDataGet_string1	s_eqDataGet_string1;
static TypeEqDataGet_ustr2	s_eqDataGet_ustr2;

static TypeGetNewEqAdr		s_getNewEqAdr;
static TypeDeleteEqAdr		s_deleteEqAdr;
static TypeEqAdrAdr			s_eqAdrAdr;
static TypeEqAdrProperty	s_eqAdrProperty;
static TypeEqAdrSet_property	s_eqAdrSet_property;

static TypeGetNewEqCall		s_getNewEqCall;
static TypeDeleteEqCall		s_deleteEqCall;
static TypeEqCallGet		s_eqCallGet;
static TypeEqCallSet		s_eqCallSet;
static TypeEqCallGet_option	s_eqCallGet_option;
static TypeEqCallNames		s_eqCallNames;


//extern "C"{

DCS_CLNT_DYN_EXPORT int DynInitDoocsCClient(const char* a_ensHostVar)
{
	if(s_libModule){return 0;}

	if(a_ensHostVar){
		size_t unStrLen = strlen(a_ensHostVar);
		if(unStrLen>1){
			setenv("ENSHOST",a_ensHostVar,1);
		}
	}  // if(a_ensHostVar){

	s_libModule = dlopen(C_LIBRARY_NAME, RTLD_LAZY);
	if(!s_libModule){return-1;}

	s_getNewEqData = reinterpret_cast<TypeGetNewEqData>(dlsym(s_libModule,"GetNewEqData"));
	s_deleteEqData = reinterpret_cast<TypeDeleteEqData>(dlsym(s_libModule,"DeleteEqData"));
	s_eqDataInit = reinterpret_cast<TypeEqDataInit>(dlsym(s_libModule,"EqDataInit"));
	s_eqDataType = reinterpret_cast<TypeEqDataType>(dlsym(s_libModule,"EqDataType"));
	s_eqDataLength = reinterpret_cast<TypeEqDataLength>(dlsym(s_libModule,"EqDataLength"));
	s_eqDataSet1 = reinterpret_cast<TypeEqDataSet1>(dlsym(s_libModule,"EqDataSet1"));
	s_eqDataGet_int1 = reinterpret_cast<TypeEqDataGet_int1>(dlsym(s_libModule,"EqDataGet_int1"));
	s_eqDataGet_ustr = reinterpret_cast<TypeEqDataGet_ustr>(dlsym(s_libModule,"EqDataGet_ustr"));
	s_eqDataGet_string1 = reinterpret_cast<TypeEqDataGet_string1>(dlsym(s_libModule,"EqDataGet_string1"));
	s_eqDataGet_ustr2 = reinterpret_cast<TypeEqDataGet_ustr2>(dlsym(s_libModule,"EqDataGet_ustr2"));

	s_getNewEqAdr = reinterpret_cast<TypeGetNewEqAdr>(dlsym(s_libModule,"GetNewEqAdr"));
	s_deleteEqAdr = reinterpret_cast<TypeDeleteEqAdr>(dlsym(s_libModule,"DeleteEqAdr"));
	s_eqAdrAdr = reinterpret_cast<TypeEqAdrAdr>(dlsym(s_libModule,"EqAdrAdr"));
	s_eqAdrProperty = reinterpret_cast<TypeEqAdrProperty>(dlsym(s_libModule,"EqAdrProperty"));
	s_eqAdrSet_property = reinterpret_cast<TypeEqAdrSet_property>(dlsym(s_libModule,"EqAdrSet_property"));

	s_getNewEqCall = reinterpret_cast<TypeGetNewEqCall>(dlsym(s_libModule,"GetNewEqCall"));
	s_deleteEqCall = reinterpret_cast<TypeDeleteEqCall>(dlsym(s_libModule,"DeleteEqCall"));
	s_eqCallGet = reinterpret_cast<TypeEqCallGet>(dlsym(s_libModule,"EqCallGet"));
	s_eqCallSet = reinterpret_cast<TypeEqCallSet>(dlsym(s_libModule,"EqCallSet"));
	s_eqCallGet_option = reinterpret_cast<TypeEqCallGet_option>(dlsym(s_libModule,"EqCallGet_option"));
	s_eqCallNames = reinterpret_cast<TypeEqCallNames>(dlsym(s_libModule,"EqCallNames"));

	return 0;
}


DCS_CLNT_DYN_EXPORT void DynCleanDoocsCClient(void)
{
	if(s_libModule){
		dlclose(s_libModule);
		s_libModule = correct_cast<HMODULE>(0);
	}
}


/*///////////////////////////////////////////////////////////////////////////////////*/

DCS_CLNT_DYN_EXPORT EqData* DynGetNewEqData(void)
{
	return (*s_getNewEqData)();
}


DCS_CLNT_DYN_EXPORT void DynDeleteEqData(EqData* a_pData)
{
	(*s_deleteEqData)(a_pData);
}


DCS_CLNT_DYN_EXPORT void DynEqDataInit(EqData* a_pData)
{
	(*s_eqDataInit)(a_pData);
}


DCS_CLNT_DYN_EXPORT int DynEqDataType(const EqData* a_pData)
{
	return (*s_eqDataType)(a_pData);
}


DCS_CLNT_DYN_EXPORT int DynEqDataLength(const EqData* a_pData)
{
	return (*s_eqDataLength)(a_pData);
}


DCS_CLNT_DYN_EXPORT int DynEqDataSet1 (EqData* a_pData,int a_i1, float a_f1, float a_f2, time_t a_tm, const std::string &a_str, int a_index)
{
	return (*s_eqDataSet1)(a_pData,a_i1, a_f1, a_f2, a_tm, a_str, a_index);
}


DCS_CLNT_DYN_EXPORT int DynEqDataGet_int1 (const EqData* a_pData)
{
	return (*s_eqDataGet_int1)(a_pData);
}


DCS_CLNT_DYN_EXPORT int DynEqDataGet_ustr (const EqData* a_pData,int *a_i1, float *a_f1, float *a_f2, time_t *a_tm, char **a_com, int a_index)
{
	return (*s_eqDataGet_ustr)(a_pData,a_i1,a_f1,a_f2, a_tm, a_com, a_index);
}


DCS_CLNT_DYN_EXPORT char* DynEqDataGet_string1 (const EqData* a_pData, char* a_pBuffer, size_t a_unBuffLen)
{
	return (*s_eqDataGet_string1)(a_pData,a_pBuffer,a_unBuffLen);
}


DCS_CLNT_DYN_EXPORT ::std::string DynEqDataGet_string2 (const EqData* a_pData)
{
	char vcBuffer[4096];
	vcBuffer[4095]=0;
	return (*s_eqDataGet_string1)(a_pData,vcBuffer,4095);
}


DCS_CLNT_DYN_EXPORT USTR* DynEqDataGet_ustr2 (const EqData* a_pData, int a_index)
{
	return (*s_eqDataGet_ustr2)(a_pData,a_index);
}

/*///////////////////////////////////////////////////////////////////////////////////*/

DCS_CLNT_DYN_EXPORT EqAdr* DynGetNewEqAdr(void)
{
	return (*s_getNewEqAdr)();
}


DCS_CLNT_DYN_EXPORT void DynDeleteEqAdr(EqAdr* a_pEqAdr)
{
	(*s_deleteEqAdr)(a_pEqAdr);
}


DCS_CLNT_DYN_EXPORT void DynEqAdrAdr(EqAdr* a_pEqAdr, const ::std::string& a_adr)
{
	(*s_eqAdrAdr)(a_pEqAdr,a_adr);
}


DCS_CLNT_DYN_EXPORT const char* DynEqAdrProperty(const EqAdr* a_pEqAdr)
{
	return (*s_eqAdrProperty)(a_pEqAdr);
}


DCS_CLNT_DYN_EXPORT void DynEqAdrSet_property(EqAdr* a_pEqAdr, const ::std::string& a_prop)
{
	(*s_eqAdrSet_property)(a_pEqAdr,a_prop);
}


/*///////////////////////////////////////////////////////////////////////////////////*/

DCS_CLNT_DYN_EXPORT EqCall* DynGetNewEqCall(void)
{
	return (*s_getNewEqCall)();
}


DCS_CLNT_DYN_EXPORT void DynDeleteEqCall(EqCall* a_pEqCall)
{
	(*s_deleteEqCall)(a_pEqCall);
}


DCS_CLNT_DYN_EXPORT int DynEqCallGet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out)
{
	return (*s_eqCallGet)(a_pEqCall,a_adr,a_data_in,a_data_out);
}


DCS_CLNT_DYN_EXPORT int DynEqCallSet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out)
{
	return (*s_eqCallSet)(a_pEqCall,a_adr,a_data_in,a_data_out);
}


DCS_CLNT_DYN_EXPORT int DynEqCallGet_option(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out, EqOption a_option)
{
	return (*s_eqCallGet_option)(a_pEqCall,a_adr, a_data_in, a_data_out, a_option);
}


DCS_CLNT_DYN_EXPORT int DynEqCallNames(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data)
{
	return (*s_eqCallNames)(a_pEqCall,a_adr, a_data);
}


//}  // extern "C"{
