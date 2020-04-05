//
// file:			entry_doocsclient_c_interface.cpp
// created on:		2020 Apr 03
// created by:		D. Kalantaryan (davit.kalantaryan@desy.de)
//

#include <doocsclient_c_interface.h>


extern "C"{

DCS_CLNT_C_EXPORT EqData* GetNewEqData(void)
{
	return new EqData;
}


DCS_CLNT_C_EXPORT void DeleteEqData(EqData* a_pData)
{
	delete a_pData;
}


DCS_CLNT_C_EXPORT void EqDataInit(EqData* a_pData)
{
	a_pData->init();
}


DCS_CLNT_C_EXPORT int EqDataType(const EqData* a_pData)
{
	return a_pData->type();
}


DCS_CLNT_C_EXPORT int EqDataLength(const EqData* a_pData)
{
	return a_pData->length();
}


DCS_CLNT_C_EXPORT int EqDataSet1 (EqData* a_pData,int a_i1, float a_f1, float a_f2, time_t a_tm, const std::string &a_str, int a_index)
{
	return a_pData->set(a_i1, a_f1, a_f2, a_tm, a_str, a_index);
}


DCS_CLNT_C_EXPORT int EqDataGet_int1 (const EqData* a_pData)
{
	return a_pData->get_int();
}


DCS_CLNT_C_EXPORT int EqDataGet_ustr (const EqData* a_pData,int *a_i1, float *a_f1, float *a_f2, time_t *a_tm, char **a_com, int a_index)
{
	return a_pData->get_ustr(a_i1,a_f1,a_f2, a_tm, a_com, a_index);
}


DCS_CLNT_C_EXPORT char* EqDataGet_string1 (const EqData* a_pData, char* a_pBuffer, size_t a_unBuffLen)
{
	return a_pData->get_string(a_pBuffer,a_unBuffLen);
}


DCS_CLNT_C_EXPORT USTR* EqDataGet_ustr2 (const EqData* a_pData, int a_index)
{
	return a_pData->get_ustr(a_index);
}

/*///////////////////////////////////////////////////////////////////////////////////*/

DCS_CLNT_C_EXPORT EqAdr* GetNewEqAdr(void)
{
	return new EqAdr;
}


DCS_CLNT_C_EXPORT void DeleteEqAdr(EqAdr* a_pEqAdr)
{
	delete a_pEqAdr;
}


DCS_CLNT_C_EXPORT void EqAdrAdr(EqAdr* a_pEqAdr, const ::std::string& a_adr)
{
	a_pEqAdr->adr(a_adr);
}


DCS_CLNT_C_EXPORT const char* EqAdrProperty(const EqAdr* a_pEqAdr)
{
	return a_pEqAdr->property();
}


DCS_CLNT_C_EXPORT void EqAdrSet_property(EqAdr* a_pEqAdr, const ::std::string& a_prop)
{
	a_pEqAdr->set_property(a_prop);
}


/*///////////////////////////////////////////////////////////////////////////////////*/

DCS_CLNT_C_EXPORT EqCall* GetNewEqCall(void)
{
	return new EqCall;
}


DCS_CLNT_C_EXPORT void DeleteEqCall(EqCall* a_pEqCall)
{
	delete a_pEqCall;
}


DCS_CLNT_C_EXPORT int EqCallGet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out)
{
	return a_pEqCall->get(a_adr,a_data_in,a_data_out);
}


DCS_CLNT_C_EXPORT int EqCallSet(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out)
{
	return a_pEqCall->set(a_adr,a_data_in,a_data_out);
}


DCS_CLNT_C_EXPORT int EqCallGet_option(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data_in, EqData *a_data_out, EqOption a_option)
{
	return a_pEqCall->get_option(a_adr, a_data_in, a_data_out, a_option);
}


DCS_CLNT_C_EXPORT int EqCallNames(EqCall* a_pEqCall,EqAdr *a_adr, EqData *a_data)
{
	return a_pEqCall->names(a_adr, a_data);
}


}  // extern "C"{
