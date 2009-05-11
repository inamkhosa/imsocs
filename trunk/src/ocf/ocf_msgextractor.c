
#include "ocf_msgextractor.h"
#include "dmbase_parser.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_utility.h"

extern int g_nDiamBaseTraceID ;

t_OCSAPIRetVal OCF_ExtractCCRAttributes(HOCFCONTEXT hOCFContext_io, HDIAMETERMESSAGE hDiamMsg_i, t_OCSCCR* poCCR_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

    DMBase_SList *poAVPList = hDiamMsg_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_io->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    DMBase_GetDiaMsgFlagOctetFromDiamMsg(hDiamStackContext, hDiamMsg_i, &poCCR_o->ucFlags);
    DMBase_GetHopByHopIDFromDiamMsg(hDiamStackContext, hDiamMsg_i, &poCCR_o->unHopByHopId);
    DMBase_GetEndToEndIDFromDiamMsg(hDiamStackContext, hDiamMsg_i, &poCCR_o->unEndToEndId);
    poCCR_o->unServiceId = OCS_UNSUED_VALUE;
    poCCR_o->unTerminationCause = OCS_UNSUED_VALUE;
    poCCR_o->ullCCSubSessionId = OCS_UNSUED_VALUE;

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return OCS_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
			case DMBASE_AVP_SESSION_ID:
				OCS_CopyDiamUTF8String(&poCCR_o->oSessionId, ((t_DiamUTF8String*)hAVP->pData));
				break;
			case OCS_AVP_SERVICE_CONTEXT_ID:
				OCS_CopyDiamUTF8String(&poCCR_o->oServiceContextId, ((t_DiamUTF8String*)hAVP->pData));
				break;
			case OCS_AVP_CC_REQUEST_TYPE:
				poCCR_o->oCCRequestType = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
				break;
			case OCS_AVP_CC_REQUEST_NUMBER:
				poCCR_o->unCCRequestNum = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
				break;
			case DMBASE_AVP_USER_NAME:
				OCS_CopyDiamUTF8String(&poCCR_o->oUserName, ((t_DiamUTF8String*)hAVP->pData));
				break;
			case OCS_AVP_SUB_SESSION_ID:
				poCCR_o->ullCCSubSessionId = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
			case OCS_AVP_SUBSCRIPTION_ID:
				break;
			case OCS_AVP_SERVICE_IDENTIFIER:
				poCCR_o->unServiceId = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
				break;
			case DMBASE_AVP_TERMINATION_CAUSE:
				poCCR_o->unTerminationCause = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
				break;
			case OCS_AVP_REQUESTED_ACTION:
				poCCR_o->unRequestedAction = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
				break;
			case OCS_AVP_MULTIPLE_SERVICES_INDICATOR:
				poCCR_o->unMultiServiceIndicator = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
				break;
			case OCS_AVP_MULTIPLE_SERVICES_CREDIT_CONTROL:
				if(poCCR_o->unMultiServiceCCCount < OCS_MAX_MULTI_SERVICE_CC)
				{
					OCF_ExtractMultiServiceCC(hOCFContext_io, hAVP, &poCCR_o->oMultiServicesCC[poCCR_o->unMultiServiceCCCount++]);
				}
				break;
			case OCS_AVP_REQUESTED_SERVICE_UNIT:
				OCF_ExtractReqServiceUnit(hOCFContext_io, hAVP, &poCCR_o->oReqServiceUnit);
				break;
			case OCS_AVP_USER_EQUIPMENT_INFO:
				break;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_ExtractMultiServiceCC(	HOCFCONTEXT 			hOCFContext_io,
											HAVP 					hMultiServiceCCAVP_i,
											t_OCSMultiServicesCC* 	poMultiServicesCC_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

    DMBase_SList *poAVPList = hMultiServiceCCAVP_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_io->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return OCS_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
			case OCS_AVP_SERVICE_IDENTIFIER:
				poMultiServicesCC_o->unServiceId = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;

				break;
			case OCS_AVP_RATING_GROUP:
				poMultiServicesCC_o->unRatingGroup = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;;
				break;
			//case OCS_AVP_T //Trigger ID
			case OCS_AVP_REQUESTED_SERVICE_UNIT:
				OCF_ExtractReqServiceUnit(hOCFContext_io, hAVP, &poMultiServicesCC_o->oCCR.oReqServiceUnit);
				break;
			case OCS_AVP_USED_SERVICE_UNIT:
				if(poMultiServicesCC_o->oCCR.unUsedServiceUnitCount < OCS_MAX_USED_SERVICE_UNIT)
				{
					OCF_ExtractUsedServiceUnit(hOCFContext_io, hAVP, &poMultiServicesCC_o->oCCR.oUsedServiceUnit[poMultiServicesCC_o->oCCR.unUsedServiceUnitCount++]);
				}
				break;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_ExtractReqServiceUnit(	HOCFCONTEXT 			hOCFContext_io,
											HAVP 					hReqServiceUnitAVP_i,
											t_OCSReqServiceUnit* 	poReqServiceUnit_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

    DMBase_SList *poAVPList = hReqServiceUnitAVP_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_io->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return OCS_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
			case  OCS_AVP_CC_TIME:
				poReqServiceUnit_o->unCCTime = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
				break;
			case OCS_AVP_CC_MONEY:
				OCF_ExtractCCMoney(hOCFContext_io, hAVP, &poReqServiceUnit_o->oCCMoney);
				break;
			case OCS_AVP_CC_TOTAL_OCTETS:
				poReqServiceUnit_o->ullCCTotalOctets = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
			case OCS_AVP_CC_INPUT_OCTETS:
				poReqServiceUnit_o->ullCCInputOctets = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
			case OCS_AVP_CC_OUTPUT_OCTETS:
				poReqServiceUnit_o->ullCCOutputOctets = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
        }
        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

t_OCSAPIRetVal OCF_ExtractCCMoney(	HOCFCONTEXT		hOCFContext_io,
									HAVP 			hCCMoneyAVP_i,
									t_OCSCCMoney*	poCCMoney_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

    DMBase_SList *poAVPList = hCCMoneyAVP_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_io->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return OCS_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
			case  OCS_AVP_UNIT_VALUE:
				OCF_ExtractUnitValue(hOCFContext_io, hAVP, &poCCMoney_o->oUnitValue);
				break;
			case OCS_AVP_CURRENCY_CODE:
				poCCMoney_o->unCurrencyCode = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
				break;
        }
        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}



t_OCSAPIRetVal OCF_ExtractUnitValue(HOCFCONTEXT		hOCFContext_io,
									HAVP 			hUnitValue_i,
									t_OCSUnitValue*	poUnitValue_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

    DMBase_SList *poAVPList = hUnitValue_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_io->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return OCS_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
			case  OCS_AVP_VALUE_DIGITS:
				poUnitValue_o->llValueDigits = *((t_DiamInteger64*)hAVP->pData)->pllInteger64;;
				break;
			case OCS_AVP_EXPONENT:
				poUnitValue_o->nExponent = *((t_DiamInteger32*)hAVP->pData)->pnInteger32;
				break;
        }
        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}



t_OCSAPIRetVal OCF_ExtractUsedServiceUnit(	HOCFCONTEXT				hOCFContext_io,
											HAVP 					hUsedServiceUnitAVP_i,
											t_OCSUsedServiceUnit* 	poUsedServiceUnit_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

    DMBase_SList *poAVPList = hUsedServiceUnitAVP_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hOCFContext_io->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return OCS_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
			case  OCS_AVP_CC_TIME:
				poUsedServiceUnit_o->unCCTime = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
				break;
			case OCS_AVP_CC_MONEY:
				break;
			case OCS_AVP_CC_TOTAL_OCTETS:
				poUsedServiceUnit_o->ullCCTotalOctets = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
			case OCS_AVP_CC_INPUT_OCTETS:
				poUsedServiceUnit_o->ullCCInputOctets = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
			case OCS_AVP_CC_OUTPUT_OCTETS:
				poUsedServiceUnit_o->ullCCOutputOctets = *((t_DiamUnsigned64*)hAVP->pData)->pullUnsigned64;
				break;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving");
	return OCS_Retval_Success;
}

int	OCS_CopyDiamUTF8String(t_OCSFixedLenUTF8String* poDest_o, t_DiamUTF8String* poSrc_i)
{
	if(NULL == poSrc_i || NULL == poSrc_i->pucOctetString || NULL == poDest_o)
	{
		return 0;
	}

    int nStrSize = poSrc_i->unNumOfOctets;

    if(nStrSize > OCS_MAX_FIXED_LEN_UTF8STRING_SIZE-1)
    {
        nStrSize = OCS_MAX_FIXED_LEN_UTF8STRING_SIZE-1;
    }

    DMBase_MemsetZero(poDest_o, sizeof(t_OCSFixedLenUTF8String));
    memcpy(poDest_o->ucString, poSrc_i->pucOctetString, nStrSize);
    poDest_o->ucString[nStrSize] = '\0';
    poDest_o->unLength = nStrSize;

    return nStrSize;
}

int	OCS_CopyDiamOctetString(t_OCSFixedLenOctetString* poDest_o, t_DiamOctetString* poSrc_i)
{
	if(NULL == poSrc_i || NULL == poSrc_i->pucOctetString || NULL == poDest_o)
	{
		return 0;
	}

    int nStrSize = poSrc_i->unNumOfOctets;

    if(nStrSize > OCS_MAX_FIXED_LEN_OCTETSTRING_SIZE-1)
    {
        nStrSize = OCS_MAX_FIXED_LEN_OCTETSTRING_SIZE-1;
    }

    DMBase_MemsetZero(poDest_o, sizeof(t_OCSFixedLenOctetString));

    memcpy(poDest_o->ucString, poSrc_i->pucOctetString, nStrSize);
    poDest_o->unLength = nStrSize;
    return nStrSize;

}

int	OCS_CopyOCSUTF8String(t_OCSFixedLenUTF8String* poDest_o, t_OCSFixedLenUTF8String* poSrc_i)
{
	if(NULL == poSrc_i || NULL == poDest_o)
	{
		return 0;
	}

    int nStrSize = poSrc_i->unLength;

    if(nStrSize > OCS_MAX_FIXED_LEN_UTF8STRING_SIZE-1)
    {
        nStrSize = OCS_MAX_FIXED_LEN_UTF8STRING_SIZE-1;
    }

    DMBase_MemsetZero(poDest_o, sizeof(t_OCSFixedLenUTF8String));

    memcpy(poDest_o->ucString, poSrc_i->ucString, nStrSize);
    poDest_o->unLength = nStrSize;
    return nStrSize;


}
