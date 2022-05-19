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
#include    "plu.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csserm.h"
#include    "shr.h"
#include    "csstbshr.h"
#include    "cskpin.h"

static SHORT s_nComTable[KPS_NUM_COM];              /* com - KP # table */
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
    SHORT         nKpIdx;         /* kitchen printer index (zero origin) */
    SHORT         nMyTermNo;      /* my own terminal number */

    /* clear tables */
    memset(s_nComTable,  0, sizeof(s_nComTable));
    memset(s_acShrTable, 0, sizeof(s_acShrTable));

    /* get my own terminal number */
    nMyTermNo = _KpsGetMyTerminalNo();

    /* for each kitchen printer, get settings and update tables */
    for (nKpIdx = 0; nKpIdx < KPS_NUMBER_OF_PRINTER; nKpIdx++) {
		PARASHAREDPRT ParaShPrt_Term = {0}; /* data from Prog #50 Addr 33-40 */
		PARASHAREDPRT ParaShPrt_Com = {0};  /* data from Prog #50 Addr 41-48 */

        /* Get Terminal number of Kitchen Printer (addr. 33-40) */
        ParaShPrt_Term.uchMajorClass = CLASS_PARASHRPRT;
        ParaShPrt_Term.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + nKpIdx);
        CliParaRead(&ParaShPrt_Term);   /* Prog50 Addr(33 + nKpIdx) */

        /* Destination table indicates only locally connected printer */
        if ((SHORT)ParaShPrt_Term.uchTermNo != nMyTermNo) {
            continue;       /* Other terminal number is specified. Ignore it. */
        }

        /* Get COM port number of Kitchen Printer (addr. 41-48) */
        ParaShPrt_Com.uchMajorClass = CLASS_PARASHRPRT;
        ParaShPrt_Com.uchAddress = (UCHAR)(SHR_COM_KP1_DEF_ADR + nKpIdx);
        CliParaRead(&ParaShPrt_Com);    /* Prog50 Addr(41 + nKpIdx) */

        /* update 'com - KP # table' only when valid port number is specified */
        if (0 < ParaShPrt_Com.uchTermNo && ParaShPrt_Com.uchTermNo <= KPS_NUM_COM) {
            s_nComTable[ParaShPrt_Com.uchTermNo - 1] = (USHORT)nKpIdx + 1;
        }

        /* update 'KP# - Shared flag table' only when port number is zero. */
        if (ParaShPrt_Com.uchTermNo == 0) {
            s_acShrTable[nKpIdx] = KPS_SHRED_PRINTER;
        }
    }
}

/* 1 <= return < KPS_NUMBER_OF_PRINTER */
SHORT   _KpsComNo2KPNo(const SHORT nComNo){
    if(1 <= nComNo && nComNo <= KPS_NUM_COM){
        return  s_nComTable[nComNo - 1];
    }
    return  0;
}


/* 1 <= return <= KPS_NUM_COM */
SHORT   _KpsKPNo2ComNo(const SHORT nKPNo){
    int     cnt;
    for(cnt = 1;cnt <= KPS_NUM_COM;cnt ++){
        if(s_nComTable[cnt - 1] == nKPNo)
            return  cnt;
    }
    return  0;
}


UCHAR   _KpsPortSts2PrtSts(void){
    UCHAR   uchKPStatus = 0;
    SHORT   cnt;

    _KpsMakePortTable();

    for(cnt = 0;cnt < KPS_NUM_COM;cnt++){
        /* check port # */
        if((g_uchPortStatus & (0x01 << cnt)) && (0 < s_nComTable[cnt])){
            /* set KP status */
            uchKPStatus |= (UCHAR)(0x01 << (s_nComTable[cnt] - 1) );
        }
    }
    return  uchKPStatus;
}

BOOL _KpsIsEnable(const SHORT nPrinterNo)
{
    BOOL bRet = FALSE;

    /* return false if passed KP number is invalid */
    if ((nPrinterNo < 1) || (KPS_NUMBER_OF_PRINTER < nPrinterNo)) {
        return bRet;
    }

    /* true if specified KP is available */
    if ((UCHAR)(0x01 << (nPrinterNo - 1)) & _KpsPortSts2PrtSts()) {
        bRet = TRUE;
    }

    /* true if specified printer number is assigned as shared printer */
    else if (s_acShrTable[nPrinterNo - 1] == KPS_SHRED_PRINTER) {
		if(BlFwIfGetPrintType())
		{
			bRet = TRUE;
		}else
		{
			bRet = FALSE;
			auchKpsDownPrinter[nPrinterNo] = KPS_SHARED_DOWN;
		}
    }

    return  bRet;
}

BOOL _KpsIsSharedPrinter(const SHORT nPrinterNo)
{
    BOOL bRet = FALSE;

    /* return false if passed KP number is invalid */
    if ((nPrinterNo < 1) || (KPS_NUMBER_OF_PRINTER < nPrinterNo)) {
        return bRet;
    }

    /* check if specified printer number is shared printer or not */
    if (s_acShrTable[nPrinterNo - 1] == KPS_SHRED_PRINTER) {
        bRet = TRUE;
    }
    return bRet;
}

VOID _KpsSetPrinterSts(const SHORT sPrinterNo,const BOOL bSetVal)
{
    SHORT   nComNo;

    if(1 <= sPrinterNo && sPrinterNo <= KPS_NUMBER_OF_PRINTER){
        nComNo = _KpsKPNo2ComNo(sPrinterNo);
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
    SYSCONFIG CONST FAR  *pSysconfig;

    /* get system configration */
    pSysconfig = PifSysConfig();

    return  (SHORT)(pSysconfig->auchLaddr[CLI_POS_UA]);
}

SHORT  _KpsGetMySharedPrinterNo (UCHAR uchPrinterNo)
{
    PARASHAREDPRT ParaShPrt_Term; /* data from Prog #50 Addr 33-40 */
    SHORT         nTermNo = 0;        /* terminal number specified in Prog#50, Addr.33-40 */

    if(1 <= uchPrinterNo && uchPrinterNo <= KPS_NUMBER_OF_PRINTER) {
		memset(&ParaShPrt_Term, 0, sizeof(ParaShPrt_Term));
		ParaShPrt_Term.uchMajorClass = CLASS_PARASHRPRT;
		ParaShPrt_Term.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + uchPrinterNo - 1);
		CliParaRead(&ParaShPrt_Term);   /* Prog50 Addr(33 + nKpIdx) */
		nTermNo = (SHORT)ParaShPrt_Term.uchTermNo;
	}
	return nTermNo;
}

BOOL    _KpsIsEpson(const SHORT nPrinterNo){
    /* ### MAKING (042000) */
    return  TRUE;
}


/* ========================================================================= */
