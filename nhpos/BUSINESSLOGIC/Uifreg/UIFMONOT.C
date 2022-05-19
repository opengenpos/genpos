/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Money declaration
* Category    : User Interface for Reg, NCR 2170 GP 2.0 Application
* Program Name: UIFMONOT.C
* Author      : hkato, Project#1, GTP&S
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* Nov-25-99:01.00.00: hrkato    : Saratoga
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
#include <log.h>
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <uic.h>
#include <regstrct.h>
#include <item.h>
#include <paraequ.h>
#include <uireg.h>
#include <appllog.h>
#include "uiregloc.h"


/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/
/* --- Function Menu of Money declaration-In --- */
UIMENU FARCONST aChildRegMoneyIn[] =   {{UifMoneyIn,    CID_MONEYIN  },
                                        {NULL,0}};

/* --- Function Menu of Money Entry --- */
UIMENU FARCONST aChildRegMoneyTend[] = {{UifMoneyTend,  CID_MONEYTEND   },
                                        {UifMoneyOut,   CID_MONEYOUT    },
                                        {UifEC,         CID_EC          },
                                        {NULL,0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifMoneyDec(KEYMSG *pData)
*     Input:    pData - key message pointer
*     Output:   nothing
*     INOut:    nothing
*
** Return:      SUCCESS
*               LDT_????_ADR    : Lead through message number
*
** Description: Money declaration
*===========================================================================
*/
SHORT UifMoneyDec(KEYMSG *pData)
{
    switch (pData->uchMsg) {

    case UIM_INIT:

        /* --- add money declaration-in function menu --- */
        UieNextMenu(aChildRegMoneyIn);

        /* --- return SUCCESS status --- */
        break;

    /* --- received accept from child --- */
    case UIM_ACCEPTED:

        /* --- if received accept from UifMoneyOut --- */
        if (pData->SEL.usFunc == CID_MONEYOUT) {

            /* --- send accept to parent --- */
            UieAccept();
        }

        /* --- if accept from another child --- */
        else {

            /* --- add money entry function menu --- */
            UieNextMenu(aChildRegMoneyTend);
        }

        /* --- exit, return SUCCESS status --- */
        break;

    /* --- received reject from child --- */
    case UIM_REJECT:

        /* --- exit, return SUCCESS status --- */
        break;


    default:
        return(UifRegDefProc(pData));
    }

    /*--- return SUCCESS --- */
    return (SUCCESS);
}

/* --- End of Source File --- */