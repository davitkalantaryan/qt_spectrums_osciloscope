//
// file:			doocsclient_c_interface.h
// created on:		2020 Apr 03
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef DOOCS_CLIENT_C_INTERFACE_H
#define DOOCS_CLIENT_C_INTERFACE_H

#include <eq_client.h>

#ifdef _WIN32
#define DCS_CLNT_C_EXPORT	__declspec(dllexport)
#define DCS_CLNT_C_IMPORT	__declspec(dllimport)
#else
#define DCS_CLNT_C_EXPORT
#define DCS_CLNT_C_IMPORT
#endif


#ifdef __cplusplus
extern "C"{
#endif

//DCS_CLNT_DYN EqData* GetNewEqData(void);
//DCS_CLNT_DYN void DeleteEqData(EqData* a_pData);
//DCS_CLNT_DYN void EqDataInit(EqData* a_pData);
//DCS_CLNT_C_EXPORT int EqDataType(const EqData* a_pData);
//DCS_CLNT_C_EXPORT int EqDataLength(const EqData* a_pData);
//DCS_CLNT_C_EXPORT int EqDataSet1 (EqData* a_pData,int a_i1, float a_f1, float a_f2, time_t a_tm, const std::string &a_str, int a_index);
//DCS_CLNT_C_EXPORT int EqDataGet_int1 (const EqData* a_pData);
//DCS_CLNT_C_EXPORT int EqDataGet_ustr (const EqData* a_pData,int *a_i1, float *a_f1, float *a_f2, time_t *a_tm, char **a_com, int a_index);
//DCS_CLNT_C_EXPORT char* EqDataGet_string1 (const EqData* a_pData, char* a_pBuffer, size_t a_unBuffLen);
//DCS_CLNT_C_EXPORT USTR* EqDataGet_ustr2 (const EqData* a_pData, int a_index);
//
//DCS_CLNT_DYN EqAdr* GetNewEqAdr(void);
//DCS_CLNT_DYN void DeleteEqAdr(EqAdr* a_pEqAdr);
//DCS_CLNT_C_EXPORT void EqAdrAdr(EqAdr* a_pEqAdr, const ::std::string& a_adr);
//DCS_CLNT_C_EXPORT const char* EqAdrProperty(const EqAdr* a_pEqAdr);
//DCS_CLNT_C_EXPORT void EqAdrSet_property(EqAdr* a_pEqAdr, const ::std::string& a_prop);
//
//DCS_CLNT_DYN EqCall* GetNewEqCall(void);
//DCS_CLNT_DYN void DeleteEqCall(EqCall* a_pEqCall);
//DCS_CLNT_C_EXPORT int EqCallGet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out);
//DCS_CLNT_C_EXPORT int EqCallSet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out);
//DCS_CLNT_C_EXPORT int EqCallGet_option(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out, EqOption a_option);
//DCS_CLNT_C_EXPORT int EqCallNames(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data);
//

#ifdef __cplusplus
}
#endif


#endif  // #ifndef DOOCS_CLIENT_C_INTERFACE_H
