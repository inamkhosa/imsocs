
#include "ratingfunc.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_appcallbackreg.h"

extern int g_nDiamBaseTraceID ;

AMPS_MODULE_HANDLE	ratingfunc_ModuleInit(	AMPS_HANDLE hAMPS_HANDLE_i,
											void* 		pvUserData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	printf("\r\n\t\tLoading Rating Function Module\r\n");

	HDIAMETERSTACKCONTEXT hDiamStackContext = NULL;
	HRATINGFUNCCONTEXT	hRatingFuncContext = NULL;

	hDiamStackContext = (HDIAMETERSTACKCONTEXT)pvUserData_i;
	DMBase_Malloc(hDiamStackContext, (void**)&hRatingFuncContext, sizeof(t_RatingFuncContext));
	hRatingFuncContext->hDiamStackContext = hDiamStackContext;
	if(OCS_Retval_Success != RatingFunc_Init(hRatingFuncContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed To Load Rating Function Module");

	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");

	return (AMPS_MODULE_HANDLE)0xABCDEF;
}

void 				ratingfunc_ModuleCleanup(	AMPS_HANDLE 		hAMPS_HANDLE_i,
												AMPS_MODULE_HANDLE 	hAMPS_MODULE_HANDLE_i)
{

}


t_OCSAPIRetVal RatingFunc_Init(HRATINGFUNCCONTEXT	hRatingFuncContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT	hDiamStackContext = NULL;
	hDiamStackContext = hRatingFuncContext_io->hDiamStackContext;

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext,OCS_RATING_FUNCTION_REQUEST_EVENT,
															RatingFunc_ProcessRequest, (void*)hRatingFuncContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_REQUEST_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext,OCS_RATING_FUNCTION_PRICE_REQUEST_EVENT,
															RatingFunc_ProcessPriceRequest, (void*)hRatingFuncContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_PRICE_REQUEST_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext,OCS_RATING_FUNCTION_TARIFF_REQUEST_EVENT,
															RatingFunc_ProcessTarrifRequest, (void*)hRatingFuncContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_TARIFF_REQUEST_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext,OCS_RATING_FUNCTION_SERVICE_USAGE_REQUEST_EVENT,
															RatingFunc_ProcessServiceUsageRequest, (void*)hRatingFuncContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_SERVICE_USAGE_REQUEST_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,OCS_DB_PRICE_RESPONSE_EVENT,
														RatingFunc_ProcessDBResponse, (void*)hRatingFuncContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_DB_PRICE_RESPONSE_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,OCS_DB_TARIFF_RESPONSE_EVENT,
														RatingFunc_ProcessDBResponse, (void*)hRatingFuncContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_DB_TARIFF_RESPONSE_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}



void	RatingFunc_ProcessRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
									void* r_pvEventHandle, void* r_pvEventPayload)
{
	HRATINGFUNCCONTEXT hAppContext = (HRATINGFUNCCONTEXT)r_hAMPS_APP_HANDLE;

//	unsigned int unEventDataSize = AMPS_EvtSysGetEventSize(r_hAMPS_HANDLE, r_pvEventHandle);
//	printf("RatingFunc_ProcessRequest %d\r\n", unEventDataSize);
	t_OCSCCR *poCCR = (t_OCSCCR*)r_pvEventPayload;
	printf("RatingFunc_ProcessRequest\r\n");
//	printf("Session-ID : %s\r\n", poCCR->oSessionId.ucString);
	printf("Sub-Session-Id : %llu\r\n", poCCR->ullCCSubSessionId);
//	printf("Request-Type : %d\r\n", poCCR->unCCRequestType);
	printf("User-Name : %s\r\n", poCCR->oUserName.ucString);


	unsigned int a = 1000;
	DMBASE_EvtSysSendIntEvt(hAppContext->hDiamStackContext, OCS_RATING_FUNCTION_RESPONSE_EVENT, (void*)&a, sizeof(unsigned int));

}


void	RatingFunc_ProcessPriceRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
									void* r_pvEventHandle, void* r_pvEventPayload)
{
	HRATINGFUNCCONTEXT hAppContext = (HRATINGFUNCCONTEXT)r_hAMPS_APP_HANDLE;

//	printf("RatingFunc_ProcessPriceRequest\r\n");

   /* AMPS_EvtSysSendIntEvt(hAppContext_i->hDiamStackContext->hFrameworkHandle,
                            CX_UAR_DATABASE_REQUEST_EVENT,
                            (void*)&poUAR_i->oUARDBRequest,
                            sizeof(t_CxUARDBRequest));*/

    t_OCSPriceRequest* poPriceRequest = (t_OCSPriceRequest*)r_pvEventPayload;

	DMBASE_EvtSysSendIntEvt(hAppContext->hDiamStackContext, OCS_DB_PRICE_REQUEST_EVENT,
			poPriceRequest, sizeof(t_OCSPriceRequest));

}

void	RatingFunc_ProcessTarrifRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
									void* r_pvEventHandle, void* r_pvEventPayload)
{

	HRATINGFUNCCONTEXT hAppContext = (HRATINGFUNCCONTEXT)r_hAMPS_APP_HANDLE;

//	printf("RatingFunc_ProcessTarrifRequest\r\n");

    t_OCSTariffRequest* poTarrifRequest = (t_OCSTariffRequest*)r_pvEventPayload;

	DMBASE_EvtSysSendIntEvt(hAppContext->hDiamStackContext, OCS_DB_TARIFF_REQUEST_EVENT,
			(void*)poTarrifRequest, sizeof(t_OCSTariffRequest));

}

void	RatingFunc_ProcessServiceUsageRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
												void* r_pvEventHandle, void* r_pvEventPayload)
{

}


void	RatingFunc_ProcessDBResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload)
{

	HRATINGFUNCCONTEXT hAppContext = (HRATINGFUNCCONTEXT)r_hAMPS_APP_HANDLE;
//	printf("RatingFunc_ProcessDBResponse\r\n");

	int unEventType = 0;
    DMBase_EvtSysGetEventType(hAppContext->hDiamStackContext, r_pvEventHandle, &unEventType);

    switch(unEventType)
    {
		case OCS_DB_PRICE_RESPONSE_EVENT:
			RatingFunc_ProcessDBPriceResponse(hAppContext, (t_OCSPriceResponse*)r_pvEventPayload);
			break;
		case OCS_DB_TARIFF_RESPONSE_EVENT:
			RatingFunc_ProcessDBTariffResponse(hAppContext, (t_OCSTariffResponse*)r_pvEventPayload);
			break;
    }
}


t_OCSAPIRetVal RatingFunc_ProcessDBPriceResponse(HRATINGFUNCCONTEXT hAppContext_io, t_OCSPriceResponse* poPriceResponse_i)
{


	DMBASE_EvtSysSendIntEvt(hAppContext_io->hDiamStackContext, OCS_RATING_FUNCTION_PRICE_RESPONE_EVENT,
			(void*)poPriceResponse_i, sizeof(t_OCSPriceResponse));
	return OCS_Retval_Success;
}


t_OCSAPIRetVal RatingFunc_ProcessDBTariffResponse(HRATINGFUNCCONTEXT hAppContext_io, t_OCSTariffResponse* poTariffResponse_i)
{

	DMBASE_EvtSysSendIntEvt(hAppContext_io->hDiamStackContext, OCS_RATING_FUNCTION_TARIFF_RESPONE_EVENT,
			(void*)poTariffResponse_i, sizeof(t_OCSTariffResponse));
	return OCS_Retval_Success;
}

