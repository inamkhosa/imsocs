

#ifndef OCSDB_H
#define OCSDB_H

#include "ocs_defines.h"
#include "AMPS_Defines.h"
#include "dmbase_data_types.h"
#include "dmbase_ampswrappers.h"
#include "dappcommon_srvrconfload.h"
#include "dappcommon_dbqueryload.h"

#define DB_OCS_DB_NUMBER_OF_THREADS		10
#define DB_OCS_QUERY_MAP_SIZE			64

#ifdef __cplusplus
extern "C" {
#endif


typedef struct __OCSDBContext
{
	HDIAMETERSTACKCONTEXT 	hDiamStackContext;
	AMPS_IOAGENTS_HANDLE	hIOAgentsHandle;
	t_ocs_db_config_params*	poOCS_DBConfParams;
	t_Dappcommon_DBQueryMap	oDBQueryMapForOCS[DB_OCS_QUERY_MAP_SIZE];
}t_OCSDBContext;

typedef t_OCSDBContext* HOCSDBCONTEXT;


typedef struct __OCSDBThreadContext
{
	HOCSDBCONTEXT				hOCSDBMainContext;
	DMBASE_DBE_HANDLE			hDBEngineHandle;
	AMPS_UIOAGENT_HANDLE		hUnitIOAgent;
} t_OCSDBThreadContext;

typedef t_OCSDBThreadContext*		HOCSDBTHREADCONTEXT;

AMPS_MODULE_HANDLE	ocsdb_ModuleInit(AMPS_HANDLE hAMPS_HANDLE, void* pvUserData_i);

void ocsdb_ModuleCleanup(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_MODULE_HANDLE r_hAMPS_MODULE_HANDLE);

void* OCSDB_ThreadInit(AMPS_HANDLE r_hAMPSHandle,
						AMPS_UIOAGENT_HANDLE r_hUIOAgentsHandle, void* hOCSDBContext_io);

void OCSDB_ThreadCleanup(void* hOCSDBThreadCtxt_io);

t_OCSAPIRetVal OCSDB_Init(HOCSDBCONTEXT hOCSDBContext_io);

void  OCSDB_RequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
								void* r_pvEventHandle, void* r_pvEventPayload);

t_OCSAPIRetVal OCSDB_ProcessPriceRequest(HOCSDBTHREADCONTEXT hOCSDBThreadCtxt_io,
										t_OCSPriceRequest* poPriceRequest_i);

t_OCSAPIRetVal OCSDB_ProcessTariffRequest(HOCSDBTHREADCONTEXT hOCSDBThreadCtxt_io,
											t_OCSTariffRequest* poTariffRequest_i);

void OCSDB_DumpPriceRequest(t_OCSPriceRequest* poPriceRequest_i);
void OCSDB_DumpTariffRequest(t_OCSTariffRequest* poTariffRequest_i);

#ifdef __cplusplus
}
#endif

#endif //OCSDB_H

