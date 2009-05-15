

#ifndef RATINGFUNC_H
#define RATINGFUNC_H

#include "ocs_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __RatingFuncContext
{
	HDIAMETERSTACKCONTEXT hDiamStackContext;
}t_RatingFuncContext;

typedef t_RatingFuncContext* HRATINGFUNCCONTEXT;

AMPS_MODULE_HANDLE	ratingfunc_ModuleInit(	AMPS_HANDLE hAMPS_HANDLE_i,
											void* 		pvUserData_i);

void 				ratingfunc_ModuleCleanup(	AMPS_HANDLE 		hAMPS_HANDLE_i,
												AMPS_MODULE_HANDLE 	hAMPS_MODULE_HANDLE_i);

t_OCSAPIRetVal		RatingFunc_Init(HRATINGFUNCCONTEXT	hRatingFuncContext_io);

void	RatingFunc_ProcessRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
									void* r_pvEventHandle, void* r_pvEventPayload);

void	RatingFunc_ProcessPriceRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
									void* r_pvEventHandle, void* r_pvEventPayload);

void	RatingFunc_ProcessTarrifRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
									void* r_pvEventHandle, void* r_pvEventPayload);

void	RatingFunc_ProcessServiceUsageRequest(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
												void* r_pvEventHandle, void* r_pvEventPayload);

void	RatingFunc_ProcessDBResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload);

t_OCSAPIRetVal RatingFunc_ProcessDBPriceResponse(HRATINGFUNCCONTEXT hAppContext_io,
													t_OCSPriceResponse* poPriceResponse_i);

t_OCSAPIRetVal RatingFunc_ProcessDBTariffResponse(HRATINGFUNCCONTEXT hAppContext_io,
													t_OCSTariffResponse* poTariffResponse_i);

#ifdef __cplusplus
}
#endif

#endif //RATINGFUNC_H

