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
* Title       : Print  total key                
* Category    : Print, NCR 2170 US Hospitarity Application 
* Program Name: PrRTtlT.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      PrtItemTotal() : print items specified class "CLASS_ITEMTOTAL"
*      PrtDispTotal() : display items specified class "CLASS_ITEMTOTAL"
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-10-93 : 01.00.12 : R.Itoh     : initial                                   
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDispTotal()                                   
* Jul-28-93 : 01.00.12 : K.You      : add soft check feature. (mod. PrtItemTotal)                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDispTotal()                                   
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-064 corrected (mod.CLASS_TOTALAUDACT)
* Jan-26-95 : 03.00.00 : M.Ozawa    : add PrtDispTotal()
* Dec-06-99 : 01.00.00 : hrkato     : Saratoga
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
#include <paraequ.h>
/* #include <para.h> */
#include "prtrin.h"
#include "para.h"
#include "csstbpar.h"


/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID   PrtITemTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints totalkey operation.
*===========================================================================
*/
VOID PrtItemTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{

    if (fsPrtStatus & PRT_TAKETOKIT) {  /* take to kitchen status */
        PrtTotal(pTran, pItem);         /* print by MI */
        return;
    }

    switch (pItem->uchMinorClass) {

    case CLASS_TOTAL1:              /* sub total */
    case CLASS_TOTAL2:              /* check total */
    case CLASS_TOTAL9:              /* Saratoga */
    case CLASS_TOTAL2_FS:           /* Saratoga */
        PrtTotal(pTran, pItem);
        break;

    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
	case CLASS_TOTAL10:
	case CLASS_TOTAL11:
	case CLASS_TOTAL12:
	case CLASS_TOTAL13:
	case CLASS_TOTAL14:
	case CLASS_TOTAL15:
	case CLASS_TOTAL16:
	case CLASS_TOTAL17:
	case CLASS_TOTAL18:
	case CLASS_TOTAL19:
	case CLASS_TOTAL20:
        switch (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) {

        case PRT_FINALIZE1:                       /* finalize total */
        case PRT_FINALIZE2:                       
        case PRT_NOFINALIZE:                      /* no finalize total */
            PrtTotal(pTran, pItem);
            break;

        case PRT_SERVICE1:
        case PRT_SERVICE2:                        /* service total */
        case PRT_CASINT1:
        case PRT_CASINT2:                         /* cashier interrupt total, R3.3 */
            if (((pTran->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK ) == CURQUAL_POSTCHECK ) &&
                ( CliParaMDCCheck( MDC_GCNO_ADR, EVEN_MDC_BIT3 ) == 0) &&
                ( CliParaMDCCheck( MDC_SPLIT_GCF_ADR, EVEN_MDC_BIT2 ) == 0)) {

                if (( pItem->fsPrintStatus & PRT_VALIDATION ) || /* split check system validation */
                    ((pItem->auchTotalStatus[2] & CURQUAL_TOTAL_PRINT_BUFFERING) == 0)) { /* split check system journal print */

                    PrtTotal(pTran, pItem);
                    break;
                }
            }

            if (auchPrtSeatCtl[0]) {    /* split check consolidation print */
                if (pItem->aszNumber[0]) {
                    pItem->uchMinorClass += CLASS_SOFTCHK_BASE_TTL;
                    PrtSoftCheckTotal(pTran, pItem);    /* print out with pre-authorization data */
                    pItem->uchMinorClass -= CLASS_SOFTCHK_BASE_TTL;
                } else {
                    PrtTotal(pTran, pItem);
                }
            } else {
                PrtServTotal(pTran, pItem);
            }
            break;

        case PRT_TRAY1:
        case PRT_TRAY2:                           /* tray total */
            PrtTrayTotal(pTran, pItem);
            break;

        default:
            break;
        }
        break;

    case CLASS_PREAUTHTOTAL:
        PrtTotal(pTran, pItem);
        break;

    case CLASS_STUB3:
    case CLASS_STUB4:
    case CLASS_STUB5:
    case CLASS_STUB6:
    case CLASS_STUB7:
    case CLASS_STUB8:
	case CLASS_STUB9:
	case CLASS_STUB10:
	case CLASS_STUB11:
	case CLASS_STUB12:
	case CLASS_STUB13:
	case CLASS_STUB14:
	case CLASS_STUB15:
	case CLASS_STUB16:
	case CLASS_STUB17:
	case CLASS_STUB18:
	case CLASS_STUB19:
	case CLASS_STUB20:
        PrtTotalStub(pTran, pItem);
        break;

    case CLASS_ORDER3:
    case CLASS_ORDER4:
    case CLASS_ORDER5:
    case CLASS_ORDER6:
    case CLASS_ORDER7:
    case CLASS_ORDER8:
	case CLASS_ORDER9:
	case CLASS_ORDER10:
	case CLASS_ORDER11:
	case CLASS_ORDER12:
	case CLASS_ORDER13:
	case CLASS_ORDER14:
	case CLASS_ORDER15:
	case CLASS_ORDER16:
	case CLASS_ORDER17:
	case CLASS_ORDER18:
	case CLASS_ORDER19:
	case CLASS_ORDER20:
        PrtTotalOrder(pTran, pItem);
        break;

    case CLASS_TOTALAUDACT:                         /* E-064 corr. 4/25     */
        PrtTotalAudact(pTran, pItem);
        break;

    case CLASS_TOTALPOSTCHECK:
        PrtServTotalPost(pTran, pItem);
        break;

    case CLASS_STUBPOSTCHECK:
        PrtServTotalStubPost(pTran, pItem);
        break;

    case CLASS_SOFTCHK_TOTAL3:
    case CLASS_SOFTCHK_TOTAL4:
    case CLASS_SOFTCHK_TOTAL5:
    case CLASS_SOFTCHK_TOTAL6:
    case CLASS_SOFTCHK_TOTAL7:
    case CLASS_SOFTCHK_TOTAL8:
    case CLASS_SOFTCHK_TOTAL9:
    case CLASS_SOFTCHK_TOTAL10:
    case CLASS_SOFTCHK_TOTAL11:
    case CLASS_SOFTCHK_TOTAL12:
    case CLASS_SOFTCHK_TOTAL13:
    case CLASS_SOFTCHK_TOTAL14:
    case CLASS_SOFTCHK_TOTAL15:
    case CLASS_SOFTCHK_TOTAL16:
    case CLASS_SOFTCHK_TOTAL17:
    case CLASS_SOFTCHK_TOTAL18:
    case CLASS_SOFTCHK_TOTAL19:
    case CLASS_SOFTCHK_TOTAL20:
        PrtSoftCheckTotal(pTran, pItem);
        break;

    default:
        break;

    }
}



/*
*===========================================================================
** Format  : VOID   PrtDispTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints totalkey operation.
*===========================================================================
*/
VOID PrtDispTotal(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem)
{
    switch (pItem->uchMinorClass) {

    case CLASS_TOTAL1:              /* sub total */
    case CLASS_TOTAL2:              /* check total */
    case CLASS_TOTAL9:              /* Saratoga */
    case CLASS_TOTAL2_FS:           /* Saratoga */
        PrtDflTotal(pTran, pItem);
        break;

    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
    case CLASS_TOTAL10:
    case CLASS_TOTAL11:
    case CLASS_TOTAL12:
    case CLASS_TOTAL13:
    case CLASS_TOTAL14:
    case CLASS_TOTAL15:
    case CLASS_TOTAL16:
    case CLASS_TOTAL17:
    case CLASS_TOTAL18:
    case CLASS_TOTAL19:
	case CLASS_TOTAL20:
        switch (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) {

        case PRT_FINALIZE1:                       /* finalize total */
        case PRT_FINALIZE2:                       
        case PRT_NOFINALIZE:                      /* no finalize total */
            PrtDflTotal(pTran, pItem); 
            break;

        case PRT_SERVICE1:
        case PRT_SERVICE2:                        /* service total */
        case PRT_CASINT1:
        case PRT_CASINT2:                         /* cashier interrupt total, R3.3 */
            PrtDflServTotal(pTran, pItem); 
            break;

        case PRT_TRAY1:
        case PRT_TRAY2:                           /* tray total */
            PrtDflTrayTotal(pTran, pItem);  
            break;

        default:
            break;
        }
        break;

    case CLASS_TOTALAUDACT:
/*        PrtDflTotalAudact(pTran, pItem); */
        break;

    case CLASS_SOFTCHK_TOTAL3:
    case CLASS_SOFTCHK_TOTAL4:
    case CLASS_SOFTCHK_TOTAL5:
    case CLASS_SOFTCHK_TOTAL6:
    case CLASS_SOFTCHK_TOTAL7:
    case CLASS_SOFTCHK_TOTAL8:
    case CLASS_TOTALPOSTCHECK:
    case CLASS_SOFTCHK_TOTAL9:
    case CLASS_SOFTCHK_TOTAL10:
    case CLASS_SOFTCHK_TOTAL11:
    case CLASS_SOFTCHK_TOTAL12:
    case CLASS_SOFTCHK_TOTAL13:
    case CLASS_SOFTCHK_TOTAL14:
    case CLASS_SOFTCHK_TOTAL15:
    case CLASS_SOFTCHK_TOTAL16:
    case CLASS_SOFTCHK_TOTAL17:
    case CLASS_SOFTCHK_TOTAL18:
    case CLASS_SOFTCHK_TOTAL19:
    case CLASS_SOFTCHK_TOTAL20:
        PrtDflServTotalStubPost(pTran, pItem); 
        break;

    /* ---- display forein currency total on KDS R3.1 ---- */
    case CLASS_FOREIGNTOTAL1:
    case CLASS_FOREIGNTOTAL2:
    case CLASS_FOREIGNTOTAL3:
    case CLASS_FOREIGNTOTAL4:
    case CLASS_FOREIGNTOTAL5:
    case CLASS_FOREIGNTOTAL6:
    case CLASS_FOREIGNTOTAL7:
    case CLASS_FOREIGNTOTAL8:
        PrtDflForeignTotal(pTran, pItem);
        break;

    default:
        break;

    }
}

/*
*===========================================================================
** Format  : VOID   PrtDispTotal(TRANINFORMATION  *pTran, ITEMTOTAL *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTOTAL        *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints totalkey operation.
*===========================================================================
*/
USHORT PrtDispTotalForm(TRANINFORMATION  *pTran, ITEMTOTAL  *pItem, TCHAR *puchBuffer)
{
    USHORT usLine = 0;
    
    switch (pItem->uchMinorClass) {

    case CLASS_TOTAL1:              /* sub total */
    case CLASS_TOTAL2:              /* check total */
    case CLASS_TOTAL2_FS:           /* Saratoga */
    case CLASS_TOTAL9:              /* Saratoga */
        usLine = PrtDflTotalForm(pTran, pItem, puchBuffer);
        break;

    case CLASS_TOTAL3:
    case CLASS_TOTAL4:
    case CLASS_TOTAL5:
    case CLASS_TOTAL6:
    case CLASS_TOTAL7:
    case CLASS_TOTAL8:
    case CLASS_TOTAL10:
    case CLASS_TOTAL11:
    case CLASS_TOTAL12:
    case CLASS_TOTAL13:
    case CLASS_TOTAL14:
    case CLASS_TOTAL15:
    case CLASS_TOTAL16:
    case CLASS_TOTAL17:
    case CLASS_TOTAL18:
    case CLASS_TOTAL19:
    case CLASS_TOTAL20:
        switch (pItem->auchTotalStatus[0] / CHECK_TOTAL_TYPE) {

        case PRT_FINALIZE1:                       /* finalize total */
        case PRT_FINALIZE2:                       
        case PRT_NOFINALIZE:                      /* no finalize total */
            usLine = PrtDflTotalForm(pTran, pItem, puchBuffer); 
            break;

        case PRT_SERVICE1:
        case PRT_SERVICE2:                        /* service total */
        case PRT_CASINT1:
        case PRT_CASINT2:                         /* cashier interrupt total, R3.3 */
            usLine = PrtDflServTotalForm(pTran, pItem, puchBuffer); 
            break;

        case PRT_TRAY1:
        case PRT_TRAY2:                           /* tray total */
            usLine = PrtDflTrayTotalForm(pTran, pItem, puchBuffer);
            break;

        default:
            break;
        }
        break;

    case CLASS_TOTALAUDACT:
/*        PrtDflTotalAudact(pTran, pItem); */
        break;

    case CLASS_SOFTCHK_TOTAL3:
    case CLASS_SOFTCHK_TOTAL4:
    case CLASS_SOFTCHK_TOTAL5:
    case CLASS_SOFTCHK_TOTAL6:
    case CLASS_SOFTCHK_TOTAL7:
    case CLASS_SOFTCHK_TOTAL8:
    case CLASS_SOFTCHK_TOTAL9:
    case CLASS_SOFTCHK_TOTAL10:
    case CLASS_SOFTCHK_TOTAL11:
    case CLASS_SOFTCHK_TOTAL12:
    case CLASS_SOFTCHK_TOTAL13:
    case CLASS_SOFTCHK_TOTAL14:
    case CLASS_SOFTCHK_TOTAL15:
    case CLASS_SOFTCHK_TOTAL16:
    case CLASS_SOFTCHK_TOTAL17:
    case CLASS_SOFTCHK_TOTAL18:
    case CLASS_SOFTCHK_TOTAL19:
    case CLASS_SOFTCHK_TOTAL20:
    case CLASS_TOTALPOSTCHECK:
        usLine = PrtDflServTotalStubPostForm(pTran, pItem, puchBuffer);
        break;

    /* ---- display forein currency total on KDS R3.1 ---- */
    case CLASS_FOREIGNTOTAL1:
    case CLASS_FOREIGNTOTAL2:
    case CLASS_FOREIGNTOTAL3:
    case CLASS_FOREIGNTOTAL4:
    case CLASS_FOREIGNTOTAL5:
    case CLASS_FOREIGNTOTAL6:
    case CLASS_FOREIGNTOTAL7:
    case CLASS_FOREIGNTOTAL8:
        usLine = PrtDflForeignTotalForm(pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break;

    }
    
    return usLine;
}

/***** End Of Source *****/