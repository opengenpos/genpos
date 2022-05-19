/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Shared Printer Manager Main                 
* Category    : Shared Printer Manager, NCR 2170 US HOSP Appl
* Program Name: Shr.C                                                      
* --------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*   ShrInit()
*    -ShrInitFile()
*    -ShrInitSem()
*    -ShrInitSPP()
*   ShrCancel()
*    -ShrRestoreWrtP()
*   ShrTermLock()
*   ShrTermUnLock()
*   ShrPrint()
*    -ShrLockCheck()
*    -ShrLock()
*    -ShrUnLock()
*    -ShrPrevDataCheck()
*    -ShrMakeRetVal2()
*    -ShrMakeRetVal()
*    -ShrFrameCheck()
*      -ShrDataLost()
*    -ShrWriteSpool()
*      -ShrWriteFile()
*    -ShrPolling()
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Jun-16-93 : 00.00.01 : NAKAJIMA,Ken : Init.                                   
* May-01-95 : 02.05.04 : M.Suzuki     : Add R3.0                                   
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
;===========================================================================
;+                  I N C L U D E     F I L E s                            +
;===========================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>
#include <memory.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <pmg.h>
#include <appllog.h>
#include <shr.h>
#include "shrlocal.h"

/*
============================================================================
+                  S T A T I C   R A M s                                   +
============================================================================
*/
/* -- semaphore handle -- */
PifSemHandle usShrSemStartSPP = PIF_SEM_INVALID_HANDLE;        /* semaphore handle for start SPP */
PifSemHandle usShrSemRWDisk = PIF_SEM_INVALID_HANDLE;          /* semaphore handle for reading and writing disk */
PifSemHandle usShrSemDrawer = PIF_SEM_INVALID_HANDLE;          /* semaphore handle for drawer and printer */
PifSemHandle usShrSemStubFunc = PIF_SEM_INVALID_HANDLE;        /* semaphore handle for stub function */

// For each terminal we provide data areas in which we store the information
// for that terminal's request.  We have sized these 2 greater because these
// data structures are used as one based rather than zero based.  In
// other words, the first element used is ShrInf[1] and not ShrInf[0].
static SHRHEADERINF    ShrInf[SHR_MAX_TERM + 2];
                                /* store previous data for each terminal */
static UCHAR  auchShrPrevLock[SHR_MAX_TERM + 2];
                                /* store previous data' s lock status */

/* -- file access -- */
USHORT usShrReadP;              /* spool file read pointer */
USHORT usShrWriteP;             /* spool file write pointer */
USHORT usShrCurWPoint;          /* spool file current write pointer */
USHORT usShrFileEnd;            /* spool file end offset */
USHORT fbShrSSFFull;            /* spool file status flag */

/* -- terminal lock -- */
SHRLOCK dShrLock;               /* terminal lock flag */

/* -- file  -- */
TCHAR FARCONST  aszShrSpoolName[] = _T("SPOOLSHR"); /* file name */
UCHAR FARCONST  aszShrNewFile[] = "n";          /* open mode new, read, write */
UCHAR FARCONST  aszShrOldFile[] = "o";          /* open mode old, read, write */
SHORT  hsShrSpool;                              /* spool file handle */


/**
;===========================================================================
;+              P R O G R A M    D E C L A R A T I O N s                   +
;===========================================================================
**/
/*
*===========================================================================
** Format  : VOID   ShrInit(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from application startup routine.
*            starts print thread, make semaphore.
*===========================================================================
*/
VOID   ShrInit(VOID)
{
    ShrInitFile();                              /* create file */
    ShrInitSem();                               /* create semaphore */
    ShrInitSPP();                               /* start SPP */
}


/*
*===========================================================================
** Format  : VOID   ShrInitFile(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function makes spool file.
*===========================================================================
*/
VOID   ShrInitFile(VOID)
{
    ULONG   ulPos;

    /* -- open spool file -- */
    if( (hsShrSpool = PifOpenFile(aszShrSpoolName, aszShrOldFile)) < 0) {
                                                /* file exist ? */
        if (hsShrSpool == PIF_ERROR_FILE_NOT_FOUND) {

            if( (hsShrSpool = PifOpenFile(aszShrSpoolName, aszShrNewFile)) < 0) {
                                                /* create file */
                PifAbort(MODULE_SHR, FAULT_ERROR_FILE_OPEN);
            }

        } else {
            PifAbort(MODULE_SHR, FAULT_ERROR_FILE_OPEN);
        }
    }

    if( PifSeekFile(hsShrSpool, SHR_SPOOL_SIZE, &ulPos) < 0) {
                                                /* seek spool file */
                                                /* check file can have */
                                                /* "SHR_SPOOL_SIZE" bytes size or not */
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_SEEK);
    }

    if( (ULONG)SHR_SPOOL_SIZE != ulPos ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_SEEK);
    }

}

/*            
*===========================================================================
** Format  : VOID   ShrInitSem(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function makes semaphore.
*===========================================================================
*/
VOID   ShrInitSem(VOID)
{
    usShrSemStartSPP = PifCreateSem(0);
    usShrSemRWDisk   = PifCreateSem(1);
    usShrSemDrawer   = PifCreateSem(1);
    usShrSemStubFunc = PifCreateSem(1);
}

/*
*===========================================================================
** Format  : VOID   ShrInitSPP(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function start print thread (SPP).
*===========================================================================
*/
VOID  ShrInitSPP(VOID)
{
	CHAR CONST  *aszShrThreadName = "SHARED_P";           /* thread name */
	VOID (THREADENTRY *pFunc)(VOID) = ShrPrintProcess;    /* Add R3.0 */

    PifBeginThread(pFunc, 0, 0, PRIO_SHARED_P, aszShrThreadName); 
}

/*
*===========================================================================
** Format  : SHORT  ShrCancel(UCHAR  uchTerm)
*               
*    Input : UCHAR  uchTerm
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from client/server module
*===========================================================================
*/
SHORT  ShrCancel(UCHAR  uchTerm)                     
{
    if (0 != dShrLock.uchTermAdr && uchTerm != dShrLock.uchTermAdr) {
        return SHR_FAIL;
    }

    PifRequestSem(usShrSemStubFunc);

    ShrRestoreWrtP();                           /* restore write pointer */

    if (dShrLock.uchTermAdr != 0) {             /* for safety */
        memset(&ShrInf[dShrLock.uchTermAdr], '\0', sizeof(SHRHEADERINF));
                                                /* clear previous data */
        auchShrPrevLock[dShrLock.uchTermAdr] = 0;
                                                /* clear previous data's lock memory */
    }
    memset(&dShrLock, '\0', sizeof(dShrLock));  /* clear lock memory */

    PmgFont(PMG_PRT_RECEIPT, PRT_FONT_A);       /* set normal font for cancel A/C#70, A/C#100 */

    PifReleaseSem(usShrSemStubFunc);

	return SHR_PERFORM;
}

/*
*===========================================================================
** Format  : VOID   ShrRestoreWrtP(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function copy write pointer to current write pointer
*===========================================================================
*/
VOID   ShrRestoreWrtP(VOID)
{
    PifRequestSem(usShrSemRWDisk);

    usShrCurWPoint = usShrWriteP;

    fbShrSSFFull &=  ~SHR_TEMP_SSF_FULL;        /* file full flag off */

    PifReleaseSem(usShrSemRWDisk);
}


/*
*===========================================================================
** Format  : SHORT   ShrTermLock(UCHAR uchTermAdr)
*               
*    Input : UCHAR uchTermAdr       -terminal unique address
*   Output : SHR_TIMERON            -lock success
*                                    (for start timer wathing)
*            SHR_BUSY               -another terminal is locked
*    InOut : none
** Return  : none
*
** Synopsis: This function locks specified terminal
*===========================================================================
*/
SHORT   ShrTermLock(UCHAR uchTermAdr)
{
    SHORT sRet = SHR_BUSY;

    PifRequestSem(usShrSemStubFunc);

    if( ( *((USHORT *)&dShrLock) == 0 ) ||      /* must use /Zp option  */
        ( dShrLock.uchTermAdr==uchTermAdr )  ) { 
        if (uchTermAdr != 0) {
			dShrLock.fbTermLock |= SHR_LOCK;
			dShrLock.uchTermAdr = uchTermAdr;

            memset(&ShrInf[uchTermAdr], '\0', sizeof(SHRHEADERINF));
			// clear previous data  for skiping previous data check */
			// if not clear this area, the data (that has same VLI, same TermAdr
			// as previous data) is skipped by ShrPrevDataCheck(). Because Consec. number
			// is not set to data header when Sup&&Prg mode operation. */

			sRet = SHR_TIMERON;                      /* make client_module start timer */
        }
		else {
			sRet = SHR_FAIL;
		}
    }

    PifReleaseSem(usShrSemStubFunc);

	if (sRet == SHR_FAIL) {
        PifLog(MODULE_SHR, LOG_ERROR_SHR_TERMADR);
	}

    return (sRet);
}


/*
*===========================================================================
** Format  : SHORT   ShrTermUnLock(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
** Synopsis: This function clear lock memory
*===========================================================================
*/
SHORT   ShrTermUnLock(VOID)
{
	BOOL  bShrTermLocked = FALSE;

    PifRequestSem(usShrSemStubFunc);

	bShrTermLocked = (dShrLock.fbTermLock & SHR_LOCK);
	bShrTermLocked = bShrTermLocked || (dShrLock.uchTermAdr != 0);

    memset(&dShrLock, 0, sizeof(dShrLock));

    PifReleaseSem(usShrSemStubFunc);

	return SHR_PERFORM;
}

/*
*===========================================================================
** Format  : SHORT    ShrPrint(VOID *pData);
*               
*    Input : pData              -print data
*   Output : SHR_PERFORM        -function success (END_FRAME data)
*            SHR_TIMERON        -function success (FIRST or CONTINUE_FRAME data)
*                                                 (when locked by ShrTermLock())
*            SHR_BUSY           -function fail    (can not write to spool)
*            SHR_FAIL           -function fail    (wrong data received)
*    InOut : none
** Return  : none
*
** Synopsis: This function writes pData to spool buffer
*===========================================================================
*/
SHORT    ShrPrint(VOID *pData)
{
    SHRHEADERINF   *pInf;
    SHORT          sRet;
    USHORT         usStart=0;

    pInf     = pData;

    if ( pInf->uchTermAdr > SHR_MAX_TERM ) {
        PifLog(MODULE_SHR, LOG_ERROR_SHR_CODE_01);
        PifLog(MODULE_SHR, FAULT_ERROR_BAD_LENGTH);
        return (SHR_PERFORM);
    }

    if( pInf->usILI == 0 ) {
        PifLog(MODULE_SHR, LOG_ERROR_SHR_CODE_02);
        PifLog(MODULE_SHR, FAULT_ERROR_BAD_LENGTH);
        return (SHR_PERFORM);
    }

    PifRequestSem(usShrSemStubFunc);

    /*
			if the data in the request is the same as the previous request
			then lets look to see if things are going fine or if we have
			a printer error.  One of the reasons why data in the request is
			the same as the previous request is because we are in a retry
			loop possible because of a problem encountered in the previous
			request.
	 */
    if (SHR_SAME == ShrPrevDataCheck(pInf, pInf->uchTermAdr)) {
		ShrPolling ();
		if ( (auchShrPrevLock[pInf->uchTermAdr] & SHR_LOCK)
			 || !(pInf->fbFrame & SHR_END_FRAME) ) {
			sRet = SHR_TIMERON;
		}
		else if (fbShrSSFFull & SHR_PRINTER_ERR) {
			sRet = SHR_BUFFER_PRINTER_ERR;
		}
		else {
			sRet = SHR_PERFORM;
		}
        PifReleaseSem(usShrSemStubFunc);
        return (sRet);
    }

    if( SHR_BUSY == ShrLockCheck(pInf->uchTermAdr) ) {
                                                /* other terminal is locked? */
        PifReleaseSem(usShrSemStubFunc);
        return (SHR_BUSY);
    }

    if ( (sRet = ShrFrameCheck(pInf, pInf->uchTermAdr)) < 0 ) {
                                                /* frame name, cons no, seq no check */
        PifReleaseSem(usShrSemStubFunc);
        PifLog(MODULE_SHR, LOG_ERROR_SHR_FRAMECHK1);
        return (sRet);
    }

    if ( SHR_BUFFER_FULL == ShrWriteSpool(pInf)) {
                                                /* write to spool file   */
        PifReleaseSem(usShrSemStubFunc);        /* if return = SHR_BUSY, */
        PifLog(MODULE_SHR, LOG_ERROR_SHR_CODE_03);  /* The data exceeded the spool file size */
        PifLog(MODULE_SHR, FAULT_ERROR_BAD_LENGTH);
        return (SHR_BUFFER_FULL);               /* cannot write to file. */
    }

    ShrLock(pInf->uchTermAdr);                  /* terminal lock */

    memcpy(&ShrInf[pInf->uchTermAdr], pInf, sizeof(SHRHEADERINF));
                                                /* copy data header for next data check */

    /* -- move write pointer & file full flag -- */
    PifRequestSem(usShrSemRWDisk);
    if ( (usShrWriteP == usShrReadP) && !(fbShrSSFFull & SHR_SSF_FULL) ) {
        usStart = 1;
    } else {
        usStart = 0;
    }
    if (fbShrSSFFull & SHR_TEMP_SSF_FULL) { 
        fbShrSSFFull |= SHR_SSF_FULL;       /* set file full flag */
    }

    fbShrSSFFull &=  ~SHR_TEMP_SSF_FULL;    /* reset file full flag */

    if( usShrWriteP == usShrCurWPoint ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_OPEN);
    }

    usShrWriteP = usShrCurWPoint;           /* increment write pointer */

    if (usStart) {
        PifReleaseSem(usShrSemStartSPP);    /* release sem. for start print thread */
    }

    PifReleaseSem(usShrSemRWDisk);          /* PifReleaseSem(usShrSemRWDisk) must be not call  */
                                            /* just behind PifReleaseSem(usShrSemStartSPP). */

	// If this is the last frame of the print then we will
	// clear our lock on the shared printer.
    if (pInf->fbFrame & SHR_END_FRAME) {
        /* unlock terminal except locked by ShrTermLock() */
        if (SHR_BUSY == ShrUnLock(pInf->uchTermAdr) ) {
			PifLog(MODULE_SHR, LOG_ERROR_SHR_UNLOCKFAILED);
		}
    }

	ShrPolling ();
    sRet = ShrMakeRetVal(pInf);

    PifReleaseSem(usShrSemStubFunc);

    return (sRet);
}

/*
*===========================================================================
** Format  : SHORT   ShrLockCheck(UCHAR uchTermAdr)
*               
*    Input : uchTermAdr             -terminal unique address
*   Output : SHR_PERFORM            -terminal can continue to transaciton
*            SHR_BUSY               -other terminal is locked
*    InOut : none
** Return  : none
*
** Synopsis: This function checks that terminal is able to entry or not.
*===========================================================================
*/
SHORT   ShrLockCheck(UCHAR uchTermAdr)
{
    if ((dShrLock.uchTermAdr == 0) || (dShrLock.uchTermAdr == uchTermAdr)) {

        return (SHR_PERFORM);
    }

    return (SHR_BUSY);
}

/*
*===========================================================================
** Format  : SHORT   ShrLock(UCHAR uchTermAdr)
*               
*    Input : uchTermAdr             -terminal unique address
*   Output : SHR_PERFORM            -terminal can continue to transaciton
*            SHR_BUSY               -other terminal is locked
*    InOut : none
** Return  : none
*
** Synopsis: This function locks specified terminal.
*===========================================================================
*/
SHORT   ShrLock(UCHAR uchTermAdr)
{
    if (dShrLock.uchTermAdr != 0 && dShrLock.uchTermAdr != uchTermAdr) {
        return (SHR_BUSY);
    }

    dShrLock.uchTermAdr = uchTermAdr;

    return (SHR_PERFORM);
}


/*
*===========================================================================
** Format  : SHORT   ShrUnLock(UCHAR uchTermAdr)
*               
*    Input : uchTermAdr             -terminal unique address
*   Output : SHR_PERFORM            -terminal can continue to transaciton
*            SHR_BUSY               -locked by ShrTermLock()
*    InOut : none
** Return  : none
*
** Synopsis: This function clear lock memory.
*===========================================================================
*/
SHORT   ShrUnLock(UCHAR uchTermAdr)
{
    if ((dShrLock.fbTermLock & SHR_LOCK) && dShrLock.uchTermAdr != 0 && dShrLock.uchTermAdr != uchTermAdr) {
        return (SHR_BUSY);
    }

    memset(&dShrLock, 0, sizeof(dShrLock));
    return (SHR_PERFORM);
}



/*
*===========================================================================
** Format  : SHORT    ShrPrevDataCheck(SHRHEADERINF   *pInf, UCHAR uchTermNo);
*               
*    Input : pInf               -print data header pointer
*            uchTermNo          -Terminal address
*   Output : SHR_PERFORM        -data is not same as previous data
*            SHR_SAME           -data is same as previous data
*    InOut : none
** Return  : none
*
** Synopsis: ShrPrint() is stub function, so there are possibility that
*            ShrPrint receive same data.
*            This function check  data is same as previous data or not.
*===========================================================================
*/
SHORT    ShrPrevDataCheck(SHRHEADERINF   *pInf, UCHAR uchTermNo)
{
    USHORT  i;
    UCHAR  *puchInf;
    USHORT  usAdd;                              /* USHORT for over flow */
    

    /* -- data set ? -- */
    puchInf = (UCHAR *)&(ShrInf[uchTermNo]);
    for (i=0; i < sizeof(SHRHEADERINF); i++) {
         if( (usAdd = *(puchInf + i)) != 0) {
            break;
        }
    }
    if (usAdd == 0) {
        return (SHR_PERFORM);
    }

    /* -- compare data -- */
    if ( 0 == memcmp(pInf, &ShrInf[uchTermNo], sizeof(SHRHEADERINF)) ) {
        return (SHR_SAME);
    }
    return (SHR_PERFORM);
}

/*
*===========================================================================
** Format  : SHORT    ShrMakeRetVal(SHRHEADERINF   *pInf);
*               
*    Input : pInf               -print data header pointer
*   Output : SHR_PERFORM        
*            SHR_TIMERON        
*    InOut : none
** Return  : none
*
** Synopsis: make return value of ShrPrint()
*===========================================================================
*/
SHORT    ShrMakeRetVal(SHRHEADERINF   *pInf)
{
    if ( (dShrLock.fbTermLock & SHR_LOCK) || !(pInf->fbFrame & SHR_END_FRAME) ) {
        return (SHR_TIMERON);
    }

	if (fbShrSSFFull & SHR_SSF_FULL) {
		return (SHR_BUFFER_FULL);
	}

	if (fbShrSSFFull & SHR_PRINTER_ERR) {
		return (SHR_BUFFER_PRINTER_ERR);
	}

    return (SHR_PERFORM);                       
}


/*
*===========================================================================
** Format  : SHORT    ShrFrameCheck(SHRHEADERINF   *pInf, UCHAR uchTermNo);
*               
*    Input : pInf               -print data header pointer
*            uchTermNo          -Terminal address
*   Output : SHR_PERFORM        -be able to continue transaction 
*            SHR_BUSY           -during other terminal transaction
*            SHR_FAIL           -data wrong
*    InOut : none
** Return  : none
*
** Synopsis: This function checks data frame name, cons. no. and seq. no.
*===========================================================================
*/
SHORT    ShrFrameCheck(SHRHEADERINF   *pInf, UCHAR uchTermNo)
{

    if (pInf->fbFrame & SHR_FIRST_FRAME) {      /* First data ? */

        /* -- set lock flag for previous data check -- */
        if ( dShrLock.fbTermLock & SHR_LOCK ) {
            auchShrPrevLock[uchTermNo] |= SHR_LOCK;
        } else {
            auchShrPrevLock[uchTermNo] &= ~SHR_LOCK;
        }

        
        PifRequestSem(usShrSemRWDisk);
        if (usShrCurWPoint != usShrWriteP) {
            PifReleaseSem(usShrSemRWDisk);

            PifLog(MODULE_SHR, LOG_ERROR_SHR_DELETE);
                                                /* write error log */
            ShrRestoreWrtP();                   /* restore write pointer */
        } else {
            PifReleaseSem(usShrSemRWDisk);
        }

    } else {

    /*  if (dShrLock.fbTermLock & SHR_LOCK) {       */
    /*         -Not check consecutive number        */
    /*         [for report(sup, prog)]              */
    /*                                              */
    /* frame from sup, prog is always START_FRAME,  */
    /*   so this transaction is not neccessary      */
    /*                                              */
    /*  }                                           */

        /* -- data lost or irregural data -- */
        if ( pInf->usConsNo != ShrInf[uchTermNo].usConsNo ) {
            ShrDataLost(uchTermNo);              /* clear lock memory */
            return (SHR_FAIL);                  
        }
        if ( pInf->uchSeqNo != ShrInf[uchTermNo].uchSeqNo + (UCHAR)1 ) {

            if( !( (pInf->uchSeqNo == 0x00) &&
                   (ShrInf[uchTermNo].uchSeqNo == 0xFF) ) ) {
                ShrDataLost(uchTermNo);          /* clear lock memory */
                return (SHR_FAIL);
            }
        }
    }

    if (dShrLock.fbTermLock & SHR_LOCK) {       /* when sup, prog mode */
        if (pInf->uchSeqNo != ShrInf[uchTermNo].uchSeqNo + (UCHAR)1) {

            if( !( (pInf->uchSeqNo == 0x00) &&
                   (ShrInf[uchTermNo].uchSeqNo == 0xFF) ) ) {
                ShrDataLost(uchTermNo);          /* clear lock memory */
                return (SHR_FAIL);
            }
        }
    }

    return (SHR_PERFORM);

}


/*
*===========================================================================
** Format  : VOID    ShrDataLost(UCHAR uchTermNo)
*               
*    Input : uchTermNo         terminal number for this shared printer  #
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This fuction is called when data is lost.
*            clear lock memory and write error log.
*===========================================================================
*/
VOID    ShrDataLost(UCHAR uchTermNo)
{

    /* -- write log -- */
    PifLog(MODULE_SHR, LOG_ERROR_SHR_LOST);

    /* -- clear previous data -- */
    memset(&dShrLock, '\0', sizeof(dShrLock));
                                        /* cancel terminal lock */
    auchShrPrevLock[uchTermNo] = 0;      /* clear previous data's lock memory */
    memset(&ShrInf[uchTermNo], '\0', sizeof(SHRHEADERINF));
                                        /* clear previous data */

    PmgFont(PMG_PRT_RECEIPT, PRT_FONT_A);       /* set normal font for cancel A/C#70, A/C#100 */

}

/*
*===========================================================================
** Format  : SHORT    ShrWriteSpool(SHRHEADERINF   *pInf);
*               
*    Input : pInf               -print data header pointer
*   Output : SHR_PERFORM        -success to write
*            SHR_BUFFER_FULL    -cannot write to spool file
*    InOut : none
** Return  : none
*
** Synopsis: This fuction write data to spool file.
*===========================================================================
*/
SHORT    ShrWriteSpool(SHRHEADERINF   *pInf)
{

    PifRequestSem(usShrSemRWDisk);              /* request sem. for access to spool file */

    /* -- case of file full -- */
    if ( (fbShrSSFFull & SHR_TEMP_SSF_FULL) || (fbShrSSFFull & SHR_SSF_FULL) ) {

        if ( !(usShrReadP - usShrCurWPoint > pInf->usILI) ) {
            PifReleaseSem(usShrSemRWDisk);      /* release sem. for access disk */
            return (SHR_BUFFER_FULL);
        }

    /* -- case of not file full -- */
    } else {

        if ( usShrCurWPoint > SHR_SPOOL_SIZE ) {
            PifAbort(MODULE_SHR, FAULT_ERROR_BAD_LENGTH);
        }

        if ( !( (USHORT)(SHR_SPOOL_SIZE-usShrCurWPoint) > pInf->usILI) ) {

            if (usShrReadP - SHR_FILE_TOP_OFF > pInf->usILI) {
                usShrFileEnd = usShrCurWPoint;      /* set file end offset */
                fbShrSSFFull |= SHR_TEMP_SSF_FULL;  /* set file full flag */
                usShrCurWPoint = SHR_FILE_TOP_OFF;  /* move write point to file top */
            } else {
                PifReleaseSem(usShrSemRWDisk);      /* release sem. for access disk */
                return (SHR_BUFFER_FULL);
            }
        }
    }

    ShrWriteFile(pInf, pInf->usILI);    /* write data to spool file */
                                        /* increment write pointer */
    PifReleaseSem(usShrSemRWDisk);      /* release sem. for access disk */
    return (SHR_PERFORM);
}


/*
*===========================================================================
** Format  : VOID    ShrWriteFile(VOID *pData, USHORT usDataLen);
*               
*    Input : pData              -data pointer
*            usDataLen          -data length
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This fuction write data to spool file. and increment write pointer
*===========================================================================
*/
VOID    ShrWriteFile(VOID *pData, USHORT usDataLen)
{
    ULONG  ulDumy;

    if( PifSeekFile(hsShrSpool, usShrCurWPoint, &ulDumy) < 0 ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_SEEK);
    }

    if( (ULONG)usShrCurWPoint != ulDumy ) {
        PifAbort(MODULE_SHR, FAULT_ERROR_FILE_SEEK);
    }

    PifWriteFile(hsShrSpool, pData, usDataLen);
                                                /* write to file */
    usShrCurWPoint += usDataLen;                /* increment write pointer */
}

/************************* Add R3.0 ****************************/
/*
*===========================================================================
** Format  : SHORT   ShrPolling(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : status of the shared printer
** Synopsis: This function is polling from K/P
*===========================================================================
*/
SHORT   ShrPolling(VOID)
{
	USHORT  fbPrtStatus = 0;                /* printer status */
	
	PmgGetStatusOnly(PMG_PRT_RECEIPT, &fbPrtStatus);

	if ( fbPrtStatus & PRT_STAT_POWER_OFF) {
		fbShrSSFFull |= SHR_PRINTER_ERR;
	}
	else {
		fbShrSSFFull &= ~SHR_PRINTER_ERR;
	}

	if (fbShrSSFFull & SHR_SSF_FULL) {
		return (SHR_BUFFER_FULL);
	}

	if (fbShrSSFFull & SHR_PRINTER_ERR) {
		return (SHR_BUFFER_PRINTER_ERR);
	}

    return SHR_PERFORM;

}
/************************* End Add R3.0 **************************/

/* ---------- End of Shr.c ---------- */
