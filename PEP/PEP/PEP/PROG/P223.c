/*
* ---------------------------------------------------------------------------
* Title         :   Report MDC Settings Wizard
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P223.C
* Copyright (C) :   2019, Georgia Southern University, All rights reserved.
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Feb-06-19 : 02.02.02 : R.Chambers : Initial
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#define _WIN32_WINNT 0x0400

#include    <Windows.h>
#include    <Windowsx.h>
#include    <string.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>

#include    "prog.h"
#include    "pepflag.h"
#include    "pepcomm.h"
#include    "p020.h"

#define  P233_TYPE_MDC_P20MNEMONIC    0x0001
#define  P233_TYPE_MDC_ONLY           0x0002

typedef struct {
	USHORT  usMdcAddr;   // if 0 then always on, otherwise MDC address for enable/disable on report
	USHORT  usMdcBit;    // if 0 then always on, otherwise MDC bit for enable/disable on report
	USHORT  usMnemType;  // type of mnemonic, P20, etc.
	USHORT  usMnemAddr;  // mnemonic address
} P223_Rpt;

typedef struct {
	P223_Rpt  *table;
	SCROLLINFO scrollInfo;
} P223_Rpt_Data;

static int          unTopIndex;    /* Zero Based Top Index No. of ListBox    */
static int          unBottomIndex; /* Zero Based Bottom Index No. of ListBox */

static  HGLOBAL hGlbMnemo;          /* Handle of Global Area (Mnemonic) */
static  HGLOBAL hGlbWork;           /* Handle of Global Area (Work) */

static LPP20MNEMO  lpMnemonic;    /* Points to Global Area of Mnemonic Data */
static LPP20WORK   lpWork;        /* Points to Global Area of Work Area     */

static P223_Rpt FinancialRpt [] = {
	{MDC_RPTFIN1_ADR, ODD_MDC_BIT0,  0x0001, TRN_CURGGT_ADR},
	{MDC_RPTFIN1_ADR, ODD_MDC_BIT1,  0x0001, TRN_NETTTL_ADR},
	{MDC_RPTFIN2_ADR, EVEN_MDC_BIT2, 0x0002, 0},
	{0, 0, 0x0001, TRN_DAIGGT_ADR},                                  // always on. no MDC
	{MDC_RPTFIN1_ADR, ODD_MDC_BIT2,  0x0001, TRN_TRNGGT_ADR},
	{MDC_RPTFIN1_ADR, ODD_MDC_BIT3,  0x0001, TRN_PLUSVD_ADR},
	{MDC_RPTFIN2_ADR, EVEN_MDC_BIT0, 0x0001, TRN_PSELVD_ADR},
	{MDC_RPTFIN46_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TRETURN1_ADR},
	{MDC_RPTFIN46_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TRETURN2_ADR},
	{MDC_RPTFIN46_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TRETURN3_ADR},
	{MDC_RPTFIN2_ADR, EVEN_MDC_BIT1, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTFIN2_ADR, EVEN_MDC_BIT2, 0x0002, 0},                     // report feed, RptFeed()
	{MDC_RPTFIN2_ADR, EVEN_MDC_BIT3, 0x0001, TRN_CONSCPN_ADR},
	{MDC_RPTFIN3_ADR, ODD_MDC_BIT0, 0x0001, TRN_ITMDISC_ADR},         // also check MDC_MODID13_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN3_ADR, ODD_MDC_BIT1, 0x0001, TRN_MODID_ADR},           // also check MDC_MODID23_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN45_ADR, EVEN_MDC_BIT0, 0x0001, TRN_ITMDISC_ADR_3},
	{MDC_RPTFIN45_ADR, EVEN_MDC_BIT1, 0x0001, TRN_ITMDISC_ADR_4},
	{MDC_RPTFIN45_ADR, EVEN_MDC_BIT2, 0x0001, TRN_ITMDISC_ADR_5},
	{MDC_RPTFIN45_ADR, EVEN_MDC_BIT3, 0x0001, TRN_ITMDISC_ADR_6},
	{MDC_RPTFIN3_ADR, ODD_MDC_BIT2, 0x0001, TRN_RDISC1_ADR},           // also check MDC_RDISC13_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN3_ADR, ODD_MDC_BIT3, 0x0001, TRN_RDISC2_ADR},           // also check MDC_RDISC23_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN29_ADR, ODD_MDC_BIT1, 0x0001, TRN_RDISC3_ADR},           // also check MDC_RDISC33_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN29_ADR, ODD_MDC_BIT2, 0x0001, TRN_RDISC4_ADR},           // also check MDC_RDISC43_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN29_ADR, ODD_MDC_BIT3, 0x0001, TRN_RDISC5_ADR},           // also check MDC_RDISC53_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN30_ADR, EVEN_MDC_BIT0, 0x0001, TRN_RDISC6_ADR},          // also check MDC_RDISC63_ADR, ODD_MDC_BIT0
	{MDC_RPTFIN29_ADR, ODD_MDC_BIT0, 0x0001, TRN_COMBCPN_ADR},
	{MDC_RPTFIN35_ADR, EVEN_MDC_BIT0, 0x0001, TRN_VCPN_ADR},
	{MDC_RPTFIN4_ADR, EVEN_MDC_BIT0, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTFIN4_ADR, EVEN_MDC_BIT1, 0x0002, 0},
	{MDC_RPTFIN4_ADR, EVEN_MDC_BIT2, 0x0001, TRN_PO_ADR},
	{MDC_RPTFIN4_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TIPPO_ADR},
	{MDC_RPTFIN5_ADR, ODD_MDC_BIT0, 0x0001, TRN_RA_ADR},
	{MDC_RPTFIN5_ADR, ODD_MDC_BIT1, 0x0001, TRN_CHRGTIP_ADR},
	{MDC_RPTFIN30_ADR, EVEN_MDC_BIT1, 0x0001, TRN_CHRGTIP2_ADR},
	{MDC_RPTFIN30_ADR, EVEN_MDC_BIT2, 0x0001, TRN_CHRGTIP3_ADR},
	{MDC_RPTFIN22_ADR, EVEN_MDC_BIT1, 0x0001, TRN_HASHDEPT_ADR},          // also check MDC_DEPT1_ADR, ODD_MDC_BIT1
	{MDC_RPTFIN34_ADR, ODD_MDC_BIT0, 0x0001, TRN_FSCRD_ADR},
	{MDC_RPTFIN5_ADR, ODD_MDC_BIT2, 0x0001, TRN_TTLR_ADR},
	{MDC_RPTFIN5_ADR, ODD_MDC_BIT3, 0x0002, 0},

	{MDC_RPTFIN6_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TEND1_ADR},        // Tender types start.
	{MDC_RPTFIN26_ADR, EVEN_MDC_BIT1, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN6_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TEND2_ADR},
	{MDC_RPTFIN26_ADR, EVEN_MDC_BIT2, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN6_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TEND3_ADR},
	{MDC_RPTFIN26_ADR, EVEN_MDC_BIT3, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN6_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TEND4_ADR},
	{MDC_RPTFIN27_ADR, ODD_MDC_BIT0, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN7_ADR, ODD_MDC_BIT0, 0x0001, TRN_TEND5_ADR},
	{MDC_RPTFIN27_ADR, ODD_MDC_BIT1, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN7_ADR, ODD_MDC_BIT1, 0x0001, TRN_TEND6_ADR},
	{MDC_RPTFIN27_ADR, ODD_MDC_BIT2, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN7_ADR, ODD_MDC_BIT2, 0x0001, TRN_TEND7_ADR},
	{MDC_RPTFIN27_ADR, ODD_MDC_BIT3, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN7_ADR, ODD_MDC_BIT3, 0x0001, TRN_TEND8_ADR},
	{MDC_RPTFIN28_ADR, EVEN_MDC_BIT0, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN25_ADR, ODD_MDC_BIT2, 0x0001, TRN_TEND9_ADR},
	{MDC_RPTFIN28_ADR, EVEN_MDC_BIT1, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN25_ADR, ODD_MDC_BIT3, 0x0001, TRN_TEND10_ADR},
	{MDC_RPTFIN28_ADR, EVEN_MDC_BIT2, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN26_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TEND11_ADR},
	{MDC_RPTFIN28_ADR, EVEN_MDC_BIT3, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN39_ADR, ODD_MDC_BIT2, 0x0001, TRN_TENDER12_ADR},
	{MDC_RPTFIN35_ADR, EVEN_MDC_BIT1, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN39_ADR, ODD_MDC_BIT3, 0x0001, TRN_TENDER13_ADR},
	{MDC_RPTFIN35_ADR, EVEN_MDC_BIT2, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN40_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TENDER14_ADR},
	{MDC_RPTFIN35_ADR, EVEN_MDC_BIT3, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN40_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TENDER15_ADR},
	{MDC_RPTFIN36_ADR, EVEN_MDC_BIT0, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN40_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TENDER16_ADR},
	{MDC_RPTFIN36_ADR, EVEN_MDC_BIT1, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN40_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TENDER17_ADR},
	{MDC_RPTFIN36_ADR, EVEN_MDC_BIT2, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN41_ADR, ODD_MDC_BIT0, 0x0001, TRN_TENDER18_ADR},
	{MDC_RPTFIN36_ADR, EVEN_MDC_BIT3, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN41_ADR, ODD_MDC_BIT1, 0x0001, TRN_TENDER19_ADR},
	{MDC_RPTFIN37_ADR, ODD_MDC_BIT0, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN41_ADR, ODD_MDC_BIT2, 0x0001, TRN_TENDER20_ADR},
	{MDC_RPTFIN37_ADR, ODD_MDC_BIT1, 0x0001, TRN_OFFTEND_ADR},
	{MDC_RPTFIN8_ADR, EVEN_MDC_BIT0, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTFIN8_ADR, EVEN_MDC_BIT1, 0x0002, 0},

	{MDC_RPTFIN8_ADR, EVEN_MDC_BIT2, 0x0001, TRN_FT1_ADR},        // Foreign Currency start
	{MDC_RPTFIN8_ADR, EVEN_MDC_BIT3, 0x0001, TRN_FT2_ADR},
	{MDC_RPTFIN32_ADR, ODD_MDC_BIT2, 0x0001, TRN_FT3_ADR},
	{MDC_RPTFIN32_ADR, ODD_MDC_BIT3, 0x0001, TRN_FT4_ADR},
	{MDC_RPTFIN33_ADR, EVEN_MDC_BIT0, 0x0001, TRN_FT5_ADR},
	{MDC_RPTFIN33_ADR, EVEN_MDC_BIT1, 0x0001, TRN_FT6_ADR},
	{MDC_RPTFIN33_ADR, EVEN_MDC_BIT2, 0x0001, TRN_FT7_ADR},
	{MDC_RPTFIN33_ADR, EVEN_MDC_BIT3, 0x0001, TRN_FT8_ADR},

	{MDC_RPTFIN9_ADR, ODD_MDC_BIT2, 0x0001, TRN_TTL3_ADR},       // Total key #3
	{MDC_RPTFIN9_ADR, ODD_MDC_BIT3, 0x0001, TRN_TTL4_ADR},
	{MDC_RPTFIN10_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TTL5_ADR},
	{MDC_RPTFIN10_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TTL6_ADR},
	{MDC_RPTFIN10_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TTL7_ADR},
	{MDC_RPTFIN10_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TTL8_ADR},
	{MDC_RPTFIN31_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TOTAL10_ADR},   // Total key #10
	{MDC_RPTFIN31_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TOTAL11_ADR},
	{MDC_RPTFIN31_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TOTAL12_ADR},
	{MDC_RPTFIN37_ADR, ODD_MDC_BIT2, 0x0001, TRN_TOTAL13_ADR},
	{MDC_RPTFIN37_ADR, ODD_MDC_BIT3, 0x0001, TRN_TOTAL14_ADR},
	{MDC_RPTFIN38_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TOTAL15_ADR},
	{MDC_RPTFIN38_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TOTAL16_ADR},
	{MDC_RPTFIN38_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TOTAL17_ADR},
	{MDC_RPTFIN38_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TOTAL18_ADR},
	{MDC_RPTFIN39_ADR, ODD_MDC_BIT0, 0x0001, TRN_TOTAL19_ADR},
	{MDC_RPTFIN39_ADR, ODD_MDC_BIT1, 0x0001, TRN_TOTAL20_ADR},
	{MDC_RPTFIN9_ADR, ODD_MDC_BIT0, 0x0001, TRN_TTLR_ADR},
	{MDC_RPTFIN9_ADR, ODD_MDC_BIT1, 0x0002, 0},

	{MDC_RPTFIN11_ADR, ODD_MDC_BIT0, 0x0002, 0},

	{MDC_RPTFIN11_ADR, ODD_MDC_BIT1, 0x0001, TRN_SERV1_ADR},     // Service Total
	{MDC_RPTFIN11_ADR, ODD_MDC_BIT2, 0x0001, TRN_SERV2_ADR},
	{MDC_RPTFIN11_ADR, ODD_MDC_BIT3, 0x0001, TRN_SERV3_ADR},
	{MDC_RPTFIN12_ADR, EVEN_MDC_BIT0, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTFIN12_ADR, EVEN_MDC_BIT1, 0x0002, 0},

	{MDC_RPTFIN12_ADR, EVEN_MDC_BIT2, 0x0001, TRN_ADCK1_ADR},    // Add Check Total
	{MDC_RPTFIN12_ADR, EVEN_MDC_BIT3, 0x0001, TRN_ADCK2_ADR},
	{MDC_RPTFIN13_ADR, ODD_MDC_BIT0, 0x0001, TRN_ADCK3_ADR},
	{MDC_RPTFIN13_ADR, ODD_MDC_BIT1, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTFIN13_ADR, ODD_MDC_BIT2, 0x0001, TRN_OTSTN_ADR},
	{MDC_RPTFIN13_ADR, ODD_MDC_BIT3, 0x0002, 0},

	{MDC_RPTFIN14_ADR, EVEN_MDC_BIT0, 0x0001, TRN_CHRGTIP_ADR},      // Charge Tip
	{MDC_RPTFIN30_ADR, EVEN_MDC_BIT3, 0x0001, TRN_CHRGTIP2_ADR},
	{MDC_RPTFIN31_ADR, EVEN_MDC_BIT0, 0x0001, TRN_CHRGTIP3_ADR},
	{MDC_RPTFIN14_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TIPPO_ADR},
	{MDC_RPTFIN14_ADR, EVEN_MDC_BIT2, 0x0001, TRN_OTSTN_ADR},
	{MDC_RPTFIN14_ADR, EVEN_MDC_BIT3, 0x0002, 0},
	{MDC_RPTFIN15_ADR, ODD_MDC_BIT0, 0x0001, TRN_DECTIP_ADR},
	{MDC_RPTFIN15_ADR, ODD_MDC_BIT1, 0x0002, 0},

	// Tax part of the Financial Report starts here. There are three different
	// tax systems: (1) US Tax, (2) Canadian Tax, and (3) VAT. The Financial Report
	// format for the different tax types varies since the number of itemizers
	// for each tax system varies.

	// US Tax System and VAT Tax System only at this point.
	{MDC_RPTFIN15_ADR, ODD_MDC_BIT2, 0x0001, TRN_TXBL1_ADR},        // Tax calculated, US/VAT/Canada
	{MDC_RPTFIN15_ADR, ODD_MDC_BIT3, 0x0001, TRN_TX1_ADR},          // Tax collected, US/VAT/Canada
	{MDC_RPTFIN16_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TXEXM1_ADR},      // Exempted from Tax, US and Canada only
	{MDC_RPTFIN34_ADR, ODD_MDC_BIT1, 0x0001, TRN_FSTXEXM1_ADR},     // Food Stamp Tax Exempt, US only

	{MDC_RPTFIN16_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TXBL2_ADR},       // Tax calculated, US/VAT/Canada
	{MDC_RPTFIN16_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TX2_ADR},         // Tax collected, US/VAT/Canada
	{MDC_RPTFIN16_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TXEXM2_ADR},      // Exempted from Tax, US and Canada only
	{MDC_RPTFIN34_ADR, ODD_MDC_BIT2, 0x0001, TRN_FSTXEXM2_ADR},     // Food Stamp Tax Exempt, US only

	{MDC_RPTFIN17_ADR, ODD_MDC_BIT0, 0x0001, TRN_TXBL3_ADR},        // Tax calculated, US/VAT/Canada
	{MDC_RPTFIN17_ADR, ODD_MDC_BIT1, 0x0001, TRN_TX3_ADR},          // Tax collected, US/VAT/Canada
	{MDC_RPTFIN17_ADR, ODD_MDC_BIT2, 0x0001, TRN_TXEXM3_ADR},       // Exempted from Tax, US and Canada only
	{MDC_RPTFIN34_ADR, ODD_MDC_BIT3, 0x0001, TRN_FSTXEXM3_ADR},     // Food Stamp Tax Exempt, US only

#if 0
	// Canadian Tax System follows.
	{MDC_RPTFIN17_ADR, ODD_MDC_BIT3, 0x0001, TRN_TXBL4_ADR},        // Tax calculated, Canada only
	{MDC_RPTFIN18_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TX4_ADR},         // Tax collected, Canada only
	{MDC_RPTFIN18_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TXEXM4_ADR},      // Exempted from Tax, Canada only
	{MDC_RPTFIN18_ADR, EVEN_MDC_BIT2, 0x0001, TRN_NONTX_ADR},       // Exempted from Tax, Canada only
	{MDC_RPTFIN18_ADR, EVEN_MDC_BIT3, 0x0001, TRN_PST_ADR},         //PST, Canada only

#endif

	{MDC_RPTFIN19_ADR, ODD_MDC_BIT0, 0x0001, TRN_CONSTX_ADR},
	{MDC_RPTFIN19_ADR, ODD_MDC_BIT1, 0x0002, 0},

	{MDC_RPTFIN19_ADR, ODD_MDC_BIT2, 0x0001, TRN_BNS1_ADR},       // Bonus Totals
	{MDC_RPTFIN19_ADR, ODD_MDC_BIT3, 0x0001, TRN_BNS2_ADR},
	{MDC_RPTFIN20_ADR, EVEN_MDC_BIT0, 0x0001, TRN_BNS3_ADR},
	{MDC_RPTFIN20_ADR, EVEN_MDC_BIT1, 0x0001, TRN_BNS4_ADR},
	{MDC_RPTFIN20_ADR, EVEN_MDC_BIT2, 0x0001, TRN_BNS5_ADR},
	{MDC_RPTFIN20_ADR, EVEN_MDC_BIT3, 0x0001, TRN_BNS6_ADR},
	{MDC_RPTFIN21_ADR, ODD_MDC_BIT0, 0x0001, TRN_BNS7_ADR},
	{MDC_RPTFIN21_ADR, ODD_MDC_BIT1, 0x0001, TRN_BNS8_ADR},
	{MDC_RPTFIN42_ADR, ODD_MDC_BIT0, 0x0001, TRN_BNS_9_ADR},
	{MDC_RPTFIN42_ADR, ODD_MDC_BIT1, 0x0001, TRN_BNS_10_ADR},     // bonus total 10
	{MDC_RPTFIN42_ADR, ODD_MDC_BIT2, 0x0001, TRN_BNS_11_ADR},     // bonus totals 11 - 20
	{MDC_RPTFIN42_ADR, ODD_MDC_BIT3, 0x0001, TRN_BNS_21_ADR},     // bonus totals 21 - 30
	{MDC_RPTFIN43_ADR, EVEN_MDC_BIT0, 0x0001, TRN_BNS_31_ADR},    // bonus totals 31 - 40
	{MDC_RPTFIN43_ADR, EVEN_MDC_BIT1, 0x0001, TRN_BNS_41_ADR},    // bonus totals 41 - 50
	{MDC_RPTFIN43_ADR, EVEN_MDC_BIT2, 0x0001, TRN_BNS_51_ADR},    // bonus totals 51 - 60
	{MDC_RPTFIN43_ADR, EVEN_MDC_BIT3, 0x0001, TRN_BNS_61_ADR},    // bonus totals 61 - 70
	{MDC_RPTFIN44_ADR, ODD_MDC_BIT0, 0x0001, TRN_BNS_71_ADR},     // bonus totals 71 - 80
	{MDC_RPTFIN44_ADR, ODD_MDC_BIT1, 0x0001, TRN_BNS_81_ADR},     // bonus totals 81 - 90
	{MDC_RPTFIN44_ADR, ODD_MDC_BIT2, 0x0001, TRN_BNS_91_ADR},     // bonus totals 91 - 100
	{MDC_RPTFIN21_ADR, ODD_MDC_BIT2, 0x0002, 0},
	{MDC_RPTFIN21_ADR, ODD_MDC_BIT3, 0x0001, TRN_TTLR_ADR},       // subtotal for bonus totals.
	{MDC_RPTFIN22_ADR, EVEN_MDC_BIT0, 0x0002, 0},

	{MDC_RPTFIN22_ADR, EVEN_MDC_BIT2, 0x0002, 0},                 // line feed after the Surcharges section of the report.

	{MDC_RPTFIN22_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TRACAN_ADR},
	{MDC_RPTFIN23_ADR, ODD_MDC_BIT0, 0x0001, TRN_MVD_ADR},
	{MDC_RPTFIN32_ADR, ODD_MDC_BIT0, 0x0001, TRN_LOAN_ADR},
	{MDC_RPTFIN32_ADR, ODD_MDC_BIT1, 0x0001, TRN_PICKUP_ADR},
	{MDC_RPTFIN23_ADR, ODD_MDC_BIT1, 0x0002, 0},

	{MDC_RPTFIN23_ADR, ODD_MDC_BIT2, 0x0001, TRN_AUD_ADR},
	{MDC_RPTFIN23_ADR, ODD_MDC_BIT3, 0x0002, 0},

	{MDC_RPTFIN24_ADR, EVEN_MDC_BIT0, 0x0001, TRN_NOSALE_ADR},      // Transaction Counts
	{MDC_RPTFIN24_ADR, EVEN_MDC_BIT1, 0x0001, TRN_ITMPROCO_ADR},
	{MDC_RPTFIN24_ADR, EVEN_MDC_BIT2, 0x0001, TRN_PSN_ADR},
	{MDC_RPTFIN24_ADR, EVEN_MDC_BIT3, 0x0001, TRN_CUST_ADR},
	{MDC_RPTFIN25_ADR, ODD_MDC_BIT0, 0x0001, TRN_CHKOPN_ADR},
	{MDC_RPTFIN25_ADR, ODD_MDC_BIT1, 0x0001, TRN_CHKCLS_ADR}
};

static P223_Rpt CashiersRpt [] = {
	{0, 0,  0x0001, TRN_DAIGGT_ADR},
	{MDC_RPTCAS1_ADR, ODD_MDC_BIT1,  0x0001, TRN_PLUSVD_ADR},
	{MDC_RPTCAS1_ADR, ODD_MDC_BIT2,  0x0001, TRN_PSELVD_ADR},
	{MDC_RPTCAS50_ADR, ODD_MDC_BIT0,  0x0001, TRN_TRETURN1_ADR},
	{MDC_RPTCAS50_ADR, ODD_MDC_BIT1,  0x0001, TRN_TRETURN2_ADR},
	{MDC_RPTCAS50_ADR, ODD_MDC_BIT2,  0x0001, TRN_TRETURN3_ADR},
	{MDC_RPTCAS1_ADR, ODD_MDC_BIT3,  0x0001, TRN_STTLR_ADR},
	{MDC_RPTCAS2_ADR, EVEN_MDC_BIT0, 0x0002, 0},
	{MDC_RPTCAS2_ADR, EVEN_MDC_BIT1,  0x0001, TRN_CONSCPN_ADR},

	// The following discount totals are actually printed if the discount
	// total is a discount rather than a surcharge. Each of the discount
	// totals has an MDC that determines if the itemizer is a discount amount
	// or a surcharge amount.
	{MDC_RPTCAS2_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ITMDISC_ADR},          // MDC_MODID13_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS2_ADR, EVEN_MDC_BIT3,  0x0001, TRN_MODID_ADR},            // MDC_MODID23_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT0,  0x0001, TRN_ITMDISC_ADR_3},       // MDC_MODID33_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT1,  0x0001, TRN_ITMDISC_ADR_4},       // MDC_MODID43_ADR, EVEN_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ITMDISC_ADR_5},       // MDC_MODID53_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT3,  0x0001, TRN_ITMDISC_ADR_6},       // MDC_MODID63_ADR, EVEN_MDC_BIT0
	{MDC_RPTCAS3_ADR, ODD_MDC_BIT0,  0x0001, TRN_RDISC1_ADR},            // MDC_RDISC13_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS3_ADR, ODD_MDC_BIT1,  0x0001, TRN_RDISC2_ADR},            // MDC_RDISC23_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT0,  0x0001, TRN_RDISC3_ADR},          // MDC_RDISC33_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT1,  0x0001, TRN_RDISC4_ADR},          // MDC_RDISC43_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT2,  0x0001, TRN_RDISC5_ADR},          // MDC_RDISC53_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT3,  0x0001, TRN_RDISC6_ADR},          // MDC_RDISC63_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS21_ADR, ODD_MDC_BIT3,  0x0001, TRN_COMBCPN_ADR},
	{MDC_RPTCAS32_ADR, EVEN_MDC_BIT1,  0x0001, TRN_VCPN_ADR},
	{MDC_RPTCAS3_ADR, ODD_MDC_BIT2,  0x0001, TRN_STTLR_ADR},
	{MDC_RPTCAS3_ADR, ODD_MDC_BIT3, 0x0002, 0},

	{MDC_RPTCAS4_ADR, EVEN_MDC_BIT0,  0x0001, TRN_PO_ADR},
	{MDC_RPTCAS4_ADR, EVEN_MDC_BIT1,  0x0001, TRN_TIPPO_ADR},
	{MDC_RPTCAS4_ADR, EVEN_MDC_BIT2,  0x0001, TRN_RA_ADR},
	{MDC_RPTCAS4_ADR, EVEN_MDC_BIT3,  0x0001, TRN_CHRGTIP_ADR},
	{MDC_RPTCAS24_ADR, EVEN_MDC_BIT0,  0x0001, TRN_CHRGTIP2_ADR},
	{MDC_RPTCAS24_ADR, EVEN_MDC_BIT1,  0x0001, TRN_CHRGTIP3_ADR},
	{MDC_RPTCAS27_ADR, ODD_MDC_BIT0,  0x0001, TRN_LOAN_ADR},
	{MDC_RPTCAS27_ADR, ODD_MDC_BIT1,  0x0001, TRN_PICKUP_ADR},
	{MDC_RPTCAS17_ADR, ODD_MDC_BIT3,  0x0001, TRN_HASHDEPT_ADR},         // MDC_DEPT1_ADR, ODD_MDC_BIT1
	{MDC_RPTCAS32_ADR, EVEN_MDC_BIT0,  0x0001, TRN_FSCRD_ADR},
	{MDC_RPTCAS23_ADR, ODD_MDC_BIT2,  0x0001, TRN_CKTO_ADR},
	{MDC_RPTCAS23_ADR, ODD_MDC_BIT3,  0x0001, TRN_CKFRM_ADR},
	{MDC_RPTCAS5_ADR, ODD_MDC_BIT0,  0x0001, TRN_TTLR_ADR},
	{MDC_RPTCAS5_ADR, ODD_MDC_BIT1, 0x0002, 0},

	{MDC_RPTCAS5_ADR, ODD_MDC_BIT2,  0x0001, TRN_TEND1_ADR},
	{MDC_RPTCAS29_ADR, ODD_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS5_ADR, ODD_MDC_BIT3,  0x0001, TRN_TEND2_ADR},
	{MDC_RPTCAS29_ADR, ODD_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS6_ADR, EVEN_MDC_BIT0,  0x0001, TRN_TEND3_ADR},
	{MDC_RPTCAS29_ADR, ODD_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS6_ADR, EVEN_MDC_BIT1,  0x0001, TRN_TEND4_ADR},
	{MDC_RPTCAS29_ADR, ODD_MDC_BIT3,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS6_ADR, EVEN_MDC_BIT2,  0x0001, TRN_TEND5_ADR},
	{MDC_RPTCAS30_ADR, EVEN_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS6_ADR, EVEN_MDC_BIT3,  0x0001, TRN_TEND6_ADR},
	{MDC_RPTCAS30_ADR, EVEN_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS7_ADR, ODD_MDC_BIT0,  0x0001, TRN_TEND7_ADR},
	{MDC_RPTCAS30_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS7_ADR, ODD_MDC_BIT1,  0x0001, TRN_TEND8_ADR},
	{MDC_RPTCAS30_ADR, EVEN_MDC_BIT3,  0x0001, TRN_ONHAND_ADR},

	{MDC_RPTCAS21_ADR, ODD_MDC_BIT0,  0x0001, TRN_TEND9_ADR},
	{MDC_RPTCAS31_ADR, ODD_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS21_ADR, ODD_MDC_BIT1,  0x0001, TRN_TEND10_ADR},
	{MDC_RPTCAS31_ADR, ODD_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS21_ADR, ODD_MDC_BIT2,  0x0001, TRN_TEND11_ADR},
	{MDC_RPTCAS31_ADR, ODD_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},

	{MDC_RPTCAS38_ADR, ODD_MDC_BIT2,  0x0001, TRN_TENDER12_ADR},
	{MDC_RPTCAS26_ADR, EVEN_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS38_ADR, ODD_MDC_BIT3,  0x0001, TRN_TENDER13_ADR},
	{MDC_RPTCAS26_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS39_ADR, EVEN_MDC_BIT0,  0x0001, TRN_TENDER14_ADR},
	{MDC_RPTCAS26_ADR, EVEN_MDC_BIT3,  0x0001, TRN_ONHAND_ADR},

	{MDC_RPTCAS39_ADR, EVEN_MDC_BIT1,  0x0001, TRN_TENDER15_ADR},
	{MDC_RPTCAS35_ADR, EVEN_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS39_ADR, EVEN_MDC_BIT2,  0x0001, TRN_TENDER16_ADR},
	{MDC_RPTCAS35_ADR, EVEN_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS39_ADR, EVEN_MDC_BIT3,  0x0001, TRN_TENDER17_ADR},
	{MDC_RPTCAS35_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS40_ADR, ODD_MDC_BIT0,  0x0001, TRN_TENDER18_ADR},
	{MDC_RPTCAS35_ADR, EVEN_MDC_BIT3,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS40_ADR, ODD_MDC_BIT1,  0x0001, TRN_TENDER19_ADR},
	{MDC_RPTCAS36_ADR, ODD_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS40_ADR, ODD_MDC_BIT2,  0x0001, TRN_TENDER20_ADR},
	{MDC_RPTCAS36_ADR, ODD_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS7_ADR, ODD_MDC_BIT2,  0x0001, TRN_TTLR_ADR},
	{MDC_RPTCAS7_ADR, ODD_MDC_BIT3, 0x0002, 0},

	{MDC_RPTCAS8_ADR, EVEN_MDC_BIT0, 0x0001, TRN_FT1_ADR},        // Foreign Currency start
	{MDC_RPTCAS33_ADR, ODD_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS8_ADR, EVEN_MDC_BIT1, 0x0001, TRN_FT2_ADR},
	{MDC_RPTCAS33_ADR, ODD_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS27_ADR, ODD_MDC_BIT2, 0x0001, TRN_FT3_ADR},
	{MDC_RPTCAS33_ADR, ODD_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS27_ADR, ODD_MDC_BIT3, 0x0001, TRN_FT4_ADR},
	{MDC_RPTCAS33_ADR, ODD_MDC_BIT3,  0x0001, TRN_ONHAND_ADR},

	{MDC_RPTCAS28_ADR, EVEN_MDC_BIT0, 0x0001, TRN_FT5_ADR},
	{MDC_RPTCAS34_ADR, EVEN_MDC_BIT0,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS28_ADR, EVEN_MDC_BIT1, 0x0001, TRN_FT6_ADR},
	{MDC_RPTCAS34_ADR, EVEN_MDC_BIT1,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS28_ADR, EVEN_MDC_BIT2, 0x0001, TRN_FT7_ADR},
	{MDC_RPTCAS34_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ONHAND_ADR},
	{MDC_RPTCAS28_ADR, EVEN_MDC_BIT3, 0x0001, TRN_FT8_ADR},
	{MDC_RPTCAS34_ADR, EVEN_MDC_BIT3,  0x0001, TRN_ONHAND_ADR},

	// There are two types of total key provisioning:
	//   - No Assume Tender
	//   - NOT No Assume Tender
	// These are printed in two different sections, one with a Subtotal and one without.
	{MDC_RPTCAS9_ADR, ODD_MDC_BIT0, 0x0001, TRN_TTL3_ADR},       // Total key #3
	{MDC_RPTCAS9_ADR, ODD_MDC_BIT1, 0x0001, TRN_TTL4_ADR},
	{MDC_RPTCAS9_ADR, ODD_MDC_BIT2, 0x0001, TRN_TTL5_ADR},
	{MDC_RPTCAS9_ADR, ODD_MDC_BIT3, 0x0001, TRN_TTL6_ADR},
	{MDC_RPTCAS10_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TTL7_ADR},
	{MDC_RPTCAS10_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TTL8_ADR},
	{MDC_RPTCAS31_ADR, ODD_MDC_BIT3, 0x0001, TRN_TOTAL10_ADR},   // Total key #10
	{MDC_RPTCAS32_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TOTAL11_ADR},
	{MDC_RPTCAS32_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TOTAL12_ADR},
	{MDC_RPTCAS36_ADR, ODD_MDC_BIT2, 0x0001, TRN_TOTAL13_ADR},
	{MDC_RPTCAS36_ADR, ODD_MDC_BIT3, 0x0001, TRN_TOTAL14_ADR},
	{MDC_RPTCAS37_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TOTAL15_ADR},
	{MDC_RPTCAS37_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TOTAL16_ADR},
	{MDC_RPTCAS37_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TOTAL17_ADR},
	{MDC_RPTCAS37_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TOTAL18_ADR},
	{MDC_RPTCAS38_ADR, ODD_MDC_BIT0, 0x0001, TRN_TOTAL19_ADR},
	{MDC_RPTCAS38_ADR, ODD_MDC_BIT1, 0x0001, TRN_TOTAL20_ADR},
	{MDC_RPTCAS8_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TTLR_ADR},      // Sub-Total for the No Assume Tender Totals
	{MDC_RPTCAS8_ADR, EVEN_MDC_BIT3, 0x0002, 0},                 // line feed before printing NOT No Assume Tender Totals

	{MDC_RPTCAS10_ADR, EVEN_MDC_BIT2, 0x0002, 0},                // line feed after printing NOT No Assume Tender Totals.

	{MDC_RPTCAS10_ADR, EVEN_MDC_BIT3, 0x0001, TRN_SERV1_ADR},     // Service Total
	{MDC_RPTCAS11_ADR, ODD_MDC_BIT0, 0x0001, TRN_SERV2_ADR},
	{MDC_RPTCAS11_ADR, ODD_MDC_BIT1, 0x0001, TRN_SERV3_ADR},
	{MDC_RPTCAS11_ADR, ODD_MDC_BIT2, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTCAS11_ADR, ODD_MDC_BIT3, 0x0002, 0},

	{MDC_RPTCAS12_ADR, EVEN_MDC_BIT0, 0x0001, TRN_ADCK1_ADR},    // Add Check Total
	{MDC_RPTCAS12_ADR, EVEN_MDC_BIT1, 0x0001, TRN_ADCK2_ADR},
	{MDC_RPTCAS12_ADR, EVEN_MDC_BIT2, 0x0001, TRN_ADCK3_ADR},
	{MDC_RPTCAS12_ADR, EVEN_MDC_BIT3, 0x0001, TRN_STTLR_ADR},
	{MDC_RPTCAS13_ADR, ODD_MDC_BIT0, 0x0001, TRN_OTSTN_ADR},
	{MDC_RPTCAS13_ADR, ODD_MDC_BIT1, 0x0002, 0},

	{MDC_RPTCAS13_ADR, ODD_MDC_BIT2, 0x0001, TRN_CHRGTIP_ADR},      // Charge Tip
	{MDC_RPTCAS25_ADR, ODD_MDC_BIT3, 0x0001, TRN_CHRGTIP2_ADR},
	{MDC_RPTCAS26_ADR, EVEN_MDC_BIT0, 0x0001, TRN_CHRGTIP3_ADR},
	{MDC_RPTCAS13_ADR, ODD_MDC_BIT3, 0x0001, TRN_TIPPO_ADR},
	{MDC_RPTCAS14_ADR, EVEN_MDC_BIT0, 0x0001, TRN_OTSTN_ADR},
	{MDC_RPTCAS24_ADR, EVEN_MDC_BIT3, 0x0002, 0},
	{MDC_RPTCAS25_ADR, ODD_MDC_BIT0, 0x0001, TRN_DECTIP_ADR},
	{MDC_RPTCAS25_ADR, ODD_MDC_BIT1, 0x0001, TRN_CALC_ADR},
	{MDC_RPTCAS25_ADR, ODD_MDC_BIT2, 0x0001, TRN_OTSTN_ADR},
	{MDC_RPTCAS14_ADR, EVEN_MDC_BIT1, 0x0002, 0},


	// Tax part of the Financial Report starts here. There are three different
	// tax systems: (1) US Tax, (2) Canadian Tax, and (3) VAT. The Financial Report
	// format for the different tax types varies since the number of itemizers
	// for each tax system varies.

	// US Tax System and VAT Tax System only at this point.
	{MDC_RPTCAS41_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TXBL1_ADR},        // Tax calculated, US/VAT/Canada
	{MDC_RPTCAS41_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TX1_ADR},          // Tax collected, US/VAT/Canada
	{MDC_RPTCAS42_ADR, ODD_MDC_BIT0, 0x0001, TRN_TXEXM1_ADR},      // Exempted from Tax, US and Canada only
	{MDC_RPTCAS45_ADR, EVEN_MDC_BIT1, 0x0001, TRN_FSTXEXM1_ADR},     // Food Stamp Tax Exempt, US only

	{MDC_RPTCAS42_ADR, ODD_MDC_BIT1, 0x0001, TRN_TXBL2_ADR},       // Tax calculated, US/VAT/Canada
	{MDC_RPTCAS42_ADR, ODD_MDC_BIT2, 0x0001, TRN_TX2_ADR},         // Tax collected, US/VAT/Canada
	{MDC_RPTCAS42_ADR, ODD_MDC_BIT3, 0x0001, TRN_TXEXM2_ADR},      // Exempted from Tax, US and Canada only
	{MDC_RPTCAS45_ADR, EVEN_MDC_BIT2, 0x0001, TRN_FSTXEXM2_ADR},     // Food Stamp Tax Exempt, US only

	{MDC_RPTCAS43_ADR, EVEN_MDC_BIT0, 0x0001, TRN_TXBL3_ADR},        // Tax calculated, US/VAT/Canada
	{MDC_RPTCAS43_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TX3_ADR},          // Tax collected, US/VAT/Canada
	{MDC_RPTCAS43_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TXEXM3_ADR},       // Exempted from Tax, US and Canada only
	{MDC_RPTCAS45_ADR, EVEN_MDC_BIT3, 0x0001, TRN_FSTXEXM3_ADR},     // Food Stamp Tax Exempt, US only

#if 0
	// Canadian Tax System follows.
	{MDC_RPTCAS43_ADR, EVEN_MDC_BIT3, 0x0001, TRN_TXBL4_ADR},        // Tax calculated, Canada only
	{MDC_RPTCAS44_ADR, ODD_MDC_BIT0, 0x0001, TRN_TX4_ADR},         // Tax collected, Canada only
	{MDC_RPTCAS44_ADR, ODD_MDC_BIT1, 0x0001, TRN_TXEXM4_ADR},      // Exempted from Tax, Canada only
	{MDC_RPTCAS44_ADR, ODD_MDC_BIT2, 0x0001, TRN_NONTX_ADR},       // Exempted from Tax, Canada only
	{MDC_RPTCAS44_ADR, ODD_MDC_BIT3, 0x0001, TRN_PST_ADR},         //PST, Canada only
#endif

	{MDC_RPTCAS14_ADR, EVEN_MDC_BIT2, 0x0001, TRN_CONSTX_ADR},
#if 0
	// VAT Tax System follows.
	{MDC_RPTCAS24_ADR, EVEN_MDC_BIT2, 0x0001, TRN_TXBL4_ADR},        // Tax calculated, VAT only
#endif
	{MDC_RPTCAS14_ADR, EVEN_MDC_BIT3, 0x0002, 0},

	{MDC_RPTCAS15_ADR, ODD_MDC_BIT0, 0x0001, TRN_BNS1_ADR},       // Bonus Totals
	{MDC_RPTCAS15_ADR, ODD_MDC_BIT1, 0x0001, TRN_BNS2_ADR},
	{MDC_RPTCAS15_ADR, ODD_MDC_BIT2, 0x0001, TRN_BNS3_ADR},
	{MDC_RPTCAS15_ADR, ODD_MDC_BIT3, 0x0001, TRN_BNS4_ADR},
	{MDC_RPTCAS16_ADR, EVEN_MDC_BIT0, 0x0001, TRN_BNS5_ADR},
	{MDC_RPTCAS16_ADR, EVEN_MDC_BIT1, 0x0001, TRN_BNS6_ADR},
	{MDC_RPTCAS16_ADR, EVEN_MDC_BIT2, 0x0001, TRN_BNS7_ADR},
	{MDC_RPTCAS16_ADR, EVEN_MDC_BIT3, 0x0001, TRN_BNS8_ADR},
	{MDC_RPTCAS46_ADR, ODD_MDC_BIT0, 0x0001, TRN_BNS_9_ADR},
	{MDC_RPTCAS46_ADR, ODD_MDC_BIT1, 0x0001, TRN_BNS_10_ADR},     // bonus total 10
	{MDC_RPTCAS46_ADR, ODD_MDC_BIT2, 0x0001, TRN_BNS_11_ADR},     // bonus totals 11 - 20
	{MDC_RPTCAS46_ADR, ODD_MDC_BIT3, 0x0001, TRN_BNS_21_ADR},     // bonus totals 21 - 30
	{MDC_RPTCAS47_ADR, EVEN_MDC_BIT0, 0x0001, TRN_BNS_31_ADR},    // bonus totals 31 - 40
	{MDC_RPTCAS47_ADR, EVEN_MDC_BIT1, 0x0001, TRN_BNS_41_ADR},    // bonus totals 41 - 50
	{MDC_RPTCAS47_ADR, EVEN_MDC_BIT2, 0x0001, TRN_BNS_51_ADR},    // bonus totals 51 - 60
	{MDC_RPTCAS47_ADR, EVEN_MDC_BIT3, 0x0001, TRN_BNS_61_ADR},    // bonus totals 61 - 70
	{MDC_RPTCAS48_ADR, ODD_MDC_BIT0, 0x0001, TRN_BNS_71_ADR},     // bonus totals 71 - 80
	{MDC_RPTCAS48_ADR, ODD_MDC_BIT1, 0x0001, TRN_BNS_81_ADR},     // bonus totals 81 - 90
	{MDC_RPTCAS48_ADR, ODD_MDC_BIT2, 0x0001, TRN_BNS_91_ADR},     // bonus totals 91 - 100

	{MDC_RPTCAS17_ADR, ODD_MDC_BIT0, 0x0002, 0},
	{MDC_RPTCAS17_ADR, ODD_MDC_BIT1, 0x0001, TRN_TTLR_ADR},       // subtotal for bonus totals.
	{MDC_RPTCAS17_ADR, ODD_MDC_BIT2, 0x0002, 0},

#if 0
	// The following discount totals are actually printed if the discount
	// total is a surcharge rather than a discount. Each of the discount
	// totals has an MDC that determines if the itemizer is a discount amount
	// or a surcharge amount.
	{MDC_RPTCAS2_ADR, EVEN_MDC_BIT2, 0x0001, TRN_ITMDISC_ADR},     // Transaction Discount Totals
	{MDC_RPTCAS2_ADR, EVEN_MDC_BIT3, 0x0001, TRN_MODID_ADR},       // See as well MDC_MODID23_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT0,  0x0001, TRN_ITMDISC_ADR_3},       // MDC_MODID33_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT1,  0x0001, TRN_ITMDISC_ADR_4},       // MDC_MODID43_ADR, EVEN_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT2,  0x0001, TRN_ITMDISC_ADR_5},       // MDC_MODID53_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS49_ADR, EVEN_MDC_BIT3,  0x0001, TRN_ITMDISC_ADR_6},       // MDC_MODID63_ADR, EVEN_MDC_BIT0
	{MDC_RPTCAS3_ADR, ODD_MDC_BIT0, 0x0001, TRN_RDISC1_ADR},      // See as well MDC_RDISC13_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS3_ADR, ODD_MDC_BIT1, 0x0001, TRN_RDISC2_ADR},      // See as well MDC_RDISC23_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT0, 0x0001, TRN_RDISC3_ADR},     // See as well MDC_RDISC33_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT1, 0x0001, TRN_RDISC4_ADR},     // See as well MDC_RDISC43_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT2, 0x0001, TRN_RDISC5_ADR},     // See as well MDC_RDISC53_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS22_ADR, EVEN_MDC_BIT3, 0x0001, TRN_RDISC6_ADR},    // See as well MDC_RDISC63_ADR, ODD_MDC_BIT0
	{MDC_RPTCAS17_ADR, ODD_MDC_BIT3, 0x0001, TRN_HASHDEPT_ADR},  // See as well MDC_DEPT1_ADR, ODD_MDC_BIT1
#endif
	{MDC_RPTCAS18_ADR, EVEN_MDC_BIT0, 0x0002, 0},                // line feed after printing the Surcharge totals

	{MDC_RPTCAS18_ADR, EVEN_MDC_BIT1, 0x0001, TRN_TRACAN_ADR},
	{MDC_RPTCAS18_ADR, EVEN_MDC_BIT2, 0x0001, TRN_MVD_ADR},
	{MDC_RPTCAS18_ADR, EVEN_MDC_BIT3, 0x0002, 0},

	{MDC_RPTCAS19_ADR, ODD_MDC_BIT0, 0x0001, TRN_AUD_ADR},
	{MDC_RPTCAS19_ADR, ODD_MDC_BIT1, 0x0002, 0},

	{MDC_RPTCAS19_ADR, ODD_MDC_BIT2, 0x0001, TRN_NOSALE_ADR},      // Transaction Counts
	{MDC_RPTCAS19_ADR, ODD_MDC_BIT3, 0x0001, TRN_ITMPROCO_ADR},
	{MDC_RPTCAS20_ADR, EVEN_MDC_BIT0, 0x0001, TRN_PSN_ADR},
	{MDC_RPTCAS20_ADR, EVEN_MDC_BIT1, 0x0001, TRN_CUST_ADR},
	{MDC_RPTCAS20_ADR, EVEN_MDC_BIT2, 0x0001, TRN_CHKOPN_ADR},
	{MDC_RPTCAS20_ADR, EVEN_MDC_BIT3, 0x0001, TRN_CHKCLS_ADR},
	{MDC_RPTCAS23_ADR, ODD_MDC_BIT0, 0x0001, TRN_POST_ADR}
};

static  BYTE    abMDCData[600];    /* Buffer of MDC Data       */

static UCHAR LclParaMDCRead( USHORT usAddress, UCHAR uchMaskData )
{
    UCHAR    uchMdc = 0;    // default is off in case usAddress is zero.

	// we use an address of zero to indicate
	// a report entry which is not to be changed.
	if (usAddress > 0) {
		/* calculate for fitting program address to ram address */
		USHORT    i = ((usAddress - 1) / 2);

		uchMdc = abMDCData[i];
		uchMdc &= uchMaskData;
	}
    return(uchMdc);                     /* return - masked bit pattern */
}

static UCHAR LclParaMDCWrite( USHORT usAddress, UCHAR uchMdc, UINT uiSetOn )
{
	// we use an address of zero to indicate
	// a report entry which is not to be changed.
	if (usAddress > 0) {
		/* calculate for fitting program address to ram address */
		USHORT    i = ((usAddress - 1) / 2);

		if (uiSetOn)
			abMDCData[i] |= uchMdc;
		else
			abMDCData[i] &= ~ uchMdc;

	}
    return(uchMdc);                     /* return - masked bit pattern */
}

static UCHAR LclP223_RptRead (const P223_Rpt rpt, wchar_t * pMnemonic)
{
	UCHAR uchMdc = 0;
	
	pMnemonic[0] = 0;
	if (rpt.usMnemType) {
		uchMdc = LclParaMDCRead( rpt.usMdcAddr, rpt.usMdcBit );
		if (rpt.usMnemAddr > 0) {
			memcpy (pMnemonic, lpWork + (rpt.usMnemAddr - 1), P20_MNE_LEN * 2);
			pMnemonic[P20_MNE_LEN] = 0;
		}
	}

	return uchMdc;
}

/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/


/*
* ===========================================================================
**  Synopsis:   VOID    P20GetMnemo();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*               WORD  wParam    - Control ID of Current Changed EditText  
*
**  Return  :   No return value
*
**  Description:
*       This function gets current inputed Mnemonic from EditText. And set them
*      to buffer.
* ===========================================================================
*/
static VOID    P223GetMnemo(HWND hDlg, WORD wItem, const P223_Rpt_Data *pRpt)
{
	WORD     wID2 = P20GETINDEX(wItem, unTopIndex);

	if (wID2 <= pRpt->scrollInfo.nMax && pRpt->table[wID2].usMnemAddr > 0) {
		WCHAR    szWork[P20_MNE_LEN + 1] = {0};

		/* ----- Get Inputed Data from EditText ----- */
		DlgItemGetText(hDlg, wItem, szWork, P20_MNE_LEN + 1);
		/* ----- Set Inputed Data to a Data Buffer ----- */
		memcpy (lpWork + (pRpt->table[wID2].usMnemAddr - 1), szWork, P20_MNE_LEN * 2);
	}
}

static VOID    P223GetMdc(HWND hDlg, WORD wItem, const P223_Rpt_Data *pRpt)
{
	WORD     wID2 = wItem - IDD_P20_DESC01 + unTopIndex;

	if (wID2 <= pRpt->scrollInfo.nMax && pRpt->table[wID2].usMdcAddr > 0) {
		UINT     uiCheck = 0;

		// get the check box state.
		// Invert the logic as if the MDC bit is set then it means NOT DO whatever
		// whereas if the MDC bit is not set then it means TO DO whatever.
		// We want this to indicate if checked that the field is enabled and will
		// show on the report.
		uiCheck = IsDlgButtonChecked (hDlg, wItem);
		if (uiCheck == BST_CHECKED) {
			uiCheck = 0;
		} else {
			uiCheck = 1;
		}
		// update the memory resident data.
		LclParaMDCWrite (pRpt->table[wID2].usMdcAddr, pRpt->table[wID2].usMdcBit, uiCheck);
	}
}

/*
* ===========================================================================
**  Synopsis:   VOID    P20DispAllocErr();
*
**  Input   :   HWND    hDlg      - Handle of a DialogBox Procedure
*               HGLOBAL hGlbMnemo - Handle of a Global Area (Mnemonic)
*               HGLOBAL hGlbWork  - Handle of a Global Area (Work)
*
**  Return  :   No return value.
*
**  Description:
*       This function shows a MessageBox for Error Message. And reset global
*     heap area, if allocation was success.
* ===========================================================================
*/
static VOID    P223DispAllocErr(HWND hDlg, HGLOBAL hGlbMnemo, HGLOBAL hGlbWork)
{
    WCHAR szCaption[P20_CAP_LEN];        /* MessageBox Caption */
    WCHAR szMessage[P20_ERR_LEN];        /* Error Message Strings */

    /* ----- Load MessageBox Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_P20, szCaption, PEP_STRING_LEN_MAC(szCaption));

    /* ----- Load Error Message from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMessage, PEP_STRING_LEN_MAC(szMessage));

    /* ----- Text out MessageBox ----- */
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, szMessage, szCaption, MB_OK | MB_ICONEXCLAMATION);

    /* ----- Reset Allocated Area ----- */
    if (hGlbMnemo) {
        GlobalFree(hGlbMnemo);
    }

    if (hGlbWork) {
        GlobalFree(hGlbWork);
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P20CalcIndex();
*
**  Input   :   WORD  wScrlCode    - scroll code from scroll bar control
*
**  Return  :   TRUE    -   Scroll up to Top or Bottom of ListBox
*               FALSE   -   Not Scroll up tp Top or Bottom of ListBox
*
**  Description:
*       This function compute top index and bottom index.
* ===========================================================================
*/
static BOOL    P223CalcIndex(WORD wScrlCode, SCROLLINFO *pScrollInfo)
{
    BOOL    fReturn = FALSE;

    switch (wScrlCode) {

    case SB_LINEUP:
		if (unTopIndex > pScrollInfo->nMin) {
			unTopIndex    -= 1;
			unBottomIndex -= 1;
		} else {
			unTopIndex    = pScrollInfo->nMin;
			unBottomIndex = unTopIndex + pScrollInfo->nPage - 1;
        }
        break;

    case SB_LINEDOWN:
		if (unBottomIndex < pScrollInfo->nMax) {
			unTopIndex    += 1;
			unBottomIndex += 1;
        }
        break;

    case SB_PAGEUP:
		if (unTopIndex > pScrollInfo->nMin + pScrollInfo->nPage) {
			unTopIndex    -= pScrollInfo->nPage;
			unBottomIndex -= pScrollInfo->nPage;
		} else {
			unTopIndex    = pScrollInfo->nMin;
			unBottomIndex = unTopIndex + pScrollInfo->nPage - 1;
		}
        break;

    case SB_PAGEDOWN:
		if (unBottomIndex < pScrollInfo->nMax - pScrollInfo->nPage) {
			unTopIndex    += pScrollInfo->nPage;
			unBottomIndex += pScrollInfo->nPage;
		} else {
			unBottomIndex = pScrollInfo->nMax;
			unTopIndex    = unBottomIndex - pScrollInfo->nPage + 1;
		}
    }
    return fReturn;
}

/*
* ===========================================================================
**  Synopsis:   VOID    P20RedrawText();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value.
*
**  Description:
*       This function redraws description & Halo data to each control.
* ===========================================================================
*/
static VOID    P223RedrawText(HWND hDlg, P223_Rpt  *table)
{
    int     unCount;
    UINT    unLoop = 0;

    for (unCount = unTopIndex; unCount <= unBottomIndex; unCount++, unLoop++) {
		WORD     wID = (WORD)(IDD_P20_EDIT01 + unLoop);
		WCHAR    szWork[P20_DESC_LEN] = {0};

        /* ----- Load Strings from Resource ----- */
		if (table[unCount].usMnemAddr > 0) {
			WCHAR	 szTempBuff[256] = {0};         // temporary buffer to load string into. JHHJ 10-15-2004

			LoadString(hResourceDll, IDS_P020DESC001 + table[unCount].usMnemAddr - 1, szTempBuff, P20_DESC_LEN);
			//copy only the size that will fit into the buffer that is allotted, this 
			//prevents overspill from reading into buffers.
			wcsncpy(szWork, szTempBuff, P20_DESC_LEN);

			EnableWindow(GetDlgItem(hDlg, wID), TRUE);    // mnemonic address and mnemnic can be changed.

			/* ----- Set Loaded Strings to StaticText ----- */
		} else {
			WCHAR	 szTempBuff[256] = {0};         // temporary buffer to load string into. JHHJ 10-15-2004

			if (! PepFetchMdcMnemonic (table[unCount].usMdcAddr, table[unCount].usMdcBit, 0, szTempBuff, P20_DESC_LEN)[0]) {
				// string returned is an empty string so lets just use a default.
				LoadString(hResourceDll, 27104, szTempBuff, P20_DESC_LEN);
			}
			//copy only the size that will fit into the buffer that is allotted, this 
			//prevents overspill from reading into buffers.
			wcsncpy(szWork, szTempBuff, P20_DESC_LEN);

			EnableWindow(GetDlgItem(hDlg, wID), FALSE);    // no mnemonic address and mnemnic can not be changed.
		}
		DlgItemRedrawText(hDlg, IDD_P20_DESC01 + unLoop, szWork);

        /* ----- Set Mnemonic Data to EditText ----- */
		if (table[unCount].usMnemAddr > 0) {
			UCHAR uchMask = LclP223_RptRead (table[unCount], szWork);
			DlgItemRedrawText(hDlg, wID, szWork); /* V3.3 */
			// Invert the logic as if the MDC bit is set then it means NOT DO whatever
			// whereas if the MDC bit is not set then it means TO DO whatever.
			// We want this to indicate if checked that the field is enabled and will
			// show on the report.
			if (uchMask) {
				CheckDlgButton (hDlg, IDD_P20_DESC01 + unLoop, BST_UNCHECKED);
			} else {
				CheckDlgButton (hDlg, IDD_P20_DESC01 + unLoop, BST_CHECKED);
			}
		} else {
			UCHAR uchMask = LclP223_RptRead (table[unCount], szWork);
			DlgItemRedrawText(hDlg, wID, szWork); /* V3.3 */
			// Invert the logic as if the MDC bit is set then it means NOT DO whatever
			// whereas if the MDC bit is not set then it means TO DO whatever.
			// We want this to indicate if checked that the field is enabled and will
			// show on the report.
			if (uchMask) {
				CheckDlgButton (hDlg, IDD_P20_DESC01 + unLoop, BST_UNCHECKED);
			} else {
				CheckDlgButton (hDlg, IDD_P20_DESC01 + unLoop, BST_CHECKED);
			}
		}
    }
}

/*
* ===========================================================================
**  Synopsis:   BOOL    P20ScrlProc();
*
**  Input   :   HWND  hDlg      - Handle of a DialogBox Procedure
*               WORD  wParam    - 16 Bits Message Parameter
*               LONG  lParam    - 32 Bits Message Parameter
*
**  Return  :   TRUE            - User Process is Executed
*               FALSE           - Default Process is Expected
*
**  Description:
*       This function is Vertical Scroll Procedure.
* ===========================================================================
*/
static BOOL    P223ScrlProc(HWND hDlg, WPARAM wParam, LPARAM lParam, P223_Rpt_Data *pRptData)
{
    BOOL    fReturn = FALSE;        /* Return Value of This Function */
	SCROLLINFO *pScrollInfo = &pRptData->scrollInfo;

    switch (LOWORD(wParam)) {

    case SB_LINEUP:
    case SB_PAGEUP:
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        if (! P223CalcIndex(LOWORD(wParam), pScrollInfo)) {
            /* ----- Redraw Description & HALO Data by New Index ----- */
			P223RedrawText(hDlg, pRptData->table);

            /* ----- Redraw Scroll Box (Thumb) ----- */
            SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

            fReturn = TRUE;
        }
        break;

    case SB_THUMBTRACK:
        /* ----- Calculate Thumb Position & Top/Bottom Index ----- */
        unTopIndex    = HIWORD(wParam);
		unBottomIndex = unTopIndex + pScrollInfo->nPage - 1;

        /* ----- Redraw Description & HALO Data by New Index ----- */
		P223RedrawText(hDlg, pRptData->table);

        /* ----- Redraw Scroll Box (Thumb) ----- */
        SetScrollPos((HWND)lParam, SB_CTL, (int)unTopIndex, TRUE);

        fReturn = TRUE;
    }
    return fReturn;
}



/*
* ===========================================================================
**  Synopsis:   VOID    P20InitDlg();
*
**  Input   :   HWND   hDlg     - Handle of a DialogBox Procedure
*
**  Return  :   No Return Value
*
**  Description:
*       This function initialize the configulation of a DialogBox.
* ===========================================================================
*/
static VOID    P223InitDlg(HWND hDlg, P223_Rpt_Data  *pRptData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    WORD    wID;                        /* Control ID for Loop */
    short   nCount;

    /* ----- Set Top Index & Bottom Index ----- */
    unTopIndex    = P20_SCBAR_MIN;
    unBottomIndex = P20_ONE_PAGE;

    for (wID = IDD_P20_EDIT01; wID <= IDD_P20_EDIT20; wID++) {
        /* ----- Set Terminal Font to Each EditText ----- */
		EditBoxProcFactory (NULL, hDlg, wID);

        PepSetFont(hDlg, wID);

        /* ----- Set Limit Length to Each EditText ----- */
        SendDlgItemMessage(hDlg, wID, EM_LIMITTEXT, P20_MNE_LEN, 0L);
    }

    /* ----- Read Mnemonic Data from Parameter File ----- */
    ParaAllRead(CLASS_PARATRANSMNEMO, (UCHAR *)lpMnemonic, (P20_ADDR_MAX * (P20_MNE_LEN) * 2), 0, &usReturnLen);

    for (nCount = 0; nCount < P20_ADDR_MAX; nCount++) {
        /* ----- Replace Double Width Key (0x12 -> '~') ----- */
        PepReplaceMnemonic((WCHAR *)(lpMnemonic + nCount), (WCHAR *)(lpWork + nCount), (P20_MNE_LEN ), PEP_MNEMO_READ);
    }

	/* ----- Read Initial Data from Parameter File ----- */
	ParaAllRead(CLASS_PARAMDC, abMDCData, sizeof(abMDCData), 0, &usReturnLen);

	/* ----- Set Description to StaticText ----- */
	P223RedrawText(hDlg, pRptData->table);

#if 0
    /* ----- Set Minimum & Maximum Position of Vertical Scroll ----- */
    SetScrollRange(GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, P20_SCBAR_MIN, P20_SCBAR_MAX, TRUE);

    /* ----- Set the Position of ScrollBox(thumb) of Vertical Scroll ----- */
    SetScrollPos(GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, P20_SCBAR_MIN, FALSE);

	{
		SCROLLINFO scrollInfo = {0};
		scrollInfo.cbSize = sizeof (scrollInfo);
		scrollInfo.nPage = P20_PAGE_MAX;
		scrollInfo.nMin = P20_SCBAR_MIN;
		scrollInfo.nMax = P20_TOTAL_MAX - 1;
		scrollInfo.nPos = P20_SCBAR_MIN;
		scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
		SetScrollInfo (GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, &scrollInfo, TRUE);
	}

#else
	SetScrollInfo (GetDlgItem(hDlg, IDD_P20_SCBAR), SB_CTL, &(pRptData->scrollInfo), TRUE);
#endif
    /* ----- Set Focus to EditText in the Top of Item ----- */
    SetFocus(GetDlgItem(hDlg, IDD_P20_EDIT01));
    SendDlgItemMessage(hDlg, IDD_P20_EDIT01, EM_SETSEL, 0, MAKELONG(0, -1));
}

/*
* ===========================================================================
**  Synopsis:   BOOL    WINAPI  P020DlgProc()
*
**  Input   :   HWND    hDlg   - handle of a dialogbox procedure
*               WORD    msg    - dispacthed message
*               WORD    wParam - 16 bits message parameter
*               LONG    lParam - 32 bits message parameter
*
**  Return  :   TRUE           - user process is executed
*               FALSE          - default process is expected
*
**  Description:
*       This is a dialogbox procedure for 
*                  Transaction Mnemonics (Program Mode # 20)
* ===========================================================================
*/
static BOOL    WINAPI  P223DlgProcExt(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam, P223_Rpt_Data *pRptData)
{
    USHORT  usReturnLen;                /* Return Length of ParaAll */
    BOOL    fReturn;                    /* Return Value of Local */
    short   nCount;

    switch (wMsg) {
    case WM_INITDIALOG:
		if (pRptData->table == FinancialRpt) {
			SetWindowText (hDlg, L"Financial Report Setup");
		} else if (pRptData->table == CashiersRpt) {
			SetWindowText (hDlg, L"Cashier Report Setup");
		}
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Allocate Grobal Area for Mnemonic Data ----- */
        hGlbMnemo = GlobalAlloc(GHND, sizeof(P20MNEMO) * P20_ADDR_MAX * 2);
        hGlbWork  = GlobalAlloc(GHND, sizeof(P20WORK)  * P20_ADDR_MAX * 2);

        if ((! hGlbMnemo) || (! hGlbWork)){     /* Memory Allocation Error */
            /* ----- Text out Error Message ----- */
            P223DispAllocErr(hDlg, hGlbMnemo, hGlbWork);
            EndDialog(hDlg, TRUE);
        } else {                                /* Allocation was Success   */
            /* ----- Lock Global Allocation Area ----- */
            lpMnemonic = (LPP20MNEMO)GlobalLock(hGlbMnemo);
            lpWork     = (LPP20WORK)GlobalLock(hGlbWork);

			/* ----- Initialize Configulation of DialogBox ----- */
			P223InitDlg(hDlg, pRptData);
        }
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_P20_SCBAR; j<=IDD_P20_CAPTION; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDD_P20_EDIT01:
        case IDD_P20_EDIT02:
        case IDD_P20_EDIT03:
        case IDD_P20_EDIT04:
        case IDD_P20_EDIT05:
        case IDD_P20_EDIT06:
        case IDD_P20_EDIT07:
        case IDD_P20_EDIT08:
        case IDD_P20_EDIT09:
        case IDD_P20_EDIT10:
        case IDD_P20_EDIT11:
        case IDD_P20_EDIT12:
        case IDD_P20_EDIT13:
        case IDD_P20_EDIT14:
        case IDD_P20_EDIT15:
        case IDD_P20_EDIT16:
        case IDD_P20_EDIT17:
        case IDD_P20_EDIT18:
        case IDD_P20_EDIT19:
        case IDD_P20_EDIT20:
            if (HIWORD(wParam) == EN_CHANGE) {
                P223GetMnemo(hDlg, LOWORD(wParam), pRptData);
                return TRUE;
            }
            return FALSE;

        case IDD_P20_DESC01:
        case IDD_P20_DESC02:
        case IDD_P20_DESC03:
        case IDD_P20_DESC04:
        case IDD_P20_DESC05:
        case IDD_P20_DESC06:
        case IDD_P20_DESC07:
        case IDD_P20_DESC08:
        case IDD_P20_DESC09:
        case IDD_P20_DESC10:
        case IDD_P20_DESC11:
        case IDD_P20_DESC12:
        case IDD_P20_DESC13:
        case IDD_P20_DESC14:
        case IDD_P20_DESC15:
        case IDD_P20_DESC16:
        case IDD_P20_DESC17:
        case IDD_P20_DESC18:
        case IDD_P20_DESC19:
        case IDD_P20_DESC20:
            if (HIWORD(wParam) == BN_CLICKED) {
                P223GetMdc(hDlg, LOWORD(wParam), pRptData);
                return TRUE;
            }
            return FALSE;

        case IDOK:
            for (nCount = 0; nCount < P20_ADDR_MAX; nCount++) {
                /* ----- Replace Double Width Key ('~' -> 0x12) ----- */
                PepReplaceMnemonic((WCHAR *)(lpWork + nCount), (WCHAR *)(lpMnemonic + nCount), (P20_MNE_LEN), PEP_MNEMO_WRITE);
            }

            ParaAllWrite(CLASS_PARATRANSMNEMO, (UCHAR *)lpMnemonic, ((P20_MNE_LEN) * 2 * P20_ADDR_MAX), 0, &usReturnLen);
			ParaAllWrite(CLASS_PARAMDC, abMDCData, sizeof(abMDCData), 0, &usReturnLen);

            PepSetModFlag(hwndProgMain, PEP_MF_PROG, 0);
            /* ----- return TRUE; ----- not used */

        case IDCANCEL:
            /* ----- Unlock Global Allocated Area & Set Them Free ----- */
            GlobalUnlock(hGlbMnemo);
            GlobalFree(hGlbMnemo);

            GlobalUnlock(hGlbWork);
            GlobalFree(hGlbWork);

            EndDialog(hDlg, FALSE);

            return TRUE;
        }
        return FALSE;

    case WM_MOUSEWHEEL:
		{
			int     dWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			LPARAM  hScroll = (LPARAM)GetDlgItem (hDlg, IDD_P20_SCBAR);

			if (dWheel < 0) {
				/* ----- Vertical Scroll Bar Control ----- */
				fReturn = P223ScrlProc(hDlg, SB_LINEDOWN, hScroll, pRptData);
			} else if (dWheel > 0) {
				fReturn = P223ScrlProc(hDlg, SB_LINEUP, hScroll, pRptData);
			}
		}

        /* ----- An application should return zero ----- */
        return FALSE;

    case WM_VSCROLL:
        /* ----- Vertical Scroll Bar Control ----- */
        fReturn = P223ScrlProc(hDlg, wParam, lParam, pRptData);

        /* ----- An application should return zero ----- */
        return FALSE;

    default:
        return FALSE;
    }
}

BOOL    WINAPI  P223DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	P223_Rpt_Data x = {0};

	x.table = FinancialRpt;
	x.scrollInfo.cbSize = sizeof (x.scrollInfo);
	x.scrollInfo.nPage = P20_PAGE_MAX;
	x.scrollInfo.nMin = 0;
	x.scrollInfo.nMax = sizeof(FinancialRpt)/sizeof(FinancialRpt[0]) - 1;
	x.scrollInfo.nPos = x.scrollInfo.nMin;
	x.scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
	return P223DlgProcExt(hDlg, wMsg, wParam, lParam, &x);
}

BOOL    WINAPI  P223DlgProc_1(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	P223_Rpt_Data x = {0};

	x.table = CashiersRpt;
	x.scrollInfo.cbSize = sizeof (x.scrollInfo);
	x.scrollInfo.nPage = P20_PAGE_MAX;
	x.scrollInfo.nMin = 0;
	x.scrollInfo.nMax = sizeof(CashiersRpt)/sizeof(CashiersRpt[0]) - 1;
	x.scrollInfo.nPos = x.scrollInfo.nMin;
	x.scrollInfo.fMask = (SIF_PAGE | SIF_POS | SIF_RANGE);
	return P223DlgProcExt(hDlg, wMsg, wParam, lParam, &x);
}

/* ===== End of P223.C ===== */
