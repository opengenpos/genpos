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
* Title       : SERVER module, Error Response Handling Source File
* Category    : SERVER module, US Hospitality Model
* Program Name: SERVESR.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           SerERHTimeOut();     C  Time out error handling
*           SerERHPowerFail();   C  Power Fail error handling
*           SerERHOther();          Other error handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Sep-29-92:H.Nakashima: Wait notice B/D recovery at power up                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
*          :           :                                    
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
#include    <csstbfcc.h>
#include    <csserm.h>
#include    <nb.h>
#include    "servmin.h"

/*
*===========================================================================
** Synopsis:    VOID    SerERHTimeOut(VOID);
*
** Return:      none.
*
** Description: This function executes handling at time out error.
*===========================================================================
*/
VOID    SerERHTimeOut(VOID)
{
    if (SER_IAM_SATELLITE) {                /* Satellite  */
        return;                             /* No action  */
    } 

    if (SER_STINQUIRY == usSerStatus) {         /*** at INQUIRY state ***/
		NHPOS_NONASSERT_NOTE("==TEMP", "==TEMP SerERHTimeOut() SER_STINQUIRY == usSerStatus");
        if (SER_IAM_MASTER) {                   /* on the Master Terminal */
            if (++SerResp.usTimeOutCo > SER_MAX_TIMEOUT) {
                SstSetFlag(SER_SPESTS_INQTIMEOUT);
		NHPOS_NONASSERT_NOTE("==TEMP", "==TEMP     SerERHTimeOut() S erSendInquiry()");
                SerSendInquiry();
                SerResp.usTimeOutCo = 0;
            }
        } else {                                /* on the Back Up Master */
			USHORT  fsComStatus = SerComReadStatus();

            if (0 == (fsComStatus & SER_COMSTS_M_OFFLINE)) {
                if (0 == SerResp.usTimeOutCo) {
                    SerSendInqReqInq();
                }
                if (++SerResp.usTimeOutCo > SER_MAX_TIMEOUT) {
                    SerResp.usTimeOutCo = 0;
                }
            } else {
                SstSetFlag(SER_SPESTS_INQTIMEOUT);
                SerResp.usTimeOutCo = 0;
            }
        }
    }
}

/*
*===========================================================================
** Synopsis:    VOID    SerERHPowerFail(VOID);
*
** Return:      none.
*
** Description: This function executes handling at power fail error.
*===========================================================================
*/
VOID    SerERHPowerFail(VOID)
{
	SHORT  sStatus;

    SerNetClose();
    sStatus = SerNetOpen();      // returns handle provided by PifNetOpen();
	NHPOS_ASSERT(sStatus >= 0);

    if (SER_IAM_SATELLITE) {                /* If Satellite, then NORMAL  */
        SerChangeStatus(SER_STNORMAL);      /* normal state */
        NbWaitForPowerUp();                 /* wait Notice B/D recovery */
        return;
    } 

    if (SerNetConfig.fchStatus & SER_NET_BACKUP) {
        SerChangeStatus(SER_STINQUIRY);     /* inquiry state */
        NbWaitForPowerUp();                 /* wait Notice B/D recovery */
        if (SER_IAM_MASTER) {               /* master ? */
            SerSendInquiry();               /* send BM the inquiry */
        }
    } else {
        SerChangeStatus(SER_STNORMAL);      /* normal state */
        NbWaitForPowerUp();                 /* wait Notice B/D recovery */
        SstChangeInqStat(SER_INQ_M_UPDATE); /* decide new status */
        SerSendNtbStart();                  /* start Notice Board */
    }
}
        
/*
*===========================================================================
** Synopsis:    VOID    SerERHOther(VOID);
*
** Return:      none.
*
** Description: This function executes handling at other error.
*===========================================================================
*/
VOID    SerERHOther(VOID)
{
}

/*===== END OF SOURCE =====*/


