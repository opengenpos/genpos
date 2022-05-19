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
* Title       : Multiline Display Transaction Open                 
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDOPEN.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemTransOpen() : display items specified class "CLASS_ITEMPRINT"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
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

#if defined(POSSIBLE_DEAD_CODE)

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- 2170 local------**/
#include	<tchar.h>
#include<ecr.h>
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h> 
#include<para.h>
#include<prt.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

extern CONST UCHAR FARCONST  aszMldSignIn[];

/*
*===========================================================================
** Format  : VOID   MldItemTransOpen(ITEMTRANSOPEN *pItem, USHORT usType
*                                   USHORT usScroll);
*   Input  : ITEMTRANSOPEN    *pItem     -Item Data address
*            USHORT           usType     -First display position
*            USHORT           usScroll   -Destination dispaly
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function displays transaction open operation.
*===========================================================================
*/
SHORT   MldItemTransOpen(ITEMTRANSOPEN  *pItem, USHORT usType, USHORT usScroll)
{
    SHORT sStatus;

    switch (pItem->uchMinorClass) {
        case CLASS_REORDER:                 /* reorder */
        case CLASS_MANUALPB:                /* manual P/B */
            sStatus = MldReorder(pItem, usType, usScroll);
            /* set reorder status for add check of post check */
            switch(usType) {
            case MLD_NEW_ITEMIZE:
            case MLD_TRANS_DISPLAY:
                MldSetSpecialStatus(usScroll, MLD_REORDER2_STATUS);
            }
            break;

        default:
            sStatus = MLD_NOT_DISPLAY;
            break;
    }
    return (sStatus);
}
#endif
/***** End Of Source *****/
