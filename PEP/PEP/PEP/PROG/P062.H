/*
* ---------------------------------------------------------------------------
* Title         :   Tender Key Parameter Header File (Prog. 62)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P062.H
* Copyright (C) :   1998, NCR Corp. E&M-OISO, All rights reserved.
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
* Sep-08-98 : 03.03.00 : A.Mitsui   : Initial
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
#define P62_SPIN_OFFSET     20

#define IDD_P62_01      26201
#define IDD_P62_02      26202
#define IDD_P62_03      26203
#define IDD_P62_04      26204
#define IDD_P62_05      26205
#define IDD_P62_06      26206
#define IDD_P62_07      26207
#define IDD_P62_08      26208
#define IDD_P62_09      26209
#define IDD_P62_10      26210
#define IDD_P62_11      26211
#define IDD_P62_12      26212
#define IDD_P62_13      26213
#define IDD_P62_14      26214
#define IDD_P62_15      26215
#define IDD_P62_16      26216
#define IDD_P62_17      26217
#define IDD_P62_18      26218
#define IDD_P62_19      26219
#define IDD_P62_20      26220

#define IDD_P62_SPIN1   (IDD_P62_01 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN2   (IDD_P62_02 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN3   (IDD_P62_03 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN4   (IDD_P62_04 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN5   (IDD_P62_05 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN6   (IDD_P62_06 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN7   (IDD_P62_07 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN8   (IDD_P62_08 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN9   (IDD_P62_09 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN10  (IDD_P62_10 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN11  (IDD_P62_11 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN12  (IDD_P62_12 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN13  (IDD_P62_13 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN14  (IDD_P62_14 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN15  (IDD_P62_15 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN16  (IDD_P62_16 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN17  (IDD_P62_17 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN18  (IDD_P62_18 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN19  (IDD_P62_19 + P62_SPIN_OFFSET)  
#define IDD_P62_SPIN20  (IDD_P62_20 + P62_SPIN_OFFSET)  

#define IDD_P62_TITLE	26241
#define IDD_P62_TEND1	26242
#define IDD_P62_TEND2	26243
#define IDD_P62_TEND3	26244
#define IDD_P62_TEND4	26245
#define IDD_P62_TEND5	26246
#define IDD_P62_TEND6	26247
#define IDD_P62_TEND7	26248
#define IDD_P62_TEND8	26249
#define IDD_P62_TEND9	26250
#define IDD_P62_TEND10	26251
#define IDD_P62_TEND11	26252
#define IDD_P62_CAPTION1	26253

#define IDS_P62_ERR_NOTUSE  26254
#define IDD_P62_TITLE_02 26264


#define IDD_P62_TEND12                  26255
#define IDD_P62_TEND13                  26256
#define IDD_P62_TEND14                  26257
#define IDD_P62_TEND15					26258
#define IDD_P62_TEND16                  26259
#define IDD_P62_TEND17                  26260
#define IDD_P62_TEND18                  26261
#define IDD_P62_TEND19                  26262
#define IDD_P62_TEND20                  26263

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P62_ADDR_MAX    MAX_TEND_NO     /* Max. Number of Address       */

#define P62_DATA_MAX   20               /* Max. Value of Data, Total Key #.  See also P98_TO_DATA_MAX */
#define P62_DATA_MIN    0               /* Min. Value of Data           */

#define P62_DATA_LEN    2               /* Max. Length of Input Data    */

#define P62_SPIN_STEP   1               /* Value of SpinButton Step     */
#define P62_SPIN_TURBO  0               /* SpinButton Turbo Flag        */

#define P62_DATA_NOTUSE 1               /* not use Data                 */

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
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
VOID    P62InitDlg(HWND);
VOID    P62DispErr(HWND, WPARAM, UINT);
VOID    P62SpinProc(HWND, WPARAM, LPARAM);
BOOL    P62SaveData(HWND);

/* ===== End of File (P062.H) ===== */
