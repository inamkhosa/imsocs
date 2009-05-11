
#include "ocf_session.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_utility.h"
#include "ocf_msgextractor.h"

extern	int	g_nDiamBaseTraceID ;

t_OCSAPIRetVal	OCF_InsertSessionObject(	HOCFCONTEXT 			 hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i,
										void* 					 poSessionObj_i)
{
    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext  ||
    		NULL == hAppContext_i->hSessionTable ||NULL == poSessionID_i ||
    		NULL == poSessionObj_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null pointer not allowed\n");
        return OCS_Retval_Null_Pointer;
    }

    t_DiamHashTableKey oHashTableKey;
    DMBase_SetStringInHashTableKey(&oHashTableKey, poSessionID_i->ucString);

    if(DMBase_Retval_Success != DMBase_HashTableInsert(	hAppContext_i->hDiamStackContext,
														hAppContext_i->hSessionTable,
														poSessionObj_i,
														&oHashTableKey))
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Session Table Insertion Failed\n");
        return OCS_Retval_Failure;
    }

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}

t_OCSAPIRetVal	OCF_InsertSubSessionObject(	HOCFCONTEXT 		 	 hAppContext_i,
											t_OCSFixedLenUTF8String* poSessionID_i,
											unsigned long long		 ullCCSubSessionId_i,
											void* 					 poSessionObj_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSSESSIONOBJECT hSession = NULL;

	if(OCS_Retval_Success != OCF_LookUpSessionObject(hAppContext_i, poSessionID_i, (void**)&hSession))
	{

		if(DMBase_Retval_Success != DMBase_Malloc(hAppContext_i->hDiamStackContext,(void**)&hSession, sizeof(t_OCSSessionObjet)))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Failed to create Session Object\n");
			return OCS_Retval_Failure;
		}

		OCS_CopyOCSUTF8String(&hSession->oSessionID, poSessionID_i);
		hSession->ullCCSubSessionId = ullCCSubSessionId_i;

		if(OCS_Retval_Success != OCF_InsertSessionObject(hAppContext_i, poSessionID_i, hSession))
		{
			DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&hSession);
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Failed to insert Session Object\n");
			return OCS_Retval_Failure;
		}
	}

	DMBase_SList* poSessionList = hSession->poSubSessionList;
	HOCSCCSUBSESSIONOBJECT hSubSession;
	while(NULL != poSessionList)
	{
		hSubSession = NULL;
		DMBase_SListGetNodeData(hAppContext_i->hDiamStackContext, (void*)poSessionList, (void**)&hSubSession);
		if(ullCCSubSessionId_i == hSubSession->ullCCSubSessionId)
		{
			return OCS_Retval_Failure;
		}
		DMBase_SListGetNextNode(hAppContext_i->hDiamStackContext, poSessionList, &poSessionList);
	}

	DMBase_AppendSList(hAppContext_i->hDiamStackContext, &hSession->poSubSessionList, poSessionObj_i);
	hSession->unSubSessionCount++;

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}



t_OCSAPIRetVal OCF_LookUpSessionObject(	HOCFCONTEXT 			 hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i,
										void** 					 ppoSessionObj_o)
{

	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext  ||
    		NULL == hAppContext_i->hSessionTable ||NULL == poSessionID_i ||
    		NULL == ppoSessionObj_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null pointer not allowed\n");
        return OCS_Retval_Null_Pointer;
    }

    t_DiamHashTableKey oHashTableKey;
    DMBase_SetStringInHashTableKey(&oHashTableKey, poSessionID_i->ucString);

    if ( DMBase_Retval_Not_Found == DMBase_HashTableSearch(hAppContext_i->hDiamStackContext,hAppContext_i->hSessionTable,
															&oHashTableKey, (void**)ppoSessionObj_o ))
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Session Object Not Found\n");
        return OCS_Retval_Not_Found;
    }

    DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Leaving\n");
    return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_RemoveSessionObject(	HOCFCONTEXT 	hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

    t_DiamHashTableKey oHashTableKey;
    DMBase_SetStringInHashTableKey(&oHashTableKey, poSessionID_i->ucString);

//    printf("%d %s\r\n", strlen((char*)poSessionID_i->ucString), poSessionID_i->ucString);
//    printf("%d %s\r\n", oHashTableKey.unKeyLength, oHashTableKey.puchKey);

    if ( DMBase_Retval_Success != DMBase_HashTableRemove(hAppContext_i->hDiamStackContext,hAppContext_i->hSessionTable,
															&oHashTableKey ))
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Failed to remove Session Object\n");
        return OCS_Retval_Not_Found;
    }

	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Leaving\n");
    return OCS_Retval_Success;
}


t_OCSAPIRetVal OCF_LookUpSubSessionObject(HOCFCONTEXT 			 hAppContext_i,
										  t_OCSFixedLenUTF8String* poSessionID_i,
										  unsigned long long	   ullCCSubSessionId,
										  void** 					 ppoSessionObj_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSSESSIONOBJECT	hSession = NULL;
	HOCSCCSUBSESSIONOBJECT hSubSession = NULL;

	if(OCS_Retval_Success != OCF_LookUpSessionObject(hAppContext_i, poSessionID_i, (void**)&hSession))
	{
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Failed to remove Session Object\n");
        return OCS_Retval_Not_Found;
	}

	DMBase_SList* poSessionList = hSession->poSubSessionList;


	while(NULL != poSessionList)
	{
		DMBase_SListGetNodeData(hAppContext_i->hDiamStackContext, (void*)poSessionList, (void**)&hSubSession);
		if(ullCCSubSessionId == hSubSession->ullCCSubSessionId)
		{
			*ppoSessionObj_o = (void*)hSubSession;
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
			return OCS_Retval_Success;
		}
		DMBase_SListGetNextNode(hAppContext_i->hDiamStackContext, poSessionList, &poSessionList);
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Not_Found;
}


t_OCSAPIRetVal OCF_RemoveSubSessionObject(	HOCFCONTEXT 	hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i,
										unsigned long long 		ullCCSubSessionId_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSSESSIONOBJECT	hSession = NULL;
	HOCSCCSUBSESSIONOBJECT hSubSession = NULL;

	if(OCS_Retval_Success != OCF_LookUpSessionObject(hAppContext_i, poSessionID_i, (void**)&hSession))
	{
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Failed to remove Session Object\n");
        return OCS_Retval_Not_Found;
	}

	DMBase_SList* poSessionList = hSession->poSubSessionList;

	while(NULL != poSessionList)
	{
		DMBase_SListGetNodeData(hAppContext_i->hDiamStackContext, (void*)poSessionList, (void**)&hSubSession);
		if(ullCCSubSessionId_i == hSubSession->ullCCSubSessionId)
		{
			break;
		}
		DMBase_SListGetNextNode(hAppContext_i->hDiamStackContext, poSessionList, &poSessionList);
	}
	//printf("\r\n", hSubSession-);

	if(NULL == poSessionList)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
	    return OCS_Retval_Not_Found;
	}

	DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&hSubSession);
	DMBase_SListRemove(hAppContext_i->hDiamStackContext, &hSession->poSubSessionList, poSessionList, NULL);
	hSession->unSubSessionCount--;

	if(0 == hSession->unSubSessionCount)
	{
		if(OCS_Retval_Success != OCF_RemoveSessionObject(hAppContext_i, poSessionID_i))
		{
			//printf("+++++++++++++++++++++++++++++++++++++++++Failed to remove\r\n");
		}
		DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&hSession);
	}
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}


t_OCSAPIRetVal OCF_CreateSubSessionObject(	HOCFCONTEXT				hOCFContext_io,
											HOCSCCSUBSESSIONOBJECT* phSessionObject_io,
											t_OCSFixedLenUTF8String* poSessionID,
											unsigned long long 		ullSubSessionId_i,
											HPEERTABLEENTRY			hPeer_i,
											t_OCSSessionState		oSessionState_i,
											t_OCSCCR*				poCCR_i,
											t_OCSChargingType		oChargingType,
											_Bool					bProcessingInProgress_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

	HOCSCCSUBSESSIONOBJECT hSessionObject = NULL;

	if(DMBase_Retval_Success != DMBase_Malloc(hOCFContext_io->hDiamStackContext,
												(void**)&hSessionObject,
												sizeof(t_OCSCCSubSessionObjet)))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to allocate memory\r\n");
		return OCS_Retval_Failure;
	}

	hSessionObject->hOCFContext = hOCFContext_io;
	OCS_CopyOCSUTF8String(&hSessionObject->oSessionID, poSessionID);
	hSessionObject->ullCCSubSessionId = ullSubSessionId_i;
	hSessionObject->hPeer = hPeer_i;
	hSessionObject->oSessionState = oSessionState_i;
	hSessionObject->poCCR = poCCR_i;
	hSessionObject->oChargingType = oChargingType;
	hSessionObject->bProcessingInProgress = bProcessingInProgress_i;

	*phSessionObject_io = hSessionObject;

	//printf("Pointer = %p\r\n", hSessionObject->poCCR);
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return OCS_Retval_Success;
}


