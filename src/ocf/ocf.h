

#ifndef OCF_H
#define OCF_H

#define OCS_SESSION_TABLE_SIZE 		63001

#include "ocs_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __OCFContext	t_OCFContext;
typedef t_OCFContext* 	HOCFCONTEXT;

typedef enum __OCSChargingType
{
	OCS_Charging_IEC,
	OCS_Charging_ECUR,
	OCS_Charging_SCUR
}t_OCSChargingType;

typedef enum __OCSEvent
{
	OCS_CC_INIT_REQ_PROCESSING_SUCCESSFUL,
	OCS_CC_INIT_REQ_PROCESSING_UNSUCCESSFUL,
	OCS_CC_UPDATE_REQ_PROCESSING_SUCCESSFUL,
	OCS_CC_UPDATE_REQ_PROCESSING_UNSUCCESSFUL,
	OCS_CC_TERMINATE_REQ_PROCESSING_SUCCESSFUL,
	OCS_CC_TERMINATE_REQ_PROCESSING_UNSUCCESSFUL,
	OCS_CC_EVENT_REQ_PROCESSING_SUCCESSFUL,
	OCS_CC_EVENT_REQ_PROCESSING_UNSUCCESSFUL,
	OCS_TCC_TIMER_EXPIRED
}t_OCSEvent;


typedef enum __OCSSessionState
{
	OCS_SESSION_STATE_OPEN,
	OCS_SESSION_STATE_IDLE
}t_OCSSessionState;

typedef struct __OCSCCSubSessionObjet
{
	t_OCSFixedLenUTF8String oSessionID;
	unsigned long long 	ullCCSubSessionId;
	HPEERTABLEENTRY 	hPeer;
	DMBASE_TIMERID 		oTccTimerId;
	t_OCSCCR* 			poCCR;
	t_OCSSessionState	oSessionState;
	t_OCSChargingType	oChargingType;
	_Bool				bProcessingInProgress;
	HOCFCONTEXT			hOCFContext;

}t_OCSCCSubSessionObjet;

typedef t_OCSCCSubSessionObjet* HOCSCCSUBSESSIONOBJECT;

typedef struct __OCSSessionObjet
{

	t_OCSFixedLenUTF8String oSessionID;
	unsigned long long 	ullCCSubSessionId;
	HPEERTABLEENTRY 	hPeer;
	DMBASE_TIMERID 		oTccTimerId;
	t_OCSCCR* 			poCCR;
	t_OCSSessionState	oSessionState;
	t_OCSChargingType	oChargingType;
	_Bool				bProcessingInProgress;
	HOCFCONTEXT			hOCFContext;

	unsigned int 		unSubSessionCount;
	DMBase_SList* 		poSubSessionList;
}t_OCSSessionObjet;

typedef t_OCSSessionObjet* HOCSSESSIONOBJECT;

typedef t_OCSAPIRetVal (*t_OCSStateMachineAction)(	HOCFCONTEXT hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													t_OCSEvent     		oEvent_i,
													 void*              pvData_i);

struct __OCFContext
{
	HDIAMETERSTACKCONTEXT 	hDiamStackContext;
	t_OCSEvent 				oEventsMapping[OCS_EVENT_REQUEST+1][OCS_FAILURE+1];
	t_OCSStateMachineAction oOCSStateEventMapping[OCS_SESSION_STATE_IDLE+1][OCS_TCC_TIMER_EXPIRED+1];
	DMBASE_HASH_HANDLE  	hSessionTable;
};



AMPS_MODULE_HANDLE	ocf_ModuleInit(	AMPS_HANDLE hAMPS_HANDLE_i,
											void* 		pvUserData_i);

void 				ocf_ModuleCleanup(	AMPS_HANDLE 		hAMPS_HANDLE_i,
												AMPS_MODULE_HANDLE 	hAMPS_MODULE_HANDLE_i);

t_OCSAPIRetVal OCF_Init(HOCFCONTEXT hOCFContext_io);

t_DiamAPIReturnVal OCF_ProcessMessage(HPEERTABLEENTRY hPeer_i, HDIAMETERMESSAGE hDiamMsg_i, void* pvAppContext);

void	OCF_ProcessRatingFuncResponse(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload);

t_OCSAPIRetVal OCF_CreatCCADiamMsg(HOCFCONTEXT hOCFContext_i, t_OCSCCR* poCCR_i, t_OCSCCA* poCCA_i, HDIAMETERMESSAGE* phDiamMsg_io);

t_OCSAPIRetVal OCF_CreatMultipleServiceCCAVP(	HOCFCONTEXT 			hOCFContext_i,
												HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
												t_OCSMultiServicesCC* 	poMultiServiceCC_i,
												HAVP* 					phMulipleServiceCCAVP_io);

t_OCSAPIRetVal OCF_CreatGrantedServiceUnitAVP(HOCFCONTEXT 				hOCFContext_i,
											  HMSGALLOCATIONCONTEXT		hMsgAllocCtxt_io,
											  t_OCSGrantedServiceUnit* 	poGrantedServiceUnit_i,
											  HAVP* 					phGrantedServiceUnitAVP_io);

t_OCSAPIRetVal OCF_CreatCostInformationAVP(	HOCFCONTEXT 			hOCFContext_i,
											HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
											t_OCSCostInformation*	poCostInfo_i,
											HAVP* 					phCostInfoAVP_io);

t_OCSAPIRetVal OCF_CreatRemainingBalanceAVP(	HOCFCONTEXT 			hOCFContext_i,
											HMSGALLOCATIONCONTEXT	hMsgAllocCtxt_io,
											t_OCSRemainingBalance*	poRemainingBalance_i,
											HAVP* 					phRemainingBalance_io);

t_OCSEvent 	OCF_CheckEventType(	HOCFCONTEXT 			hOCFContext_i,
								t_OCSCCRequestType		oRequestType_i,
								t_OCSRatingFuncResult	oRatingFunctionResult_i);

t_OCSChargingType OCF_CheckChargingType(HOCFCONTEXT	hOCFContext_i,
										t_OCSCCR*	poCCR_i);

t_OCSAPIRetVal	OCF_ProcessInitialRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i);
t_OCSAPIRetVal  OCF_ProcessUpdateRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i);
t_OCSAPIRetVal	OCF_ProcessTerminateRequest(HOCFCONTEXT hOCFContext_io, t_OCSCCR* poCCR_i, HPEERTABLEENTRY hPeer_i);


#ifdef __cplusplus
}
#endif

#endif //OCF_H

