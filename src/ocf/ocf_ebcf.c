
#include "ocf_ebcf.h"
#include "ocf.h"
#include "ocf_session.h"
#include "ocf_statemachine.h"
#include "ocf_msgextractor.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_utility.h"

extern int g_nDiamBaseTraceID ;

void	OCF_ProcessPriceResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload)
{
	//printf("OCF_ProcessPriceResponse\r\n");
	HOCFCONTEXT hOCFContext = (HOCFCONTEXT)r_hAMPS_APP_HANDLE;
	t_OCSPriceResponse* poPriceResponse = (t_OCSPriceResponse*)r_pvEventPayload;
	HOCSCCSUBSESSIONOBJECT hSessionObject = NULL;

	if(OCS_Retval_Success != OCF_LookUpSubSessionObject(hOCFContext, &poPriceResponse->oSessionId,
													poPriceResponse->ullCCSubSessionId,
														(void**)&hSessionObject))
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Session Object not found\n");
		return;
	}
	t_OCSEvent oEventType = OCF_CheckEventType(hOCFContext, hSessionObject->poCCR->oCCRequestType, poPriceResponse->oResult);
	OCF_ExecStateMachine(hOCFContext, hSessionObject, oEventType , r_pvEventPayload);
}


t_OCSAPIRetVal OCF_EventBasedCharging(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HOCSCCSUBSESSIONOBJECT hSessionObject = NULL;

	if(NULL == hOCFContext_io || NULL == poCCR_i || NULL == hPeer_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Null Pointer");
		return OCS_Retval_Null_Pointer;
	}

	if(OCS_Retval_Success != OCF_CreateSubSessionObject(hOCFContext_io, &hSessionObject, &poCCR_i->oSessionId,
														poCCR_i->ullCCSubSessionId, hPeer_i, OCS_SESSION_STATE_IDLE,
														poCCR_i, OCS_Charging_SCUR, TRUE))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to create session object\r\n");
		return OCS_Retval_Failure;
	}

	if(OCS_Retval_Success != OCF_InsertSubSessionObject(hOCFContext_io, &poCCR_i->oSessionId, poCCR_i->ullCCSubSessionId, (void*)hSessionObject))
	{
		DMBase_Free(hOCFContext_io->hDiamStackContext, (void**)&hSessionObject);
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to insert session object\r\n");
		return OCS_Retval_Failure;
	}

	t_OCSPriceRequest oPriceRequest;
	DMBase_MemsetZero(&oPriceRequest, sizeof(t_OCSPriceRequest));
	OCF_PopulatePriceRequest(&oPriceRequest, poCCR_i, OCF_GetRequestType(poCCR_i));


	if(DMBase_Retval_Success != DMBASE_EvtSysSendIntEvt(hOCFContext_io->hDiamStackContext,
														OCS_RATING_FUNCTION_PRICE_REQUEST_EVENT,
														(void*)&oPriceRequest,
														sizeof(t_OCSPriceRequest)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to send event");
		return OCS_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSRequestSubType OCF_GetRequestType(t_OCSCCR* poCCR_i)
{
	static t_OCSRequestSubType oRequestType[] = {	OCS_Direct_Debiting,
													OCS_Refund_Account,
													OCS_Check_Balance,
													OCS_Price_Enquiry
												};


	if(OCS_DIRECT_DEBITING <= poCCR_i->unRequestedAction && OCS_PRICE_ENQUIRY >= poCCR_i->unRequestedAction)
	{
		return oRequestType[poCCR_i->unRequestedAction];
	}
	else
	{
		return OCS_Invalid_Request_Type;
	}
}

t_OCSAPIRetVal	OCF_PopulatePriceRequest(	t_OCSPriceRequest* 	poPriceRequest_io,
											t_OCSCCR* 			poCCR_i,
											t_OCSRequestSubType	oRequestSubType_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	if(NULL == poPriceRequest_io || NULL == poCCR_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Null Pointer");
		return DMBase_Retval_Null_Pointer;
	}


	OCS_CopyOCSUTF8String(&poPriceRequest_io->oSessionId, &poCCR_i->oSessionId);
	OCS_CopyOCSUTF8String(&poPriceRequest_io->oServiceContextId, &poCCR_i->oServiceContextId);
	poPriceRequest_io->ullCCSubSessionId = poCCR_i->ullCCSubSessionId;
	memcpy(&poPriceRequest_io->oSubscriptionId, &poCCR_i->oSubscriptionId, sizeof(t_OCSSubscriptionId));
	//memcpy(&poPriceRequest_io->oUserName, &poCCR_i->oUserName, sizeof(t_OCSFixedLenUTF8String));
	OCS_CopyOCSUTF8String(&poPriceRequest_io->oUserName, &poCCR_i->oUserName);
	poPriceRequest_io->oRequestSubType = oRequestSubType_i;

	if(OCS_UNSUED_VALUE != poCCR_i->unServiceId)
	{
		poPriceRequest_io->unServiceId = poCCR_i->unServiceId;
	}
	else
	{
		poPriceRequest_io->unServiceId = poCCR_i->oMultiServicesCC[0].unServiceId;
	}

	if(OCS_Direct_Debiting == oRequestSubType_i)
	{
		if(OCS_UNSUED_VALUE != poCCR_i->unServiceId)
		{
			memcpy(&poPriceRequest_io->oReqServiceUnit, &poCCR_i->oReqServiceUnit,
					sizeof(t_OCSReqServiceUnit));
		}
		else
		{
			memcpy(&poPriceRequest_io->oReqServiceUnit, &poCCR_i->oMultiServicesCC[0].oCCR.oReqServiceUnit,
					sizeof(t_OCSReqServiceUnit));
		}
	}
	else if(OCS_Refund_Account == oRequestSubType_i)
	{
		if(OCS_UNSUED_VALUE != poCCR_i->unServiceId)
		{
			memcpy(&poPriceRequest_io->oReqServiceUnit, &poCCR_i->oReqServiceUnit,
					sizeof(t_OCSReqServiceUnit));
		}
		else
		{
			memcpy(&poPriceRequest_io->oReqServiceUnit, &poCCR_i->oMultiServicesCC[0].oCCR.oReqServiceUnit,
					sizeof(t_OCSReqServiceUnit));
		}
	}
	else if(OCS_Check_Balance == oRequestSubType_i)
	{

	}
	else if(OCS_Price_Enquiry == oRequestSubType_i)
	{

		//poPriceRequest_io->unServiceId = poCCR_i->oMultiServicesCC[0].unServiceId;
	}
	else if(OCS_Reservation == oRequestSubType_i)
	{
		memcpy(&poPriceRequest_io->oReqServiceUnit, &poCCR_i->oMultiServicesCC[0].oCCR.oReqServiceUnit,
				sizeof(t_OCSReqServiceUnit));
	}
	else if(OCS_Release == oRequestSubType_i)
	{
		poPriceRequest_io->unTerminationCause = poCCR_i->unTerminationCause;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal	OCF_ProcessECURInitialRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSCCSUBSESSIONOBJECT hSessionObject = NULL;

	if(NULL == hOCFContext_io || NULL == poCCR_i || NULL == hPeer_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Null Pointer");
		return OCS_Retval_Null_Pointer;
	}

	if(OCS_Retval_Success != OCF_CreateSubSessionObject(hOCFContext_io, &hSessionObject, &poCCR_i->oSessionId,
														poCCR_i->ullCCSubSessionId, hPeer_i, OCS_SESSION_STATE_IDLE,
														poCCR_i, OCS_Charging_ECUR, TRUE))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to create session object\r\n");
		return OCS_Retval_Failure;
	}

	if(OCS_Retval_Success != OCF_InsertSubSessionObject(hOCFContext_io, &poCCR_i->oSessionId, poCCR_i->ullCCSubSessionId, (void*)hSessionObject))
	{
		DMBase_Free(hOCFContext_io->hDiamStackContext, (void**)&hSessionObject);
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to insert session object\r\n");
		return OCS_Retval_Failure;
	}

	t_OCSPriceRequest oPriceRequest;
	DMBase_MemsetZero(&oPriceRequest, sizeof(t_OCSPriceRequest));
	OCF_PopulatePriceRequest(&oPriceRequest, poCCR_i, OCS_Reservation);


	if(DMBase_Retval_Success != DMBASE_EvtSysSendIntEvt(hOCFContext_io->hDiamStackContext,
														OCS_RATING_FUNCTION_PRICE_REQUEST_EVENT,
														(void*)&oPriceRequest,
														sizeof(t_OCSPriceRequest)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to send event");
		return OCS_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}


t_OCSAPIRetVal	OCF_ProcessECURTerminateRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSCCSUBSESSIONOBJECT hSubSession = NULL;

	if(OCS_Retval_Success != OCF_LookUpSubSessionObject(hOCFContext_io, &poCCR_i->oSessionId,
														poCCR_i->ullCCSubSessionId, (void**)&hSubSession))
	{
		//generate unknown session error.
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "No Session Object Found\n");
		return OCS_Retval_Failure;
	}

	if(hSubSession->bProcessingInProgress)
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Current Session already in processing\n");
		return OCS_Retval_Failure;
	}

	hSubSession->bProcessingInProgress = TRUE;
	hSubSession->hPeer = hPeer_i;
	hSubSession->poCCR = poCCR_i;

	t_OCSPriceRequest oPriceRequest;
	DMBase_MemsetZero(&oPriceRequest, sizeof(t_OCSPriceRequest));
	t_OCSRequestSubType oRequestSubType = (OCS_UNSUED_VALUE == poCCR_i->unTerminationCause ? OCS_Debit:OCS_Release);
	OCF_PopulatePriceRequest(&oPriceRequest, poCCR_i, oRequestSubType);

	if(DMBase_Retval_Success != DMBASE_EvtSysSendIntEvt(hOCFContext_io->hDiamStackContext,
														OCS_RATING_FUNCTION_PRICE_REQUEST_EVENT,
														(void*)&oPriceRequest,
														sizeof(t_OCSPriceRequest)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to send event");
		return OCS_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}




