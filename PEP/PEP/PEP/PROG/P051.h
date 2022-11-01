/*
* ---------------------------------------------------------------------------
* Title         :   Assignment IP Address for KDS Header File (Prog # 51)
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
* Nov-15-93 : 00.00.01 :            : Initial (Migrate from HP US Model)
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
/*
* ===========================================================================
*       Dialog Controls Definitions
* ===========================================================================
*/
//****************************
//	SR 14 ESMITH
//#define P51_SPIN_OFFSET    30
#define P51_SPIN_OFFSET    90

#define IDD_P51_MDC363BITD    15100   /* KDS No.1 IP Address */

#define IDD_P51_IPADD11    15101   /* KDS No.1 IP Address */
#define IDD_P51_IPADD21    15105   /* KDS No.2 IP Address */
#define IDD_P51_IPADD31    15109   /* KDS No.3 IP Address */
#define IDD_P51_IPADD41    15113   /* KDS No.3 IP Address */
#define IDD_P51_IPADD51    15117   /* KDS No.1 IP Address */
#define IDD_P51_IPADD61    15121   /* KDS No.2 IP Address */
#define IDD_P51_IPADD71    15125   /* KDS No.3 IP Address */
#define IDD_P51_IPADD81    15129   /* KDS No.3 IP Address */

#define IDD_P51_IPADD12    15102   /* KDS No.1 IP Address */
#define IDD_P51_IPADD22    15106   /* KDS No.2 IP Address */
#define IDD_P51_IPADD32    15110   /* KDS No.3 IP Address */
#define IDD_P51_IPADD42    15114   /* KDS No.3 IP Address */
#define IDD_P51_IPADD52    15118   /* KDS No.1 IP Address */
#define IDD_P51_IPADD62    15122   /* KDS No.2 IP Address */
#define IDD_P51_IPADD72    15126   /* KDS No.3 IP Address */
#define IDD_P51_IPADD82    15130   /* KDS No.3 IP Address */

#define IDD_P51_IPADD13    15103   /* KDS No.1 IP Address */
#define IDD_P51_IPADD23    15107   /* KDS No.2 IP Address */
#define IDD_P51_IPADD33    15111   /* KDS No.3 IP Address */
#define IDD_P51_IPADD43    15115   /* KDS No.3 IP Address */
#define IDD_P51_IPADD53    15119   /* KDS No.1 IP Address */
#define IDD_P51_IPADD63    15123   /* KDS No.2 IP Address */
#define IDD_P51_IPADD73    15127   /* KDS No.3 IP Address */
#define IDD_P51_IPADD83    15131   /* KDS No.3 IP Address */

#define IDD_P51_IPADD14    15104   /* KDS No.1 IP Address */
#define IDD_P51_IPADD24    15108   /* KDS No.2 IP Address */
#define IDD_P51_IPADD34    15112   /* KDS No.3 IP Address */
#define IDD_P51_IPADD44    15116   /* KDS No.3 IP Address */
#define IDD_P51_IPADD54    15120   /* KDS No.1 IP Address */
#define IDD_P51_IPADD64    15124   /* KDS No.2 IP Address */
#define IDD_P51_IPADD74    15128   /* KDS No.3 IP Address */
#define IDD_P51_IPADD84    15132   /* KDS No.3 IP Address */


#define IDD_P51_PORT1      15133   /* KDS No.1 Port No.   */
#define IDD_P51_PORT2      15134   /* KDS No.2 Port No.   */
#define IDD_P51_PORT3      15135   /* KDS No.3 Port No.   */
#define IDD_P51_PORT4      15136   /* KDS No.4 Port No.   */

//************************
//	SR 14 ESMITH
#define IDD_P51_PORT5      15137   /* KDS No.5 Port No.   */
#define IDD_P51_PORT6      15138   /* KDS No.6 Port No.   */
#define IDD_P51_PORT7      15139   /* KDS No.7 Port No.   */
#define IDD_P51_PORT8      15140   /* KDS No.8 Port No.   */
#define IDD_P51_PORT9      15141   /* KDS No.9 Port No.   */
#define IDD_P51_PORT10     15142   /* KDS No.10 Port No.   */
#define IDD_P51_PORT11     15143   /* KDS No.11 Port No.   */
#define IDD_P51_PORT12     15144   /* KDS No.12 Port No.   */
#define IDD_P51_PORT13     15145   /* KDS No.13 Port No.   */
#define IDD_P51_PORT14     15146   /* KDS No.14 Port No.   */
#define IDD_P51_PORT15     15147   /* KDS No.15 Port No.   */
#define IDD_P51_PORT16     15148   /* KDS No.16 Port No.   */

#define IDD_P51_IPADDSPN51 (IDD_P51_IPADD51 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN52 (IDD_P51_IPADD52 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN53 (IDD_P51_IPADD53 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN54 (IDD_P51_IPADD54 + P51_SPIN_OFFSET)

#define IDD_P51_IPADDSPN61 (IDD_P51_IPADD61 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN62 (IDD_P51_IPADD62 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN63 (IDD_P51_IPADD63 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN64 (IDD_P51_IPADD64 + P51_SPIN_OFFSET)

#define IDD_P51_IPADDSPN71 (IDD_P51_IPADD71 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN72 (IDD_P51_IPADD72 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN73 (IDD_P51_IPADD73 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN74 (IDD_P51_IPADD74 + P51_SPIN_OFFSET)

#define IDD_P51_IPADDSPN81 (IDD_P51_IPADD81 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN82 (IDD_P51_IPADD82 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN83 (IDD_P51_IPADD83 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN84 (IDD_P51_IPADD84 + P51_SPIN_OFFSET)

#define IDD_P51_PORTSPN5 (IDD_P51_PORT5 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN6 (IDD_P51_PORT6 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN7 (IDD_P51_PORT7 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN8 (IDD_P51_PORT8 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN9 (IDD_P51_PORT9 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN10 (IDD_P51_PORT10 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN11 (IDD_P51_PORT11 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN12 (IDD_P51_PORT12 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN13 (IDD_P51_PORT13 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN14 (IDD_P51_PORT14 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN15 (IDD_P51_PORT15 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN16 (IDD_P51_PORT16 + P51_SPIN_OFFSET)
//
//**************************


#define IDD_P51_IPADDSPN11 (IDD_P51_IPADD11 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN12 (IDD_P51_IPADD12 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN13 (IDD_P51_IPADD13 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN14 (IDD_P51_IPADD14 + P51_SPIN_OFFSET)

#define IDD_P51_IPADDSPN21 (IDD_P51_IPADD21 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN22 (IDD_P51_IPADD22 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN23 (IDD_P51_IPADD23 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN24 (IDD_P51_IPADD24 + P51_SPIN_OFFSET)

#define IDD_P51_IPADDSPN31 (IDD_P51_IPADD31 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN32 (IDD_P51_IPADD32 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN33 (IDD_P51_IPADD33 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN34 (IDD_P51_IPADD34 + P51_SPIN_OFFSET)

#define IDD_P51_IPADDSPN41 (IDD_P51_IPADD41 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN42 (IDD_P51_IPADD42 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN43 (IDD_P51_IPADD43 + P51_SPIN_OFFSET)
#define IDD_P51_IPADDSPN44 (IDD_P51_IPADD44 + P51_SPIN_OFFSET)

#define IDD_P51_PORTSPN1 (IDD_P51_PORT1 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN2 (IDD_P51_PORT2 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN3 (IDD_P51_PORT3 + P51_SPIN_OFFSET)
#define IDD_P51_PORTSPN4 (IDD_P51_PORT4 + P51_SPIN_OFFSET)

#define IDD_P51_STRNO       15150
#define IDD_P51_STR1        (IDD_P51_STRNO + 1)
#define IDD_P51_STR2        (IDD_P51_STRNO + 2)
#define IDD_P51_STR3        (IDD_P51_STRNO + 3)
#define IDD_P51_STR4        (IDD_P51_STRNO + 4)
#define IDD_P51_STR5        (IDD_P51_STRNO + 5)
#define IDD_P51_STR6        (IDD_P51_STRNO + 6)
#define IDD_P51_STR7        (IDD_P51_STRNO + 7)
#define IDD_P51_STR8        (IDD_P51_STRNO + 8)

//************************
//	SR 14 ESMITH
#define IDD_P51_STR9         (IDD_P51_STRNO +  9)
#define IDD_P51_STR10        (IDD_P51_STRNO + 10)
#define IDD_P51_STR11        (IDD_P51_STRNO + 11)
#define IDD_P51_STR12        (IDD_P51_STRNO + 12)
#define IDD_P51_STR13        (IDD_P51_STRNO + 13)
#define IDD_P51_STR14        (IDD_P51_STRNO + 14)
#define IDD_P51_STR15        (IDD_P51_STRNO + 15)
#define IDD_P51_STR16        (IDD_P51_STRNO + 16)
#define IDD_P51_STR17        (IDD_P51_STRNO + 17)
#define IDD_P51_STR18        (IDD_P51_STRNO + 18)
#define IDD_P51_STR19        (IDD_P51_STRNO + 19)
#define IDD_P51_STR20        (IDD_P51_STRNO + 20)
#define IDD_P51_STR21        (IDD_P51_STRNO + 21)
#define IDD_P51_STR22        (IDD_P51_STRNO + 22)
#define IDD_P51_STR23        (IDD_P51_STRNO + 23)
#define IDD_P51_STR24        (IDD_P51_STRNO + 24)
//
//*************************


#define IDD_P51_STRDELILEN  (IDD_P51_STRNO + 25)
#define IDD_P51_STRPOSLEN   (IDD_P51_STRNO + 26)
#define IDD_P51_STRPOSLEN2  (IDD_P51_STRNO + 27)

/*
* ===========================================================================
*       String ID Definitions
* ===========================================================================
*/

#define IDS_P51_CAPTION     (IDS_P51)
#define IDS_P51_DESC1       (IDS_P51 +  1)
#define IDS_P51_DESC2       (IDS_P51 +  2)
#define IDS_P51_DESC3       (IDS_P51 +  3)
#define IDS_P51_DESC4       (IDS_P51 +  4)
#define IDS_P51_DESC5       (IDS_P51 +  5)
#define IDS_P51_DESC6       (IDS_P51 +  6)
#define IDS_P51_DESC7       (IDS_P51 +  7)
#define IDS_P51_DESC8       (IDS_P51 +  8)
#define IDS_P51_DELILEN     (IDS_P51 +  9)
#define IDS_P51_POSLEN      (IDS_P51 + 10)
//************************
//	SR 14 ESMITH
#define IDS_P51_DESC9       (IDS_P51 + 11)
#define IDS_P51_DESC10      (IDS_P51 + 12)
#define IDS_P51_DESC11      (IDS_P51 + 13)
#define IDS_P51_DESC12      (IDS_P51 + 14)
#define IDS_P51_DESC13      (IDS_P51 + 15)
#define IDS_P51_DESC14      (IDS_P51 + 16)
#define IDS_P51_DESC15      (IDS_P51 + 17)
#define IDS_P51_DESC16      (IDS_P51 + 18)
#define IDS_P51_DESC17      (IDS_P51 + 19)
#define IDS_P51_DESC18      (IDS_P51 + 20)
#define IDS_P51_DESC19      (IDS_P51 + 21)
#define IDS_P51_DESC20      (IDS_P51 + 22)
#define IDS_P51_DESC21      (IDS_P51 + 23)
#define IDS_P51_DESC22      (IDS_P51 + 24)
#define IDS_P51_DESC23      (IDS_P51 + 25)
#define IDS_P51_DESC24      (IDS_P51 + 26)
//
//************************

/*
* ===========================================================================
*       Value Definitions
* ===========================================================================
*/
#define P51_IPPOS_MIN     0     /* Min. Value of IP Address Position    */
#define P51_IPPOS_MAX     255   /* Max. Value of IP Address Position    */
#define P51_DATA_MIN      0     /* Min. Value of Port No.               */
#define P51_DATA_MAX      9999  /* Max. Value of Port No.               */

/*** NCR2172
#define P51_MAX_ROW      3
***/
//*********************************
//	SR 14 ESMITH
//#define P51_MAX_ROW      4
#define P51_MAX_COL      4
#define P51_MAX_ROW      8
#define P51_MAX_PORT	16


#define P51_IPADD1POS     0
#define P51_IPADD2POS     1
#define P51_IPADD3POS     2
#define P51_IPADD4POS     3
#define P51_IPADD5POS     4
#define P51_IPADD6POS     5
#define P51_IPADD7POS     6
#define P51_IPADD8POS     7
#define P51_PORTPOS       0

#define P51_MAX_IPLEN     3   /* Max. Length of Input IP Data(TEXT)     */
#define P51_MAX_PORTLEN   4   /* Max. Length of Input PORT Data(TEXT)   */

#define P51_SPIN_STEP     1   /* Normal Step Count of Spin Button       */
#define P51_SPIN_TURBO    0   /* Turbo Point of Spin Button (0 = OFF)   */

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
VOID    P051InitData(HWND);
BOOL    P051SetData(HWND);
BOOL    P051ChkRndPos(HWND, WPARAM);
VOID    P051DispErr(HWND, WPARAM);
VOID    P051SpinProc(HWND, WPARAM, LPARAM);

/* ===== End of P051.H ===== */
