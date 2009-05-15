
#include "ocf.h"
#include "ocf_statemachine.h"
#include "ocf_session.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_appcallbackreg.h"
#include "ocf_msgextractor.h"
#include "ocf_sbcf.h"
#include "ocf_ebcf.h"
#include "dmbase_parser.h"
#include "dmbase_avp.h"
#include "dmbase_initconfig.h"
#include "dmbase_msgvalidate.h"

extern int g_nDiamBaseTraceID ;

AMPS_MODULE_HANDLE	ocf_ModuleInit(	AMPS_HANDLE hAMPS_HANDLE_i,
											void* 		pvUserData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	printf("\r\n\t\tLoading OCF Module\r\n");
	HDIAMETERSTACKCONTEXT hDiamStackContext = NULL;
	HOCFCONTEXT	hOCFContext = NULL;

	hDiamStackContext = (HDIAMETERSTACKCONTEXT)pvUserData_i;
	DMBase_Malloc(hDiamStackContext, (void**)&hOCFContext, sizeof(t_OCFContext));
	hOCFContext->hDiamStackContext = hDiamStackContext;
	if(OCS_Retval_Success != OCF_Init(hOCFContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed To Load OCF Module");

	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");

	return (AMPS_MODULE_HANDLE)0xABCDEF;
}

void 				ocf_ModuleCleanup(	AMPS_HANDLE 		hAMPS_HANDLE_i,
										AMPS_MODULE_HANDLE 	hAMPS_MODULE_HANDLE_i)
{

}

int	Event1ValidationCallback(AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, int r_nEventID,
							  void* r_pvEventPayload)
{
//	printf("\n\n DBModule , inside Event1ValidationCallback\n\n");

	return AMPS_SUCCESS;
}


t_OCSAPIRetVal OCF_Init(HOCFCONTEXT hOCFContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT	hDiamStackContext = NULL;
	hDiamStackContext = hOCFContext_io->hDiamStackContext;

	//Registration of Internal events
	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext,OCS_RATING_FUNCTION_RESPONSE_EVENT,
															OCF_ProcessRatingFuncResponse, (void*)hOCFContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_RESPONSE_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext, OCS_RATING_FUNCTION_PRICE_RESPONE_EVENT,
															OCF_ProcessPriceResponse, (void*)hOCFContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_PRICE_RESPONE_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext,OCS_RATING_FUNCTION_TARIFF_RESPONE_EVENT,
															OCF_ProcessTariffResponse, (void*)hOCFContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_TARIFF_RESPONE_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegIntEvt(hDiamStackContext, OCS_RATING_FUNCTION_SERVICE_USAGE_RESPONE_EVENT,
															OCF_ProcessServiceUsageResponse, (void*)hOCFContext_io))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OCS_RATING_FUNCTION_SERVICE_USAGE_RESPONE_EVENT event registration  failed\r\n");
        return OCS_Retval_Failure;
    }


    //Register application callback
	if(DMBase_Retval_Success != DMBase_RegisterApplicationCallback(	hDiamStackContext,
																	OCS_APPLICATION_ID,
																	OCF_ProcessMessage,
																	NULL,
																	(void*)hOCFContext_io))
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Registration of OCS Application callback failed\r\n");
        return OCS_Retval_Failure;
	}

	AMPS_EvtSysSetEvtHandleValidationCallback(hDiamStackContext->hFrameworkHandle, Event1ValidationCallback);


	OCF_InitStateMachine(hOCFContext_io);

    if(DMBase_Retval_Success != DMBase_CreateHashTable(hDiamStackContext, OCS_SESSION_TABLE_SIZE,
														&hOCFContext_io->hSessionTable, NULL))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Session Table creation failed\r\n");
        return OCS_Retval_Failure;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}



t_DiamAPIReturnVal OCF_ProcessMessage(HPEERTABLEENTRY hPeer_i, HDIAMETERMESSAGE hDiamMsg_i, void* pvAppContext)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HOCFCONTEXT hOCFContext = (HOCFCONTEXT)pvAppContext;
	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext->hDiamStackContext;
	t_OCSCCR *poCCR = NULL;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Failure;
	unsigned int unCommandCode;

	DMBase_GetDiaMsgCmdCodeFromDiamMsg(hDiamStackContext, hDiamMsg_i, &unCommandCode);
	if(OCS_CREDIT_CONTROL_REQUEST != unCommandCode)
	{
		DMBase_GenerateProtocolErr(hOCFContext->hDiamStackContext, hPeer_i,hDiamMsg_i, DIAMETER_COMMAND_UNSUPPORTED, NULL);
		return DMBase_Retval_Not_Supported;
	}

	if(DMBase_Retval_Success != DMBase_Malloc(hOCFContext->hDiamStackContext, (void**)&poCCR, sizeof(t_OCSCCR)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to allocate memory");
		return DMBase_Retval_Failure;
	}


	if(DMBase_Retval_Success != OCF_ExtractCCRAttributes(hOCFContext, hDiamMsg_i, poCCR))
	{
		//Add Validation code
		DMBase_Free(hOCFContext->hDiamStackContext, (void**)&poCCR);
		return DMBase_Retval_Failure;
	}

	switch(poCCR->oCCRequestType)
	{
		case OCS_INITIAL_REQUEST:
			oReturnValue = OCF_ProcessInitialRequest(hOCFContext, poCCR, hPeer_i);
			break;
		case OCS_UPDATE_REQUEST:
			oReturnValue = OCF_ProcessUpdateRequest(hOCFContext, poCCR, hPeer_i);
			break;
		case OCS_TERMINATION_REQUEST:
			oReturnValue = OCF_ProcessTerminateRequest(hOCFContext, poCCR, hPeer_i);
			break;
		case OCS_EVENT_REQUEST:
			oReturnValue = OCF_EventBasedCharging(hOCFContext, poCCR, hPeer_i);
			break;
	}

	if(OCS_Retval_Not_Found == oReturnValue)
	{
		DMBase_Free(hOCFContext->hDiamStackContext, (void**)&poCCR);
		DMBase_GeneratePermanentFailure(hOCFContext->hDiamStackContext, hPeer_i,hDiamMsg_i, DIAMETER_UNKNOWN_SESSION_ID, NULL);
		return DMBase_Retval_Failure;
	}
	else if(OCS_Retval_Success != oReturnValue)
	{
		DMBase_Free(hOCFContext->hDiamStackContext, (void**)&poCCR);
		DMBase_GeneratePermanentFailure(hOCFContext->hDiamStackContext, hPeer_i,hDiamMsg_i, DIAMETER_UNABLE_TO_COMPLY, NULL);
		return DMBase_Retval_Failure;
	}

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return DMBase_Retval_Success;
}


void	OCF_ProcessRatingFuncResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");
	printf("OCF_ProcessRatingFuncResponse\r\n");
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
}


t_OCSAPIRetVal OCF_CreatCCADiamMsg(HOCFCONTEXT hOCFContext_i, t_OCSCCR* poCCR_i, t_OCSCCA* poCCA_i, HDIAMETERMESSAGE* phDiamMsg_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERMESSAGE hDiamCCAMsg = NULL;
	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HMSGALLOCATIONCONTEXT	hMsgAllocCtxt = NULL;
	HAVP hTempAVP;
	DMBase_SList*	poAVPList = NULL;
	unsigned char*	pucHostName = NULL;
	unsigned char*	pucRealmName = NULL;

    if(DMBase_Retval_Success != DMBase_CreateDiameterMessage(	hDiamStackContext,
																&hDiamCCAMsg,
																DIAMETER_VERSION_NUMBER,
																OCS_CREDIT_CONTROL_ANSWER,
																OCS_APPLICATION_ID,
																poCCR_i->ucFlags,
																poCCR_i->unHopByHopId,
																poCCR_i->unEndToEndId))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create message\n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
    }


    DMBase_SetDiaMsgRFlag(hDiamStackContext, hDiamCCAMsg,0);


	if (DMBase_Retval_Success !=  DMBase_Get_AllocationContext_ForMessage(hDiamStackContext,
																hDiamCCAMsg,&hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_Get_AllocationContext_ForMessage Failed , Leaving \n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}



	if(0 < poCCR_i->oSessionId.unLength)
	{
		hTempAVP = NULL;
	    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_SESSION_ID, &hTempAVP,
						poCCR_i->oSessionId.ucString, poCCR_i->oSessionId.unLength , hMsgAllocCtxt))
	    {
	        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
	        oReturnValue = OCS_Retval_Failure;
	        goto ExitMe;
	    }
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
	}



    if(0 != poCCA_i->unResultCode)
    {
    	hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_RESULT_CODE,
													 &hTempAVP, poCCA_i->unResultCode, hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
		}
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
    }

    hTempAVP = NULL;
    DMBase_GetHostNameFromDiameterContext(hDiamStackContext, (char**)&pucHostName);
    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext,
															 DMBASE_AVP_ORIGIN_HOST,
															 &hTempAVP,
															 pucHostName,
															 strlen((char*)pucHostName),
															 hMsgAllocCtxt ))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
        oReturnValue = OCS_Retval_Failure;
        goto ExitMe;
    }
    DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);

    hTempAVP = NULL;
    DMBase_GetRealmNameFromDiameterContext(hDiamStackContext, (char**)&pucRealmName);

    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext,
															 DMBASE_AVP_ORIGIN_REALM,
															 &hTempAVP,
															 pucRealmName,
															 strlen((char*)pucRealmName) ,
															 hMsgAllocCtxt ))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
        oReturnValue = OCS_Retval_Failure;
        goto ExitMe;
    }
    DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);

	hTempAVP = NULL;
    if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_AUTH_APPLICATION_ID,
															&hTempAVP, OCS_APPLICATION_ID, hMsgAllocCtxt))
    {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
     	oReturnValue = OCS_Retval_Failure;
      	goto ExitMe;
    }
    DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);

	hTempAVP = NULL;
	if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, OCS_AVP_CC_REQUEST_TYPE,
												 &hTempAVP, poCCR_i->oCCRequestType, hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}
	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);


	hTempAVP = NULL;
    if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_CC_REQUEST_NUMBER,
															&hTempAVP, poCCR_i->unCCRequestNum, hMsgAllocCtxt))
    {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
     	oReturnValue = OCS_Retval_Failure;
      	goto ExitMe;
    }
    DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);

    if(OCS_UNSUED_VALUE != poCCR_i->ullCCSubSessionId)
    {
		hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateUnsigned64AVP(hDiamStackContext, OCS_AVP_SUB_SESSION_ID,
																&hTempAVP, poCCR_i->ullCCSubSessionId, hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
		}
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
    }

    if(NULL != poCCA_i->poGrantedServiceUnits)
    {
 	   //Add Granted Service Units
 	   hTempAVP = NULL;
 	   if(OCS_Retval_Success != OCF_CreatGrantedServiceUnitAVP(hOCFContext_i,hMsgAllocCtxt,
 	   											  poCCA_i->poGrantedServiceUnits,
 	   											  &hTempAVP))
 	   {
 	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
 	    	oReturnValue = OCS_Retval_Failure;
 	     	goto ExitMe;
 	   }
 	  DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
    }

    //Add Multiple-Services-Credit-Control

    if(0 < poCCA_i->unMultiServiceCCCount)
    {

    	int nCount;
    	for(nCount = 0; nCount < poCCA_i->unMultiServiceCCCount; nCount++)
    	{
    		hTempAVP = NULL;
    		OCF_CreatMultipleServiceCCAVP(hOCFContext_i, hMsgAllocCtxt, &poCCA_i->poMultiServicesCC[nCount], &hTempAVP);
    		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
    	}
    }


    if(NULL != poCCA_i->poCostInfo)
    {
    	//Add Cost-Information here
    	hTempAVP = NULL;
    	OCF_CreatCostInformationAVP(hOCFContext_i, hMsgAllocCtxt, poCCA_i->poCostInfo, &hTempAVP);
    	if(NULL != hTempAVP)
    	{
    		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
    	}
    }

    //Low-Balance-Indication Here
	hTempAVP = NULL;
	if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, OCS_AVP_LOW_BALANCE_INDICATION,
												 &hTempAVP, poCCA_i->oLowBalanceIndication, hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}
	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);


    if(NULL != poCCA_i->poRemainingBalance)
    {
    	//Add Remaining-Balance here

    	hTempAVP = NULL;
    	OCF_CreatRemainingBalanceAVP(hOCFContext_i, hMsgAllocCtxt, poCCA_i->poRemainingBalance, &hTempAVP);
    	if(NULL != hTempAVP)
    	{
    		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
    	}
    }

    //Check Balance Result
    if(OCS_NOT_USED != poCCA_i->oCheckBalanceResult)
    {
    	hTempAVP = NULL;
    	if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, OCS_AVP_CHECK_BALANCE_RESULT,
    												 &hTempAVP, poCCA_i->oCheckBalanceResult, hMsgAllocCtxt))
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
    		oReturnValue = OCS_Retval_Failure;
    		goto ExitMe;
    	}
    	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);

    }

    //Free  pucHostName, pucRealmName
    DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext, hDiamCCAMsg, poAVPList);
    DMBase_Free(hOCFContext_i->hDiamStackContext, (void**)&pucHostName);
    DMBase_Free(hOCFContext_i->hDiamStackContext, (void**)&pucRealmName);

    *phDiamMsg_io = hDiamCCAMsg;
    ExitMe:

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");

    return oReturnValue;
}


t_OCSAPIRetVal OCF_CreateUnitValueAVP(HOCFCONTEXT 			hOCFContext_i,
									  HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
									  t_OCSUnitValue* 		poUnitValue_i,
									  HAVP* 				phUnitValue_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HAVP hUnitValueAVP = NULL;
	HAVP hTempAVP = NULL;

	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hUnitValueAVP, hMsgAllocCtxt_io,
															OCS_AVP_UNIT_VALUE, 0))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}


	if(0 < poUnitValue_i->llValueDigits)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateInteger64AVP(hDiamStackContext, OCS_AVP_VALUE_DIGITS,
																&hTempAVP, poUnitValue_i->llValueDigits,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hUnitValueAVP, hTempAVP);
	}


	if(0 < poUnitValue_i->nExponent)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateIntAVP(hDiamStackContext, OCS_AVP_EXPONENT,
																&hTempAVP, poUnitValue_i->nExponent,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hUnitValueAVP, hTempAVP);
	}

	*phUnitValue_io = hUnitValueAVP;
	ExitMe:

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return oReturnValue;
}


t_OCSAPIRetVal OCF_CreateCCMoneyAVP(HOCFCONTEXT 			hOCFContext_i,
									HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
									t_OCSCCMoney* 			poCCMoney_i,
									HAVP* 					phCCMoneyAVP_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HAVP hCCMoneyAVP = NULL;
	HAVP hTempAVP = NULL;

	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hCCMoneyAVP, hMsgAllocCtxt_io,
															OCS_AVP_CC_MONEY, 0))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}

	OCF_CreateUnitValueAVP(hOCFContext_i, hMsgAllocCtxt_io, &poCCMoney_i->oUnitValue, &hTempAVP);

	if(NULL != hTempAVP)
	{
	   DMBase_AddAVPToGroup(hDiamStackContext, hCCMoneyAVP, hTempAVP);
	}

	if(0 < poCCMoney_i->unCurrencyCode)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_CURRENCY_CODE,
																&hTempAVP, poCCMoney_i->unCurrencyCode,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hCCMoneyAVP, hTempAVP);
	}

	*phCCMoneyAVP_io = hCCMoneyAVP;
	ExitMe:

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return oReturnValue;
}

t_OCSAPIRetVal OCF_CreatGrantedServiceUnitAVP(HOCFCONTEXT 				hOCFContext_i,
											  HMSGALLOCATIONCONTEXT		hMsgAllocCtxt_io,
											  t_OCSGrantedServiceUnit* 	poGrantedServiceUnit_i,
											  HAVP* 					phGrantedServiceUnitAVP_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HAVP hGrantedServiceUnitAVP = NULL;
//	DMBase_SList*	poAVPList = NULL;
	HAVP hTempAVP = NULL;

	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hGrantedServiceUnitAVP,hMsgAllocCtxt_io,
															OCS_AVP_GRANTED_SERVICE_UNIT, 0))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}

	if(0 < poGrantedServiceUnit_i->unTariffTimeChange)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_TARIFF_TIME_CHANGE,
																&hTempAVP, poGrantedServiceUnit_i->unTariffTimeChange,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hGrantedServiceUnitAVP, hTempAVP);
	}

	if(0 < poGrantedServiceUnit_i->unCCTime)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_CC_TIME,
																&hTempAVP, poGrantedServiceUnit_i->unCCTime,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hGrantedServiceUnitAVP, hTempAVP);
	}


	if(0 < poGrantedServiceUnit_i->oCCMoney.oUnitValue.llValueDigits)
	{
		hTempAVP = NULL;
	   if(OCS_Retval_Success != OCF_CreateCCMoneyAVP(hOCFContext_i, hMsgAllocCtxt_io,
									   &poGrantedServiceUnit_i->oCCMoney , &hTempAVP))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hGrantedServiceUnitAVP, hTempAVP);
	}

	if(0 < poGrantedServiceUnit_i->ullCCInputOctets)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned64AVP(hDiamStackContext, OCS_AVP_CC_INPUT_OCTETS,
																&hTempAVP, poGrantedServiceUnit_i->ullCCInputOctets,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hGrantedServiceUnitAVP, hTempAVP);
	}

	if(0 < poGrantedServiceUnit_i->ullCCOutputOctets)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned64AVP(hDiamStackContext, OCS_AVP_CC_OUTPUT_OCTETS,
																&hTempAVP, poGrantedServiceUnit_i->ullCCOutputOctets,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hGrantedServiceUnitAVP, hTempAVP);
	}

	if(0 < poGrantedServiceUnit_i->ullCCServiceSpecificUnits)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned64AVP(hDiamStackContext, OCS_AVP_CC_SERVICE_SPECIFIC_UNITS,
																&hTempAVP, poGrantedServiceUnit_i->ullCCServiceSpecificUnits,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hGrantedServiceUnitAVP, hTempAVP);
	}

	*phGrantedServiceUnitAVP_io = hGrantedServiceUnitAVP;

	ExitMe:

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return oReturnValue;
}


t_OCSAPIRetVal OCF_CreatMultipleServiceCCAVP(	HOCFCONTEXT 			hOCFContext_i,
												HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
												t_OCSMultiServicesCC* 	poMultiServiceCC_i,
												HAVP* 					phMulipleServiceCCAVP_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HAVP hMulipleServiceCCAVP = NULL;
//	DMBase_SList*	poAVPList = NULL;
	HAVP hTempAVP = NULL;

   if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hMulipleServiceCCAVP,hMsgAllocCtxt_io,
															OCS_AVP_MULTIPLE_SERVICES_CREDIT_CONTROL, 0))
   {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
    	oReturnValue = OCS_Retval_Failure;
     	goto ExitMe;
   }

   if(NULL != poMultiServiceCC_i->oCCA.poGrantedServiceUnits)
   {
	   //Add Granted Service Units
	   hTempAVP = NULL;
	   if(OCS_Retval_Success != OCF_CreatGrantedServiceUnitAVP(hOCFContext_i,hMsgAllocCtxt_io,
	   											  poMultiServiceCC_i->oCCA.poGrantedServiceUnits,
	   											  &hTempAVP))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->unServiceId)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_SERVICE_IDENTIFIER,
																&hTempAVP, poMultiServiceCC_i->unServiceId,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->unRatingGroup)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_RATING_GROUP,
																&hTempAVP, poMultiServiceCC_i->unRatingGroup,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(NULL != poMultiServiceCC_i->oCCA.poGSUPoolReference)
   {

   }

   if(0 < poMultiServiceCC_i->oCCA.unValidityTime)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_VALIDITY_TIME,
																&hTempAVP, poMultiServiceCC_i->oCCA.unValidityTime,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->oCCA.unResultCode)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_RESULT_CODE,
																&hTempAVP, poMultiServiceCC_i->oCCA.unResultCode,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(NULL != poMultiServiceCC_i->oCCA.poFinalUnitIndication)
   {

   }

   if(0 < poMultiServiceCC_i->oCCA.unTimeQuotaThreshold)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_TIME_QUOTA_THRESHOLD,
																&hTempAVP, poMultiServiceCC_i->oCCA.unTimeQuotaThreshold,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->oCCA.unVolumeQuotaThreshold)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_VOLUME_QUOTA_THRESHOLD,
																&hTempAVP, poMultiServiceCC_i->oCCA.unVolumeQuotaThreshold,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->oCCA.unUnitQuotaThreshold)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_UNIT_QUOTA_THRESHOLD,
																&hTempAVP, poMultiServiceCC_i->oCCA.unUnitQuotaThreshold,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->oCCA.unQuotaHoldingTime)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_QUOTA_HOLDING_TIME,
																&hTempAVP, poMultiServiceCC_i->oCCA.unQuotaHoldingTime,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(0 < poMultiServiceCC_i->oCCA.unQuotaConsumptionTime)
   {
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_QUOTA_CONSUMPTION_TIME,
																&hTempAVP, poMultiServiceCC_i->oCCA.unQuotaConsumptionTime,
																hMsgAllocCtxt_io))
	   {
	    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
	    	oReturnValue = OCS_Retval_Failure;
	     	goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hMulipleServiceCCAVP, hTempAVP);
   }

   if(NULL != poMultiServiceCC_i->oCCA.poTrigger)
   {

   }

   if(NULL != poMultiServiceCC_i->oCCA.poRefundInfo)
   {

   }

   if(NULL != poMultiServiceCC_i->oCCA.poEnvelopReporting)
   {

   }

   if(NULL != poMultiServiceCC_i->oCCA.poTimeQuotaMechanism)
   {

   }

   *phMulipleServiceCCAVP_io = hMulipleServiceCCAVP;

   ExitMe:

   DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
   return oReturnValue;
}


t_OCSAPIRetVal OCF_CreatCostInformationAVP(	HOCFCONTEXT 			hOCFContext_i,
											HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
											t_OCSCostInformation*	poCostInfo_i,
											HAVP* 					phCostInfoAVP_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HAVP hCostInfoAVP = NULL;
//	DMBase_SList*	poAVPList = NULL;
	HAVP hTempAVP = NULL;

	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hCostInfoAVP, hMsgAllocCtxt_io,
															OCS_AVP_COST_INFORMATION, 0))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}

	hTempAVP = NULL;
	OCF_CreateUnitValueAVP(hOCFContext_i, hMsgAllocCtxt_io, &poCostInfo_i->oUnitValue, &hTempAVP);

	if(NULL != hTempAVP)
	{
	   DMBase_AddAVPToGroup(hDiamStackContext, hCostInfoAVP, hTempAVP);
	}

	if(0 < poCostInfo_i->unCurrencyCode)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_CURRENCY_CODE,
																&hTempAVP, poCostInfo_i->unCurrencyCode,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hCostInfoAVP, hTempAVP);
	}

	if(0 < poCostInfo_i->oCostUnit.unLength)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, OCS_AVP_COST_UNIT,
																&hTempAVP, poCostInfo_i->oCostUnit.ucString,
																poCostInfo_i->oCostUnit.unLength,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hCostInfoAVP, hTempAVP);
	}


	*phCostInfoAVP_io = hCostInfoAVP;

	ExitMe:

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return oReturnValue;
}



t_OCSAPIRetVal OCF_CreatRemainingBalanceAVP(	HOCFCONTEXT 			hOCFContext_i,
											HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
											t_OCSRemainingBalance*	poRemainingBalance_i,
											HAVP* 					phRemainingBalance_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_i->hDiamStackContext;
	t_OCSAPIRetVal oReturnValue = OCS_Retval_Success;
	HAVP hRemainingBalanceAVP = NULL;
//	DMBase_SList*	poAVPList = NULL;
	HAVP hTempAVP = NULL;

	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hRemainingBalanceAVP, hMsgAllocCtxt_io,
															OCS_AVP_REMAINING_BALANCE, 0))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
		oReturnValue = OCS_Retval_Failure;
		goto ExitMe;
	}


	hTempAVP = NULL;
	OCF_CreateUnitValueAVP(hOCFContext_i, hMsgAllocCtxt_io, &poRemainingBalance_i->oUnitValue, &hTempAVP);

	if(NULL != hTempAVP)
	{
	   DMBase_AddAVPToGroup(hDiamStackContext, hRemainingBalanceAVP, hTempAVP);
	}

	if(0 < poRemainingBalance_i->unCurrencyCode)
	{
		hTempAVP = NULL;
	   if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, OCS_AVP_CURRENCY_CODE,
																&hTempAVP, poRemainingBalance_i->unCurrencyCode,
																hMsgAllocCtxt_io))
	   {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
			oReturnValue = OCS_Retval_Failure;
			goto ExitMe;
	   }
	   DMBase_AddAVPToGroup(hDiamStackContext, hRemainingBalanceAVP, hTempAVP);
	}


	*phRemainingBalance_io = hRemainingBalanceAVP;

	ExitMe:

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return oReturnValue;
}



t_OCSEvent 	OCF_CheckEventType(	HOCFCONTEXT 			hOCFContext_i,
								t_OCSCCRequestType		oRequestType_i,
								t_OCSRatingFuncResult	oRatingFunctionResult_i)
{

	return hOCFContext_i->oEventsMapping[oRequestType_i][oRatingFunctionResult_i];
}


t_OCSChargingType OCF_CheckChargingType(HOCFCONTEXT	hOCFContext_i,
										t_OCSCCR*	poCCR_i)
{

	unsigned int unServiceId = poCCR_i->oMultiServicesCC[0].unServiceId;

	if(1 == unServiceId || 2 == unServiceId )
	{
		return OCS_Charging_ECUR;
	}
	else
	{
		return OCS_Charging_SCUR;
	}
}


t_OCSAPIRetVal	OCF_ProcessInitialRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	t_OCSAPIRetVal oReturnVal;
	if(OCS_Charging_ECUR == OCF_CheckChargingType(hOCFContext_io, poCCR_i))
	{
		oReturnVal = OCF_ProcessECURInitialRequest(hOCFContext_io, poCCR_i, hPeer_i);
	}
	else
	{
		oReturnVal = OCF_ProcessSCURInitialRequest(hOCFContext_io, poCCR_i, hPeer_i);
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return oReturnVal;
}

t_OCSAPIRetVal  OCF_ProcessUpdateRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");
	t_OCSAPIRetVal oReturnVal;

	oReturnVal = OCF_ProcessSCURUpdateRequest(hOCFContext_io, poCCR_i, hPeer_i);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return oReturnVal;
}

t_OCSAPIRetVal	OCF_ProcessTerminateRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	t_OCSAPIRetVal oReturnVal;

	if(OCS_Charging_ECUR == OCF_CheckChargingType(hOCFContext_io, poCCR_i))
	{
		oReturnVal = OCF_ProcessECURTerminateRequest(hOCFContext_io, poCCR_i, hPeer_i);
	}
	else
	{
		oReturnVal = OCF_ProcessSCURTerminateRequest(hOCFContext_io, poCCR_i, hPeer_i);
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return oReturnVal;
}



