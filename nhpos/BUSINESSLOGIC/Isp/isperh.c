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
* Title       : ISP Server module, Error Response Handling Source File
* Category    : ISP Server module, US Hospitality Model
* Program Name: ISPERH.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           IspERHTimeOut();        Time  out   error handling
*           IspERHPowerFail();      Power Fail  error handling
*           IspERHOther();          Other       error handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
*          :        :           :                                    
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

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    SerERHTimeOut(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  This function executes handling at time out error.
*===========================================================================
*/
VOID    IspERHTimeOut(VOID)
{
    SHORT sError;

    if (ISP_ST_PASSWORD != usIspState) {       /*** at not PASSWARD State ***/

        IspCheckGCFUnLock();                   /* if Response = GCF read & lock, then unlock */
        IspResp.usTimeOutCo++;         
        if (IspResp.usTimeOutCo < 10) {  /* Check reach to Max */
            return;
        }
        IspResp.usTimeOutCo = 0 ;              /* Clear counter */
        IspCleanUpSystem();                    /* Clean-up if need  */
        IspCleanUpLockFun();                   /* Clean-up if need  */
        IspChangeStatus(ISP_ST_PASSWORD);      /* Change state to PASSWORD  */
    } else {
        if (0 == (IspNetConfig.fchStatus & ISP_NET_OPEN)) { /* Check net is open or not */
            sError = IspNetChkBoard() ;    /* Check system condition */
            if ( sError == PIF_ERROR_NET_NOT_PROVIDED ) {
                IspAbort(FAULT_BAD_ENVIRONMENT);    /* Abort */
            } 

            IspNetOpen();                           /* Open Net */
            if (0 == (IspNetConfig.fchStatus & ISP_NET_OPEN)) {
                PifSleep(ISP_SLEEP_WAIT);  /* if not open, then Sleep 5 Seconds */
            }

        }

    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspERHPowerFail(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  This function executes handling at power fail error.
*===========================================================================
*/
VOID    IspERHPowerFail(VOID)
{
    IspNetClose();                       /* Close Net work    */
	PifSleep (3000);                     // give the network time to settle down
    IspNetOpen();                        /* Re-open Net work  */

    IspCleanUpSystem();                  /* Execute cleanup function  */
    IspCleanUpLockFun();                 /* Execute unlock  functions */
    IspChangeStatus(ISP_ST_PASSWORD);    /* Change state to PASSWORD  */
}
        
/*
*===========================================================================
** Synopsis:    VOID    IspERHOther(VOID);
*     Input:    nothing 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  This function executes handling at other error.
*===========================================================================
*/
VOID    IspERHOther(VOID)
{
    LONG    lTime;

    if ( ISP_ERR_INVALID_MESSAGE == IspResp.sError) {   /* INVALID error ? */
        lTime = IspTimerRead();             /* Get timer */
        if ( ISP_MAX_RECEIVE_TIME <= lTime ) {
            IspCleanUpSystem();                  /* Clean-up if need  */
            IspCleanUpLockFun();                 /* Clean-up if need  */
            IspChangeStatus(ISP_ST_PASSWORD);    /* Change state to PASSWORD */
			PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_MAX_RECEIVE_TIME);
        }
    } else {            /* Detect network other 3 times error  */

        sIspExeError++;
        if ( 3 < sIspExeError ) {  
            sIspExeError = 0;
            if ( ISP_ST_PASSWORD != usIspState )  {
                IspCleanUpSystem();                  /* Clean-up if need  */
                IspCleanUpLockFun();                 /* Clean-up if need  */
                IspChangeStatus(ISP_ST_PASSWORD);    /* Change state to PASSWORD */
				PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_MAX_EXE_ERROR);
            }
        }
        lTime = IspTimerRead();                 /* Get timer */
        if (ISP_ELAPSED_TIME > lTime ) {
            PifSleep(ISP_SLEEP_WAIT);            /* Sleep 5 seconds   */    
        }
    }
}


/*===== END OF SOURCE =====*/

