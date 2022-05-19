/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Cleanup Functions Source File
* Category    : ISP Server module, US Hospitality Model
* Program Name: ISPCLEAN.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           IspCleanUpSystem();         Cleanup System
*             IspCleanUpUpdatePLU();      Update sub-index
*             IspCleanUpLockKB();         Unlock KeyBoard
*
*           IspCleanUpLockFun();    C   Cleanup locked Function
*             IspCleanUpLockCas();        Cleanup Cashier function
*             IspCleanUpLockWai();        Cleanup Waiter  function
*             IspCleanUpLockGCF();        Cleanup GCF     function
*             IspCleanUpLockOp();         Cleanup Op      function
*             IspCleanUpLockETK();  A     Cleanup ETK     function
*
*           IspSetNbStopFunc(VOID);     Set   Stop function flag
*           IspResetNbStopFunc(VOID);   Reset Stop function flag
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-23-92:00.00.01:H.Yamaguchi: initial                                   
* Sep-23-92:00.00.02:T.Kojima   : add to call "RmtInitFileTbl()"  and 
*          :        :           : "RmtCloseAllFile()" in "IspCleanUpSystem()"                                   
* Jun-23-93:00.00.01:H.Yamaguchi: Adds ETK unlock                                   
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
* Dec-07-99:01.00.00:hrkato     : Saratoga
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
#include	<tchar.h>
#include    <memory.h>
#include    <stdio.h>
#include    <math.h>

#include    <ecr.h>
#include	<rfl.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <nb.h>
#include    <log.h>
#include    <appllog.h>
#include    <uic.h>
#include    <cscas.h>
#include    <csstbcas.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <csop.h>
#include    <csstbopr.h>
#include    <csstbfcc.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <isp.h>
#include    <ej.h>
#include    "ispcom.h"
#include    "rmt.h"
#include    <csttl.h>
#include "ttl.h"
#include    <csstbstb.h>
#include    <BlFWif.h>

/*
*===========================================================================
** Synopsis:    VOID    IspCleanUpSystem(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  Cleanup all system condition.
*===========================================================================
*/
#if defined(IspCleanUpSystem)
VOID   IspCleanUpSystem_Special (VOID);
VOID   IspCleanUpSystem_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	USHORT fsIspCleanUP_Save = fsIspCleanUP;           /* Locked all function flag */

	IspCleanUpSystem_Special();

	sprintf (xBuff, "IspCleanUpSystem() fsIspCleanUP 0x%x -> 0x%x  called from %s  %d", fsIspCleanUP_Save, fsIspCleanUP, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
}
VOID   IspCleanUpSystem_Special(VOID)
#else
VOID    IspCleanUpSystem(VOID)
#endif
{
     
    IspCleanUpUpdatePLU();               /* Creates sub index */
    IspCleanUpLockKB();                  /* Unlock  keyboard */
    fsIspCleanUP = 0;                    /* Reset   cleanup flag */
    RmtCloseAllFile();                   /* close all files              */
    RmtInitFileTbl();                    /* initialize remote file table */

}    

/*
*===========================================================================
** Synopsis:    VOID    IspCleanUpUpdatePLU(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  Update sub-index of PLU.
*===========================================================================
*/
#if defined(IspCleanUpUpdatePLU)
SHORT   IspCleanUpUpdatePLU_Special (VOID);
SHORT   IspCleanUpUpdatePLU_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspCleanUP_Save = fsIspCleanUP;           /* Locked all function flag */

	sRetValue = IspCleanUpUpdatePLU_Special();

	sprintf (xBuff, "IspCleanUpUpdatePLU() fsIspCleanUP 0x%x -> 0x%x  sRetValue %d called from %s  %d", fsIspCleanUP_Save, fsIspCleanUP, sRetValue, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpUpdatePLU_Special(VOID)
#else
SHORT    IspCleanUpUpdatePLU(VOID)
#endif
{
	SHORT  sReturn = 0;

    if ( fsIspCleanUP & ISP_CREATE_SUBINDEX_PLU ) { 
        PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CHANGE_PLU);   /* Write log */
        sReturn = SerOpPluFileUpdate(husIspOpHand);         /* Create sub-index for PLU */
        fsIspCleanUP &= ~ISP_CREATE_SUBINDEX_PLU;           /* Reset flag */
        PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CHANGE_PLU);   /* Write log */
        PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sReturn));   /* Write log */
		if (IspIamMaster() == ISP_SUCCESS) {
			// we be Master so inform Satellite Terminals of PLU change
			NbWriteMessage(NB_MESOFFSET0, NB_REQPLU);
			PifSleep(1500);   // Give message time to be broadcast to cluster before responding back to requestor
			BlFwIfReloadOEPGroup();
		}
    }

	return sReturn;
}    

/*
*===========================================================================
** Synopsis:    SHORT    IspCleanUpLockKB(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      UIC_SUCCESS or UIC_OTHER_USE
*
** Description:  Unlock KeyBoard
*===========================================================================
*/
#if defined(IspCleanUpLockKB)
SHORT   IspCleanUpLockKB_Special (VOID);
SHORT   IspCleanUpLockKB_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspCleanUP_Save = fsIspCleanUP;           /* Locked all function flag */

	sRetValue = IspCleanUpLockKB_Special();

	sprintf (xBuff, "IspCleanUpLockKB() fsIspCleanUP 0x%x -> 0x%x  sRetValue %d called from %s  %d", fsIspCleanUP_Save, fsIspCleanUP, sRetValue, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpLockKB_Special(VOID)
#else
SHORT   IspCleanUpLockKB(VOID)
#endif
{
    SHORT sError = UIC_SUCCESS;

    if ( hsIspKeyBoardHand > 0 ) {
        sError = UicResetFlag(hsIspKeyBoardHand);   /* Unlock KeyBoard */
        if ( sError == UIC_SUCCESS)  {
            hsIspKeyBoardHand = 0 ;                     /* Clear */
            fsIspCleanUP &= ~ISP_LOCKED_KEYBOARD ;    /* Reset Locked flag */
            PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_UNLOCK_KEYBOARD);    /* Write log */
            IspResetNbStopFunc();                     /* Reset Stop all function flag */
            return(sError);
        }
    }
    return(sError);
}    

/*
*===========================================================================
** Synopsis:    VOID    IspCleanUpLockFun(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  Cleanup Cashier/Waiter/GCF/Opepara. functions.
*===========================================================================
*/
#if defined(IspCleanUpLockFun)
VOID   IspCleanUpLockFun_Special (VOID);
VOID   IspCleanUpLockFun_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	USHORT fsIspCleanUP_Save = fsIspCleanUP;           /* Locked all function flag */

	IspCleanUpLockFun_Special();

	sprintf (xBuff, "IspCleanUpLockFun() fsIspCleanUP 0x%x -> 0x%x  called from %s  %d", fsIspCleanUP_Save, fsIspCleanUP, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
}
VOID   IspCleanUpLockFun_Special(VOID)
#else
VOID    IspCleanUpLockFun(VOID)
#endif
{
    IspCleanUpLockCas(0);                   /* Unlock Cashier  */
    IspCleanUpLockGCF(0);                   /* Unlock GCF      */
    IspCleanUpLockOp();                     /* Unlock Opepara  */
    IspCleanUpLockETK();                    /* Unlock ETK      */
    IspCleanUpEJ();                         /* Clean-Up E/J Status, V3.3 */
    EJUnLock(EJLOCKFG_ISP);                 /* Unlock ISP Reset Flag, V3.3 */
    IspCleanUpPickupCas(ulIspPickupCasNO);  /* Close pickup cashier number, Saratoga */
    IspCleanUpLockMas();                    /* clean up MMM, saratoga */
    IspCleanUpLockPLUTtl();                 /* Unlock PLU Total, saratoga */
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCleanUpLockCas(USHORT fsFlag);
*     Input:    fsFlag    - 0: Mean all unlock and ind. unlock
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CAS_PERFORM
*
** Description:  This function executes handling at other error.
*===========================================================================
*/
#if defined(IspCleanUpLockCas)
SHORT   IspCleanUpLockCas_Special (USHORT fsFlag);
SHORT   IspCleanUpLockCas_Debug(USHORT fsFlag, char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	sRetValue = IspCleanUpLockCas_Special(fsFlag);

	sprintf (xBuff, "IspCleanUpLockCas() fsFlag 0x%x fsIspLockedFC 0x%x -> 0x%x  sRetValue %d called from %s  %d", fsFlag, fsIspLockedFC_Save, fsIspLockedFC, sRetValue, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpLockCas_Special(USHORT fsFlag)
#else
SHORT   IspCleanUpLockCas(USHORT fsFlag)
#endif
{       
    SHORT   sError = CAS_PERFORM;

    if ( 0 == fsFlag || ISP_LOCK_ALLCASHIER == fsFlag) { 
        if ( fsIspLockedFC & ISP_LOCK_ALLCASHIER ) {
            SerCasAllUnLock();               /* All unlock cashier */
            fsIspLockedFC &= ~ISP_LOCK_ALLCASHIER;
        }
    }

    if ( 0 == fsFlag || ISP_LOCK_INDCASHIER == fsFlag) { 
        if ( fsIspLockedFC & ISP_LOCK_INDCASHIER ) {
            sError = SerCasIndUnLock(ulIspLockedCasNO);   /* Ind. unlock cashier no. */
            fsIspLockedFC &= ~ISP_LOCK_INDCASHIER;
        }
    }
    return(sError);
}    
/*
*===========================================================================
** Synopsis:    SHORT    IspCleanUpLockGCF(USHORT fsFlag);
*     Input:    fsFlag    - 0: Mean all unlock and ind. unlock
*     Output:   nothing
*     InOut:    nothing
*
** Return:      GCF_SUCCESS
*
** Description:  This function executes handling at other error.
*===========================================================================
*/
#if defined(IspCleanUpLockGCF)
SHORT   IspCleanUpLockGCF_Special (USHORT fsFlag);
SHORT   IspCleanUpLockGCF_Debug(USHORT fsFlag, char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	sRetValue = IspCleanUpLockGCF_Special(fsFlag);

	sprintf (xBuff, "IspCleanUpLockGCF() fsFlag 0x%x fsIspLockedFC 0x%x -> 0x%x  sRetValue %d called from %s  %d", fsFlag, fsIspLockedFC_Save, fsIspLockedFC, sRetValue, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpLockGCF_Special(USHORT fsFlag)
#else
SHORT    IspCleanUpLockGCF(USHORT fsFlag)
#endif
{
    SHORT   sError = GCF_SUCCESS;

    if ( 0 == fsFlag || ISP_LOCK_ALLGCF == fsFlag ) { 
        if ( fsIspLockedFC & ISP_LOCK_ALLGCF) {
            sError = SerGusAllUnLock();       /* All unlock GCF function */
            fsIspLockedFC &= ~ISP_LOCK_ALLGCF;
        }
    }

    if ( 0 == fsFlag || ISP_LOCK_INDGCF == fsFlag ) { 
        if ( fsIspLockedFC & ISP_LOCK_INDGCF) {
            sError = SerGusResetReadFlag(usIspLockedGCFNO);  /* Unlock a GCF  */
            fsIspLockedFC &= ~ISP_LOCK_INDGCF;
        }
    }
    return(sError);
}    

/*
*===========================================================================
** Synopsis:    VOID     IspCleanUpLockOp(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  This function executes handling at other error.
*===========================================================================
*/
#if defined(IspCleanUpLockOp)
VOID   IspCleanUpLockOp_Special (VOID);
VOID   IspCleanUpLockOp_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	IspCleanUpLockOp_Special();

	sprintf (xBuff, "IspCleanUpLockOp() fsIspLockedFC 0x%x -> 0x%x called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
}
VOID   IspCleanUpLockOp_Special(VOID)
#else
VOID    IspCleanUpLockOp(VOID)
#endif
{

    if ( fsIspLockedFC & ISP_LOCK_ALLOPEPARA) {
        SerOpUnLock();                    /* All unlock opepara function */
        fsIspLockedFC &= ~ISP_LOCK_ALLOPEPARA;
        husIspOpHand = 0 ;                /* Clear OpHandle handle */
    }
}    

/*
*===========================================================================
** Synopsis:    VOID    IspSetNbStopFunc();
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Set " Stop function " to Notice Board.
*===========================================================================
*/
#if defined(IspSetNbStopFunc)
VOID   IspSetNbStopFunc_Special (VOID);
VOID   IspSetNbStopFunc_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	IspSetNbStopFunc_Special();

	sprintf (xBuff, "IspSetNbStopFunc() fsIspLockedFC 0x%x -> 0x%x called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
}
VOID   IspSetNbStopFunc_Special(VOID)
#else
VOID    IspSetNbStopFunc(VOID)
#endif
{
    if ( fsIspLockedFC & ISP_LOCK_ALLCASHIER ) {
		USHORT  usType = NB_MTBAKCOPY_FLAG  ;      /* Master's backup copy inf. flag  */

        if (ISP_IAM_BMASTER) {
            usType = NB_BMBAKCOPY_FLAG  ;  /* B-Master's backup copy inf. flag */
        }
        NbWriteInfFlag(usType, NB_STOPALLFUN );   /* Write Stop function flag */
        fsIspCleanUP |= ISP_NB_STOPPED;           /* Set NB Stop flag */ 
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspResetNbStopFunc();
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Reset " Stop function " to Notice Board.
*===========================================================================
*/
#if defined(IspResetNbStopFunc)
VOID   IspResetNbStopFunc_Special (VOID);
VOID   IspResetNbStopFunc_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	IspResetNbStopFunc_Special();

	sprintf (xBuff, "IspResetNbStopFunc() fsIspLockedFC 0x%x -> 0x%x called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
}
VOID   IspResetNbStopFunc_Special(VOID)
#else
VOID    IspResetNbStopFunc(VOID)
#endif
{
    if ( fsIspCleanUP & ISP_NB_STOPPED ) { 
		USHORT  usType = NB_MTBAKCOPY_FLAG  ;      /* Master's backup copy inf. flag  */

        if (ISP_IAM_BMASTER) {
            usType = NB_BMBAKCOPY_FLAG  ;  /* B-Master's backup copy inf. flag */
        }
        NbWriteInfFlag(usType, 0 );        /* Reset Stop flag      */
        fsIspCleanUP &= ~ISP_NB_STOPPED;   /* Reset NB Stop flag   */ 
    }
}

/*
*===========================================================================
** Synopsis:    VOID     IspCleanUpLockETK(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  This function executes handling at other error.
*===========================================================================
*/
#if defined(IspCleanUpLockETK)
VOID   IspCleanUpLockETK_Special (VOID);
VOID   IspCleanUpLockETK_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	IspCleanUpLockETK_Special();

	sprintf (xBuff, "IspCleanUpLockETK() fsIspLockedFC 0x%x -> 0x%x  called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
}
VOID   IspCleanUpLockETK_Special(VOID)
#else
VOID    IspCleanUpLockETK(VOID)
#endif
{
    if ( fsIspLockedFC & ISP_LOCK_ALLETK) {
        SerEtkAllUnLock();
        fsIspLockedFC &= ~ISP_LOCK_ALLETK;
    }
}    

/*
*===========================================================================
** Synopsis:    SHORT    IspCleanUpPickupCas(USHORT usCashierNo);
*     Input:    usCashierNo  - Cashier number
*     Output:   nothing
*     InOut:    nothing
*
** Return:      CAS_PERFORM
*
** Description:  This function executes to close signout.       Saratoga
*===========================================================================
*/
#if defined(IspCleanUpPickupCas)
SHORT   IspCleanUpPickupCas_Special (ULONG ulCashierNo);
SHORT   IspCleanUpPickupCas_Debug(ULONG ulCashierNo, char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	sRetValue = IspCleanUpPickupCas_Special(ulCashierNo);

	sprintf (xBuff, "IspCleanUpPickupCas() fsIspLockedFC 0x%x -> 0x%x  sRetValue %d called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, sRetValue, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpPickupCas_Special(ULONG ulCashierNo)
#else
SHORT   IspCleanUpPickupCas(ULONG ulCashierNo)
#endif
{
    SHORT           sError = CAS_PERFORM;

    if (fsIspLockedFC & ISP_LOCK_PICKUPCASHIER) {
		CASIF           Casif = {0};

		Casif.ulCashierNo = ulCashierNo;

        if (0 != sIspCounter) {
            IspMakeClosePickFormat();

            do {
                sError = TrnSendTotal();
            } while (TRN_SUCCESS != sError);

        }

        sError = CliCasClosePickupLoan(&Casif);   /* close pickup cashier no. */
        
		/* clear consolidation storage to avoid storage full error, V1.0.12 */
        TrnInitialize(TRANI_GCFQUAL | TRANI_CURQUAL | TRANI_ITEM | TRANI_CONSOLI);

        SerCasIndTermUnLock(CliReadUAddr());

        fsIspLockedFC &= ~ISP_LOCK_PICKUPCASHIER;
    }

    return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCleanUpLockMas(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      SPLU_COMPLETED
*
** Description:  This function executes handling at other error.    Saratoga
*===========================================================================
*/
#if defined(IspCleanUpLockMas)
SHORT   IspCleanUpLockMas_Special (VOID);
SHORT   IspCleanUpLockMas_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	sRetValue = IspCleanUpLockMas_Special();

	sprintf (xBuff, "IspCleanUpLockMas() fsIspLockedFC 0x%x -> 0x%x called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpLockMas_Special(VOID)
#else
SHORT   IspCleanUpLockMas(VOID)
#endif
{
    SHORT   sError = SPLU_COMPLETED;

    if (fsIspLockedFC & ISP_LOCK_MASSMEMORY) {
        SerIspPluExitCritMode(husIspMasHand , 0); /* release handle  */
        fsIspLockedFC &= ~ISP_LOCK_MASSMEMORY;
    }
    return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCleanUpLockPLUTtl(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      SPLU_COMPLETED
*
** Description:  This function executes handling at other error.    Saratoga
*===========================================================================
*/
#if defined(IspCleanUpLockPLUTtl)
SHORT   IspCleanUpLockPLUTtl_Special (VOID);
SHORT   IspCleanUpLockPLUTtl_Debug(char *aszFilePath, int nLineNo)
{
	char xBuff[256];
	SHORT sRetValue;
	USHORT fsIspLockedFC_Save = fsIspLockedFC;           /* Locked all function flag */

	sRetValue = IspCleanUpLockPLUTtl_Special();

	sprintf (xBuff, "IspCleanUpLockPLUTtl() fsIspLockedFC 0x%x -> 0x%x called from %s  %d", fsIspLockedFC_Save, fsIspLockedFC, RflPathChop(aszFilePath), nLineNo);
	NHPOS_NONASSERT_TEXT(xBuff)
	return sRetValue;
}
SHORT   IspCleanUpLockPLUTtl_Special(VOID)
#else
SHORT   IspCleanUpLockPLUTtl(VOID)
#endif
{
    SHORT   sError = ISP_SUCCESS;

    if (fsIspLockedFC & ISP_LOCK_PLUTTL) {
        TtlPLUCloseRec();
        fsIspLockedFC &= ~ISP_LOCK_PLUTTL;
    }
    return(sError);
}

/*===== END OF SOURCE =====*/