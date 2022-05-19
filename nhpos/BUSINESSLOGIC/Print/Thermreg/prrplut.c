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
* Title       : Print  PLU sales                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRPluT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-11-93 : 00.00.01 :  R.Itoh    :                                    
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print.(mod. PrtPLU())                                   
* Dec-16-99 : 01.00.00 : hrkato     : Saratoga
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
#include <memory.h>
/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include "pmg.h"
#include <para.h>
#include <csstbpar.h>
#include "prtrin.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID    PrtPLU(TRANINFORMATION  *pTran, ITEMSALES *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMSALES        *pItem     -Item Data address
*   Output : none
*    InOut : none
*
** Return  : none
*
** Synopsis: This function prints plu sales
*===========================================================================
*/
VOID PrtPLU(TRANINFORMATION  *pTran, ITEMSALES  *pItem)
{
    /* -- set print portion to static area "fsPrtPrintPort" -- */
    if ((pTran->TranCurQual.auchTotalStatus[4] & CURQUAL_TOTAL_NOCONSOLIDATE_PRINT) ||    /* not consolidation */
        ((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_PRE_UNBUFFER) ||        /* unbuffering print */
        (pItem->fsPrintStatus & (PRT_SINGLE_RECPT | PRT_DOUBLE_RECPT))) {  /* tichet */

        PrtPortion(pItem->fsPrintStatus);
    } else {                                                    /* consolidation */
        PrtPortion2(pItem);
    }

    /* ---- In case of PLU# 9951-9999 -> only K/P print, 50RFC05513 --- */
    if ((_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_LOW, NUM_PLU_LEN) >= 0) &&
        (_tcsncmp(pItem->auchPLUNo, MLD_NO_DISP_PLU_HIGH, NUM_PLU_LEN) <= 0)) {
        if ((pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_2] & PLU_SALES_KP_NO_1_4) != 0 ||
            (pItem->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] & PLU_SALES_KP_NO_5_8) != 0)
		{
            PrtKPItem(pTran, pItem);
            return;
        }

    } else {
        if ( fsPrtStatus & PRT_REQKITCHEN ) {     /* kitchen print */
            PrtKPItem(pTran, pItem);
        }
    }
    
    if ( pItem->fsPrintStatus & PRT_VALIDATION ) { /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) ) {
            if(PRT_SUCCESS != PrtSPVLInit())
			{
				return;
			}
			PmgBeginSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            PrtSPVLHead(pTran);                     /* E-061 corr. '94 4/22 */
            PrtDeptPLU_SP(pTran, pItem);
            PrtSPVLTrail(pTran);
			PmgEndSmallValidation( PMG_PRT_SLIP );//change to allow Small Slips
            return;
        }
    }

    if ( fsPrtPrintPort & PRT_SLIP ) {        /* slip print */
        PrtDeptPLU_SP(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_RECEIPT ) {     /* thermal print */
        PrtDeptPLU_TH(pTran, pItem);
    }

    if ( fsPrtPrintPort & PRT_JOURNAL ) {     /* electric journal */
        PrtDeptPLU_EJ(pTran, pItem);
    }
}

/* --- End of Source File --- */