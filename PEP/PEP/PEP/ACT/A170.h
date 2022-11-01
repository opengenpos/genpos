/*
* ---------------------------------------------------------------------------
* Title         :   Set Sales Restriction Table of PLU ( AC 170)
* Category      :   Maintenance, NCR 2172 PEP for Windows (Hotel US Model)
* Program Name  :   P051.H
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
* Nov-15-93 : 00.00.01 :            : Initial (NCR2172)
*
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

#define _A170_PROT_

/*
* ===========================================================================
*       Dialog Controls Definitions
* ===========================================================================
*/
#define A170_SPIN_OFFSET    40

/*Test Edit*/
#define IDD_A170_DATE1      17001   /* #1 Date Address         */
#define IDD_A170_DATE2      17002   /* #2 Date Address         */
#define IDD_A170_DATE3      17003   /* #3 Date Address         */
#define IDD_A170_DATE4      17004   /* #4 Date Address         */
#define IDD_A170_DATE5      17005   /* #5 Date Address         */
#define IDD_A170_DATE6      17006   /* #6 Date Address         */
#define IDD_A170_WEEK1      17007   /* #1 Week Address         */
#define IDD_A170_WEEK2      17008   /* #2 Week Address         */
#define IDD_A170_WEEK3      17009   /* #3 Week Address         */
#define IDD_A170_WEEK4      17010   /* #4 Week Address         */
#define IDD_A170_WEEK5      17011   /* #5 Week Address         */
#define IDD_A170_WEEK6      17012   /* #6 Week Address         */
#define IDD_A170_SHOUR1     17013   /* #1 Start Hour Address   */
#define IDD_A170_SHOUR2     17014   /* #2 Start Hour Address   */
#define IDD_A170_SHOUR3     17015   /* #3 Start Hour Address   */
#define IDD_A170_SHOUR4     17016   /* #4 Start Hour Address   */
#define IDD_A170_SHOUR5     17017   /* #5 Start Hour Address   */
#define IDD_A170_SHOUR6     17018   /* #6 Start Hour Address   */
#define IDD_A170_SMINUTE1   17019   /* #1 Start Minute Address */
#define IDD_A170_SMINUTE2   17020   /* #2 Start Minute Address */
#define IDD_A170_SMINUTE3   17021   /* #3 Start Minute Address */
#define IDD_A170_SMINUTE4   17022   /* #4 Start Minute Address */
#define IDD_A170_SMINUTE5   17023   /* #5 Start Minute Address */
#define IDD_A170_SMINUTE6   17024   /* #6 Start Minute Address */
#define IDD_A170_EHOUR1     17025   /* #1 End Hour Address     */
#define IDD_A170_EHOUR2     17026   /* #2 End Hour Address     */
#define IDD_A170_EHOUR3     17027   /* #3 End Hour Address     */
#define IDD_A170_EHOUR4     17028   /* #4 End Hour Address     */
#define IDD_A170_EHOUR5     17029   /* #5 End Hour Address     */
#define IDD_A170_EHOUR6     17030   /* #6 End Hour Address     */
#define IDD_A170_EMINUTE1   17031   /* #1 End Minute Address   */
#define IDD_A170_EMINUTE2   17032   /* #2 End Minute Address   */
#define IDD_A170_EMINUTE3   17033   /* #3 End Minute Address   */
#define IDD_A170_EMINUTE4   17034   /* #4 End Minute Address   */
#define IDD_A170_EMINUTE5   17035   /* #5 End Minute Address   */
#define IDD_A170_EMINUTE6   17036   /* #6 End Minute Address   */

/* Spin Control*/
#define IDD_A170_DATESPIN1    (IDD_A170_DATE1 + A170_SPIN_OFFSET)
#define IDD_A170_DATESPIN2    (IDD_A170_DATE2 + A170_SPIN_OFFSET)
#define IDD_A170_DATESPIN3    (IDD_A170_DATE3 + A170_SPIN_OFFSET)
#define IDD_A170_DATESPIN4    (IDD_A170_DATE4 + A170_SPIN_OFFSET)
#define IDD_A170_DATESPIN5    (IDD_A170_DATE5 + A170_SPIN_OFFSET)
#define IDD_A170_DATESPIN6    (IDD_A170_DATE6 + A170_SPIN_OFFSET)
#define IDD_A170_WEEKSPIN1    (IDD_A170_WEEK1 + A170_SPIN_OFFSET)
#define IDD_A170_WEEKSPIN2    (IDD_A170_WEEK2 + A170_SPIN_OFFSET)
#define IDD_A170_WEEKSPIN3    (IDD_A170_WEEK3 + A170_SPIN_OFFSET)
#define IDD_A170_WEEKSPIN4    (IDD_A170_WEEK4 + A170_SPIN_OFFSET)
#define IDD_A170_WEEKSPIN5    (IDD_A170_WEEK5 + A170_SPIN_OFFSET)
#define IDD_A170_WEEKSPIN6    (IDD_A170_WEEK6 + A170_SPIN_OFFSET)
#define IDD_A170_SHOURSPIN1   (IDD_A170_SHOUR1 + A170_SPIN_OFFSET)
#define IDD_A170_SHOURSPIN2   (IDD_A170_SHOUR2 + A170_SPIN_OFFSET)
#define IDD_A170_SHOURSPIN3   (IDD_A170_SHOUR3 + A170_SPIN_OFFSET)
#define IDD_A170_SHOURSPIN4   (IDD_A170_SHOUR4 + A170_SPIN_OFFSET)
#define IDD_A170_SHOURSPIN5   (IDD_A170_SHOUR5 + A170_SPIN_OFFSET)
#define IDD_A170_SHOURSPIN6   (IDD_A170_SHOUR6 + A170_SPIN_OFFSET)

#define IDD_A170_SMINUTESPIN1 (IDD_A170_SMINUTE1 + A170_SPIN_OFFSET)
#define IDD_A170_SMINUTESPIN2 (IDD_A170_SMINUTE2 + A170_SPIN_OFFSET)
#define IDD_A170_SMINUTESPIN3 (IDD_A170_SMINUTE3 + A170_SPIN_OFFSET)
#define IDD_A170_SMINUTESPIN4 (IDD_A170_SMINUTE4 + A170_SPIN_OFFSET)
#define IDD_A170_SMINUTESPIN5 (IDD_A170_SMINUTE5 + A170_SPIN_OFFSET)
#define IDD_A170_SMINUTESPIN6 (IDD_A170_SMINUTE6 + A170_SPIN_OFFSET)
                              
#define IDD_A170_EHOURSPIN1   (IDD_A170_EHOUR1 + A170_SPIN_OFFSET)
#define IDD_A170_EHOURSPIN2   (IDD_A170_EHOUR2 + A170_SPIN_OFFSET)
#define IDD_A170_EHOURSPIN3   (IDD_A170_EHOUR3 + A170_SPIN_OFFSET)
#define IDD_A170_EHOURSPIN4   (IDD_A170_EHOUR4 + A170_SPIN_OFFSET)
#define IDD_A170_EHOURSPIN5   (IDD_A170_EHOUR5 + A170_SPIN_OFFSET)
#define IDD_A170_EHOURSPIN6   (IDD_A170_EHOUR6 + A170_SPIN_OFFSET)
                              
#define IDD_A170_EMINUTESPIN1 (IDD_A170_EMINUTE1 + A170_SPIN_OFFSET)
#define IDD_A170_EMINUTESPIN2 (IDD_A170_EMINUTE2 + A170_SPIN_OFFSET)
#define IDD_A170_EMINUTESPIN3 (IDD_A170_EMINUTE3 + A170_SPIN_OFFSET)
#define IDD_A170_EMINUTESPIN4 (IDD_A170_EMINUTE4 + A170_SPIN_OFFSET)
#define IDD_A170_EMINUTESPIN5 (IDD_A170_EMINUTE5 + A170_SPIN_OFFSET)
#define IDD_A170_EMINUTESPIN6 (IDD_A170_EMINUTE6 + A170_SPIN_OFFSET)


#define IDD_A170_STRNO       17077
#define IDD_A170_STR1        (IDD_A170_STRNO + 1)
#define IDD_A170_STR2        (IDD_A170_STRNO + 2)
#define IDD_A170_STR3        (IDD_A170_STRNO + 3)
#define IDD_A170_STR4        (IDD_A170_STRNO + 4)
#define IDD_A170_STR5        (IDD_A170_STRNO + 5)
#define IDD_A170_STR6        (IDD_A170_STRNO + 6)
#define IDD_A170_STRDELILEN  (IDD_A170_STRNO + 7)
#define IDD_A170_STRPOSLEN   (IDD_A170_STRNO + 8)

#define IDD_A170_CAPTION1	17086
#define IDD_A170_CAPTION2	(IDD_A170_CAPTION1+1)
#define IDD_A170_CAPTION3	(IDD_A170_CAPTION1+2)
#define IDD_A170_CAPTION4	(IDD_A170_CAPTION1+3)
#define IDD_A170_CAPTION5	(IDD_A170_CAPTION1+4)
#define IDD_A170_CAPTION6	(IDD_A170_CAPTION1+5)
#define IDD_A170_CAPTION7	(IDD_A170_CAPTION1+6)
#define IDD_A170_CAPTION8	(IDD_A170_CAPTION1+7)
#define IDD_A170_CAPTION9	(IDD_A170_CAPTION1+8)
#define IDD_A170_CAPTION10	(IDD_A170_CAPTION1+9)
#define IDD_A170_DAYS_1	    (IDD_A170_CAPTION1+10)
#define IDD_A170_DAYS_2	    (IDD_A170_DAYS_1+1)
#define IDD_A170_DAYS_3	    (IDD_A170_DAYS_1+2)
#define IDD_A170_DAYS_4	    (IDD_A170_DAYS_1+3)
#define IDD_A170_DAYS_5	    (IDD_A170_DAYS_1+4)
#define IDD_A170_DAYS_6	    (IDD_A170_DAYS_1+5)
#define IDD_A170_MDC_PLU5_ADR	29347    // indicates current state of MDC MDC_PLU5_ADR Bit B
#define IDD_A170_DAYS       	29348    // indicates current state of MDC MDC_PLU5_ADR Bit B

/*
* ===========================================================================
*       String ID Definitions
* ===========================================================================
*/

#define IDS_A170_CAPTION     (IDS_A170)
#define IDS_A170_DESC1       (IDS_A170 +  1)
#define IDS_A170_DESC2       (IDS_A170 +  2)
#define IDS_A170_DESC3       (IDS_A170 +  3)
#define IDS_A170_DESC4       (IDS_A170 +  4)
#define IDS_A170_DESC5       (IDS_A170 +  5)
#define IDS_A170_DESC6       (IDS_A170 +  6)
#define IDS_A170_DELILEN     (IDS_A170 +  7)
#define IDS_A170_POSLEN      (IDS_A170 +  8)
#define IDS_PEP_REMINDER_A170 (IDS_A170 +  9)

/*
* ===========================================================================
*       Value Definitions
* ===========================================================================
*/

#define A170_DATE_MIN     0     /* Min. Value of Date             */
#define A170_DATE_MAX     31    /* Max. Value of Date             */
#define A170_WEEK_MIN     0     /* Min. Value of Week             */
#define A170_WEEK_MAX     7     /* Max. Value of Week             */
#define A170_HOUR_MIN     0     /* Min. Value of Start/End Hour   */
#define A170_HOUR_MAX     24    /* Max. Value of Start/End Hour   */
#define A170_MINUTE_MIN   0     /* Min. Value of Start/End Minute */
#define A170_MINUTE_MAX   59    /* Max. Value of Start/End Minute */

#define A170_DATEPOS      0
#define A170_WEEKPOS      1
#define A170_SHOURPOS     2
#define A170_SMINUTEPOS   3
#define A170_EHOURPOS     4
#define A170_EMINUTEPOS   5

#define A170_MAX_LEN      2     /* Max. Length of Input Data      */
                                /* Date, Start/End Hour, Start/End Minute*/
#define A170_MAX_WEEKLEN  1     /* Max. Length of Input Data      */
                                /* Day of the week                */

#define A170_SPIN_STEP    1   /* Normal Step Count of Spin Button       */
#define A170_SPIN_TURBO   0   /* Turbo Point of Spin Button (0 = OFF)   */

/*
* ===========================================================================
*       Structure type defintions
* ===========================================================================
*/
/*
* ===========================================================================
*       Macro Difinitions
* ===========================================================================
*/
/*
* ===========================================================================
*       External Valiables
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototypes
* ===========================================================================
*/
VOID    A170InitDlg(HWND);
BOOL    A170SetData(HWND);
BOOL    A170ChkDatePos(HWND, WPARAM);
BOOL    A170ChkWeekPos(HWND, WPARAM);
BOOL    A170ChkHourPos(HWND, WPARAM);
BOOL    A170ChkMinutePos(HWND, WPARAM);
VOID    A170DispErr(HWND, WPARAM);
VOID    A170SpinProc(HWND, WPARAM, LPARAM);
HWND	A170ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND	A170ModeLessDialogWnd (void);

/* ===== End of A170.H ===== */
