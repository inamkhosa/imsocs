
#ifndef OCS_DEFINES_H
#define OCS_DEFINES_H

#include "dmbase_data_types.h"


//from RFC 4006 , Diameter Credit Control Application
#define		OCS_AVP_CC_CORRELATION_ID						411
#define		OCS_AVP_CC_INPUT_OCTETS	 	 					412
#define		OCS_AVP_CC_MONEY								413
#define		OCS_AVP_CC_OUTPUT_OCTETS						414
#define		OCS_AVP_CC_REQUEST_NUMBER						415
#define		OCS_AVP_CC_REQUEST_TYPE							416
#define		OCS_AVP_CC_SERVICE_SPECIFIC_UNITS					417
#define		OCS_AVP_SESSION_FAILOVER						418
#define		OCS_AVP_SUB_SESSION_ID							419
#define		OCS_AVP_CC_TIME									420
#define		OCS_AVP_CC_TOTAL_OCTETS							421
#define		OCS_AVP_CC_UNIT_TYPE							454
#define		OCS_AVP_CHECK_BALANCE_RESULT					422
#define		OCS_AVP_COST_INFORMATION						423
#define		OCS_AVP_COST_UNIT								424
#define		OCS_AVP_CREDIT_CONTROL							426
#define		OCS_AVP_CREDIT_CONTROL_FAILURE_HANDLING			427
#define		OCS_AVP_CURRENCY_CODE							425
#define		OCS_AVP_DIRECT_DEBITING_FAILURE_HANDLING		428
#define		OCS_AVP_EXPONENT								429
#define		OCS_AVP_FINAL_UNIT_ACTION						449
#define		OCS_AVP_FINAL_UNIT_INDICATION					430
#define		OCS_AVP_GRANTED_SERVICE_UNIT					431
#define		OCS_AVP_G_S_U_POOL_IDENTIFIER					453
#define		OCS_AVP_G_S_U_POOL_REFERENCE					457
#define		OCS_AVP_MULTIPLE_SERVICES_CREDIT_CONTROL		456
#define		OCS_AVP_MULTIPLE_SERVICES_INDICATOR				455
#define		OCS_AVP_RATING_GROUP							432
#define		OCS_AVP_REDIRECT_ADDRESS_TYPE					433
#define		OCS_AVP_REDIRECT_SERVER							434
#define		OCS_AVP_REDIRECT_SERVER_ADDRESS					435
#define		OCS_AVP_REQUESTED_ACTION						436
#define		OCS_AVP_REQUESTED_SERVICE_UNIT					437
#define		OCS_AVP_RESTRICTION_FILTER_RULE					438
#define		OCS_AVP_SERVICE_CONTEXT_ID						461
#define		OCS_AVP_SERVICE_IDENTIFIER						439
#define		OCS_AVP_SERVICE_PARAMETER_INFO					440
#define		OCS_AVP_SERVICE_PARAMETER_TYPE					441
#define		OCS_AVP_SERVICE_PARAMETER_VALUE					442
#define		OCS_AVP_SUBSCRIPTION_ID							443
#define		OCS_AVP_SUBSCRIPTION_ID_DATA					444
#define		OCS_AVP_SUBSCRIPTION_ID_TYPE					450
#define		OCS_AVP_TARIFF_CHANGE_USAGE						452
#define		OCS_AVP_TARIFF_TIME_CHANGE						451
#define		OCS_AVP_UNIT_VALUE								445
#define		OCS_AVP_USED_SERVICE_UNIT						446
#define		OCS_AVP_USER_EQUIPMENT_INFO						458
#define		OCS_AVP_USER_EQUIPMENT_INFO_TYPE				459
#define		OCS_AVP_USER_EQUIPMENT_INFO_VALUE				460
#define		OCS_AVP_VALUE_DIGITS							447
#define		OCS_AVP_VALIDITY_TIME							448


//the following AVPs are taken from 3GPP TS 32299-830


#define		OCS_AVP_SERVICE_INFORMATION						873
#define		OCS_AVP_POC_INFORMATION 						879
#define		OCS_AVP_POC_SERVER_ROLE 						883
#define		OCS_AVP_POC_SESSION_TYPE 						884
#define		OCS_AVP_POC_USER_ROLE 							1252
#define		OCS_AVP_POC_SESSION_INITIATION_TYPE 			1277
#define		OCS_AVP_POC_EVENT_TYPE 							2025
#define		OCS_AVP_NUMBER_OF_PARTICIPANTS 					885
#define		OCS_AVP_PARTICIPANTS_INVOLVED 					887
#define		OCS_AVP_PARTICIPANT_GROUP 						1260
#define		OCS_AVP_TALK_BURST_EXCHANGE 					1255
#define		OCS_AVP_POC_CONTROLLING_ADDRESS 				858
#define		OCS_AVP_POC_GROUP_NAME 							859
#define		OCS_AVP_POC_SESSION_ID 							1229
#define		OCS_AVP_CHARGED_PARTY 							857
#define		OCS_AVP_POC_USER_ROLE_IDS 						1253
#define		OCS_AVP_POC_USER_ROLE_INFO_UNITS 				1254
#define		OCS_AVP_CALLED_PARTY_ADDRESS 					832
#define		OCS_AVP_PARTICIPANT_ACCESS_PRIORITY 			1259
#define		OCS_AVP_USER_PARTICIPATING_TYPE 				1279
#define		OCS_AVP_POC_CHANGE_TIME 						1262
#define		OCS_AVP_NUMBER_OF_TALK_BURSTS 					1283
#define		OCS_AVP_TALK_BURST_VOLUME 						1287
#define		OCS_AVP_TALK_BURST_TIME 						1286
#define		OCS_AVP_NUMBER_OF_RECEIVED_TALK_BURSTS 			1282
#define		OCS_AVP_RECEIVED_TALK_BURST_VOLUME 				1285
#define		OCS_AVP_RECEIVED_TALK_BURST_TIME 				1284
#define		OCS_AVP_POC_CHANGE_CONDITION 					1261
#define		OCS_AVP_LOW_BALANCE_INDICATION					2020
#define		OCS_AVP_REMAINING_BALANCE						2021

//TODO: Add AVP info to dictionary
#define		OCS_AVP_TIME_QUOTA_THRESHOLD					868
#define		OCS_AVP_VOLUME_QUOTA_THRESHOLD					869
#define		OCS_AVP_QUOTA_HOLDING_TIME						871
#define		OCS_AVP_QUOTA_CONSUMPTION_TIME					881
#define		OCS_AVP_UNIT_QUOTA_THRESHOLD					1226


//Multiple Services Indicator
#define		OCS_MULTIPLE_SERVICES_NOT_SUPPORTED				0
#define 	OCS_MULTIPLE_SERVICES_SUPPORTED					1
//Requested Action
#define		OCS_DIRECT_DEBITING								0
#define		OCS_REFUND_ACCOUNT								1
#define 	OCS_CHECK_BALANCE								2
#define		OCS_PRICE_ENQUIRY								3

//Result Codes Transient Failure
#define		DIAMETER_END_USER_SERVICE_DENIED				4010
#define		DIAMETER_CREDIT_CONTROL_NOT_APPLICABLE			4011
#define		DIAMETER_CREDIT_LIMIT_REACHED					4012

//Result Codes Permanent Failures
#define		DIAMETER_USER_UNKNOWN							5030
#define		DIAMETER_RATING_FAILED							5031

#define		OCS_MAX_FIXED_LEN_UTF8STRING_SIZE				256
#define		OCS_MAX_FIXED_LEN_OCTETSTRING_SIZE				256
#define		OCS_NUM_OF_SUBSC_IDS							128
#define		OCS_MAX_MULTI_SERVICE_CC						4
#define 	OCS_MAX_SERVICE_ID								16
#define		OCS_MAX_USED_SERVICE_UNIT						8
#define		OCS_MAX_REPORTING_REASON						8
#define		OCS_MAX_ENVELOPE								8
#define		OCS_MAX_SERVICE_SPECIFIC_INFO					8
#define		OCS_MAX_TRIGGER_TYPE							16
#define		OCS_MAX_RESTRICTION_FILTER_RULE					8
#define		OCS_MAX_FILTER_ID								8
#define		OCS_MAX_GSU_POOL_REFERENCE						8


 //OCS Events
#define		OCS_OCF_REQUEST_EVENT							5
#define		OCS_OCF_RESPONSE_EVENT							6
#define		OCS_RATING_FUNCTION_PRICE_REQUEST_EVENT			7
#define		OCS_RATING_FUNCTION_PRICE_RESPONE_EVENT			8
#define		OCS_RATING_FUNCTION_TARIFF_REQUEST_EVENT		9
#define		OCS_RATING_FUNCTION_TARIFF_RESPONE_EVENT		10
#define		OCS_RATING_FUNCTION_SERVICE_USAGE_REQUEST_EVENT	11
#define		OCS_RATING_FUNCTION_SERVICE_USAGE_RESPONE_EVENT	12
#define		OCS_RATING_FUNCTION_REQUEST_EVENT				13
#define		OCS_RATING_FUNCTION_RESPONSE_EVENT				14
#define		OCS_DB_PRICE_REQUEST_EVENT						15
#define		OCS_DB_PRICE_RESPONSE_EVENT						16
#define		OCS_DB_TARIFF_REQUEST_EVENT						17
#define		OCS_DB_TARIFF_RESPONSE_EVENT					18
//Application Id
#define		OCS_APPLICATION_ID								4

//Command Codes
#define		OCS_CREDIT_CONTROL_REQUEST						272
#define		OCS_CREDIT_CONTROL_ANSWER						272


#define		OCS_UNSUED_VALUE								-1

typedef enum OCSAPIRetVal
{
	OCS_Retval_Success,
	OCS_Retval_Failure,
	OCS_Retval_IncorrectParameters,
	OCS_Retval_MoreInfoRequired,
	OCS_Retval_AccessDenied,
	OCS_Retval_UnableToOpenDevice,
	OCS_Retval_Insufficient_Buffer,
	OCS_Retval_Not_Found,
	OCS_Retval_Not_Supported,
	OCS_Retval_Null_Pointer,
	OCS_Retval_Unknown_AVP,
	OCS_Retval_InvalidAVPData,
	OCS_Retval_InvalidMsgHeader,
	OCS_Retval_Limit_Exceed
}t_OCSAPIRetVal;

typedef struct __OCSFixedLenOctetString
{
	unsigned char ucString[OCS_MAX_FIXED_LEN_OCTETSTRING_SIZE];
	unsigned int  unLength;
}t_OCSFixedLenOctetString;

typedef struct __OCSFixedLenUTF8String
{
	unsigned char ucString[OCS_MAX_FIXED_LEN_UTF8STRING_SIZE];
	unsigned int  unLength;

}t_OCSFixedLenUTF8String;

typedef enum __OCSCCRequestType
{
	OCS_INITIAL_REQUEST = 1,
	OCS_UPDATE_REQUEST,
	OCS_TERMINATION_REQUEST,
	OCS_EVENT_REQUEST
}t_OCSCCRequestType;


typedef enum __OCSSubscriptionIdType
{
	OCS_END_USER_E164,
	OCS_END_USER_IMSI,
	OCS_END_USER_SIP_URI,
	OCS_END_USER_NAI,
	OCS_END_USER_PRIVATE
}t_OCSSubscriptionIdType;

typedef struct __OCSSubscriptionId
{
	t_OCSSubscriptionIdType	oSubscriptionIdType;
	t_OCSFixedLenUTF8String oSubscriptionIdData;
}t_OCSSubscriptionId;

typedef struct __OCSUnitValue
{
	long long 	llValueDigits;
	int 		nExponent;
}t_OCSUnitValue;

typedef struct __OCSCCMoney
{
	t_OCSUnitValue 	oUnitValue;
	unsigned int	unCurrencyCode;
}t_OCSCCMoney;

typedef struct __OCSReqServiceUnit
{
	unsigned int 		unCCTime;
	t_OCSCCMoney 		oCCMoney;
	unsigned long long 	ullCCTotalOctets;
	unsigned long long 	ullCCInputOctets;
	unsigned long long 	ullCCOutputOctets;
	unsigned long long 	ullCCServiceSpecificUnits;
}t_OCSReqServiceUnit;

typedef struct __OCSUsedServiceUnit
{
	unsigned int		unTariffChangeUsage;
	unsigned int 		unCCTime;
	t_OCSCCMoney 		oCCMoney;
	unsigned long long 	ullCCTotalOctets;
	unsigned long long 	ullCCInputOctets;
	unsigned long long 	ullCCOutputOctets;
	unsigned long long 	ullCCServiceSpecificUnits;
}t_OCSUsedServiceUnit;

typedef struct __OCSGrantedServiceUnit
{
	unsigned int		unTariffTimeChange;
	unsigned int 		unCCTime;
	t_OCSCCMoney 		oCCMoney;
	unsigned long long 	ullCCTotalOctets;
	unsigned long long 	ullCCInputOctets;
	unsigned long long 	ullCCOutputOctets;
	unsigned long long 	ullCCServiceSpecificUnits;

}t_OCSGrantedServiceUnit;

typedef struct __OCSTigger
{
	unsigned int oTriggerType[OCS_MAX_TRIGGER_TYPE];
	unsigned int unTriggerTypeCount;
}t_OCSTigger;

typedef struct __OCSEnvelope
{
	t_DiamTime			oEnvelopeStartTime;
	t_DiamTime			oEnvelopeEndTime;
	unsigned long long 	ullCCTotalOctets;
	unsigned long long 	ullCCInputOctets;
	unsigned long long 	ullCCOutputOctets;
	unsigned long long 	ullCCServiceSpecific;
}t_OCSEnvelope;

typedef struct __OCSAFCorrelationInfo
{
	//Add AF-Charging-Identifier here
	//Add Flows here
}t_OCSAFCorrelationInfo;

typedef struct __OCSServiceSpecificInfo
{
	t_OCSFixedLenUTF8String oServiceSpecificData;
	unsigned int 			unServiceSpecificType;
}t_OCSServiceSpecificInfo;

typedef enum __OCSCCUnitType
{
	OCS_TIME,
	OCS_MONEY,
	OCS_TOTAL_OCTETS,
	OCS_INPUT_OCTETS,
	OCS_OUTPUT_OCTETS,
	OCS_SERVICE_SPECIFIC_UNITS
}t_OCSCCUnitType;

typedef struct __OCSGSUPoolReference
{
	unsigned int	unGSUPoolId;
	t_OCSCCUnitType	oCCUnitType;
	t_OCSUnitValue	oUnitValue;

}t_OCSGSUPoolReference;

typedef enum __OCSFinalUnitAction
{
	OCS_TERMINATE,
	OCS_REDIRECT,
	OCS_REDIRECT_ACCESS

}t_OCSFinalUnitAction;

typedef enum __OCSRedirectAddressType
{
	OCS_IPv4,
	OCS_IPv6,
	OCS_URL,
	OCS_SIP_URI
}t_OCSRedirectAddressType;

typedef struct __OCSRedirectServer
{
	t_OCSRedirectAddressType oAddressType;
	t_OCSFixedLenUTF8String	 oAddress;
}t_OCSRedirectServer;

typedef struct __OCSFinalUnitIndication
{
	t_OCSFinalUnitAction		oFinalUnitAction;
	t_OCSFixedLenOctetString	oRestrictionFilterRule[OCS_MAX_RESTRICTION_FILTER_RULE];
	unsigned int				unRestFilterRuleCount;
	t_OCSFixedLenUTF8String		oFilterId[OCS_MAX_FILTER_ID];
	unsigned int				unFilterIdCount;
	t_OCSRedirectServer			oRedirectServer;


}t_OCSFinalUnitIndication;

typedef enum __OCSPSAppendFreeFormatData
{
	OCS_Append,
	OCS_Overwrite
}t_OCSPSAppendFreeFormatData;

typedef enum __OCSCheckBalanceResult
{
	OCS_ENOUGH_CREDIT,
	OCS_NO_CREDIT,
	OCS_NOT_USED
}t_OCSCheckBalanceResult;

typedef struct __OCSPSFurnishChargingInfo
{
	//3GPP-Charging-Id
	t_OCSFixedLenOctetString	oPSFreeFormatData;
	t_OCSPSAppendFreeFormatData	oPSAppendFreeFormatData;

}t_OCSPSFurnishChargingInfo;

typedef enum __OCSEnvelopeReporting
{
	OCS_DO_NOT_REPORT_ENVELOPES,
	OCS_REPORT_ENVELOPES,
	OCS_REPORT_ENVELOPES_WITH_VOLUME,
	OCS_REPORT_ENVELOPES_WITH_EVENTS,
	OCS_REPORT_ENVELOPES_WITH_VOLUME_AND_EVENTS

}t_OCSEnvelopeReporting;

typedef enum __OCSTimeQuotaType
{
	OCS_DISCRETE_TIME_PERIOD,
	OCS_CONTINUOUS_TIME_PERIOD
}t_OCSTimeQuotaType;

typedef struct __OCSTimeQuotaMechanism
{
	t_OCSTimeQuotaType  oTimeQuotaType;
	unsigned int		unBaseTimeInterval;
}t_OCSTimeQuotaMechanism;

typedef struct __OCSMultiServicesCC
{
	unsigned int				unServiceId;// TBD about multiple service ids[OCS_MAX_SERVICE_ID];
//	unsigned int				unServiceIdCount;
	unsigned int				unRatingGroup;

	union
	{
		struct
		{
			t_OCSReqServiceUnit 		oReqServiceUnit;
			t_OCSUsedServiceUnit 		oUsedServiceUnit[OCS_MAX_USED_SERVICE_UNIT];
			unsigned int				unUsedServiceUnitCount;
			unsigned int				oReportingReason[OCS_MAX_REPORTING_REASON];
			unsigned int				unReportingReasonCount;
			t_OCSFixedLenOctetString 	oRefundInfo;
			t_OCSEnvelope				oEnvelope[OCS_MAX_ENVELOPE];
			unsigned int				unEnvelopeCount;
//			t_OCSAFCorrelationInfo 		oAFCorrelationInfo[];  //Related to Gx Interface
			unsigned int				unAFCorrelationInfoCount;
			t_OCSServiceSpecificInfo	oServiceSpecificInfo[OCS_MAX_SERVICE_SPECIFIC_INFO];
			unsigned int				unServiceSpecificInfoCount;
			t_OCSTigger					oTrigger;

		}oCCR;

		struct
		{
			t_OCSGrantedServiceUnit*	poGrantedServiceUnits;
			t_OCSGSUPoolReference*		poGSUPoolReference;
			unsigned int				unGSUPoolReferenceCount;
			unsigned int				unValidityTime;
			unsigned int				unResultCode;
			t_OCSFinalUnitIndication*	poFinalUnitIndication;
			unsigned int				unTimeQuotaThreshold;
			unsigned int				unVolumeQuotaThreshold;
			unsigned int				unUnitQuotaThreshold;
			unsigned int				unQuotaHoldingTime;
			unsigned int				unQuotaConsumptionTime;
//			t_OCSPSFurnishChargingInfo	oPSFurnishChargingInfo; //No related to 3gpp 32.296
			t_OCSFixedLenOctetString* 	poRefundInfo;
			t_OCSEnvelopeReporting*		poEnvelopReporting;
			t_OCSTimeQuotaMechanism*	poTimeQuotaMechanism;
			t_OCSTigger*				poTrigger;
		}oCCA;
	};
}t_OCSMultiServicesCC;

typedef struct __OCSServiceParamInfo
{
	unsigned int 			 unServiceParamType;
	t_OCSFixedLenOctetString oServiceParamValue;

}t_OCSServiceParamInfo;

typedef struct __OCSUserEquipmentInfo
{
	unsigned int			 unUserEquipmentInfoType;
	t_OCSFixedLenOctetString oUserEquipmentInfoType;
}t_OCSUserEquipmentInfo;

typedef struct __OCSCostInformation
{
	t_OCSUnitValue 			oUnitValue;
	unsigned int			unCurrencyCode;
	t_OCSFixedLenUTF8String oCostUnit;
}t_OCSCostInformation;

typedef enum __OCSLowBalanceIndication
{
	OCS_NOT_APPLICABLE,
	OCS_YES
}t_OCSLowBalanceIndication;

typedef struct __OCSRemainingBalance
{
	t_OCSUnitValue 			oUnitValue;
	unsigned int			unCurrencyCode;
}t_OCSRemainingBalance;

typedef enum __OCSDDFailureHandling
{
	OCS_TERMINATE_OR_BUFFER,
	OCS_CONTINUE
}t_OCSDDFailureHandling;


typedef struct __OCSCCR
{
	unsigned char				ucFlags;
	unsigned int				unHopByHopId;
	unsigned int				unEndToEndId;
	t_OCSFixedLenUTF8String 	oSessionId;
	t_OCSFixedLenUTF8String 	oServiceContextId;
	t_OCSCCRequestType			oCCRequestType;
	unsigned int				unCCRequestNum;
	t_OCSFixedLenUTF8String		oUserName;
	unsigned long long 			ullCCSubSessionId;
	unsigned int				unServiceId;
	//Declaration oEventTimestamp as Time
	t_OCSSubscriptionId			oSubscriptionId;
	unsigned int				unTerminationCause;
	t_OCSReqServiceUnit			oReqServiceUnit;
	unsigned int				unRequestedAction;
	unsigned int				unMultiServiceIndicator;
	t_OCSMultiServicesCC		oMultiServicesCC[OCS_MAX_MULTI_SERVICE_CC];
	unsigned int				unMultiServiceCCCount;
	t_OCSUserEquipmentInfo		oUserEquipmentInfo;
}t_OCSCCR;

typedef struct __OCSCCA
{
	t_OCSFixedLenUTF8String 	oSessionId;
	unsigned int				unResultCode;
	t_OCSCCRequestType			oCCRequestType;
	unsigned int				unCCRequestNum;
	unsigned long long 			ullCCSubSessionId;
	t_OCSMultiServicesCC*		poMultiServicesCC;
	unsigned int				unMultiServiceCCCount;
	t_OCSCostInformation*		poCostInfo;
	t_OCSLowBalanceIndication	oLowBalanceIndication;
	t_OCSRemainingBalance*		poRemainingBalance;
	t_OCSCheckBalanceResult		oCheckBalanceResult;
	unsigned int				unCCFailureHandling;
	t_OCSGrantedServiceUnit*	poGrantedServiceUnits;
	t_OCSDDFailureHandling*		poDDFailureHandling;
}t_OCSCCA;

typedef enum __OCSRatingFuncResult
{
	OCS_SUCCESS,
	OCS_FAILURE
}t_OCSRatingFuncResult;

typedef enum __OCSRequestSubType
{
	OCS_AoC,
	OCS_Reservation,
	OCS_Debit,
	OCS_Release,
	OCS_Direct_Debiting,
	OCS_Refund_Account,
	OCS_Check_Balance,
	OCS_Price_Enquiry,
	OCS_Invalid_Request_Type
}t_OCSRequestSubType;

typedef enum __OCSDestinationIdType
{
	OCS_Destination_Number,
	OCS_Destination_APN,
	OCS_Destination_URL,
	OCS_Destination_EmailAddress,
	OCS_Destination_PrivateID
}t_OCSDestinationIdType;

typedef struct __OCSDestinationId
{
	t_OCSDestinationIdType	oDestinationIdType;
	t_OCSFixedLenUTF8String oDestinationIdData;
}t_OCSDestinationId;

typedef struct __OCSPriceRequest
{
	t_OCSFixedLenUTF8String 	oSessionId;
	t_OCSFixedLenUTF8String 	oServiceContextId;
	unsigned long long			ullCCSubSessionId;
	unsigned int				unActualTime;
	t_OCSSubscriptionId			oSubscriptionId;
	t_OCSFixedLenUTF8String 	oUserName;
	unsigned int				unServiceId;
	t_OCSDestinationId			oDestinationId;
	//Extension is not defined by 3GPP
	t_OCSRequestSubType			oRequestSubType;
	t_OCSReqServiceUnit			oReqServiceUnit;
	unsigned int				unTerminationCause;
}t_OCSPriceRequest;

typedef struct __OCSImpactOnCounter
{
	unsigned int	unCounterId;
	int				nCounterValueBegin;
	int				nCounterValueChange;
	int				nCounterValueEnd;
}t_OCSImpactOnCounter;

typedef struct __OCSPriceResponse
{
	t_OCSFixedLenUTF8String 	oSessionId;
	unsigned long long			ullCCSubSessionId;
	t_OCSGrantedServiceUnit		oGrantedServiceUnit;
	t_OCSFixedLenUTF8String		oBillingInfo;
	//Extension is not defined by 3GPP
	//BasicPrice is not used in current scenario
	//CounterPrice is not used in current scenario
	unsigned int				unExpiryTime;
	t_OCSImpactOnCounter		oImpactOnCounter;
	t_OCSRequestSubType			oRequestSubType;
	t_OCSCostInformation		oCostInfo;
	t_OCSRemainingBalance		oRemainingBalance;
	t_OCSLowBalanceIndication	oLowBalanceIndication;
	t_OCSCheckBalanceResult		oCheckBalanceResult;
	t_OCSRatingFuncResult		oResult;
	unsigned int				unRatingFuncResultCode;
}t_OCSPriceResponse;



typedef struct __OCSMonetaryTariff
{
	int		nEParameterE1;
	int		nEParameterE2;
	int		nEParameterE3;
	int		nEParameterE4;
	int		nEParameterE5;
	int		nEParameterE6;
	int		nEParameterE7;

}t_OCSMonetaryTariff;

typedef struct __OCSNextMonetaryTariff
{
	int		nEParameterE1;
	int		nEParameterE2;
	int		nEParameterE3;
	int		nEParameterE4;
	int		nEParameterE5;
	int		nEParameterE6;
	int		nEParameterE7;

}t_OCSNextMonetaryTariff;

typedef struct __OCSTariffRequest
{
	t_OCSFixedLenUTF8String		oSessionId;
	unsigned long long			ullCCSubSessionId;
	t_OCSFixedLenUTF8String		oUserName;
	unsigned int				unBeginTime;
	unsigned int				unActualTime;
	t_OCSSubscriptionId			oSubscriptionId;
	unsigned int				unServiceId;
	t_OCSDestinationId			oDestinationId;
	//Service Information
	//Extension
	t_OCSRequestSubType			oRequestSubType;
	unsigned int				unRequestedUnits;
	unsigned int				unConsumedUnits;
	unsigned int				unConsumedUnitsAfterTarifSwitch;
	t_OCSReqServiceUnit			oReqServiceUnit;
	unsigned int 				unTerminationCause;
}t_OCSTariffRequest;

typedef struct __OCSTariffResponse
{
	t_OCSFixedLenUTF8String		oSessionId;
	unsigned long long			ullCCSubSessionId;
	unsigned int				unTariffSwitchTime;
	t_OCSMonetaryTariff			oMonetaryTariff;
	t_OCSNextMonetaryTariff		oNextMonetaryTariff;
	unsigned int				unExpiryTime;
	unsigned int				unValidUnits;
	t_OCSFixedLenUTF8String		oBillingInfo;
	unsigned int				unPrice;
	t_OCSImpactOnCounter		oImpactOnCounter;
	t_OCSRatingFuncResult		oResult;
	t_OCSRemainingBalance		oRemainingBalance;
	t_OCSLowBalanceIndication 	oLowBalanceIndication;

}t_OCSTariffResponse;


#endif //OCS_DEFINES_H



