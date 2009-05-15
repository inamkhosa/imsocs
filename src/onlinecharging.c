
#include "dappcommon_srvrconfload.h"
#include "onlinecharging.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "dmbase_data_types.h"
#include "dmbase_ampswrappers.h"


int g_nDiamBaseTraceID = 0;



HDIAMETERSTACKCONTEXT g_hDiameterStackContext = NULL;

int  main (int argc, char *argv[])
{


    printf("\n\n ****** Starting OCS Server ******  \n\n");
	
	/*
	if ( 2 !=argc)
	{
		printf("\n Incorrect parameters \n");
		printf("\n Correct Usage:\n\t OCS PathOfOCSConfigFile.xml\n\n");
		return 0;
	}*/

	unsigned char ucConfigFile[1024] = {0};
	strcpy(ucConfigFile, DATADIR);
	strcat(ucConfigFile,"ocs_config.xml");

	FILE*	fpTemp = NULL;
	_Bool	bIsPathRelative= 0;
	//try opening as relative path
	fpTemp = fopen(ucConfigFile,"r");
	if (NULL == fpTemp )
	{
		bIsPathRelative = 1;
	}
	else
	{
		bIsPathRelative = 0;
	}
	if  ( NULL != fpTemp)
	{
		fclose(fpTemp);
		fpTemp = NULL;
	}



    AMPS_HANDLE hAMPSHandle;
    AMPS_BOOL   btrace = 1;
    char pFilePath[] = "/home/guest/DEBUGAMPS.txt"	;
	DMBase_SList*	pListOfModuleInfo = NULL;

    FRAMEWORK_HANDLE hFrameworkHandle = AMPS_Init(AMPS_TIMER_RESOLUTION_20MS,btrace, pFilePath ,AMPS_PORT_POOL_SIZE_10, 10000);

    if (NULL == hFrameworkHandle)
    {
        printf("AMPS_Init: AMPS_Init is failed.\n");
        return DMBase_Retval_Failure;
    }

    AMPS_SetTraceMode(hFrameworkHandle, AMPS_TRACE_MODE_DISPLAY);

	//AMPS_TRACE_LEVEL_ERROR
    AMPS_SetTraceLevel(hFrameworkHandle, AMPS_TRACE_LEVEL_ERROR); // 31 for all
    //AMPS_SetTraceForSAPI(hFrameworkHandle);
	//AMPS_SetTraceForEventSystem(hFrameworkHandle);
	//AMPS_SetTraceForIOAgent(hFrameworkHandle);
	//AMPS_SetTraceForSched(hFrameworkHandle);
    AMPS_EnableLogging(hFrameworkHandle, "testapp_log.txt");
    g_nDiamBaseTraceID = AMPS_GetTraceID(hFrameworkHandle);
    AMPS_SetTraceForTraceID(hFrameworkHandle, g_nDiamBaseTraceID);


    if(NULL == AMPS_SchedInit(hFrameworkHandle,OCS_MAX_NUM_INTERNAL_EVENTS,OCS_MAX_NUM_EXTERNAL_EVENTS,NULL,NULL,NULL, NULL))
    {
        printf("AMPS_Init: AMPS_SchedInit has failed ");
        return DMBase_Retval_Failure;

    }

    //Create Context
    HDIAMETERSTACKCONTEXT hDiameterStackContext = NULL;

    if(DMBase_Retval_Failure == DMBase_CreateDiameterContext(&hDiameterStackContext))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "CreateDiameterContext failed.\n");
        return DMBase_Retval_Failure;
    }

    DMBase_SetFrameworkHandleInDiameterContext(hDiameterStackContext , hFrameworkHandle);

    t_DiaStackInitData DiaStackInitDataObj = {0};

    DMBase_MemsetZero(&DiaStackInitDataObj, sizeof(t_DiaStackInitData));
    t_DiaStackInitData*  pDiaStackInitData =  NULL;
    DMBase_Malloc(hDiameterStackContext, (void**)&pDiaStackInitData,sizeof(t_DiaStackInitData));
	t_DiamOctetString	oAppConfigFilePathName = {0};
	if (0==bIsPathRelative)
	{
			DMBase_CopyCharArrayToDiamString(&oAppConfigFilePathName,
									 ucConfigFile);
	}
	else
	{
		//path is relative, make it absolute
		char	pchCWD[1024]= {0};
		getcwd(pchCWD,1024);
		DMBase_CopyCharArrayToDiamString(&oAppConfigFilePathName,
									 pchCWD);
		DMBase_AppendCharStringToDiamString(&oAppConfigFilePathName,
											"/");
		DMBase_AppendCharStringToDiamString(&oAppConfigFilePathName,
											argv[1]);

	}


	t_Dappcommon_AppPath oAppPath = {0};
	strcpy(oAppPath.ucDatadir, DATADIR);
	strcpy(oAppPath.ucLibdir, LIBDIR);

	if ( DappCommon_Retval_Success != DappCommon_LoadServerConfigParamsFromFile(hDiameterStackContext ,
  														oAppConfigFilePathName, &oAppPath,
													 pDiaStackInitData ,
																  &pListOfModuleInfo)
	   )
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "ERROR: DMBase_ConfigAndInitAppFromFile Failed.\n");
        return DMBase_Retval_Failure;
    }


    if ( DMBase_Retval_Success != DMBase_InitDiameterStack ( hDiameterStackContext ,  pDiaStackInitData ))
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "ERROR: DMBase_InitDiameterStack Failed.\n");
        return DMBase_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DappCommon_LoadDiamAppModules(hDiameterStackContext , pListOfModuleInfo,1) )
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "ERROR: DappCommon_LoadDiamAppModules Failed.\n");
        return DMBase_Retval_Failure;
    }


	g_hDiameterStackContext = hDiameterStackContext;

	DMBase_ShowPeerTable(hDiameterStackContext);

	DMBase_ShowRealmTable(hDiameterStackContext);

    AMPS_Main(hFrameworkHandle);
    return 0;
}




