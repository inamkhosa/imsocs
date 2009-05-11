
#include "ocsdb.h"
#include "dmbase_utility.h"

#define DUMP_TARIFF_REQUEST 1
#define DUMP_PRICE_REQUEST	1


extern int g_nDiamBaseTraceID ;

AMPS_MODULE_HANDLE	onlinecharg_db_ModuleInit(AMPS_HANDLE hAMPS_HANDLE, void* pvUserData_i)
{

	printf("\n Initializing OCS DB Module \r\n");


	t_ocs_db_config_params*	poOCS_DBConfParams = (t_ocs_db_config_params*) pvUserData_i;
	HDIAMETERSTACKCONTEXT	hDiamStackContext = poOCS_DBConfParams->hDiameterStackContext;
	HOCSDBCONTEXT hOCSDBContext = NULL;


	DMBase_Malloc(hDiamStackContext, (void**)&hOCSDBContext, sizeof(t_OCSDBContext));
	hOCSDBContext->hDiamStackContext = hDiamStackContext;
	hOCSDBContext->poOCS_DBConfParams = poOCS_DBConfParams;


	//registering for events

    OCSDB_Init(hOCSDBContext);
/*
	if ( Db_HSS_Retval_Success != db_hss_RegisterForCxEvents(g_hDB_HSSContext))
	{
        printf("db_hss_RegisterForCxEvents failed , Leaving.\n");
        return NULL;
    }
*/

	return (AMPS_MODULE_HANDLE)hOCSDBContext;


}



void onlinecharg_db_ModuleCleanup(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_MODULE_HANDLE r_hAMPS_MODULE_HANDLE)
{

}


void* OCSDB_ThreadInit(AMPS_HANDLE r_hAMPSHandle,
						AMPS_UIOAGENT_HANDLE r_hUIOAgentsHandle, void* hOCSDBContext_io)
{
	HOCSDBTHREADCONTEXT	hOCSDBThreadCtxt = NULL;
	HOCSDBCONTEXT		hOCSDBContext = (HOCSDBCONTEXT)hOCSDBContext_io;

	printf("Entering db_HSS_ThreadInit .\n");

	if ( DMBase_Retval_Success != DMBase_Malloc(hOCSDBContext->hDiamStackContext,
									(void**)&hOCSDBThreadCtxt, sizeof(t_OCSDBThreadContext)))


    {
        printf("OCSDB_ThreadInit:  DMBase_Malloc failed.\n");
        return NULL;
    }

	hOCSDBThreadCtxt->hOCSDBMainContext = hOCSDBContext;
	hOCSDBThreadCtxt->hUnitIOAgent = r_hUIOAgentsHandle;


    //open connection to DB
	if ( DMBase_Retval_Success != DMBASE_DBEngin_Init( hOCSDBContext->hDiamStackContext,
						 hOCSDBContext->poOCS_DBConfParams->oDBMSConfigParams.pucHostName,
						 hOCSDBContext->poOCS_DBConfParams->oDBMSConfigParams.pucUserName,
						 hOCSDBContext->poOCS_DBConfParams->oDBMSConfigParams.pucPassword,
						 hOCSDBContext->poOCS_DBConfParams->oDBMSConfigParams.pucDatabaseName,
						 &(hOCSDBThreadCtxt->hDBEngineHandle)))
	{
		printf(" DMBASE_DBEngin_Init failed for current thread \r\n");
	}

	printf("Leaving db_HSS_ThreadInit .\n");

    return hOCSDBThreadCtxt;
}

void OCSDB_ThreadCleanup(void* hOCSDBThreadCtxt_io)
{
	printf("Entering db_HSS_ThreadCleanup .\n");

	HOCSDBTHREADCONTEXT	hOCSDBThreadCtxt = hOCSDBThreadCtxt_io;
	HOCSDBCONTEXT		hOCSDBContext = hOCSDBThreadCtxt->hOCSDBMainContext;

//	HDBHSSTHREADCONTEXT	hDBHSS_ThreadCtxt = hDBHSS_ThreadCtxt_io;
//	HDBHSSCONTEXT		hdbHSSContext = hDBHSS_ThreadCtxt->hDBHSSMainContext;

//	DMBASE_DBEngin_Cleanup(hdbHSSContext->hDiamStackContext,hDBHSS_ThreadCtxt->hDBEngineHandle);
	DMBase_Free(hOCSDBContext->hDiamStackContext,(void**)hOCSDBThreadCtxt);

	printf("Leaving db_HSS_ThreadCleanup .\n");
}



t_OCSAPIRetVal OCSDB_Init(HOCSDBCONTEXT hOCSDBContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

	HDIAMETERSTACKCONTEXT	hDiamStackContext = NULL;
	hDiamStackContext = hOCSDBContext_io->hDiamStackContext;
//	AMPS_IOAGENTS_HANDLE hIOAgentsHandle = hOCSDBContext_io->hIOAgentsHandle;

	hOCSDBContext_io->hIOAgentsHandle = AMPS_IOAgentsInit(hDiamStackContext->hFrameworkHandle,
														DB_OCS_DB_NUMBER_OF_THREADS,
														NULL, OCSDB_ThreadInit,
														OCSDB_ThreadCleanup,
														hOCSDBContext_io);

    if (NULL == hOCSDBContext_io->hIOAgentsHandle)
    {
        printf("AMPS_IOAgentsInit failed , Leaving.\n");
        return OCS_Retval_Failure;
    }

    //Registration of events

	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hOCSDBContext_io->hIOAgentsHandle, OCS_DB_PRICE_REQUEST_EVENT, OCSDB_RequestHandler))
	{
		printf("AMPS_IOAgentsRegEvt failed for OCS_DB_PRICE_REQUEST_EVENT \r\n");
		return OCS_Retval_Failure;
	}

	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hOCSDBContext_io->hIOAgentsHandle, OCS_DB_TARIFF_REQUEST_EVENT, OCSDB_RequestHandler))
	{
		printf("AMPS_IOAgentsRegEvt failed for OCS_DB_TARIFF_REQUEST_EVENT \r\n");
		return OCS_Retval_Failure;
	}


    t_DiamOctetString oOCSDBQueryFile = {0};
    char              pchCWD[1024]= {0};
	getcwd(pchCWD,1024);


	DMBase_CopyCharArrayToDiamString(&oOCSDBQueryFile,DATADIR);
	DMBase_AppendCharStringToDiamString(&oOCSDBQueryFile, "db_ocs_queries.xml");


	if ( DappCommon_Retval_Success != DappCommon_LoadDBQueryConfigFromFile
										 (oOCSDBQueryFile,
										 hOCSDBContext_io->oDBQueryMapForOCS,
										 DB_OCS_QUERY_MAP_SIZE))
	{
		printf("DappCommon_LoadDBQueryConfigFromFile failed \r\n");
	}
	else
	{
		printf("DappCommon_LoadDBQueryConfigFromFile succeeded for OCS DB Query config file \r\n");
	}

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}


void  OCSDB_RequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
								void* r_pvEventHandle, void* r_pvEventPayload)
{
//	printf("DB Request\r\n");

	HOCSDBTHREADCONTEXT hOCSDBThreadCtxt = (HOCSDBTHREADCONTEXT) r_pvEventData;

	int unEventType = 0;
    DMBase_EvtSysGetEventType(hOCSDBThreadCtxt->hOCSDBMainContext->hDiamStackContext, r_pvEventHandle, &unEventType);

    switch(unEventType)
    {
		case OCS_DB_PRICE_REQUEST_EVENT:
			OCSDB_ProcessPriceRequest(hOCSDBThreadCtxt, (t_OCSPriceRequest*)r_pvEventPayload);
			break;
		case OCS_DB_TARIFF_REQUEST_EVENT:
			OCSDB_ProcessTariffRequest(hOCSDBThreadCtxt, (t_OCSTariffRequest*)r_pvEventPayload);
			break;
    }
}


t_OCSAPIRetVal OCSDB_ProcessPriceRequest(HOCSDBTHREADCONTEXT hOCSDBThreadCtxt_io, t_OCSPriceRequest* poPriceRequest_i)
{

	t_OCSPriceResponse oPriceResponse;
	DMBase_MemsetZero(&oPriceResponse, sizeof(t_OCSPriceResponse));
	memcpy((void*)&oPriceResponse.oSessionId, &poPriceRequest_i->oSessionId, sizeof(t_OCSFixedLenUTF8String));
	oPriceResponse.oResult = OCS_SUCCESS;
	oPriceResponse.ullCCSubSessionId = poPriceRequest_i->ullCCSubSessionId;
	oPriceResponse.oRequestSubType = poPriceRequest_i->oRequestSubType ;
	oPriceResponse.unRatingFuncResultCode = 10;

#ifdef DUMP_PRICE_REQUEST
	OCSDB_DumpPriceRequest(poPriceRequest_i);
#endif

	switch(oPriceResponse.oRequestSubType)
	{
		case OCS_AoC:
			break;
		case OCS_Reservation:
			memcpy(&oPriceResponse.oGrantedServiceUnit.oCCMoney, &poPriceRequest_i->oReqServiceUnit.oCCMoney, sizeof(t_OCSCCMoney));
			break;
		case OCS_Debit:
		case OCS_Release:
			break;
		case OCS_Direct_Debiting:
			oPriceResponse.oGrantedServiceUnit.oCCMoney.oUnitValue.llValueDigits = 50;
			oPriceResponse.oGrantedServiceUnit.oCCMoney.oUnitValue.nExponent = 0;
			oPriceResponse.oGrantedServiceUnit.oCCMoney.unCurrencyCode = 1;
			break;
		case OCS_Refund_Account:
			oPriceResponse.oCostInfo.oUnitValue.llValueDigits = 50;
			oPriceResponse.oCostInfo.oUnitValue.nExponent = 0;
			oPriceResponse.oCostInfo.unCurrencyCode = 1;
			strcpy(oPriceResponse.oCostInfo.oCostUnit.ucString, "Rs.");
			oPriceResponse.oCostInfo.oCostUnit.unLength = 3;
			break;
		case OCS_Check_Balance:
			oPriceResponse.oCheckBalanceResult = OCS_ENOUGH_CREDIT;
			break;
		case OCS_Price_Enquiry:
			oPriceResponse.oCostInfo.oUnitValue.llValueDigits = 50;
			oPriceResponse.oCostInfo.oUnitValue.nExponent = 0;
			oPriceResponse.oCostInfo.unCurrencyCode = 1;
			strcpy(oPriceResponse.oCostInfo.oCostUnit.ucString, "Rs.");
			oPriceResponse.oCostInfo.oCostUnit.unLength = 3;
			break;
		case OCS_Invalid_Request_Type:
			break;
	}

	oPriceResponse.oRemainingBalance.oUnitValue.llValueDigits = 500;
	oPriceResponse.oRemainingBalance.oUnitValue.nExponent = 0;
	oPriceResponse.oRemainingBalance.unCurrencyCode = 1;

	oPriceResponse.oLowBalanceIndication = OCS_NOT_APPLICABLE;

	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hOCSDBThreadCtxt_io->hUnitIOAgent,
			OCS_DB_PRICE_RESPONSE_EVENT, (void* ) &oPriceResponse, sizeof(t_OCSPriceResponse) )
		)
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for OCS_DB_PRICE_RESPONSE_EVENT \n\n ");
	}

	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCSDB_ProcessTariffRequest(HOCSDBTHREADCONTEXT hOCSDBThreadCtxt_io, t_OCSTariffRequest* poTariffRequest_i)
{
//	printf("DB Module\r\n");
	t_OCSTariffResponse oTariffResponse;
	HOCSDBCONTEXT hOCSDBContext = hOCSDBThreadCtxt_io->hOCSDBMainContext;
	DMBase_MemsetZero(&oTariffResponse, sizeof(t_OCSTariffResponse));
	char cSQLQuery[2048] = {0};
	char ucQueryStr[2048] = {0};
	char cUserName[16] = "mansoor";
	char cCallingNo[16] = "";
	char cCalledNo[16] = "92051511111";
	char cSoftswitchFlag[16] = {0};
	int nAccNo = 0;
	char cAuthConfID[32] = {0};
	int nServiceID = 0;
	char cSessionID[16] = "Seeeion2";
	int nUnitsToBeReserved = 10;
	int nAuthCriteria = 0;
	char cAccessNumber[16] = {0};
	int nNoOfDecimalPoints = 2;
	int nRetCode ;
	DMBASE_DBEngineResult oDBResult;

#ifdef DUMP_TARIFF_REQUEST
	OCSDB_DumpTariffRequest(poTariffRequest_i);
#endif


	DMBase_MemsetZero(&oDBResult, sizeof(DMBASE_DBEngineResult));
	sprintf(cAuthConfID, "%d-%d", rand(), rand());



    DMBase_CopyDiamStrToCharArray(ucQueryStr, hOCSDBContext->oDBQueryMapForOCS[1].oQueryString);

	sprintf(cSQLQuery, ucQueryStr, poTariffRequest_i->oUserName.ucString,
			 cCallingNo, cCalledNo, cSoftswitchFlag, nAccNo, cAuthConfID, nServiceID,
			 poTariffRequest_i->oSessionId.ucString, nUnitsToBeReserved, nAuthCriteria,
			cAccessNumber, nNoOfDecimalPoints);

//	printf("%s\r\n", cSQLQuery);
	if(DMBase_Retval_Success == DMBASE_DBEngineQuery(hOCSDBThreadCtxt_io->hOCSDBMainContext->hDiamStackContext,
									hOCSDBThreadCtxt_io->hDBEngineHandle, cSQLQuery, strlen(cSQLQuery), &oDBResult))
	{
		DMBase_MemsetZero(&ucQueryStr, 2048);
		DMBase_CopyDiamStrToCharArray(ucQueryStr, hOCSDBContext->oDBQueryMapForOCS[2].oQueryString);
		sprintf(cSQLQuery, ucQueryStr);

		if(DMBase_Retval_Success == DMBASE_DBEngineQuery(hOCSDBThreadCtxt_io->hOCSDBMainContext->hDiamStackContext,
										hOCSDBThreadCtxt_io->hDBEngineHandle, cSQLQuery, strlen(cSQLQuery), &oDBResult))
		{

			/*printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n",
					oDBResult.ppoAMPSDBEngineTable[0][0].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][1].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][2].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][3].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][4].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][5].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][6].pchField,
					oDBResult.ppoAMPSDBEngineTable[0][7].pchField);*/

			printf("Query Successfully executed\r\n\r\n");/**/
			nRetCode = atoi(oDBResult.ppoAMPSDBEngineTable[0][6].pchField);
			if(0 == nRetCode)
			{

				oTariffResponse.oResult = OCS_SUCCESS;
				oTariffResponse.unExpiryTime = 10000;
				oTariffResponse.unValidUnits = atoi(oDBResult.ppoAMPSDBEngineTable[0][3].pchField);
				oTariffResponse.unPrice = atoi(oDBResult.ppoAMPSDBEngineTable[0][2].pchField);;
			}
			else if(1 == nRetCode)
			{
				oTariffResponse.oResult = OCS_FAILURE;
			}

			DMBASE_DBEngineFreeResults(hOCSDBThreadCtxt_io->hOCSDBMainContext->hDiamStackContext, hOCSDBThreadCtxt_io->hDBEngineHandle, &oDBResult);
		}
	}
	else
	{
		printf("Query Failed\r\n");
		oTariffResponse.oResult = OCS_FAILURE;
	}


	memcpy((void*)&oTariffResponse.oSessionId, &poTariffRequest_i->oSessionId, sizeof(t_OCSFixedLenUTF8String));
	oTariffResponse.ullCCSubSessionId = poTariffRequest_i->ullCCSubSessionId;

	oTariffResponse.oRemainingBalance.oUnitValue.llValueDigits = 500;
	oTariffResponse.oRemainingBalance.oUnitValue.nExponent = 0;
	oTariffResponse.oRemainingBalance.unCurrencyCode = 1;

	oTariffResponse.oLowBalanceIndication = OCS_NOT_APPLICABLE;

	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hOCSDBThreadCtxt_io->hUnitIOAgent,
			OCS_DB_TARIFF_RESPONSE_EVENT, (void* ) &oTariffResponse, sizeof(t_OCSTariffResponse) )
		)
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for OCS_DB_TARIFF_RESPONSE_EVENT \n\n ");
	}

	return OCS_Retval_Success;
}


void OCSDB_DumpPriceRequest(t_OCSPriceRequest* poPriceRequest_i)
{

	printf("Session-Id : %s\r\n", poPriceRequest_i->oSessionId.ucString);
	printf("User Name : %s\r\n", poPriceRequest_i->oUserName.ucString);
	printf("Requested Unit : %llu\r\n", poPriceRequest_i->oReqServiceUnit.oCCMoney.oUnitValue.llValueDigits);
	printf("Service-Id : %d\r\n", poPriceRequest_i->unServiceId);
	printf("Request Sub Type : %u\r\n ", poPriceRequest_i->oRequestSubType);

}


void OCSDB_DumpTariffRequest(t_OCSTariffRequest* poTariffRequest_i)
{

	printf("Session-Id : %s\r\n", poTariffRequest_i->oSessionId.ucString);
	printf("User Name : %s\r\n", poTariffRequest_i->oUserName.ucString);
	printf("Requested Unit : %u\r\n", poTariffRequest_i->oReqServiceUnit.unCCTime);
	printf("Service-Id : %d\r\n", poTariffRequest_i->unServiceId);
	printf("Request Sub Type : %u\r\n ", poTariffRequest_i->oRequestSubType);
}
