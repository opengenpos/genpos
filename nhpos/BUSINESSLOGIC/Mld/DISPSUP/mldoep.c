/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MLD For Order Entry Prompt Module  ( SUPER MODE )
* Category    : MLD, NCR 2170 US Hospitality Application Program    
* Program Name: MLDOEP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls Order Entry Prompt.
*
*    The provided function names are as follows: 
* 
*        MldParaOEPDisp()  : Display parameter in Order Entry Prompt to MLD.
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-20-95: 03.00.00 : T.Satoh   : initial                                   
* 
*** NCR2171 **
* Aug-26-99: 01.00.00 : M.Teraki  : initial (for Win32)
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
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <csttl.h>
#include <csstbttl.h>
#include <appllog.h>

#include <report.h>
#include <mld.h>
#include <mldsup.h> 

#include "maintram.h" 

/*
*===========================================================================
*   MLD Format of Order Entry Prompt Maintenance Modeule
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    VOID MldParaOEPDisp(PARAOEPTBL *pOep)
*               
*     Input:    PARAOEPTBL *pOep
*                  [ When Display Data about All Address        ]
*                   .uchTblNumber    <= Target Table Number
*                   .uchTblAddr      <= 0
*                  [ When Display Data about Individual Address ]
*                   .uchTblAddr      <= 1 - 10
*                   .uchOepData[0]   <= Target Data
*                              ^^^
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data and display to MLD.
*===========================================================================
*/

VOID MldParaOEPDisp(PARAOEPTBL *pOep)
{
    PARAOEPTBL  OepWork;                            /* For All Address     	*/
    SHORT       sTblAddr;

    /**********************************************/
    /* Display Data about All Address (One Table) */
    /**********************************************/
    if(pOep->uchTblAddr == 0){
        MldScrollClear(MLD_SCROLL_1);               /* Clear Scroll Display */

        /*
         * Read All Data from Table of Target No.
         */
        OepWork.uchTblNumber = pOep->uchTblNumber;  /* Set Target Table No. */
        OepWork.uchTblAddr   = 0;                   /* Indicate ALL ADDRESS */
        ParaOepRead(&OepWork);                      /* Reading Table        */

        /*
         * Output to MLD
         */
        for(sTblAddr=0;sTblAddr<10;sTblAddr++){
            MldPrintf(MLD_SCROLL_1,
                        (USHORT)(MLD_SCROLL_TOP + sTblAddr),
                            &(aszMldAC160[1][0]), sTblAddr + 1,
                                (USHORT)(OepWork.uchOepData[sTblAddr]));
        }
    }

    /*****************************************/
    /* Display Data about Individual Address */
    /*****************************************/
    else{
        MldPrintf(MLD_SCROLL_1,
                    (USHORT)(MLD_SCROLL_TOP + pOep->uchTblAddr - 1),
                        &(aszMldAC160[1][0]), pOep->uchTblAddr,
                            (USHORT)(pOep->uchOepData[0]));
    }

    return;
}
