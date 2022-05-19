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
* Title       : Print  total key                
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PrRMul_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemMulti() : print items specified class "CLASS_ITEMMULTI"
*      PrtDispMulti() : display items specified class "CLASS_ITEMMULTI"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-16-92 : 00.00.01 : K.Nakajima :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDispMulti()                                   
* Oct-28-93 : 02.00.02 : K.You      : del. PrtDispMulti()                                   
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDispMulti() for display on the fly
*           :          :            :                                    
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
/* #include <paraequ.h> */
/* #include <para.h> */
#include "prtrin.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtItemMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multicheck operation.
*===========================================================================
*/
VOID PrtItemMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{

    switch (pItem->uchMinorClass) {

    case CLASS_MULTICHECKPAYMENT:
        PrtMulti(pTran, pItem);
        break;

    case CLASS_MULTIPOSTCHECK:
        PrtMultiPost(pTran, pItem);
        break;

    case CLASS_MULTITOTAL:
        PrtMultiTotal(pTran, pItem);
        break;

    case CLASS_MULTIPOSTTOTAL:
        PrtMultiPostTotal(pTran, pItem);
        break;     

    default:
        break;

    }
}

/*
*===========================================================================
** Format  : VOID   PrtDispMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints multicheck operation.
*===========================================================================
*/
VOID PrtDispMulti(TRANINFORMATION  *pTran, ITEMMULTI *pItem)
{
    switch (pItem->uchMinorClass) {

    case CLASS_MULTICHECKPAYMENT:
    case CLASS_MULTIPOSTCHECK:
        PrtDflMulti(pTran, pItem);
        break;

    case CLASS_MULTITOTAL:
    case CLASS_MULTIPOSTTOTAL:
        PrtDflMultiTotal(pTran, pItem);
        break;

    default:
        break;

    }
}

/***** End Of Source *****/
