/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
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
* Memory Model: Midium Model
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
* Nov-05-93 :          : H.Yama   : Deletes TAXTBL4 and PIG
* Nov-12-93 : 02.00.02 : K.You    : Adds ParaHotelId
* Aug-04-98 : 03.03.00 : hrkato   : V3.3 (Fast Finalize Improvement)
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
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <log.h>
#include <pif.h>
#include <plu.h>
#include <appllog.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>
/**fhfh
;=============================================================================
;
;    Synopsis: SHORT ParaAllRead( UCHAR uchClass,
;                                 UCHAR *puchRcvBuffer,
;                                 USHORT usRcvBufLen,
;                                 USHORT usStartPointer,
;                                 USHORT *pusReturnLen )
;
;       input:  uchClass
;               *puchRcvBuffer
;               usRcvBufLen
;               usStartPointer
;      output:  *puchRcvBuffer
;               *pusReturnLen
;      Return:  SUCCESS               : received buffer is over ram size
;               PARA_NOTOVER_RAMSIZE  : received buffer is not over ram size.
;
; Descruption:  This function reads SYSTEM RAM.
;
;===============================================================================
fhfh**/

SHORT ParaAllRead( USHORT usClass,
                   VOID *puchRcvBuffer,
                   USHORT usRcvBufLen,
                   USHORT usStartPointer,
                   USHORT *pusReturnLen )
{

    USHORT usRamSize;
    UCHAR FAR *puchSystemRam;


    /* distinguish data class */

    switch(usClass) {
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
        puchSystemRam = ( UCHAR *)ParaPromotion[0];
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
		/* usRamSize = sizeof(ParaFSC1); */
		/* puchSystemRam = ( UCHAR FAR *)ParaFSC1; */
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
        usRamSize = sizeof(Para.ParaTransMnemo);
        puchSystemRam = ( UCHAR *)Para.ParaTransMnemo[0];
        break;

    case CLASS_PARALEADTHRU:
        usRamSize = sizeof(Para.ParaLeadThru);
        puchSystemRam = ( UCHAR *)Para.ParaLeadThru[0];
        break;

    case CLASS_PARAREPORTNAME:
        usRamSize = sizeof(ParaReportName);
        puchSystemRam = ( UCHAR *)ParaReportName[0];
        break;

    case CLASS_PARASPECIALMNEMO:
        usRamSize = sizeof(Para.ParaSpeMnemo);
        puchSystemRam = ( UCHAR *)Para.ParaSpeMnemo[0];
        break;

    case CLASS_PARATAXTBL1:
    case CLASS_PARATAXTBL2:
    case CLASS_PARATAXTBL3:
    case CLASS_PARATAXTBL4:
        usRamSize = sizeof(Para.ParaTaxTable);
        puchSystemRam = Para.ParaTaxTable;
        break;

    case CLASS_PARAADJMNEMO:
        usRamSize = sizeof(Para.ParaAdjMnemo);
        puchSystemRam = ( UCHAR *)Para.ParaAdjMnemo[0];
        break;

    case CLASS_PARAPRTMODI:
        usRamSize = sizeof(ParaPrtModi);
        puchSystemRam = ( UCHAR *)ParaPrtModi[0];
        break;

    case CLASS_PARAMAJORDEPT:
        usRamSize = sizeof(ParaMajorDEPT);
        puchSystemRam = ( UCHAR *)ParaMajorDEPT[0];
        break;

    case CLASS_PARACHAR24:
        usRamSize = sizeof(Para.ParaChar24);
        puchSystemRam = ( UCHAR *)Para.ParaChar24[0];
        break;

    case CLASS_PARACTLTBLMENU:
        usRamSize = sizeof(ParaCtlTblMenu);
        puchSystemRam = ParaCtlTblMenu;
        break;

    case CLASS_PARAAUTOALTKITCH:
        usRamSize = sizeof(ParaAutoAltKitch);
        puchSystemRam = ParaAutoAltKitch;
        break;

    case CLASS_PARATEND:
        usRamSize = sizeof(ParaTend);               /* V3.3 */
        puchSystemRam = ParaTend;
        break;

    case CLASS_TENDERKEY_INFO:
#pragma message("Need to complete CLASS_TENDERKEY_INFO in ParaAllRead.")
        usRamSize = sizeof(ParaTenderKeyInformation);               /* V3.3 */
        puchSystemRam = ( UCHAR *)ParaTenderKeyInformation;
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
        usRamSize = sizeof(Para.ParaCashABAssign);
        puchSystemRam = ( UCHAR *)Para.ParaCashABAssign;    /* USHORT -> UCHAR */
        break;

    case CLASS_PARASPCCO:
        usRamSize = sizeof(Para.ParaSpcCo);
        puchSystemRam = ( UCHAR *)Para.ParaSpcCo;           /* USHORT -> UCHAR */
        break;

    case CLASS_PARAPCIF:
        usRamSize = sizeof(ParaPCIF);
        puchSystemRam = ( UCHAR *)ParaPCIF;
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

    case CLASS_PARAPIGRULE:                             /* A/C 130 */
        usRamSize = sizeof(ParaPigRule);
        puchSystemRam = ( UCHAR FAR *)ParaPigRule;      /* ULONG -> UCHAR */
        break;

    case CLASS_PARASOFTCHK:                             /* A/C 87 */
        usRamSize = sizeof(ParaSoftChk);
        puchSystemRam = ( UCHAR *)ParaSoftChk[0];
        break;

    case CLASS_PARAHOTELID:                             /* Prog# 54 */
        usRamSize = sizeof(Para.ParaHotelId);
        puchSystemRam = ( UCHAR *)&Para.ParaHotelId;
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

	case CLASS_PARACOLORPALETTE:
		usRamSize = sizeof(ParaColorPalette);
		puchSystemRam = ( UCHAR FAR *)ParaColorPalette;
		break;

    case CLASS_PARADEPTNOMENU:
        usRamSize = sizeof(ParaDeptNoMenu);
        puchSystemRam = ( UCHAR FAR *)ParaDeptNoMenu;   /* USHORT -> UCHAR, 2172 */
        break;

    case CLASS_PARAMISCPARA:
        usRamSize = sizeof(ParaMiscPara);
        puchSystemRam = ( UCHAR FAR *)ParaMiscPara;   /* Misc para 2172 */
        break;

    case CLASS_PARASTOREFORWARD:
        usRamSize = sizeof(Para.ParaStoreForward);
        puchSystemRam = ( UCHAR *)Para.ParaStoreForward;   /* Misc para 2172 */
        break;

    case CLASS_PARATERMINALINFO:
        usRamSize = sizeof(ParaTerminalInformationPara);
        puchSystemRam = ( UCHAR FAR *)ParaTerminalInformationPara;   /* Misc para 2172 */
        break;

	case CLASS_FILEVERSION_INFO:
		usRamSize = sizeof(ParaFileVersion);
		puchSystemRam = (UCHAR *)ParaFileVersion;
		break;

	case CLASS_PARAAUTOCPN:
		usRamSize = sizeof(ParaAutoCoupon);
		puchSystemRam = ( UCHAR FAR *)&ParaAutoCoupon; /* PARAAUTOCPN -> UCHAR */
		break;

	case CLASS_PARATTLKEYORDERDEC:
		usRamSize = sizeof(Para.ParaTtlKeyOrderDecType);
		puchSystemRam = (UCHAR *)& Para.ParaTtlKeyOrderDecType;
		break;

	case CLASS_PARAREASONCODE:
		usRamSize = sizeof(Para.ParaReasonCode);
		puchSystemRam = ( UCHAR *)Para.ParaReasonCode;
		break;

    default:
        usRamSize = 0;
        puchSystemRam = NULL;
        PifLog(MODULE_PARA_ID, FAULT_INVALID_DATA);
        PifLog(MODULE_DATA_VALUE(MODULE_PARA_ID), usClass);
        PifLog(MODULE_LINE_NO(MODULE_PARA_ID), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT(0,"Invalid Class Code in ParaAllRead()");
//        PifAbort(MODULE_PARA_ID, FAULT_INVALID_DATA);
		return PARA_ERR_INVALID_MESSAGE;
    }

    /* check if offset is over ram size */

    if ( usStartPointer > usRamSize ) {
        return(PARA_OFFOVER_RAMSIZE);             /* error - offset is over ram size */
    } else {

        /* check if received buffer is over ram size */

        if ( usRamSize - usStartPointer > usRcvBufLen ) {  /* received buffer is not over ram size */
			memcpy(puchRcvBuffer, puchSystemRam + usStartPointer, usRcvBufLen ); /* ### Mod (2171 for Win32) V1.0 */
            *pusReturnLen = usRcvBufLen;
            return(PARA_NOTOVER_RAMSIZE);
        } else {                         /* received buffer is over ram size */
            USHORT  usCopySize = ( USHORT)(usRamSize - usStartPointer);
			memcpy(puchRcvBuffer, puchSystemRam + usStartPointer, usCopySize );
            *pusReturnLen = usCopySize;
            return(SUCCESS);
        }
    }

}

/**fhfh
;=============================================================================
;
;    Synopsis: SHORT ParaAllWrite( UCHAR uchClass,
;                                  UCHAR *puchWrtBuffer,
;                                  USHORT usWrtBufLen,
;                                  USHORT usStartPointer,
;                                  USHORT *pusReturnLen )
;
;       input: uchClass
;              *puchWrtBuffer
;               usRWrtBufLen
;               usStartPointer
;      output: *pusReturnLen
;
;      Return: SUCCESS               : Successful
;              PARA_OFFOVER_RAMSIZE  : Offset is over RAM SIZE
;
;
; Descruption:  This function writes SYSTEM RAM.
:
:               WARNING:  If you add new types of data you must also
:                         consider changing the data request tables
:                         in file nwop.c, especially table auchClassNoAll[]
:                         so that when a terminal is requesting parameter data
:                         from the Master Terminal it will receive the new data.
:                         See function OpResAll() and server message request
:                         code CLI_FCOPREQALL for details as to parameter 
:                         data broadcast.
;
;===============================================================================
fhfh**/

SHORT ParaAllWrite( USHORT usClass,
                    VOID *puchWrtBuffer,
                    USHORT usWrtBufLen,
                    USHORT usStartPointer,
                    USHORT *pusReturnLen )
{

    USHORT usRamSize;
    UCHAR FAR *puchSystemRam;

    /* distinguish data class */

    switch(usClass) {
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
        puchSystemRam = ( UCHAR *)ParaPromotion[0];
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
        /* usRamSize = sizeof(ParaFSC1); */
        /* puchSystemRam = ( UCHAR FAR *)ParaFSC1; */
		//MWS Keyboard Issue
		//For the FSC table, after build 2.0.0.58, we will only allow
		//Menu pages 1-9 to be available for modification.
		usRamSize = (sizeof(ParaFSC) - (sizeof(PARAFSCTBL) * 2));
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
        usRamSize = sizeof(Para.ParaTransMnemo);
        puchSystemRam = ( UCHAR *)Para.ParaTransMnemo[0];
        break;

    case CLASS_PARALEADTHRU:
        usRamSize = sizeof(Para.ParaLeadThru);
        puchSystemRam = ( UCHAR *)Para.ParaLeadThru[0];
        break;

    case CLASS_PARAREPORTNAME:
        usRamSize = sizeof(ParaReportName);
        puchSystemRam = ( UCHAR *)ParaReportName[0];
        break;

    case CLASS_PARASPECIALMNEMO:
        usRamSize = sizeof(Para.ParaSpeMnemo);
        puchSystemRam = ( UCHAR *)Para.ParaSpeMnemo[0];
        break;

    case CLASS_PARATAXTBL1:
    case CLASS_PARATAXTBL2:
    case CLASS_PARATAXTBL3:
    case CLASS_PARATAXTBL4:
        usRamSize = sizeof(Para.ParaTaxTable);
        puchSystemRam = Para.ParaTaxTable;
        break;

    case CLASS_PARAADJMNEMO:
        usRamSize = sizeof(Para.ParaAdjMnemo);
        puchSystemRam = ( UCHAR *)Para.ParaAdjMnemo[0];
        break;

    case CLASS_PARAPRTMODI:
        usRamSize = sizeof(ParaPrtModi);
        puchSystemRam = ( UCHAR *)ParaPrtModi[0];
        break;

    case CLASS_PARAMAJORDEPT:
        usRamSize = sizeof(ParaMajorDEPT);
        puchSystemRam = ( UCHAR *)ParaMajorDEPT[0];
        break;

    case CLASS_PARACHAR24:
        usRamSize = sizeof(Para.ParaChar24);
        puchSystemRam = ( UCHAR *)Para.ParaChar24[0];
        break;

    case CLASS_PARACTLTBLMENU:
        usRamSize = sizeof(ParaCtlTblMenu);
        puchSystemRam = ParaCtlTblMenu;
        break;

    case CLASS_PARAAUTOALTKITCH:
        usRamSize = sizeof(ParaAutoAltKitch);
        puchSystemRam = ParaAutoAltKitch;
        break;

    case CLASS_PARATEND:
        usRamSize = sizeof(ParaTend);               /* V3.3 */
        puchSystemRam = ParaTend;
        break;

    case CLASS_TENDERKEY_INFO:
#pragma message("Need to complete CLASS_TENDERKEY_INFO in ParaAllWrite.\z")
        usRamSize = sizeof(ParaTenderKeyInformation);               /* V3.3 */
        puchSystemRam = ( UCHAR *)ParaTenderKeyInformation;
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
        usRamSize = sizeof(Para.ParaCashABAssign);
        puchSystemRam = ( UCHAR *)Para.ParaCashABAssign;    /* USHORT -> UCHAR */
        break;

    case CLASS_PARASPCCO:
        usRamSize = sizeof(Para.ParaSpcCo);
        puchSystemRam = ( UCHAR *)Para.ParaSpcCo;           /* USHORT -> UCHAR */
        break;

    case CLASS_PARAPCIF:
        usRamSize = sizeof(ParaPCIF);
        puchSystemRam = ( UCHAR *)ParaPCIF;
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

    case CLASS_PARAPIGRULE:                             /* A/C 130 */
        usRamSize = sizeof(ParaPigRule);
        puchSystemRam = ( UCHAR FAR *)ParaPigRule;      /* ULONG -> UCHAR */
        break;

    case CLASS_PARASOFTCHK:                             /* A/C 87 */
        usRamSize = sizeof(ParaSoftChk);
        puchSystemRam = ( UCHAR *)ParaSoftChk[0];
        break;

    case CLASS_PARAHOTELID:                             /* Prog# 54 */
        usRamSize = sizeof(Para.ParaHotelId);
        puchSystemRam = ( UCHAR *)&Para.ParaHotelId;
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

	case CLASS_PARACOLORPALETTE:
		usRamSize = sizeof(ParaColorPalette);
		puchSystemRam = ( UCHAR FAR *)ParaColorPalette;
		break;

    case CLASS_PARADEPTNOMENU:
        usRamSize = sizeof(ParaDeptNoMenu);
        puchSystemRam = ( UCHAR FAR *)ParaDeptNoMenu;   /* USHORT -> UCHAR, 2172 */
        break;

    case CLASS_PARAMISCPARA:
        usRamSize = sizeof(ParaMiscPara);
        puchSystemRam = ( UCHAR FAR *)ParaMiscPara;   /* Misc para 2172 */
        break;

    case CLASS_PARASTOREFORWARD:
        usRamSize = sizeof(Para.ParaStoreForward);
        puchSystemRam = ( UCHAR *)Para.ParaStoreForward;   /* Misc para 2172 */
        break;

    case CLASS_PARATERMINALINFO:
        usRamSize = sizeof(ParaTerminalInformationPara);
        puchSystemRam = ( UCHAR FAR *)ParaTerminalInformationPara;   /* Misc para 2172 */
        break;

	case CLASS_FILEVERSION_INFO:
		usRamSize = sizeof(ParaFileVersion);
		puchSystemRam = (UCHAR *)ParaFileVersion;
		break;

	case CLASS_PARAAUTOCPN:
		usRamSize = sizeof(ParaAutoCoupon);
		puchSystemRam = ( UCHAR FAR *)&ParaAutoCoupon; /* PARAAUTOCPN -> UCHAR */
		break;
	
	case CLASS_PARATTLKEYORDERDEC:
		usRamSize = sizeof(Para.ParaTtlKeyOrderDecType);
		puchSystemRam = (UCHAR *)& Para.ParaTtlKeyOrderDecType;
		break;

	case CLASS_PARAREASONCODE:
		usRamSize = sizeof(Para.ParaReasonCode);
		puchSystemRam = ( UCHAR *)Para.ParaReasonCode;
		break;

    default:
        usRamSize = 0;
        puchSystemRam = NULL;
        PifLog(MODULE_PARA_ID, FAULT_INVALID_DATA);
        PifLog(MODULE_DATA_VALUE(MODULE_PARA_ID), usClass);
        PifLog(MODULE_LINE_NO(MODULE_PARA_ID), (USHORT)__LINE__);
		NHPOS_ASSERT_TEXT(0,"Invalid Class Code in ParaAllWrite()");
//        PifAbort(MODULE_PARA_ID, FAULT_INVALID_DATA);
		return PARA_ERR_INVALID_MESSAGE;
    }

    /* check if offset is over ram size */
	if (usClass == CLASS_PARAFSC) {
		// When copying the PARAFSC class we are simulating the copy of 11 menu
		// pages when in actuality we are only copying the first 9 menu pages.
		// We do this by copying the first 9 pages, incrementing the starting write
		// pointer in the buffer to the size of the write. After the 9 pages, we
		// return the length that is supposed to be written, just as long as it does
		// not exceed the size of the simulated write size, 11 menu pages. If the
		// start point + the size of the write is supposed to go past the buffer
		// length, we return the buffer length - the point inside of the buffer we
		// are writting. This size will always me smaller than the size that is
		// supposed to be written.
		if ( usStartPointer >= usRamSize ) {
			if ( usStartPointer < sizeof(ParaFSC) ) {
				if ((usStartPointer + usWrtBufLen) > sizeof(ParaFSC)) {
					*pusReturnLen = (sizeof(ParaFSC) - usStartPointer);
				} else {
					*pusReturnLen = usWrtBufLen;
				}
			} else {
				return(PARA_OFFOVER_RAMSIZE);             /* error - offset is over ram size */
			}
		} else {
			if ( usRamSize - usStartPointer > usWrtBufLen ) {  /* write buffer is not over ram size */
				memcpy( puchSystemRam + usStartPointer, puchWrtBuffer, usWrtBufLen );
				*pusReturnLen = usWrtBufLen;
			} else {                         /* write buffer is over ram size */
                USHORT  usCopySize = ( USHORT)(usRamSize - usStartPointer);
				memcpy( puchSystemRam + usStartPointer, puchWrtBuffer, usCopySize );
				*pusReturnLen = usWrtBufLen;
			}
		}
	} else {
		if ( usStartPointer > usRamSize ) {
			return(PARA_OFFOVER_RAMSIZE);             /* error - offset is over ram size */
		} else {

			/* check if write buffer is over ram size */

			if ( usRamSize - usStartPointer > usWrtBufLen ) {  /* write buffer is not over ram size */
				memcpy( puchSystemRam + usStartPointer, puchWrtBuffer, usWrtBufLen );
				*pusReturnLen = usWrtBufLen;
			} else {                         /* write buffer is over ram size */
                USHORT  usCopySize = ( USHORT)(usRamSize - usStartPointer);
				memcpy( puchSystemRam + usStartPointer, puchWrtBuffer, usCopySize );
				*pusReturnLen = usCopySize;
			}
		}
	}

	// Do final steps to ensure that the ParaTerminalInformation is congruent
	// with the written information.
	switch (usClass) {
		case CLASS_PARASTOREGNO :
#if 0
			// Code if defed out as we do not want to override this terminal's
			// store number and register number with that for the Master Terminal.
			// Requested by Walter Rose, NCR Professional Services, US Government Division
			// Richard Chambers,  November 26, 2008
			{
				USHORT usTerminalPosition;
				SYSCONFIG *pSysConfig = PifSysConfig();

				usTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

				// guard the terminal position to ensure it is in correct range, default is we be Master
				if (usTerminalPosition < 1 || usTerminalPosition > PIF_NET_MAX_IP) {
					usTerminalPosition = 1;
				}

//				ParaTerminalInformationPara[usTerminalPosition - 1].usStoreNumber = ParaStoRegNo[0];
//				ParaTerminalInformationPara[usTerminalPosition - 1].usRegisterNumber = ParaStoRegNo[1];
			}
#endif
		break;

		case CLASS_PARAHOTELID :
			{
				int j;
				for (j = 0; j < MAX_TEND_NO; j++) {
					ParaTenderKeyInformation[j].auchEptTend = Para.ParaHotelId.auchTend[j];
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
					Para.ParaHotelId.auchTend[j] = ParaTenderKeyInformation[j].auchEptTend;
				}
			}
			break;

		case CLASS_PARATERMINALINFO:
			{
				USHORT usTerminalPosition;
				const SYSCONFIG *pSysConfig = PifSysConfig();

				usTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);

				// guard the terminal position to ensure it is in correct range, default is we be Master
				if (usTerminalPosition < 1 || usTerminalPosition > PIF_NET_MAX_IP) {
					usTerminalPosition = 1;
				}
				ParaCtlTblMenu[0] = ParaTerminalInformationPara[usTerminalPosition - 1].uchMenuPageDefault;
			}
			break;



		default:
			break;
	}
    return(SUCCESS);                    /* perform */
}

/****** End of Source ******/
