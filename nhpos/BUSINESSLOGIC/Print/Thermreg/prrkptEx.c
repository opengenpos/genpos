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
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Kitchen printer
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRKPEx_.C
* --------------------------------------------------------------------------
* Abstract:
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
*
** GenPOS **
* Dec-07-20 : v2.3.2.0 : R.Chambers  : removed include of unused file oldstdc.h.
* Dec-27-23 : c2.4.0   : R.Chambers  : replaced CliParaRead() and CliParaAllRead() with specific Para functions, ParaSharedPrtRead() etc.
* Dec-27-23 : c2.4.0   : R.Chambers  : replace boolean flag for auto/manual with masks: KPS_MASK16_ALT_MAN, KPS_MASK16_ALT_AUTO
* Jan-01-23 : v2.4.0   : R.Chambers  : removed unneeded include files.
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/

#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
#include <regstrct.h>
#include <paraequ.h>
#include <para.h>
#include <cskp.h>
#include <csstbkps.h>
#include <rfl.h>
#include <pif.h>



static  UCHAR   s_AutoAltTbl[KPS_NUMBER_OF_PRINTER];
static  UCHAR   s_ManAltTbl[KPS_NUMBER_OF_PRINTER];
static  UCHAR   s_KPTbl[KPS_NUMBER_OF_PRINTER];

static  VOID    _PrtMakeAltTbl(void) {
    memset(s_AutoAltTbl,0,sizeof(s_AutoAltTbl));
    memset(s_ManAltTbl,0,sizeof(s_ManAltTbl));
    memset(s_KPTbl,0,sizeof(s_KPTbl));

    /* auto */
    ParaAutoAltKitchReadAll(s_AutoAltTbl);

    /* manual */
    ParaManuAltKitchReadAll(s_ManAltTbl);

    for(USHORT cnt = 0; cnt < KPS_NUMBER_OF_PRINTER; cnt++){
        PARASHAREDPRT   ParaShPrt_Term = { 0 };

        ParaShPrt_Term.uchMajorClass = CLASS_PARASHRPRT;
        ParaShPrt_Term.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + cnt);
        ParaSharedPrtRead(&ParaShPrt_Term);   /* Prog50 Addr(33+cnt) */
        s_KPTbl[cnt] = ParaShPrt_Term.uchTermNo;
    }
}


static  BOOL    _PrtGetAltInfo(SHORT nPrtNo, USHORT usType, SHORT *pAltTerm, SHORT *pAltPrt){
    BOOL    bRet = FALSE;

    if (pAltTerm) *pAltTerm = 0;
    if (pAltPrt) *pAltPrt = 0;

    if (KPS_CHK_KPNUMBER(nPrtNo)) {
        SHORT   nPrt = 0;

        switch (usType) {
        case (USHORT)KPS_MASK16_ALT_MAN:
            nPrt = s_ManAltTbl[nPrtNo - 1];
            break;
        case (USHORT)KPS_MASK16_ALT_AUTO:
            nPrt = s_AutoAltTbl[nPrtNo - 1];
            break;
        }

        if (KPS_CHK_KPNUMBER(nPrt)) {
            if (KPS_CHK_KPNUMBER(s_KPTbl[nPrt - 1])) {
                if (pAltTerm) *pAltTerm = s_KPTbl[nPrt - 1];
                if (pAltPrt) *pAltPrt = nPrt;
                bRet = TRUE;
            }
        }
    }

    return  bRet;
}


static USHORT _PrtMakeAltInfo(SHORT nPrtNo, USHORT usType) {
    USHORT usAltInfo = 0;

    if( KPS_CHK_KPNUMBER(nPrtNo) ) {
        usAltInfo = nPrtNo | usType;
    }

    return usAltInfo;
}


SHORT   PrtSendKps(TCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,SHORT nKPNo){

    SHORT   nCliKpsPrtSts;
    USHORT  usOutPrinterInfo = 0;
    SHORT   nTargetTerm = 0;

    _PrtMakeAltTbl();

    /* ManualAlt ??? */
    usOutPrinterInfo = (USHORT)nKPNo;
    if(_PrtGetAltInfo(nKPNo, KPS_MASK16_ALT_MAN, &nTargetTerm, NULL)) {
        /* manual alternation (set flag) */
        usOutPrinterInfo = _PrtMakeAltInfo(nKPNo, KPS_MASK16_ALT_MAN);
    }else{
        /* normal */
        nTargetTerm = (SHORT)uchTarget;
    }

    /* send to kitchen printer */
    nCliKpsPrtSts = CliKpsPrintEx(auchSndBuffer,usSndBufLen,
                       uchUniqueAddress, pusPrintStatus, (UCHAR)nTargetTerm,usOutPrinterInfo);

    /* if manual alternate print has succeed, assume required printer is alive */
    if ((usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN) && (nCliKpsPrtSts == KPS_PERFORM)) {
        *pusPrintStatus = 0x01 << ((usOutPrinterInfo - 1) & 0x000f);
    }

    /* if print failed, retry with auto alternate printer */
    if (nCliKpsPrtSts == KPS_PRINTER_DOWN  && !(usOutPrinterInfo & (USHORT)KPS_MASK16_ALT_MAN)){
        /* get terminal No */
        if(_PrtGetAltInfo(nKPNo, KPS_MASK16_ALT_AUTO, &nTargetTerm, NULL)) {
            /* auto alternation */
            usOutPrinterInfo = _PrtMakeAltInfo(nKPNo, KPS_MASK16_ALT_AUTO);
            /* retry */
            nCliKpsPrtSts = CliKpsPrintEx(auchSndBuffer,usSndBufLen,
                       uchUniqueAddress, pusPrintStatus, (UCHAR)nTargetTerm,usOutPrinterInfo);

            /* if auto alternate print is succeed, assume required printer is alive */
            if (nCliKpsPrtSts == KPS_PERFORM) {
                *pusPrintStatus = 0x0001 << ((usOutPrinterInfo - 1) & 0x000f);
            }
        }
    }

    return  nCliKpsPrtSts;
}
/* end of prrkptEx.c */

