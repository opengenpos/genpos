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
* Title       : Multiline Display Item  Header trailer
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDHEAD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldItemPrint() : display items specified class "CLASS_ITEMPRINT"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-23-95 : 03.00.00 :  M.Ozawa   : Initial
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- M S - C --------**/
#include	<tchar.h>
#include<string.h>
/**------- 2170 local------**/
#include<ecr.h>
#include<paraequ.h>
#include<para.h>
#include<regstrct.h>
#include<transact.h>
#include<csstbpar.h>
#include<mld.h>
#include"mldlocal.h"
#include"mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/


/*
*===========================================================================
** Format  : VOID  MldItemPrint(ITEMPRINT *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMPRINT        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays  Header & Trailer.
*===========================================================================
*/
SHORT   MldItemPrint(CONST ITEMPRINT *pItem, USHORT usType, USHORT usScroll)
{

    return (MldDummyScrollDisplay(usType, usScroll));

    pItem = pItem;

}

/****** End of Source ******/
