#include	<tchar.h>


#include    <string.h>
#include    <stdlib.h>
#include    "ecr.h"
#include    "pif.h"
#include    "paraequ.h"
#include    "para.h"
#include    "cskp.h"
#include    "csstbpar.h"
#include    "appllog.h"
#include    "nb.h"
#include    "plu.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csserm.h"
#include    "shr.h"
#include    "csstbshr.h"
#include    "cskpin.h"

#include    "cskpinEx.h"

BOOL    _IsSaratoga(void){
    return  CliParaMDCCheck(MDC_KP2_ADR, EVEN_MDC_BIT2);
}


SHORT   _KpsGetAltPrinterNo(const USHORT usOutPrinterInfo){
    UCHAR   auchAltTbl[KPS_NUMBER_OF_PRINTER];
    SHORT   nAltPrtNo = 0,nPrtNo;
    USHORT  usRetLen;

    nPrtNo = (SHORT)(usOutPrinterInfo & 0x00FF);
    if( !(KPS_CHK_KPNUMBER( nPrtNo )) )
        return  0;

    if(usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN){
        /* manual */
        CliParaAllRead(CLASS_PARAMANUALTKITCH,auchAltTbl,KPS_NUMBER_OF_PRINTER,0,&usRetLen);
    }else if(usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_AUTO){
        /* auto */
        CliParaAllRead(CLASS_PARAAUTOALTKITCH,auchAltTbl,KPS_NUMBER_OF_PRINTER,0,&usRetLen);
    }
    else
        return  0;

    nAltPrtNo = auchAltTbl[nPrtNo - 1];
    if(_KpsIsEnable(nAltPrtNo))
        return  nAltPrtNo;
    else
        return  0;

}


BOOL    _KpsIsAlt(const USHORT usOutPrinterInfo){
    if((usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN) ||
        (usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_AUTO) ){
        return  TRUE;
    }
    return  FALSE;
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
VOID    KpsMakeOutPutPrinterEx(KPSBUFFER *pKpsBuffer, USHORT * pPrinterSts)
{
    SHORT   nPrinterNo,nAltPrtNo;

    nPrinterNo = (pKpsBuffer->usOutPrinterInfo & 0x00ff);
    *pPrinterSts = 0;
    memset(pKpsBuffer->auchOutputPrinter,0,sizeof(UCHAR) * KPS_NUMBER_OF_PRINTER);

    if( !KPS_CHK_KPNUMBER( nPrinterNo ) )
        return;

    if((pKpsBuffer->usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN) || 
        (pKpsBuffer->usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_AUTO)){
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
/*
*==========================================================================
* end of file
*==========================================================================
*/
