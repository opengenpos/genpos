/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : maint.h                                           :
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-05 : 00.00.00     :                         :
:   Update Histories   : 92-11-19 : 01.00.00     : Adds MaintTareRead,     :
:                      :          :              :      MaintTareWrite,    :
:                      :          :              :      MaintTareReport.   :
:                      : 92-11-24 : 01.00.00     : Adds PARATARE Work Area :
:                      :          :              :                         :
:                      :          :              : Adds MAINTOPESTATUS,    :
:                      :          :              :      MaintOpeStatus.    :
:                      : 93-06-21 : 01.00.12     : Chg. MaintFeedRec() Pro-:
:                      :          :              :      totype.            :
:                      : 93-06-22 : 02.00.00     : Add. PARAEMPLOYEENO     :
:                      :          :              : Add. MAINT_EMPL_MAX     :
:                      :          :              : Add. PARASHAREDPRT      :
:                      : 93-06-23 : 01.00.12     : Add MAINT_TERM_MAX      :
:                      :          :              : Add MaintSharedPrt()    :
:                      :          :              : Prototype               :
:                      : 93-06-24 : 01.00.12     : Add MaintShrTermLock()  :
:                      :          :              : Add MaintShrTermUnLock():
:                      : 93-07-02 : 01.00.12     : Add MaintPresetAmountRead(),
:                      :          :              : MaintPresetAmountWrite(),
:                      :          :              : MaintPresetAmountReport().
:                      : 93-07-08 : 01.00.12     : Adds CLASS_MAINTTRAILER_SENDSHR
:                      : 93-07-09 : 01.00.12     : Add MaintPigRuleRead(),
:                      :          :              : MaintPigRuleWrite(),
:                      :          :              : MaintPigRuleReport().
:                      : 93-07-22 : 01.00.12     : Chg "Return Value Define"
:                      :          :              : to MINUS VALUE
:                      : 93-08-23 : 01.00.13     : Adds Trailer Print for Soft Check
:                      : 93-08-25 : 01.00.13     : Adds MAINT_CRLF_LEN
:                      : 93-08-31 : 01.00.14     : Add MaintShrTermLockSup()
:                      :          :              : & MaintShrTermLockProg()
:                      : 93-10-05 : 02.00.01     : Adds MaintEtkFileXXXX()
:                      : 93-11-11 : 03.00.00     : Adds MaintStartStopCpm()
:                      : 95-02-07 : 03.00.00     : Adds PLU_FIELD14_ADR,PLU_FIELD15_ADR
:                      :          :              : Chg  From PLU_FIELD14_ADR To PLU_FIELD16_ADR
:                      :          :              : Chg  From PLU_FIELD15_ADR To PLU_FIELD17_ADR
:                      :          :              : Chg  From PLU_FIELD16_ADR To PLU_FIELD18_ADR
:                      :          :              : Chg  From PLU_FIELD17_ADR To PLU_FIELD19_ADR
:                      :          :              : Chg  From PLU_FIELD18_ADR To PLU_FIELD20_ADR
:                      : 95-03-01 : 03.00.00     : Add MAINTCPN,
:                      :          :              : Add PARACPN of MAINTWORK
:                      :          :              : Add Prototype of MATCPN.C
:                      :          :              : Add Return mnemonics setting status of CPN
:                      : 95-03-02 : 03.00.00     : Add COUPON FIELD NUMBER
:                      : 95-03-14 : 03.00.00     : Add MAINT_CPN_ADJ_ADR
:                      : 95-03-15 : 03.00.00     : Add uchCPNAdj of MAINTCPN
:                      : 95-04-20 : 03.00.00     : Add prototype VOID MaintCPNReport()
:                      : 95-07-19 : 03.00.00     : Add FDT definitions
:                      : 98-08-06 : 03.03.00     : Add CAS_FIELD_ADR
:                      : 99-08-11 : 03.05.00     : Remove WAITER_MODEL
:
: ** NCR2172 **
:                      : 99-08-26 : 01.00.00     : initial (for Win32)
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/

#include <regstrct.h>

#include "csetk.h"

/**
==============================================================================
;                  R A M   A R E A   D E C L A R A T I O Ns
=============================================================================
**/

extern UCHAR   uchMaintOpeCo;               /* operational counter */
extern UCHAR   uchMaintMldPageNo;           /* scroll display page */

/*------------------------------------------------------------------------*\

            D E F I N E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
*   UNIVERSAL DEFINITION
\*------------------------------------------------------------------------*/

#define MAINT_WITHOUT_DATA    0x01   /* Status Flag (without data) */
#define MAINT_DDMMYY_DATA     0x02   /* Status Flag (DD/MM/YY) */
#define MAINT_MILITARY_DATA   0x04   /* Status Flag (military time) */
#define MAINT_MSR_DATA        0x08   /* Status Flag (msr entry) */
#define MAINT_6DECIMAL_DATA   0x10   /* Status Flag (6 decimal FC Rate, V3.4) */
#define MAINT_SCANNER_INPUT   0x20   /* Status Flag (data input from scanner) 2172 */
#define MAINT_DATA_INPUT      0x40   /* Status Flag (data already inputted) 2172 */
#define MAINT_DATA_INPUT2     0x80   /* Status Flag (data already inputted) or used to indicate Connection Engine input origination  */
#define MAINT_DATA_INPUT_CE   0x80   /* Status Flag (data already inputted) or used to indicate Connection Engine input origination  */

// The following defined constants are used with global variable uchMaintOpeCo to
// indicate specific states within the maintenance and Supervisor Mode procedures.
#define MAINT_1STOPERATION    0x00   /* First Operation */
#define MAINT_ALREADYOPERATE  0x01   /* Already Operate */
#define MAINT_WRITECOMPLETE   0x02   /* Inputted Data has been writed to File */

#define MAINT_PRGSECRET_MAX   4   /* Max digits for program mode secret no.*/
#define MAINT_SUPSECRET_MAX   5   /* Max digits for supervisor mode secret no.*/

#define MAINT_BEEP           500

#define MAINT_SETTAXONWDC    0x01      /* check if tax on WDC is set */
#define MAINT_OVER2WDC       0x02      /* check if taxable amount is over 2*WDC */
#define MAINT_1STTAXTBL      0x04      /* check 1st operation */
#define MAINT_FEEDCTL        0x08      /* feed control flag */
#define MAINT_TAXABLE_CTL    0x10      /* check if 1st taxable amount is inputed */
#define MAINT_TAX_CTL        0x20      /* check if 1st tax amount is inputed */

#define MAINT_NOT_PRINT      0x01

#define MAINT_ALREADY_PRINT    0x01    /* for ETK File Maintenance */

#define MAINT_5DECIMAL       5         /* 5 decimal FC Rate, V3.4 */
#define MAINT_6DECIMAL       6         /* 6 decimal FC Rate, V3.4 */

/*------------------------------------------------------------------------*\
*   MAINTENANCE - MINOR DATA CLASS DEFINE
\*------------------------------------------------------------------------*/


#define CLASS_MAINTHEADER_RPT        1      /* print format 1 */
#define CLASS_MAINTHEADER_PARA       2      /* print format 2 */
#define CLASS_MAINTHEADER_NOTSEND    3      /* do not send header data to EJ file */

/* for MaintMakeTrailer */

#define CLASS_MAINTTRAILER_PRTPRG    4      /* program mode */
#define CLASS_MAINTTRAILER_PRTSUP    5      /* supervisor mode */
#define CLASS_MAINTTRAILER_NOTSEND   6      /* do not send trailer data to EJ file */
#define CLASS_MAINTTRAILER_SENDSHR   7      /* do not send trailer data to EJ file */
                                            /* but send trailer data to shared printer */
#define CLASS_MAINTTRAILER_PRTBKGND  8      /* back ground or some other thread than user interface */


/* for Department Edit, CLASS_MAINTDEPT */

#define CLASS_MAINTDEPT_READ        1       /* Read Case */
#define CLASS_MAINTDEPT_WRITE       2       /* Write Case */

/* for PLU Edit, CLASS_MAINTPLU */

#define CLASS_MAINTPLU_READ         1       /* Read Case */
#define CLASS_MAINTPLU_WRITE        2       /* Write Case */

/* for TOD, CLASS_MAINTTOD */

#define CLASS_MAINTTOD_READ         1       /* Set Address Case */
#define CLASS_MAINTTOD_WRITE        2       /* Set Data Case */

/* for Tax Table Edit, CLASS_MAINTTAXTBL */

#define CLASS_MAINTTAXTBL_PRTWRTINT       1   /* Print Tax Rate */
#define CLASS_MAINTTAXTBL_PRTRPTINT       2   /* Print Tax Rate */
#define CLASS_MAINTTAXTBL_PRTTAXABLE      3   /* Print Taxable & Tax Amount */
#define CLASS_MAINTTAXTBL_PRTTAX          4   /* Print Start & End Taxable Amount & Tax Amount */

#define CLASS_MAINTTAXTBL_DISP0_INT       5
#define CLASS_MAINTTAXTBL_DISP1_INT       6   /* Display Tax Rate for Tax Table #1 */
#define CLASS_MAINTTAXTBL_DISP2_INT       7
#define CLASS_MAINTTAXTBL_DISP2_TAXABL    8
#define CLASS_MAINTTAXTBL_DISP2_TAX       9
#define CLASS_MAINTTAXTBL_DISP3_TAXABL   10
#define CLASS_MAINTTAXTBL_DISP4_INT      11
#define CLASS_MAINTTAXTBL_DISP4_TAX      12

/* for Loan/Pickup Function */
#define CLASS_MAINTTENDER1        CLASS_TENDER1    /* Tender 1(Cash) */
#define CLASS_MAINTTENDER2        CLASS_TENDER2    /* Tender 2(Food) */
#define CLASS_MAINTTENDER3        CLASS_TENDER3    /* Tender 3(Charge) */
#define CLASS_MAINTTENDER4        CLASS_TENDER4    /* Tender 4(Misc1) */
#define CLASS_MAINTTENDER5        CLASS_TENDER5    /* Tender 5(Misc2) */
#define CLASS_MAINTTENDER6        CLASS_TENDER6    /* Tender 6(Misc3) */
#define CLASS_MAINTTENDER7        CLASS_TENDER7    /* Tender 7(Misc4) */
#define CLASS_MAINTTENDER8        CLASS_TENDER8    /* Tender 8(Misc5) */
#define CLASS_MAINTTENDER9        CLASS_TENDER9    /* Tender 9(Misc6) */
#define CLASS_MAINTTENDER10       CLASS_TENDER10   /* Tender 10(Misc7) */
#define CLASS_MAINTTENDER11       CLASS_TENDER11   /* Tender 11(Misc8) */
#define CLASS_MAINTTENDER12       CLASS_TENDER12   /* Tender 12(Misc8) */
#define CLASS_MAINTTENDER13       CLASS_TENDER13   /* Tender 13(Misc8) */
#define CLASS_MAINTTENDER14       CLASS_TENDER14   /* Tender 14(Misc8) */
#define CLASS_MAINTTENDER15       CLASS_TENDER15   /* Tender 15(Misc8) */
#define CLASS_MAINTTENDER16       CLASS_TENDER16   /* Tender 16(Misc8) */
#define CLASS_MAINTTENDER17       CLASS_TENDER17   /* Tender 17(Misc8) */
#define CLASS_MAINTTENDER18       CLASS_TENDER18   /* Tender 18(Misc8) */
#define CLASS_MAINTTENDER19       CLASS_TENDER19   /* Tender 19(Misc8) */
#define CLASS_MAINTTENDER20       CLASS_TENDER20   /* Tender 20(Misc8) */
#define CLASS_MAINTFOREIGN1       CLASS_FOREIGN1   /* F/C Tender 1, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN2       CLASS_FOREIGN2   /* F/C Tender 2, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN3       CLASS_FOREIGN3   /* F/C Tender 3, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN4       CLASS_FOREIGN4   /* F/C Tender 4, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN5       CLASS_FOREIGN5   /* F/C Tender 5, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN6       CLASS_FOREIGN6   /* F/C Tender 6, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN7       CLASS_FOREIGN7   /* F/C Tender 7, should be same as CLASS_FOREIGN1 */
#define CLASS_MAINTFOREIGN8       CLASS_FOREIGN8   /* F/C Tender 8, should be same as CLASS_FOREIGN1 */

#define CLASS_MAINTCASIN                 91   /* Cashier Sign In */
#define CLASS_MAINTCASOUT                92   /* Cashier Sign Out */
#define CLASS_MAINTOTHER                 93   /* VOID, E/C, For, Qty */
#define CLASS_MAINTCOUNTUP               94   /* Loan/Pickup Count Up */

/*------------------------------------------------------------------------*\
     Loan/Pickup Define
\*------------------------------------------------------------------------*/

#define MAINT_MODIFIER_VOID      0x0008                  /* VOID */
#define MAINT_MODIFIER_EC        0x0080                  /* E/C */
#define MAINT_MODIFIER_QTY       0x1000                  /* QTY */

#define MAINT_AMOUNTENTRY        1                       /* slip control */
#define MAINT_HEADTRAIL          2                       /* slip control */

/* for Receipt/Journal Feed */

#define CLASS_MAINTFEED_INSIDE           1   /* Inside Transaction */
#define CLASS_MAINTFEED_OUTSIDE          2   /* Outside Transaction */


/* for Control Descriptor and Redisplay */

#define CLASS_MAINTREDISP_VOID           1   /* Void Case */
#define CLASS_MAINTREDISP_TRAN           2   /* Training Case */
#define CLASS_MAINTREDISP_ALPHA          3   /* Alpha Case */
#define CLASS_MAINTREDISP_CLR            4   /* All Clear Case */
#define CLASS_MAINTREDISP_CLRWOALPH      5   /* All Clear W/o ALPHA Case */
#define CLASS_MAINTREDISP_NORECEIPT      6   /* No Receipt Case */
#define CLASS_MAINTREDISP_RECEIPT        7   /* Cansel Of No Receipt Case */
#define CLASS_MAINTREDISP_PAGEDISP       8   /* Page No Control Case */

/* for Control Display */

#define CLASS_MAINTDSPCTL_10N            1   /* 10N10D Case */
#define CLASS_MAINTDSPCTL_10NPG          2   /* 10N10D Case, for Page Number */
#define CLASS_MAINTDSPCTL_AMT            3   /* Amount with Decimal Point Case */

/* for Mode-in Display */

#define CLASS_MAINTPREMD_OP              1   /* Operator Display */
#define CLASS_MAINTPREMD_CUS             2   /* Customer Display */
#define CLASS_MAINTPREMD_OPPROG          3   /* Operator Display (Prog.) (V3.3) */

/* for Cashier Number Setting */

#define CLASS_PARACASHIERNO_DATA1    1   /* Operator Status #1 */
#define CLASS_PARACASHIERNO_DATA2    2   /* Operator Status #2 */
#define CLASS_PARACASHIERNO_DATA3    3   /* Operator Status #3 */
#define CLASS_PARACASHIERNO_DATA4    4   /* Operator Status #4 */
#define CLASS_PARACASHIERNO_DATA5    5   /* Operator Status #5 ***PDINU*/
#define CLASS_PARACASHIERNO_DATA6    6   /* Guest Check Number From */
#define CLASS_PARACASHIERNO_DATA7    7   /* Guest Check Number To */
#define CLASS_PARACASHIERNO_DATA8    8   /* Charge Tip Rate */
#define CLASS_PARACASHIERNO_DATA9    9   /* Team Number */
#define CLASS_PARACASHIERNO_DATA10    10   /* Allowed Terminal Number */

#define CLASS_PARACASHIERNO_MNEMO   11   /* Indicate Cashier Name Data */
#define CLASS_PARACASHIERNO_READ    12   /* Indicate Read Cashier Data */
#define CLASS_PARACASHIERNO_WRITE   13   /* Indicate Write Cashier Data */
#define CLASS_PARACASHIERNO_STS     14   /* Indicate Status of Cashier Data */

/*------------------------------------------------------------------------*\
     Return Value Define
\*------------------------------------------------------------------------*/

#define MAINT_PTD_EXIST         -101                   /* Return PTD Flag Exist Status From MaintFlexMemEdit() */
#define MAINT_FSC_EXIST         -102                   /* Return Extended FSC Exist Status From MaintFSCEdit() */
#define MAINT_DEPT_MNEMO_ADR    -103                   /* Return Mnemonics Setting Status at DEPT Maintenance */
#define MAINT_PLU_MNEMO_ADR     -104                   /* Return Mnemonics Setting Status at PLU Maintenance */
#define MAINT_PLU_CPYDEPT_ADR   -105                   /* Return Copy DEPT Status at PLU Maintenance */
#define MAINT_2WDC_OVER         -106                   /* Return Double WDC Status at Tax Table Programming */

#define MAINT_RND_DELI          -107                   /* Return Next Input Mode is Delimitor Mode */
#define MAINT_RND_POSI          -108                   /* Return Next Input Mode is Position Mode */

#define MAINT_CPN_MNEMO_ADR     -109                   /* Return Mnemonics Setting Status at CPN Maintenance */
#define MAINT_CPN_ADJ_ADR       -110                   /* Return Adjective Setting Status at CPN Maintenance */

#define MAINT_EMPLOYEE_MNEMO_ADR -111                  /* Return Mnemonics Setting Status at ETK Maintenance */

#define MAINT_ADJECTIVE_EXIST   -112                   /* Return Adjective Code Exist Status From MaintFlexMemEdit() */

#define MAINT_CAS_MNEMO_ADR -113               /* Return Mnemonics Setting at Cashier Maintenance */
#define MAINT_CAS_STSGC_ADR -114               /* Return Start Guest Check Number Setting at Cashier Maintenance */

/*------------------------------------------------------------------------*\
     Department Field Address Define (A/C 114)
\*------------------------------------------------------------------------*/

#define DPT_FIELD1_ADR          1
#define DPT_FIELD2_ADR          2
#define DPT_FIELD3_ADR          3
#define DPT_FIELD4_ADR          4
#define DPT_FIELD5_ADR          5
#define DPT_FIELD6_ADR          6
#define DPT_FIELD7_ADR          7
#define DPT_FIELD8_ADR          8
#define DPT_FIELD9_ADR          9
#define DPT_FIELD10_ADR        10
#define DPT_FIELD11_ADR        11
#define DPT_FIELD12_ADR        12
#define DPT_FIELD13_ADR        13
#define DPT_FIELD14_ADR        14 /* 2172 */

/*------------------------------------------------------------------------*\
     PLU Field Address Define
\*------------------------------------------------------------------------*/

#define PLU_FIELD0_ADR          0      /* for PLUType */
#define PLU_FIELD1_ADR          1
#define PLU_FIELD2_ADR          2
#define PLU_FIELD3_ADR          3
#define PLU_FIELD4_ADR          4
#define PLU_FIELD5_ADR          5
#define PLU_FIELD6_ADR          6
#define PLU_FIELD7_ADR          7
#define PLU_FIELD8_ADR          8
#define PLU_FIELD9_ADR          9
#define PLU_FIELD10_ADR        10
#define PLU_FIELD11_ADR        11
#define PLU_FIELD12_ADR        12
#define PLU_FIELD13_ADR        13
#define PLU_FIELD14_ADR        14
#define PLU_FIELD15_ADR        15
#define PLU_FIELD16_ADR        16
#define PLU_FIELD17_ADR        17
#define PLU_FIELD18_ADR        18
#define PLU_FIELD19_ADR        19
#define PLU_FIELD20_ADR        20
#define PLU_FIELD21_ADR        21
#define PLU_FIELD22_ADR        22
#define PLU_FIELD23_ADR        23
#define PLU_FIELD24_ADR        24 /* 2172 */
#define PLU_FIELD25_ADR        25 /* 2172 */
#define PLU_FIELD26_ADR        26 /* 2172 */
#define PLU_FIELD27_ADR        27 /* 2172 */
#define PLU_FIELD28_ADR        28 /* 2172 */


/*------------------------------------------------------------------------*\
     COUPON Field Address Define
\*------------------------------------------------------------------------*/

#define CPN_FIELD0_ADR          0
#define CPN_FIELD1_ADR          1
#define CPN_FIELD2_ADR          2
#define CPN_FIELD3_ADR          3
#define CPN_FIELD4_ADR          4
#define CPN_FIELD5_ADR          5
#define CPN_FIELD6_ADR          6
#define CPN_FIELD7_ADR          7
#define CPN_FIELD8_ADR          8
#define CPN_FIELD9_ADR          9
#define CPN_FIELD10_ADR        10
#define CPN_FIELD11_ADR        11
#define CPN_FIELD12_ADR        12
#define CPN_FIELD13_ADR        13
#define CPN_FIELD14_ADR        14
#define CPN_FIELD15_ADR        15
#define CPN_FIELD16_ADR        16
#define CPN_FIELD17_ADR        17
#define CPN_FIELD18_ADR        18
#define CPN_FIELD19_ADR        19
#define CPN_FIELD20_ADR        20
#define CPN_FIELD21_ADR		   21
#define CPN_FIELD22_ADR		   22

/*------------------------------------------------------------------------*\
     ETK Field Address Define
\*------------------------------------------------------------------------*/

#define ETK_FIELD0_ADR          0
#define ETK_FIELD1_ADR          1
#define ETK_FIELD2_ADR          2
#define ETK_FIELD3_ADR          3
#define ETK_FIELD4_ADR          4

/*------------------------------------------------------------------------*\
     FLEXIBLE DRIVE THROUGH PARAMERTER Field Address Define
\*------------------------------------------------------------------------*/

#define FXDRV_FIELD0_ADR          0
#define FXDRV_FIELD1_ADR          1
#define FXDRV_FIELD2_ADR          2
#define FXDRV_FIELD3_ADR          3
#define FXDRV_FIELD4_ADR          4
#define FXDRV_FIELD5_ADR          5
#define FXDRV_FIELD6_ADR          6
#define FXDRV_FIELD7_ADR          7
#define FXDRV_FIELD8_ADR          8
#define FXDRV_FIELD9_ADR          9
#define FXDRV_FIELD10_ADR        10
#define FXDRV_FIELD11_ADR        11
#define FXDRV_FIELD12_ADR        12
#define FXDRV_FIELD13_ADR        13
#define FXDRV_FIELD14_ADR        14
#define FXDRV_FIELD15_ADR        15
#define FXDRV_FIELD16_ADR        16
#define FXDRV_FIELD17_ADR        17
#define FXDRV_FIELD18_ADR        18
#define FXDRV_FIELD19_ADR        19
#define FXDRV_FIELD20_ADR        20
#define FXDRV_FIELD21_ADR        21
#define FXDRV_FIELD22_ADR        22
#define FXDRV_FIELD23_ADR        23
#define FXDRV_FIELD24_ADR        24
#define FXDRV_FIELD25_ADR        25
#define FXDRV_FIELD26_ADR        26
#define FXDRV_FIELD27_ADR        27
#define FXDRV_FIELD28_ADR        28
#define FXDRV_FIELD29_ADR        29
#define FXDRV_FIELD30_ADR        30
#define FXDRV_FIELD31_ADR        31
#define FXDRV_FIELD32_ADR        32
#define FXDRV_FIELD33_ADR        33
#define FXDRV_FIELD34_ADR        34
#define FXDRV_FIELD35_ADR        35
#define FXDRV_FIELD36_ADR        36

/*------------------------------------------------------------------------*\
     Cashier Field Address Define
\*------------------------------------------------------------------------*/

#define CAS_FIELD1_ADR          1
#define CAS_FIELD2_ADR          2
#define CAS_FIELD3_ADR          3
#define CAS_FIELD4_ADR          4
#define CAS_FIELD5_ADR          5
#define CAS_FIELD6_ADR          6
#define CAS_FIELD7_ADR          7
#define CAS_FIELD8_ADR          8
#define CAS_FIELD9_ADR          9
#define CAS_FIELD10_ADR        10
#define CAS_FIELD11_ADR		   11

/* Warning !!! You have to change this Define When Above Max. Field is changed */
/* But Max. Field is not CAS_ENDGCNO_ADR */

#define CAS_FIELD_MAX           11     /* Max Field */

/*------------------------------------------------------------------------*\
     Range Define
\*------------------------------------------------------------------------*/

#define MAINT_HALO_MAX          79
#define MAINT_MDEPT_MAX         30                  /* Saratoga */
#define MAINT_CASH_MAX     99999999L                 /* Cashier No. Max Value */
#define MAINT_WAIT_MAX          999                 /* Waiter No. Max Value */
#define MAINT_ETK_MAX    999999999L                 /* Employee No. Max Value R3.1 */
#define MAINT_DISC_MAX          100                 /* Discount Rate Max Value */

//----     P18 Printer Feed and EJ Control parameters    ----
#define MAINT_MAXFEED_MAX        99                 /* Max Feed Line # of GC for Slip Printer - P18, address 1 */
#define MAINT_MAXLINE_MAX        31                 /* Max Line up to 1st Print Line - P18, address 2 */
#define MAINT_VALIDATION_MIN      0                 /* Minimum Number of Times to Allow Validation SR47 - P18, address 3 */
#define MAINT_VALIDATION_MAX      9                 /* Max Number of Times to Allow Validation SR47 - P18, address 3 */
#define MAINT_EJLEN_MIN          50                 /* Minimum electoric journal line for 1 page - P18, address 4 */
#define MAINT_EJLEN_MAX         150                 /* Max electoric journal line for 1 page - P18, address 4 */
#define MAINT_REVEJLEN_MAX       99                 /* Max electoric journal line for Reverse func. - P18, address 5 */
#define MAINT_NEARFULL_MAX       99                 /* Max near full % for electoric journal file - P18, address 6 */

#define MAINT_CTLPG_MAX           9                 /* Max Default Page, R3.1 */
#define MAINT_CTLPG_MIN           1                 /* Minimum Default Page */
#define MAINT_CTLPG_ALLOW         0                 /* Allow Menu Page */
#define MAINT_CTLPG_PROHIBIT      1                 /* Prohibit Menu Page */
#define MAINT_PAGECTL_MAX         1                 /* Max Page Control Value */
#define MAINT_CTLPG_ADJ_MAX       5                 /* Max Adjective Page, 2172 */
#define MAINT_TLTY_MAX           167                 /* Total Key Type & Index Max Value */
#define MAINT_TLTY_MIN           21                 /* Total Key Type & Index Min Value */
#define MAINT_POSITION_MAX        3                 /* Round Position Max Value */
#define MAINT_POSITION_MIN       -1                 /* Round Position Min Value */
#define MAINT_MINORFSC_DEPT      50                 /* Dept No Max. Value,  Saratoga */
#define MAINT_HOURBLK_MAX        48                 /* Max Hourly Block, V3.3 */
#define MAINT_SUPERNO_MIN       100                 /* Minimum Supervisor Number */
#define MAINT_SUPERNO_MAX       999                 /* Max Supervisor Number */
#define MAINT_TAXEXEMPT_MAX   65535                 /* Max Tax Exempt Limit Value */
#define MAINT_TAXTBL_MUSTSIZE     8
#define MAINT_TARE001_MAX       999                 /* Max Tare (minimum unit is 0.01) */
#define MAINT_TARE0001_MAX     9999                 /* Max Tare (minimum unit is 0.001 or 0.005) */
#define MAINT_PIG_COLIMIT_MAX    9L                 /* Max Counter Limit for Non GST Tax */
#define MAINT_FXDRV_SYSTEM_MAX    2                 /* Max Data for FDT System Selection */
#define MAINT_FXDRV_TERM_MAX     13                 /* Max Data for FDT Each Terminal Setting */
#define MAINT_SERVTIME_MAX     9999                 /* Max Data for Service Time Border */
#define MAINT_LABORCOST_MAX   65535                 /* Max Data for Labor Cost Value */
#define MAINT_TEAMNO_MAX          9                 /* MAX Team Number */
#define MAINT_CHGTIP_MAX          3                 /* MAX Charg Tip Rate */

#define MAINT_KDS_IP_LEN           3                 /* length of 1 block for IP address */
#define MAINT_KDS_IP_NUM           4                 /* number of block for IP address */
#define MAINT_KDS_IP_MAX          MAINT_KDS_IP_LEN*MAINT_KDS_IP_NUM /* MAX IP Address for Input */
#define MAINT_KDS_PORT_MAX         4                 /* MAX port no. for input */

#define MAINT_BOUNDAGE_MAX        100                 /* Max Boundary Age */
#define MAINT_FOODDENOMI_MAX     9999                 /* Max Food Stamp Denomination */

/*------------------------------------------------------------------------*\
     Feed Define
\*------------------------------------------------------------------------*/

#define MAINT_DEFAULT_FEED      1L              /* Number of Default Feed at Report */
#define MAINT_JOUKEYDEF_FEED    4L              /* Number of Default Feed at Journal Key */
#define MAINT_RECKEYDEF_FEED    8L              /* Number of Default Feed at Receipt Key */

/*------------------------------------------------------------------------*\
     Cashier/Waiter Control Bit Define
\*------------------------------------------------------------------------*/

#define MAINT_TRAIN_BIT         0x0001          /* Indicate Training Cashier */
#define MAINT_HEADCAS_BIT       0x0001          /* Indicate Training Cashier */
/*------------------------------------------------------------------------*\
     Tax Table Define
\*------------------------------------------------------------------------*/

#define MAINT_RATE_LEN               5                  /* Tax Table for (WDC + Tax Rate) */

/*------------------------------------------------------------------------*\
     PC I/F Define   (PROG# 39)
\*------------------------------------------------------------------------*/

#define MAINT_CRLF_LEN               2                  /* for Modem Strings */

/*------------------------------------------------------------------------*\
    OEP Status Define ( Super.160)
\*------------------------------------------------------------------------*/

#define MAINT_STS_TBLNUMBER          1                  /* When Table Number Inputed */
#define MAINT_STS_TBLADDRESS         2                  /* When Table Address Inputed */
#define MAINT_STS_TBLDATA            3                  /* When Table Number Inputed */
#define MAINT_STS_TBLADDINC          4                  /* When W/O data and Increment Address */

/*------------------------------------------------------------------------*\
    CSTR Define ( Super.33)
\*------------------------------------------------------------------------*/

#define MAINT_FSC_BIAS             100     /* Control Strings Programable FSC Base Code */
#define MAINT_NUM_BIAS            0x30     /* Control Strings Numeric Base Code */
#define MAINT_CSTR_NUMERIC_MAX       9     /* Control Strings Numeric Max Value */
#define MAINT_CSTR_ADR_MAX         MAX_PARACSTR_SIZE     /* Control Strings Address Max Value */
#define MAINT_CSTR_PAUSE_MAX        20     /* Pause Number Max Value */
#define MAINT_CSTR_MODE_MIN          2     /* Mode Switch Min Value */
#define MAINT_CSTR_MODE_MAX          5     /* Mode Switch Max Value */

/*------------------------------------------------------------------------*\
    FSC Page Number ( Super. Shift Key Inputed Case )
\*------------------------------------------------------------------------*/

#define MAINT_PAGE_1                 1     /* FSC Key Page 1 */
#define MAINT_PAGE_2                 2     /* FSC Key Page 2 */
#define MAINT_PAGE_3                 3     /* FSC Key Page 3 */
#define MAINT_PAGE_4                 4     /* FSC Key Page 4 */
#define MAINT_PAGE_5                 5     /* FSC Key Page 5 */

/*------------------------------------------------------------------------*\
    PPI Define ( Super.71)
\*------------------------------------------------------------------------*/

#define MAINT_PPI_NO_MAX           300     /* PPI Code Max Value ESMITH */
#define MAINT_PPI_QTY_MAX          255     /* PPI QTY Max Value */
#define MAINT_PPI_PRICE_MAX     999999     /* PPI Price Max Value */

// Used in areas dealing with Supervisor Numbers
#define MAINT_SUP_LEVEL_NUMBER_MIN   101    // Minimum internal Supervisor Number allowed
#define MAINT_SUP_LEVEL_NUMBER_MAX   999    // Maximum internal Supervisor Number allowed

#define MAINT_SUP_GET_SECRET_NUMBER(x) (USHORT)((x)%100L)   /* isolate the two digit secret code from an internal Supervisor Number */
#define MAINT_SUP_GET_SUPER_NUMBER(x) (USHORT)((x)/100L)    /* remove the two digit secret code from an internal Supervisor Number */

#define MAINT_EMP_MASK_DIGITS_KEEP(x,n) ((x)%((n)*10L))     /* mask digits providing specified number of rightmost digits counting from right or least significant */


/*------------------------------------------------------------------------*\

            T Y P E D E F     S T R U C T U R Es

\*------------------------------------------------------------------------*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/*--- for print header ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    WCHAR   aszReportName1[PARA_REPORTNAME_LEN+1];    /* Report Name */
    WCHAR   aszReportName2[PARA_REPORTNAME_LEN+1];    /* Report Name */
    WCHAR   aszSpeMnemo[PARA_SPEMNEMO_LEN+1]; /* Special Mnemonics */
    WCHAR   aszReportName3[PARA_REPORTNAME_LEN+1];    /* Report Name */
    UCHAR   uchReportType;          /* Report Type */
    USHORT  uchPrgSupNo;            /* Program, Super Number */
    UCHAR   uchResetType;           /* Reset Type */
    USHORT  usPrtControl;           /* Printer Control */
}MAINTHEADER;

/*--- for print trailer ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    WCHAR   aszDayRstMnemo[PARA_SPEMNEMO_LEN+1];  /* Daily Reset Counter Mnemonics */
    USHORT  usDayRstCount;          /* Daily reset Counter */
    WCHAR   aszPTDRstMnemo[PARA_SPEMNEMO_LEN+1];  /* PTD Reset Counter Mnemonics */
    USHORT  usPTDRstCount;          /* PTD Reset Counter */
    WCHAR   aszPrgCountMnemo[PARA_SPEMNEMO_LEN+1];  /* Program Counter Mnemonics */
    USHORT  usPrgCount;             /* Program Counter */
    ULONG   ulSuperNumber;          /* Supervisor Number */
    USHORT  usStoreNumber;          /* Store Number */
    USHORT  usRegNumber;            /* Register Number */
    USHORT  usConsCount;            /* Consecutive Counter */
    USHORT  usMin;                  /* Minute */
    USHORT  usHour;                 /* Hour */
    USHORT  usMDay;                 /* Day */
    USHORT  usMonth;                /* Month */
    USHORT  usYear;                 /* Year */
    UCHAR   uchStatus;              /* Control Status */
    USHORT  usPrtControl;           /* Printer Control */
}MAINTTRAILER;

/*--- for abort ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    WCHAR   aszTransMnemo[PARA_TRANSMNEMO_LEN+1]; /* Transaction Mnemonics */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrtControl;
}MAINTTRANS;

/*--- for error ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    SHORT   sErrorCode;             /* Error Code */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrtControl;           /* Printer Control */
}MAINTERRORCODE;

/*--- program/super mode in ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    ULONG   ulModeInNumber;         /* Mode In Number */
    WCHAR   aszModeInNumber[7];     /* Mode In Number for MSR R3.1 */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
}MAINTMODEIN;

/*--- read/write/edit TOD ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    UCHAR   uchAddress;             /* Address */
    LONG    lTODData;               /* Date/Time Data */
    UCHAR   uchStrLen;              /* String Length */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrtControl;           /* Printer Control */
}MAINTTOD;

/*--- check supervisor security level ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT  usSuperNo;              /* Supervisor Number */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
}MAINTENT;

/*--- set menu page no. in maintenance-work ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    UCHAR   uchDescriptor;          /* Descriptor */
    UCHAR   uchACNumber;            /* A/C No. for Display */
    UCHAR   uchPageNumber;          /* Menu Page Number */
    UCHAR   uchPTDType;             /* PTD Type */
    UCHAR   uchRptType;             /* Report Type */
    UCHAR   uchRstType;             /* Reset Type */
    ULONG   ulAmount;               /* Amount Data */
    UCHAR   uchLeadAddr;            /* Lead Through Address */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
}MAINTSETPAGE;

/*--- receipt/journal feed ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    LONG    lFeedAmount;            /* Number of Feed Line */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrtControl;           /* Print Control */
}MAINTFEED;

/*--- descriptor/page/leadthrough/program/action control ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT  usPGACNumber;           /* Page , Action Number */
    USHORT  usDescriptor;           /* Descriptor */
    UCHAR   uchPageNumber;          /* Page Number */
    UCHAR   uchPTDType;             /* PTD Type */
    UCHAR   uchRptType;             /* Report Type */
    UCHAR   uchResetType;           /* Reset Type */
    ULONG   ulAmount;               /* amount */
    WCHAR   aszMnemonics[PARA_LEADTHRU_LEN+1];
}MAINTDSPCTL;

/*--- display any mnemonics to any position ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    ULONG   ulData;                 /* Data */
    WCHAR   aszMnemonics[PARA_LEADTHRU_LEN+1]; /* Mnemonics */
    USHORT  usPrtControl;                       /* Printer Control */
}MAINTDSPFLINE;

/*--- edit Tax Table  ---*/

typedef struct {
    UCHAR   uchMajorClass;                      /* Major Data Class Definition */
    UCHAR   uchMinorClass;                      /* Minor Data Class Definition */
    UCHAR   uchInteger;                         /* Integer of Tax Rate */
    UCHAR   uchNumerator;                       /* Numerator of Tax Rate */
    UCHAR   uchDenominator;                     /* Denominator of Tax Rate */
    ULONG   ulStartTaxableAmount;               /* Start Taxable Amount */
    ULONG   ulEndTaxableAmount;                 /* End Taxable Amount */
    WCHAR   aszMnemonics[PARA_LEADTHRU_LEN + 1];/* Lead Through */
    USHORT  usTaxAmount;                        /* Tax Amount */
    UCHAR   uchStatus;                          /* 0=with data, 1=without data */
    USHORT  usPrtControl;                       /* Printer Control */
}MAINTTAXTBL;

/*--- edit DEPT  ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT  usACNumber;             /* A/C Number */
    UCHAR   uchFieldAddr;           /* Field Address of DEPT */
    USHORT  usDEPTNumber;           /* Department Number */
    ULONG   ulInputData;            /* Input Data */
    WCHAR   aszMnemonics[PARA_DEPT_LEN+1];  /* saratoga */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
} MAINTDEPT;

/*--- edit PLU  ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT  usACNumber;             /* A/C Number */
    UCHAR   uchFieldAddr;           /* Field Address of PLU */
    WCHAR   auchPLUNumber[PLU_MAX_DIGIT]; /* PLU Number, 2172 */
    ULONG   ulInputData;            /* Input Data */
    WCHAR   aszMnemonics[PARA_PLU_LEN+1];   /* Saratoga */
    UCHAR   uchPageNumber;          /* Page Number */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    UCHAR   uchEVersion;             /* E-version, 2172 */
}MAINTPLU;

/*--- edit CPN  ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT  usACNumber;             /* A/C Number */
    UCHAR   uchFieldAddr;           /* Field Address of CPN */
    UCHAR   uchCPNNumber;           /* CPN Number */
    UCHAR   uchCPNAdj;              /* Adjective Code of PLU */
    ULONG   ulInputData;            /* Input Data */
    WCHAR   aszMnemonics[PARA_LEADTHRU_LEN+1];
    WCHAR   auchCPNTarget[PLU_MAX_DIGIT+1]; /* w/ scan code, 2172 */
    UCHAR   uchPageNumber;          /* Page Number */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
}MAINTCPN;

/*--- print set menu of PLU ---*/

typedef struct  {
        UCHAR   uchMajorClass;          /* Major Class Data Definition */
        UCHAR   uchMinorClass;          /* Minor Class Data Definition */
        UCHAR   uchArrayAddr;           /* MENUPLU Array Address */
        MENUPLU SetPLU;                 /* PLU Number and Adjective Number */
        WCHAR   aszPLUMnemonics[PARA_PLU_LEN+1];    /* PLU Mnemonics */
        UCHAR   uchPageNumber;          /* Page Number */
        UCHAR   uchStatus;              /* 0=with data, 1=without data */
        USHORT  usPrintControl;         /* Print Control */
}MAINTMENUPLUTBL;

/*--- set hourly Time ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchAddress;            /* Address */
        USHORT usHourTimeData;        /* Number of Block / Time Data */
        UCHAR  uchStatus;             /* 0=with data, 1=without data */
}MAINTHOURLYTIME;

/*--- broadcast/request to download parameter ---*/

typedef struct {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        UCHAR  uchTermNo;             /* Terminal Number */
        USHORT usRegNo;               /* Register Number */
        UCHAR  uchBcasStatus;         /* Communication Status */
        USHORT usPrtControl;          /* Print Control */
}MAINTBCAS;

/*--- control descriptor and redisplay ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
}MAINTREDISP;

/*--- Operator/Guest Check Status Report ---*/

typedef struct  {
        UCHAR  uchMajorClass;         /* Major Class Data Definition */
        UCHAR  uchMinorClass;         /* Minor Class Data Definition */
        ULONG  ulOperatorId;          /* Cashier/Server/GCF Number */
        USHORT usPrintControl;        /* Print Control */
}MAINTOPESTATUS;

typedef struct  {
        UCHAR   uchMajorClass;         /* Major Class Data Definition */
        UCHAR   uchMinorClass;         /* Minor Class Data Definition */
		USHORT  usPrtControl;         /* Printer Control */
        WCHAR   aszPrintLine[60];      /* Print Control */
} MAINTPRINTLINE;

/*--- edit CSTR  ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    UCHAR   uchAddr;                /* Address of CSTR */
    UCHAR   uchACNumber;            /* ACNumber of CSTR */
    USHORT  usCstrNumber;           /* CSTR Number */
    WCHAR   aszMnemonics[PARA_LEADTHRU_LEN+1];
    UCHAR   uchInputData;            /* Input Data */
    UCHAR   uchMajorCstrData;       /* FSC Data */
    UCHAR   uchMinorCstrData;       /* EXTEND FSC Data */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrintControl;        /* Print Control */
}MAINTCSTR;

/*--- edit PPI  ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT   uchAddr;                /* Address of CSTR */
    UCHAR   uchACNumber;            /* ACNumber of CSTR */
    USHORT   uchPPINumber;           /* CSTR Number */
    WCHAR   aszMnemonics[PARA_LEADTHRU_LEN+1];
    ULONG   ulInputData;            /* Input Data */
    UCHAR   uchQTY;                 /* QTY Data */
    ULONG   ulPrice;                /* Sales Price Data */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrintControl;        /* Print Control */
}MAINTPPI;

/*--- ETK File Maintenance --- */
typedef struct {
        UCHAR       uchMajorClass;                       /* Major Class Data Definition */
        UCHAR       uchMinorClass;                       /* Minor Class Data Definition */
        ULONG       ulEmployeeNo;                        /* Employee Number R3.1 */
        USHORT      usBlockNo;                           /* Block Number */
		ETK_FIELD   EtkField;
        WCHAR       aszTransMnemo[PARA_TRANSMNEMO_LEN+1]; /* Transaction Mnemonics */
        WCHAR       aszMnemonics[PARA_LEADTHRU_LEN+1];   /* Mnemonics */
        UCHAR       uchStatus;                           /* Status */
        USHORT      usPrintControl;                      /* Print Control */
}MAINTETKFL;

/*--- Maint Cpm -----*/
typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    UCHAR   uchType;                /* Type Start or Stop */
    UCHAR   uchStatus;              /* 0=with data, 1=without data */
    USHORT  usPrtControl;           /* Printer Control */
} MAINTCPM;


/*--- cashier number assignment ---*/

typedef struct {
    UCHAR  uchMajorClass;       /* Major Class Data Definition */

    UCHAR  uchMinorClass;       /* Minor Class Data Definition */

    UCHAR  usACNumber;      /* A/C Number */

    UCHAR  uchFieldAddr;        /* Field Address */
    CASIF  CashierIf;        /* Cashier I/F Structure */
    WCHAR  aszMnemonics[PARA_LEADTHRU_LEN+1];   /* Mnemonics */
    UCHAR  uchStatus;       /* 0=with data, 1=without data */
    USHORT usPrintControl;      /* Print Control */
}MAINTCASHIERIF;

/*--- Loan/Pick-up, Saratoga ---*/
/*--- slip header/trailer (for loan/pickup) ---*/

typedef struct {
    UCHAR   uchMajorClass;          /* Major Data Class Definition */
    UCHAR   uchMinorClass;          /* Minor Data Class Definition */
    USHORT  usACNumber;             /* Action Code */
    ULONG   ulCashierNo;            /* Number */
    WCHAR   aszMnemonics[PARA_CASHIER_LEN+1]; /* Mnemonics */
    USHORT  usConsCount;           /* consective counter */
    USHORT  usDayRstCount;
    USHORT  usPTDRstCount;
    USHORT  usStoreNumber;
    USHORT  usRegNumber;
    ULONG   ulSuperNumber;         /* Supervisor Number */
    USHORT  usPrintControl;        /* Print Control */
}MAINTSPHEADER;

typedef struct {
        UCHAR       uchMajorClass;         /* Major Class Data Definition */
        UCHAR       uchMinorClass;         /* Minor Class Data Definition */
        ULONG       ulCashierNo;           /* Cashier Number */
        WCHAR       auchCashierName[PARA_CASHIER_LEN+1]; /* Cashier Mnemonic] */
        USHORT      usModifier;            /* Loan/Pickup Modifier Status */
        LONG        lForQty;               /* For, Qty Amount */
        DCURRENCY   lUnitAmount;           /* Qty Unit Amount */
        ULONG       ulFCRate;              /* Foreign Currency Rate */
        DCURRENCY   lNativeAmount;         /* Native Amount for FC */
        UCHAR       uchFCMDC;              /* Foreign Currency Control MDC */
        UCHAR       uchFCMDC2;             /* Foreign Currency Control MDC, V2.2 */
        DCURRENCY   lAmount;               /* Loan/Pickup Amount */
        DCURRENCY   lTotal;                /* Loan/Pickup Total Amount */
        UCHAR       uchLoanPickStatus;     /* 0=with data, 1=without data */
        MAINTSPHEADER HeadTrail;      /* for slip */
}MAINTLOANPICKUP;

/*--- display loan/pick up amount ---*/

typedef struct {
    UCHAR        uchMajorClass;          /* Major Data Class Definition */
    UCHAR        uchMinorClass;          /* Minor Data Class Definition */
    DCURRENCY    lAmount;                /* Amount, For, Qty */
    UCHAR        uchFCMDC;               /* Decimal Point MDC for FC */
    WCHAR        aszMnemonics[PARA_TRANSMNEMO_LEN+1]; /* Mnemonics */
}MAINTDSPLOANPICKUP;

/*--- Display PLU# and Mnemonic at Prog#3,  Saratoga ---*/
typedef struct {
    WCHAR   auchPLUNumber[PLU_MAX_DIGIT+1];     /* PLU Number */
    WCHAR   aszMnemonics[PARA_PLU_LEN+1];       /* Saratoga */
}MAINTFSCPLU;


/*--- work area for maintenance module ---*/

typedef union {
    PARAGUESTNO       GuestNo;
    PARAPLUNOMENU     PLUNoMenu;
    PARAROUNDTBL      RoundTbl;
    PARADISCTBL       DiscTbl;
    PARAPROMOTION     Promotion;
    PARACURRENCYTBL   CurrencyTbl;
    PARAMENUPLUTBL    MenuPLUTbl;
    PARATAXRATETBL    TaxRateTbl;
    PARADEPT          DEPT;
    PARAPLU           PLU;
    PARACPN           CPN;
    PARAMDC           MDC;
    PARAFSC           FSC;
    PARASECURITYNO    SecurityNo;
    PARASUPLEVEL      SupLevel;
    PARAACTCODESEC    ActCodeSec;
    PARATRANSHALO     TransHALO;
    PARATEND          Tend;
    PARAHOURLYTIME    HourlyTime;
    PARASLIPFEEDCTL   SlipFeedCtl;
    PARATRANSMNEMO    TransMnemo;
    PARALEADTHRU      LeadThru;
    PARAREPORTNAME    ReportName;
    PARASPEMNEMO      SpeMnemo;
    PARATAXTBL        TaxTbl;
    PARAADJMNEMO      AdjMnemo;
    PARAPRTMODI       PrtModi;
    PARAMAJORDEPT     MajorDEPT;
    PARACHAR24        Char24;
    PARACTLTBLMENU    CtlTblMenu;
    PARAALTKITCH      AltKitch;
    PARAFLEXMEM       FlexMem;
    PARASTOREGNO      StoRegNo;
    PARATTLKEYTYP     TtlKeyTyp;
    PARATTLKEYCTL     TtlKeyCtl;
/*    PARACASHIERNO     ParaCashierNo; */
    PARAEMPLOYEENO    ParaEmployeeNo;           /* 93-06-22 */
    PARACASHABASSIGN  CashABAssign;
    PARASHAREDPRT     SharedPrt;
    PARAPCIF          PCIF;
    PARATOD           ParaTOD;
    MAINTSETPAGE      MaintSetPage;
    MAINTENT          MaintEnt;
    MAINTTRANS        MaintABORT;
    MAINTTOD          MaintTOD;
    MAINTMODEIN       MaintModeIn;
    MAINTTAXTBL       MaintTaxTbl;
    MAINTDSPCTL       MaintDspCtl;
    MAINTDSPFLINE     MaintDspFLine;
	MAINTCSTR         MaintCstr;
	MAINTCPN          MaintCpn;
	MAINTPLU          MaintPlu;
	MAINTPPI          MaintPpi;
	MAINTDEPT         MaintDept;
    MAINTETKFL        EtkFl;
    MAINTCPM          MaintCpm;
    MAINTCASHIERIF    MaintCashierIf;
    MAINTLOANPICKUP   LoanPickup;   /* Saratoga */
    PARATONECTL       Tone;
    PARATARE          Tare;
    PARAPRESETAMOUNT  PresetAmount;
    PARASOFTCHK       SoftChk;
    PARAHOTELID       ParaHotelId;
    PARAOEPTBL        ParaOepTbl;       /* Order Entry Prompt Table */
    PARACSTR          CSTR;             /* Control String Table */
    PARAFXDRIVE       ParaFxDrive;      /* Flexible Drive Through Parameter */
    PARAPPI           PPI;              /* Promotional Pricing Item */
    PARASERVICETIME   ServTime;
    PARAPIGRULE       PigRuleTbl;
    PARALABORCOST     LaborCost;
    PARADISPPARA      DispPara;
    PARAMLDMNEMO      MldMnemo;
    PARADEPTNOMENU    DeptNoMenu; /* 2172 */
    PARAKDSIP         KdsIp;      /* 2172 */
    PARABOUNDAGE      BoundAge;   /* 2172 */
    PARARESTRICTION   Restriction; /* 2172 */
    PARAMISCPARA      MiscPara;
	PARAUNLOCKNO      UnlockNo;
	PARAAUTOCPN		  AutoCoupon;
}MAINTWORK;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*------------------------------------------------------------------------*\

            P R O T O T Y P E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

SHORT MaintGuestNoWrite( PARAGUESTNO *pdata);           /* MATGCNO.C */
SHORT MaintPLUNoMenuRead( PARAPLUNOMENU *pData );
SHORT MaintPLUNoMenuWrite( PARAPLUNOMENU *pData );
VOID  MaintPLUNoMenuReport( VOID );
VOID  MaintPLUNoMenuReplace( VOID );
SHORT MaintPLUNoMenuEdit( PARAPLUNOMENU *pData ); /* 2172 */
SHORT MaintPLUNoMenuCheck( PARAPLU *pParaPLU ); /* 2172 */
SHORT MaintRoundRead( PARAROUNDTBL *pData );
SHORT MaintRoundWrite( PARAROUNDTBL *pData );
VOID  MaintRoundReport( VOID );
SHORT MaintDiscRead( PARADISCTBL *pData );              /* MATDISC.C */
SHORT MaintDiscWrite( PARADISCTBL *pData );             /* MATDISC.C */
VOID  MaintDiscReport( VOID );                          /* MATDISC.C */
SHORT MaintOepRead( PARAOEPTBL *pData );                /* MATOEP.C */
SHORT MaintOepWrite( PARAOEPTBL *pData );               /* MATOEP.C */
VOID  MaintOepReport( VOID );                           /* MATOEP.C */
SHORT MaintPromotionRead( PARAPROMOTION *pData );
SHORT MaintPromotionWrite( PARAPROMOTION *pData );
VOID  MaintPromotionReport( VOID );
SHORT MaintCurrencyRead( PARACURRENCYTBL *pData );
SHORT MaintCurrencyWrite( PARACURRENCYTBL *pData );
VOID  MaintCurrencyReport( VOID );
UCHAR MaintCurrencyMDCRead( UCHAR uchAddress );
SHORT MaintMenuPLURead( PARAMENUPLUTBL *pData );
SHORT MaintMenuPLUEdit( MAINTMENUPLUTBL *pData );
/* SHORT MaintMenuPLUEdit( PARAMENUPLUTBL *pData ); */
VOID  MaintMenuPLUWrite( VOID );
VOID  MaintMenuPLUReport( VOID );
SHORT MaintMenuPLUDelete( PARAMENUPLUTBL *pData );
SHORT MaintMenuPLUModifier( UCHAR uchMinorClass );
SHORT MaintTaxRateRead( PARATAXRATETBL *pData );
SHORT MaintTaxRateWrite( PARATAXRATETBL *pData );
VOID  MaintTaxRateReport( VOID );
SHORT MaintDEPTRead( PARADEPT *pData );
SHORT MaintDEPTEdit( MAINTDEPT *pData );
SHORT MaintDEPTWrite( VOID );
SHORT MaintDEPTReport( VOID );
SHORT MaintPLURead( PARAPLU *pData );
SHORT MaintPLUEdit( MAINTPLU *pData );
SHORT MaintDEPTDelete( PARADEPT *pData );
SHORT MaintPLUWrite( VOID );
SHORT MaintPLUDelete( VOID );
SHORT MaintPLUReport( VOID );
VOID MaintPLUDisplayFirstPage( VOID );  /* R3.1 */
VOID MaintPLUDisplaySecondPage( VOID ); /* R3.1 */
SHORT MaintPLUModifier( MAINTPLU *pData  ); /* 2172 */
SHORT   MaintPluLabelAnalysis(PARAPLU *ParaPLU, UCHAR uchEVersion); /* 2172 */
/***
SHORT MaintCashierNoRead( PARACASHIERNO *pData );
SHORT MaintCashierNoWrite( PARACASHIERNO *pData );
SHORT MaintCashierNoDelete( PARACASHIERNO *pData );
SHORT MaintCashierNoReport( VOID );
***/
SHORT MaintCashierEdit( MAINTCASHIERIF *pData );        /* matcas.c */
SHORT MaintCashierWrite( VOID );                /* matcas.c */
SHORT MaintCashierDelete( MAINTCASHIERIF *pdata );      /* matcas.c */
SHORT MaintCashierReport( VOID );               /* matcas.c */
SHORT MaintCashierRead( MAINTCASHIERIF *pData );        /* matcas.c */
SHORT MaintMDCRead( PARAMDC *pData );
SHORT MaintMDCWrite( PARAMDC *pData );
VOID  MaintMDCReport( VOID );
SHORT MaintFSCRead( PARAFSC *pData );
SHORT MaintFSCEdit( PARAFSC *pData );
VOID  MaintFSCWrite( VOID );
VOID  MaintFSCReport( VOID );

SHORT MaintSecurityNoRead( PARASECURITYNO *pData );
SHORT MaintSecurityNoWrite( PARASECURITYNO *pData );
VOID  MaintSecurityNoReport( VOID );
SHORT MaintSupLevelRead( PARASUPLEVEL *pData );
SHORT MaintSupLevelWrite( PARASUPLEVEL *pData );
VOID  MaintSupLevelReport( VOID );
SHORT MaintActCodeSecRead( PARAACTCODESEC *pData );
SHORT MaintActCodeSecWrite( PARAACTCODESEC *pData );
VOID  MaintActCodeSecReport( VOID );
SHORT MaintTransHALORead( PARATRANSHALO *pData );
SHORT MaintTransHALOWrite( PARATRANSHALO *pData );
VOID  MaintTransHALOReport( VOID );
SHORT MaintHourlyTimeRead( MAINTHOURLYTIME *pData );
SHORT MaintHourlyTimeWrite( MAINTHOURLYTIME *pData );
VOID  MaintHourlyTimeReport( VOID );
SHORT MaintSlipFeedCtlRead( PARASLIPFEEDCTL *pData );
SHORT MaintSlipFeedCtlWrite( PARASLIPFEEDCTL *pData );
VOID  MaintSlipFeedCtlReport( VOID );
SHORT MaintTransMnemoRead( PARATRANSMNEMO *pData );
SHORT MaintTransMnemoWrite( PARATRANSMNEMO *pData );
VOID  MaintTransMnemoReport( VOID );
SHORT MaintLeadThruRead( PARALEADTHRU *pData );
SHORT MaintLeadThruWrite( PARALEADTHRU *pData );
VOID  MaintLeadThruReport( VOID );
SHORT MaintReportNameRead( PARAREPORTNAME *pData );
SHORT MaintReportNameWrite( PARAREPORTNAME *pData );
VOID  MaintReportNameReport( VOID );
SHORT MaintSpeMnemoRead( PARASPEMNEMO *pData );
SHORT MaintSpeMnemoWrite( PARASPEMNEMO *pData );
VOID  MaintSpeMnemoReport( VOID );
VOID  MaintTaxTblReadNo1( VOID );                       /* MATTAX1.C */
SHORT MaintTaxRateEditNo1(MAINTTAXTBL *pData);          /* MATTAX1.C */
SHORT MaintTaxableAmountEditNo1(MAINTTAXTBL *pData);    /* MATTAX1.C */
SHORT MaintTaxAmountEditNo1(MAINTTAXTBL *pData);        /* MATTAX1.C */
SHORT MaintTaxableAmountErrorNo1(MAINTTAXTBL *pData);   /* MATTAX1.C */
VOID  MaintTaxTblWriteNo1( VOID );                      /* MATTAX1.C */
VOID  MaintTaxTblDeleteNo1( VOID );                     /* MATTAX1.C */
SHORT MaintTaxTblCalNo1( MAINTTAXTBL *pData );          /* MATTAX1.C */
VOID  MaintTaxTblReportNo1( VOID );                     /* MATTAX1.C */
VOID  MaintTaxTblTmpReportNo1( VOID );                  /* MATTAX1.C */
VOID  MaintTaxTblRead( UCHAR uchMajorClass );           /* MATTAX1.C */
SHORT MaintTaxRateEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass); /* MATTAX1.C */
SHORT MaintTaxableAmountEdit(MAINTTAXTBL *pData, UCHAR uchMajorClass); /* MATTAX1.C */
SHORT MaintTaxAmountEdit(MAINTTAXTBL *pData,
        UCHAR uchMajorClass, USHORT usOffset, UCHAR uchOffTblLen); /* MATTAX1.C */
SHORT MaintTaxableAmountError(MAINTTAXTBL *pData,
        UCHAR uchMajorClass, USHORT usOffset);          /* MATTAX1.C */
VOID  MaintTaxTblDelete( UCHAR uchMajorClass );         /* MATTAX1.C */
SHORT MaintTaxTblCal( MAINTTAXTBL *pData,
        UCHAR uchMajorClass, USHORT usType );           /* MATTAX1.C */
VOID  MaintTaxTblReport( UCHAR uchMajorClass );         /* MATTAX1.C */
VOID  MaintTaxTblTmpReport( UCHAR uchMajorClass,
        USHORT usOffset, UCHAR uchOffTblLen );          /* MATTAX1.C */
VOID  MaintTaxTblInit(UCHAR uchMajorClass);             /* MATTAX1.C */
VOID  MaintTaxTblReadNo2( VOID );                       /* MATTAX2.C */
SHORT MaintTaxRateEditNo2(MAINTTAXTBL *pData);          /* MATTAX2.C */
SHORT MaintTaxableAmountEditNo2(MAINTTAXTBL *pData);    /* MATTAX2.C */
SHORT MaintTaxAmountEditNo2(MAINTTAXTBL *pData);        /* MATTAX2.C */
SHORT MaintTaxAmountErrorNo2(MAINTTAXTBL *pData);       /* MATTAX2.C */
SHORT MaintTaxableAmountErrorNo2(MAINTTAXTBL *pData);   /* MATTAX2.C */
VOID  MaintTaxTblWriteNo2( VOID );                      /* MATTAX2.C */
VOID  MaintTaxTblDeleteNo2( VOID );                     /* MATTAX2.C */
SHORT MaintTaxTblCalNo2( MAINTTAXTBL *pData );          /* MATTAX2.C */
VOID  MaintTaxTblTmpReportNo2( VOID );                  /* MATTAX2.C */
VOID  MaintTaxTblReportNo2( VOID );                     /* MATTAX2.C */
VOID  MaintTaxTblReadNo3( VOID );                       /* MATTAX3.C */
SHORT MaintTaxRateEditNo3(MAINTTAXTBL *pData);          /* MATTAX3.C */
SHORT MaintTaxableAmountEditNo3(MAINTTAXTBL *pData);    /* MATTAX3.C */
SHORT MaintTaxAmountEditNo3(MAINTTAXTBL *pData);        /* MATTAX3.C */
SHORT MaintTaxAmountErrorNo3(MAINTTAXTBL *pData);       /* MATTAX3.C */
SHORT MaintTaxableAmountErrorNo3(MAINTTAXTBL *pData);   /* MATTAX3.C */
VOID  MaintTaxTblWriteNo3( VOID );                      /* MATTAX3.C */
VOID  MaintTaxTblDeleteNo3( VOID );                     /* MATTAX3.C */
SHORT MaintTaxTblCalNo3( MAINTTAXTBL *pData );          /* MATTAX3.C */
VOID  MaintTaxTblTmpReportNo3( VOID );                  /* MATTAX3.C */
VOID  MaintTaxTblReportNo3( VOID );                     /* MATTAX3.C */
VOID  MaintTaxTblReadNo4( VOID );                       /* MATTAX4.C */
SHORT MaintTaxRateEditNo4(MAINTTAXTBL *pData);          /* MATTAX4.C */
SHORT MaintTaxableAmountEditNo4(MAINTTAXTBL *pData);    /* MATTAX4.C */
SHORT MaintTaxAmountEditNo4(MAINTTAXTBL *pData);        /* MATTAX4.C */
SHORT MaintTaxAmountErrorNo4(MAINTTAXTBL *pData);       /* MATTAX4.C */
SHORT MaintTaxableAmountErrorNo4(MAINTTAXTBL *pData);   /* MATTAX4.C */
VOID  MaintTaxTblWriteNo4( VOID );                      /* MATTAX4.C */
VOID  MaintTaxTblDeleteNo4( VOID );                     /* MATTAX4.C */
SHORT MaintTaxTblCalNo4( MAINTTAXTBL *pData );          /* MATTAX4.C */
VOID  MaintTaxTblTmpReportNo4( VOID );                  /* MATTAX4.C */
VOID  MaintTaxTblReportNo4( VOID );                     /* MATTAX4.C */
SHORT MaintAdjMnemoRead( PARAADJMNEMO *pData );         /* MATADJ.C  */
SHORT MaintAdjMnemoWrite( PARAADJMNEMO *pData );        /* MATADJ.C  */
VOID  MaintAdjMnemoReport( VOID );                      /* MATADJ.C  */
SHORT MaintPrtModiRead( PARAPRTMODI *pData );
SHORT MaintPrtModiWrite( PARAPRTMODI *pData );
VOID  MaintPrtModiReport( VOID );
SHORT MaintMajorDEPTRead( PARAMAJORDEPT *pData );
SHORT MaintMajorDEPTWrite( PARAMAJORDEPT *pData );
VOID  MaintMajorDEPTReport( VOID );
SHORT MaintChar24Read( PARACHAR24 *pData );             /* MATCH24.C */
SHORT MaintChar24Write( PARACHAR24 *pData );            /* MATCH24.C */
VOID  MaintChar24Report( VOID );                        /* MATCH24.C */
SHORT MaintPCIFRead( PARAPCIF *pData );                 /* MATPCIF.C */
SHORT MaintPCIFWrite( PARAPCIF *pData );                /* MATPCIF.C */
VOID  MaintPCIFReport( VOID );                          /* MATPCIF.C */
SHORT MaintCtlTblMenuRead( PARACTLTBLMENU *pData );
SHORT MaintCtlTblMenuWrite( PARACTLTBLMENU *pData );
VOID  MaintCtlTblMenuReport( VOID );
SHORT MaintAutoAltKitchRead( PARAALTKITCH *pData );
SHORT MaintAutoAltKitchWrite( PARAALTKITCH *pData );
VOID  MaintAutoAltKitchReport( VOID );
SHORT MaintFlexMemRead( PARAFLEXMEM *pData );
SHORT MaintFlexMemEdit( PARAFLEXMEM *pData );
SHORT MaintFlexMemWrite( VOID );
VOID  MaintFlexMemReport( VOID );
SHORT MaintStoRegNoRead( PARASTOREGNO *pData );
SHORT MaintStoRegNoWrite( PARASTOREGNO *pData );
VOID  MaintStoRegNoReport( VOID );
SHORT MaintTtlKeyTypRead( PARATTLKEYTYP *pData );
SHORT MaintTtlKeyTypWrite( PARATTLKEYTYP *pData );
VOID  MaintTtlKeyTypReport( VOID );
SHORT MaintTtlKeyCtlRead( PARATTLKEYCTL *pData );
SHORT MaintTtlKeyCtlWrite( PARATTLKEYCTL *pData );
VOID  MaintTtlKeyCtlReport( VOID );
SHORT MaintManuAltKitchRead( PARAALTKITCH *pData );
SHORT MaintManuAltKitchWrite( PARAALTKITCH *pData );
VOID  MaintManuAltKitchFin( VOID );
VOID  MaintManuAltKitchReport( VOID );
SHORT MaintCashABAssignRead( PARACASHABASSIGN *pData );
SHORT MaintCashABAssignWrite( PARACASHABASSIGN *pData );
VOID  MaintCashABAssignReport( VOID );
VOID  MaintPreModeInPrg( VOID );
VOID  MaintPreModeInSup( VOID );
SHORT MaintModeInPrg( MAINTMODEIN *pData );
SHORT MaintModeInSup( MAINTMODEIN *pData );
VOID  MaintTODInit( VOID );
SHORT MaintTODEdit( MAINTTOD *pData );
VOID  MaintTODWrite( VOID );
SHORT MaintTODCalWeek( USHORT usYear, USHORT usMonth, USHORT usMDay);
SHORT MaintToneRead( PARATONECTL *pData );
SHORT MaintToneWrite( PARATONECTL *pData );
SHORT MaintTareRead( PARATARE *pData );                 /* MATTARE.c */
SHORT MaintTareWrite( PARATARE *pData );                /* MATTARE.c */
VOID  MaintTareReport( VOID );                          /* MATTARE.c */
SHORT MaintPresetAmountRead( PARAPRESETAMOUNT *pData ); /* MATPAMT.c */
SHORT MaintPresetAmountWrite( PARAPRESETAMOUNT *pData );/* MATPAMT.c */
VOID  MaintPresetAmountReport( VOID );                  /* MATPAMT.c */
SHORT MaintSharedPrtRead( PARASHAREDPRT *pData );       /* MATSHR.c */
SHORT MaintSharedPrtWrite( PARASHAREDPRT *pData );      /* MATSHR.c */
VOID  MaintSharedPrtReport( VOID );                     /* MATSHR.c */
SHORT MaintPigRuleRead( PARAPIGRULE *pData );           /* MATPIG.c */
SHORT MaintPigRuleWrite( PARAPIGRULE *pData );          /* MATPIG.c */
VOID  MaintPigRuleReport( VOID );                       /* MATPIG.c */
SHORT MaintAutoCouponRead( PARAAUTOCPN *pData ); /* MATPAMT.c */
SHORT MaintAutoCouponWrite( PARAAUTOCPN *pData );/* MATPAMT.c */
VOID  MaintPresetAmountReport( VOID );                  /* MATPAMT.c */
VOID  MaintABORT(VOID);
SHORT MaintEntSup( MAINTENT *pData );
SHORT MaintEntPrg( MAINTENT *pData );
SHORT MaintSetPage( MAINTSETPAGE *pData );
UCHAR MaintGetPage( VOID );
SHORT MaintSetPageOnly( MAINTSETPAGE *pData );
VOID  MaintReDisp( MAINTREDISP *pData );
VOID  MaintFinExt(UCHAR uchMinorClass, USHORT usPrtControl);
VOID  MaintFin(UCHAR uchMinorClass);
SHORT MaintCopyTtl( VOID );
SHORT MaintPifGetHostAddress(UCHAR *puchLocalAddr, UCHAR *puchHostAddr, TCHAR *wszHostName2);     /* MATCOPY.C */
SHORT MaintJoinCluster(VOID);                                                                     /* MATCOPY.C */
SHORT MaintUnjoinCluster(VOID);                                                                   /* MATCOPY.C */
SHORT MaintSFRemoval(VOID);																		  /* MATCOPY.C */
SHORT CheckUnjoinStatus(VOID);																	  /* MATCOPY.C */
SHORT MaintLoadPLU( VOID );                             /* MATLPLU.C */
SHORT MaintLoadAll( ULONG ulLoadAllFlags );             /* MATLALL.C */
SHORT MaintLoadSup( VOID );                             /* MATLSUP.C */

USHORT  MaintIncreSpcCo(UCHAR uchCoAddress);
USHORT  MaintCurrentSpcCo(UCHAR uchCoAddress);
USHORT  MaintResetSpcCo(UCHAR uchCoAddress);
VOID  MaintMakeAbortKey(VOID);
VOID  MaintHeaderCtl(USHORT uchPrgSupNo, UCHAR uchRptAdr1);
VOID  MaintHeaderCtlExt(USHORT uchPrgSupNo, UCHAR uchRptAdr1, USHORT usPrtControl);
VOID  MaintMakeFeedExt(USHORT usPrtControl);
VOID  MaintFeed(VOID);
VOID  MaintLoanPickFeed(USHORT usPrtControl);    /* Saratoga */
VOID  MaintMakePrintLine (TCHAR  *pLineToPrint, USHORT usPrtControl );
VOID  MaintMakeHeader( UCHAR uchMinorClass,                     /* Print Header */
                       UCHAR uchRptAdr1,
                       UCHAR uchRptAdr2,
                       UCHAR uchSpeAdr,
                       UCHAR uchRptAdr3,
                       UCHAR uchReportType,
                       USHORT usPrgSupNo,
                       UCHAR uchResetType,
                       USHORT usPrtControl);
VOID MaintMakeHeaderFile( VOID *pvFile, UCHAR uchMinorClass,
                      UCHAR uchRptAdr1,
                      UCHAR uchRptAdr2,
                      UCHAR uchSpeAdr,
                      UCHAR uchRptAdr3,
                      UCHAR uchReportType,
                      USHORT uchPrgSupNo,
                      UCHAR uchResetType, 
                      USHORT usPrtControl );    
VOID  MaintMakeTrailer(UCHAR uchMinorClass);                    /* Print Trailer */
VOID  MaintMakeTrailerFile( VOID *pvFile, UCHAR uchMinorClass);
VOID  MaintMakeTrailerExt(UCHAR uchMinorClass, USHORT usPrtControl);   /* Print Trailer */
VOID  MaintFeedRec(LONG lFeedLine);                             /* Receipt Feed */
VOID  MaintFeedJou(VOID);                                       /* Journal Feed */
SHORT MaintPLUCopy(VOID);                                       /* Copy DEPT Status to PLU Status */
VOID  MaintSetDEPTDispData(UCHAR uchFieldAddr);
VOID  MaintSetPLUDispData(UCHAR uchFieldAddr);
VOID  MaintSetPriceDispData(UCHAR uchFieldAddr);
VOID  MaintSetMnemoDispData( VOID );
SHORT MaintClearSetPin(ULONG ulEmployeeId, ULONG ulEmployeePin);     /* clear the PIN of an Operator Record of specified Employee Id. */
/* SHORT MaintConvErrorCode(SHORT sError);                           Return Error Status */
VOID MaintConv3bto4b(ULONG *ulPrice, UCHAR auchPrice[]);             /* Convert 3 Byte Data to ULONG Data */
VOID MaintConv4bto3b(UCHAR auchPrice[], ULONG *ulPrice);             /* Convert ULONG Data to 3 Byte Data */
UCHAR MaintChkPLUType(UCHAR uchPLUType);
SHORT MaintChkBinary(WCHAR auchChkString[], UCHAR uchChkDigit);      /* Check Binary Data */
UCHAR MaintAtoStatus(WCHAR auchString[]);
SHORT MaintPLUDEPTLock(VOID);
VOID  MaintPLUDEPTUnLock(VOID);
VOID  MaintPLUFileUpDate(VOID);
UCHAR MaintChkFcFormat( UCHAR uchMDCAddress );
SHORT MaintGetPLUNo(UCHAR uchMinorFSCData, WCHAR *paszPLUNumber, UCHAR *puchModStat);   /* MATCOM.C */
SHORT MaintGetDEPTNo(UCHAR uchMinorFSCData, USHORT *pusDeptNumber);   /* MATCOM.C */
SHORT MaintTODChkDay(USHORT usYear, USHORT usMonth, USHORT usMDay);
SHORT MaintChkGCNo( ULONG ulGCNumber, USHORT *usRtnGCNumber);       /* MATCOM.C */
SHORT MaintDisconMas(VOID);                                         /* MATDISM.C */
SHORT MaintForceInquiry(VOID);                                         /* MATDISM.C */
VOID  MaintDisp(USHORT usPGACNo,                                    /* MATCOM.C */
                UCHAR  uchDispType,
                UCHAR  uchPageNo,
                UCHAR  uchPTD,
                UCHAR  uchRpt,
                UCHAR  uchReset,
                ULONG  ulAmount,
                UCHAR  uchLeadThruAddr);
VOID  MaintInit( VOID );                                            /* MATCOM.C */
VOID  MaintOpeStatus( VOID );                                       /* MATOPSTS.C */
SHORT MaintShrTermLockSup(USHORT usSupNo );                         /* MATCOM.C */
SHORT MaintShrTermLockProg(USHORT usPrgNo );                        /* MATCOM.C */
VOID  MaintShrTermUnLock( VOID );                                   /* MATCOM.C */
SHORT MaintSupASK(UCHAR uchFSC);                                    /* Saratoga */
SHORT MaintEmployeeNoRead( PARAEMPLOYEENO *pData );
SHORT MaintEmployeeNoEdit( PARAEMPLOYEENO *pData  );
SHORT MaintEmployeeNoWrite( VOID );
SHORT MaintEmployeeNoDelete( PARAEMPLOYEENO *pData );
SHORT MaintEmployeeNoReport( UCHAR uchMinorClass );
VOID MaintEmployeeNoDisp( PARAEMPLOYEENO *pData );
VOID MaintEmployeeNoDispJob( PARAEMPLOYEENO *pData );
SHORT MaintEmployeeNoIncJob( PARAEMPLOYEENO *pData );
VOID MaintEmployeeMnemo( UCHAR uchAddress );
SHORT MaintSoftChkMsgRead( PARASOFTCHK *pData );
SHORT MaintSoftChkMsgWrite( PARASOFTCHK *pData );
VOID  MaintSoftChkMsgReport( VOID );
SHORT MaintETKFileRead( PARAETKFL *pData );      /* MATETKFL.C */
SHORT MaintETKFileWrite( PARAETKFL *pData );     /* MATETKFL.C */
VOID  MaintETKFilePrint( VOID );                 /* MATETKFL.C */
VOID  MaintEmpNoPrint( VOID );                   /* MATETKFL.C */
SHORT MaintETKFileDisp( VOID );                  /* MATETKFL.C */
SHORT MaintETKFileMldAllDisp(USHORT usIOStatus); /* MATETKFL.C */
SHORT MaintEtkFileDateTime(PARAETKFL *pData );   /* MATETKFL.C */
/* SHORT MaintPromoSlipRead( PARACHAR44 *pData );   */
/* SHORT MaintPromoSlipWrite( PARACHAR44 *pData );  */
/* VOID  MaintPromoSlipReport( VOID );              */
VOID MaintCalcIndTtl( UCHAR uchType ,USHORT usTtlTime, USHORT usTtlMinute,
                      USHORT *pusCalcTime, USHORT *pusCalcMinute);
SHORT MaintStartStopCpm( UCHAR uchStatus );      /* A/C 78 */
VOID  MaintEptInit(VOID);
SHORT MaintHotelIdRead( PARAHOTELID *pData );
SHORT MaintHotelIdWrite( PARAHOTELID *pData );
VOID  MaintHotelIdReport( VOID );
SHORT MaintCPNRead( PARACPN *pData );                /* A/C 161 */
SHORT MaintCPNEdit( MAINTCPN *pData );
SHORT MaintCPNWrite( VOID );
VOID  MaintSetCPNDispData(UCHAR uchFieldAddr);
SHORT MaintCPNLock( VOID );
VOID  MaintCPNUnLock( VOID );
VOID  MaintCPNReport( VOID );
SHORT MaintCPNPLUModifier( UCHAR uchMinorClass );
VOID MaintCouponDisplayFirstPage( VOID );
VOID MaintCouponDisplaySecondPage( VOID );
SHORT MaintFlexDriveRead( PARAFXDRIVE *pData );         /* MATFXDRV.C */
SHORT MaintFlexDriveWrite( PARAFXDRIVE *pData );        /* MATFXDRV.C */
VOID  MaintFlexDriveReport( VOID );                     /* MATEXDRV.C */
SHORT MaintCSTRRead( MAINTCSTR *pData );                /* A/C 33 */
SHORT MaintCSTREdit( MAINTCSTR *pData );
SHORT MaintCSTRDelete( MAINTCSTR *pData );
SHORT MaintCSTRWrite( VOID );
SHORT MaintCSTRReport( VOID );
SHORT MaintCstrLock( VOID );
VOID  MaintCstrUnLock( VOID );
SHORT MaintPPIRead( MAINTPPI *pData, USHORT pPage );                /* A/C 71 */
SHORT MaintPPIEdit( MAINTPPI *pData );

SHORT MaintPPIEditAddtlSettings( MAINTPPI *pData ); //CSMALL
SHORT MaintPPICheckLength( VOID ); //CSMALL

SHORT MaintPPIDelete( MAINTPPI *pData );
SHORT MaintPPIWrite( VOID );

SHORT MaintPPIWriteAddtlSettings( VOID ); //CSMALL

SHORT MaintPPIReport( VOID );
SHORT MaintPPILock( VOID );
VOID  MaintPPIUnLock( VOID );
SHORT MaintServiceTimeRead( PARASERVICETIME *pData );          /* MATSERV.C */
SHORT MaintServiceTimeWrite( PARASERVICETIME *pData );         /* MATSERV.C */
VOID  MaintServiceTimeReport( VOID );                          /* MATSERV.C */
SHORT MaintLaborCostRead( PARALABORCOST *pData );              /* MATCOST.C */
SHORT MaintLaborCostWrite( PARALABORCOST *pData );             /* MATCOST.C */
VOID  MaintLaborCostReport( VOID );                            /* MATCOST.C */
SHORT MaintDispParaRead( PARADISPPARA *pData );
SHORT MaintDispParaEdit( PARADISPPARA *pData );
VOID  MaintDispParaWrite( VOID );
VOID  MaintDispParaReport( VOID );
SHORT MaintMldMnemoRead( PARAMLDMNEMO *pData );
SHORT MaintMldMnemoWrite( PARAMLDMNEMO *pData );
VOID  MaintMldMnemoReport( VOID );
SHORT MaintTendRead( PARATEND *pData );                       /* V3.3 */
SHORT MaintTendWrite( PARATEND *pData );                      /* V3.3 */
VOID  MaintTendReport( VOID );                                /* V3.3 */
UCHAR   MaintResetLog(UCHAR uchAction);                     /* V3.3 */
VOID    MaintDelayedBalance(UCHAR uchStatus);				/* Delayed Balance for EOD JHHJ */
SHORT MaintDeptNoMenuRead( PARADEPTNOMENU *pData ); /* 2172 */
SHORT MaintDeptNoMenuWrite( PARADEPTNOMENU *pData ); /* 2172 */
VOID MaintDeptNoMenuReport( VOID ); /* 2172 */
VOID MaintDeptNoMenuReplace( VOID ); /* 2172 */
SHORT MaintKdsIpRead( PARAKDSIP *pData ); /* 2172 */
SHORT MaintKdsIpWrite( PARAKDSIP *pData ); /* 2172 */
VOID MaintKdsIpReport( VOID ); /* 2172 */
SHORT MaintBoundAgeRead( PARABOUNDAGE *pData ); /* 2172 */
SHORT MaintBoundAgeWrite( PARABOUNDAGE *pData ); /* 2172 */
VOID MaintBoundAgeReport( VOID ); /* 2172 */
SHORT MaintRestRead( PARARESTRICTION *pData ); /* 2172 */
SHORT MaintRestEdit( PARARESTRICTION *pData ); /* 2172 */
SHORT MaintRestWrite( VOID ); /* 2172 */
VOID MaintRestReport( VOID ); /* 2172 */
VOID MaintRestDispData(PARARESTRICTION *pData); /* 2172 */
SHORT MaintRestChkStatus( VOID ); /* 2172 */
USHORT  MaintGetACNumber(VOID);                 /* Saratoga */
VOID MaintSetPrintMode( UCHAR uchMode ); //ESMITH PRTFILE

/*----- matlpcom.c, Saratoga -----*/
SHORT MaintLoanPickupConnectionEngine (MAINTLOANPICKUP  *pLoanPickupData);
SHORT MaintLoanPickupCasSignIn( MAINTLOANPICKUP *pData,USHORT usDrawer );   /* R2.0 */
SHORT MaintLoanPickupCasSignOut( MAINTLOANPICKUP *pData );
SHORT MaintLoanPickupAbort( VOID );
SHORT MaintLoanPickupErrorCorrect( MAINTLOANPICKUP *pData );
VOID  MaintLoanPickupDisp( MAINTLOANPICKUP *pData );
VOID  MaintLoanPickupHeaderCtl( MAINTLOANPICKUP *pData );
VOID MaintSPHeadTrailSet(MAINTSPHEADER *pData, ULONG ulCashierNo, WCHAR *pszMnemo);
VOID MaintSPHeaderCtl(MAINTSPHEADER *pData);
BOOL MaintSPTrailCtl( MAINTSPHEADER *pData);                        /* Saratoga */
VOID MaintTrnOpen(UCHAR uchMinorClass, ULONG ulCashierNo, DCURRENCY lAmount);     /* Saratoga */
SHORT   MaintLoanPickupFinalize(MAINTLOANPICKUP *pData);			//PDINU

/*----- matloan.c, Saratoga -----*/
SHORT MaintLoanAmountSet( MAINTLOANPICKUP *pData );

/*----- matpick.c, Saratoga -----*/
SHORT MaintPickupAmountSet( MAINTLOANPICKUP *pData );
VOID MaintAllPickupAmountRead(ULONG ulCashierNumber );
VOID MaintAllPickupAmountSet1(MAINTLOANPICKUP *pData );
VOID MaintAllPickupAmountSet2(MAINTLOANPICKUP *pData );
SHORT MaintPickupFCAmountSet(MAINTLOANPICKUP *pData );
VOID    MaintGetFSC(UCHAR *puchMajorFSC, UCHAR *puchMinorFSC);  /* Saratoga */
SHORT   MaintPowerDownLog(UCHAR uchLog);                        /* Saratoga */

/*----- matmisc.c -----*/
SHORT MaintMiscRead(PARAMISCPARA *pData);
SHORT MaintMiscWrite(PARAMISCPARA *pData);
VOID  MaintMiscReport(VOID);

SHORT MaintHostsIpRead( PARAKDSIP *pData ); /* 2172 */
SHORT MaintHostsIpWrite( PARAKDSIP *pData ); /* 2172 */
VOID MaintHostsIpReport( VOID ); /* 2172 */
VOID MaintHostsIpReboot( VOID ); /* 2172 */

/*GSU SR 15 Additions for software security NHPOS Rel 1.4
  By: cwunn, RJC
  These functions allow reading and writing of registry key values
     The report function prints the unlock number.
	 maintUnlockNORegReadCopy is a parsing function to remove high-byte nulls from character
	   arrays type strings. These nulls exist because registry queries return wide characters
	   to a system using standard characters
*/
SHORT MaintUnlockNoRead( PARAUNLOCKNO *pData ); /* 2172 */
SHORT MaintUnlockNoWrite( PARAUNLOCKNO *pData ); /* 2172 */
VOID MaintUnlockNoReport( VOID ); /* 2172 */
SHORT maintUnlockNoRegWrite (PARAUNLOCKNO *securityData);
SHORT maintUnlockNoRegReadCopy (PARAUNLOCKNO *securityData, UCHAR *registryData, int dataLen );
SHORT maintUnlockNoRegRead (PARAUNLOCKNO *securityData);
SHORT maintCheckDataLength(PARAUNLOCKNO *UnlockInfo);

SHORT   MaintOptimizePLUDB(VOID);

SHORT   ItemMiscLoanPickup (MAINTLOANPICKUP  *pLoanPickupData);

/* --- End of Header --- */