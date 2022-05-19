/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Copy Total File Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCOPY.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows: 
*                   
*               MaintCopyTtl()      : This Function Copys Total Counter File   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : K.You     : initial                                   
* Aug-11-99: 03.05.00 : M.Teraki  : Remove WAITER_MODEL
* Feb-08-00: 01.00.00 : hrkato    : Saratoga
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include "windows.h"
#include <tchar.h>
#include <stdio.h>
#include <string.h>

#include "ecr.h"
#include "pif.h"
#include "cswai.h"
#include "cscas.h"
#include "csstbcas.h"
#include "csstbwai.h"
#include "plu.h"
#include "csstbfcc.h"
#include "csstbstb.h"
#include "csstbbak.h"
#include "paraequ.h"
#include "para.h"
#include <pararam.h>
#include "maint.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csttl.h"
#include "ttl.h"
#include "maintram.h"
#include "appllog.h"
#include "log.h"
#include "scf.h"
#include <csop.h>
#include <csstbopr.h>
#include <report.h>
#include    "ej.h"
#include <csstbept.h>
#include <eept.h>
#include	"EEptTrans.h"
#include <mld.h>

/*
*===========================================================================
** Synopsis:    SHORT MaintCopyTtl( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*
** Return:      SUCCESS        : Successful
*               LDT_PROHBT_ADR : Prohibit Operation
*
** Description: This Function Copys Total Counter File.
*===========================================================================
*/
SHORT   MaintCopyTtl(VOID)
{
    SHORT           sError;
    SHORT           sReturn = SUCCESS;
    MAINTERRORCODE  MaintErrCode;
	SHORT           sCountDown = 20;

    /* Lock All Waiter/Cashier File */
    if ((sError = SerCasAllLock()) != CAS_PERFORM && sError != CAS_FILE_NOT_FOUND) {
        return(CasConvertError(sError));
    }

    /* Execute Copy Total Counter File */
    
    /* wait for total update is done completely, 06/05/01 */
	sCountDown = 20;
    while(TTL_SUCCESS != (sError = TtlReadWrtChk())) { /* total still exist */
		if (sCountDown == 0) {
			char  xBuff[128];
			sprintf(xBuff, "TtlReadWrtChk(): sError = %d - %d, %d", sError, TtlUphdata.usReadPointer, TtlUphdata.usWritePointer);
			NHPOS_ASSERT_TEXT((sCountDown > 0), xBuff);
		}

	    PifSleep(500);
		sCountDown--;
    }

    /* 02/12/01 */
	sCountDown = 20;
    sError = TtlPLUGetBackupStatus();
    while (sError == TTL_BK_STAT_BUSY) {
		if (sCountDown == 0) {
			char  xBuff[128];
			sprintf(xBuff, "TtlPLUGetBackupStatus(): sError = %d", sError);
			NHPOS_ASSERT_TEXT((sCountDown > 0), xBuff);
		}

        PifSleep(500);
        sError = TtlPLUGetBackupStatus();
		sCountDown--;
    }

    PifRequestSem(usTtlSemDBBackup);      /* Request Semaphore for Total database File backup */

	//Memory Leak 7.8.05
	//TtlPLUCloseDB();	/* Saratoga */
    sError = SerStartBackUp();
	if (sError != STUB_SUCCESS) {
		char xBuff[128];

		sprintf(xBuff, "**WARNING: MaintCopyTtl(): SerStartBackUp return %d", sError);
		NHPOS_ASSERT_TEXT((sError == STUB_SUCCESS), xBuff);
	}

	UieSetExtendedStatus (sError);    // set the extended status for additional information in case of error.

    SerCasAllUnLock();                                  /* UnLock Cashier File */

	/* 02/09/01 */
	//Memory Leak 7.8.05
    //TtlPLUOpenDB();                                     /* Saratoga */

    PifReleaseSem(usTtlSemDBBackup);      /* Release Semaphore for Total database File backup */

    /* lock target shared printer at this point, in case of target is down at AC85, 12/08/01 */
	{
		SHORT  sErrorLock;

		if ((sErrorLock = MaintShrTermLockSup(AC_COPY_TTL)) != SUCCESS) {        /* In case of SYSTEM BUSY */
			char xBuff[128];
			sprintf (xBuff, "**WARNING:  MaintCopyTtl() - MaintShrTermLockSup() returned %d", sErrorLock);
			NHPOS_ASSERT_TEXT((sError == SUCCESS), xBuff);
			return(SUCCESS);
		}
	}

    MaintHeaderCtl(AC_COPY_TTL, RPT_ACT_ADR);           /* Make Header */
	if (sError != STUB_SUCCESS) {
		char  xBuff[128];

        sReturn = CstConvertError(sError);

		sprintf(xBuff, "==NOTE: MaintCopyTtl() - SerStartBackup() returned %d, %d", sError, sReturn);
		NHPOS_ASSERT_TEXT((sError == STUB_SUCCESS), xBuff);

        /* Print Error Code */
        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.sErrorCode = sReturn;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintErrCode);
	}
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */

    return(sReturn);
}



SHORT MaintPifGetHostAddress(UCHAR *puchLocalAddr, UCHAR *puchHostAddr, TCHAR *wszHostName2)
{
	TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = { 0 };
    TCHAR  *paszHost;
    USHORT usSrc;
	SHORT  sRet = PIF_OK;

    PifGetLocalHostName(wszHostName);

	if (wszHostName2) {
		_tcscpy (wszHostName2, wszHostName);
	}

    /* device name "NCR2172-xx" */
    paszHost = _tcschr (wszHostName, _T('-'));

    if (paszHost != NULL) {
        paszHost++;

        usSrc = (USHORT)_ttoi (paszHost);

        puchLocalAddr[0] = 192;
        puchLocalAddr[1] = 0;
        puchLocalAddr[2] = 0;
        puchLocalAddr[3] = (UCHAR)usSrc; /* terminal no. */
    }
	else {
        PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_PIFNET_BAD_NAME);
	}

	if (0 == PifGetHostAddrByName (wszHostName, puchHostAddr)) {
        PifLog(MODULE_PIF_NETOPEN, LOG_EVENT_HOST_LOOKUP_FAIL);
        memset (puchHostAddr, 0, PIF_LEN_IP);
//        dwError = WSAGetLastError();
		sRet = PIF_ERROR_NET_UNREACHABLE;
	}
	return sRet;
}


SHORT   MaintJoinCluster(VOID)
{
    SHORT           sError;
    SHORT           sReturn = SUCCESS;
    MAINTERRORCODE  MaintErrCode;
    TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = { 0 };
	UCHAR  auchHostIpAddress[4];
	UCHAR  auchLocalAddr[4];
	UCHAR  auchTerminalNo = 0;
	USHORT usTerminalPosition = 11;

    memset(wszHostName, 0, sizeof(wszHostName));
    memset(auchLocalAddr, 0, sizeof(auchLocalAddr));
    memset(auchHostIpAddress, 0, sizeof(auchHostIpAddress));
	MaintPifGetHostAddress(auchLocalAddr, auchHostIpAddress, wszHostName);

    MaintIncreSpcCo(SPCO_CONSEC_ADR);
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    0,                                      /* Report Name Address, uchRptAdr1, CLASS_PARAREPORTNAME       */
                    0,                                      /* Report Name Address, uchRptAdr2,  CLASS_PARAREPORTNAME       */
                    0,                                      /* Special Mnemonics Address, uchSpeAdr, CLASS_PARASPECIALMNEMO */
                    0,                                      /* Report Name Address, uchRptAdr3, CLASS_PARAREPORTNAME       */
                    3,                                      /* Reset Type, uchReportType               */
                    AC_JOIN_CLUSTER,                        /* A/C Number, uchPrgSupNo                */
                    0,                                      /* Reset Type, uchResetType                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */


	sError = CliOpJoinCluster (wszHostName, auchHostIpAddress, &auchTerminalNo);

	if (sError != STUB_SUCCESS) {
		PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_FAILED_JOIN);
        sReturn = OpConvertError(sError);  

		MaintMakePrintLine (_T("Join Error"), PRT_RECEIPT|PRT_JOURNAL);

        /* Print Error Code */
        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.sErrorCode = sReturn;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintErrCode);
	}
	else {
		TCHAR   atchDeviceName[SCF_USER_BUFFER];
		TCHAR   atchDllName[SCF_USER_BUFFER];
		TCHAR   *pDeviceName;
		TCHAR   *pDllName;
		DWORD   dwRet;

		pDeviceName = atchDeviceName;
		pDllName    = atchDllName;

		_tcscpy (pDeviceName, SCF_DEVICENAME_JOIN_JOINSTATUS);
		_tcscpy (pDllName, SCF_DLLNAME_JOINSTATUS_JOINED);
		dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

		_tcscpy (pDeviceName, SCF_DEVICENAME_JOIN_LASTJOINMASTER);
		_tcscpy (pDllName, wszHostName);
		dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

		MaintMakePrintLine (_T("Totals Transfer Start"), PRT_RECEIPT|PRT_JOURNAL);


		usTerminalPosition = auchTerminalNo;
		CliOpTransferTotals(usTerminalPosition);

		MaintMakePrintLine (_T("Totals Transfer Complete"), PRT_RECEIPT|PRT_JOURNAL);
		MaintMakePrintLine (_T("Join Complete"), PRT_RECEIPT|PRT_JOURNAL);
	}

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */

    return(sReturn);
}


SHORT   MaintUnjoinCluster(VOID)
{
    SHORT           sError, SFReturn = SUCCESS;
    SHORT           sReturn = SUCCESS;
    MAINTERRORCODE  MaintErrCode;
    TCHAR  wszHostName[PIF_LEN_HOST_NAME + 1] = { 0 };
	UCHAR  auchHostIpAddress[4];
	UCHAR  auchLocalAddr[4];
	UCHAR  auchTerminalNo = 0;

    memset(wszHostName, 0, sizeof(wszHostName));
    memset(auchLocalAddr, 0, sizeof(auchLocalAddr));
    memset(auchHostIpAddress, 0, sizeof(auchHostIpAddress));
	MaintPifGetHostAddress(auchLocalAddr, auchHostIpAddress, wszHostName);

    /* Print Header Name */
    MaintIncreSpcCo(SPCO_CONSEC_ADR);
    MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* Minor Data Class          */              
                    0,                                      /* Report Name Address, uchRptAdr1, CLASS_PARAREPORTNAME       */
                    0,                                      /* Report Name Address, uchRptAdr2,  CLASS_PARAREPORTNAME       */
                    0,                                      /* Special Mnemonics Address, uchSpeAdr, CLASS_PARASPECIALMNEMO */
                    0,                                      /* Report Name Address, uchRptAdr3, CLASS_PARAREPORTNAME       */
                    2,                                      /* Reset Type, uchReportType               */
                    AC_JOIN_CLUSTER,                        /* A/C Number, uchPrgSupNo                */
                    0,                                      /* Reset Type, uchResetType                */
                    PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit         */
	
	//Check to see if the unjoin is ready to be unjoined
	sError = CheckUnjoinStatus();
	if(sError != SUCCESS){
		MaintMakePrintLine (_T("Unjoin Not Ready."), PRT_RECEIPT|PRT_JOURNAL);
		MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */
		PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_BAD_JOINSTATUS);
		PifLog(MODULE_ERROR_NO(MODULE_MAINT_LOG_ID), (USHORT)(abs(sError)));
		return sError;
	}

	sError = CliOpUnjoinCluster (wszHostName, auchHostIpAddress, &auchTerminalNo);

	if (sError != STUB_SUCCESS) {
		PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_FAILED_UNJOIN);
        sReturn = OpConvertError(sError);  

		MaintMakePrintLine (_T("Unjoin Error"), PRT_RECEIPT|PRT_JOURNAL);


        /* Print Error Code */
        MaintErrCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrCode.sErrorCode = sReturn;
        MaintErrCode.usPrtControl = PRT_RECEIPT | PRT_JOURNAL;
		if (RptCheckReportOnMld()) {
			MldDispItem(&MaintErrCode, 0);
		}
        PrtPrintItem(NULL, &MaintErrCode);
	}
	else {
		TCHAR   atchDeviceName[SCF_USER_BUFFER];
		TCHAR   atchDllName[SCF_USER_BUFFER];
		TCHAR   *pDeviceName;
		TCHAR   *pDllName;
		DWORD   dwRet;

		pDeviceName = atchDeviceName;
		pDllName    = atchDllName;

		_tcscpy (pDeviceName, SCF_DEVICENAME_JOIN_JOINSTATUS);
		_tcscpy (pDllName, SCF_DLLNAME_JOINSTATUS_UNJOINED);
		dwRet = ScfSetActiveDevice(SCF_TYPENAME_DISCONNECTED_SAT, atchDeviceName, atchDllName);

		PrtPrintLine((PRT_JOURNAL | PRT_RECEIPT), _T("Unjoin Complete"));

		TtlReinitialTotalFiles();
		
		MaintSFRemoval();
	}
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);        /* Make Trailer */

    return(sReturn);
}

/**
*	CheckUnjoinStatus() check to see if the terminal is in condition to
*   perform an Unjoin.
*
*   We are interested in the following conditions:
*       Store and Forward
*           Is the terminal in the middle of doing a Forwarding action?
*           Does the terminal have Stored Transactions that have not been processed?
*/
SHORT CheckUnjoinStatus(VOID)
{
	SHORT sStatus = SUCCESS;

	if (OpSignInStatus () != 0) {
		sStatus = LDT_PROHBT_ADR;    //  CAS_NOT_ALLOWED returned from CheckUnjoinStatus()
		PifLog(MODULE_PIF_LOADFARDATA, LOG_ERROR_CASHIER_SIGN_OUT);
	}

	return sStatus;
} 

//Removes the Approved files in the Store and Forward file
SHORT MaintSFRemoval(VOID){
return XEEptUnjoinClear(); 
}
/* --- End of Source File --- */