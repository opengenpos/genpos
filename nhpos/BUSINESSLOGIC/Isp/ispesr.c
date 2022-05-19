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
* Title       : ISP Server module, Execute Special Request Source File                        
* Category    : ISP Server module, US Hospitality Model
* Program Name: ISPESR.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*   IspESRCheckTerminalStatus();  C  Check terminal status 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-22-92:00.00.01:H.Yamaguchi: initial                                   
* Jun-05-93:00.00.01:H.Yamaguchi: Modified Enhance
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
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <nb.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <storage.h>
#include    <isp.h>
#include    <ej.h>
#include    "ispcom.h"
#include    <appllog.h>

/*
*===========================================================================
** Synopsis:    VOID    IspESRCheckTerminalStatus(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:  This function checks current system conditions.
*===========================================================================
*/
VOID    IspESRCheckTerminalStatus(VOID)
{
    SHORT       sError;
    
    if (ISP_SUCCESS != IspResp.sError) {        /* Receive not Success ? */
        SHORT sIspCleanUpEJ = IspCleanUpEJ();   /* Clean-Up E/J Status,   V3.3 */
        EJUnLock(EJLOCKFG_ISP);                 /* Unlock ISP Reset Flag, V3.3 */
		if (ISP_ST_PASSWORD != usIspState) {       /*** at not PASSWARD State ***/
			char  xBuff[128];
			sprintf (xBuff, "==NOTE: IspESRCheckTerminalStatus() IspCleanUpEJ %d - usIspState %d sError %d", sIspCleanUpEJ, usIspState, IspResp.sError);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
    }

    if ( 0 == (fsIspCleanUP & ISP_WORKS_ASMASTER) ) {  
        return;                                 /* if work as satellite */
    }

    if (ISP_IAM_SATELLITE) {
        return;                                 /* if I am satellite, then return */
    }

    if (ISP_ST_NORMAL == usIspState) {          /* At NORMAL State */

        sError = IspCheckResponse();            /* Get current status  */

        if ( ISP_ERR_NOT_MASTER == sError ) {
            
            IspCleanUpSystem();                 /* Clean-up if need  */
            IspCleanUpLockFun();                /* Clean-up if need  */
            IspChangeStatus(ISP_ST_PASSWORD);   /* Change state to PASSWORD */

        }  else if ( ISP_ERR_DUR_BACKUP == sError ) {
            /* Current is Blank */
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    IspCleanUpEJ(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
** Return:      none.
*
** Description:  This function clean-up E/J status. V3.3
*===========================================================================
*/
#if defined(IspCleanUpEJ)
SHORT    IspCleanUpEJ_Debug (char *aszFilePath, int nLineNo)
{
	SHORT  sRetIndic = 0;

    if (usIspEJState & ISP_ST_EJ) {
		int iLen = 0;
		char  xBuffer[256];

		iLen = strlen (aszFilePath);
		if (iLen > 30) {
			iLen = iLen - 30;
		}
		else {
			iLen = 0;
		}

		sprintf (xBuffer, "==NOTE: IspCleanUpEJ_Debug() usIspEJState = 0x%x, usIspState %d, File %s, lineno = %d", usIspEJState, usIspState, aszFilePath + iLen, nLineNo);
		NHPOS_NONASSERT_TEXT(xBuffer);

        EJ_RelSemIsp();
		sRetIndic = 1;    // indicate that we have release the EJ semaphore.
    }

	return sRetIndic;
}
#else
SHORT    IspCleanUpEJ(VOID)
{
	SHORT  sRetIndic = 0;

    if (usIspEJState & ISP_ST_EJ) {
        EJ_RelSemIsp();
		sRetIndic = 1;    // indicate that we have release the EJ semaphore.
    }

	return sRetIndic;
}
#endif

/*===== END OF SOURCE =====*/

