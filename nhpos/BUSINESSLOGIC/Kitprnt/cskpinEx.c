/*========================================================================*\
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
*   Title              : Client/Server KITCHEN PRINTER module, Program List
*   Category           : Client/Server KITCHEN PRINTER module, NCR2170 US HOSPITAL FOR US MODEL
*   Program Name       : CSKP.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows.
*
*       VOID    KpsInit         * Initiarize for kitchen print *
*       SHORT   KpsPrint        * print kitchen printer *
*       SHORT   KpsUnlock       * Clear Terminal Lock memory *
*       USHORT  KpsConvertError * Convert Error Code to leadthru number *
*       SHORT   KpsCheckPrint   * Check if printer is printing or not. *
*       SHORT   KpsCreateFile   * Create buffer file
*       SHORT   KpsDeleteFile   * Delete buffer file
*
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*
** GenPOS **
*   Dec-24-23 : 02.04.00 : R.Chambers   : eliminated include file cskpinEx.h from source tree
*   Dec-24-23 : 02.04.00 : R.Chambers   : removed unused include files
*   Dec-24-23 : 02.04.00 : R.Chambers   : removed unused function _IsSaratoga()
\*=======================================================================*/

#include	<tchar.h>

#include    <string.h>
#include    <stdlib.h>
#include    "ecr.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "para.h"
#include    "cskp.h"
#include    "appllog.h"
#include    "shr.h"
#include    "csstbshr.h"
#include    "cskpin.h"

#if defined(HISTORICAL_NOTES)
// following left for historical purposes as it appears that
// in PEP, this MDC is marked as Reserved.
// Looks like this MDC bit was used to in the kitchen printer code
// was an NHPOS terminal (Project Saratoga) or not. It is no longer used.
//     Richard Chambers, Jan-01-2023
//
BOOL    _IsSaratoga(void) {
    return  CliParaMDCCheck(MDC_KP2_ADR, EVEN_MDC_BIT2);  // historical notes only.
}
#endif

SHORT   _KpsGetAltPrinterNo(USHORT usOutPrinterInfo){
    SHORT   nAltPrtNo = 0;  // default is no alternate printer
    SHORT   nPrtNo = (SHORT)(usOutPrinterInfo & 0x00FF);

    if (KPS_CHK_KPNUMBER(nPrtNo)) do {
        UCHAR   auchAltTbl[MAX_DEST_SIZE] = { 0 };
        if(usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN){
            /* manual */
            ParaManuAltKitchReadAll(auchAltTbl);
        }else if(usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_AUTO){
            /* auto */
            ParaAutoAltKitchReadAll(auchAltTbl);
        }
        else
            break;  // no alternate printer

        if( _KpsIsEnable(auchAltTbl[nPrtNo - 1]))
            nAltPrtNo = auchAltTbl[nPrtNo - 1];

    } while (0);

    return  nAltPrtNo;
}


BOOL    _KpsIsAlt(USHORT usOutPrinterInfo){
    return  (usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN) || (usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_AUTO);
}

/*
*==========================================================================
**    Synopsis:   VOID KpsMakeOutPutPrinterEx(KPSBUFFER *pKpsBuffer, USHORT *pPrinterSts)
*
*       Input:    KPSBUFFER *pKpsBuffer
*
*      Output:    UCHAR     *puchKpsPrinterStatus
*       InOut:    Nothing
*
**  Return   :    
*                 
**  Description:
*               Make Output Printer Table.
*==========================================================================
*/
VOID    KpsMakeOutPutPrinterEx(KPSBUFFER* pKpsBuffer, USHORT* pPrinterSts)
{
    SHORT   nPrinterNo = (pKpsBuffer->usOutPrinterInfo & 0x00ff);

    *pPrinterSts = 0;
    memset(pKpsBuffer->auchOutputPrinter, 0, sizeof(pKpsBuffer->auchOutputPrinter));

    if (KPS_CHK_KPNUMBER(nPrinterNo)) {
       if(_KpsIsAlt(pKpsBuffer->usOutPrinterInfo)) {
            SHORT   nAltPrtNo;
            /*** ALTERNATE(manual/auto) ***/
            nAltPrtNo = _KpsGetAltPrinterNo(pKpsBuffer->usOutPrinterInfo);
            if(nAltPrtNo > 0){
                pKpsBuffer->auchOutputPrinter[nAltPrtNo - 1] = (UCHAR)nAltPrtNo;
                *pPrinterSts |= (USHORT)(0x0001 << (nAltPrtNo - 1));
                return;
            }
        }
        if(_KpsIsEnable(nPrinterNo)){
            pKpsBuffer->auchOutputPrinter[nPrinterNo - 1] = (UCHAR)nPrinterNo;
            *pPrinterSts |= (USHORT)(0x0001 << (nPrinterNo - 1));
        }
    }
}
/*
*==========================================================================
* end of file
*==========================================================================
*/
