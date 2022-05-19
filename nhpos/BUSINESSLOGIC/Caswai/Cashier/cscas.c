/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server CASHIER module, Program List                        
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCAS.C                                            
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as floolws.
*
*    CasInit();              * cashier function initiarize  *
*    CasCreatFile();         * cashier file create *
*    CasCheckAndCreatFile()  * cashier check and create file *
*    CasCheckAndDeleteFile() * cashier check and delete file *
*    CasSignIn();            * signin in function *
*    CasSignOut();           * signout function *
*    CasClose();             * close function *
*    CasAssign();            * cashier assign function *
*    CasDelete();            * cashier delete function *
*    CasSecretClr();         * clear secret code *
*    CasAllIdRead();         * read all cashier id *
*    CasIndRead();           * read cashier paramater individually *
*    CasIndLock();           * lock cashier individually *
*    CasIndUnLock();         * unlock cashier individually *
*    CasAllLock();           * lock all cashier *
*    CasAllUnLock();         * unlock all cashier *
*    CasIndTermLock();       * lock terminal individually R3.1 *
*    CasIndTermUnLock();     * unlock terminal individually R3.1 *
*    CasAllTermLock();       * lock all terminal R3.1 *
*    CasAllTermUnLock();     * unlock all terminal R3.1 *
*    CasChkSignIn();         * check sign in *
*    CasSendFile();          * send file for back up *
*    CasRcvFile();           * receive file for back up *
*                            
*       Cas_Abort();               * Saved memory size
*       Cas_OpenFile();            * Saved memory size
*       Cas_GetHeader();
*       Cas_PutHeader();
*       Cas_Index();
*       Cas_IndexDel();
*       Cas_ParaRead();
*       Cas_ParaWrite();
*       Cas_EmpBlkGet();
*       Cas_BlkEmp();                                         
*       Cas_IndLockCheck();
*       Cas_LockCheck();
*       Cas_IndTermLockCheck(); R3.1
*       Cas_SignInCheck();
*       Cas_IndSignInCheck();
*       Cas_CloseFileReleaseSem();
*       Cas_WriteFile();
*       Cas_ReadFile();
*       Cas_CompIndex();
*
*    CasPreSignOut();              * pre signout function *
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*                                 
*    Date      Ver.Rev :NAME       :Description
*    May-06-92:00.00.01:M.YAMAMOTO :Initial
*    Oct-19-92:00.01.09:H.YAMAGUCHI:Modify CasSignIn because FVT Commemts
*    Oct-28-92:00.01.10:H.YAMAGUCHI:Adds CasCheckAndCreatFile, CasCheckAndDeleteFile
*    Nov-30-92:01.00.00:H.YAMAGUCHI:Adds CasPreSignOut
*    Jun-06-93:00.00.01:H.YAMAGUCHI:Change No of cashier from 16 to 32
*    Aug-25-93:00.00.01:H.YAMAGUCHI:Adds Cas_Abort/Cas_OpenFile
*    Dec-06-95:03.01.00:M.OZAWA    :Adds CasIndTermLock/CasIndTermUnLock/
*                                   CasAllTermLock/CasAllTermUnLock/
*                                   Cas_IndTermLockCheck
*    Jun-28-98:03.03.00:M.OZAWA    :Speed up Sign-In/Out Performance
*                                   for cashier interrupt
*    Aug-03-98:03.03.00:M.OZAWA    :Enhanced to R3.3 Cashier feature
*
** NCR2171 **
*    Aug-26-99:01.00.00:M.Teraki   :initial (for 2171)
*    Dec-01-99:01.00.00:hrkato     :Saratoga
** GenPOS Rel 2.2
*    Jan-30-15:00.00.01:R.CHAMBERS :Removed Cas_Abort replacing with PifAbort
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <math.h>
#include    <string.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "pif.h"
#include    "uie.h"
#include    "paraequ.h"
#include    "cscas.h"
#include    "rfl.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csstbcas.h"
#include    "appllog.h"
#include    "cscasin.h"
#include	"BlFWif.h"
#include	"display.h"
#include	"transact.h"
//#include	"DFPRExports.h"

static TCHAR   auchCAS_CASHIER[] = _T("PARAMCAS");

static PifSemHandle  husCashierSem = PIF_SEM_INVALID_HANDLE;        /* semaphore guarding access to cashier file */
static SHORT   hsCashierFileHandle = -1;
static ULONG   ausCasTerminalTbl[CAS_TERMINAL_NO_EXTENDED];
static DATE_TIME  datetimeCasTerminalTbl[CAS_TERMINAL_NO_EXTENDED];
static ULONG   ausCasPreIndTerminalTbl[CAS_TERMINAL_NO_EXTENDED];	/* avoid ac233 reset during pre-sign-out */
/* USHORT  ausCasPreTerminalTbl[CAS_TERMINAL_NO_EXTENDED];  / R3.3 */
static ULONG   ausNwCasPreTerminalTbl[CAS_NUMBER_OF_MAX_CASHIER]; 
static ULONG   ausCasWaiTerminalTbl[CAS_TERMINAL_NO_EXTENDED];  /* V3.3 */
static DATE_TIME  datetimeCasWaiTerminalTbl[CAS_TERMINAL_NO_EXTENDED];
static ULONG   ausCasWaiPreIndTerminalTbl[CAS_TERMINAL_NO_EXTENDED];	/* avoid ac233 reset during pre-sign-out */
/* USHORT  ausCasWaiPreTerminalTbl[CAS_TERMINAL_NO_EXTENDED];  / V3.3 */
static ULONG 	ausCasDBTerminalTbl[CAS_TERMINAL_NO_EXTENDED];			//Delayed Balance JHHJ

static ULONG   ulCasIndLockMem = 0;           
static UCHAR   uchCasAllLockMem = 0;
/* R3.3, replaced by auchCasPreTerminalTbl[] */
/* USHORT  usCasPreClose;           */
static CHAR    chCasIndTermLockMem = 0;    /* R3.1 */

static SHORT   Cas_OpenFile(VOID);
static VOID    Cas_GetHeader(CAS_FILEHED *CasFileHed);
static VOID    Cas_PutHeader(CAS_FILEHED *CasFileHed);
static SHORT   Cas_Index(CAS_FILEHED *CasFileHed, USHORT usIndexMode, ULONG ulCashierNo, USHORT *usParaBlockNo);
static SHORT   Cas_IndexDel(CAS_FILEHED *CasFileHed, ULONG ulCashierNo);
static VOID    Cas_ParaRead(CAS_FILEHED CONST *CasFileHed, USHORT usParablockNo, CAS_PARAENTRY *pArg);
static VOID    Cas_ParaWrite(CAS_FILEHED CONST *CasFileHed, USHORT usParaBlockNo, CAS_PARAENTRY *pArg);
static SHORT   Cas_EmpBlkGet(CAS_FILEHED CONST *CasFileHed, USHORT *usParaBlcokNo);
static VOID    Cas_BlkEmp(CAS_FILEHED CONST *CasFileHed, USHORT usParaBlockNo);                                         
static SHORT   Cas_IndLockCheck(ULONG ulCashierNo);
static SHORT   Cas_LockCheck(VOID);
static SHORT   Cas_IndTermLockCheck(USHORT usTerminalNo);  /* R3.1 */
static SHORT   Cas_SignInCheck(VOID);
static SHORT   Cas_IndSignInCheck(ULONG ulCashierNo);
static SHORT   Cas_IndPreSignInCheck(ULONG ulCashierNo);  /* R3.3 */
static VOID    Cas_CloseFileReleaseSem(VOID);
static VOID    Cas_WriteFile(ULONG offulFileSeek,VOID *pTableHeadAddress, USHORT cusSizeofWrite);
static SHORT   Cas_ReadFile(ULONG offulFileSeek, VOID *pTableHeadAddress, ULONG ulSizeofRead);

static SHORT   Cas_SearchFile(USHORT offusTableHeadAddress, 
                       USHORT usTableSize, 
                       USHORT *poffusSearchPoint,
                       VOID   *pSearchData, 
                       USHORT usSearchDataSize, 
                       SHORT (*Comp)(VOID *pKey, VOID *pusCompKey));
static VOID   Cas_InsertTable( USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusInsertPoint, 
                        VOID   *pInsertData, 
                        USHORT usInsertDataSize);
static VOID    Cas_DeleteTable(USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusDeletePoint, 
                        USHORT usDeleteDataSize);

/*
*==========================================================================
**    Synopsis:    SHORT    Cas_CompIndex(USHORT *pusKey, CAS_INDEXENTRY *pusHitPoint )
*
*       Input:    USHORT         *pusKey            Target key
*                 CAS_INDEXENTRY *pusHitPoint     Checked key
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    0x00   : Match
*                < 0x00 : Target key > Checked key
*                > 0x00 : Target key < Checked key    
*
**  Description:
*               Compare routine returning an indication as follows:
*                 - negative then pusKey is less than cashier no at hit point
*                 - zero then pusKey is equal to cashier no at hit point
*                 - positive then pusKey is greater than cashier no at hit point
*==========================================================================
*/
static SHORT    Cas_CompIndex(ULONG *pusKey, CAS_INDEXENTRY *pusHitPoint)
{
    LONG  lKeyDifference = (((LONG)(*pusKey) - (LONG)pusHitPoint->ulCashierNo));
	SHORT sKeyDifference = 0;

	if (lKeyDifference < 0)
		sKeyDifference = -1;
	else if (lKeyDifference > 0)
		sKeyDifference = 1;

    return sKeyDifference;
}

static SHORT    Cas_OpenFileGetHeader (CAS_FILEHED  *pCasFileHed)
{
	SHORT  sRet;

    /* File Open */
    if ((sRet = Cas_OpenFile()) >= 0) {
		// was able to open the Cashier file successfully so now lets
		// read in the header of the file.
		Cas_ReadFile(CAS_FILE_HED_POSITION, pCasFileHed, sizeof(CAS_FILEHED));
    }

	return sRet;
}

static VOID  Cas_AssignCasparaToCasif (CASIF *pCasif, CAS_PARAENTRY *pCaspara)
{
	// do not modify the following elements of the CASIF struct which are not part of the
	// Cashier Record but are instead run time elements of current Signed-in Cashier. 
	//    ulCashierNo
	//    ulCashierOption
	//    usUniqueAddress

	memcpy(pCasif->fbCashierStatus, pCaspara->fbCashierStatus, PARA_LEN_CASHIER_STATUS);
	pCasif->usGstCheckStartNo	= pCaspara->usGstCheckStartNo;
	pCasif->usGstCheckEndNo		= pCaspara->usGstCheckEndNo;
	pCasif->uchChgTipRate		= pCaspara->uchChgTipRate;
	pCasif->uchTeamNo			= pCaspara->uchTeamNo;
	pCasif->uchTerminal			= pCaspara->uchTerminal;
	_tcsncpy(pCasif->auchCashierName, pCaspara->auchCashierName, PARA_CASHIER_LEN);
	pCasif->ulCashierSecret		= pCaspara->ulCashierSecret; 
	pCasif->usSupervisorID		= pCaspara->usSupervisorID;		//new in version 2.2.0
	pCasif->usCtrlStrKickoff	= pCaspara->usCtrlStrKickoff;	//new in version 2.2.0
	pCasif->usStartupWindow		= pCaspara->usStartupWindow;		//new in version 2.2.0
	pCasif->ulGroupAssociations = pCaspara->ulGroupAssociations;	//new in version 2.2.0
}


static VOID  Cas_AssignCasifToCaspara (CAS_PARAENTRY *pCaspara, CASIF *pCasif)
{
	memcpy (pCaspara->fbCashierStatus, pCasif->fbCashierStatus, PARA_LEN_CASHIER_STATUS);
    pCaspara->usGstCheckStartNo		= pCasif->usGstCheckStartNo;
    pCaspara->usGstCheckEndNo		= pCasif->usGstCheckEndNo;
    pCaspara->uchChgTipRate			= pCasif->uchChgTipRate;
    pCaspara->uchTeamNo				= pCasif->uchTeamNo;
    pCaspara->uchTerminal			= pCasif->uchTerminal;
    _tcsncpy (pCaspara->auchCashierName, pCasif->auchCashierName, PARA_CASHIER_LEN);
    pCaspara->ulCashierSecret		= pCasif->ulCashierSecret;
	pCaspara->usSupervisorID		= pCasif->usSupervisorID;		//new in version 2.2.0
	pCaspara->usCtrlStrKickoff		= pCasif->usCtrlStrKickoff;		//new in version 2.2.0
	pCaspara->usStartupWindow		= pCasif->usStartupWindow;		//new in version 2.2.0
	pCaspara->ulGroupAssociations	= pCasif->ulGroupAssociations;	//new in version 2.2.0
}

/*
*==========================================================================
**    Synopsis:   VOID    CasInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Initialize for Cashier Module.
*               Save semaphore handle. 
*==========================================================================
*/
VOID    CasInit(VOID)
{
    /* Create Semaphore */
    husCashierSem = PifCreateSem( CAS_COUNTER_OF_SEM );  /* save semaphore handle */
	PifSetTimeoutSem(husCashierSem, 5000);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasCreatFile(UCHAR uchNumberOfCashier)
*
*       Input:    UCHAR uchNumberOfCashier - Number of Cashier
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*          Normal End:   CAS_PERFORM
*        Abnormal End:   CAS_NO_MAKE_FILE
*
**  Description:
*               Create Cashier File. V3.3
*==========================================================================
*/
SHORT   CasCreatFile(USHORT usNumberOfCashier)
{
	CAS_FILEHED CasFileHed = {0};             /* Cashier file header */
    ULONG ulActualPosition;             /* for PifSeekFile */

    PifRequestSem(husCashierSem);       /* Request Semaphore */

    PifDeleteFile(auchCAS_CASHIER);     /* Delete Cashier File  Ignore error */

    /*
		Check NumberOfCashier to see if we should just delete the cashier file
		without creating a new one (number of cashiers == 0) or if we now need
		to create a new cashier with room for the requested number of cashiers.
	 */
    if (0 == usNumberOfCashier) {
        PifReleaseSem(husCashierSem);
        return(CAS_PERFORM);
    }

    /* Open Cashier File */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchNEW_FILE_WRITE)) < 0) {  /* pif_error_file_exist */
		char  xBuff[128];
		sprintf (xBuff, "**ERROR: CasCreatFile() - Cashier file open new error PifOpenFile() %d", hsCashierFileHandle);
		NHPOS_ASSERT_TEXT(0, xBuff);
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_OPEN);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(hsCashierFileHandle));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_OPEN);
    }
    
    /* Make File Header */
    CasFileHed.usNumberOfMaxCashier = usNumberOfCashier;
    CasFileHed.usNumberOfResidentCashier = 0x00;
    CasFileHed.offusOffsetOfIndexTbl = sizeof(CAS_FILEHED);
    CasFileHed.offusOffsetOfParaEmpTbl = CasFileHed.offusOffsetOfIndexTbl + (usNumberOfCashier * sizeof(CAS_INDEXENTRY));
    CasFileHed.offusOffsetOfParaTbl = CasFileHed.offusOffsetOfParaEmpTbl + CAS_PARA_EMPTY_TABLE_SIZE;
    CasFileHed.ulCasFileSize = (ULONG)CasFileHed.offusOffsetOfParaTbl + ((ULONG)usNumberOfCashier * sizeof(CAS_PARAENTRY));

    /* Check file size and Creat file */
    if (PifSeekFile(hsCashierFileHandle, CasFileHed.ulCasFileSize, &ulActualPosition) < 0) {
        PifCloseFile(hsCashierFileHandle);
		hsCashierFileHandle = -1;           // set file handle to invalid value after PifCloseFile()
        PifDeleteFile(auchCAS_CASHIER);
        PifReleaseSem(husCashierSem);
        return(CAS_NO_MAKE_FILE);
    }

    /* Write File Header */
    Cas_PutHeader(&CasFileHed);

    /* Clear Empty table and Write */
	{
		UCHAR auchParaEmpTable[CAS_PARA_EMPTY_TABLE_SIZE] = {0}; /* Casheir Paramater empty table */

		Cas_WriteFile((ULONG)CasFileHed.offusOffsetOfParaEmpTbl, auchParaEmpTable, CAS_PARA_EMPTY_TABLE_SIZE);
	}

	//initialize the fingerprint file
	//DFPRInitFile(L"DigitalPersona 4500",3);

    /* Close File and Release Semaphoer */
    Cas_CloseFileReleaseSem();
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasCheckAndCreatFile(UCHAR uchNumberOfCashier)
*
*       Input:    UCHAR uchNumberOfCashier - Number of Cashier
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*          Normal End:   CAS_PERFORM
*        Abnormal End:   CAS_NO_MAKE_FILE
*
**  Description:
*               Check if not exist , then Create Cashier File.
*==========================================================================
*/
SHORT   CasCheckAndCreatFile(USHORT usNumberOfCashier)
{
    SHORT   sStatus;

    /* Open Cashier File */
    if ((sStatus = Cas_OpenFile()) < 0) {  /* pif_error_file_exist */
        sStatus = CasCreatFile(usNumberOfCashier);
	} else {
		// file already exists so do not create it.
		PifCloseFile(hsCashierFileHandle);
		hsCashierFileHandle = -1;           // set file handle to invalid value after PifCloseFile()
		sStatus = CAS_PERFORM;              // indicate everything is fine.
	}

    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   CasCheckAndDeleteFile(UCHAR uchNumberOfCashier)
*
*       Input:    UCHAR uchNumberOfCashier - Number of Cashier
*      Output:    nothing
*       InOut:    nothing
*
**  Return    :
*            Normal End: CAS_PERFORM    
*                        CAS_DELETE_FILE
**  Description:
*               Check cashier file, if not equal then delete file.
*==========================================================================
*/
SHORT   CasCheckAndDeleteFile(USHORT usNumberOfCashier)
{
	SHORT       sOpenStatus = 0;
	CAS_FILEHED CasFileHed = {0};     /* cashier file header */

    if ((sOpenStatus = Cas_OpenFileGetHeader(&CasFileHed)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_DELETE_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sOpenStatus));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_PERFORM);
	} else if (sOpenStatus < 0) {
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_DELETE_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sOpenStatus));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
	}

    PifCloseFile(hsCashierFileHandle);
	hsCashierFileHandle = -1;           // set file handle to invalid value after PifCloseFile()

    if ( CasFileHed.usNumberOfMaxCashier != usNumberOfCashier) {
        PifDeleteFile(auchCAS_CASHIER);   /* Delete Cashier File  Ignore error */
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_DELETE_FILE);
        return(CAS_DELETE_FILE);
    }
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasSignIn(CASIF *pCasif)
*
*       Input:    Casif->ulCashierNo
*                 Casif->ulCashierSecret
*                 Casif->usUniqueAddress
*                 Casif->ulCashierOption
*      Output:    Casif->auchCashierStatus
*                 Casif->auchCashierName
*       InOut:
*
**  Return    :
*            Normal End: CAS_PERFORM    
*          Abnormal End: CAS_LOCK
*                        CAS_NOT_IN_FILE
*                        CAS_SECRET_UNMATCH
*                        CAS_ALREADY_OPENED(Prohibit Multi open at Cluster system)
*
**  Description:
*               Cashier sign-in function, Prohibit Multi open at Cluster system.
*               This function is called with two primary pieces of data:
*                 - ulCashierNo which specifies the operator/employee id
*                 - usUniqueAddress which specifies the terminal number to be signed into
*
*               In addition the following information may be specified:
*                 - ulCashierOption specifying if special operator such as bar or surrogate
*                 - ulCashierSecret specifying secret code or PIN if required by provisioning
*
*               Output of this function is Cashier/Operator data such as operator mnemonic,
*               
*==========================================================================
*/
SHORT   CasSignIn(CASIF *pCasif)
{
    USHORT      usParaBlockNo;      /* paramater block # */
    USHORT      i;
	SHORT       sRetVal;
	int			unlockStatus;	/* License status - TLDJR */
	CAS_FILEHED CasFileHed = {0};     /* cashier file header */

#ifndef VBOLOCK
//Set the pointer for security
//then do a license status to cause the pointer
//to be updated by an event in the security control
	BlFwIfSecStatusProp(ulCheck);
	unlockStatus = BlFwIfLicStatus(); // unlockStatus isn't used but this function returns an int
//using the Macros CHECKGUARDBITS and CHECKSECBITS
//to determine if the application has a valid
//security number
	CHECKGUARDBITS(ulGuard, ulCheck)
	CHECKSECBITS(ulCheck);
#else 	
	unlockStatus = BlFwIfLicStatus();
	switch(unlockStatus)
	{
		case UNREGISTERED_COPY:return UNREGISTERED_COPY;
		case LICENSE_MISSING:  return UNREGISTERED_COPY;
		case DEMO_MODE: break;
	}
#endif
    PifRequestSem(husCashierSem);    /* Request Semaphore */

    /* Cashier individual lock check */    
    if ((Cas_IndLockCheck(pCasif->ulCashierNo)) != CAS_UNLOCK) {
        PifReleaseSem(husCashierSem);
        return(CAS_LOCK);
    }

    /* ---- check terminal lock status for reset report R3.1 ---- */
    if ((Cas_IndTermLockCheck(pCasif->usUniqueAddress)) != CAS_UNLOCK) {
        PifReleaseSem(husCashierSem);
        return(CAS_LOCK);
    }

    /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    /* Check Cashier assign */
    if ((Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, pCasif->ulCashierNo, &usParaBlockNo)) == CAS_NOT_IN_FILE) {
        Cas_CloseFileReleaseSem();
        return(CAS_NOT_IN_FILE);        
    }

    /* check surrogate sign-in status, V3.3 */
    if (pCasif->ulCashierOption & (CAS_WAI_SIGN_IN|CAS_MISC_SIGN_IN)) {
        if ( ausCasWaiTerminalTbl[pCasif->usUniqueAddress - 1] != 0 ) {
            if ( ausCasWaiTerminalTbl[pCasif->usUniqueAddress - 1] == pCasif->ulCashierNo ) {
            
                /* Cas_CloseFileReleaseSem();
                return(CAS_DURING_SIGNIN); */

            /* } else
            if ( ausCasWaiPreTerminalTbl[pCasif->usUniqueAddress - 1] ==
                                        pCasif->usCashierNo ) {
            
                Cas_CloseFileReleaseSem();      / close file and release semaphore /
                return(CAS_DURING_SIGNIN);
			*/
            }  else {
                if ( ausCasWaiTerminalTbl[pCasif->usUniqueAddress - 1] != pCasif->ulCashierNo  ) {
	    			for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
				        if (ausCasTerminalTbl[i] == pCasif->ulCashierNo) {
			                Cas_CloseFileReleaseSem();
	    	                return(CAS_ALREADY_OPENED);
				        }
				        if (ausCasWaiTerminalTbl[i] == pCasif->ulCashierNo) {
			                Cas_CloseFileReleaseSem();
    	    	            return(CAS_ALREADY_OPENED);
			    	    }
			    	}
                }
            }
        } else {
            /* Check another terminal */
    		for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
	    	    if (ausCasTerminalTbl[i] == pCasif->ulCashierNo) {
	                Cas_CloseFileReleaseSem();
            	    return(CAS_ALREADY_OPENED);
		        }
		        if (ausCasWaiTerminalTbl[i] == pCasif->ulCashierNo) {
	                Cas_CloseFileReleaseSem();
        	        return(CAS_ALREADY_OPENED);
	        	}
		    }
        }
    }

    /* Check Cashier sign-out */

    /* R3.3 */
    /* check only whether sign-in cashier no exists on terminal table */
    /* ignor another cashier no exits on pre-teminal table */
    if ( ausCasTerminalTbl[pCasif->usUniqueAddress - 1] != 0 ) {
        if ( ausCasTerminalTbl[pCasif->usUniqueAddress - 1] == pCasif->ulCashierNo ) {
            
            /* Cas_CloseFileReleaseSem();      / close file and release semaphore */
            /* return(CAS_DURING_SIGNIN); */

        /* } else
        if ( ausCasPreTerminalTbl[pCasif->usUniqueAddress - 1] ==
                                    pCasif->usCashierNo ) {
            
            Cas_CloseFileReleaseSem();      / close file and release semaphore /
            return(CAS_DURING_SIGNIN);
        */
        
        }  else {
            if ( ausCasTerminalTbl[pCasif->usUniqueAddress - 1] != pCasif->ulCashierNo  ) {
	        	/* 06/02/01, allow sign-in during pre-signout */
    			for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
			        if (ausCasTerminalTbl[i] == pCasif->ulCashierNo) {
		                Cas_CloseFileReleaseSem();
    	                return(CAS_ALREADY_OPENED);
			        }
			        if (ausCasWaiTerminalTbl[i] == pCasif->ulCashierNo) {
		                Cas_CloseFileReleaseSem();
    	                return(CAS_ALREADY_OPENED);
			        }
			    }
            }
        }
    } else {
        /* Check another terminal */
        /* 06/02/01, allow sign-in during pre-signout */
    	for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
	        if (ausCasTerminalTbl[i] == pCasif->ulCashierNo) {
                Cas_CloseFileReleaseSem();
                return(CAS_ALREADY_OPENED);
	        }
	        if (ausCasWaiTerminalTbl[i] == pCasif->ulCashierNo) {
                Cas_CloseFileReleaseSem();
                return(CAS_ALREADY_OPENED);
	        }
	    }
    }

	{
		CAS_PARAENTRY Arg;          /* paramater table entry */

		//----------------------------------------------------------------------------------
		// Asserts for invariant conditions needed for successful use of this function

			NHPOS_ASSERT(sizeof(pCasif->fbCashierStatus) >= sizeof(Arg.fbCashierStatus));
			NHPOS_ASSERT(sizeof(pCasif->auchCashierName) >= sizeof(Arg.auchCashierName));

			// Asserts for array bounds are large enough
			NHPOS_ASSERT_ARRAYSIZE(pCasif->auchCashierName, PARA_CASHIER_LEN);
			NHPOS_ASSERT_ARRAYSIZE(Arg.auchCashierName, PARA_CASHIER_LEN);
			NHPOS_ASSERT_ARRAYSIZE(pCasif->fbCashierStatus, PARA_LEN_CASHIER_STATUS);
			NHPOS_ASSERT_ARRAYSIZE(Arg.fbCashierStatus, PARA_LEN_CASHIER_STATUS);

			// Asserts for array data types are the same for memcpy, memset, _tcsncpy, _tcsnset, etc
			NHPOS_ASSERT(sizeof(pCasif->fbCashierStatus[CAS_CASHIERSTATUS_0]) == sizeof(Arg.fbCashierStatus[0]));
		//-----------------------------------------------------------------------------------

		/* read Paramater */
		Cas_ParaRead(&CasFileHed, usParaBlockNo, &Arg);

			/* check secret code */
		if ((Arg.fbCashierStatus[CAS_CASHIERSTATUS_0] & CAS_OPERATOR_DISABLED_AM) == CAS_OPERATOR_DISABLED_AM) {  // Amtrak change to check if Operator is disabled
		   Cas_CloseFileReleaseSem();
			NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: Cashier Status == CAS_OPERATOR_DISABLED_AM");
			return (CAS_CAS_DISABLED);
		}
		if ((Arg.fbCashierStatus[CAS_CASHIERSTATUS_3] & CAS_OPERATOR_DISABLED) == CAS_OPERATOR_DISABLED) {  // New Operator disabled flag for Rel 2.2.0
		   Cas_CloseFileReleaseSem();
			NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: Cashier Status == CAS_OPERATOR_DISABLED");
			return (CAS_CAS_DISABLED);
		}

		if (!(pCasif->ulCashierOption & CAS_MISC_SIGN_IN)) {    /* ignor misc. sign-in, V3.3 */
			/* check allowed terminal, V3.3 */
			if (Arg.uchTerminal) {
				if (Arg.uchTerminal != (UCHAR)pCasif->usUniqueAddress) {
					Cas_CloseFileReleaseSem();      /* close file and release semaphore */
					return (CAS_NOT_ALLOWED);
				}
			}

			/* check secret code */
			if (Arg.fbCashierStatus[CAS_CASHIERSTATUS_2] & CAS_PIN_REQIRED_FOR_SIGNIN) { /* V3.3 */
				if (pCasif->ulCashierSecret != 0x00) {
					if (Arg.ulCashierSecret != 0x00){
						if (Arg.ulCashierSecret != pCasif->ulCashierSecret) {
							Cas_CloseFileReleaseSem();
							return(CAS_SECRET_UNMATCH);  /* return secret code unmatch */
						}
					 } else { /* if secret code is not assign, assign secret code */ 
							Arg.ulCashierSecret = pCasif->ulCashierSecret;
							Cas_ParaWrite(&CasFileHed, usParaBlockNo, &Arg);
					 }
				} else {
					Cas_CloseFileReleaseSem();      /* close file and release semaphore */
					return(CAS_REQUEST_SECRET_CODE);    /* return request secret code */
				}
			}
		}

		/* 06/02/01, allow sign-in during pre-signout */
   		for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
			if (ausNwCasPreTerminalTbl[i] == pCasif->ulCashierNo) {
				ausNwCasPreTerminalTbl[i] = 0;
			}
		}

		/* make output condition */
		/* V3.3 */
		Cas_AssignCasparaToCasif (pCasif, &Arg);

		/*
		  *  set Cashier no in the appropriate terminal table.  terminal table
		  *  used depends on the ulCashierOption flags.
		  */
		if (pCasif->ulCashierOption & (CAS_WAI_SIGN_IN | CAS_MISC_SIGN_IN)) {
			ausCasWaiTerminalTbl[pCasif->usUniqueAddress - 1] = pCasif->ulCashierNo;
			PifGetDateTime (&datetimeCasWaiTerminalTbl[pCasif->usUniqueAddress - 1]);
		} else {
			ausCasTerminalTbl[pCasif->usUniqueAddress - 1] = pCasif->ulCashierNo;
			PifGetDateTime (&datetimeCasTerminalTbl[pCasif->usUniqueAddress - 1]);
		}
	}

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(CAS_PERFORM);
}

/*                 
*==========================================================================
**    Synopsis:    SHORT   CasSignOut(CASIF *pCasif)
*
*       Input:    pCasif->usCashierNo
*                 pCasif->usUniqueAddress
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_NOT_SIGNIN
*
**  Description:
*                Clear Terminal Table for Cashier Sign-out function
*==========================================================================
*/
SHORT   CasSignOut(CASIF *pCasif)
{
	USHORT i;

    PifRequestSem(husCashierSem);   /* Request Semaphore */
    
    /* 06/02/01, allow sign-in during pre-signout */
    for (i = 0; i < CAS_NUMBER_OF_MAX_CASHIER; i++) {
		if (ausNwCasPreTerminalTbl[i] == pCasif->ulCashierNo) {
			ausNwCasPreTerminalTbl[i] = 0;
		}
    }
    
    if (ausCasWaiPreIndTerminalTbl[pCasif->usUniqueAddress - 1]) {
        ausCasWaiPreIndTerminalTbl[pCasif->usUniqueAddress - 1] = 0x0000; /* Clear Termianl Table */
    } else {
        if (pCasif->ulCashierNo == ausCasPreIndTerminalTbl[pCasif->usUniqueAddress - 1]) {
            ausCasPreIndTerminalTbl[pCasif->usUniqueAddress - 1] = 0x0000; /* Clear Termianl Table */
        }
    }

	SerOpSetOperatorMessageStatusTable (pCasif->usUniqueAddress, 0x0002);  // indicate sign out for this terminal

    PifReleaseSem(husCashierSem);   /* Release Semaphore */
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasClose(USHORT usCashierNo)
*
*       Input:    USHORT usCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_NO_SIGNIN
*            
**  Description:
*                Enforce Sign-out
*==========================================================================
*/
SHORT   CasClose(ULONG ulCashierNo)
{
    USHORT  cusi, cusj;
	USHORT  i;

    PifRequestSem(husCashierSem);                   /* Request Semaphore */

    cusj = 0;
    for (cusi = 0; cusi < CAS_TERMINAL_NO_EXTENDED; cusi++) {
        if (ausCasTerminalTbl[cusi] == ulCashierNo) {
            ausCasTerminalTbl[cusi] = 0x0000;       /* Clear Terminal Table */
            cusj++;                                 /* Increment Number of Sign-in */
        }
        if (ausCasPreIndTerminalTbl[cusi] == ulCashierNo) {
            ausCasPreIndTerminalTbl[cusi] = 0x0000;       /* Clear Terminal Table */
            cusj++;                                 /* Increment Number of Sign-in */
        } 
        /* V3.3 */
        if (ausCasWaiTerminalTbl[cusi] == ulCashierNo) {
            ausCasWaiTerminalTbl[cusi] = 0x0000;       /* Clear Terminal Table */
            cusj++;                                 /* Increment Number of Sign-in */
        }
        if (ausCasWaiPreIndTerminalTbl[cusi] == ulCashierNo) {
            ausCasWaiPreIndTerminalTbl[cusi] = 0x0000;       /* Clear Terminal Table */
            cusj++;                                 /* Increment Number of Sign-in */
        }
    }
    /* 06/02/01, allow sign-in during pre-signout */
    for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
		if (ausNwCasPreTerminalTbl[i] == ulCashierNo) {
			ausNwCasPreTerminalTbl[i] = 0;
            cusj++;                                 /* Increment Number of Sign-in */
		}
    }
    PifReleaseSem(husCashierSem);
    if (!cusj) {                                    /* (cusj == 0) */
        return (CAS_NO_SIGNIN);                 /* return not sign-in */
    } else {
        return(CAS_PERFORM);                    /* return exist sign-in */
    }
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasAssign(CASIF *pCasif)
*
*       Input:      pCasif->usCashierNo
*                   pCasif->auchCashierName
*                   pCasif->uchCashierSecret
*                   pCasif->fbCashierStatus
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_CASHIER_FULL
*                           CAS_LOCK
*
**  Description:
*                Add, Regist Inputed Cashier.
*==========================================================================
*/
SHORT   CasAssign(CASIF *pCasif)
{
    USHORT      usParaBlockNo;
	SHORT       sRetVal;
    CAS_PARAENTRY   Arg;
    CAS_FILEHED CasFileHed;

	//----------------------------------------------------------------------------------
	// Asserts for invariant conditions needed for successful use of this function

		NHPOS_ASSERT_ARRAYSIZE(Arg.auchCashierName, PARA_CASHIER_LEN);
		NHPOS_ASSERT_ARRAYSIZE(pCasif->auchCashierName, PARA_CASHIER_LEN);
		NHPOS_ASSERT(sizeof(Arg.auchCashierName[0]) == sizeof(pCasif->auchCashierName[0]));

	//----------------------------------------------------------------------------------

    PifRequestSem(husCashierSem);

    /* CHECK LOCK */
    if (Cas_IndLockCheck(pCasif->ulCashierNo) != CAS_UNLOCK) {
        PifReleaseSem(husCashierSem);
        return(CAS_LOCK);
    }

    /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

	/* Check Assign Cashier */
    if (Cas_Index(&CasFileHed, CAS_REGIST, pCasif->ulCashierNo, &usParaBlockNo) == CAS_CASHIER_FULL) {
		Cas_CloseFileReleaseSem();
		return(CAS_CASHIER_FULL);   /* return if assign cashier full */
    }

    /* Make Paramater Table from input condition and Write to file */
    /* V3.3 */
	Cas_AssignCasifToCaspara (&Arg, pCasif);

    Cas_ParaWrite(&CasFileHed, usParaBlockNo, &Arg);

    Cas_CloseFileReleaseSem();  /* file close and release semaphore */
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasDelete(USHORT usCashierNo)
*
*       Input:    USHORT usCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*                           CAS_NOT_IN_FILE
*
**  Description:
*                Delete Inputed Cashier No from index table
*==========================================================================
*/
SHORT   CasDelete(ULONG ulCashierNo)
{
    SHORT       sRetVal;           /* error status save area */
	CAS_FILEHED CasFileHed = {0};         /* file header */

    PifRequestSem(husCashierSem);   /* request semaphore */

    /* CHECK SIGN-IN */
    if (Cas_IndSignInCheck(ulCashierNo) == CAS_DURING_SIGNIN) {
        PifReleaseSem(husCashierSem);
        return(CAS_DURING_SIGNIN);
    }

    /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    /* Check cashier assign and delete */
    if (Cas_IndexDel(&CasFileHed, ulCashierNo) == CAS_NOT_IN_FILE) {
        sRetVal = CAS_NOT_IN_FILE;
    } else {
        sRetVal = CAS_PERFORM; 
    }

    Cas_CloseFileReleaseSem();      /* file close and releoase semaphore */
    return(sRetVal);        
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasSecretClr(USHORT usCashierNo)
*
*       Input:    USHORT usCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*                           CAS_NOT_IN_FILE
*
**  Description:
*                Clear Secret Code of Cashier No when inputted.
*==========================================================================
*/
SHORT   CasSecretClr(ULONG ulCashierNo)
{
    SHORT           sRetVal;           /* error status save area */
    USHORT          usParaBlockNo;          /* paramater block entry save area */
    CAS_PARAENTRY   Arg;            /* Paramater table entry save area */
    CAS_FILEHED     CasFileHed;         /* file header save area */

    PifRequestSem(husCashierSem);   /* request samaphore */

    /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    /* check cashier assgin */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, ulCashierNo, &usParaBlockNo) != CAS_RESIDENT) {
        sRetVal = CAS_NOT_IN_FILE;  
    } else if (Cas_IndSignInCheck(ulCashierNo) == CAS_DURING_SIGNIN) {
        sRetVal = CAS_DURING_SIGNIN;                    
    } else { 
        Cas_ParaRead(&CasFileHed, usParaBlockNo, &Arg);     /* GET PARAMATER W/ SECRET CODE */
        Arg.ulCashierSecret = 0x00;                        /* CLEAR SECRET CODE */
        Cas_ParaWrite(&CasFileHed, usParaBlockNo, &Arg);    /* WRITE CLEARD SECRET CODE */
        sRetVal = CAS_PERFORM;
    }

    Cas_CloseFileReleaseSem();                                /* close file and release semaphore */
    return(sRetVal);
}

/*
*==========================================================================
**    Synopsis:   SHORT  CasAllIdRead(USHORT usRcvBufferSize, USHORT *ausRcvBuffer)
*
*       Input:    USHORT usRcvBufferSize
*      Output:    
*       InOut:    ULONG *aulRcvBuffer
*
**  Return    :
*           Number of Cashier          * 0 > 
*           CAS_FILE_NOT_FOUND         * 0 <
*
**  Description:
*                Read all Cashier No. in this file. V3.3
*==========================================================================
*/
SHORT  CasAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer)
{
    SHORT          cusi;
	SHORT          sRetVal;
    ULONG          ulOffset;
    CAS_FILEHED    CasFileHed;         /* file header */
    CAS_INDEXENTRY CasIndexEntry;                               /* index table entry */ 

    PifRequestSem(husCashierSem);       /* request semaphore */

    /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    ulOffset = CasFileHed.offusOffsetOfIndexTbl;

    /* Count inputed cashier no in index table */
    for (cusi = 0 ;((cusi * sizeof(CasIndexEntry.ulCashierNo)) < usRcvBufferSize) &&
        (cusi < CasFileHed.usNumberOfResidentCashier); cusi++) {

        /* get index table, V3.3 */
        Cas_ReadFile(ulOffset, &CasIndexEntry, sizeof(CasIndexEntry));

        *aulRcvBuffer++ = CasIndexEntry.ulCashierNo;

        ulOffset += sizeof(CasIndexEntry);
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(cusi);
}

SHORT  CasAllSignedInRead(USHORT usRcvBufferLen, CAS_TERM_SIGNIN *aulRcvBuffer)
{
	USHORT usTermNo = 0;
	USHORT usCount = 0;

	aulRcvBuffer->usTermNo = 0;
	aulRcvBuffer->ulCashierNo = 0;
	for (usTermNo = 0; usCount <= usRcvBufferLen && usTermNo < CAS_TERMINAL_NO_EXTENDED; usTermNo++) {
		if (ausCasTerminalTbl[usTermNo]) {
			aulRcvBuffer->usTermNo = usTermNo + 1;
			aulRcvBuffer->ulCashierNo = ausCasTerminalTbl[usTermNo];
			aulRcvBuffer->dtSignInDateTime = datetimeCasTerminalTbl[usTermNo];
			aulRcvBuffer++;
			usCount++;
		}
	}
	return usCount;
}
                                                                                                                                               
/*
*==========================================================================
**    Synopsis:    SHORT   CasIndRead(CASIF *pCasif)
*
*       Input:    pCasif->usCashierNo     
*      Output:    pCasif->ucharCashierStatus
*                 pCasif->auchCashierName
*                 pCasif->uchCashierSecret
*       InOut:
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_NOT_IN_FILE
*
**  Description:
*                Read Paramater of inputed Cashier individually.
*==========================================================================
*/
SHORT   CasIndRead(CASIF *pCasif)
{
    USHORT  usParaBlockNo;              /* paramater block no save area */
    SHORT   sRetVal;                    /* error status save area */
    CAS_FILEHED CasFileHed;             /* file header save area */

    PifRequestSem(husCashierSem);       /* request samaphore */

     /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    /* Search inputed cashier no in Index table */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, pCasif->ulCashierNo, &usParaBlockNo) == CAS_RESIDENT) {
		CAS_PARAENTRY CasParaEntry;         /* paramater entry table save area */

        /* make output condition */
        /* V3.3 */
        Cas_ParaRead(&CasFileHed, usParaBlockNo, &CasParaEntry);

		Cas_AssignCasparaToCasif (pCasif, &CasParaEntry);

        sRetVal = CAS_PERFORM;
    } else {
        sRetVal = CAS_NOT_IN_FILE;
    }
    Cas_CloseFileReleaseSem();          /* close file and release semaphore */
    return(sRetVal);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasIndLock(USHORT usCashierNo)
*
*       Input:    USHORT    usCashierNo
*      Output:
*       InOut:
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*                           CAS_NOT_IN_FILE
*                           CAS_OTHER_LOCK
*
**  Description:
*                Lock inputted Cashier
*==========================================================================
*/
SHORT   CasIndLock(ULONG ulCashierNo)
{
    SHORT       sRetVal;           /*  error status save area */
    USHORT      usParaBlockNo;     /*  paramater block no save area */
    CAS_FILEHED CasFileHed;        /*  file header save area */

    PifRequestSem(husCashierSem);   /* request samaphore */

     /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    /* search inputed cashier no and lock */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, ulCashierNo, &usParaBlockNo) == CAS_RESIDENT) {
        if (Cas_LockCheck() == CAS_LOCK) {
            sRetVal = CAS_OTHER_LOCK;
        } else {
            if (Cas_IndSignInCheck(ulCashierNo) == CAS_DURING_SIGNIN) {
                sRetVal = CAS_DURING_SIGNIN;  /* inputed cashier during signin */
            } else {
                ulCasIndLockMem = ulCashierNo;
                sRetVal = CAS_PERFORM;        /* inputed cashier not sign in */
            }
        }
    } else {
        sRetVal = CAS_NOT_IN_FILE;       
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sRetVal);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasIndUnLock(USHORT usCashierNo)
*
*       Input:    USHORT usCashierNo
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DIFF_NO
*
**  Description:
*                UnLock inputted Cashier
*==========================================================================
*/
SHORT   CasIndUnLock(ULONG ulCashierNo)
{
    SHORT   sErrorStatus;       /* error status save area */

    PifRequestSem(husCashierSem);       /* requse semaphore */

    /* check lock */
    if (ulCasIndLockMem == ulCashierNo) {
        ulCasIndLockMem = 0x00;
        sErrorStatus = CAS_PERFORM;     /* not other cashier is locked */
    } else {
        sErrorStatus = CAS_DIFF_NO;     /* other cashier is locked */
    }
    PifReleaseSem(husCashierSem);       /* release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasAllLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*                           CAS_ALREADY_LOCK
*
**  Description:
*                Lock All Cashier. 
*==========================================================================
*/
SHORT   CasAllLock(VOID)
{
    SHORT   sErrorStatus;

	sErrorStatus = SerCasAllLockForceOutCheck();
	{
		char xBuff[128];
		sprintf (xBuff, "==NOTE: CasAllLock(): SerCasAllLockForceOutCheck() %d", sErrorStatus);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	PifRequestSem(husCashierSem);               /* reqest semaphore */

    if (Cas_OpenFile() < 0) {  /* pif_error_file_exist */
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    if (Cas_LockCheck() != CAS_UNLOCK) {        /* check all lock memory. */
        sErrorStatus = CAS_ALREADY_LOCK;
    } else {
        if (Cas_SignInCheck() != 0x00) {        /* check during sign in */
            sErrorStatus = CAS_DURING_SIGNIN;
        } else {
            uchCasAllLockMem = 0xff;
            sErrorStatus = CAS_PERFORM;
        }
    }
    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    VOID   CasAllUnLock(VOID)
*
*       Input:    Nothing
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*                Unlock All Cahiser 
*==========================================================================
*/
VOID   CasAllUnLock(VOID)
{
    PifRequestSem(husCashierSem);       /* reqest semaphore */
    uchCasAllLockMem = 0x00;            /* clear all cashier lock mamory */
    PifReleaseSem(husCashierSem);       /* release semaphore */
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasIndTermLock(USHORT usTerminalNo)
*
*       Input:    USHORT    usTerminalNo
*      Output:
*       InOut:
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*
**  Description:
*                Lock inputted terminal for terminal report R3.1
*==========================================================================
*/
SHORT   CasIndTermLock(USHORT usTerminalNo)
{

    PifRequestSem(husCashierSem);   /* request samaphore */

    /* check cashier sign-in table */
    if ( ausCasTerminalTbl[usTerminalNo - 1] != 0 ) {
        PifReleaseSem(husCashierSem);
        return(CAS_DURING_SIGNIN);
    }
    if ( ausCasPreIndTerminalTbl[usTerminalNo - 1] != 0 ) {
        PifReleaseSem(husCashierSem);
        return(CAS_DURING_SIGNIN);
    } 
    /* V3.3 */
    if ( ausCasWaiTerminalTbl[usTerminalNo - 1] != 0 ) {
        PifReleaseSem(husCashierSem);
        return(CAS_DURING_SIGNIN);
    }
    if ( ausCasWaiPreIndTerminalTbl[usTerminalNo - 1] != 0 ) {
        PifReleaseSem(husCashierSem);
        return(CAS_DURING_SIGNIN);
    }

    /* check terminal is already locked */
    if (chCasIndTermLockMem) {
        if (chCasIndTermLockMem == (CHAR)usTerminalNo) {
            PifReleaseSem(husCashierSem);
            return(CAS_ALREADY_LOCK);
        } else {
            PifReleaseSem(husCashierSem);
            return(CAS_OTHER_LOCK);
        }
    }

    /* set cashier terminal lock table */
    chCasIndTermLockMem = (CHAR)usTerminalNo;

    PifReleaseSem(husCashierSem);
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasIndTermUnLock(USHORT usTerminalNo)
*
*       Input:    USHORT usTerminalNo
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DIFF_NO
*
**  Description:
*                UnLock inputted terminal for terminal report R3.1
*==========================================================================
*/
SHORT   CasIndTermUnLock(USHORT usTerminalNo)
{
    SHORT   sErrorStatus;       /* error status save area */

    PifRequestSem(husCashierSem);       /* requse semaphore */

    /* check lock */
    if (chCasIndTermLockMem == (CHAR)usTerminalNo) {
        chCasIndTermLockMem = 0x00;
        sErrorStatus = CAS_PERFORM;     /* not other terminal is locked */
    } else {
        sErrorStatus = CAS_DIFF_NO;     /* other terminal is locked */
    }
    PifReleaseSem(husCashierSem);       /* release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasAllTermLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*                           CAS_ALREADY_LOCK
*
**  Description:
*                Lock All Terminal. R3.1
*==========================================================================
*/
SHORT   CasAllTermLock(VOID)
{
    SHORT   sErrorStatus;

    PifRequestSem(husCashierSem);               /* reqest semaphore */

    if (Cas_SignInCheck() != 0x00) {        /* check during sign in */
        sErrorStatus = CAS_DURING_SIGNIN;
    } else {
        if (chCasIndTermLockMem) {
            if (chCasIndTermLockMem == (CHAR)0xFF) {
                sErrorStatus = CAS_ALREADY_LOCK;
            } else {
                sErrorStatus = CAS_OTHER_LOCK;  /* individual lock */
            }
        } else {
            chCasIndTermLockMem = (CHAR)0xff;     /* set all terminal lock */
            sErrorStatus = CAS_PERFORM;
        }
    }

    PifReleaseSem(husCashierSem);
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    VOID   CasAllTermUnLock(VOID)
*
*       Input:    Nothing
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*                Unlock All Terminal R3.1
*==========================================================================
*/
VOID   CasAllTermUnLock(VOID)
{
    PifRequestSem(husCashierSem);       /* reqest semaphore */
    chCasIndTermLockMem = 0x00;     /* clear all terminal lock memory */
    PifReleaseSem(husCashierSem);       /* release semaphore */
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasChkSignIn(USHORT *ausRcvBuffer[CAS_TERMINAL_OF_CLUSTER])
*
*       Input:    Nothing
*      Output:    ULONG    aulRcvBuffer
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_DURING_SIGNIN
*                
**  Description:
*                Check if sign-in Cashier exist.
*==========================================================================
*/
SHORT   CasChkSignIn(ULONG *aulRcvBuffer)
{
    SHORT   sErrorStatus;               /* return status save area */
    USHORT  i,j;

	NHPOS_ASSERT_ARRAYSIZE(ausNwCasPreTerminalTbl, CAS_NUMBER_OF_MAX_CASHIER);

    PifRequestSem(husCashierSem);       /* reqest semaphore */

    /* check sign in */
    if (Cas_OpenFile() < 0) {  /* pif_error_file_exist */
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    if (Cas_SignInCheck() != 0x00) {
    	/* 06/02/01, allow sign-in during pre-signout */
    	/* 06/21/01, fixed not reported all operators */
    	memcpy(aulRcvBuffer, ausNwCasPreTerminalTbl, sizeof(ausNwCasPreTerminalTbl[0]) * STD_NUM_OF_TERMINALS * 2);
    	for (i = 0, j = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
			if (ausCasTerminalTbl[i]) {
				do {
					if (aulRcvBuffer[j] == 0) {
						aulRcvBuffer[j] = ausCasTerminalTbl[i];
						j++;
						break;
					}
					j++;
				} while (j < CAS_NUMBER_OF_MAX_CASHIER);
        	}
        }
    	for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
			if (ausCasWaiTerminalTbl[i]) {
				do {
					if (aulRcvBuffer[j] == 0) {
						aulRcvBuffer[j] = ausCasWaiTerminalTbl[i];
						j++;
						break;
					}
					j++;
				} while (j < CAS_NUMBER_OF_MAX_CASHIER);
        	}
        }
        sErrorStatus = CAS_DURING_SIGNIN;
    } else {
        sErrorStatus = CAS_PERFORM;
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sErrorStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CasSendFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_COM_ERROR
*                
**  Description:
*               Request Back Up.
*==========================================================================
*/
SHORT   CasSendFile(VOID)
{
    CAS_BACKUP      Cas_BackUp, *pCasBackUpRcv;
    CAS_BAKDATA     *pCasBackUpData;
    USHORT          usRcvLen;
    UCHAR           auchRcvBuf[OP_BACKUP_WORK_SIZE];
    SHORT           sStatus;

	NHPOS_ASSERT(sizeof(auchRcvBuf) >= sizeof(CAS_BACKUP) + sizeof(CAS_BAKDATA));

    PifRequestSem(husCashierSem);       /* Request Semaphore */

    if (Cas_OpenFile() < 0) {  /* pif_error_file_exist */
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_PERFORM);
    }

    uchCasAllLockMem = 0x00;            /* clear all cashier lock mamory */
    ulCasIndLockMem = 0x00;             /* clear ind. cashier */
  
    memset(&ausCasTerminalTbl, 0, sizeof(ausCasTerminalTbl));
    memset(&ausCasPreIndTerminalTbl, 0, sizeof(ausCasPreIndTerminalTbl));
    memset(&ausCasWaiTerminalTbl, 0, sizeof(ausCasWaiTerminalTbl));  /* V3.3 */
    memset(&ausCasWaiPreIndTerminalTbl, 0, sizeof(ausCasWaiPreIndTerminalTbl));
    memset(&ausNwCasPreTerminalTbl, 0, sizeof(ausNwCasPreTerminalTbl));
    memset(&Cas_BackUp, 0 , sizeof(CAS_BACKUP));

    for (;;) {
        usRcvLen = OP_BACKUP_WORK_SIZE;

        sStatus = SstReqBackUp(CLI_FCBAKCASHIER, (UCHAR *)&Cas_BackUp, sizeof(CAS_BACKUP), auchRcvBuf, &usRcvLen);
        if (sStatus < 0) {
            break;
        }

        pCasBackUpRcv  = (CAS_BACKUP  *)auchRcvBuf;
        pCasBackUpData = (CAS_BAKDATA *)(auchRcvBuf + (sizeof(CAS_BACKUP)));

        if ( pCasBackUpRcv->usSeqNO != Cas_BackUp.usSeqNO ) {
            sStatus = CAS_COMERROR;
            break;
        }

        if( usRcvLen - sizeof(CAS_BACKUP) - sizeof(CAS_BAKDATA) != pCasBackUpData->usDataLen ) {
            sStatus = CAS_COMERROR;
            break;
        }

		NHPOS_ASSERT(sizeof(auchRcvBuf) >= pCasBackUpData->usDataLen + sizeof(CAS_BACKUP) + sizeof(CAS_BAKDATA));

        Cas_WriteFile(Cas_BackUp.offulFilePosition, pCasBackUpData->auchData, pCasBackUpData->usDataLen);

        if (pCasBackUpRcv->uchStatus == CAS_END) {
            sStatus = CAS_PERFORM;
            break;
        }

        Cas_BackUp.offulFilePosition += pCasBackUpData->usDataLen;
        Cas_BackUp.usSeqNO ++;
    }
    Cas_CloseFileReleaseSem();
    return(sStatus);
}
/*
*==========================================================================
*
**    Synopsis:    SHORT   CasRcvFile(UCHAR  *puchRcvData,
*                                     USHORT usRcvLen,
*                                     UCHAR  *puchSndData,
*                                     USHORT *pusSndLen)
*
*       Input:    UCHAR  *puchRcvData   * Pointer to Request Data. *
*                 USHORT usRcvLen       * Width of puchRcvData *
*
*       Output:   UCHAR  *puchSndData   * Storage location for Backup data. *
*                 USHORT *pusSndLen     * Width of puchSndData *
*
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    Length of guest check data
*          Abnormal End:    
*                
**  Description:
*               Response Back Up.
*==========================================================================
*/
SHORT   CasRcvFile(UCHAR *puchRcvData,
                   USHORT usRcvLen,
                   UCHAR *puchSndData,
                   USHORT *pusSndLen)
{
    CAS_BACKUP  *pCas_BackUpRcv, *pCas_BackUpSnd;
    CAS_BAKDATA *pCas_BackUpData;
    CAS_FILEHED CasFileHed;             /* file header save area */
    USHORT      usWorkLen;
	SHORT       sRetVal;

    PifRequestSem(husCashierSem);       /* request samaphore */

     /* Open File and get the file header */
    if ((sRetVal = Cas_OpenFileGetHeader(&CasFileHed)) < 0) {
        PifReleaseSem(husCashierSem);
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sRetVal));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        return(CAS_FILE_NOT_FOUND);
    }

    if (usRcvLen != sizeof(CAS_BACKUP)) {
        Cas_CloseFileReleaseSem();
        return(CAS_COMERROR);
    }
    
    pCas_BackUpRcv  = (CAS_BACKUP  *)puchRcvData;
    pCas_BackUpSnd  = (CAS_BACKUP  *)puchSndData;
    pCas_BackUpData = (CAS_BAKDATA *)(puchSndData + sizeof(CAS_BACKUP));
    usWorkLen = OP_BACKUP_WORK_SIZE - sizeof(CAS_BACKUP) - sizeof(CAS_BAKDATA) /*2*/;

    if ( CasFileHed.ulCasFileSize <= pCas_BackUpRcv->offulFilePosition ) {
        Cas_CloseFileReleaseSem();
        return(CAS_COMERROR);
    }

    if ((CasFileHed.ulCasFileSize - pCas_BackUpRcv->offulFilePosition) > 
        usWorkLen ) {
        pCas_BackUpSnd->uchStatus = CAS_CONT;
    } else {
        usWorkLen = (USHORT)(CasFileHed.ulCasFileSize - pCas_BackUpRcv->offulFilePosition);
        pCas_BackUpSnd->uchStatus = CAS_END;
    }

    Cas_ReadFile(pCas_BackUpRcv->offulFilePosition, pCas_BackUpData->auchData, usWorkLen);

    pCas_BackUpData->usDataLen = usWorkLen;
    pCas_BackUpSnd->usSeqNO    = pCas_BackUpRcv->usSeqNO;
    pCas_BackUpSnd->offulFilePosition = pCas_BackUpRcv->offulFilePosition;
    *pusSndLen = usWorkLen + sizeof(CAS_BACKUP) + sizeof(CAS_BAKDATA)/*2*/;

    Cas_CloseFileReleaseSem();
    return(CAS_PERFORM);
}
/*
***************************************************************************
    MODULE INTERNAL PROCESS
***************************************************************************
*/

/*    Saved memory size   8-25-93     */
/*    Saved memory size   8-25-93     */

/*                 
*==========================================================================
**    Synopsis:   SHORT  Cas_OpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMCAS" file.
*==========================================================================
*/
static SHORT  Cas_OpenFile(VOID)
{
    hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE);
	if (hsCashierFileHandle < 0) {
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_OPEN_FILE);
        PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(hsCashierFileHandle));
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
	}
    return (hsCashierFileHandle);
}

/*    Saved memory size   8-25-93      */
/*    Saved memory size   8-25-93      */

/*
*==========================================================================
**    Synopsis:   VOID  Cas_GetHeader(CAS_FILEHED *pCasFileHed)
*
*       Input:    Nothing
*      Output:    CAS_FILEHED *pCasFileHed
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*                Get Header in File.
*==========================================================================
*/
static VOID   Cas_GetHeader(CAS_FILEHED *pCasFileHed)
{
    Cas_ReadFile(CAS_FILE_HED_POSITION, pCasFileHed, sizeof(CAS_FILEHED));
}

/*
*==========================================================================
**    Synopsis:   VOID  Cas_PutHeader(CAS_FILEHED *pCasFileHed)
*
*       Input:    CAS_FILEHED *pCasFileHed
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*                Put Header in File
*==========================================================================
*/
static VOID   Cas_PutHeader(CAS_FILEHED *pCasFileHed)
{
    Cas_WriteFile(CAS_FILE_HED_POSITION, pCasFileHed, sizeof(CAS_FILEHED));
}

/*
*==========================================================================
**  Synopsis:    SHORT   Cas_Index(CAS_FILEHED *pCasFileHed, USHORT usIndexMode,
*                                  USHORT usCashierNo, UCHAR *uchParaBlockNo)
*
*       Input:    CAS_FILEHED *pCasFileHed     file header
*                 ULONG ulCashierNo          cashier no.
*                 UCHAR *uchParaBlockNo       paramater block no.
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_RESIDENT
*                           CAS_NOT_IN_FILE
*                           CAS_CASHIER_FULL
*   
**  Description:
*                Check if Cashier exsisted V3.3
*==========================================================================
*/
static SHORT   Cas_Index(CAS_FILEHED *pCasFileHed, USHORT usIndexMode, ULONG ulCashierNo, USHORT *usParaBlockNo)
{
    CAS_INDEXENTRY CasIndexEntry;                              /* index table entry */
    SHORT    sStatus;                                          /* return status save area */
    USHORT  offusSearchPoint;

    /* read 1 entry from index table */
    CasIndexEntry.ulCashierNo = ulCashierNo;

    sStatus = Cas_SearchFile((USHORT)pCasFileHed->offusOffsetOfIndexTbl,
                             (USHORT)(pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)),
                             &offusSearchPoint, 
                             &CasIndexEntry.ulCashierNo,
                             sizeof(CAS_INDEXENTRY),
                             Cas_CompIndex );

    if ((sStatus == CAS_NOT_IN_FILE) && (usIndexMode == CAS_SEARCH_ONLY)) {
        return (CAS_NOT_IN_FILE);
    }

    if (sStatus == CAS_RESIDENT) {   /* CASHIER FOUND */
        Cas_ReadFile( offusSearchPoint, &CasIndexEntry, sizeof( CAS_INDEXENTRY ));
        *usParaBlockNo   = CasIndexEntry.usParaBlock;  /* make output condition */
        return (CAS_RESIDENT);
    }

    /* BELOW IS NEW CASHIER  ASSIGN */

    /* Check Paramater Table full and get paramater No. */
    if (Cas_EmpBlkGet(pCasFileHed, &CasIndexEntry.usParaBlock) == CAS_CASHIER_FULL) {    /* get empty block */
        return(CAS_CASHIER_FULL);                       /* return if paramater empty area */ 
    }

    /* make space 1 record in index table */
    Cas_InsertTable((USHORT)pCasFileHed->offusOffsetOfIndexTbl,
                    (USHORT)(pCasFileHed->usNumberOfResidentCashier *  sizeof(CAS_INDEXENTRY)),
                    (USHORT)(offusSearchPoint - (USHORT)pCasFileHed->offusOffsetOfIndexTbl),
                    &CasIndexEntry,
                    sizeof(CAS_INDEXENTRY));

    pCasFileHed->usNumberOfResidentCashier++;

    *usParaBlockNo   = CasIndexEntry.usParaBlock;  /* make output condition */

    Cas_PutHeader(pCasFileHed); /* update file header */
    return(CAS_REGISTED);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_IndexDel( CAS_FILEHED *pCasFileHed, USHORT usCashierNo )
*
*       Input:    CAS_FILEHED *pCasFileHed
*                 USHORT usCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:    CAS_PERFORM
*          Abnormal End:    CAS_NOT_IN_FILE
*
**  Description:
*                Delete inputted Cashier from index table 
*==========================================================================
*/
static SHORT   Cas_IndexDel( CAS_FILEHED *pCasFileHed, ULONG ulCashierNo )
{
    CAS_INDEXENTRY CasIndexEntry;       /* index table entry poiter */
    USHORT  offusSearchPoint;

    /* get index table to work buffer */
    if ( pCasFileHed->usNumberOfResidentCashier == 0 ) {
        return(CAS_NOT_IN_FILE);
    }

    CasIndexEntry.ulCashierNo = ulCashierNo;

    if ( Cas_SearchFile((USHORT)pCasFileHed->offusOffsetOfIndexTbl,
                        (USHORT)(pCasFileHed->usNumberOfResidentCashier * sizeof( CAS_INDEXENTRY )),
                        &offusSearchPoint, 
                        &CasIndexEntry.ulCashierNo,
                        sizeof(CAS_INDEXENTRY),
                        Cas_CompIndex ) == CAS_NOT_IN_FILE) {
        return (CAS_NOT_IN_FILE);
    }

    /* Get Paramater Block No */
    Cas_ReadFile(offusSearchPoint, &CasIndexEntry, sizeof( CAS_INDEXENTRY ));

    /* Delete space 1 record in index table */
    Cas_DeleteTable((USHORT)pCasFileHed->offusOffsetOfIndexTbl,
                    (USHORT)(pCasFileHed->usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)),
                    (USHORT)(offusSearchPoint - (USHORT)pCasFileHed->offusOffsetOfIndexTbl), 
                    sizeof(CAS_INDEXENTRY));

    /* Empty para empty table */
    Cas_BlkEmp(pCasFileHed, CasIndexEntry.usParaBlock);  /* let empty block is empty */

    /* Resident Cashier -1 */
    if (pCasFileHed->usNumberOfResidentCashier > 0) {
		pCasFileHed->usNumberOfResidentCashier--;
	}

    /* write file header */
    Cas_PutHeader(pCasFileHed);
    return(CAS_PERFORM);

}

/*
*==========================================================================
**    Synopsis:    VOID    Cas_ParaRead(CAS_FILEHED *pCasFileHed, UCHAR uchParablockNo, CAS_PARAENTRY *pArg)
*
*       Input:    CAS_FILEHED *pCasFileHed
*                 UCHAR uchParablockNo
*                 CAS_PARAENTRY *pArg
*      Output:    CAS_PARAENTRY *pArg
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*                Read Cashier Paramater.
*==========================================================================
*/
static VOID    Cas_ParaRead(CAS_FILEHED CONST *pCasFileHed, USHORT usParablockNo, CAS_PARAENTRY *pArg)
{
    ULONG   ulActualPosition;  /* for PifSeekFile */
	ULONG	ulActualBytesRead; //For PifReadFile 11-7-3

    if (PifSeekFile(hsCashierFileHandle, (ULONG)pCasFileHed->offusOffsetOfParaTbl + (sizeof(CAS_PARAENTRY) * (usParablockNo - 1)), &ulActualPosition) < 0 ) {
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
	//RPH 11-7-3 Changes to PifReadFile 
	PifReadFile(hsCashierFileHandle, pArg, sizeof(CAS_PARAENTRY), &ulActualBytesRead);
    if (ulActualBytesRead != sizeof(CAS_PARAENTRY)) {
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
    }
}

/*
*==========================================================================
**    Synopsis:    VOID    Cas_ParaWrite(CAS_FILEHED *pCasFileHed, UCHAR uchParaBlockNo, CAS_PARAENTRY *pArg)
*
*       Input:    CAS_FILEHED *pCasFileHed
*                 UCHAR uchParaBlockNo
*                 CAS_PARAENTRY *pArg
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*                Nothing
**  Description:
*                Write Cashier Paramater 
*==========================================================================
*/
static VOID    Cas_ParaWrite(CAS_FILEHED CONST *pCasFileHed, USHORT usParaBlockNo, CAS_PARAENTRY *pArg)
{
    ULONG   ulActualPosition;   /* for PifOpSeekFile */

    if (PifSeekFile(hsCashierFileHandle, (ULONG)pCasFileHed->offusOffsetOfParaTbl + (sizeof(CAS_PARAENTRY) * (usParaBlockNo - 1)), &ulActualPosition) < 0)  {
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
    PifWriteFile(hsCashierFileHandle, pArg, sizeof(CAS_PARAENTRY));
}

/*
*==========================================================================
**    Synopsis:   SHORT Cas_EmpBlkGet(CAS_FILEHED *pCasFileHed, UCHAR *uchParaBlcokNo)
*
*       Input:    CAS_FILEHED *pCasFileHed
*      Output:    UCHAR *uchParaBlcokNo
*       InOut:    Nothing
*
**  Return    :   CAS_PERFORM
*                 CAS_CASHIER_FULL
*                
**  Description:
*                Get empty Block No.
*==========================================================================
*/
static SHORT   Cas_EmpBlkGet(CAS_FILEHED CONST *pCasFileHed, USHORT *usParaBlockNo)
{
    UCHAR    uchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE];  /* paramater empty table */
    SHORT    sI, sJ, sAnswer;

    /* get paramater empty table */
    Cas_ReadFile((ULONG)pCasFileHed->offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);

    /* Search empty block from paramater empty table */
    sAnswer = 0;
    for (sI = 0; sI < CAS_PARA_EMPTY_TABLE_SIZE; sI++ ) {
        if (uchParaEmpBlk[sI] != 0xff) {    
            for (sJ = 0; sJ < 8; sJ++) {          
                if (((uchParaEmpBlk[sI] >> sJ) & 0x01) == 0x00) {
                    uchParaEmpBlk[sI] |= (UCHAR)(0x01 << sJ);
                    sAnswer = 1;
                    break;
                }
            }
        }
        if (sAnswer == 1) {
            break;
        }
    }

    /* Check and Make Paramater Block No.   */
    if (sAnswer == 1) {
        *usParaBlockNo = (USHORT)((sI * 8) + (sJ + 1));   /* Make Paramater block No */
        if (*usParaBlockNo > pCasFileHed->usNumberOfMaxCashier) {
            return(CAS_CASHIER_FULL);
        }
        Cas_WriteFile((ULONG)pCasFileHed->offusOffsetOfParaEmpTbl, uchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
        return(CAS_PERFORM);
    }
    return(CAS_CASHIER_FULL);
}

/*
*==========================================================================
**    Synopsis:   VOID  Cas_BlkEmp(CAS_FILEHED *pCasFileHed, UCHAR uchParaBlockNo)
*
*       Input:    CAS_FILEHED *pCasFileHed
*                 UCHAR uchParaBlockNo
*                 
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*            Let Empty Empty Block
*==========================================================================
*/
static VOID    Cas_BlkEmp(CAS_FILEHED CONST *pCasFileHed, USHORT usParaBlockNo)
{
    UCHAR    auchParaEmpBlk[CAS_PARA_EMPTY_TABLE_SIZE];
    SHORT    sI, sJ;

    Cas_ReadFile((ULONG)pCasFileHed->offusOffsetOfParaEmpTbl, auchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
    sI = (SHORT)(usParaBlockNo - 1) / 8;
    sJ = (SHORT)(usParaBlockNo - 1) % 8;

    auchParaEmpBlk[sI] &= ~( 0x01 << sJ );
                                                          
    Cas_WriteFile((ULONG)pCasFileHed->offusOffsetOfParaEmpTbl, auchParaEmpBlk, CAS_PARA_EMPTY_TABLE_SIZE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_IndLockCheck(USHORT usCashierNo)
*
*       Input:    ULONG ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    CAS_LOCK
*                  CAS_ALLLOCK
*                  CAS_UNLOCK
*
**  Description:
*               Check during lock
*==========================================================================
*/
static SHORT   Cas_IndLockCheck(ULONG ulCashierNo)
{

    if (ulCasIndLockMem == ulCashierNo) {
        return(CAS_LOCK);
    } else  if (!uchCasAllLockMem) {    /* (uchCasAllLockMem = 0) */
        return(CAS_UNLOCK);
    }
    return(CAS_ALLLOCK);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_LockCheck(VOID)
*
*       Input:    Nothing
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    CAS_UNLOCK
*                  CAS_LOCK
*
**  Description:
*                Check if Locked Cashier existed.
*==========================================================================
*/
static SHORT   Cas_LockCheck(VOID)
{
    if (!ulCasIndLockMem) {         /* (usCasIndLockMem == 0) */
        if (!uchCasAllLockMem) {    /* (uchCasAllLockMem == 0) */
            return(CAS_UNLOCK);
        }
    }    
    return(CAS_LOCK);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_IndTermLockCheck(USHORT usTerminalNo)
*
*       Input:    USHORT usTerminalNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    CAS_LOCK
*                  CAS_UNLOCK
*
**  Description:
*               Check during lock of destination terminal R3.1
*==========================================================================
*/
static SHORT   Cas_IndTermLockCheck(USHORT usTerminalNo)
{

    if (chCasIndTermLockMem == (CHAR)usTerminalNo) {
        return(CAS_LOCK);
    } else if (chCasIndTermLockMem == (CHAR)0xFF) {   /* all terminal lock */
        return(CAS_LOCK);
    }
    return(CAS_UNLOCK);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_SignInCheck(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    Nomber of SIGN-INed Terminal.
*
**  Description:
*                Checi if Sign-In Cashier existed
*==========================================================================
*/
static SHORT   Cas_SignInCheck(VOID)
{
    SHORT    sI, sJ;
    
#if 1
	// this check removed but kept for future usage if problems develop with this functionality.
	{
		NHPOS_NONASSERT_TEXT("==NOTE: Cas_SignInCheck(): ");
	}
#endif
    sJ = 0x00;
    for (sI = 0; sI < CAS_TERMINAL_NO_EXTENDED; sI++) {
        if (ausCasTerminalTbl[sI] != 0x00) {
			if(ausCasDBTerminalTbl[sI] == 0x00)
			{
				char  xBuff[128];

				sJ++;
				sprintf (xBuff, "    -1- ausCasTerminalTbl[%d] == %d", sI, ausCasTerminalTbl[sI]);
				NHPOS_ASSERT_TEXT(0, xBuff);
			}
        } else
        if (ausCasWaiTerminalTbl[sI] != 0x00) { /* V3.3 */
			if(ausCasDBTerminalTbl[sI] == 0x00)
			{
				char  xBuff[128];

				sJ++;
				sprintf (xBuff, "    -1- ausCasWaiTerminalTbl[%d] == %d", sI, ausCasWaiTerminalTbl[sI]);
				NHPOS_ASSERT_TEXT(0, xBuff);
			}
        }
    }
    /* 06/02/01, allow sign-in during pre-signout */
    for (sI = 0; sI < CAS_NUMBER_OF_MAX_CASHIER; sI++) {
		if (ausNwCasPreTerminalTbl[sI] != 0x00) {
			char  xBuff[128];

			sJ++;
			sprintf (xBuff, "    -3- ausNwCasPreTerminalTbl[%d] == %d", sI, ausNwCasPreTerminalTbl[sI]);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
    }
    return(sJ);
}

SHORT   Cas_SignInCheckAgainstForcedOut(VOID)
{
	ULONG   ulTerminalStatus[2];
	LONG    lTerminalTableEntry;
    SHORT    sI, sJ;

	SerOpGetOperatorMessageStatusTable (1, ulTerminalStatus);

#if 1
	// this check removed but kept for future usage if problems develop with this functionality.
	{
		char xBuff[128];
		sprintf (xBuff, "==NOTE: Cas_SignInCheckAgainstForcedOut(): ulTerminalStatus 0x%x, 0x%x", ulTerminalStatus[0], ulTerminalStatus[1]);
		NHPOS_NONASSERT_TEXT(xBuff);
	}
#endif

    sJ = 0x00;
    for (sI = 0; sI < CAS_TERMINAL_NO_EXTENDED; sI++) {
        if (ausCasTerminalTbl[sI] != 0x00) {
			// if the terminal is not in Delayed Balance then having a Cashier Signed In
			// causes the check to fail.
			if(ausCasDBTerminalTbl[sI] == 0x00)
			{
				sJ++;
				{
					char  xBuff[128];
					sprintf (xBuff, "    -1- ausCasTerminalTbl[%d] == %d", sI, ausCasTerminalTbl[sI]);
					NHPOS_ASSERT_TEXT(0, xBuff);
				}
				if ((ulTerminalStatus[0] & 0x0003) != 0) {
					// either a Forced Sign-out Operator Message was received or
					// a multiple (Forced Sign-out, Auto Sign-out, Block Sign-in) was received.
					// If we did not get a message from the terminal then just clear the Sign-in indicator.
					// If we did but we are doing a multiple then just clear the Sign-in indicator anyway.
					NHPOS_ASSERT_TEXT(0, "** Signed-in, check Operator Message");
					if ((ulTerminalStatus[0] & 0x0003) == 0x0003) {
						// Multiple message so just clear it.
						NHPOS_ASSERT_TEXT(0, "   --  Multi message, clear it");
						sJ--;
						ausCasTerminalTbl[sI] = 0x00;
					} else {
						lTerminalTableEntry = SerOpGetOperatorMessageStatusTable (sI+1, 0);
						if (lTerminalTableEntry >= 0) {
							char  xBuff[128];
							sprintf(xBuff, "   --  we have table entry 0x%x", lTerminalTableEntry);
							NHPOS_ASSERT_TEXT(0, xBuff);
							if ((lTerminalTableEntry & 0x0001) == 0) {
								// this terminal did not request an Operator Message.
								// however our table shows someone is Signed in.
								NHPOS_ASSERT_TEXT(0, "   --  no from terminal, clear it");
								sJ--;
								ausCasTerminalTbl[sI] = 0x00;
							}
						}
					}
				}
			}
        } else if (ausCasWaiTerminalTbl[sI] != 0x00) { /* V3.3 */
			if(ausCasDBTerminalTbl[sI] == 0x00)
			{
				char  xBuff[128];

				sJ++;
				sprintf (xBuff, "    -1- ausCasWaiTerminalTbl[%d] == %d", sI, ausCasWaiTerminalTbl[sI]);
				NHPOS_ASSERT_TEXT(0, xBuff);
			}
        }
    }
    /* 06/02/01, allow sign-in during pre-signout */
    for (sI = 0; sI < CAS_NUMBER_OF_MAX_CASHIER; sI++) {
		if (ausNwCasPreTerminalTbl[sI] != 0x00) {
			char  xBuff[128];

			sJ++;
			sprintf (xBuff, "    -3- ausNwCasPreTerminalTbl[%d] == %d", sI, ausNwCasPreTerminalTbl[sI]);
			NHPOS_ASSERT_TEXT(0, xBuff);
		}
    }
    return(sJ);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_IndSignInCheck(USHORT usCashierNo)
*
*       Input:    ULONG ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    CAS_DURING_SIGNIN
*                  CAS_NOT_SIGNIN
*
**  Description:
*==========================================================================
*/
static SHORT   Cas_IndSignInCheck(ULONG ulCashierNo)
{
    USHORT    usI;

    for (usI = 0; usI < CAS_TERMINAL_NO_EXTENDED; usI++) {
        if (ausCasTerminalTbl[usI] == ulCashierNo) {
            return(CAS_DURING_SIGNIN);
        }

        if (ausCasWaiTerminalTbl[usI] == ulCashierNo) {
            return(CAS_DURING_SIGNIN);
        }
    }
    /* 06/02/01, allow sign-in during pre-signout */
    for (usI = 0; usI < CAS_NUMBER_OF_MAX_CASHIER; usI++) {
		if (ausNwCasPreTerminalTbl[usI] == ulCashierNo) {
            return(CAS_DURING_SIGNIN);
		}
    }
    return(CAS_NOT_SIGNIN);    
}

/*
*==========================================================================
**    Synopsis:    SHORT   Cas_IndPreSignInCheck(USHORT usCashierNo)
*
*       Input:    ULONG ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    CAS_DURING_SIGNIN
*                  CAS_NOT_SIGNIN
*
**  Description: R3.3
*==========================================================================
*/
static SHORT   Cas_IndPreSignInCheck(ULONG ulCashierNo)
{
    USHORT    usI;

    /* 06/02/01, allow sign-in during pre-signout */
    for (usI=0; usI<CAS_NUMBER_OF_MAX_CASHIER; usI++) {
		if (ausNwCasPreTerminalTbl[usI] == ulCashierNo) {
            return(CAS_DURING_SIGNIN);
		}
    }
    return(CAS_NOT_SIGNIN);    
}
/*
*==========================================================================
**    Synopsis:    VOID    Cas_WriteFile(ULONG offulFileSeek, VOID *pTableHeadAddress, USHORT usSizeofWrite)
*
*       Input:   ULONG offulFileSeek   * offset from file head *
*                VOID  *pTableHeadAddress * table head address for writing into file *
*                USHORT usSizeofWrite  * size of for writing into file *
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing (error is system error(progrum is abort)
*
**  Description:
*               Write data to Cashier file. 
*==========================================================================
*/
static VOID    Cas_WriteFile(ULONG offulFileSeek, VOID *pTableHeadAddress, USHORT usSizeofWrite)
{
    ULONG   ulActualPosition;   /* DUMMY FOR PifSeekFile */

    if (!usSizeofWrite) {
        return;
    }

    if ((PifSeekFile(hsCashierFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
        
    PifWriteFile(hsCashierFileHandle, pTableHeadAddress, usSizeofWrite);
}
/*
*==========================================================================
**    Synopsis:    SHORT    Cas_ReadFile(ULONG offulFileSeek, VOID *pTableHeadAddress, USHORT usSizeofRead)
*
*       Input:    ULONG offulFileSeek   * offset from file head *
*                 USHORT usSizeofRead   * size of for reading buffer *
*
*      Output:    VOID  *pTableHeadAddress * table head address for reading from file *
*       InOut:    Nothing
*
**  Return   :    CAS_PERFORM
*                 CAS_NO_READ_SIZE           read size is 0 
*                 (file error is system halt)
*                    
*
**  Description:
*               Read data in the cashier file
*==========================================================================
*/
static SHORT    Cas_ReadFile(ULONG offulFileSeek, VOID *pTableHeadAddress, ULONG ulSizeofRead)
{
    ULONG ulActualPosition;
	ULONG ulActualBytesRead;

    if ((PifSeekFile(hsCashierFileHandle, offulFileSeek, &ulActualPosition)) < 0 ) {
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_SEEK);
    }
    if (ulSizeofRead == 0x00){
        return (CAS_NO_READ_SIZE);
    }
    
	//RPH 11-7-3 Changes to PifReadFile
	PifReadFile(hsCashierFileHandle, pTableHeadAddress, ulSizeofRead, &ulActualBytesRead);
	if ( ulActualBytesRead != ulSizeofRead) {
        PifLog(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
        PifLog(MODULE_LINE_NO(MODULE_CASHIER), (USHORT)__LINE__);
        PifAbort(MODULE_CASHIER, FAULT_ERROR_FILE_READ);
    }

    return(CAS_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    VOID    Cas_CloseFileReleaseSem(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    Nothing (error is system error(progrum is abort))
*                
*                    
*
**  Description:
    
*               Close the Cashier file and release the cashier semaphore.
*==========================================================================
*/
static VOID    Cas_CloseFileReleaseSem(VOID)
{
    PifCloseFile(hsCashierFileHandle);
	hsCashierFileHandle = -1;           // set file handle to invalid value after PifCloseFile()
    PifReleaseSem(husCashierSem);
}


/*                 
*==========================================================================
**    Synopsis:    SHORT   CasPreSignOut(CASIF *pCasif)
*
*       Input:    pCasif->usCashierNo
*                 pCasif->usUniqueAddress
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    
*            Normal End:    CAS_PERFORM
*
**  Description:
*                Set pre close flag.
*==========================================================================
*/
SHORT   CasPreSignOut(CASIF CONST *pCasif)
{
	USHORT i;

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    /* V3.3 */
    if (pCasif->ulCashierNo == ausCasWaiTerminalTbl[pCasif->usUniqueAddress - 1]) {
	    /* 06/02/01, allow sign-in during pre-signout */
    	for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
			if (ausNwCasPreTerminalTbl[i] == pCasif->ulCashierNo) {
				ausNwCasPreTerminalTbl[i] = 0;
			}
    	}
    	for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
			if (ausNwCasPreTerminalTbl[i] == 0) {
				ausNwCasPreTerminalTbl[i] = pCasif->ulCashierNo;
				break;
			}
		}

        /* set cashier no on pre-terminal table */
        ausCasWaiPreIndTerminalTbl[pCasif->usUniqueAddress - 1] = pCasif->ulCashierNo;
        ausCasWaiTerminalTbl[pCasif->usUniqueAddress - 1] = 0x0000; /* Clear Termianl Table */
    } else {
	    /* 06/02/01, allow sign-in during pre-signout */
    	for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
			if (ausNwCasPreTerminalTbl[i] == pCasif->ulCashierNo) {
				ausNwCasPreTerminalTbl[i] = 0;
			}
    	}
    	for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
			if (ausNwCasPreTerminalTbl[i] == 0) {
				ausNwCasPreTerminalTbl[i] = pCasif->ulCashierNo;
				break;
			}
		}

        /* set cashier no on pre-terminal table */
        ausCasPreIndTerminalTbl[pCasif->usUniqueAddress - 1] = pCasif->ulCashierNo;
        ausCasTerminalTbl[pCasif->usUniqueAddress - 1] = 0x0000; /* Clear Termianl Table */
    }

    PifReleaseSem(husCashierSem);   /* Release Semaphore */
    return(CAS_PERFORM);
}


/*
*==========================================================================
**    Synopsis:   SHORT   Cas_SearchFile(USHORT offusTableHeadAddress, 
*                                        USHORT usTableSize, 
*                                        USHORT *poffusSearchPoint,
*                                        VOID   *pSearchData, 
*                                        USHORT usSearchDataSize, 
*                                        SHORT (*Comp)(VOID *pKey, 
*                                                      VOID *pusCompKey))
*
*       Input:    offulTableHeadAddress
*                 ulTableSize
*                 *pSearchPoint
*                 *pSearchData
*                 usSearchDataSize
*                 (*Comp)(VOID *A, VOID *B)   A - B
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   CAS_RESIDENT
*                 CAS_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:  Search data on the Table in a File. V3.3
*==========================================================================
*/
static SHORT   Cas_SearchFile(USHORT offusTableHeadAddress, 
                       USHORT usTableSize, 
                       USHORT *poffusSearchPoint,
                       VOID   *pSearchData, 
                       USHORT usSearchDataSize, 
                       SHORT (*Comp)(VOID *pKey, VOID *pusCompKey))
{
    SHORT sStatus;
    UCHAR auchKeyData[32];
    UCHAR auchWorkBuffer[CAS_MAX_WORK_BUFFER];
    SHORT sCompStatus;
    USHORT offusTableTailAddress;
    USHORT offusTableSize;
    USHORT *pusHitPoint;
    USHORT usNumberOfEntry;             /* number of entry */

	if (usTableSize == 0) {
		*poffusSearchPoint = offusTableHeadAddress;
		return CAS_NOT_IN_FILE;
	}

    sStatus = 0;

    /* make Table Tail address */
    offusTableTailAddress = offusTableHeadAddress + usTableSize;
    offusTableSize = usTableSize;

    /* Search key in the file */
    while ( offusTableSize > CAS_MAX_WORK_BUFFER ) {    
        usNumberOfEntry = (offusTableSize / usSearchDataSize) / 2;
        *poffusSearchPoint = (usNumberOfEntry * usSearchDataSize) + offusTableHeadAddress;
        Cas_ReadFile(*poffusSearchPoint, auchKeyData, usSearchDataSize);
        if ((sCompStatus = (*Comp)(pSearchData, auchKeyData)) == 0) {
            return(CAS_RESIDENT);
            break;
        } else if (sCompStatus > 0) {   /* pSearchData > auchKeyData */
            offusTableHeadAddress = *poffusSearchPoint;
            offusTableHeadAddress += usSearchDataSize;
        } else {
            offusTableTailAddress = *poffusSearchPoint;
        }
        offusTableSize = offusTableTailAddress - offusTableHeadAddress;
    }

    Cas_ReadFile((ULONG)offusTableHeadAddress, auchWorkBuffer, offusTableSize);   /* offusTableSize != 0 */

    /* Search key in a table from the file  */
    usNumberOfEntry = offusTableSize / usSearchDataSize;
    sStatus = Rfl_SpBsearch(pSearchData, usSearchDataSize, (USHORT *)auchWorkBuffer, usNumberOfEntry, &pusHitPoint, Comp);

    *poffusSearchPoint = (UCHAR *)pusHitPoint - auchWorkBuffer;
    *poffusSearchPoint += offusTableHeadAddress; /* offset in the file */
    if (sStatus == RFL_HIT) {
        return(CAS_RESIDENT);
    }
    return (CAS_NOT_IN_FILE);
}

/*
*==========================================================================
**    Synopsis:   VOID   Cas_InsertTable(USHORT offusTableHeadAddress, 
*                                         USHORT usTableSize
*                                         USHORT offusInsertPoint, 
*                                         VOID   *pInsertData, 
*                                         USHORT usInsertDataSize)
*
*       Input:   offusTableHeadAddress  Table head address in a file.
*                usTableSize            Table size in a file.
*                offInsertPoint         Table Insert Point (offset from tabel head address) in a file.
*                *pInsertData           Inserted Data
*                usInsertDataSize       Insert Data size
*               
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 
*
**  Description: Insert data to Table in a File. V3.3
*==========================================================================
*/
static VOID   Cas_InsertTable( USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusInsertPoint, 
                        VOID   *pInsertData, 
                        USHORT usInsertDataSize)
{
    UCHAR   auchWorkBuffer[CAS_MAX_WORK_BUFFER];    /* work buffer */
    USHORT  offusTailInFile;    /* Table Tail Address */
    USHORT  usMoveByte;         /* moving byte save area */

    if (usTableSize <= offusInsertPoint) {
        Cas_WriteFile(offusInsertPoint + offusTableHeadAddress, pInsertData, usInsertDataSize );  /* insert data */
        return;
    }

    /* Calculate moving byte */
    usMoveByte = usTableSize - offusInsertPoint;
    offusTailInFile = offusTableHeadAddress + usTableSize;

    /* Move moving byte in a file */
    while ( usMoveByte > CAS_MAX_WORK_BUFFER ) {
        offusTailInFile -= CAS_MAX_WORK_BUFFER;
        Cas_ReadFile( offusTailInFile, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        Cas_WriteFile( offusTailInFile + usInsertDataSize, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        usMoveByte -= CAS_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Cas_ReadFile( offusTableHeadAddress + offusInsertPoint, auchWorkBuffer, usMoveByte);
        Cas_WriteFile((offusTableHeadAddress + offusInsertPoint + usInsertDataSize), auchWorkBuffer, usMoveByte);
    }

    /* insert data */
    Cas_WriteFile( offusTableHeadAddress + offusInsertPoint, pInsertData, usInsertDataSize );
}

/*
*==========================================================================
**    Synopsis:   VOID   Cas_DeleteTable(USHOT  offusTableHeadAddress, 
*                                        USHORT usTableSize
*                                        USHORT offusDeletePoint, 
*                                        USHORT usDeleteDataSize)
*                                        
*
*       Input:    offusTableHeadAddress     Table head address in a file.
*                 usTableSize               Table size in a file.
*                 offusDeletePoint          Point 
*                 usDeleteDataSize          Data size
*                   
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*                 (file error is system halt)
*
**  Description:
*               Delete data from Index Table in a File. V3.3
*==========================================================================
*/
static VOID    Cas_DeleteTable(USHORT offusTableHeadAddress, 
                        USHORT usTableSize,
                        USHORT offusDeletePoint, 
                        USHORT usDeleteDataSize)
{
    UCHAR   auchWorkBuffer[CAS_MAX_WORK_BUFFER];  /* work buffer */
    USHORT  usMoveByte;     /* moving byte save area */
    
    /* Calculate moving byte */
    usMoveByte = usTableSize - offusDeletePoint - usDeleteDataSize;

    /* Move moving byte in a file */
    offusDeletePoint += offusTableHeadAddress;

    while ( usMoveByte > CAS_MAX_WORK_BUFFER ) {
        Cas_ReadFile( offusDeletePoint + usDeleteDataSize, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        Cas_WriteFile( offusDeletePoint, auchWorkBuffer, CAS_MAX_WORK_BUFFER);
        usMoveByte -= CAS_MAX_WORK_BUFFER;
        offusDeletePoint += CAS_MAX_WORK_BUFFER;
    }

    if (usMoveByte != 0) {
        Cas_ReadFile( offusDeletePoint + usDeleteDataSize, auchWorkBuffer, usMoveByte);
        Cas_WriteFile( offusDeletePoint, auchWorkBuffer, usMoveByte);
    }
}


/*
*==========================================================================
**    Synopsis:    SHORT   CasOpenDrawer(CASIF *pCasIf, USHORT usDrawer)
*
*       Input:    CASIF *pCasIf - Cashier I/F data
*                 USHORT usDrawer - Drawer to Open
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   CAS_PERFORM
*                 CAS_SEQERROR
*
**  Description:
*                Unlock All Terminal R2.0
*==========================================================================
*/
SHORT   CasOpenDrawer(CASIF CONST *pCasIf, USHORT usDrawer)
{
    switch( usDrawer ) {            /* which drawer is selected? */
    case ASN_AKEY_ADR:
        /* is it right drawer? */
        if( pCasIf->fbCashierStatus[1] & CAS_NOT_DRAWER_A ) {
            return( CAS_NOT_ALLOWED );
        }
        UieOpenDrawer(ASN_AKEY_ADR);            /* Drawer A, COMMON_DRAWER_A */
        break;

    case ASN_BKEY_ADR:
        /* is it right drawer? */
        if( pCasIf->fbCashierStatus[1] & CAS_NOT_DRAWER_B ) {
            return( CAS_NOT_ALLOWED );
        }
        UieOpenDrawer(ASN_AKEY_ADR);            /* Drawer B, COMMON_DRAWER_B */
        break;

    default:
        return( CAS_PERFORM );
        break;
    }

    return( CAS_PERFORM );
}

/*
*==========================================================================
**    Synopsis:   SHORT   CasOpenPickupLoan(CASIF *Casif)
*
*       Input:    Casif->ulCashierNo
*      Output:    Casif->auchCashierStatus
*                 Casif->uchCashierSecret
*                 Casif->auchCashierName
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_LOCK
*                 CAS_NOT_IN_FILE
*                 CAS_ALREADY_OPENED(Prohibit Multi open)
*                 CAS_FILE_NOT_FOUND
*
**  Description: Cashier sign-in for pickup/loan function,
*                Prohibit Multi open at Cluster system.
*==========================================================================
*/
SHORT   CasOpenPickupLoan(CASIF *pCasif)
{
    USHORT          usParaBlockNo;
    CAS_PARAENTRY   Arg;            /* paramater table entry */
    CAS_FILEHED     CasFileHed;     /* cashier file header */
    USHORT          i;

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    /* Cashier individual lock check */
    if ((Cas_IndLockCheck(pCasif->ulCashierNo)) != CAS_UNLOCK) {
        PifReleaseSem(husCashierSem);
        return(CAS_LOCK);
    }

    /* File Open */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    Cas_GetHeader(&CasFileHed);

    /* Check Cashier assign */
    if ((Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, pCasif->ulCashierNo, &usParaBlockNo)) == CAS_NOT_IN_FILE) {
        Cas_CloseFileReleaseSem();
        return(CAS_NOT_IN_FILE);
    }

    /* Check Cashier sign-out */

    /* R3.3 */
    /* check only whether sign-in cashier no exists on terminal table */
    /* ignor another cashier no exits on pre-teminal table */
    if ( ausCasTerminalTbl[pCasif->usUniqueAddress - 1] != 0 ) {
        if ( ausCasTerminalTbl[pCasif->usUniqueAddress - 1] == pCasif->ulCashierNo ) {
        }  else {
            if ( ausCasTerminalTbl[pCasif->usUniqueAddress - 1] != pCasif->ulCashierNo  ) {
    			for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
			        if (ausCasTerminalTbl[i] == pCasif->ulCashierNo) {
				        Cas_CloseFileReleaseSem();	/* 10/04/01 */
    	                return(CAS_ALREADY_OPENED);
			        }
			        if (ausCasWaiTerminalTbl[i] == pCasif->ulCashierNo) {
				        Cas_CloseFileReleaseSem();	/* 10/04/01 */
    	                return(CAS_ALREADY_OPENED);
			        }
			    }
            }
        }
    } else {
        /* Check another terminal */
    	for (i = 0; i < CAS_TERMINAL_NO_EXTENDED; i++) {
	        if (ausCasTerminalTbl[i] == pCasif->ulCashierNo) {
		        Cas_CloseFileReleaseSem();	/* 10/04/01 */
                return(CAS_ALREADY_OPENED);
	        }
	        if (ausCasWaiTerminalTbl[i] == pCasif->ulCashierNo) {
		        Cas_CloseFileReleaseSem();	/* 10/04/01 */
                return(CAS_ALREADY_OPENED);
	        }
	    }
    }

    /* 06/02/01, allow sign-in during pre-signout */
   	for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
		if (ausNwCasPreTerminalTbl[i] == pCasif->ulCashierNo) {
			ausNwCasPreTerminalTbl[i] = 0;
		}
    }

    /* read Paramater */
    Cas_ParaRead(&CasFileHed, usParaBlockNo, &Arg);

    /* make output condition */
    _tcsncpy(pCasif->auchCashierName, Arg.auchCashierName, PARA_CASHIER_LEN);
	memcpy(pCasif->fbCashierStatus, Arg.fbCashierStatus, PARA_LEN_CASHIER_STATUS);
    pCasif->ulCashierSecret = Arg.ulCashierSecret;
    ausCasTerminalTbl[pCasif->usUniqueAddress - 1] = pCasif->ulCashierNo;
	PifGetDateTime (&datetimeCasTerminalTbl[pCasif->usUniqueAddress - 1]);
    Cas_CloseFileReleaseSem();      /* close file and release semaphore */

    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT   CasClosePickupLoan(CASIF *Casif)
*
*       Input:    Casif->usCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_NOT_IN_FILE
*                 CAS_FILE_NOT_FOUND
*
**  Description: Cashier sign-in close for pickup/loan function,
*                Prohibit Multi open at Cluster system.
*==========================================================================
*/
SHORT   CasClosePickupLoan(CASIF *pCasif)
{
    USHORT          usParaBlockNo;  /* paramater block # */
    CAS_FILEHED     CasFileHed;     /* cashier file header */
	USHORT			i;

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    /* File Open */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    Cas_GetHeader(&CasFileHed);

    /* Check Cashier assign */
    if ((Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, pCasif->ulCashierNo, &usParaBlockNo)) == CAS_NOT_IN_FILE) {
        Cas_CloseFileReleaseSem();
        return(CAS_NOT_IN_FILE);
    }

    /* 06/02/01, allow sign-in during pre-signout */
   	for (i=0; i<CAS_NUMBER_OF_MAX_CASHIER; i++) {
		if (ausNwCasPreTerminalTbl[i] == pCasif->ulCashierNo) {
			ausNwCasPreTerminalTbl[i] = 0;
		}
    }
    /* ausCasPreTerminalTbl[pCasif->usUniqueAddress - 1] = 0x0000; */
    ausCasTerminalTbl[pCasif->usUniqueAddress - 1] = 0x0000; /* Clear Termianl Table */

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */

    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT   CasDelayedBalance(UCHAR uchTermNo)
*
*       Input:    Casif->usCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
*
*==========================================================================
*/
SHORT   CasDelayedBalance(UCHAR uchTermNo, ULONG ulState)
{
	//we set the terminal information to know that it is in delay balance mode
	//that way if we do a check while performing EOD, the Delayed Balance terminals 
	//can still do transactions.
	ausCasDBTerminalTbl[uchTermNo -1] = ulState;

    return SUCCESS;
}
/*====== End of Source ======*/