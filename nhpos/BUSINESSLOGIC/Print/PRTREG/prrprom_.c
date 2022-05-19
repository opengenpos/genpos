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
* Title       : Print Promotional header                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRProm.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*      PrtPromotion() : print promotional header
*       PrtProm_R()
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-18-92 : 00.00.01 : K.Nakajima :                                    
* Jul-13-93 : 01.00.12 : K.Nakajima : add PrtProm_R()
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- M S - C --------**/
#include	<tchar.h>
#include<string.h>
/**------- 2170 local------**/
#include<ecr.h>
#include"rfl.h"
#include<regstrct.h>
#include<transact.h>
#include<paraequ.h>
#include<para.h>
#include<csstbpar.h>
#include<pmg.h>
#include<prt.h>
#include"prtrin.h"
#include"prrcolm_.h"

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s                      +
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtPromotion(TRANINFORMATION *pTran, ITEMPRINT *pItem);      
*
*   Input  : TRANINFORMATION *pTran    -transaction information
*            ITEMPRINT       *pItem    -item information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This functions prints Promotional header. 
*===========================================================================
*/
VOID PrtPromotion(TRANINFORMATION  *pTran, SHORT fsPrintStatus)
{

    PrtPortion(fsPrintStatus); /* check no-print, compulsory print */
                                      /* and write status to "fsPrtPrintPort" */


    if( (pTran->TranCurQual.fsCurStatus & CURQUAL_MULTICHECK) ) {
        return;    
    }

    if ( !(pTran->TranCurQual.flPrintStatus & CURQUAL_NOT_PRINT_PROM) &&
          (fsPrtPrintPort & PRT_RECEIPT) ) {

        PrtProm_R();
    }

}   

/*
*===========================================================================
** Format  : VOID  PrtProm_R(VOID);      
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This functions prints Promotional header. (receipt) 
*===========================================================================
*/
VOID  PrtProm_R(VOID)
{
    USHORT i;
    PARAPROMOTION  PM;


    PM.uchMajorClass = CLASS_PARAPROMOTION;

    for ( i = PRM_LINE1_ADR; i <= PRM_LINE3_ADR; i++) {

        PM.uchAddress = (UCHAR)i;
        tcharnset(PM.aszProSales, PRT_SPACE, TCHARSIZEOF(PM.aszProSales));
        CliParaRead(&PM);

        if (PM.aszProSales[usPrtRJColumn - 1] == PRT_DOUBLE) {
            PM.aszProSales[usPrtRJColumn - 1] = PRT_SPACE;
        }

        PmgPrint(PMG_PRT_RECEIPT, PM.aszProSales, usPrtRJColumn);
    }
}
