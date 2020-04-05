//
// file:			doocsclient_dynamic.h
// created on:		2020 Apr 03
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef DOOCS_CLIENT_DYNAMIC_H
#define DOOCS_CLIENT_DYNAMIC_H

#include <doocsclient_c_interface.h>

#ifdef DCS_CLNT_DYN_CREATING_LIB
#define DCS_CLNT_DYN_EXPORT	DCS_CLNT_C_EXPORT
#elif defined(DCS_CLNT_DYN_USING_SOURCES)
#define DCS_CLNT_DYN_EXPORT
#else
#define DCS_CLNT_DYN_EXPORT	DCS_CLNT_C_IMPORT
#endif


#ifdef __cplusplus
//extern "C"{
#endif

DCS_CLNT_DYN_EXPORT int		DynInitDoocsCClient(const char* ensHostVar);
DCS_CLNT_DYN_EXPORT void	DynCleanDoocsCClient(void);

DCS_CLNT_DYN_EXPORT EqData* DynGetNewEqData(void);
DCS_CLNT_DYN_EXPORT void	DynDeleteEqData(EqData* a_pData);
DCS_CLNT_DYN_EXPORT void	DynEqDataInit(EqData* a_pData);
DCS_CLNT_DYN_EXPORT int		DynEqDataType(const EqData* a_pData);
DCS_CLNT_DYN_EXPORT int		DynEqDataLength(const EqData* a_pData);
DCS_CLNT_DYN_EXPORT int		DynEqDataSet1 (EqData* a_pData,int a_i1, float a_f1, float a_f2, time_t a_tm, const std::string &a_str, int a_index);
DCS_CLNT_DYN_EXPORT int		DynEqDataGet_int1 (const EqData* a_pData);
DCS_CLNT_DYN_EXPORT int		DynEqDataGet_ustr (const EqData* a_pData,int *a_i1, float *a_f1, float *a_f2, time_t *a_tm, char **a_com, int a_index);
DCS_CLNT_DYN_EXPORT char*	DynEqDataGet_string1 (const EqData* a_pData, char* a_pBuffer, size_t a_unBuffLen);
DCS_CLNT_DYN_EXPORT ::std::string DynEqDataGet_string2 (const EqData* a_pData);
DCS_CLNT_DYN_EXPORT USTR*	DynEqDataGet_ustr2 (const EqData* a_pData, int a_index);

DCS_CLNT_DYN_EXPORT EqAdr*	DynGetNewEqAdr(void);
DCS_CLNT_DYN_EXPORT void	DynDeleteEqAdr(EqAdr* a_pEqAdr);
DCS_CLNT_DYN_EXPORT void	DynEqAdrAdr(EqAdr* a_pEqAdr, const ::std::string& a_adr);
DCS_CLNT_DYN_EXPORT const char*		DynEqAdrProperty(const EqAdr* a_pEqAdr);
DCS_CLNT_DYN_EXPORT void	DynEqAdrSet_property(EqAdr* a_pEqAdr, const ::std::string& a_prop);

DCS_CLNT_DYN_EXPORT EqCall* DynGetNewEqCall(void);
DCS_CLNT_DYN_EXPORT void	DynDeleteEqCall(EqCall* a_pEqCall);
DCS_CLNT_DYN_EXPORT int		DynEqCallGet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out);
DCS_CLNT_DYN_EXPORT int		DynEqCallSet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out);
DCS_CLNT_DYN_EXPORT int		DynEqCallGet_option(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out, EqOption a_option);
DCS_CLNT_DYN_EXPORT int		DynEqCallNames(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data);

#ifdef __cplusplus
//}
#endif


#endif  // #ifndef DOOCS_CLIENT_DYNAMIC_H
