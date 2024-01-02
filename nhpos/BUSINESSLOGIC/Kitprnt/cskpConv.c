/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-06-92 : 00.00.01 :M.YAMAMOTO   :Initial
*
** GenPOS **
*   Dec-31-23 : 02.04.00 : R.Chambers   : replaced using KPS_NUM_COM with KPS_NUMBER_OF_PRINTER.
*   Dec-31-23 : 02.04.00 : R.Chambers   : replaced CliParaRead() with ParaSharedPrtRead()
*/

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
#include    "BlFWif.h"
#include    "nb.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csserm.h"
#include    "shr.h"
#include    "csstbshr.h"
#include    "cskpin.h"

static SHORT s_nComTable[KPS_NUMBER_OF_PRINTER];    /* com - KP # table */
static UCHAR s_acShrTable[KPS_NUMBER_OF_PRINTER];   /* KP# - 'Shared flag' table */

/*
*==========================================================================
**    Synopsis:   VOID _KpsMakePortTable(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:
*               Update s_nComTable[] and s_acShrTable[] from current
*               settings Prog #50 Addr 33-40, 41-48.
*==========================================================================
*/
/*** COM - Printer TABLE ***/
VOID _KpsMakePortTable(VOID)
{
    const SHORT     nMyTermNo = _KpsGetMyTerminalNo();      /* my own terminal number */

    /* clear tables */
    memset(s_nComTable,  0, sizeof(s_nComTable));
    memset(s_acShrTable, 0, sizeof(s_acShrTable));

    /* for each kitchen printer, get P50 settings and update tables */
    for (SHORT nKpIdx = 0; nKpIdx < KPS_NUMBER_OF_PRINTER; nKpIdx++) {
		PARASHAREDPRT ParaShPrt_Term = {0}; /* data from Prog #50 Addr 33-40 */
		PARASHAREDPRT ParaShPrt_Com = {0};  /* data from Prog #50 Addr 41-48 */

        /* Get Terminal number of Kitchen Printer (addr. 33-40) */
        ParaShPrt_Term.uchMajorClass = CLASS_PARASHRPRT;
        ParaShPrt_Term.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + nKpIdx);
        ParaSharedPrtRead(&ParaShPrt_Term);   /* Prog50 Addr(33 + nKpIdx) */

        /* Destination table indicates only locally connected printer */
        if ((SHORT)ParaShPrt_Term.uchTermNo != nMyTermNo) {
            continue;       /* Other terminal number is specified. Ignore it. */
        }

        /* Get COM port number of Kitchen Printer (addr. 41-48) */
        ParaShPrt_Com.uchMajorClass = CLASS_PARASHRPRT;
        ParaShPrt_Com.uchAddress = (UCHAR)(SHR_COM_KP1_DEF_ADR + nKpIdx);
        ParaSharedPrtRead(&ParaShPrt_Com);    /* Prog50 Addr(41 + nKpIdx) */

        /* update 'com - KP # table' only when valid port number is specified */
        if (0 < ParaShPrt_Com.uchTermNo && ParaShPrt_Com.uchTermNo <= KPS_NUMBER_OF_PRINTER) {
            s_nComTable[ParaShPrt_Com.uchTermNo - 1] = (USHORT)nKpIdx + 1;
        }

        /* update 'KP# - Shared flag table' only when port number is zero. */
        if (ParaShPrt_Com.uchTermNo == 0) {
            s_acShrTable[nKpIdx] = KPS_SHRED_PRINTER;
        }
    }
}

/* 1 <= return < KPS_NUMBER_OF_PRINTER */
SHORT   _KpsComNo2KPNo(SHORT nComNo){
    if(1 <= nComNo && nComNo <= KPS_NUMBER_OF_PRINTER){
        return  s_nComTable[nComNo - 1];
    }
    return  0;
}


/* 1 <= return <= KPS_NUMBER_OF_PRINTER */
SHORT   _KpsKPNo2ComNo(SHORT nKPNo){
    for(SHORT cnt = 0; cnt < KPS_NUMBER_OF_PRINTER; cnt ++){
        if(s_nComTable[cnt] == nKPNo)
            return  cnt + 1;  // move cnt into proper range for a kitchen printer number
    }
    return  0;
}


USHORT   _KpsPortSts2PrtSts(void){
    USHORT   usKPStatus = 0;

    _KpsMakePortTable();

    for(USHORT cnt = 0; cnt < KPS_NUMBER_OF_PRINTER; cnt++){
        /* if KP in use, check port # and port status */
        if((0 < s_nComTable[cnt]) && (g_uchPortStatus & (0x01 << cnt))){
            /* set KP status */
            usKPStatus |= (0x01 << (s_nComTable[cnt] - 1) );
        }
    }
    return  usKPStatus;
}

BOOL _KpsIsEnable(SHORT nPrinterNo)
{
    BOOL bRet = FALSE;

    if (KPS_CHK_KPNUMBER(nPrinterNo)) {
        /* true if specified KP is available */
        if ((0x01 << (nPrinterNo - 1)) & _KpsPortSts2PrtSts()) {
            bRet = TRUE;
        }
        else if (s_acShrTable[nPrinterNo - 1] == KPS_SHRED_PRINTER) {
            /* true if specified printer number is assigned as shared printer */

            if(BlFwIfGetPrintType())
		    {
			    bRet = TRUE;
		    }else
		    {
			    bRet = FALSE;
			    auchKpsDownPrinter[nPrinterNo] = KPS_SHARED_DOWN;
		    }
        }
   }

    return  bRet;
}

BOOL _KpsIsSharedPrinter(SHORT nPrinterNo)
{
    BOOL bRet = FALSE;

    /* return false if passed KP number is invalid */
    if (KPS_CHK_KPNUMBER(nPrinterNo)) {
        /* check if specified printer number is shared printer or not */
        bRet = (s_acShrTable[nPrinterNo - 1] == KPS_SHRED_PRINTER);
    }

    return bRet;
}

VOID _KpsSetPrinterSts(SHORT sPrinterNo, BOOL bSetVal)
{
    if(KPS_CHK_KPNUMBER(sPrinterNo)) {
        SHORT   nComNo = _KpsKPNo2ComNo(sPrinterNo);
        if(0 < nComNo){
            if(bSetVal){
                /* set */
                g_uchPortStatus |= (0x01 << (nComNo - 1) );
            }else{
                /* reset */
                g_uchPortStatus &= ~(0x01 << (nComNo - 1) );
            }
        }
    }
}

SHORT   _KpsGetMyTerminalNo(void){
    SYSCONFIG CONST  *pSysconfig = PifSysConfig();

    return  (SHORT)(pSysconfig->auchLaddr[CLI_POS_UA]);
}

SHORT  _KpsGetMySharedPrinterNo (UCHAR uchPrinterNo)
{
    SHORT         nTermNo = 0;        /* terminal number specified in Prog#50, Addr.33-40 */

    if(KPS_CHK_KPNUMBER(uchPrinterNo)) {
        PARASHAREDPRT ParaShPrt_Term = { 0 }; /* data from Prog #50 Addr 33-40 */

		ParaShPrt_Term.uchMajorClass = CLASS_PARASHRPRT;
		ParaShPrt_Term.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + uchPrinterNo - 1);
        ParaSharedPrtRead(&ParaShPrt_Term);   /* Prog50 Addr(33 + nKpIdx) */
		nTermNo = (SHORT)ParaShPrt_Term.uchTermNo;
	}
	return nTermNo;
}

BOOL    _KpsIsEpson(SHORT nPrinterNo){
    /* ### MAKING (042000) */
    return  TRUE;
}


/* ========================================================================= */
