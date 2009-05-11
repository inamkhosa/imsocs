

#ifndef OCF_SESSION_H
#define	OCF_SESSION_H

#include "dmbase_data_types.h"
#include "ocs_defines.h"
#include "ocf.h"


#ifdef __cplusplus
extern "C" {
#endif

t_OCSAPIRetVal	OCF_InsertSessionObject(	HOCFCONTEXT 			 hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i,
										void* 					 poSessionObj_i);

t_OCSAPIRetVal	OCF_InsertSubSessionObject(	HOCFCONTEXT 		 	 hAppContext_i,
											t_OCSFixedLenUTF8String* poSessionID_i,
											unsigned long long		 ullCCSubSessionId,
											void* 					 poSessionObj_i);

t_OCSAPIRetVal OCF_LookUpSessionObject(	HOCFCONTEXT 	hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i,
										void** 			ppoSessionObj_o);

t_OCSAPIRetVal OCF_LookUpSubSessionObject(HOCFCONTEXT 			 hAppContext_i,
										  t_OCSFixedLenUTF8String* poSessionID_i,
										  unsigned long long	   ullCCSubSessionId,
										  void** 					 ppoSessionObj_o);

t_OCSAPIRetVal OCF_RemoveSubSessionObject(	HOCFCONTEXT 	hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i,
										unsigned long long 		ullSubSessionId_i);

t_OCSAPIRetVal OCF_RemoveSessionObject(	HOCFCONTEXT 	hAppContext_i,
										t_OCSFixedLenUTF8String* poSessionID_i);

t_OCSAPIRetVal OCF_CreateSubSessionObject(	HOCFCONTEXT				hOCFContext_io,
											HOCSCCSUBSESSIONOBJECT* phSessionObject_io,
											t_OCSFixedLenUTF8String* poSessionID,
											unsigned long long 		ullSubSessionId_i,
											HPEERTABLEENTRY			hPeer_i,
											t_OCSSessionState		oSessionState_i,
											t_OCSCCR*				poCCR_i,
											t_OCSChargingType		oChargingType,
											_Bool					bProcessingInProgress_i);


#ifdef __cplusplus
}
#endif


#endif //OCF_SESSION_H

