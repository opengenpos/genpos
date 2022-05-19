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
* Title       : ISP Server module, Main Functions Source File                        
* Category    : ISP Server module, US Hospitality Model
* Program Name: ISPMAIN.C                                            
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       IspMain();          : Isp server start up (main loop)
*
*        IspInitialize();   : Initialize
*        IspRevRequest();   : Receive request
*        IspExeSpeReq();    : Exec. special request
*        IspRevMesHand();   : Response message handling
*        IspErrResHand();   : Error handling
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
#include    <storage.h>
#include    <isp.h>
#include    "ispcom.h"
#include    <appllog.h>
#include    <appllog.h>
#include    "uie.h"
#include    "pifmain.h"

/*
*===========================================================================
** Synopsis:    VOID    IspMain(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: ISP Server Module starts up functions (Main Loop).
*===========================================================================
*/
VOID THREADENTRY IspMain(VOID)
{
    USHORT  usForEver = 1;

    IspInitialize();                            /* ISP server Initialize */
    IspTimerStart();                            /* Start timer           */
    while(usForEver) {
        IspRevRequest();                        /* Receive Request */ 
        IspExeSpeReq();                         /* Exec. special request */
        if (ISP_SUCCESS == IspResp.sError) {    /* Receive Success ? */
            IspRevMesHand();                    /* Receive msg. handling */
            IspTimerStart();                    /* Start timer           */
        } else {                                /* Receive Error ? */
            IspErrResHand();                    /* Error Handling */
        }
    }
    PifEndThread();                             /* Stop supporting ISP function */
}

/*
*===========================================================================
** Synopsis:    VOID    IspInitialize(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: Server Module Initializes functions.
*===========================================================================
*/
VOID    IspInitialize(VOID)
{
    IspNetOpen();                       /* Open Net work */
    IspChangeStatus(ISP_ST_PASSWORD);   /* Change state to PASSWORD  */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRevRequest(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function requests to receive a request from ISP.
*===========================================================================
*/
SHORT   IspRevRequest(VOID)
{
    IspNetReceive();                     /* Net receive                */
    return IspCheckNetRcvData();         /* Check received data length */
}

/*
*===========================================================================
** Synopsis:    VOID    IspExeSpeReq(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function checks an executive special request.
*===========================================================================
*/
VOID   IspExeSpeReq(VOID)
{
    IspESRCheckTerminalStatus();       /* Check Terminal status  */
}

/*
*===========================================================================
** Synopsis:    VOID    IspRevMesHand(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description: This function executes a receive message handling.
*===========================================================================
*/
VOID    IspRevMesHand(VOID)
{
    switch(usIspState) {

    case  ISP_ST_NORMAL :              /* Normal State */
        IspRMHNormal();
        break;

    case  ISP_ST_MULTI_SEND :          /* Multi sending State */
        IspRMHMulSnd();
        break;

    case  ISP_ST_MULTI_RECV :          /* Multi receiving State */
        IspRMHMulRcv();
        break;

    case  ISP_ST_PASSWORD :            /* Exchange password State */
        IspRMHPassWord();
        break;

    default:                           /* not used */
		PifLog(MODULE_ISP_LOG, LOG_ERROR_ISP_RCVMSGHND_INV_ST);
        break;
    }

}

/*
*===========================================================================
** Synopsis:    VOID    IspErrResHand(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      none.
*
** Description:   This function executes an error response handling.
*===========================================================================
*/
VOID    IspErrResHand(VOID)
{
    switch(IspResp.sError) {
    case  ISP_ERR_TIME_OUT :            /* Time Out Error */
        IspERHTimeOut();             
        break;

    case  ISP_ERR_POWER_DOWN :          /* Power Fail */
        IspERHPowerFail();
        break;

    default:                            /* Other Error */
        IspERHOther();                  /* ERR_NETWORK or ERR_INVALID */
        break;
    }
}

/*===== END OF SOURCE =====*/

