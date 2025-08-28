/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file used by user of print module                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: Prt.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-18-92 : 00.00.01 : K.Nakajima :                                    
* May-24-94 : 02.05.00 : Yoshiko.Jim: Dupli.Print for CP,EPT (add PrtPrintCpEpt())
*
** NCR2172 **
*
* Jan-07-00 : 01.00.00 : H.Endo     : Add Include prtRJ.h or prtTherm.h for function name change
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
#ifndef PRT_H_HEADER_INCLUDE
#define PRT_H_HEADER_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <regstrct.h>
#include <transact.h>

#if defined PRINTER_RJ
/* include for function name change for RJ */
#include "PRTRJ.H"
#endif

#if defined PRINTER_THERMAL
/* include for function name change for Thermal */
#include "PRTTherm.H"   
#endif

/**
;========================================================================
            D E F I N E    D E C L A R A T I O N s                        
;========================================================================
**/
/* -- printer model status -- */
#define PRT_RJ_PRINTER          1                       /* R/J Printer Model */
#define PRT_THERMAL_PRINTER     2                       /* Thermal Printer Model */

/* -- shared printer abort key entry status -- */
#define PRT_ABORTED            -1                      /* abort key entered */

#define PRT_MAX_PERCENT  60000          /* used to check if PERCENT is illegal in RPTDEPT -> usPercent. used in MLD and Thermsup */

// The following two mask variables are used to control which printers will be used when printing.
// How these are used in when they are combined seems to vary in the printer source code.
// Sometimes both are used to indicate a printer is compulsory and sometimes only or
// the other is used to determine if a printer action should be done for a printer.
// There appear to be two types of masks that are used with these, one indicating the
// actual printer station. A physical printer may have more than one station such as
// a printer that has both thermal receipt station and a physical paper slip station.
typedef struct {
    USHORT  fsPrtCompul;       // value for/of printer global fsPrtCompul - mask indicating compulsory printers
    USHORT  fsPrtNoPrtMask;    // value for/of printer global fsPrtNoPrtMask - No-Print mask indicating printers allowed to print
} PrtPrintCompulMask;


extern const TCHAR aszPrtAM[];
extern const TCHAR aszPrtPM[];

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s 
;========================================================================
**/

#if defined PRINTER_MAIN

/* -- PrCTran_.c -- */
extern USHORT   RJPrtPrintTran(TRANINFORMATION *pTran);        /* transaction print */
extern VOID     RJPrtPrintDemand(TRANINFORMATION *pTran);      /* Print on Demand print */
extern VOID     RJPrtPrintSummaryReceipt(TRANINFORMATION *pTran, ITEMTENDER *pItemTender, int iPrintItems);
extern USHORT   RJPrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit, USHORT usType, USHORT usNo);
                                                      /* split gcf print */
extern VOID    RJPrtInitialize(VOID);                                  /* initalize static R3.1 */

/* -- PrCItem_.c -- */
extern USHORT   RJPrtPrintLine(USHORT usPrtControl, TCHAR *pLineToPrint);   /* item print  */
extern USHORT   RJPrtPrintLineImmediate(USHORT usPrtControl, TCHAR *pLineToPrint);   /* item print  */
extern USHORT   RJPrtPrintItem(TRANINFORMATION *pTran, VOID *pItem);   /* item print  */
extern USHORT   RJPrtPrintCpEpt(TRANINFORMATION *pTran, VOID *pItem);  /* dupli print */
extern VOID     RJPrtDispItem(TRANINFORMATION *pTran, VOID *pItem);    /* item display */
extern USHORT   RJPrtCheckTenderMedia(ITEMTENDER *pItem);
extern USHORT   RJPrtGetStatus(VOID);
extern USHORT   RJPrtDispItemForm(TRANINFORMATION  *pTran, VOID *pItem, TCHAR *puchBuffer);

/* -- PrCShr_.c -- */
extern SHORT   RJPrtCheckAbort(VOID);                /* check abort key entered or not */
extern VOID    RJPrtChangeFont(USHORT usType);       /* change font size of thermal printer */
extern VOID    RJPrtShrPartialSend(VOID);            /* send data partially ( for maintanance ) */
extern VOID    RJPrtShrInitReverse(VOID);
extern VOID    RJPrtShrEnd(VOID);
extern UCHAR   RJPrtCheckShr(UCHAR   uchTrgPrt);     /* check shared printer system or not */
extern SHORT   RJPrtShrTermLock(UCHAR uchPrtStatus);
extern VOID    RJPrtShrTermUnLock( VOID );



/* -- PrCTran_.c -- */
extern USHORT   ThermPrtPrintTran(TRANINFORMATION *pTran);        /* transaction print */
extern VOID     ThermPrtPrintDemand(TRANINFORMATION *pTran);      /* Print on Demand print */
extern VOID     ThermPrtPrintSummaryReceipt(TRANINFORMATION *pTran, ITEMTENDER *pItemTender, int iPrintItems);
extern USHORT   ThermPrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit, USHORT usType, USHORT usNo);
                                                      /* split gcf print */
extern VOID    ThermPrtInitialize(VOID);                                  /* initalize static R3.1 */

/* -- PrCItem_.c -- */
extern USHORT   ThermPrtPrintLine(USHORT usPrtControl, TCHAR *pLineToPrint);   /* item print  */
extern USHORT   ThermPrtPrintLineImmediate(USHORT usPrtControl, TCHAR *pLineToPrint);   /* item print  */
extern PrtPrintCompulMask  ThermPrtSetPrintCompulMask(PrtPrintCompulMask compulmask);   /* set printer compulsory and mask */
extern USHORT   ThermPrtPrintItem(TRANINFORMATION *pTran, VOID *pItem);   /* item print  */
extern USHORT   ThermPrtPrintCpEpt(TRANINFORMATION *pTran, VOID *pItem);  /* dupli print */
extern VOID     ThermPrtDispItem(TRANINFORMATION *pTran, VOID *pItem);    /* item display */
extern USHORT   ThermPrtCheckTenderMedia(ITEMTENDER *pItem);
extern USHORT   ThermPrtGetStatus(VOID);
extern USHORT   ThermPrtDispItemForm(TRANINFORMATION  *pTran, VOID *pItem, TCHAR *puchBuffer);

/* -- PrCShr_.c -- */
extern SHORT   ThermPrtCheckAbort(VOID);                /* check abort key entered or not */
extern VOID    ThermPrtChangeFont(USHORT usType);       /* change font size of thermal printer */
extern VOID    ThermPrtShrPartialSend(VOID);            /* send data partially ( for maintanance ) */
extern VOID    ThermPrtShrInitReverse(VOID);
extern VOID    ThermPrtShrEnd(VOID);
extern UCHAR   ThermPrtCheckShr(UCHAR   uchTrgPrt);     /* check shared printer system or not */
extern SHORT   ThermPrtShrTermLock(UCHAR uchPrtStatus);
extern VOID    ThermPrtShrTermUnLock( VOID );

#endif

/*-----------------------------------
    PROGRPT PRINT (PRRCOMN_.C)
-----------------------------------*/
USHORT PrtFeed(USHORT  usPrtType, USHORT usLineNum);
USHORT PrtPrint(USHORT usPrtType, CONST TCHAR *puchBuff, USHORT usLen);
USHORT PrtPrintf(USHORT usPrtType, const TCHAR *pszFormat, ...);
VOID   PrtDouble(TCHAR *pszDest, USHORT usDestLen, CONST TCHAR *pszSource);
VOID   PrtSupItoa( UCHAR uchData, TCHAR auchString[] );    /* prscomt.c  */

/* -- PrCTran_.c -- */
USHORT   PrtPrintTran(TRANINFORMATION *pTran);        /* transaction print */
VOID     PrtPrintDemand(TRANINFORMATION *pTran);      /* Print on Demand print */
VOID     PrtPrintSummaryReceipt(TRANINFORMATION *pTran, ITEMTENDER *pItemTender, int iPrintItems);      /* Print on receipt summary print */
USHORT   PrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit, USHORT usType, USHORT usNo);
                                                      /* split gcf print */
VOID    PrtInitialize(VOID);                                  /* initalize static R3.1 */

/* -- PrCItem_.c -- */
USHORT   PrtPrintLine(USHORT usPrtControl, TCHAR *pLineToPrint);   /* item print  */
USHORT   PrtPrintLineImmediate(USHORT usPrtControl, TCHAR *pLineToPrint);   /* item print  */
PrtPrintCompulMask  PrtSetPrintCompulMask(PrtPrintCompulMask compulmask);   /* set the printing control masks */
USHORT   PrtPrintItem(TRANINFORMATION *pTran, VOID *pItem);   /* item print  */
USHORT   PrtPrintCpEpt(TRANINFORMATION *pTran, VOID *pItem);  /* dupli print */
VOID     PrtDispItem(TRANINFORMATION *pTran, VOID *pItem);    /* item display */
USHORT   PrtCheckTenderMedia(ITEMTENDER *pItem);
USHORT   PrtGetStatus(VOID);
USHORT   PrtDispItemForm(TRANINFORMATION  *pTran, VOID *pItem, TCHAR *puchBuffer);

USHORT   PrtGetSlipPageLine(VOID);
VOID     PrtSetSlipPageLine(USHORT usLine);

/* -- PrCShr_.c -- */
SHORT   PrtCheckAbort(VOID);                /* check abort key entered or not */
VOID    PrtChangeFont(USHORT usType);       /* change font size of thermal printer */
VOID    PrtShrPartialSend(USHORT usPrintControl);    /* send data partially ( for maintanance ) */
VOID    PrtShrInitReverse(VOID);
VOID    PrtShrEnd(VOID);
UCHAR   PrtCheckShr(UCHAR   uchTrgPrt);     /* check shared printer system or not */
SHORT   PrtShrTermLock(UCHAR uchPrtStatus);
VOID    PrtShrTermUnLock( VOID );
VOID	PrtSetPrintMode(UCHAR uchMode);		//ESMITH PRTFILE

// US Customs to allow itmisc.c to call PrtCancel main funnel function
// -- PrRcncl_.c AND PrRcnclT.c --
// -- "Pr" = print
// -- "R" = receipt, not slip
// -- "cncl" = cancel module
// -- "_" = R/J module |  "t" = Thermal Module
VOID  PrtCancel(TRANINFORMATION  *pTran, ITEMMISC  *pItem);

SHORT  SetReceiptPrint(USHORT usCopyType);

#endif