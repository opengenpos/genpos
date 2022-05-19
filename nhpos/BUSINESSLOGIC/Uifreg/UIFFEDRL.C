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
* Title       : Reg Feed/Release Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIFFEDRL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               UifFeedRelease() : Reg Feed/Release
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-11-92:00.00.01:M.Suzuki   : initial                                   
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
#include <ecr.h>
#include <uie.h>
#include <regstrct.h>
#include <paraequ.h>
#include <uireg.h>
#include "uiregloc.h"

                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
UIMENU FARCONST aChildRegRelease[] = {{UifRelease, CID_RELEASE},{NULL, 0}};
UIMENU FARCONST aChildRegFeed[] = {{UifFeed, CID_FEED},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifFeedRelease(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Feed/Release Module
*===========================================================================
*/
SHORT UifFeedRelease(KEYMSG *pData)
{

    switch (pData->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildRegRelease);      /* open release */
        break;

    case UIM_ACCEPTED:
        if (pData->SEL.usFunc == CID_RELEASE) {     /* from release ? */
            UieNextMenu(aChildRegFeed);     /* open feed */
            break;
        }
        UieAccept();                        /* send accepted status to parent */
        break;

    case UIM_REJECT:
        break;

    default:                                        /* not use */
        return(UifRegDefProc(pData));               /* default process */
    }
    return (SUCCESS);
}

