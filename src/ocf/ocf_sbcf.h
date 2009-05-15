
#ifndef OCF_SBCF_H
#define OCF_SBCF_H

#include "ocs_defines.h"
#include "dmbase_data_types.h"
#include "ocf.h"

#ifdef __cplusplus
extern "C" {
#endif

void	OCF_ProcessTariffResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload);

void	OCF_ProcessServiceUsageResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload);

t_OCSAPIRetVal	OCF_ProcessSCURInitialRequest(	HOCFCONTEXT 	hOCFContext_io,
												t_OCSCCR* 		poCCR_i,
												HPEERTABLEENTRY hPeer_i);

t_OCSAPIRetVal  OCF_ProcessSCURUpdateRequest(	HOCFCONTEXT 	hOCFContext_io,
												t_OCSCCR* 		poCCR_i,
												HPEERTABLEENTRY hPeer_i);

t_OCSAPIRetVal	OCF_ProcessSCURTerminateRequest(HOCFCONTEXT 	hOCFContext_io,
												t_OCSCCR* 		poCCR_i,
												HPEERTABLEENTRY hPeer_i);

t_OCSAPIRetVal	OCF_CreateTariffRequest(HOCFCONTEXT 		hOCFContext_io,
										t_OCSTariffRequest* poTariffRequest_io,
										t_OCSCCR* 			poCCR_i,
										t_OCSRequestSubType oRequestSubType_i);
#ifdef __cplusplus
}
#endif

#endif //OCF_SBCF_H
