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
* Title       : Multiline Display Set Descriptor Mnemonics
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDDESC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      MldSetDescriptor(): set desctptor mnemonic
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jul-25-95 : 03.00.03 :  M.Ozawa   : Initial
* May-08-96 : 03.01.05 :  M.Ozawa   : Change from CliGetEJStatus() to PrtGetStatus()
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbstb.h>
#include <prt.h>
#include <uie.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "termcfg.h"
/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/

/* ---- Register Mode without E/J Descriptor Definitions ---- */

/*
		WARNING: The descriptors are positioned as to how they show on the
		descriptor area of the display.  They are in two rows just as they
		would display in the descriptor area.  The position in the TCHAR
		arrays is the position of the text in the displayed descriptor area.
		Each row currently contains 46 (see UIE_LCD_COLUMN2) characters numbered 0 through 45 for the first
		row and 46 through 91 for the second row.
 */
/* descriptor display position */
#define chars_in_Reg 46 // Number of characters in first line of indicator display, szRegDescName[] below.
						// If number of characters is changed, this number must be updated so that the second line
						// will be displayed correctly.
UIEDESCLCD FARCONST aRegInfo[] = {{0,3},				/* COM   (0)  */
                                 {chars_in_Reg,3},		/* MAS   (1)  */
                                 {4,3},					/* B-M   (2)  */
                                 {chars_in_Reg+4,3},	/* PRT   (3)  */
                                 {9,5},					/* NW/ST (4)  */
                                 {chars_in_Reg+7,5},	/* TRAIN (5)  */
                                 {15,5},				/* RE/PB (6)  */
                                 {chars_in_Reg+12,5},	/* TX/TR (7)  */
                                 {20,3},				/* PAY   (8)  */
                                 {chars_in_Reg+18,3},	/* %/-   (9)  */
                                 {23,4},				/* VOID  (10) */
                                 {chars_in_Reg+22,5},	/* CR/AL (11) */
                                 {27,6},				/* T-VOID(12) */
                                 {chars_in_Reg+28,5},	/* RECPT (13) */
                                 {34,3},				/* TTL   (14) */
                                 {chars_in_Reg+34,3},	/* CHG   (15) */
								 {37,3},				/* KDS   (16) SR 236 JHHJ*/
								 {chars_in_Reg+37,3},	/* D-B	 (17) */
								 {41,3},				/* 	 (18)  S-F, _2X20_SF */
                                 {chars_in_Reg+40,3},	/*   (19) DSC - Disconnected Satellite, _2X20_DISCON_SAT  */
                                 {chars_in_Reg+43,3},	/*   (20) DTTL - Disconnected Satellite Totals, _2X20_DISCON_SAT_TOTALS  */
								 {44, 1}				/* 	 (21)  * - Store and Forward is transferring, _2X20_SF_TRANSFERRING */
							};

/* descriptor mnemonics */
TCHAR FARCONST szRegDescName[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                 _T("MAS PRTTRAINTX/TR %/- CR/AL RECPT CHGD-B      "); 

TCHAR FARCONST szRegDescNameDisSat[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                       _T("MAS PRTTRAINTX/TR %/- CR/ALDISPLAYCHGD-BDCSTTL"); 

/* ---- Register Mode with E/J Descriptor Definitions ---- */

/* descriptor display position */
#define chars_in_RegEJ 46	// Number of characters in first line of indicator display, szRegEJDescName[] below.
							// If number of characters is changed, this number must be updated so that the second line
							// will be displayed correctly.
UIEDESCLCD FARCONST aRegEJInfo[] = {{0,3},				/* COM   (0)  */
                                 {chars_in_RegEJ,3},	/* MAS   (1)  */
                                 {4,3},					/* B-M   (2)  */
                                 {chars_in_RegEJ+4,3},	/* RMP/EJ(3)  */
                                 {9,5},					/* NW/ST (4)  */
                                 {chars_in_RegEJ+7,5},	/* TRAIN (5)  */
                                 {15,5},				/* RE/PB (6)  */
                                 {chars_in_RegEJ+12,5},	/* TX/TR (7)  */
                                 {20,3},				/* PAY   (8)  */
                                 {chars_in_RegEJ+18,3},	/* %/-   (9)  */
                                 {23,4},				/* VOID  (10) */
                                 {chars_in_RegEJ+22,5},	/* CR/AL (11) */
                                 {27,6},				/* T-VOID(12) */
                                 {chars_in_RegEJ+28,5},	/* RECPT (13) */
                                 {34,3},				/* TTL   (14) */
                                 {chars_in_RegEJ+34,3},	/* CHG   (15) */
								 {37,3},				/* KDS   (16) SR 236 JHHJ*/
								 {chars_in_RegEJ+37,3},	/* D-B	 (17) Delayed Balance JHHJ*/
								 {41,3},				/* 	 (18)  S-F, _2X20_SF */
                                 {chars_in_Reg+40,3},	/*   (19) DSC - Disconnected Satellite, _2X20_DISCON_SAT  */
                                 {chars_in_Reg+43,3},	/*   (20) DTTL - Disconnected Satellite Totals, _2X20_DISCON_SAT_TOTALS  */
								 {44, 1}				/* 	 (21)  * - Store and Forward is transferring, _2X20_SF_TRANSFERRING */
							};
								 

/* descriptor mnemonics */
TCHAR FARCONST szRegEJDescName[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                   _T("MAS PRTTRAINTX/TR %/- CR/AL RECPT CHGD-B      "); 

TCHAR FARCONST szRegEJDescNameDisSat[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                         _T("MAS PRTTRAINTX/TR %/- CR/ALDISPLAYCHGD-BDCSTTL"); 

/* ---- Supervisor Mode without E/J Descriptor Definitions ---- */

/* descriptor display position */
#define chars_in_Sup 46	// Number of characters in first line of indicator display, szSupDescName[] below.
						// If number of characters is changed, this number must be updated so that the second line
						// will be displayed correctly.
UIEDESCLCD FARCONST aSupInfo[] = {{0,3},				/* COM   (0)  */
                                 {chars_in_Sup,3},		/* MAS   (1)  */
                                 {4,3},					/* B-M   (2)  */
                                 {chars_in_Sup+4,3},	/* RM-P  (3)  */
                                 {9,5},					/* NW/ST (4)  */
                                 {chars_in_Sup+7,5},	/* TRAIN (5)  */
                                 {15,5},				/* RE/PB (6)  */
                                 {chars_in_Sup+12,5},	/* TX/TR (7)  */
                                 {20,3},				/* PAY   (8)  */
                                 {chars_in_Sup+18,3},	/* %/-   (9)  */
                                 {23,4},				/* VOID  (10) */
                                 {chars_in_Sup+22,5},	/* CR/AL (11) */
                                 {27,6},				/* T-VOID(12) */
                                 {chars_in_Sup+27,7},	/* DISPLAY (13) */
                                 {34,3},				/* TTL   (14) */
                                 {chars_in_Sup+34,3},	/* CHG   (15) */
								 {37,3},				/* KDS   (16) SR 236 JHHJ*/
								 {chars_in_Sup+37,3},	/* D-B	 (17) Delayed Balance JHHJ*/
								 {41,3},				/* 	 (18)  S-F, _2X20_SF */
                                 {chars_in_Reg+40,3},	/*   (19) DSC - Disconnected Satellite, _2X20_DISCON_SAT  */
                                 {chars_in_Reg+43,3},	/*   (20) DTTL - Disconnected Satellite Totals, _2X20_DISCON_SAT_TOTALS  */
								 {44, 1}				/* 	 (21)  * - Store and Forward is transferring, _2X20_SF_TRANSFERRING */
							};

/* descriptor mnemonics */
TCHAR FARCONST szSupDescName[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                 _T("MAS PRTTRAINTX/TR %/- CR/ALDISPLAYCHGD-B      "); 

TCHAR FARCONST szSupDescNameDisSat[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                       _T("MAS PRTTRAINTX/TR %/- CR/ALDISPLAYCHGD-BDCSTTL"); 

/* ---- Supervisor Mode with E/J Descriptor Definitions ---- */

/* descriptor display position */
#define chars_in_SupEJ 46	// Number of characters in first line of indicator display, szSupEJDescName[] below.
							// If number of characters is changed, this number must be updated so that the second line
							// will be displayed correctly.
UIEDESCLCD FARCONST aSupEJInfo[] = {{0,3},					/* COM   (0)  */
                                 {chars_in_SupEJ,3},		/* MAS   (1)  */
                                 {4,3},						/* B-M   (2)  */
                                 {chars_in_SupEJ+4,3},		/* RMP/EJ(3)  */
                                 {9,5},						/* NW/ST (4)  */
                                 {chars_in_SupEJ+7,5},		/* TRAIN (5)  */
                                 {15,5},					/* RE/PB (6)  */
                                 {chars_in_SupEJ+12,5},		/* TX/TR (7)  */
                                 {20,3},					/* PAY   (8)  */
                                 {chars_in_SupEJ+18,3},		/* %/-   (9)  */
                                 {23,4},					/* VOID  (10) */
                                 {chars_in_SupEJ+22,5},		/* CR/AL (11) */
                                 {27,6},					/* T-VOID(12) */
                                 {chars_in_SupEJ+27,7},		/* DISPLAY (13) */
                                 {34,3},					/* TTL   (14) */
                                 {chars_in_SupEJ+34,3},		/* CHG   (15) */
								 {37,3},					/* KDS   (16) SR 236 JHHJ*/
								 {chars_in_SupEJ+37,3},		/* D-B	 (17) Delayed Balance JHHJ*/
								 {41,3},					/* 	 (18)  S-F, _2X20_SF */
                                 {chars_in_Reg+40,3},		/*   (19) DSC - Disconnected Satellite, _2X20_DISCON_SAT  */
                                 {chars_in_Reg+43,3},		/*   (20) DTTL - Disconnected Satellite Totals, _2X20_DISCON_SAT_TOTALS  */
								 {44, 1}					/* 	 (21)  * - Store and Forward is transferring, _2X20_SF_TRANSFERRING */
							};

/* descriptor mnemonics */
TCHAR FARCONST szSupEJDescName[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                   _T("MAS PRTTRAINTX/TR %/- CR/ALDISPLAYCHGD-B      "); 

TCHAR FARCONST szSupEJDescNameDisSat[] = _T("COM B-M  NW/ST RE/PBPAYVOIDT-VOID TTLKDS S-F* ")\
                                         _T("MAS PRTTRAINTX/TR %/- CR/ALDISPLAYCHGD-BDCSTTL"); 

/*
*===========================================================================
** Format  : SHORT MldSetDescriptor(USHORT usMode)
*
*   Input  : 
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: Set Descriptor Name on LCD according to Mode and Configrations
*===========================================================================
*/
SHORT    MldSetDescriptor(USHORT usMode)
{
    CONST SYSCONFIG   *pSysConfig = PifSysConfig();                    /* get system config */
	int         iPos = 0, iLen = 0;
	struct _ArrayDescriptor {
		UIEDESCLCD FARCONST  *paInfo;
		TCHAR FARCONST       *pStandardTerminal;
		TCHAR FARCONST       *pDisconnectedTerminal;
	} ArrayDescriptors[4] = {
		{aSupInfo,   szSupDescName,   szSupDescNameDisSat},
		{aSupEJInfo, szSupEJDescName, szSupEJDescNameDisSat},
		{aRegInfo,   szRegDescName,   szRegDescNameDisSat},
		{aRegEJInfo, szRegEJDescName, szRegEJDescNameDisSat}
	};


    PifRequestSem(husMldSemaphore);

    /* set descriptor position and mnemonic */
    if (usMode == MLD_SUPER_MODE) {
        if (PrtGetStatus() != PRT_THERMAL_PRINTER) {        /* Not Thermal System */
            /* ---- supervisor without ej system ---- */
			iPos = 0;
			iLen = sizeof(aSupInfo)/sizeof(aSupInfo[0]);
        } else {
            /* ---- supervisor with ej system ---- */
			iPos = 1;
			iLen = sizeof(aSupEJInfo)/sizeof(aSupEJInfo[0]);
        }
    } else {
        if (PrtGetStatus() != PRT_THERMAL_PRINTER) {        /* Not Thermal System */
            /* ---- register without ej system ---- */
			iPos = 2;
			iLen = sizeof(aRegInfo)/sizeof(aRegInfo[0]);
        } else {
            /* ---- register with ej system ---- */
			iPos = 3;
			iLen = sizeof(aRegEJInfo)/sizeof(aRegEJInfo[0]);
        }
    }

	if ((pSysConfig->usTerminalPosition & (PIF_CLUSTER_DISCONNECTED_SAT | PIF_CLUSTER_JOINED_SAT)) == PIF_CLUSTER_DISCONNECTED_SAT)
		UieSetDescriptor(ArrayDescriptors[iPos].paInfo, ArrayDescriptors[iPos].pDisconnectedTerminal, iLen);
	else
		UieSetDescriptor(ArrayDescriptors[iPos].paInfo, ArrayDescriptors[iPos].pStandardTerminal, iLen);

    PifReleaseSem(husMldSemaphore);

    return (MLD_SUCCESS);
}

/**** End of File ****/
