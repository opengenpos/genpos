/*
* ---------------------------------------------------------------------------
* Title         :   Assignment Terminal No. Header File (Prog. 50)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P050.H
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
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Jan-24-96 : 03.00.05 : M.Suzuki   : Add R3.1
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
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID 
* ===========================================================================
*/
#define P50_EDIT_OFFSET			10
#define P50_SPIN_OFFSET			10
#define P50_COM_OFFSET			16	/* Offset needed for newly inserted com descriptors */

#define IDD_P50_SCBAR   25000
//#define IDD_P50_RPSCBAR 25200              /* NCR2172 */ ESMITH P50DLG CHANGE

#define IDD_P50_DESC01  (IDD_P50_SCBAR + 1)
#define IDD_P50_DESC02  (IDD_P50_SCBAR + 2)
#define IDD_P50_DESC03  (IDD_P50_SCBAR + 3)
#define IDD_P50_DESC04  (IDD_P50_SCBAR + 4)
#define IDD_P50_DESC05  (IDD_P50_SCBAR + 5)
#define IDD_P50_DESC06  (IDD_P50_SCBAR + 6)
#define IDD_P50_DESC07  (IDD_P50_SCBAR + 7)
#define IDD_P50_DESC08  (IDD_P50_SCBAR + 8)
#define IDD_P50_DESC09  (IDD_P50_SCBAR + 9)
#define IDD_P50_DESC10  (IDD_P50_SCBAR + 10)

#define IDD_P50_SHARE01 (IDD_P50_DESC01 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE02 (IDD_P50_DESC02 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE03 (IDD_P50_DESC03 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE04 (IDD_P50_DESC04 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE05 (IDD_P50_DESC05 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE06 (IDD_P50_DESC06 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE07 (IDD_P50_DESC07 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE08 (IDD_P50_DESC08 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE09 (IDD_P50_DESC09 + P50_EDIT_OFFSET)
#define IDD_P50_SHARE10 (IDD_P50_DESC10 + P50_EDIT_OFFSET)

#define IDD_P50_SHRSP01 (IDD_P50_SHARE01 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP02 (IDD_P50_SHARE02 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP03 (IDD_P50_SHARE03 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP04 (IDD_P50_SHARE04 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP05 (IDD_P50_SHARE05 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP06 (IDD_P50_SHARE06 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP07 (IDD_P50_SHARE07 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP08 (IDD_P50_SHARE08 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP09 (IDD_P50_SHARE09 + P50_SPIN_OFFSET)
#define IDD_P50_SHRSP10 (IDD_P50_SHARE10 + P50_SPIN_OFFSET)

#define IDD_P50_ALTER01 (IDD_P50_SHRSP01 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER02 (IDD_P50_SHRSP02 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER03 (IDD_P50_SHRSP03 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER04 (IDD_P50_SHRSP04 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER05 (IDD_P50_SHRSP05 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER06 (IDD_P50_SHRSP06 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER07 (IDD_P50_SHRSP07 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER08 (IDD_P50_SHRSP08 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER09 (IDD_P50_SHRSP09 + P50_EDIT_OFFSET)
#define IDD_P50_ALTER10 (IDD_P50_SHRSP10 + P50_EDIT_OFFSET)

#define IDD_P50_ALTSP01 (IDD_P50_ALTER01 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP02 (IDD_P50_ALTER02 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP03 (IDD_P50_ALTER03 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP04 (IDD_P50_ALTER04 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP05 (IDD_P50_ALTER05 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP06 (IDD_P50_ALTER06 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP07 (IDD_P50_ALTER07 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP08 (IDD_P50_ALTER08 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP09 (IDD_P50_ALTER09 + P50_SPIN_OFFSET)
#define IDD_P50_ALTSP10 (IDD_P50_ALTER10 + P50_SPIN_OFFSET)

#define	P50_RMDEVICE_CP_OFFSET	(IDD_P50_ALTSP01 + P50_EDIT_OFFSET)

#define IDD_P50_KPADD01 (P50_RMDEVICE_CP_OFFSET + 1)
#define IDD_P50_KPADD02 (P50_RMDEVICE_CP_OFFSET + 2)
#define IDD_P50_KPADD03 (P50_RMDEVICE_CP_OFFSET + 3)
#define IDD_P50_KPADD04 (P50_RMDEVICE_CP_OFFSET + 4)
#define IDD_P50_KPADD05 (P50_RMDEVICE_CP_OFFSET + 5)
#define IDD_P50_KPADD06 (P50_RMDEVICE_CP_OFFSET + 6)
#define IDD_P50_KPADD07 (P50_RMDEVICE_CP_OFFSET + 7)
#define IDD_P50_KPADD08 (P50_RMDEVICE_CP_OFFSET + 8)
#define IDD_P50_KPADD09 (P50_RMDEVICE_CP_OFFSET + 9)
#define IDD_P50_KPADD10 (P50_RMDEVICE_CP_OFFSET + 10)
#define IDD_P50_KPADD11 (P50_RMDEVICE_CP_OFFSET + 11)
#define IDD_P50_KPADD12 (P50_RMDEVICE_CP_OFFSET + 12)
#define IDD_P50_KPADD13 (P50_RMDEVICE_CP_OFFSET + 13)
#define IDD_P50_KPADD14 (P50_RMDEVICE_CP_OFFSET + 14)
#define IDD_P50_KPADD15 (P50_RMDEVICE_CP_OFFSET + 15)
#define IDD_P50_KPADD16 (P50_RMDEVICE_CP_OFFSET + 16)

#define IDD_P50_KPASP01 (IDD_P50_KPADD01 + P50_COM_OFFSET)
#define IDD_P50_KPASP02 (IDD_P50_KPADD02 + P50_COM_OFFSET)
#define IDD_P50_KPASP03 (IDD_P50_KPADD03 + P50_COM_OFFSET)
#define IDD_P50_KPASP04 (IDD_P50_KPADD04 + P50_COM_OFFSET)
#define IDD_P50_KPASP05 (IDD_P50_KPADD05 + P50_COM_OFFSET)
#define IDD_P50_KPASP06 (IDD_P50_KPADD06 + P50_COM_OFFSET)
#define IDD_P50_KPASP07 (IDD_P50_KPADD07 + P50_COM_OFFSET)
#define IDD_P50_KPASP08 (IDD_P50_KPADD08 + P50_COM_OFFSET)
#define IDD_P50_KPASP09 (IDD_P50_KPADD09 + P50_COM_OFFSET)
#define IDD_P50_KPASP10 (IDD_P50_KPADD10 + P50_COM_OFFSET)
#define IDD_P50_KPASP11 (IDD_P50_KPADD11 + P50_COM_OFFSET)
#define IDD_P50_KPASP12 (IDD_P50_KPADD12 + P50_COM_OFFSET)
#define IDD_P50_KPASP13 (IDD_P50_KPADD13 + P50_COM_OFFSET)
#define IDD_P50_KPASP14 (IDD_P50_KPADD14 + P50_COM_OFFSET)
#define IDD_P50_KPASP15 (IDD_P50_KPADD15 + P50_COM_OFFSET)
#define IDD_P50_KPASP16 (IDD_P50_KPADD16 + P50_COM_OFFSET)


#define IDD_P50_KPDES01 (IDD_P50_KPASP01 + P50_COM_OFFSET)
#define IDD_P50_KPDES02 (IDD_P50_KPASP02 + P50_COM_OFFSET)
#define IDD_P50_KPDES03 (IDD_P50_KPASP03 + P50_COM_OFFSET)
#define IDD_P50_KPDES04 (IDD_P50_KPASP04 + P50_COM_OFFSET)
#define IDD_P50_KPDES05 (IDD_P50_KPASP05 + P50_COM_OFFSET)
#define IDD_P50_KPDES06 (IDD_P50_KPASP06 + P50_COM_OFFSET)
#define IDD_P50_KPDES07 (IDD_P50_KPASP07 + P50_COM_OFFSET)
#define IDD_P50_KPDES08 (IDD_P50_KPASP08 + P50_COM_OFFSET)
#define IDD_P50_KPDES09 (IDD_P50_KPASP09 + P50_COM_OFFSET)
#define IDD_P50_KPDES10 (IDD_P50_KPASP10 + P50_COM_OFFSET)
#define IDD_P50_KPDES11 (IDD_P50_KPASP11 + P50_COM_OFFSET)
#define IDD_P50_KPDES12 (IDD_P50_KPASP12 + P50_COM_OFFSET)
#define IDD_P50_KPDES13 (IDD_P50_KPASP13 + P50_COM_OFFSET)
#define IDD_P50_KPDES14 (IDD_P50_KPASP14 + P50_COM_OFFSET)
#define IDD_P50_KPDES15 (IDD_P50_KPASP15 + P50_COM_OFFSET)
#define IDD_P50_KPDES16 (IDD_P50_KPASP16 + P50_COM_OFFSET)


#define IDD_P50_TITLE	25290
#define IDD_P50_CAPTION	25291
/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P50_DESC    IDS_P50
#define IDS_P50_KAD1    IDS_P50 + 1
#define IDS_P50_KAD2    IDS_P50 + 2
#define IDS_P50_KAD3    IDS_P50 + 3
#define IDS_P50_KAD4    IDS_P50 + 4
#define IDS_P50_KAD5    IDS_P50 + 5
#define IDS_P50_KAD6    IDS_P50 + 6
#define IDS_P50_KAD7    IDS_P50 + 7
#define IDS_P50_KAD8    IDS_P50 + 8

/*** NCR2172 ***/

#define IDS_P50_KAD9    IDS_P50 + 9
#define IDS_P50_KAD10   IDS_P50 +10
#define IDS_P50_KAD11   IDS_P50 +11
#define IDS_P50_KAD12   IDS_P50 +12
#define IDS_P50_KAD13   IDS_P50 +13
#define IDS_P50_KAD14   IDS_P50 +14
#define IDS_P50_KAD15   IDS_P50 +15
#define IDS_P50_KAD16   IDS_P50 +16

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P50_ADDR_MAX    MAX_SHAREDPRT_SIZE  /* Maximum Number of Address    */
#define P50_ADDR_NO     (P50_ADDR_MAX / 2)


#define P50_KPAD_MAX_REMOTE_DEVICE    8     /* Max. No. of remote devices in P50 dialog  */
#define P50_CPAD_MAX_REMOTE_DEVICE	  8		/* Max. No. of com ports (1 per remote device in P50 Dialog */ //ESMITH P50DLG CHANGE
                                            /* Added 2000.07.12 START */
#define P50_PORT_MAX    6                   /* Max. No. of Communication Port */
                                            /* Added 2000.07.12 END */


#define P50_KPAD_MAX    16                   /* Max. No. of Kitchen Printer  */
#define P50_KPCP_MAX_DESC	16				/* Total Descriptions, 1 for every kitchen printer address, 1 for every com port address */ //ESMITH P50DLG CHANGE

#define P50_KPAD_BASE   16                  /* Base. PointerNo. of Kitchen Printer  */
#define P50_COMAD_BASE  20                  /* Base. PointerNo. of Com port#  NCR2172 */

#define P50_DATA_MAX    16                  /* Max. Value of Input Data     */
#define P50_DATA_MIN    0                   /* Min. Value of Input Data     */
#define P50_DATA_LEN    2                   /* Max. Length of Input Data    */

#define P50_DESC_LEN    128                 /* Length of Description        */

#define P50_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P50_MAX_PAGE    10                  /* No. of Item in One Page      */
#define P50_ONE_PAGE    (P50_MAX_PAGE - P50_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */
#define P50_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#define P50_SCBAR_MAX   (P50_ADDR_NO - P50_MAX_PAGE) - 4
#endif

#define P50_SCBAR_MAX   (P50_ADDR_NO - P50_MAX_PAGE) - 8
                                            /* Max. Value of Scroll Range   */

#define P50_CHK_ADDR    2                   /* Even/Odd Address Checker     */

#define P50_SPIN_STEP   1
#define P50_SPIN_TURBO  0

/* NCR2172 */
#define P50_INDEXTOP01  0
#define P50_INDEXTOP02  1
#define P50_INDEXTOP03  2
#define P50_INDEXTOP04  3
#define P50_INDEXTOP05  4
#define P50_INDEXTOP06  5
#define P50_INDEXTOP07  6

#define P50_BASETOP01   0
#define P50_BASETOP02   1
#define P50_BASETOP03   2
#define P50_BASETOP04   3

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P50GETSHRINDEX(id, top) (BYTE)(((id) - IDD_P50_SHARE01) + (top))
#define P50GETALTINDEX(id, top) (BYTE)(((id) - IDD_P50_ALTER01) + (top))

#define P50GETINDEX(id, top)    (BYTE)(((id) - IDD_P50_KPADD01) /*+ top*/)
/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct _P50data {
    BYTE    bShare;         /* Data Value of P50 (Odd Address Data)     */
    BYTE    bAlter;         /* Data Value of P50 (Even Address Data)    */
} P50DATA, FAR *LPP50DATA;

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
VOID    P50InitDlg(HWND, LPP50DATA);
BOOL    P50GetData(HWND, WPARAM, LPP50DATA);
VOID    P50DispErr(HWND, int);
VOID    P50ResetData(HWND, WPARAM);
VOID    P50SpinProc(HWND, WPARAM, LPARAM, LPP50DATA);
VOID    P50ScrlProc(HWND, WPARAM, LPARAM, LPP50DATA);
BOOL    P50CalcIndex(WORD wScrlCode);
VOID    P50RedrawText(HWND, LPP50DATA);
VOID    P50RedrawThumb(LPARAM);

VOID
P50ScrlProcRP(HWND hDlg, WPARAM wParam, LPARAM lParam, LPP50DATA lpaData);
VOID    P50RedrawThumbRP(LPARAM lCtrl);
BOOL    P50CalcIndexRP(WORD wScrlCode);

/* ===== End of File (P050.H) ===== */
