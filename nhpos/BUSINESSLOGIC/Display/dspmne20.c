/*
		Copyright	Georgia Southern University
					Statesboro,  GA 30460

					NCR Project Development Team
*===========================================================================
* Title       : Mnemonics Setting( SUPER & PROGRAM MODE )                       
* Category    : Display-2x20, NCR 2170 US Hospitality Application Program        
* Program Name: DSPMNE20.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*               
*               DispSupMnemo20()  : Display Mnemonics
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jul-04-92: 00.00.01 : K.You     : initial                                   
* Aug-23-93: 01.00.13 : J.IKEDA   : Add PARASOFTCHK, Del PARACHR44                                   
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
=============================================================================
**/
#include	<tchar.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "pif.h"
/* #include <log.h> */
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include <appllog.h>
#include "display.h"

#include "disp20.h"


/*
*===========================================================================
** Synopsis:    VOID DispSupMnemo20( VOID *pData )
*               
*       Input:  *pData          : Pointer to Structure for PARAPROMOTION, PARATRANSMNEMO
*                                                          PARALEADTHRU, PARAREPORTNAME
*                                                          PARASPECIALMNEMO, PARAADJMNEMO
*                                                          PARAPRTMODI, PARAMAJORDEPT
*                                                          PARACHAR24,PARAPCIF,PARASOFTCHK
*      Output:  Nothing 
*
** Return:      Nothing
*
** Description: This function displays Mnemonics Data 
*===========================================================================
*/

VOID DispSupMnemo20( VOID *pData )
{
    /* Define Display Format */
    static const TCHAR  auchDispSupMnemo20A[] = _T("%3u  %3u\t%10s\n%-19s%s");
    static const TCHAR  auchDispSupMnemo20B[] = _T("%3u  %3u\t%10s\n%-19s%s");
    static const TCHAR  auchDispSupMnemo20AL[] = _T("%3u  %3u\t%10s\n%-39s%s");
    static const TCHAR  auchDispSupMnemo20BL[] = _T("%3u  %3u\t%10s\n%-39s%s");
    static const TCHAR  auchDispSupMnemo20C[] = _T("%3u  %3u\t%10s\n%-20s");
    static const TCHAR  auchDispSupMnemo20D[] = _T("%3u  %3u\t%10s\n%-20s");
    static const TCHAR  auchDispSupMnemo20CL[] = _T("%3u  %3u\t%10s\n%-40s");
    static const TCHAR  auchDispSupMnemo20DL[] = _T("%3u  %3u\t%10s\n%-40s");


	TCHAR  aszBuffer[PARA_SOFTCHK_LEN + 1] = { 0 };
    TCHAR  aszDspNull[1] = {0};
    TCHAR  aszDspPage[4] = {0};
    USHORT usAddress = 0;
    USHORT usNoData = 0;
                        
    PARAPROMOTION       * const pParaPromotion = pData;
    PARATRANSMNEMO      * const pParaTransMnemo = pData;
    PARALEADTHRU        * const pParaLeadThru = pData;
    PARAREPORTNAME      * const pParaReportName = pData;
    PARASPEMNEMO        * const pParaSpecialMnemo = pData;
    PARAADJMNEMO        * const pParaAdjMnemo = pData;
    PARAPRTMODI         * const pParaPrtModi = pData;
    PARAMAJORDEPT       * const pParaMajorDEPT = pData;
    PARACHAR24          * const pParaChar24 = pData;
    PARAPCIF            * const pParaPCIF = pData;
    PARASOFTCHK         * const pParaSoftChk = pData;
    PARAHOTELID         * const pParaHotelId = pData;
    PARAMLDMNEMO        * const pParaMldMnemo = pData;

 
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_PROMOTION_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_TRANSMNEMO_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_LEADTHRU_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_REPORTNAME_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_SPEMNEMO_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_ADJMNEMO_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_PRTMODI_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_MAJORDEPT_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_CHAR24_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_PCIF_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_SOFTCHK_LEN);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, MAX_HOTELID_SIZE);
	NHPOS_ASSERT_ARRAYSIZE(aszBuffer, PARA_MLDMNEMO_LEN);


	NHPOS_ASSERT(uchMaintMenuPage < 99);

    /* set super/program number */
    switch(pParaPromotion->uchMajorClass) {
    case CLASS_PARAPROMOTION:
        usAddress = pParaPromotion->uchAddress;             /* set Address */
        usNoData = AC_SALES_PROMOTION;                      /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaPromotion->aszProSales, PARA_PROMOTION_LEN);     /* Set Mnemonics */
        DispSupSaveData.Promotion = *pParaPromotion;       /* Save Data for redisplay if needed */   
        _itot(uchMaintMenuPage, aszDspPage, 10);
        break;                                                                  

    case CLASS_PARATRANSMNEMO:
        usAddress = pParaTransMnemo->uchAddress;        /* set Address */
        usNoData = PG_TRAN_MNEMO;                       /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaTransMnemo->aszMnemonics, PARA_TRANSMNEMO_LEN);  /* Set Mnemonics */
        DispSupSaveData.TransMnemo = *pParaTransMnemo;  /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARALEADTHRU:
        usAddress = pParaLeadThru->uchAddress;          /* set Address */
        usNoData = PG_LEAD_MNEMO;                       /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaLeadThru->aszMnemonics, PARA_LEADTHRU_LEN);  /* Set Mnemonics */
        DispSupSaveData.LeadThru = *pParaLeadThru;      /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARAREPORTNAME:
        usAddress = pParaReportName->uchAddress;        /* set Address */
        usNoData = PG_RPT_NAME;                         /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaReportName->aszMnemonics, PARA_REPORTNAME_LEN);  /* Set Mnemonics */
        DispSupSaveData.ReportName = *pParaReportName;  /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARASPECIALMNEMO:
        usAddress = pParaSpecialMnemo->uchAddress;      /* set Address */
        usNoData = PG_SPEC_MNEMO;                       /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaSpecialMnemo->aszMnemonics, PARA_SPEMNEMO_LEN);  /* Set Mnemonics */
        DispSupSaveData.SpeMnemo = *pParaSpecialMnemo;   /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARAADJMNEMO:
        usAddress = pParaAdjMnemo->uchAddress;           /* set Address */
        usNoData = PG_ADJ_MNEMO;                         /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaAdjMnemo->aszMnemonics, PARA_ADJMNEMO_LEN);  /* Set Mnemonics */
        DispSupSaveData.AdjMnemo = *pParaAdjMnemo;       /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARAPRTMODI:
        usAddress = pParaPrtModi->uchAddress;           /* set Address */
        usNoData = PG_PRT_MNEMO;                        /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaPrtModi->aszMnemonics, PARA_PRTMODI_LEN);    /* Set Mnemonics */
        DispSupSaveData.PrtModi = *pParaPrtModi;        /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARAMAJORDEPT:
        usAddress = pParaMajorDEPT->uchAddress;             /* set Address */
        usNoData = PG_MAJDEPT_MNEMO;                        /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaMajorDEPT->aszMnemonics, PARA_MAJORDEPT_LEN);    /* Set Mnemonics */
        DispSupSaveData.MajorDEPT = *pParaMajorDEPT;        /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARACHAR24:
        usAddress = pParaChar24->uchAddress;        /* set Address */
        usNoData = PG_CHA24_MNEMO;                  /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaChar24->aszMnemonics, PARA_CHAR24_LEN);  /* Set Mnemonics */
        DispSupSaveData.Char24 = *pParaChar24;      /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARAPCIF:
        usAddress = pParaPCIF->uchAddress;      /* set Address */
        usNoData = PG_PCIF;                     /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaPCIF->aszMnemonics, PARA_PCIF_LEN);  /* Set Mnemonics */
        DispSupSaveData.PCIF = *pParaPCIF;      /* Save Data for Redisplay if needed. */   
        break;
 
    case CLASS_PARASOFTCHK:
        usAddress = pParaSoftChk->uchAddress;            /* Set Address */
        usNoData = AC_SOFTCHK_MSG;                       /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaSoftChk->aszMnemonics, PARA_SOFTCHK_LEN);     /* Set Mnemonics */
        DispSupSaveData.SoftChk = *pParaSoftChk;         /* Save Data for Redisplay if needed. */   
        _itot(uchMaintMenuPage, aszDspPage, 10);
        break;                                                                  

    case CLASS_PARAHOTELID:                                                  
        usAddress = pParaHotelId->uchAddress;                /* Set Address */
        usNoData  = PG_HOTELID;                              /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaHotelId->aszMnemonics, MAX_HOTELID_SIZE);     /* Set Mnemonics */
        DispSupSaveData.ParaHotelId = *pParaHotelId;         /* Save Data for Redisplay if needed. */   
        break;

    case CLASS_PARAMLDMNEMO:                               /* V3.3 */
        usAddress = pParaMldMnemo->usAddress;              /* Set Address */
        usNoData  = PG_MLD_MNEMO;                          /* Set Program,A/C No. */
        _tcsncpy(aszBuffer, pParaMldMnemo->aszMnemonics, PARA_MLDMNEMO_LEN);     /* Set Mnemonics */
        DispSupSaveData.MldMnemo = *pParaMldMnemo;         /* Save Data for Redisplay if needed. */   
        break;

    default:
        PifLog(MODULE_DISPSUP_ID, LOG_EVENT_UI_DISPLAY_ERR_04);
		return;
        break;

    }
#define SINGLE_WIDTH
#if defined(SINGLE_WIDTH)
    if (PifSysConfig()->uchOperType == DISP_LCD) {
        /* Display ForeGround For LCD */

        switch(pParaPromotion->uchMajorClass) {
        case CLASS_PARAPROMOTION:
        case CLASS_PARASOFTCHK:
        case CLASS_PARAMLDMNEMO:
        /* display with shift page */

            UiePrintf(UIE_OPER,                     /* Operator Display */
                      0,                            /* Row */
                      0,                            /* Column */
                      auchDispSupMnemo20AL,         /* Format */
                      usNoData,                     /* Super/Program Number */
                      usAddress,                    /* Address */
                      aszDspNull,                   /* Null */
                      aszBuffer,                    /* Mnemonics Data */
                      aszDspPage);                  /* Page Number */

            /* Display BackGound */
            UieEchoBackGround(UIE_KEEP,             /* Keep Display */
                              UIE_ATTR_NORMAL,      /* Attribute */
                              auchDispSupMnemo20BL, /* Format */
                              usNoData,             /* Super/Program number */
                              usAddress,            /* Address */
                              aszDspNull,           /* Null */
                              aszDspNull,           /* Null */
                              aszDspPage);          /* Page Number */
            break;

        default:
            UiePrintf(UIE_OPER,                     /* Operator Display */
                      0,                            /* Row */
                      0,                            /* Column */
                      auchDispSupMnemo20CL,         /* Format */
                      usNoData,                     /* Super/Program Number */
                      usAddress,                    /* Address */
                      aszDspNull,                   /* Null */
                      aszBuffer);                   /* Mnemonics Data */

            /* Display BackGound */
            UieEchoBackGround(UIE_KEEP,             /* Keep Display */
                              UIE_ATTR_NORMAL,      /* Attribute */
                              auchDispSupMnemo20DL, /* Format */
                              usNoData,             /* Super/Program number */
                              usAddress,            /* Address */
                              aszDspNull,           /* Null */
                              aszDspNull);          /* Null */
        }
    } else {
        /* Display ForeGround For 2X20 */

        switch(pParaPromotion->uchMajorClass) {
        case CLASS_PARAPROMOTION:
        case CLASS_PARASOFTCHK:
        /* display with shift page */

            UiePrintf(UIE_OPER,                     /* Operator Display */
                      0,                            /* Row */
                      0,                            /* Column */
                      auchDispSupMnemo20A,          /* Format */
                      usNoData,                     /* Super/Program Number */
                      usAddress,                    /* Address */
                      aszDspNull,                   /* Null */
                      aszBuffer,                    /* Mnemonics Data */
                      aszDspPage);                  /* Page Number */

            /* Display BackGound */
            UieEchoBackGround(UIE_KEEP,             /* Keep Display */
                              UIE_ATTR_NORMAL,      /* Attribute */
                              auchDispSupMnemo20B,  /* Format */
                              usNoData,             /* Super/Program number */
                              usAddress,            /* Address */
                              aszDspNull,           /* Null */
                              aszDspNull,           /* Null */
                              aszDspPage);          /* Page Number */
            break;

        default:
            UiePrintf(UIE_OPER,                     /* Operator Display */
                      0,                            /* Row */
                      0,                            /* Column */
                      auchDispSupMnemo20C,          /* Format */
                      usNoData,                     /* Super/Program Number */
                      usAddress,                    /* Address */
                      aszDspNull,                   /* Null */
                      aszBuffer);                   /* Mnemonics Data */

            /* Display BackGound */
            UieEchoBackGround(UIE_KEEP,             /* Keep Display */
                              UIE_ATTR_NORMAL,      /* Attribute */
                              auchDispSupMnemo20D,  /* Format */
                              usNoData,             /* Super/Program number */
                              usAddress,            /* Address */
                              aszDspNull,           /* Null */
                              aszDspNull);          /* Null */
        }
    }
#else
    /* display text on both 2x20 and LCD - RFC 01/30/2001 */

    switch(pParaPromotion->uchMajorClass) {
    case CLASS_PARAPROMOTION:
    case CLASS_PARASOFTCHK:
    /* display with shift page */

        UiePrintf(UIE_OPER,                     /* Operator Display */
                  0,                            /* Row */
                  0,                            /* Column */
                  auchDispSupMnemo20A,          /* Format */
                  usNoData,                     /* Super/Program Number */
                  usAddress,                    /* Address */
                  aszDspNull,                   /* Null */
                  aszBuffer,                    /* Mnemonics Data */
                  aszDspPage);                  /* Page Number */

        /* Display BackGound */
        UieEchoBackGround(UIE_KEEP,             /* Keep Display */
                          UIE_ATTR_NORMAL,      /* Attribute */
                          auchDispSupMnemo20B,  /* Format */
                          usNoData,             /* Super/Program number */
                          usAddress,            /* Address */
                          aszDspNull,           /* Null */
                          aszDspNull,           /* Null */
                          aszDspPage);          /* Page Number */
        break;

    default:
        UiePrintf(UIE_OPER,                     /* Operator Display */
                  0,                            /* Row */
                  0,                            /* Column */
                  auchDispSupMnemo20C,          /* Format */
                  usNoData,                     /* Super/Program Number */
                  usAddress,                    /* Address */
                  aszDspNull,                   /* Null */
                  aszBuffer);                   /* Mnemonics Data */

        /* Display BackGound */
        UieEchoBackGround(UIE_KEEP,             /* Keep Display */
                          UIE_ATTR_NORMAL,      /* Attribute */
                          auchDispSupMnemo20D,  /* Format */
                          usNoData,             /* Super/Program number */
                          usAddress,            /* Address */
                          aszDspNull,           /* Null */
                          aszDspNull);          /* Null */
    }
#endif
}
