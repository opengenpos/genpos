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
:   Program Name       : mldmenu.h                                        :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 95-04-04 : 00.00.00     :                         :
:                      :          :              :                         :
:                      :          :              :                         :
:    ** NCR2172 **     :          :              :                         :
:                      : 99-10-05 : 01.00.00     : Initial (for Win32)     :
:                      : 00-01-14 :              : function name change    :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/

#if !defined(MLDMENU_H_INCLUDED)
#define MLDMENU_H_INCLUDED
/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/* include files for change function name */
#include "mld2x20.h"
#include "mldlcd.h"

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
}MLDITEMDATA;

/* V3.3 */

typedef struct _MLDMENU {
    USHORT  usAddress;                         /* address of PARAMLDMNEMO */
    TCHAR   *aszData;                         /* entry data mnemonics    */
} MLDMENU;

//extern CONST MLDMENU aszMldAC20[];
//extern CONST MLDMENU aszMldAC63[];
//extern CONST MLDMENU aszMldAC68[];
extern CONST MLDMENU aszMldAC114[];

#define PRT_MAX_PERCENT  60000  

#define MLD_MENU_COLUMN 39

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*------------------------------------------------------------------------*\

            E X T E R N     D E F I N T I O Ns

\*------------------------------------------------------------------------*/
extern  const TCHAR aszPrtAM[];
extern  const TCHAR aszPrtPM[];

extern  MLDMENU CONST    * PGEntryMenuSatt[];
extern  MLDMENU CONST    * PGEntryMenu[];
extern  MLDMENU CONST    * PGEntrySatt[];
extern  MLDMENU CONST    * TypeSelectReport10[];   /* V3.3 */
extern  MLDMENU CONST    * TypeSelectReport9[];
extern  MLDMENU CONST    * TypeSelectReport8[];
extern  MLDMENU CONST    * TypeSelectReport7[];
extern  MLDMENU CONST    * TypeSelectReport6[];
extern  MLDMENU CONST    * TypeSelectReport5[];
extern  MLDMENU CONST    * TypeSelectReport4[];
extern  MLDMENU CONST    * TypeSelectReport3[];
extern  MLDMENU CONST    * TypeSelectReport2[];
extern  MLDMENU CONST    * TypeSelectReport1[];
extern  MLDMENU CONST    * ResetTypeSelect[];
extern  MLDMENU CONST    * DaylyPTDSelect[];
extern  MLDMENU CONST    * DaylyPTDSaveSelect[];
extern  MLDMENU CONST    * ACEntryMenu[];
extern  MLDMENU CONST    * ACEntryDisconnectedMenu[];
extern  MLDMENU CONST    * ACEntryMenuSatt[];
extern  MLDMENU CONST    * ACEntryMenuDisconnectedSatt[];
extern  MLDMENU CONST    * ACEntrySatt[];
extern  MLDMENU CONST    * ACEntryDisconnectedSatt[];
extern  MLDMENU CONST    * ACEntryMenuBMast[];
extern	MLDMENU	CONST    * TypeStoreForward[];
extern	MLDMENU	CONST    * TypeStoreForwardSubMenu[];
extern	MLDMENU CONST    * TypeAC444[];
extern	MLDMENU	CONST    * TypeStoreReport[];


//Implemented new touchscreen MLD menus for NHPOS
//these new menus lock the user out of some of the 
//AC and Program options that could potentially lock
//up NHPOS if used.  SRs 474,423, 466 JHHJ

extern MLDMENU CONST  *  PGEntryMenuTouch[];
extern MLDMENU CONST  *  PGEntryMenuSattTouch[];

extern MLDMENU CONST * ACEntryMenuTouch[];
extern MLDMENU CONST * ACEntryMenuDisconnectedTouch[];
extern MLDMENU CONST * ACEntryMenuSattTouch[];
extern MLDMENU CONST * ACEntryMenuDisconnectedSattTouch[];
extern MLDMENU CONST * ACEntryMenuBMastTouch[];
/* other define */


#if defined DISPLAY_MAIN
/*******************************************
* Prottype name of LCD 
********************************************/
/* prototype define */
UCHAR  MLDLCDUifACPGMLDDispCom(CONST MLDMENU * *); 
VOID   MLDLCDUifACPGMLDClear(VOID); 
/*VOID MLDLCDUifACMenu(UCHAR uchPageNumber);*/ /* this function belong to Uifsup*/
/*VOID MLDLCDUifPGMenu(UCHAR uchPageNumber);*/ /* this function belong to Uifprog*/
/*VOID MLDLCDPrtDouble(UCHAR *pszDest, USHORT   usDestLen, UCHAR *pszSource);*/ /* this function belong to Print*/

/*******************************************
* Prottype name of 2x20 display 
********************************************/
/* prototype define */
UCHAR  MLD2X20UifACPGMLDDispCom(CONST MLDMENU * *); 
VOID   MLD2X20UifACPGMLDClear(VOID); 
/*VOID MLD2X20UifACMenu(UCHAR uchPageNumber);*//* this function belong to Uifsup*/
/*VOID MLD2X20UifPGMenu(UCHAR uchPageNumber);*//* this function belong to Uifprog*/
/*VOID MLD2X20PrtDouble(UCHAR *pszDest, USHORT   usDestLen, UCHAR *pszSource); *//* this function belong to Print*/

#endif

/* prototype define */
UCHAR  UifACPGMLDDispCom(CONST MLDMENU * *); 
VOID   UifACPGMLDClear(VOID); 
VOID   UifACMenu(UCHAR uchPageNumber);
VOID   UifPGMenu(UCHAR uchPageNumber);
VOID   PrtDouble(TCHAR *pszDest, USHORT   usDestLen, TCHAR *pszSource); 

#endif
