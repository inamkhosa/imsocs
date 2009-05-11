
#ifndef OCF_STATEMACHINE_H
#define OCF_STATEMACHINE_H

#include "ocs_defines.h"
#include "ocf.h"

#ifdef __cplusplus
extern "C" {
#endif


t_OCSAPIRetVal OCF_InitStateMachine(HOCFCONTEXT hOCFAppContext_io);

t_OCSAPIRetVal OCF_ExecStateMachine(HOCFCONTEXT 		hOCFAppContext_io,
									HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
									t_OCSEvent     		oEvent_i,
									void*              	pvData_i);


t_OCSAPIRetVal OCF_Idle_InitReqProcessingSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
                                                    t_OCSEvent     		oEvent_i,
                                                    void*              	pvData_i);

t_OCSAPIRetVal OCF_Idle_InitReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

t_OCSAPIRetVal OCF_Idle_EventReqProcessingSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);


t_OCSAPIRetVal OCF_Idle_EventReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
													HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

t_OCSAPIRetVal OCF_Open_UpdateReqProcessingSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

t_OCSAPIRetVal OCF_Open_UpdateReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

t_OCSAPIRetVal OCF_Open_TerminateReqProcessingSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

t_OCSAPIRetVal OCF_Open_TerminateReqProcessingUnSuccessful(HOCFCONTEXT 		hOCFAppContext_io,
														HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

t_OCSAPIRetVal OCF_Open_TccTimerExpires(HOCFCONTEXT 	hOCFAppContext_io,
											HOCSCCSUBSESSIONOBJECT   hSubSessionObject_i,
													  t_OCSEvent     	oEvent_i,
													  void*             pvData_i);

#ifdef __cplusplus
}
#endif

#endif //OCF_STATEMACHINE_H
