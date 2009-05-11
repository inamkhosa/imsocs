
#include "ocf_statemachine.h"
#include "dmbase_message.h"
#include "dmbase_utility.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_peer_mgmt.h"
#include "ocf_session.h"
#include "ocf_msgextractor.h"

extern int g_nDiamBaseTraceID ;

t_OCSAPIRetVal OCF_InitStateMachine(HOCFCONTEXT hOCFAppContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	hOCFAppContext_io->oEventsMapping[OCS_INITIAL_REQUEST][OCS_SUCCESS] = OCS_CC_INIT_REQ_PROCESSING_SUCCESSFUL;
	hOCFAppContext_io->oEventsMapping[OCS_INITIAL_REQUEST][OCS_FAILURE] = OCS_CC_INIT_REQ_PROCESSING_UNSUCCESSFUL;

	hOCFAppContext_io->oEventsMapping[OCS_UPDATE_REQUEST][OCS_SUCCESS] = OCS_CC_UPDATE_REQ_PROCESSING_SUCCESSFUL;
	hOCFAppContext_io->oEventsMapping[OCS_UPDATE_REQUEST][OCS_FAILURE] = OCS_CC_UPDATE_REQ_PROCESSING_UNSUCCESSFUL;

	hOCFAppContext_io->oEventsMapping[OCS_TERMINATION_REQUEST][OCS_SUCCESS] = OCS_CC_TERMINATE_REQ_PROCESSING_SUCCESSFUL;
	hOCFAppContext_io->oEventsMapping[OCS_TERMINATION_REQUEST][OCS_FAILURE] = OCS_CC_TERMINATE_REQ_PROCESSING_UNSUCCESSFUL;

	hOCFAppContext_io->oEventsMapping[OCS_EVENT_REQUEST][OCS_SUCCESS] = OCS_CC_EVENT_REQ_PROCESSING_SUCCESSFUL;
	hOCFAppContext_io->oEventsMapping[OCS_EVENT_REQUEST][OCS_FAILURE] = OCS_CC_EVENT_REQ_PROCESSING_UNSUCCESSFUL;


	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_OPEN][OCS_CC_UPDATE_REQ_PROCESSING_SUCCESSFUL] = OCF_Open_UpdateReqProcessingSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_OPEN][OCS_CC_UPDATE_REQ_PROCESSING_UNSUCCESSFUL] = OCF_Open_UpdateReqProcessingUnSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_OPEN][OCS_CC_TERMINATE_REQ_PROCESSING_SUCCESSFUL] = OCF_Open_TerminateReqProcessingSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_OPEN][OCS_CC_TERMINATE_REQ_PROCESSING_UNSUCCESSFUL] = OCF_Open_TerminateReqProcessingUnSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_OPEN][OCS_TCC_TIMER_EXPIRED] = OCF_Open_TccTimerExpires;

	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_IDLE][OCS_CC_INIT_REQ_PROCESSING_SUCCESSFUL] = OCF_Idle_InitReqProcessingSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_IDLE][OCS_CC_INIT_REQ_PROCESSING_UNSUCCESSFUL] = OCF_Idle_InitReqProcessingUnSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_IDLE][OCS_CC_EVENT_REQ_PROCESSING_SUCCESSFUL] = OCF_Idle_EventReqProcessingSuccessful;
	hOCFAppContext_io->oOCSStateEventMapping[OCS_SESSION_STATE_IDLE][OCS_CC_EVENT_REQ_PROCESSING_UNSUCCESSFUL] = OCF_Idle_EventReqProcessingUnSuccessful;

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}


t_OCSAPIRetVal OCF_ExecStateMachine(HOCFCONTEXT 		hOCFAppContext_io,
									HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
									t_OCSEvent     		oEvent_i,
									void*              	pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	if(NULL == hOCFAppContext_io || NULL == hSubSessionObject_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hSubSessionObject_i is NULL");
		return OCS_Retval_Null_Pointer;
	}

	t_OCSAPIRetVal oReturnVal;

	if(NULL == hOCFAppContext_io->oOCSStateEventMapping[hSubSessionObject_i->oSessionState][oEvent_i])
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Invalid Action handler");
		return OCS_Retval_Failure;
	}

	oReturnVal = hOCFAppContext_io->oOCSStateEventMapping[hSubSessionObject_i->oSessionState][oEvent_i](
																						hOCFAppContext_io,
																						hSubSessionObject_i,
																						oEvent_i,
																						pvData_i);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return oReturnVal;
}

int OCF_TimeOutNotification(void* pvHandle, void* r_pvData)
{

	HOCSCCSUBSESSIONOBJECT hCCSubSession = (HOCSCCSUBSESSIONOBJECT)r_pvData;
	t_OCSAPIRetVal oReturnVal;

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	if(NULL == hCCSubSession)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," r_pvData is NULL");
		return 0;
	}


	oReturnVal = OCF_ExecStateMachine(hCCSubSession->hOCFContext, hCCSubSession, OCS_TCC_TIMER_EXPIRED, NULL);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return 0;
}


t_OCSAPIRetVal OCF_Idle_InitReqProcessingSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
                                                    t_OCSEvent     		oEvent_i,
                                                    void*              	pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFAppContext_io->hDiamStackContext;

	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
	t_OCSGrantedServiceUnit oGrantedUnit;
	HDIAMETERMESSAGE hDiamCCAMsg = NULL;
	unsigned int unTimerExpiryTime = 0;

	DMBase_MemsetZero((void*)&oCCA, sizeof(t_OCSCCA));
	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));
	DMBase_MemsetZero((void*)&oGrantedUnit, sizeof(t_OCSGrantedServiceUnit));
	oCCA.oCheckBalanceResult = OCS_NOT_USED;

	if(OCS_Charging_SCUR == hSubSessionObject_i->oChargingType)
	{
		t_OCSTariffResponse* poTariffResponse = (t_OCSTariffResponse*)pvData_i;


		oGrantedUnit.unCCTime = poTariffResponse->unValidUnits;
		oGrantedUnit.oCCMoney.oUnitValue.llValueDigits = poTariffResponse->unPrice;
		oGrantedUnit.oCCMoney.unCurrencyCode = 1;

		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
		oMultiServiceCC[0].oCCA.unValidityTime = poTariffResponse->unExpiryTime;
		oMultiServiceCC[0].oCCA.poGrantedServiceUnits = &oGrantedUnit;
		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_SUCCESS;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;
		oCCA.unResultCode = DIAMETER_SUCCESS;

		unTimerExpiryTime = 2 * poTariffResponse->unExpiryTime;

		oCCA.poRemainingBalance = &poTariffResponse->oRemainingBalance;
		oCCA.oLowBalanceIndication = poTariffResponse->oLowBalanceIndication;
	}
	else
	{
		//ECUR

		t_OCSPriceResponse* poPriceResponse	= (t_OCSPriceResponse*)pvData_i;

		oGrantedUnit.unCCTime = 100;

		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;

		oMultiServiceCC[0].oCCA.unValidityTime = poPriceResponse->unExpiryTime;
		oMultiServiceCC[0].oCCA.poGrantedServiceUnits = &poPriceResponse->oGrantedServiceUnit;
		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_SUCCESS;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;
		oCCA.unResultCode = DIAMETER_SUCCESS;

		unTimerExpiryTime = 2 * poPriceResponse->unExpiryTime;

		oCCA.poRemainingBalance = &poPriceResponse->oRemainingBalance;
		oCCA.oLowBalanceIndication = poPriceResponse->oLowBalanceIndication;
	}



	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}

	//printf("OK---%s\r\n", hSubSessionObject_i->oSessionID.ucString);
	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		printf("Failed---\r\n");
		//Delete Session Object
		return OCS_Retval_Failure;
	}

	hSubSessionObject_i->bProcessingInProgress = FALSE;
	hSubSessionObject_i->oSessionState = OCS_SESSION_STATE_OPEN;

	DMBase_Free(hDiamStackContext, (void**)&hSubSessionObject_i->poCCR);
	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);
	DMBase_StartTimer(hOCFAppContext_io->hDiamStackContext, unTimerExpiryTime,
			OCF_TimeOutNotification, (void*)hSubSessionObject_i, &hSubSessionObject_i->oTccTimerId);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}


t_OCSAPIRetVal OCF_Idle_InitReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFAppContext_io->hDiamStackContext;

	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
	HDIAMETERMESSAGE hDiamCCAMsg = NULL;
	t_OCSFixedLenUTF8String oSessonID;
	unsigned long long unllCCSubSessionId;


	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));

	if(OCS_Charging_SCUR == hSubSessionObject_i->oChargingType)
	{
//		t_OCSTariffResponse* poTariffResponse = (t_OCSTariffResponse*)pvData_i;

		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_RATING_FAILED;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;

		oCCA.unResultCode = DIAMETER_RATING_FAILED;
	}
	else
	{

	}

	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}


	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		//Delete Session Object
		return OCS_Retval_Failure;
	}

	DMBase_Free(hDiamStackContext, (void**)&hSubSessionObject_i->poCCR);
	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);

	//Delete Sesion Object

	DMBase_MemsetZero(&oSessonID, sizeof(t_OCSFixedLenUTF8String));
	OCS_CopyOCSUTF8String(&oSessonID, &hSubSessionObject_i->oSessionID);
	unllCCSubSessionId = hSubSessionObject_i->ullCCSubSessionId;
	OCF_RemoveSubSessionObject(hOCFAppContext_io, &oSessonID, unllCCSubSessionId);


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_Idle_EventReqProcessingSuccessful(HOCFCONTEXT 			hOCFAppContext_io,
													 HOCSCCSUBSESSIONOBJECT hSubSessionObject_i,
													 t_OCSEvent     		oEvent_i,
													 void*             		pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	t_OCSPriceResponse* poPriceResponse	= (t_OCSPriceResponse*)pvData_i;
	HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFAppContext_io->hDiamStackContext;
	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
	DMBase_MemsetZero((void*)&oCCA, sizeof(t_OCSCCA));
	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));
	oCCA.oCheckBalanceResult = OCS_NOT_USED;

	switch(poPriceResponse->oRequestSubType)
	{
		case OCS_AoC:
		case OCS_Reservation:
		case OCS_Debit:
		case OCS_Release:
			break;
		case OCS_Direct_Debiting:
		{
			if(OCS_UNSUED_VALUE != hSubSessionObject_i->poCCR->unServiceId)
			{
				oCCA.poGrantedServiceUnits = &poPriceResponse->oGrantedServiceUnit;
			}
			else
			{
				oMultiServiceCC[0].oCCA.poGrantedServiceUnits = &poPriceResponse->oGrantedServiceUnit;
			}
		}
			break;
		case OCS_Refund_Account:
			oCCA.poCostInfo = &poPriceResponse->oCostInfo;
			break;
		case OCS_Check_Balance:
			oCCA.oCheckBalanceResult = poPriceResponse->oCheckBalanceResult;
			break;
		case OCS_Price_Enquiry:
			oCCA.poCostInfo = &poPriceResponse->oCostInfo;
			break;
		case OCS_Invalid_Request_Type:
			break;
	}

	oCCA.poRemainingBalance = &poPriceResponse->oRemainingBalance;
	oCCA.oLowBalanceIndication = poPriceResponse->oLowBalanceIndication;

	if(OCS_UNSUED_VALUE != hSubSessionObject_i->poCCR->unServiceId)
	{
		oCCA.unResultCode = poPriceResponse->unRatingFuncResultCode;
	}
	else
	{
		oCCA.unResultCode = DIAMETER_SUCCESS;
		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
		oMultiServiceCC[0].oCCA.unResultCode = poPriceResponse->unRatingFuncResultCode;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;
	}




	HDIAMETERMESSAGE hDiamCCAMsg = NULL;

	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}

	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		//Delete Session Object
		return OCS_Retval_Failure;
	}


	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);
	DMBase_Free(hDiamStackContext, (void**)&hSubSessionObject_i->poCCR);
	OCF_RemoveSubSessionObject(hOCFAppContext_io, &poPriceResponse->oSessionId,
												poPriceResponse->ullCCSubSessionId);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}


t_OCSAPIRetVal OCF_Idle_EventReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_Open_UpdateReqProcessingSuccessful(HOCFCONTEXT 			 hOCFAppContext_io,
													  HOCSCCSUBSESSIONOBJECT hSubSessionObject_i,
													  t_OCSEvent     		 oEvent_i,
													  void*             	 pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT 	hDiamStackContext = hOCFAppContext_io->hDiamStackContext;
	t_OCSTariffResponse* 	poTariffResponse = (t_OCSTariffResponse*)pvData_i;
	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
	t_OCSGrantedServiceUnit oGrantedUnit;
	HDIAMETERMESSAGE hDiamCCAMsg = NULL;

	DMBase_MemsetZero((void*)&oCCA, sizeof(t_OCSCCA));
	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));
	DMBase_MemsetZero((void*)&oGrantedUnit, sizeof(t_OCSGrantedServiceUnit));

	oCCA.oCheckBalanceResult = OCS_NOT_USED;
	oGrantedUnit.unCCTime = poTariffResponse->unValidUnits;
	oGrantedUnit.oCCMoney.oUnitValue.llValueDigits = poTariffResponse->unPrice;
	oGrantedUnit.oCCMoney.unCurrencyCode = 1;
	oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
	oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
	oMultiServiceCC[0].oCCA.unValidityTime = poTariffResponse->unExpiryTime;
	oMultiServiceCC[0].oCCA.poGrantedServiceUnits = &oGrantedUnit;
	oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_SUCCESS;

	oCCA.poMultiServicesCC = oMultiServiceCC;
	oCCA.unMultiServiceCCCount = 1;
	oCCA.unResultCode = DIAMETER_SUCCESS;


	oCCA.poRemainingBalance = &poTariffResponse->oRemainingBalance;
	oCCA.oLowBalanceIndication = poTariffResponse->oLowBalanceIndication;

	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}

	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		//Delete Session Object
		return OCS_Retval_Failure;
	}

	DMBase_Free(hDiamStackContext, (void**)&hSubSessionObject_i->poCCR);
	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);
	DMBase_KillTimer(hOCFAppContext_io->hDiamStackContext, hSubSessionObject_i->oTccTimerId);
	hSubSessionObject_i->oTccTimerId = NULL;
	DMBase_StartTimer(hOCFAppContext_io->hDiamStackContext, 2 * poTariffResponse->unExpiryTime,
			OCF_TimeOutNotification, (void*)hSubSessionObject_i, &hSubSessionObject_i->oTccTimerId);

	hSubSessionObject_i->bProcessingInProgress = FALSE;
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}


t_OCSAPIRetVal OCF_Open_UpdateReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT 	hDiamStackContext = hOCFAppContext_io->hDiamStackContext;
//	t_OCSTariffResponse* 	poTariffResponse = (t_OCSTariffResponse*)pvData_i;
	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
	t_OCSGrantedServiceUnit oGrantedUnit;
	HDIAMETERMESSAGE hDiamCCAMsg = NULL;

	DMBase_MemsetZero((void*)&oCCA, sizeof(t_OCSCCA));
	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));
	DMBase_MemsetZero((void*)&oGrantedUnit, sizeof(t_OCSGrantedServiceUnit));

	if(OCS_Charging_SCUR == hSubSessionObject_i->oChargingType)
	{
//		t_OCSTariffResponse* poTariffResponse = (t_OCSTariffResponse*)pvData_i;

	//	oGrantedUnit.unCCTime = 1000;
		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
	//	oMultiServiceCC[0].oCCA.unValidityTime = poTariffResponse->unExpiryTime;
	//	oMultiServiceCC[0].oCCA.poGrantedServiceUnits = &oGrantedUnit;
		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_RATING_FAILED;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;

		oCCA.unResultCode = DIAMETER_RATING_FAILED;
	}
	else
	{

	}

	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}

	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		//Delete Session Object
		return OCS_Retval_Failure;
	}

	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);

	DMBase_KillTimer(hOCFAppContext_io->hDiamStackContext, hSubSessionObject_i->oTccTimerId);
	hSubSessionObject_i->oTccTimerId = NULL;

//	OCF_RemoveSubSessionObject(hOCFAppContext_io, &poTariffResponse->oSessionId, poTariffResponse->ullCCSubSessionId);
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_Open_TerminateReqProcessingSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT 	hDiamStackContext = hOCFAppContext_io->hDiamStackContext;

	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
//	t_OCSGrantedServiceUnit oGrantedUnit;
	HDIAMETERMESSAGE hDiamCCAMsg = NULL;

	DMBase_MemsetZero((void*)&oCCA, sizeof(t_OCSCCA));
	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));
//	DMBase_MemsetZero((void*)&oGrantedUnit, sizeof(t_OCSGrantedServiceUnit));

	if(OCS_Charging_SCUR == hSubSessionObject_i->oChargingType)
	{
		t_OCSTariffResponse* poTariffResponse = (t_OCSTariffResponse*)pvData_i;
		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_SUCCESS;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;
		oCCA.unResultCode = DIAMETER_SUCCESS;

		oCCA.poRemainingBalance = &poTariffResponse->oRemainingBalance;
		oCCA.oLowBalanceIndication = poTariffResponse->oLowBalanceIndication;

	}
	else
	{

		t_OCSPriceResponse* poPriceResponse	= (t_OCSPriceResponse*)pvData_i;

		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;

		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_SUCCESS;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;
		oCCA.unResultCode = DIAMETER_SUCCESS;

		oCCA.poRemainingBalance = &poPriceResponse->oRemainingBalance;
		oCCA.oLowBalanceIndication = poPriceResponse->oLowBalanceIndication;
	}

	oCCA.oCheckBalanceResult = OCS_NOT_USED;

	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}

	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		//Delete Session Object
		return OCS_Retval_Failure;
	}

	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);
	DMBase_Free(hDiamStackContext, (void**)&hSubSessionObject_i->poCCR);
	DMBase_KillTimer(hDiamStackContext, hSubSessionObject_i->oTccTimerId);

	t_OCSFixedLenUTF8String oSessonID;
	unsigned long long unllCCSubSessionId;

	DMBase_MemsetZero(&oSessonID, sizeof(t_OCSFixedLenUTF8String));
	OCS_CopyOCSUTF8String(&oSessonID, &hSubSessionObject_i->oSessionID);
	unllCCSubSessionId = hSubSessionObject_i->ullCCSubSessionId;

	OCF_RemoveSubSessionObject(hOCFAppContext_io, &oSessonID, unllCCSubSessionId);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_Open_TerminateReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT 	hDiamStackContext = hOCFAppContext_io->hDiamStackContext;

	t_OCSCCA oCCA;
	t_OCSMultiServicesCC oMultiServiceCC[1];
	HDIAMETERMESSAGE hDiamCCAMsg = NULL;
//	t_OCSGrantedServiceUnit oGrantedUnit;

	DMBase_MemsetZero((void*)&oCCA, sizeof(t_OCSCCA));
	DMBase_MemsetZero((void*)oMultiServiceCC, sizeof(t_OCSMultiServicesCC));


	if(OCS_Charging_SCUR == hSubSessionObject_i->oChargingType)
	{
//		t_OCSTariffResponse* 	poTariffResponse = (t_OCSTariffResponse*)pvData_i;
	//	oGrantedUnit.unCCTime = 1000;
		oMultiServiceCC[0].unRatingGroup = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unRatingGroup;
		oMultiServiceCC[0].unServiceId = hSubSessionObject_i->poCCR->oMultiServicesCC[0].unServiceId;
	//	oMultiServiceCC[0].oCCA.unValidityTime = poTariffResponse->unExpiryTime;
	//	oMultiServiceCC[0].oCCA.poGrantedServiceUnits = &oGrantedUnit;
		oMultiServiceCC[0].oCCA.unResultCode = DIAMETER_RATING_FAILED;

		oCCA.poMultiServicesCC = oMultiServiceCC;
		oCCA.unMultiServiceCCCount = 1;
		oCCA.unResultCode = DIAMETER_RATING_FAILED;
	}
	else
	{

	}

	if(OCS_Retval_Success != OCF_CreatCCADiamMsg(hOCFAppContext_io, hSubSessionObject_i->poCCR, &oCCA, &hDiamCCAMsg))
	{
		//Delete session object
		return OCS_Retval_Failure;
	}

	if(DMBase_Retval_Success != DMBase_SendMessageToPeer(hDiamStackContext, hSubSessionObject_i->hPeer, hDiamCCAMsg))
	{
		//Delete Session Object
		return OCS_Retval_Failure;
	}

	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiamCCAMsg);
	DMBase_Free(hDiamStackContext, (void**)&hSubSessionObject_i->poCCR);

	DMBase_KillTimer(hOCFAppContext_io->hDiamStackContext, hSubSessionObject_i->oTccTimerId);

	OCF_RemoveSubSessionObject(hOCFAppContext_io, &hSubSessionObject_i->oSessionID, hSubSessionObject_i->ullCCSubSessionId);


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_Open_TccTimerExpires(HOCFCONTEXT 	hOCFAppContext_io,
										HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	t_OCSFixedLenUTF8String oSessonID;
	unsigned long long unllCCSubSessionId;

	printf("Timer Expires %s\r\n", hSubSessionObject_i->oSessionID.ucString);

	DMBase_MemsetZero(&oSessonID, sizeof(t_OCSFixedLenUTF8String));
	OCS_CopyOCSUTF8String(&oSessonID, &hSubSessionObject_i->oSessionID);
	unllCCSubSessionId = hSubSessionObject_i->ullCCSubSessionId;
	OCF_RemoveSubSessionObject(hOCFAppContext_io, &oSessonID, unllCCSubSessionId);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}
