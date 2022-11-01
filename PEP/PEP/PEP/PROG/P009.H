/*
* ---------------------------------------------------------------------------
* Title         :   Action Code Security Header File (Prog. 9)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P009.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-08-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Mar-14-95 : 03.00.00 : H.Ishida   : Add A/C 30,33,40,160,161,162
* Jan-24-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-06-98 : 03.03.00 : A.Mitsui   : V3.3
* Nov-11-99 :          : K.Yanagida : Add A/C 10,11,115,170,218
* Jul-12-00 : V1.0.0.3 : M.Teraki   : Removed AC66, AC115
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
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID definition
* ===========================================================================
*/
#define IDD_P09LISTBOX      20901   /* listbox                    */
#define IDD_P09GROUP        20902   /* edit group tytle           */
#define IDD_P09L00          20903   /* level 0 allowed            */
#define IDD_P09L01          20904   /* level 0 prohibited         */
#define IDD_P09L10          20905   /* level 1 allowed            */
#define IDD_P09L11          20906   /* level 1 prohibited         */
#define IDD_P09L20          20907   /* level 2 allowed            */
#define IDD_P09L21          20908   /* level 2 prohibited         */

//static text
#define IDD_P09_LEVEL	20910
#define IDD_P09_AC		20911
#define IDD_P09_DESC	20912
#define IDD_P09_ALLOW	20913
#define IDD_P09_SUP		20914
#define IDD_P09_LEVEL0	20915
#define IDD_P09_LEVEL1	20916
#define IDD_P09_LEVEL2	20917
#define IDD_P09_LEVDESC	20918
/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/
#define IDS_P09_DSC_001  (IDS_P09 +  1)     /* description resource ID */
#define IDS_P09_DSC_002  (IDS_P09 +  2)     /* description resource ID */
#define IDS_P09_DSC_003  (IDS_P09 +  3)     /* description resource ID */
#define IDS_P09_DSC_004  (IDS_P09 +  4)     /* description resource ID */
#define IDS_P09_DSC_005  (IDS_P09 +  5)     /* description resource ID */
#define IDS_P09_DSC_006  (IDS_P09 +  6)     /* description resource ID */
#define IDS_P09_DSC_007  (IDS_P09 +  7)     /* description resource ID */
#define IDS_P09_DSC_008  (IDS_P09 +  8)     /* description resource ID */
#define IDS_P09_DSC_009  (IDS_P09 +  9)     /* description resource ID */
#define IDS_P09_DSC_010  (IDS_P09 + 10)     /* description resource ID */
#define IDS_P09_DSC_011  (IDS_P09 + 11)     /* description resource ID */
#define IDS_P09_DSC_012  (IDS_P09 + 12)     /* description resource ID */
#define IDS_P09_DSC_013  (IDS_P09 + 13)     /* description resource ID */
#define IDS_P09_DSC_014  (IDS_P09 + 14)     /* description resource ID */
#define IDS_P09_DSC_015  (IDS_P09 + 15)     /* description resource ID */
#define IDS_P09_DSC_016  (IDS_P09 + 16)     /* description resource ID */
#define IDS_P09_DSC_017  (IDS_P09 + 17)     /* description resource ID */
#define IDS_P09_DSC_018  (IDS_P09 + 18)     /* description resource ID */
#define IDS_P09_DSC_019  (IDS_P09 + 19)     /* description resource ID */
#define IDS_P09_DSC_020  (IDS_P09 + 20)     /* description resource ID */
#define IDS_P09_DSC_021  (IDS_P09 + 21)     /* description resource ID */
#define IDS_P09_DSC_022  (IDS_P09 + 22)     /* description resource ID */
#define IDS_P09_DSC_023  (IDS_P09 + 23)     /* description resource ID */
#define IDS_P09_DSC_024  (IDS_P09 + 24)     /* description resource ID */
#define IDS_P09_DSC_025  (IDS_P09 + 25)     /* description resource ID */
#define IDS_P09_DSC_026  (IDS_P09 + 26)     /* description resource ID */
#define IDS_P09_DSC_027  (IDS_P09 + 27)     /* description resource ID */
#define IDS_P09_DSC_028  (IDS_P09 + 28)     /* description resource ID */
#define IDS_P09_DSC_029  (IDS_P09 + 29)     /* description resource ID */
#define IDS_P09_DSC_030  (IDS_P09 + 30)     /* description resource ID */
#define IDS_P09_DSC_031  (IDS_P09 + 31)     /* description resource ID */
#define IDS_P09_DSC_032  (IDS_P09 + 32)     /* description resource ID */
#define IDS_P09_DSC_033  (IDS_P09 + 33)     /* description resource ID */
#define IDS_P09_DSC_034  (IDS_P09 + 34)     /* description resource ID */
#define IDS_P09_DSC_035  (IDS_P09 + 35)     /* description resource ID */
#define IDS_P09_DSC_036  (IDS_P09 + 36)     /* description resource ID */
#define IDS_P09_DSC_037  (IDS_P09 + 37)     /* description resource ID */
#define IDS_P09_DSC_038  (IDS_P09 + 38)     /* description resource ID */
#define IDS_P09_DSC_039  (IDS_P09 + 39)     /* description resource ID */
#define IDS_P09_DSC_040  (IDS_P09 + 40)     /* description resource ID */
#define IDS_P09_DSC_041  (IDS_P09 + 41)     /* description resource ID */
#define IDS_P09_DSC_042  (IDS_P09 + 42)     /* description resource ID */
#define IDS_P09_DSC_043  (IDS_P09 + 43)     /* description resource ID */
#define IDS_P09_DSC_044  (IDS_P09 + 44)     /* description resource ID */
#define IDS_P09_DSC_045  (IDS_P09 + 45)     /* description resource ID */
#define IDS_P09_DSC_046  (IDS_P09 + 46)     /* description resource ID */
#define IDS_P09_DSC_047  (IDS_P09 + 47)     /* description resource ID */
#define IDS_P09_DSC_048  (IDS_P09 + 48)     /* description resource ID */
#define IDS_P09_DSC_049  (IDS_P09 + 49)     /* description resource ID */
#define IDS_P09_DSC_050  (IDS_P09 + 50)     /* description resource ID */
#define IDS_P09_DSC_051  (IDS_P09 + 51)     /* description resource ID */
#define IDS_P09_DSC_052  (IDS_P09 + 52)     /* description resource ID */
#define IDS_P09_DSC_053  (IDS_P09 + 53)     /* description resource ID */
#define IDS_P09_DSC_054  (IDS_P09 + 54)     /* description resource ID */
#define IDS_P09_DSC_055  (IDS_P09 + 55)     /* description resource ID */
#define IDS_P09_DSC_056  (IDS_P09 + 56)     /* description resource ID */
#define IDS_P09_DSC_057  (IDS_P09 + 57)     /* description resource ID */
#define IDS_P09_DSC_058  (IDS_P09 + 58)     /* description resource ID */
#define IDS_P09_DSC_059  (IDS_P09 + 59)     /* description resource ID */
#define IDS_P09_DSC_060  (IDS_P09 + 60)     /* description resource ID */
#define IDS_P09_DSC_061  (IDS_P09 + 61)     /* description resource ID */
#define IDS_P09_DSC_062  (IDS_P09 + 62)     /* description resource ID */
#define IDS_P09_DSC_063  (IDS_P09 + 63)     /* description resource ID */
#define IDS_P09_DSC_064  (IDS_P09 + 64)     /* description resource ID */
#define IDS_P09_DSC_065  (IDS_P09 + 65)     /* description resource ID */
#define IDS_P09_DSC_066  (IDS_P09 + 66)     /* description resource ID */
#define IDS_P09_DSC_067  (IDS_P09 + 67)     /* description resource ID */
#define IDS_P09_DSC_068  (IDS_P09 + 68)     /* description resource ID */
#define IDS_P09_DSC_069  (IDS_P09 + 69)     /* description resource ID */
#define IDS_P09_DSC_070  (IDS_P09 + 70)     /* description resource ID */
#define IDS_P09_DSC_071  (IDS_P09 + 71)     /* AC169, AC_TONE_CTL */
#define IDS_P09_DSC_072  (IDS_P09 + 72)     /* AC170, AC_PLU_SLREST */
#define IDS_P09_DSC_073  (IDS_P09 + 73)     /* AC175, AC_SUP_DOWN */
#define IDS_P09_DSC_074  (IDS_P09 + 74)     /* AC176, AC_CSOPN_RPT */
#define IDS_P09_DSC_075  (IDS_P09 + 75)     /* AC208, AC_BOUNDARY_AGE */
#define IDS_P09_DSC_076  (IDS_P09 + 76)     /* AC223, AC_IND_READ_RPT */
#define IDS_P09_DSC_077  (IDS_P09 + 77)     /* AC233, AC_IND_RESET_RPT */
#define IDS_P09_DSC_078  (IDS_P09 + 78)     /* AC275, AC_SET_DELAYED_BAL */
#define IDS_P09_DSC_079  (IDS_P09 + 79)     /* AC102, AC_STORE_FORWARD */
#define IDS_P09_DSC_080  (IDS_P09 + 80)     /* AC444, AC_JOIN_CLUSTER */


#define IDS_P09_MARK0    (IDS_P09 + 90)         /* mark resource ID    */
#define IDS_P09_MARK1    (IDS_P09_MARK0 + 1)    /* mark resource ID    */
#define IDS_P09_MARK2    (IDS_P09_MARK0 + 2)    /* mark resource ID    */
#define IDS_P09_MARK3    (IDS_P09_MARK0 + 3)    /* mark resource ID    */
#define IDS_P09_MARK4    (IDS_P09_MARK0 + 4)    /* mark resource ID    */
#define IDS_P09_MARK5    (IDS_P09_MARK0 + 5)    /* mark resource ID    */
#define IDS_P09_MARK6    (IDS_P09_MARK0 + 6)    /* mark resource ID    */
#define IDS_P09_MARK7    (IDS_P09_MARK0 + 7)    /* mark resource ID    */

#define IDS_P09_ADDR     (IDS_P09 + 100)     /* address resource ID     */

/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/
/*** NCR2172
#define P09_TOTALACTION     (IDS_P09_DSC_073 - IDS_P09)
***/
/* #define P09_TOTALACTION     (IDS_P09_DSC_078 - IDS_P09)  */
#define P09_TOTALACTION     (IDS_P09_DSC_080 - IDS_P09)  //Increased to IDS_P09_DSC_084 ***PDINU
                                            /* total Action Code No.        */
#define P09_BITCOMB         8               /* combination of 3 bits        */
#define P09_DSCRB_NO        P09_TOTALACTION
                                            /* number of description        */
#define P09_DSCRB_LEN       64              /* length of description        */
#define P09_MARK_LEN        8               /* length of mark               */
#define P09_LIST_LEN        (P09_DSCRB_LEN + P09_MARK_LEN + 8)
                                            /* length of listbox string     */
#define P09_ADR_LEN         16              /* address length               */

#define P09_CHK_ODD         2               /* check even/odd number        */
#define P09_BITCOUNT0       0               /* for bit count                */
#define P09_BITCOUNT1       1               /* for bit count                */
#define P09_BITCOUNT2       2               /* for bit count                */
#define P09_BITCOUNT4       4               /* for bit count                */
#define P09_BITCOUNT7       7               /* for bit count                */

#define P09_CHKBIT0         0x01            /* 0 bit on                     */
#define P09_CHKBIT1         0x02            /* 1 bit on                     */
#define P09_CHKBIT2         0x04            /* 2 bit on                     */
#define P09_CHKBIT4         0x10            /* 4 bit on                     */
#define P09_CHKBIT5         0x20            /* 5 bit on                     */
#define P09_CHKBIT6         0x40            /* 6 bit on                     */

/*
* ===========================================================================
*       Macro definition
* ===========================================================================
*/
#define GETBIT40(x)     (BYTE)((BYTE)x & 0x11)    /* 0 & 4 bit on         */
#define GETBIT51(x)     (BYTE)((BYTE)x & 0x22)    /* 1 & 5 bit on         */
#define GETBIT62(x)     (BYTE)((BYTE)x & 0x44)    /* 2 & 6 bit on         */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _P09STR {
    WCHAR    aszDscrb[P09_DSCRB_NO][P09_DSCRB_LEN];  /* description */
    WCHAR    aszMark[P09_BITCOMB][P09_MARK_LEN];     /* $ mark table */
} P09STR, *LPP09STR;

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
WORD    P09InitDlg(HWND, LPBYTE, LPHGLOBAL);
VOID    P09SetListData(HWND, LPBYTE);
WORD    P09SetMaintData(HWND, LPBYTE);
VOID    P09GetMaintData(HWND, LPBYTE, WPARAM);
VOID    P09FinDlg(HWND, LPBYTE, HGLOBAL, WPARAM);

/* ===== End of P009.H ===== */
