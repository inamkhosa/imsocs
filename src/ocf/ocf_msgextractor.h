
#ifndef OCF_MSG_EXTRACTOR_H
#define OCF_MSG_EXTRACTOR_H

#include "ocs_defines.h"
#include "ocf.h"
#include "dmbase_data_types.h"


t_OCSAPIRetVal OCF_ExtractCCRAttributes(HOCFCONTEXT hOCFContext_io, HDIAMETERMESSAGE hDiamMsg_i, t_OCSCCR* poCCR_o);
t_OCSAPIRetVal OCF_ExtractMultiServiceCC(HOCFCONTEXT hOCFContext_io, HAVP hMultiServiceCCAVP_i, t_OCSMultiServicesCC* poMultiServicesCC_o);
t_OCSAPIRetVal OCF_ExtractReqServiceUnit(HOCFCONTEXT hOCFContext_io, HAVP hReqServiceUnitAVP_i, t_OCSReqServiceUnit* poReqServiceUnit_o);

t_OCSAPIRetVal OCF_ExtractCCMoney(	HOCFCONTEXT		hOCFContext_io,
									HAVP 			hCCMoneyAVP_i,
									t_OCSCCMoney*	poCCMoney_o);

t_OCSAPIRetVal OCF_ExtractUnitValue(HOCFCONTEXT		hOCFContext_io,
									HAVP 			hUnitValue_i,
									t_OCSUnitValue*	poUnitValue_o);

t_OCSAPIRetVal OCF_ExtractUsedServiceUnit(HOCFCONTEXT hOCFContext_io, HAVP hUsedServiceUnitAVP_i, t_OCSUsedServiceUnit* poUsedServiceUnit_o);

int	OCS_CopyDiamUTF8String(t_OCSFixedLenUTF8String* poDest_o, t_DiamUTF8String* poSrc_i);
int	OCS_CopyDiamOctetString(t_OCSFixedLenOctetString* poDest_o, t_DiamOctetString* poSrc_i);
int	OCS_CopyOCSUTF8String(t_OCSFixedLenUTF8String* poDest_o, t_OCSFixedLenUTF8String* poSrc_i);

#endif //OCF_MSG_EXTRACTOR_H
