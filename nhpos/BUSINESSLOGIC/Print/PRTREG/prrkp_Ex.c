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
*===========================================================================
* Title       : Print  Kitchen printer
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRKP_Ex.C
* --------------------------------------------------------------------------
* --------------------------------------------------------------------------
* Abstract:
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
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
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <cskp.h>
#include <csstbkps.h>
#include <csstbstb.h>
#include <csstbpar.h>
#include <rfl.h>
#include <uie.h>
#include <fsc.h>
#include <pif.h>
#include <pmg.h>
#include "prtrin.h"
#include "prrcolm_.h"



static  UCHAR   s_AutoAltTbl[KPS_NUMBER_OF_PRINTER];
static  UCHAR   s_ManAltTbl[KPS_NUMBER_OF_PRINTER];
static  UCHAR   s_KPTbl[KPS_NUMBER_OF_PRINTER];

static  VOID    _PrtMakeAltTbl(void){
    USHORT  usRet,cnt;
    PARASHAREDPRT   ParaShPrt_Term;

    memset(s_AutoAltTbl,0,sizeof(s_AutoAltTbl));
    memset(s_ManAltTbl,0,sizeof(s_ManAltTbl));
    memset(s_KPTbl,0,sizeof(s_KPTbl));

    /* auto */
    CliParaAllRead(CLASS_PARAAUTOALTKITCH,s_AutoAltTbl,KPS_NUMBER_OF_PRINTER,0,&usRet);

    /* manual */
    CliParaAllRead(CLASS_PARAMANUALTKITCH,s_ManAltTbl,KPS_NUMBER_OF_PRINTER,0,&usRet);

    for(cnt = 0;cnt < KPS_NUMBER_OF_PRINTER;cnt++){
        memset(&ParaShPrt_Term, 0, sizeof(ParaShPrt_Term));
        ParaShPrt_Term.uchMajorClass = CLASS_PARASHRPRT;
        ParaShPrt_Term.uchAddress = (UCHAR)(SHR_KP1_DEF_ADR + cnt);
        CliParaRead(&ParaShPrt_Term);   /* Prog50 Addr(33+cnt) */
        s_KPTbl[cnt] = ParaShPrt_Term.uchTermNo;
    }
}


static  BOOL    _PrtGetAltInfo(SHORT nPrtNo,BOOL bManual,SHORT *pAltTerm,SHORT *pAltPrt){
    SHORT   nPrt;

    *pAltTerm = 0;
    *pAltPrt = 0;

    if( !KPS_CHK_KPNUMBER(nPrtNo) )
        return  FALSE;

    if(bManual)
        nPrt = (SHORT)s_ManAltTbl[nPrtNo - 1];
    else
        nPrt = (SHORT)s_AutoAltTbl[nPrtNo - 1];

    if( !KPS_CHK_KPNUMBER(nPrt) )
        return  FALSE;
    if(0 == s_KPTbl[nPrt - 1])
        return  FALSE;

    *pAltTerm = (SHORT)s_KPTbl[nPrt - 1];
    *pAltPrt =  nPrt;
    return  TRUE;
}


static VOID _PrtMakeAltInfo(SHORT nPrtNo,BOOL bManual,USHORT * pAltInfo){

    if( !KPS_CHK_KPNUMBER(nPrtNo) ){
        *pAltInfo = 0;
        return;
    }

    *pAltInfo = nPrtNo;

    if(bManual)
        *pAltInfo |= KPS_MASK16_ALT_MAN;
    else
        *pAltInfo |= KPS_MASK16_ALT_AUTO;
}


SHORT   PrtSendKps(TCHAR *auchSndBuffer, USHORT usSndBufLen,
                    UCHAR uchUniqueAddress, USHORT *pusPrintStatus, UCHAR uchTarget,SHORT nKPNo){

    SHORT   nCliKpsPrtSts;
    USHORT  usOutPrinterInfo = 0,usDummy;
    SHORT   nTargetTerm = 0;

    _PrtMakeAltTbl();

    /* ManualAlt ??? */
    usOutPrinterInfo = (USHORT)nKPNo;
    if(_PrtGetAltInfo(nKPNo,TRUE,&nTargetTerm,&usDummy) == TRUE){
        /* manual alternation (set flag) */
        _PrtMakeAltInfo(nKPNo,TRUE,&usOutPrinterInfo);
    }else{
        /* normal */
        nTargetTerm = (SHORT)uchTarget;
    }

    /* send to kitchen printer */
    nCliKpsPrtSts = CliKpsPrintEx(auchSndBuffer,usSndBufLen,
                       uchUniqueAddress, pusPrintStatus, (UCHAR)nTargetTerm,usOutPrinterInfo);

    /* if manual alternate print has succeed, assume required printer is alive */
    if ((usOutPrinterInfo & KPS_MASK16_ALT_MAN) && (nCliKpsPrtSts == KPS_PERFORM)) {
        *pusPrintStatus = 0x01 << ((usOutPrinterInfo - 1) & 0x000f);
    }

    /* if print failed, retry with auto alternate printer */
    if (nCliKpsPrtSts == KPS_PRINTER_DOWN  && !(usOutPrinterInfo & KPS_MASK16_ALT_MAN)){
        /* get terminal No */
        if(_PrtGetAltInfo(nKPNo,FALSE,&nTargetTerm,&usDummy)){
            /* auto alternation */
            _PrtMakeAltInfo(nKPNo,FALSE,&usOutPrinterInfo);
            /* retry */
            nCliKpsPrtSts = CliKpsPrintEx(auchSndBuffer,usSndBufLen,
                       uchUniqueAddress, pusPrintStatus, (UCHAR)nTargetTerm,usOutPrinterInfo);

            /* if auto alternate print is succeed, assume requierd printer is alive */
            if (nCliKpsPrtSts == KPS_PERFORM) {
                *pusPrintStatus = 0x0001 << ((usOutPrinterInfo - 1) & 0x000f);
            }

        }
    }

    return  nCliKpsPrtSts;
}
