/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server CASHIER module, Program List                        
*   Category           : C/S CASHIER module, NCR2170 US GENERAL PARPOSE APPLICATION
*   Program Name       : CSCAS.C                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as floolws.
*
*    CasInit();            * cashier function initiarize  *
*    CasCheckAndCreatFile  * cashier check and create file *
*    CasCheckAndDeleteFile * cashier check and delete file *
*    CasSignIn();          * signin in function *
*    CasSignOut();         * signout function *
*    CasClose();           * close function *
*    CasAssign();          * cashier assign function *
*    CasDelete();          * cashier delete function *
*    CasSecretClr();       * clear secret code *
*    CasAllIdRead();       * read all cashier id *
*    CasIndRead();         * read cashier paramater individually *
*    CasIndLock();         * lock cashier individually *
*    CasIndUnLock();       * unlock cashier individually *
*    CasAllLock();         * lock all cashier *
*    CasAllUnLock();       * unlock all cashier *
*    CasChkSignIn();       * check sign in *
*    CasSendFile();        * send file for back up *
*    CasRcvFile();         * receive file for back up *
*    CasPreSignOut();      * pre signout function *
*    CasOpenPickupLoan();  * open  for pickup/loan *
*    CasClosePickupLoan(); * close for pickup/loan *
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*    Date     :Ver.Rev :NAME       :Description
*    Feb-15-93:00.00.01:H.YAMAGUCHI:Initial
*    Sep-03-98:03.03.00:A.Mitsui   :V3.3 Initial
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include    <string.h>
#include    <ecr.h>
#include    <r20_pif.h>
#include    <plu.h>
#include    <paraequ.h>
#include    "cscas.h"
#include    <rfl.h>
#include    <csstbfcc.h>
#include    <appllog.h>
#include    "cscasin.h"
#include    "mypifdefs.h"

#define WIDE(s) L##s

WCHAR   FARCONST  auchCAS_CASHIER[] = WIDE("PARAMCAS");
WCHAR   FARCONST  auchCAS_CASHIEROLD[] = WIDE("PARAM!CA");

USHORT  husCashierSem;    
SHORT   hsCashierFileHandle;
SHORT   hsCashierNewFileHandle;
ULONG	aulCasTerminalTbl[CAS_TERMINAL_OF_CLUSTER];
ULONG	ulCasIndLockMem;
UCHAR   uchCasAllLockMem;
UCHAR   auchCasFlag[CAS_NUMBER_OF_MAX_CASHIER];           
USHORT  ausCasOffset[CAS_TERMINAL_OF_CLUSTER];           

/*
*==========================================================================
**    Synopsis:   VOID CasInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description: Save semaphore handle.
*==========================================================================
*/
VOID CasInit(VOID)
{
    /* Create Semaphore */
    husCashierSem = PifCreateSem( CAS_COUNTER_OF_SEM );  /* save semaphore handle */
}

/*
*==========================================================================
**    Synopsis:   SHORT CasCheckAndCreatFile(USHORT uchNumberOfCashier)
*
*       Input:    usNumberOfCashier - Number of Cashier
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  CAS_PERFORM
*   Abnormal End:  CAS_NO_MAKE_FILE
*
**  Description: Check if not exist , then Create Cashier File.
*==========================================================================
*/
SHORT CasCheckAndCreatFile(USHORT usNumberOfCashier)
{
    SHORT   sStatus;

    /* Open Cashier File */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) < 0) {  /* pif_error_file_exist */
		sStatus = CasCreatFile(usNumberOfCashier);
		return(sStatus);
    }

    PifCloseFile(hsCashierFileHandle);
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasCheckAndDeleteFile(USHOT usNumberOfCashier)
*
*       Input:    ushNumberOfCashier - Number of Cashier
*      Output:    nothing
*       InOut:    nothing
*
**  Return    :
*     Normal End: CAS_PERFORM    
*   Abnormal End: CAS_DELETE_FILE
*
**  Description: Check cashier file, if not equal then delete file.
*==========================================================================
*/
SHORT CasCheckAndDeleteFile(USHORT usNumberOfCashier)
{
    CAS_FILEHED CasFileHed;     /* cashier file header */

    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND){
        return(CAS_PERFORM);
    }

    Cas_GetHeader(&CasFileHed);
    PifCloseFile(hsCashierFileHandle);

    if ( CasFileHed.usNumberOfMaxCashier != usNumberOfCashier) {
        PifDeleteFile(auchCAS_CASHIER);   /* Delete Cashier File  Ignore error */
        PifLog(MODULE_CASHIER, LOG_EVENT_CAS_DELETE_FILE);
        return(CAS_DELETE_FILE);
    }
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasSignIn(CASIF *pCasif)
*
*       Input:    Casif->usCashierNo
*                 Casif->uchCashierSecret
*                 Casif->uchUniqueAddress
*      Output:    Casif->auchCashierStatus
*                 Casif->auchCashierName
*       InOut:
*
**  Return    :
*     Normal End: CAS_PERFORM    
*   Abnormal End: CAS_LOCK
*                 CAS_NOT_IN_FILE
*                 CAS_FILE_NOT_FOUND
*                 CAS_SECRET_UNMATCH
*                 CAS_ALREADY_OPENED (Prohibit Multi open)
*
**  Description: Cashier sign-in function,
*                Prohibit Multi open at Cluster system.
*==========================================================================
*/
SHORT CasSignIn(CASIF *pCasif)
{
    USHORT          usParaBlockNo;  /* paramater block # */
    CAS_PARAENTRY   Arg;            /* paramater table entry */
    CAS_FILEHED     CasFileHed;     /* cashier file header */

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    /* Cashier individual lock check */    
    if ((Cas_IndLockCheck(pCasif->ulCashierNo)) != CAS_UNLOCK) {
        PifReleaseSem(husCashierSem);
        return(CAS_LOCK);
    }

    /* File Open */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND){
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    Cas_GetHeader(&CasFileHed);

    /* Check Cashier assign */
    if ((Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, pCasif->ulCashierNo, &usParaBlockNo)) == CAS_NOT_IN_FILE){
        Cas_CloseFileReleaseSem();
        return(CAS_NOT_IN_FILE);        
    }

    /* Check Cashier sign-out */
    if ( aulCasTerminalTbl[pCasif->usUniqueAddress - 1] != 0){
        if (CAS_PRECLOSE_SIGNIN & auchCasFlag[usParaBlockNo - 1]){            
            Cas_CloseFileReleaseSem();      /* close file and release semaphore */
            return(CAS_DURING_SIGNIN);
        } else {    
            if (aulCasTerminalTbl[pCasif->usUniqueAddress - 1] != pCasif->ulCashierNo){            
                if (Cas_IndSignInCheck(pCasif->ulCashierNo) == CAS_DURING_SIGNIN){
                    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
                    return(CAS_ALREADY_OPENED);
                }
            }
        }
    } else {
        /* Check another terminal */
        if (Cas_IndSignInCheck(pCasif->ulCashierNo) == CAS_DURING_SIGNIN){
            Cas_CloseFileReleaseSem();      /* close file and release semaphore */
            return(CAS_ALREADY_OPENED);
        }
    }

    /* read Paramater */
    Cas_ParaRead(&CasFileHed, usParaBlockNo, &Arg);

    /* check secret code */
    if (pCasif->ulCashierSecret != 0) {
        if (Arg.ulCashierSecret != 0){
            if (Arg.ulCashierSecret != pCasif->ulCashierSecret) {
                Cas_CloseFileReleaseSem();
                return(CAS_SECRET_UNMATCH);  /* return secret code unmatch */
            }
         } else { /* if secret code is not assign, assign secret code */ 
                Arg.ulCashierSecret = pCasif->ulCashierSecret;
                Cas_ParaWrite(&CasFileHed, usParaBlockNo, &Arg);
         }
    }

    /* make output condition */
    memcpy(pCasif->auchCashierName, Arg.auchCashierName, sizeof(pCasif->auchCashierName));
/*    pCasif->fbCashierStatus  = Arg.fbCashierStatus; */
    memcpy(pCasif->fbCashierStatus, Arg.fbCashierStatus, sizeof(pCasif->auchCashierName));
    pCasif->ulCashierSecret = Arg.ulCashierSecret;

    /* set Cashier no in terminal table */
    aulCasTerminalTbl[pCasif->usUniqueAddress - 1] = pCasif->ulCashierNo;
    ausCasOffset[pCasif->usUniqueAddress - 1]     = usParaBlockNo;
    Cas_ChangeFlag(usParaBlockNo, CAS_SET_STATUS, CAS_OPEN_SIGNIN);
    
    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(CAS_PERFORM);
}

/*                 
*==========================================================================
**    Synopsis:   SHORT CasSignOut(CASIF *pCasif)
*
*       Input:    pCasif->usCashierNo
*                 pCasif->usUniqueAddress
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    
*     Normal End: CAS_PERFORM
*
**  Description: Clear Terminal Table for Cashier Sign-out function
*==========================================================================
*/
SHORT CasSignOut(CASIF *pCasif)
{
    USHORT   usParaBlockNo;     /* paramater block # */

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    usParaBlockNo = ausCasOffset[pCasif->usUniqueAddress - 1];
    aulCasTerminalTbl[pCasif->usUniqueAddress - 1] = 0; /* Clear Termianl Table */

    Cas_ChangeFlag(usParaBlockNo, CAS_RESET_STATUS, CAS_OPEN_SIGNIN | CAS_PRECLOSE_SIGNIN);
    PifReleaseSem(husCashierSem);   /* Release Semaphore */
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasClose(ULONG usCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_NO_SIGNIN
*                 CAS_FILE_NOT_FOUND
*            
**  Description: Enforce Sign-out
*==========================================================================
*/
SHORT CasClose(ULONG ulCashierNo)
{
    CAS_FILEHED CasFileHed;         /* cashier file header */
    SHORT       sStatus, csi;       /* status */
    USHORT      usParaBlockNo;      /* paramater block # */

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    /* File Open */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND){
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    Cas_GetHeader(&CasFileHed);

    /* Check Cashier assign */
    sStatus =(Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, ulCashierNo, &usParaBlockNo));
              
    if (sStatus != CAS_NOT_IN_FILE){ 
		Cas_ChangeFlag(usParaBlockNo, CAS_RESET_STATUS, CAS_OPEN_SIGNIN | CAS_PRECLOSE_SIGNIN | CAS_OPEN_PICKUP_LOAN);
		sStatus = CAS_PERFORM;
    } else {
		sStatus = CAS_NO_SIGNIN;
    }

    for (csi = 0; csi < CAS_TERMINAL_OF_CLUSTER; csi++) {
        if (aulCasTerminalTbl[csi] == ulCashierNo) {
            aulCasTerminalTbl[csi] = 0;       /* Clear Terminal Table */
        }
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sStatus);                /* return exist sign-in */
}

/*
*==========================================================================
**    Synopsis:   SHORT CasAssign(CASIF *pCasif)
*
*       Input:    pCasif->ulCashierNo
*                 pCasif->auchCashierName
*                 pCasif->uchCashierSecret
*                 pCasif->fbCashierStatus
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_CASHIER_FULL
*                 CAS_LOCK
*                 CAS_FILE_NOT_FOUND
*
**  Description: Add or replacea Cashier.
*==========================================================================
*/
SHORT CasAssign(CASIF *pCasif)
{
	CAS_PARAENTRY   Arg = { 0 };
    USHORT          usParaBlockNo = 0;
	CAS_FILEHED     CasFileHed = { 0 };

    PifRequestSem(husCashierSem);

    /* CHECK LOCK */
    if (Cas_IndLockCheck(pCasif->ulCashierNo) != CAS_UNLOCK) {
        PifReleaseSem(husCashierSem);
        return(CAS_LOCK);
    }

    /* Open File */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND){
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* Get File header */
    Cas_GetHeader(&CasFileHed);

    /* Check Assign Cashier */
    if (Cas_Index(&CasFileHed, CAS_REGIST, pCasif->ulCashierNo, &usParaBlockNo) == CAS_CASHIER_FULL){
		Cas_CloseFileReleaseSem();
		return(CAS_CASHIER_FULL);   /* return if assign cashier full */
    }

    /* Make Paramater Table from input condition and Write to file */
    memcpy(Arg.auchCashierName, pCasif->auchCashierName, (CAS_NUMBER_OF_CASHIER_NAME + 1) * sizeof(WCHAR));
    memcpy(Arg.fbCashierStatus, pCasif->fbCashierStatus, sizeof(Arg.fbCashierStatus));
    Arg.ulCashierSecret		= pCasif->ulCashierSecret;
    Arg.usGstCheckStartNo	= pCasif->usGstCheckStartNo;  /* V3.3 */
    Arg.usGstCheckEndNo		= pCasif->usGstCheckEndNo;    /* V3.3 */
    Arg.uchChgTipRate		= pCasif->uchChgTipRate;      /* V3.3 */
    Arg.uchTeamNo			= pCasif->uchTeamNo;          /* V3.3 */
    Arg.uchTerminal			= pCasif->uchTerminal;        /* V3.3 */
	Arg.usSupervisorID		= pCasif->usSupervisorID;		//new in version 2.2.0
	Arg.usCtrlStrKickoff	= pCasif->usCtrlStrKickoff;		//new in version 2.2.0
	Arg.usStartupWindow		= pCasif->usStartupWindow;		//new in version 2.2.0
	Arg.ulGroupAssociations = pCasif->ulGroupAssociations;	//new in version 2.2.0
    Cas_ParaWrite(&CasFileHed, usParaBlockNo, &Arg);
    Cas_ChangeFlag(usParaBlockNo, CAS_RESET_STATUS, 0xff);

    Cas_CloseFileReleaseSem();  /* file close and release semaphore */
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasDelete(USHORT usCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_DURING_SIGNIN
*                 CAS_NOT_IN_FILE
*                 CAS_FILE_NOT_FOUND
*
**  Description: Delete Inputed Cashier No from index table
*==========================================================================
*/
SHORT CasDelete(ULONG ulCashierNo)
{
    SHORT       sStatus;        /* error status save area */
    USHORT      usParaBlockNo; /* paramater block entry save area */
    CAS_FILEHED CasFileHed;     /* file header */

    PifRequestSem(husCashierSem);   /* request semaphore */

    /* Open file */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* get file header */
    Cas_GetHeader(&CasFileHed);

    /* Check cashier SIGNIN */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, ulCashierNo, &usParaBlockNo) != CAS_RESIDENT) {
        Cas_CloseFileReleaseSem();      /* file close and releoase semaphore */
        return(CAS_NOT_IN_FILE);    
    } else {
        if (auchCasFlag[usParaBlockNo - 1] & ( CAS_OPEN_SIGNIN | CAS_OPEN_PICKUP_LOAN )) {
            sStatus = CAS_DURING_SIGNIN;
        } else {

            /* Check cashier assign and delete */
			if (Cas_IndexDel(&CasFileHed, ulCashierNo) == CAS_NOT_IN_FILE) {
                sStatus = CAS_NOT_IN_FILE;
            } else {
                sStatus = CAS_PERFORM; 
            }
        }
    }
    Cas_CloseFileReleaseSem();      /* file close and releoase semaphore */
    return(sStatus);        
}

/*
*==========================================================================
**    Synopsis:   SHORT CasSecretClr(USHORT usCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_DURING_SIGNIN
*                 CAS_NOT_IN_FILE
*                 CAS_FILE_NOT_FOUND
*
**  Description: Clear Secret Code of Cashier No when inputted.
*==========================================================================
*/
SHORT CasSecretClr(ULONG ulCashierNo)
{
    SHORT           sStatus;        /* error status save area */
    CAS_PARAENTRY   Arg;            /* Paramater table entry save area */
    USHORT          usParaBlockNo;  /* paramater block entry save area */
    CAS_FILEHED     CasFileHed;     /* file header save area */

    PifRequestSem(husCashierSem);   /* request samaphore */

    /* open file */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* get file header */
    Cas_GetHeader(&CasFileHed);

    /* check cashier assgin */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, ulCashierNo, &usParaBlockNo) != CAS_RESIDENT) {
        sStatus = CAS_NOT_IN_FILE;  
    } else if (auchCasFlag[usParaBlockNo - 1] & ( CAS_OPEN_SIGNIN | CAS_OPEN_PICKUP_LOAN )) {
        sStatus = CAS_DURING_SIGNIN;                    
    } else { 
        Cas_ParaRead(&CasFileHed, usParaBlockNo, &Arg);		/* GET PARAMATER W/ SECRET CODE */
        Arg.ulCashierSecret = 0;							/* CLEAR SECRET CODE */
        Cas_ParaWrite(&CasFileHed, usParaBlockNo, &Arg);	/* WRITE CLEARD SECRET CODE */
        sStatus = CAS_PERFORM;
    }
    Cas_CloseFileReleaseSem();                                /* close file and release semaphore */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasAllIdRead(USHORT usRcvBufferSize, 
*                                             USHORT *ausRcvBuffer)
*
*       Input:    usRcvBufferSize
*      Output:    Nothing
*       InOut:    *auchRcvBuffer
*
**  Return    :
*     Normal End: Number of Cashier  * 0 > or = 0
*   Abnormal End: CAS_FILE_NOT_FOUND * 0 <
*
**  Description: Read all Cashier No. in this file.
*==========================================================================
*/
SHORT CasAllIdRead(USHORT usRcvBufferSize, ULONG *aulRcvBuffer)
{
    CAS_FILEHED     CasFileHed;         /* file header */
    USHORT          cusi;                                                
    CAS_INDEXENTRY  aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER];   /* index table */

    PifRequestSem(husCashierSem);       /* request semaphore */

    /* open file */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* get file header */
    Cas_GetHeader(&CasFileHed);

    if ( 0 == CasFileHed.usNumberOfResidentCashier ) {
        Cas_CloseFileReleaseSem();      /* close file and release semaphore */
        return(CAS_PERFORM);
    }

    /* get index table */
    Cas_ReadFile(CasFileHed.offusOffsetOfIndexTbl, aWorkRcvBuffer, (CasFileHed.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

    /* Count inputed cashier no in index table */
    for (cusi = 0 ;	(cusi < usRcvBufferSize) && (cusi < CasFileHed.usNumberOfResidentCashier) ; cusi++ )   {
        aulRcvBuffer[cusi] = aWorkRcvBuffer[cusi].ulCashierNo;
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(cusi);
}
                                                                                                                                               
/*
*==========================================================================
**    Synopsis:   SHORT CasIndRead(CASIF *pCasif)
*
*       Input:    pCasif->ulCashierNo     
*      Output:    pCasif->ucharCashierStatus
*                 pCasif->auchCashierName
*                 pCasif->uchCashierSecret
*       InOut:
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_NOT_IN_FILE
*                 CAS_FILE_NOT_FOUND
*
**  Description: Read Paramater of inputed Cashier individually.
*==========================================================================
*/
SHORT CasIndRead(CASIF *pCasif)
{
    USHORT          usParaBlockNo;   /* paramater block no save area */
    SHORT           sStatus;          /* error status save area */
    CAS_PARAENTRY   CasParaEntry;     /* paramater entry table save area */
    CAS_FILEHED     CasFileHed;       /* file header save area */

    PifRequestSem(husCashierSem);       /* request samaphore */

    /* open file */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* get file header */
    Cas_GetHeader(&CasFileHed);

    /* Search inputed cashier no in Index table */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, pCasif->ulCashierNo, &usParaBlockNo) == CAS_RESIDENT) {

        /* make output condition */
        Cas_ParaRead(&CasFileHed, usParaBlockNo, &CasParaEntry);
/*        pCasif->fbCashierStatus  = CasParaEntry.fbCashierStatus; */
		memcpy(pCasif->auchCashierName, CasParaEntry.auchCashierName, (CAS_NUMBER_OF_CASHIER_NAME) * sizeof(WCHAR));
        memcpy(pCasif->fbCashierStatus, CasParaEntry.fbCashierStatus, sizeof(pCasif->fbCashierStatus));
        pCasif->ulCashierSecret		= CasParaEntry.ulCashierSecret;
        pCasif->usGstCheckStartNo	= CasParaEntry.usGstCheckStartNo; /* V3.3 */
        pCasif->usGstCheckEndNo		= CasParaEntry.usGstCheckEndNo;   /* V3.3 */
        pCasif->uchChgTipRate		= CasParaEntry.uchChgTipRate;     /* V3.3 */
        pCasif->uchTeamNo			= CasParaEntry.uchTeamNo;         /* V3.3 */
        pCasif->uchTerminal			= CasParaEntry.uchTerminal;       /* V3.3 */
		pCasif->usSupervisorID		= CasParaEntry.usSupervisorID;		//new in version 2.2.0
		pCasif->usCtrlStrKickoff	= CasParaEntry.usCtrlStrKickoff;	//new in version 2.2.0
		pCasif->usStartupWindow		= CasParaEntry.usStartupWindow;		//new in version 2.2.0
		pCasif->ulGroupAssociations = CasParaEntry.ulGroupAssociations;	//new in version 2.2.0
        sStatus = CAS_PERFORM;
    } else {
        sStatus = CAS_NOT_IN_FILE;
    }

    Cas_CloseFileReleaseSem();          /* close file and release semaphore */
    return(sStatus);
}

SHORT CasCheckFileBlockData (VOID)
{
	SHORT           sStatus;          /* error status save area */
	CAS_FILEHED     CasFileHed;       /* file header save area */

	PifRequestSem(husCashierSem);       /* request samaphore */

	/* open file */
	if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) == PIF_ERROR_FILE_NOT_FOUND) {
		PifReleaseSem(husCashierSem);
		return(CAS_FILE_NOT_FOUND);
	}

	/* get file header */
	Cas_GetHeader(&CasFileHed);

	sStatus = Cas_CheckBlockData (&CasFileHed);

	Cas_CloseFileReleaseSem();          /* close file and release semaphore */
	return sStatus;
}

/*
*==========================================================================
**    Synopsis:   SHORT CasIndLock(USHORT ulCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_DURING_SIGNIN
*                 CAS_NOT_IN_FILE
*                 CAS_OTHER_LOCK
*                 CAS_FILE_NOT_FOUND
*
**  Description: Lock inputted Cashier
*==========================================================================
*/
SHORT CasIndLock(ULONG ulCashierNo)
{
    SHORT       sStatus;            /*  error status save area */
    USHORT      usParaBlockNo;     /*  paramater block no save area */
    CAS_FILEHED CasFileHed;         /*  file header save area */

    PifRequestSem(husCashierSem);   /* request samaphore */

    /* open file */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ)) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* get file header */
    Cas_GetHeader(&CasFileHed);

    /* search inputed cashier no and lock */
    if (Cas_Index(&CasFileHed, CAS_SEARCH_ONLY, ulCashierNo, &usParaBlockNo) == CAS_RESIDENT) {
        if (Cas_LockCheck() == CAS_LOCK) {
            sStatus = CAS_OTHER_LOCK;
        } else {
            if ( auchCasFlag[usParaBlockNo - 1] & ( CAS_OPEN_SIGNIN | CAS_OPEN_PICKUP_LOAN )) {
                sStatus = CAS_DURING_SIGNIN;  /* inputed cashier during signin */
            } else {
                ulCasIndLockMem = ulCashierNo;
                sStatus = CAS_PERFORM;        /* inputed cashier not sign in */
            }
        }
    } else {
        sStatus = CAS_NOT_IN_FILE;       
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasIndUnLock(USHORT ulCashierNo)
*
*       Input:    ulCashierNo
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_DIFF_NO
*
**  Description: UnLock inputted Cashier
*==========================================================================
*/
SHORT CasIndUnLock(ULONG ulCashierNo)
{
    SHORT   sStatus;       /* error status save area */

    PifRequestSem(husCashierSem);       /* requse semaphore */

    /* check lock */
    if (ulCasIndLockMem == ulCashierNo) {
        ulCasIndLockMem = 0;
        sStatus = CAS_PERFORM;     /* not other cashier is locked */
    } else {
        sStatus = CAS_DIFF_NO;     /* other cashier is locked */
    }

    PifReleaseSem(husCashierSem);       /* release semaphore */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasAllLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_DURING_SIGNIN
*                 CAS_ALREADY_LOCK
*                 CAS_FILE_NOT_FOUND
*
**  Description: Lock All Cashier.
*==========================================================================
*/
SHORT CasAllLock(VOID)
{
    SHORT           sStatus;
    USHORT          cusi;                                                
    CAS_FILEHED     CasFileHed;         /* file header */
    CAS_INDEXENTRY  aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER];   /* index table */

    PifRequestSem(husCashierSem);               /* reqest semaphore */

    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) < 0) {  /* pif_error_file_exist */
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    if (Cas_LockCheck() != CAS_UNLOCK) {        /* check all lock memory. */
        sStatus = CAS_ALREADY_LOCK;
    } else {
        /* get file header */
        Cas_GetHeader(&CasFileHed);
        if ( 0 == CasFileHed.usNumberOfResidentCashier ) {
            Cas_CloseFileReleaseSem();  /* close file and release semaphore */
            return(CAS_PERFORM);
        }

        /* get index table */
        Cas_ReadFile(CasFileHed.offusOffsetOfIndexTbl, aWorkRcvBuffer, (CasFileHed.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

        /* Count inputed cashier no in index table */
        sStatus = CAS_PERFORM;
        for (cusi = 0 ; cusi < CasFileHed.usNumberOfResidentCashier ; cusi++) {
            if ( auchCasFlag[(aWorkRcvBuffer[cusi].usParaBlock) -1] & ( CAS_OPEN_SIGNIN | CAS_OPEN_PICKUP_LOAN ) ) {
                sStatus = CAS_DURING_SIGNIN;
                break;
            }
        }

        if ( sStatus == CAS_PERFORM ) {        /* check during sign in */
            uchCasAllLockMem = 0xff;
        }
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   VOID CasAllUnLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Unlock All Cahiser
*==========================================================================
*/
VOID CasAllUnLock(VOID)
{
    PifRequestSem(husCashierSem);       /* reqest semaphore */
    uchCasAllLockMem = 0x00;            /* clear all cashier lock mamory */
    PifReleaseSem(husCashierSem);       /* release semaphore */
}

/*
*==========================================================================
**    Synopsis:   SHORT CasChkSignIn(ULONG *aulRcvBuffer)
*
*       Input:    Nothing
*      Output:    *aulRcvBuffer
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_DURING_SIGNIN
*                 CAS_FILE_NOT_FOUND
*                
**  Description: Check if sign-in Cashier exist.
*==========================================================================
*/
SHORT CasChkSignIn(ULONG *aulRcvBuffer)
{
    SHORT           sStatus;            /* return status save area */
    USHORT          cusi;                                                
    CAS_FILEHED     CasFileHed;         /* file header */
    CAS_INDEXENTRY  aWorkRcvBuffer[CAS_NUMBER_OF_MAX_CASHIER];   /* index table */

    PifRequestSem(husCashierSem);       /* reqest semaphore */

    /* check sign in */
    if ((hsCashierFileHandle = PifOpenFile(auchCAS_CASHIER, auchOLD_FILE_READ_WRITE)) < 0) {  /* pif_error_file_exist */
        PifReleaseSem(husCashierSem);
        return(CAS_FILE_NOT_FOUND);
    }

    /* get file header */
    Cas_GetHeader(&CasFileHed);

    if (0 == CasFileHed.usNumberOfResidentCashier ) {
        Cas_CloseFileReleaseSem();      /* close file and release semaphore */
        return(CAS_PERFORM);
    }

    /* get index table */
    Cas_ReadFile(CasFileHed.offusOffsetOfIndexTbl, aWorkRcvBuffer, (CasFileHed.usNumberOfResidentCashier * sizeof(CAS_INDEXENTRY)));

    /* Count inputed cashier no in index table */
    sStatus = CAS_PERFORM;
    memset(aulRcvBuffer, 0x00, CAS_NUMBER_OF_MAX_CASHIER * sizeof(ULONG));

    for (cusi = 0 ; cusi < CasFileHed.usNumberOfResidentCashier ; cusi++) {
        if ( auchCasFlag[(aWorkRcvBuffer[cusi].usParaBlock) -1] & ( CAS_OPEN_SIGNIN | CAS_OPEN_PICKUP_LOAN ) ) {
            aulRcvBuffer[cusi] = aWorkRcvBuffer[cusi].ulCashierNo;
            sStatus = CAS_DURING_SIGNIN;
        }
    }

    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasSendFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_COM_ERROR
*                
**  Description: Request Back Up.
*==========================================================================
*/
SHORT CasSendFile(VOID)
{
    return(CAS_PERFORM);
}
/*
*==========================================================================
*
**    Synopsis:   SHORT CasRcvFile(CHAR  *puchRcvData,
*                                           USHORT usRcvLen,
*                                           UCHAR  *puchSndData,
*                                           USHORT *pusSndLen)
*
*       Input:    *puchRcvData   * Pointer to Request Data. *
*                 usRcvLen       * Width of puchRcvData *
*       Output:   *puchSndData   * Storage location for Backup data. *
*                 *pusSndLen     * Width of puchSndData *
*       InOut:    Nothing
*
**  Return    :
*     Normal End: CAS_PERFORM
*   Abnormal End: CAS_COMM_ERROR
*                 CAS_FILE_NOT_FOUND   
*                
**  Description: Response Back Up.
*==========================================================================
*/
SHORT CasRcvFile(UCHAR  *puchRcvData,
                          USHORT usRcvLen,
                          UCHAR  *puchSndData,
                          USHORT *pusSndLen)
{
    USHORT  usDmy;
    UCHAR   *puchDmy;

    puchDmy=puchRcvData;
    puchDmy=puchSndData;
    usDmy=*pusSndLen;
    usDmy=usRcvLen;

    return(CAS_PERFORM);
}

/*                 
*==========================================================================
**    Synopsis:   SHORT CasPreSignOut(CASIF *pCasif)
*
*       Input:    pCasif->usUniqueAddress
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    
*     Normal End: CAS_PERFORM
*
**  Description: Set pre close flag.
*==========================================================================
*/
SHORT CasPreSignOut(CASIF *pCasif)
{
    USHORT   usParaBlockNo;      /* paramater block # */

    PifRequestSem(husCashierSem);   /* Request Semaphore */

    usParaBlockNo = ausCasOffset[pCasif->usUniqueAddress - 1];

    Cas_ChangeFlag(usParaBlockNo, CAS_SET_STATUS, CAS_PRECLOSE_SIGNIN);

    PifReleaseSem(husCashierSem);      /* release semaphore */
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasOpenPickupLoan(CASIF *Casif)
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
SHORT CasOpenPickupLoan(CASIF *pCasif)
{
    USHORT          usParaBlockNo;  /* paramater block # */
    CAS_PARAENTRY   Arg;            /* paramater table entry */
    CAS_FILEHED     CasFileHed;     /* cashier file header */

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
    if ( ( CAS_OPEN_PICKUP_LOAN ) & auchCasFlag[usParaBlockNo - 1] ) {        
        Cas_CloseFileReleaseSem();      /* close file and release semaphore */
        return(CAS_ALREADY_OPENED);
    } 

    /* read Paramater */
    Cas_ParaRead(&CasFileHed, usParaBlockNo, &Arg);

    /* make output condition */
    memcpy(pCasif->auchCashierName, Arg.auchCashierName, sizeof(pCasif->auchCashierName));
/*    pCasif->fbCashierStatus  = Arg.fbCashierStatus; */
    memcpy(pCasif->fbCashierStatus, Arg.fbCashierStatus, sizeof(pCasif->fbCashierStatus));

    pCasif->ulCashierSecret = Arg.ulCashierSecret;

    Cas_ChangeFlag(usParaBlockNo, CAS_SET_STATUS, CAS_OPEN_PICKUP_LOAN);
    
    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(CAS_PERFORM);
}

/*
*==========================================================================
**    Synopsis:   SHORT CasClosePickupLoan(CASIF *Casif)
*
*       Input:    Casif->ulCashierNo
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
SHORT CasClosePickupLoan(CASIF *pCasif)
{
    USHORT          usParaBlockNo;  /* paramater block # */
    CAS_FILEHED     CasFileHed;     /* cashier file header */

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

    Cas_ChangeFlag(usParaBlockNo, CAS_RESET_STATUS, CAS_OPEN_PICKUP_LOAN);
    
    Cas_CloseFileReleaseSem();      /* close file and release semaphore */
    return(CAS_PERFORM);
}

/*====== End of Source ======*/
