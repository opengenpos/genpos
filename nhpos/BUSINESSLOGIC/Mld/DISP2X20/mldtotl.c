/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display total key (SubTotal, no finalize total, finalize total)               
* Category    : Multiline, NCR 2170 US Hospitarity Application 
* Program Name: MLDTOTL.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               MldTotal() : prints total key (Total)
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-16-95 : 03.01.00 : M.Ozawa    : Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

#if defined(POSSIBLE_DEAD_CODE)

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
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h> 
#include <para.h>
#include <csstbpar.h>
#include <uie.h>
#include <fsc.h>
#include <rfl.h>
#include <display.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll);
*
*   Input  : ITEMTOTAL        *pItem     -Item Data address
*            USHORT           usType     -First Display Position
*            USHORT           usScroll   -Destination Scroll Display
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays total key
*===========================================================================
*/
SHORT MldTotal(ITEMTOTAL *pItem, USHORT usType, USHORT usScroll)
{
    UCHAR           uchSystem = 0;
    REGDISPMSG      DisplayData;
    PARATRANSMNEMO  TransMnemoRcvBuff;
    /* TRANMODEQUAL    WorkMode; *//* ### Del (2171 for Win32) V1.0 */
    TRANCURQUAL     WorkCur;
    TRANGCFQUAL     *WorkGCF;

    memset(&DisplayData, 0, sizeof(REGDISPMSG));
    DisplayData.uchMajorClass = CLASS_UIFREGTOTAL;
    DisplayData.uchMinorClass = CLASS_UITOTAL1;

    switch (pItem->uchMinorClass) {
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
        case MLD_SERVICE1:
        case MLD_SERVICE2:                        /* service total */
            DisplayData.lAmount = pItem->lService;
            break;
        }
        break;
    default:
        DisplayData.lAmount = pItem->lMI;
    }
 
    TransMnemoRcvBuff.uchMajorClass = CLASS_PARATRANSMNEMO;

    switch (pItem->uchMinorClass) {
    case CLASS_TOTALPOSTCHECK:
        TransMnemoRcvBuff.uchAddress = TRN_CKSUM_ADR;
        break;

    case CLASS_PREAUTHTOTAL:
        TransMnemoRcvBuff.uchAddress = TRN_PREAUTHTTL_ADR;
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
        TransMnemoRcvBuff.uchAddress =  (UCHAR)((pItem->uchMinorClass
                        - CLASS_SOFTCHK_BASE_TTL
                        - CLASS_TOTAL1)
                        + TRN_TTL1_ADR);
        break;

    default:
        TransMnemoRcvBuff.uchAddress =  (UCHAR)((pItem->uchMinorClass
                        - CLASS_TOTAL1)
                        + TRN_TTL1_ADR);
    }

    CliParaRead(&TransMnemoRcvBuff);

    _tcsncpy(DisplayData.aszMnemonic, TransMnemoRcvBuff.aszMnemonics, PARA_TRANSMNEMO_LEN);
    //memcpy(&DisplayData.aszMnemonic, TransMnemoRcvBuff.aszMnemonics, PARA_TRANSMNEMO_LEN);

    flDispRegDescrControl |= TOTAL_CNTRL;   
    DisplayData.fbSaveControl = 0;

    /*TrnGetModeQual( &WorkMode );*//* get mode qualifier *//* ### Del (2171 for Win32) V1.0 */
    TrnGetGCFQual( &WorkGCF );                                  /* get GCF qualifier */            
    TrnGetCurQual(&WorkCur); /* ### Add (2171 for Win32) V1.0 */
    if ( ( (WorkCur.fsCurStatus & CURQUAL_OPEMASK ) == CURQUAL_CASHIER ) || /* cashier system */
        ( ( WorkGCF->fsGCFStatus & GCFQUAL_USEXEMPT ) ) ) {     /* not exempt */
    
        flDispRegDescrControl |= TAXEXMPT_CNTRL; 

    }
                              
    /* set scroll symbol */
    DisplayData.uchArrow = MldSetScrollSymbol(usType);

    DisplayData.fbSaveControl = 1;        /* 0: not save data, 1: save display data */

    DispWrite(&DisplayData);                /* display the above data */
    
    /*----- TURN OFF DESCRIPTOR CONTROL FLAG -----*/

    flDispRegDescrControl &= ~(TOTAL_CNTRL|TAXEXMPT_CNTRL); 

    return(MLD_SUCCESS);

    usScroll = usScroll;
}
#endif
/***** End Of Source *****/
