
#include "ocf.h"
#include "ocf_sbcf.h"
#include "ocs_defines.h"
#include "ocf_session.h"
#include "dmbase_utility.h"
#include "ocf_msgextractor.h"
#include "ocf_statemachine.h"
#include "dmbase_ampswrappers.h"


extern	int	g_nDiamBaseTraceID ;

void	OCF_ProcessTariffResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload)
{
	HOCFCONTEXT hOCFContext = (HOCFCONTEXT)r_hAMPS_APP_HANDLE;
	t_OCSTariffResponse* poTariffResponse = (t_OCSTariffResponse*)r_pvEventPayload;
	HOCSCCSUBSESSIONOBJECT hSessionObject = NULL;
	t_OCSEvent oEventType;

	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	if(NULL == poTariffResponse)
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "poTariffResponse is NULL\n");
		return;
	}

	if(OCS_Retval_Success != OCF_LookUpSubSessionObject(hOCFContext, &poTariffResponse->oSessionId,
														poTariffResponse->ullCCSubSessionId,
														(void**)&hSessionObject))
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Session Object not found\n");
		return;
	}

	oEventType = OCF_CheckEventType(hOCFContext, hSessionObject->poCCR->oCCRequestType,
												poTariffResponse->oResult);

	if(OCS_Retval_Success != OCF_ExecStateMachine(hOCFContext, hSessionObject, oEventType , r_pvEventPayload))
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " State Machine processing failed\n");
	}

	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Leaving\n");
}

void	OCF_ProcessServiceUsageResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload)
{

}


t_OCSAPIRetVal	OCF_ProcessSCURInitialRequest(	HOCFCONTEXT 	hOCFContext_io,
												t_OCSCCR* 		poCCR_i,
												HPEERTABLEENTRY hPeer_i)
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


	t_OCSTariffRequest oTarrifRequest;
	DMBase_MemsetZero(&oTarrifRequest, sizeof(t_OCSTariffRequest));

	OCF_CreateTariffRequest(hOCFContext_io, &oTarrifRequest, poCCR_i, OCS_Reservation);

	if(DMBase_Retval_Success != DMBASE_EvtSysSendIntEvt(hOCFContext_io->hDiamStackContext,
														OCS_RATING_FUNCTION_TARIFF_REQUEST_EVENT,
														(void*)&oTarrifRequest,
														sizeof(t_OCSTariffRequest)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to send event");
		return OCS_Retval_Failure;
	}

	printf("Tariff Req sent\r\n");
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}

t_OCSAPIRetVal  OCF_ProcessSCURUpdateRequest(	HOCFCONTEXT 	hOCFContext_io,
												t_OCSCCR* 		poCCR_i,
												HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSCCSUBSESSIONOBJECT hSubSession = NULL;

	if(OCS_Retval_Success != OCF_LookUpSubSessionObject(hOCFContext_io, &poCCR_i->oSessionId,
														poCCR_i->ullCCSubSessionId, (void**)&hSubSession))
	{
		//generate unknown session error.

		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "No Session Object Found\n");
		return OCS_Retval_Not_Found;
	}

	if(hSubSession->bProcessingInProgress)
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Current Session already in processing\n");
		return OCS_Retval_Failure;
	}
	/*
	if(NULL != hSubSession->oTccTimerId)
	{
		DMBase_KillTimer(hOCFContext_io->hDiamStackContext, hSubSession->oTccTimerId);
	}*/

	hSubSession->bProcessingInProgress = TRUE;
	hSubSession->hPeer = hPeer_i;
	hSubSession->poCCR = poCCR_i;

//	t_OCSTariffRequest oTarrifRequest1[poCCR_i->unMultiServiceCCCount];
//	oTarrifRequest1[0].unActualTime = 2;
	t_OCSTariffRequest oTarrifRequest;
	DMBase_MemsetZero(&oTarrifRequest, sizeof(t_OCSTariffRequest));

	OCF_CreateTariffRequest(hOCFContext_io, &oTarrifRequest, poCCR_i, OCS_Reservation);

	if(DMBase_Retval_Success != DMBASE_EvtSysSendIntEvt(hOCFContext_io->hDiamStackContext,
														OCS_RATING_FUNCTION_TARIFF_REQUEST_EVENT,
														(void*)&oTarrifRequest,
														sizeof(t_OCSTariffRequest)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to send event");
		return OCS_Retval_Failure;
	}


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}

t_OCSAPIRetVal	OCF_ProcessSCURTerminateRequest(HOCFCONTEXT 	hOCFContext_io,
												t_OCSCCR* 		poCCR_i,
												HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSCCSUBSESSIONOBJECT hSubSession = NULL;

	if(OCS_Retval_Success != OCF_LookUpSubSessionObject(hOCFContext_io, &poCCR_i->oSessionId,
														poCCR_i->ullCCSubSessionId, (void**)&hSubSession))
	{
		//generate unknown session error.
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "No Session Object Found\n");
		return OCS_Retval_Not_Found;
	}

	if(hSubSession->bProcessingInProgress)
	{
		DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Current Session already in processing\n");
		return OCS_Retval_Failure;
	}

	hSubSession->bProcessingInProgress = TRUE;
	hSubSession->hPeer = hPeer_i;
	hSubSession->poCCR = poCCR_i;

	t_OCSTariffRequest oTarrifRequest;
	DMBase_MemsetZero(&oTarrifRequest, sizeof(t_OCSTariffRequest));

	t_OCSRequestSubType oRequestSubType = (OCS_UNSUED_VALUE == poCCR_i->unTerminationCause ? OCS_Debit:OCS_Release);

	OCF_CreateTariffRequest(hOCFContext_io, &oTarrifRequest, poCCR_i, oRequestSubType);

	if(DMBase_Retval_Success != DMBASE_EvtSysSendIntEvt(hOCFContext_io->hDiamStackContext,
														OCS_RATING_FUNCTION_TARIFF_REQUEST_EVENT,
														(void*)&oTarrifRequest,
														sizeof(t_OCSTariffRequest)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to send event");
		return OCS_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}

t_OCSAPIRetVal	OCF_CreateTariffRequest(HOCFCONTEXT 		hOCFContext_io,
										t_OCSTariffRequest* poTariffRequest_io,
										t_OCSCCR* 			poCCR_i,
										t_OCSRequestSubType oRequestSubType_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	OCS_CopyOCSUTF8String(&poTariffRequest_io->oSessionId, &poCCR_i->oSessionId);
	poTariffRequest_io->ullCCSubSessionId = poCCR_i->ullCCSubSessionId;
	poTariffRequest_io->oRequestSubType = oRequestSubType_i;

	OCS_CopyOCSUTF8String(&poTariffRequest_io->oUserName, &poCCR_i->oUserName);

	poTariffRequest_io->oSubscriptionId = poCCR_i->oSubscriptionId;
	poTariffRequest_io->unServiceId = poCCR_i->oMultiServicesCC[0].unServiceId;
	memcpy(&poTariffRequest_io->oReqServiceUnit, &poCCR_i->oMultiServicesCC[0].oCCR.oReqServiceUnit,
												sizeof(t_OCSReqServiceUnit));

	poTariffRequest_io->unTerminationCause =  poCCR_i->unTerminationCause;

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}


