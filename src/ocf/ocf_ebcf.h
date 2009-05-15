
#ifndef OCF_EBCF_H
#define OCF_EBCF_H

#include "ocs_defines.h"
#include "ocf.h"
#include "dmbase_data_types.h"
#include "AMPS_Defines.h"

#ifdef __cplusplus
extern "C" {
#endif

void	OCF_ProcessPriceResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload);

t_OCSAPIRetVal OCF_EventBasedCharging(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i);

t_OCSRequestSubType OCF_GetRequestType(t_OCSCCR* poCCR_i);

t_OCSAPIRetVal	OCF_PopulatePriceRequest(	t_OCSPriceRequest* 	poPriceRequest_io,
											t_OCSCCR* 			poCCR_i,
											t_OCSRequestSubType	oRequestSubType_i);

t_OCSAPIRetVal	OCF_ProcessECURInitialRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i);
t_OCSAPIRetVal	OCF_ProcessECURTerminateRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i);

#ifdef __cplusplus
}
#endif

#endif //OCF_EBCF_H
