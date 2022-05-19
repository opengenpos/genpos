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
* Title       : Descriptor control Module                         
* Category    : Notice Board, NCR 2170 US Hospitality Application         
* Program Name: NBDESC.C
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               NbDescriptor()       : Disply descriptor 
*               NbDispWithoutBoard() : Display desc., when without a board 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-01-92:00.00.01:H.Yamaguchi: initial                                   
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
#include <tchar.h>
#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <uie.h>
#include <nb.h>
#include "nbcom.h"

#include "paraequ.h"
#include "para.h"
#include "plu.h"
#include "pararam.h"


/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
                                                                                
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID NbDescriptor(USHORT fsSystemInf)
*     Input:    fsSystemInf
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display the Notice Board synchronization status in the
				indicator region of the display.  This routine is called
				in several places but the most important is the periodic
				call in function NbMain (), the Notice Board thread.  The
				Notice Board thread is a recurring thread which cycles through the
				Notice Board functionality for maintaining the heartbeat
				of messages between the Master Terminal and other terminals
				in the cluster.

				We are interested in three pieces of information for each of
				the terminals, Master Terminal and Backup Master Terminal:
					- is the Terminal up to date, NB_MTUPTODATE or NB_BMUPTODATE on
					- is the Terminal on-line, NB_SETMTONLINE or NB_SETBMONLINE on
					- are we in inquiry mode, NB_INQUIRY on
				If we are in inquiry mode then we are not sure what our status is
				so being in inquiry mode over rides the other settings.

				We will use the following standard for operator notification of status:
					- UIE_DESC_OFF -> on-line, up to date, not in inquiry mode
					- UIE_DESC_BLINK -> in inquiry mode or not on line
					- UIE_DESC_ON -> not up to date

				Basically, if UIE_DESC_BLINK displays, then we are not sure of the
				status of the terminal other than knowing that it was up to date.

				UIE_DESC_ON displays if we know that the terminal is not up to date.
*===========================================================================
*/
VOID  NbDescriptor(USHORT fsSystemInf)
{
    UCHAR    uchCon;

	/*
		if the caller has specified NB_REDISPLAY then we will redisplay
		the last status.  NB_REDISPLAY is a special command and not a status.
		Otherwise, we will assume that fsSystemInf contains the status bit map
		which we are going to display.  We will always keep a copy of the
		status we are displaying in case we have to handle a NB_REDISPLAY request.
	 */
	if(fsSystemInf == NB_REDISPLAY)
	{
		fsSystemInf = usNbPreDesc;
	} else {
		fsSystemInf &= NB_RSTFORDESC ;             /* Reset for descriptor control */
		if (usNbPreDesc)  {                      /* If status is the same as previous, then return */
			if ( !( fsSystemInf & NB_INQUIRY )) {
				if ( usNbPreDesc == fsSystemInf ) {    /* If status is the same as previous, then return */
					return;
				}
			}
		}
	}

    usNbPreDesc = fsSystemInf;

	/*
		Determine the Master Terminal status to display and then display it.
	 */
	uchCon = UIE_DESC_OFF;                         /* Assume Desc should be off */
    if ( fsSystemInf & NB_INQUIRY )
	{
        uchCon = UIE_DESC_BLINK;                   /* If in Inquiry mode, blink desc */
    } else {
		if ((fsSystemInf & NB_SETMTONLINE) == 0)   /* if not on-line, blick desc. */
		{
			uchCon = UIE_DESC_BLINK;               
		}
		if ((fsSystemInf & NB_MTUPTODATE) == 0)    /* if not up to date, turn on desc. */
		{
			uchCon = UIE_DESC_ON;                 
		}
    }
    
    UieDescriptor(UIE_OPER, NB_MASTER_DESC, uchCon);

	/*
		Determine the Backup Master Terminal status to display and then display it.
		If we are not a Backup Master system, then nothing else to do.  Otherwise
		determine the proper setting for the descriptor and display it.
	 */
    uchCon = UIE_DESC_OFF;                           /* Assume Desc should be off */
    if ((NbSysFlag.fsSystemInf & NB_WITHOUTBM) == 0)
	{
		if ( fsSystemInf & NB_INQUIRY )
		{
			uchCon = UIE_DESC_BLINK;                 /* If in Inquiry mode, blink desc */
		} else {
			if ((fsSystemInf & NB_SETBMONLINE) == 0)  /* if not on-line, blick desc. */
			{        
				uchCon = UIE_DESC_BLINK;
			}
			if ((fsSystemInf & NB_BMUPTODATE) == 0)    /* if not up to date, turn on desc. */
			{
				uchCon = UIE_DESC_ON;
			}
		}
    }

    UieDescriptor(UIE_OPER, NB_BMASTER_DESC, uchCon); /* Disply B-Master descriptor */

	//if the delay balance flag is on, we need to turn the descriptor on, 
	//AFTER the master light has been turned either on/off. JHHJ
	if(uchMaintDelayBalanceFlag)
	{
		UieDescriptor(UIE_OPER,NB_DELAY_BALANCE_DESC, UIE_DESC_ON);
	}
}


/*
*===========================================================================
** Synopsis:    VOID NbDispWithoutboard(USHORT fsSystemInf)
*     Input:    fsSystemInf
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: Display the descriptor , when the system has not a comm board.
*===========================================================================
*/
VOID   NbDispWithoutBoard(USHORT fsSystemInf)       /* Display desc. without a board */
{
    SHORT     sRet;

    sRet = NbNetChkBoard(NB_BMASTERUADD);           /* Check Comm. Board */

    if ( sRet == PIF_ERROR_NET_NOT_PROVIDED ) { 

        NbDescriptor(fsSystemInf);                  /* Display the master descriptor */
    }    
}

/*====== End of Source File ======*/
