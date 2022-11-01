/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-1998       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaAll Module
* Category    : User Interface For Supervisor, NCR 2170 System Application
* Program Name: PARAALL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               ParaAllRead()  : read system ram
*               ParaAllWrite() : write system ram
*
* --------------------------------------------------------------------------
* Update Histories
*    Date   : Ver.Rev  :  Name    : Description
* May-07-92 : 00.00.01 : J.Ikeda  : initial
* Nov-25-92 : 01.00.00 : J.Ikeda  : Adds ParaTare
* Nov-30-92 : 01.00.00 : K.You    : Chg from "pararam.h" to <pararam.h>
* Dec-01-92 : 01.00.02 : K.You    : Adds Cast of USHORT for PEP.
* Dec-07-92 : 01.00.02 : K.You    : Inserted usRamSize = 0,
*           :          :          :          puchSystemRam = NULL for PEP.
* Jun-23-93 : 01.00.12 : J.IKEDA  : Adds ParaSharedPrt
* Jul-02-93 : 01.00.12 : K.You    : Adds ParaPresetAmount
* Jul-09-93 : 01.00.12 : K.You    : Adds ParaPigRule
* Aug-23-93 : 01.00.13 : J.IKEDA  : Adds ParaSoftChk, Del ParaChar44
* Apr-24-95 : 03.00.00 : H.Ishida : Adds Order Entry Prompt and
*           :          :          : FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER
* Sep-08-98 : 03.03.00 : A.Mitsui : Adds ParaTend
*
* Jan-24-00 : 01.00.00 : K.Yanagida : Saratoga

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
==============================================================================
;                      I N C L U D E     F I L E s
;=============================================================================
**/
#include <Windows.h>
#include <string.h>

#include <ecr.h>
#include <log.h>
#include <r20_pif.h>
#include <plu.h>       /* Saratoga */
#include <appllog.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include    "mypifdefs.h"

/* read transaction mnemonics of appointed address */
VOID RflGetTranMnem (WCHAR *pszDest, USHORT usAddress)
{
    --usAddress;   /* "-1" fits program address to RAM address */
	if( usAddress >= MAX_TRANSM_NO ) {
		memset(pszDest, 0x00, PARA_TRANSMNEMO_LEN * sizeof(pszDest[0]));
	}
	else{
		wcsncpy(pszDest, &ParaTransMnemo[usAddress][0], PARA_TRANSMNEMO_LEN );
		pszDest[PARA_TRANSMNEMO_LEN] = '\0';    // ensure zero termination of string.
	}
}

/* set transaction mnemonics in appointed address */
VOID RflPutTranMnem (WCHAR *pszDest, USHORT usAddress)
{
    --usAddress;   /* "-1" fits program address to RAM address */
	if (usAddress < MAX_TRANSM_NO ) {
		wcsncpy( &ParaTransMnemo[usAddress][0], pszDest, PARA_TRANSMNEMO_LEN );
	}
}

/*
*=============================================================================
*
*    Synopsis: SHORT  ParaAllRead( UCHAR uchClass,
*                                           UCHAR FAR *puchRcvBuffer,
*                                           USHORT usRcvBufLen,
*                                           USHORT usStartPointer,
*                                           USHORT FAR *pusReturnLen )
*
*       input:  uchClass
*               *puchRcvBuffer
*               usRcvBufLen
*               usStartPointer
*      output:  *puchRcvBuffer
*               *pusReturnLen
*      Return:  SUCCESS               : received buffer is over ram size
*               PARA_NOTOVER_RAMSIZE  : received buffer is not over ram size.
*
* Descruption:  This function reads SYSTEM RAM.
*
*===============================================================================
*/
SHORT ParaAllRead( UCHAR uchClass,
                            UCHAR FAR *puchRcvBuffer,
                            USHORT usRcvBufLen,
                            USHORT usStartPointer,
                            USHORT FAR *pusReturnLen )
{

    USHORT usRamSize;
    UCHAR FAR *puchSystemRam;
    USHORT usCopySize;


    /* distinguish data class */

    switch(uchClass) {
    case CLASS_PARAPLUNOMENU:
        usRamSize = sizeof(ParaPLUNoMenu);
        puchSystemRam = ( UCHAR FAR *)ParaPLUNoMenu;   /* USHORT -> UCHAR */
        break;

    case CLASS_PARAROUNDTBL:
        usRamSize = sizeof(ParaRound);
        puchSystemRam = ParaRound[0];
        break;

    case CLASS_PARADISCTBL:
        usRamSize = sizeof(ParaDisc);
        puchSystemRam = ParaDisc;
        break;

    case CLASS_PARAPROMOTION:
        usRamSize = sizeof(ParaPromotion);
        puchSystemRam = (UCHAR *)ParaPromotion;//ParaPromotion[0];
        break;

    case CLASS_PARACURRENCYTBL:
        usRamSize = sizeof(ParaCurrency);
        puchSystemRam = ( UCHAR FAR *)ParaCurrency;    /* ULONG -> UCHAR */
        break;

    case CLASS_PARAMENUPLUTBL:
        usRamSize = sizeof(ParaMenuPLU);
        puchSystemRam = ( UCHAR FAR *)ParaMenuPLU;     /* MENUPLU -> UCHAR */
        break;

    case CLASS_PARATAXRATETBL:
        usRamSize = sizeof(ParaTaxRate);
        puchSystemRam = ( UCHAR FAR *)ParaTaxRate;     /* ULONG -> UCHAR */
        break;

    case CLASS_PARAMDC:
        usRamSize = sizeof(ParaMDC);
        puchSystemRam = ParaMDC;
        break;

    case CLASS_PARAFSC:
        usRamSize = sizeof(ParaFSC);
        puchSystemRam = ( UCHAR FAR *)ParaFSC;
        break;

    case CLASS_PARASECURITYNO:
        usRamSize = sizeof(ParaSecurityNo);
        puchSystemRam = ( UCHAR FAR *)&ParaSecurityNo;  /* USHORT -> UCHAR */
        break;

    case CLASS_PARASUPLEVEL:
        usRamSize = sizeof(ParaSupLevel);
        puchSystemRam = ( UCHAR FAR *)ParaSupLevel;    /* SUPLEVEL -> UCHAR */
        break;

    case CLASS_PARAACTCODESEC:
        usRamSize = sizeof(ParaActCodeSec);
        puchSystemRam = ParaActCodeSec;
        break;

    case CLASS_PARATRANSHALO:
        usRamSize = sizeof(ParaTransHALO);
        puchSystemRam = ParaTransHALO;
        break;

    case CLASS_PARAHOURLYTIME:
        usRamSize = sizeof(ParaHourlyTime);
        puchSystemRam = ( UCHAR FAR *)ParaHourlyTime;  /* USHORT -> UCHAR */
        break;

    case CLASS_PARASLIPFEEDCTL:
        usRamSize = sizeof(ParaSlipFeedCtl);
        puchSystemRam = ParaSlipFeedCtl;
        break;

    case CLASS_PARATRANSMNEMO:
        usRamSize = sizeof(ParaTransMnemo);
        puchSystemRam = ( UCHAR *)ParaTransMnemo;//ParaTransMnemo[0];
        break;

    case CLASS_PARALEADTHRU:
        usRamSize = sizeof(Para_LeadThru);
        puchSystemRam = (UCHAR *)Para_LeadThru;//Para_LeadThru[0];
        break;

    case CLASS_PARAREPORTNAME:
        usRamSize = sizeof(ParaReportName);
        puchSystemRam = (UCHAR *)ParaReportName;//ParaReportName[0];
        break;

    case CLASS_PARASPECIALMNEMO:
        usRamSize = sizeof(ParaSpeMnemo);
        puchSystemRam = (UCHAR *)ParaSpeMnemo;//ParaSpeMnemo[0];
        break;

    case CLASS_PARATAXTBL1:
    case CLASS_PARATAXTBL2:
    case CLASS_PARATAXTBL3:
    case CLASS_PARATAXTBL4:
        usRamSize = sizeof(ParaTaxTable);
        puchSystemRam = ParaTaxTable;
        break;

    case CLASS_PARAADJMNEMO:
        usRamSize = sizeof(ParaAdjMnemo);
        puchSystemRam = (UCHAR *)ParaAdjMnemo;//ParaAdjMnemo[0];
        break;

    case CLASS_PARAPRTMODI:
        usRamSize = sizeof(ParaPrtModi);
        puchSystemRam = (UCHAR *)ParaPrtModi;//ParaPrtModi[0];
        break;

    case CLASS_PARAMAJORDEPT:
        usRamSize = sizeof(ParaMajorDEPT);
        puchSystemRam = (UCHAR *)ParaMajorDEPT;//ParaMajorDEPT[0];
        break;

    case CLASS_PARACHAR24:
        usRamSize = sizeof(ParaChar24);
        puchSystemRam = (UCHAR *)ParaChar24;//ParaChar24[0];
        break;

    case CLASS_PARACTLTBLMENU:
        usRamSize = sizeof(ParaCtlTblMenu);
        puchSystemRam = ParaCtlTblMenu;
        break;

    case CLASS_PARAAUTOALTKITCH:
        usRamSize = sizeof(ParaAutoAltKitch);
        puchSystemRam = ParaAutoAltKitch;
        break;

    case CLASS_PARATEND:    /* Prog #62 */
        usRamSize = sizeof(ParaTend);               /* V3.3 */
        puchSystemRam = ParaTend;
        break;

    case CLASS_TENDERKEY_INFO:
#pragma message("Need to complete CLASS_TENDERKEY_INFO in ParaAllRead.")
        usRamSize = sizeof(ParaTenderKeyInformation);               /* V3.3 */
        puchSystemRam = (UCHAR *)ParaTenderKeyInformation;
        break;

	case CLASS_FILEVERSION_INFO:
		usRamSize = sizeof(ParaFileVersion);
		puchSystemRam = (UCHAR *)ParaFileVersion;
		break;



    case CLASS_PARAFLEXMEM:
        usRamSize = sizeof(ParaFlexMem);
        puchSystemRam = ( UCHAR FAR *)ParaFlexMem; /* FLEXMEM -> UCHAR */
        break;

    case CLASS_PARASTOREGNO:
        usRamSize = sizeof(ParaStoRegNo);
        puchSystemRam = ( UCHAR FAR *)ParaStoRegNo;    /* USHORT -> UCHAR */
        break;

    case CLASS_PARATTLKEYTYP:
        usRamSize = sizeof(ParaTtlKeyTyp);
        puchSystemRam = ParaTtlKeyTyp;
        break;

    case CLASS_PARATTLKEYCTL:
        usRamSize = sizeof(ParaTtlKeyCtl);
        puchSystemRam = (UCHAR *)ParaTtlKeyCtl;//ParaTtlKeyCtl[0];
        break;

    case CLASS_PARAMANUALTKITCH:
        usRamSize = sizeof(ParaManuAltKitch);
        puchSystemRam = ParaManuAltKitch;
        break;

    case CLASS_PARACASHABASSIGN:
        usRamSize = sizeof(ParaCashABAssign);
        puchSystemRam = ( UCHAR FAR *)ParaCashABAssign;    /* USHORT -> UCHAR */
        break;

    case CLASS_PARASPCCO:
        usRamSize = sizeof(ParaSpcCo);
        puchSystemRam = ( UCHAR FAR *)ParaSpcCo;           /* USHORT -> UCHAR */
        break;

    case CLASS_PARAPCIF:
        usRamSize = sizeof(ParaPCIF);
        puchSystemRam = (UCHAR *)ParaPCIF;
        break;

    case CLASS_PARATONECTL:
        usRamSize = sizeof(ParaToneCtl);
        puchSystemRam = ParaToneCtl;
        break;

    case CLASS_PARATARE:
        usRamSize = sizeof(ParaTare);
        puchSystemRam = ( UCHAR FAR *)ParaTare;            /* USHORT -> UCHAR */
        break;

    case CLASS_PARASHRPRT:
        usRamSize = sizeof(Para_SharedPrt);
        puchSystemRam = Para_SharedPrt;
        break;

    case CLASS_PARAPRESETAMOUNT:
        usRamSize = sizeof(ParaPresetAmount);
        puchSystemRam = ( UCHAR FAR *)ParaPresetAmount;    /* ULONG -> UCHAR */
        break;

   case CLASS_PARAPIGRULE:                               /* A/C 130 */
        usRamSize = sizeof(ParaPigRule);
        puchSystemRam = ( UCHAR FAR *)ParaPigRule;       /* ULONG -> UCHAR */
        break;

    case CLASS_PARASOFTCHK:                             /* A/C 87 */
        usRamSize = sizeof(ParaSoftChk);
        /*puchSystemRam = ( UCHAR FAR *)&ParaSoftChk;*/
        puchSystemRam = (UCHAR *)ParaSoftChk;//ParaSoftChk[0]; /* ### MOD Saratoga (051900) */
        break;

    case CLASS_PARAHOTELID:                             /* Prog# 54 */
        usRamSize = sizeof(Para_HotelId);
        puchSystemRam = ( UCHAR FAR *)&Para_HotelId;
        break;

    case CLASS_PARAOEPTBL:                             /* A/C 160 */
        usRamSize = sizeof(ParaOep);
        puchSystemRam = ( UCHAR FAR *)&ParaOep;
        break;

    case CLASS_PARAFXDRIVE:                             /* A/C 162 */
        usRamSize = sizeof(ParaFlexDrive);
        puchSystemRam = ( UCHAR FAR *)&ParaFlexDrive;
        break;

    case CLASS_PARASERVICETIME:                        /* A/C 133 */
        usRamSize = sizeof(ParaServiceTime);
        puchSystemRam = ( UCHAR FAR *)&ParaServiceTime;
        break;

    case CLASS_PARALABORCOST:                          /* A/C 154 */
        usRamSize = sizeof(ParaLaborCost);
        puchSystemRam = ( UCHAR FAR *)&ParaLaborCost;
        break;

    case CLASS_PARADISPPARA:
        usRamSize = sizeof(Para_DispPara);
        puchSystemRam = ( UCHAR FAR *)Para_DispPara; /* DISPPARA -> UCHAR */
        break;

    case CLASS_PARAKDSIP:
        usRamSize = sizeof(Para_KdsIp);
        puchSystemRam = ( UCHAR FAR *)Para_KdsIp; /* Prog 51, 2172 */
        break;

    case CLASS_PARARESTRICTION:
        usRamSize = sizeof(ParaRestriction);
        puchSystemRam = ( UCHAR FAR *)ParaRestriction;     /* RESTRICT -> UCHAR, 2172 */
        break;

    case CLASS_PARABOUNDAGE:                             /* A/C 208, 2172 */
        usRamSize = sizeof(Para.auchParaBoundAge);
        puchSystemRam = ( UCHAR FAR *)&Para.auchParaBoundAge;
        break;

    case CLASS_PARADEPTNOMENU:
        usRamSize = sizeof(ParaDeptNoMenu);
        puchSystemRam = ( UCHAR FAR *)ParaDeptNoMenu;   /* USHORT -> UCHAR, 2172 */
        break;

    case CLASS_PARAMISCPARA:   /* ### ADD Saratoga (051900) */
        usRamSize = sizeof(ParaMiscPara);
        puchSystemRam = ( UCHAR FAR *)ParaMiscPara;   /* Misc para 2172 */
        break;

    case CLASS_PARASTOREFORWARD:
        usRamSize = sizeof(ParaStoreForward);
        puchSystemRam = ( UCHAR *)&ParaStoreForward;   /* Misc para 2172 */
        break;

    case CLASS_PARATERMINALINFO:
        usRamSize = sizeof(ParaTerminalInformationPara);
        puchSystemRam = ( UCHAR FAR *)ParaTerminalInformationPara;   /* Misc para 2172 */
        break;

	case CLASS_PARAAUTOCPN:
		usRamSize = sizeof(ParaAutoCoupon);
		puchSystemRam = (UCHAR FAR *)&ParaAutoCoupon; /* PARAAUTOCPN -> UCHAR */
		break;

	case CLASS_PARACOLORPALETTE:	//CSMALL-colorpalette
		usRamSize = sizeof(ParaColorPalette);
		puchSystemRam = (UCHAR FAR *)&ParaColorPalette; // PARACOLORPALETTE -> UCHAR
		break;

	case CLASS_PARATTLKEYORDERDEC:
		usRamSize = sizeof(ParaTtlKeyOrderDecType);
		puchSystemRam = (UCHAR FAR *)&ParaTtlKeyOrderDecType;
		break;

	case CLASS_PARACASHABASSIGNJOB:
		usRamSize = sizeof(ParaCashABAssignJob);
		puchSystemRam = ( UCHAR FAR *)ParaCashABAssignJob;
		break;

	case CLASS_PARAREASONCODE:
		usRamSize = sizeof(ParaReasonCode);
		puchSystemRam = ( UCHAR *)ParaReasonCode;
		break;

    default:
        usRamSize = 0;
        puchSystemRam = NULL;
        PifAbort(MODULE_PARA_ID, FAULT_INVALID_DATA);
    }

    /* check if offset is over ram size */

    if ( usStartPointer > usRamSize ) {
        return(PARA_OFFOVER_RAMSIZE);             /* error - offset is over ram size */
    } else {

        /* check if received buffer is over ram size */

        if ( usRamSize - usStartPointer > usRcvBufLen ) {  /* received buffer is not over ram size */
            memcpy( ( UCHAR FAR *)puchRcvBuffer, puchSystemRam + usStartPointer, usRcvBufLen );
            *pusReturnLen = usRcvBufLen;
            return(PARA_NOTOVER_RAMSIZE);
        } else {                         /* received buffer is over ram size */
            usCopySize = ( USHORT)(usRamSize - usStartPointer);
            memcpy( ( UCHAR FAR *)puchRcvBuffer, puchSystemRam + usStartPointer, usCopySize );
            *pusReturnLen = usCopySize;
            return(SUCCESS);
        }
    }

}

/*
*=============================================================================
*
*    Synopsis: SHORT ParaAllWrite( UCHAR uchClass,
*                                           UCHAR FAR *puchWrtBuffer,
*                                           USHORT usWrtBufLen,
*                                           USHORT usStartPointer,
*                                           USHORT FAR *pusReturnLen )
*
*       input: uchClass
*              *puchWrtBuffer
*               usRWrtBufLen
*               usStartPointer
*      output: *pusReturnLen
*
*      Return: SUCCESS               : Successful
*              PARA_OFFOVER_RAMSIZE  : Offset is over RAM SIZE
*
*
* Descruption:  This function writes SYSTEM RAM.
*
*===============================================================================
*/
SHORT ParaAllWrite( UCHAR uchClass,
                             UCHAR FAR *puchWrtBuffer,
                             USHORT usWrtBufLen,
                             USHORT usStartPointer,
                             USHORT FAR *pusReturnLen )
{

    USHORT usRamSize;
    UCHAR FAR *puchSystemRam;
    USHORT  usCopySize;


    /* distinguish data class */

    switch(uchClass) {
    case CLASS_PARAPLUNOMENU:
        usRamSize = sizeof(ParaPLUNoMenu);
        puchSystemRam = ( UCHAR FAR *)ParaPLUNoMenu;   /* USHORT -> UCHAR */
        break;

    case CLASS_PARAROUNDTBL:
        usRamSize = sizeof(ParaRound);
        puchSystemRam = ParaRound[0];
        break;

    case CLASS_PARADISCTBL:
        usRamSize = sizeof(ParaDisc);
        puchSystemRam = ParaDisc;
        break;

    case CLASS_PARAPROMOTION:
        usRamSize = sizeof(ParaPromotion);
        puchSystemRam = (UCHAR *)ParaPromotion;//ParaPromotion[0];
        break;

    case CLASS_PARACURRENCYTBL:
        usRamSize = sizeof(ParaCurrency);
        puchSystemRam = ( UCHAR FAR *)ParaCurrency;    /* ULONG -> UCHAR */
        break;

    case CLASS_PARAMENUPLUTBL:
        usRamSize = sizeof(ParaMenuPLU);
        puchSystemRam = ( UCHAR FAR *)ParaMenuPLU;     /* MENUPLU -> UCHAR */
        break;

    case CLASS_PARATAXRATETBL:
        usRamSize = sizeof(ParaTaxRate);
        puchSystemRam = ( UCHAR FAR *)ParaTaxRate;     /* ULONG -> UCHAR */
        break;

    case CLASS_PARAMDC:
        usRamSize = sizeof(ParaMDC);
        puchSystemRam = ParaMDC;
        break;

    case CLASS_PARAFSC:
        usRamSize = sizeof(ParaFSC);
        puchSystemRam = ( UCHAR FAR *)ParaFSC;
        break;

    case CLASS_PARASECURITYNO:
        usRamSize = sizeof(ParaSecurityNo);
        puchSystemRam = ( UCHAR FAR *)&ParaSecurityNo;  /* USHORT -> UCHAR */
        break;

    case CLASS_PARASUPLEVEL:
        usRamSize = sizeof(ParaSupLevel);
        puchSystemRam = ( UCHAR FAR *)ParaSupLevel;    /* SUPLEVEL -> UCHAR */
        break;

    case CLASS_PARAACTCODESEC:
        usRamSize = sizeof(ParaActCodeSec);
        puchSystemRam = ParaActCodeSec;
        break;

    case CLASS_PARATRANSHALO:
        usRamSize = sizeof(ParaTransHALO);
        puchSystemRam = ParaTransHALO;
        break;

    case CLASS_PARAHOURLYTIME:
        usRamSize = sizeof(ParaHourlyTime);
        puchSystemRam = ( UCHAR FAR *)ParaHourlyTime;  /* USHORT -> UCHAR */
        break;

    case CLASS_PARASLIPFEEDCTL:
        usRamSize = sizeof(ParaSlipFeedCtl);
        puchSystemRam = ParaSlipFeedCtl;
        break;

    case CLASS_PARATRANSMNEMO:
        usRamSize = sizeof(ParaTransMnemo);
        puchSystemRam = (UCHAR *)ParaTransMnemo;//ParaTransMnemo[0];
        break;

    case CLASS_PARALEADTHRU:
        usRamSize = sizeof(Para_LeadThru);
        puchSystemRam = (UCHAR *)Para_LeadThru;//Para_LeadThru[0];
        break;

    case CLASS_PARAREPORTNAME:
        usRamSize = sizeof(ParaReportName);
        puchSystemRam = (UCHAR *)ParaReportName;//ParaReportName[0];
        break;

    case CLASS_PARASPECIALMNEMO:
        usRamSize = sizeof(ParaSpeMnemo);
        puchSystemRam = (UCHAR *)ParaSpeMnemo;//ParaSpeMnemo[0];
        break;

    case CLASS_PARATAXTBL1:
    case CLASS_PARATAXTBL2:
    case CLASS_PARATAXTBL3:
    case CLASS_PARATAXTBL4:
        usRamSize = sizeof(ParaTaxTable);
        puchSystemRam = ParaTaxTable;
        break;

    case CLASS_PARAADJMNEMO:
        usRamSize = sizeof(ParaAdjMnemo);
        puchSystemRam = (UCHAR *)ParaAdjMnemo;//ParaAdjMnemo[0];
        break;

    case CLASS_PARAPRTMODI:
        usRamSize = sizeof(ParaPrtModi);
        puchSystemRam = (UCHAR *)ParaPrtModi;//ParaPrtModi[0];
        break;

    case CLASS_PARAMAJORDEPT:
        usRamSize = sizeof(ParaMajorDEPT);
        puchSystemRam = (UCHAR *)ParaMajorDEPT;//ParaMajorDEPT[0];
        break;

    case CLASS_PARACHAR24:
        usRamSize = sizeof(ParaChar24);
        puchSystemRam = (UCHAR *)ParaChar24;//ParaChar24[0];
        break;

    case CLASS_PARACTLTBLMENU:
        usRamSize = sizeof(ParaCtlTblMenu);
        puchSystemRam = ParaCtlTblMenu;
        break;

    case CLASS_PARAAUTOALTKITCH:
        usRamSize = sizeof(ParaAutoAltKitch);
        puchSystemRam = ParaAutoAltKitch;
        break;

    case CLASS_PARATEND:  /*  add V3.3 */               /* Prog #62 */
        usRamSize = sizeof(ParaTend);
        puchSystemRam = ParaTend;
        break;

    case CLASS_TENDERKEY_INFO:
#pragma message("Need to complete CLASS_TENDERKEY_INFO in ParaAllRead.")
        usRamSize = sizeof(ParaTenderKeyInformation);               /* V3.3 */
        puchSystemRam = (UCHAR *)ParaTenderKeyInformation;
        break;

	case CLASS_FILEVERSION_INFO:
		usRamSize = sizeof(ParaFileVersion);
		puchSystemRam = (UCHAR *)ParaFileVersion;
		break;

    case CLASS_PARAFLEXMEM:
        usRamSize = sizeof(ParaFlexMem);
        puchSystemRam = ( UCHAR FAR *)ParaFlexMem; /* FLEXMEM -> UCHAR */
        break;

    case CLASS_PARASTOREGNO:
        usRamSize = sizeof(ParaStoRegNo);
        puchSystemRam = ( UCHAR FAR *)ParaStoRegNo;    /* USHORT -> UCHAR */
        break;

    case CLASS_PARATTLKEYTYP:
        usRamSize = sizeof(ParaTtlKeyTyp);
        puchSystemRam = ParaTtlKeyTyp;
        break;

    case CLASS_PARATTLKEYCTL:
        usRamSize = sizeof(ParaTtlKeyCtl);
        puchSystemRam = ParaTtlKeyCtl[0];
        break;

    case CLASS_PARAMANUALTKITCH:
        usRamSize = sizeof(ParaManuAltKitch);
        puchSystemRam = ParaManuAltKitch;
        break;

    case CLASS_PARACASHABASSIGN:
        usRamSize = sizeof(ParaCashABAssign);
        puchSystemRam = ( UCHAR FAR *)ParaCashABAssign;    /* USHORT -> UCHAR */
        break;

    case CLASS_PARASPCCO:
        usRamSize = sizeof(ParaSpcCo);
        puchSystemRam = ( UCHAR FAR *)ParaSpcCo;           /* USHORT -> UCHAR */
        break;

    case CLASS_PARAPCIF:
        usRamSize = sizeof(ParaPCIF);
        puchSystemRam = (UCHAR *)ParaPCIF;
        break;

    case CLASS_PARATONECTL:
        usRamSize = sizeof(ParaToneCtl);
        puchSystemRam = ParaToneCtl;
        break;

    case CLASS_PARATARE:
        usRamSize = sizeof(ParaTare);
        puchSystemRam = ( UCHAR FAR *)ParaTare;            /* USHORT -> UCHAR */
        break;

    case CLASS_PARASHRPRT:
        usRamSize = sizeof(Para_SharedPrt);
        puchSystemRam = Para_SharedPrt;
        break;

    case CLASS_PARAPRESETAMOUNT:
        usRamSize = sizeof(ParaPresetAmount);
        puchSystemRam = ( UCHAR FAR *)ParaPresetAmount;    /* ULONG -> UCHAR */
        break;

    case CLASS_PARAPIGRULE:                               /* A/C 130 */
        usRamSize = sizeof(ParaPigRule);
        puchSystemRam = ( UCHAR FAR *)ParaPigRule;      /* ULONG -> UCHAR */
        break;

    case CLASS_PARASOFTCHK:                             /* A/C 87 */
        usRamSize = sizeof(ParaSoftChk);
        /*puchSystemRam = ( UCHAR FAR *)&ParaSoftChk;*/
        puchSystemRam = (UCHAR *)ParaSoftChk;//ParaSoftChk[0]; /* ### MOD Saratoga (051900) */
        break;

    case CLASS_PARAHOTELID:                             /* Prog# 54 */
        usRamSize = sizeof(Para_HotelId);
        puchSystemRam = ( UCHAR FAR *)&Para_HotelId;
        break;

    case CLASS_PARAOEPTBL:                             /* A/C 160 */
        usRamSize = sizeof(ParaOep);
        puchSystemRam = ( UCHAR FAR *)&ParaOep;
        break;

    case CLASS_PARAFXDRIVE:                             /* A/C 162 */
        usRamSize = sizeof(ParaFlexDrive);
        puchSystemRam = ( UCHAR FAR *)&ParaFlexDrive;
        break;

    case CLASS_PARASERVICETIME:                         /* A/C 133 */
        usRamSize = sizeof(ParaServiceTime);
        puchSystemRam = ( UCHAR FAR *)&ParaServiceTime;
        break;

    case CLASS_PARALABORCOST:                           /* A/C 154 */
        usRamSize = sizeof(ParaLaborCost);
        puchSystemRam = ( UCHAR FAR *)&ParaLaborCost;
        break;

    case CLASS_PARADISPPARA:
        usRamSize = sizeof(Para_DispPara);
        puchSystemRam = ( UCHAR FAR *)Para_DispPara; /* DISPPARA -> UCHAR */
        break;

    case CLASS_PARAKDSIP:
        usRamSize = sizeof(Para_KdsIp);
        puchSystemRam = ( UCHAR FAR *)Para_KdsIp; /* Prog 51, 2172 */
        break;

    case CLASS_PARARESTRICTION:
        usRamSize = sizeof(ParaRestriction);
        puchSystemRam = ( UCHAR FAR *)ParaRestriction;     /* RESTRICT -> UCHAR, 2172 */
        break;

    case CLASS_PARABOUNDAGE:                             /* A/C 208, 2172 */
        usRamSize = sizeof(Para.auchParaBoundAge);
        puchSystemRam = ( UCHAR FAR *)&Para.auchParaBoundAge;
        break;

    case CLASS_PARADEPTNOMENU:
        usRamSize = sizeof(ParaDeptNoMenu);
        puchSystemRam = ( UCHAR FAR *)ParaDeptNoMenu;   /* USHORT -> UCHAR, 2172 */
        break;

    case CLASS_PARAMISCPARA:   /* ### ADD Saratoga (051900) */
        usRamSize = sizeof(ParaMiscPara);
        puchSystemRam = ( UCHAR FAR *)ParaMiscPara;   /* Misc para 2172 */
        break;

    case CLASS_PARASTOREFORWARD:
        usRamSize = sizeof(ParaStoreForward);
        puchSystemRam = ( UCHAR *)&ParaStoreForward;   /* Misc para 2172 */
        break;

     case CLASS_PARATERMINALINFO:
        usRamSize = sizeof(ParaTerminalInformationPara);
        puchSystemRam = ( UCHAR FAR *)ParaTerminalInformationPara;   /* Misc para 2172 */
        break;

	case CLASS_PARAAUTOCPN:
		usRamSize = sizeof(ParaAutoCoupon);
		puchSystemRam = (UCHAR FAR *)&ParaAutoCoupon; /* PARAAUTOCPN -> UCHAR */
		break;

	case CLASS_PARACOLORPALETTE:  //CSMALL-colorpalette
		usRamSize = sizeof(ParaColorPalette);
		puchSystemRam = (UCHAR FAR *)&ParaColorPalette; // PARACOLORPALETTE -> UCHAR 
		break;

	case CLASS_PARATTLKEYORDERDEC:
		usRamSize = sizeof(ParaTtlKeyOrderDecType);
		puchSystemRam = (UCHAR FAR *)&ParaTtlKeyOrderDecType;
		break;

	case CLASS_PARACASHABASSIGNJOB:
		usRamSize = sizeof(ParaCashABAssignJob);
		puchSystemRam = (UCHAR FAR *)&ParaCashABAssignJob;
		break;

	case CLASS_PARAREASONCODE:
		usRamSize = sizeof(ParaReasonCode);
		puchSystemRam = ( UCHAR *)ParaReasonCode;
		break;

    default:
        usRamSize = 0;
        puchSystemRam = NULL;
        PifAbort(MODULE_PARA_ID, FAULT_INVALID_DATA);
    }

    /* check if offset is over ram size */

    if ( usStartPointer > usRamSize ) {
        return(PARA_OFFOVER_RAMSIZE);             /* error - offset is over ram size */
    } else {

        /* check if write buffer is over ram size */

        if ( usRamSize - usStartPointer > usWrtBufLen ) {  /* write buffer is not over ram size */
            memcpy( puchSystemRam + usStartPointer, ( UCHAR FAR *)puchWrtBuffer, usWrtBufLen );
            *pusReturnLen = usWrtBufLen;
        } else {                         /* write buffer is over ram size */
            usCopySize = ( USHORT)(usRamSize - usStartPointer);
            memcpy( puchSystemRam + usStartPointer, ( UCHAR FAR *)puchWrtBuffer, usCopySize );
            *pusReturnLen = usCopySize;
        }
    }

	// Do final steps to ensure that the UNINIRAM Para database is congruent
	// between various parts.
	switch (uchClass) {
		case CLASS_PARAHOTELID :
			{
				int j;
				for (j = 0; j < 11; j++) {
					ParaTenderKeyInformation[j].auchEptTend = Para_HotelId.auchTend[j];
				}
			}
		break;
		case CLASS_PARATEND:
			{
				int j;
				for (j = 0; j < MAX_TEND_NO; j++) {
					ParaTenderKeyInformation[j].uchTend = ParaTend[j];
				}
			}
			break;
		case CLASS_TENDERKEY_INFO:
			{
				int j;
				for (j = 0; j < MAX_TEND_NO; j++) {
					ParaTend[j] = ParaTenderKeyInformation[j].uchTend;
				}
				for (j = 0; j < 11; j++) {
					Para_HotelId.auchTend[j] = ParaTenderKeyInformation[j].auchEptTend;
				}
			}
			break;
		default:
			break;
	}
    return(SUCCESS);                    /* perform */
}

/****** End of Source ******/

