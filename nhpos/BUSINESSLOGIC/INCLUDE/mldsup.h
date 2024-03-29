/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS E&M OISO                 **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline display module Include file used by user of this
* Category    : Multiline display, NCR 2170 US Hospitarity Application
* Program Name: MLDSUP.H                                                      
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
*    -  -95 : 03.00.00 : M.Waki     : Initial
* Aug-11-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
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
/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/* include files for change function name */
#include "mldlcd.h"

/**
;========================================================================
            D E F I N E    D E C L A R A T I O N s                        
;========================================================================
**/
// following defines are used to indicate report print state using the
// global variable uchUifACRptOnOffMld used in reports to determine if
// displaying the report to the screen or printing it. we've since added
// the historical reports folder functionality with the output going to a file.
#define RPT_DISPLAY_STREAM   2      // indicates report generated and written to a file
#define RPT_DISPLAY_ON       1      // indicates report generated and displayed in window
#define RPT_DISPLAY_OFF      0      // indicates report generated and printe. must be same as FALSE for use with RptCheckReportOnMld().

/* Print to file on/off Flag */  //ESMITH PRTFILE
// see also defines for PRT_SENDTO_MASKFUNC and the PRT_SENDTO_ indicators as 
// well as functions MaintSetPrintMode() and PrtSetPrintMode().
// this functionality is older attempt to print reports to a file.
#define RPT_PRINTFILE_ON	1      
#define RPT_PRINTFILE_OFF	0

/* Define For Report Pause */
#define  CID_MLDPKEYCHECK       1               /* Mld Pause Proccess Dialog ID */
#define  CID_MLDCKEYCHECK       2               /* Continue Proccess Dialog ID */
#define  MLD_PAUSE_END         -1               /* Successful */
#define  MLD_CONTINUE          -2               /* Continue    */
#define  MLD_ABORTED           -3               /* Aborted By User */

/**
;========================================================================
            E X T E R N A L    D E C L A R A T I O N s                        
;========================================================================
**/

extern UCHAR       uchUifACRptOnOffMld;
extern USHORT       uchUifTrailerOnOff;
extern UCHAR	   uchUifACRptOnOffFile; //ESMITH PRTFILE

extern UCHAR       uchUifACDataType;               /* Data Type Save Area */
extern UCHAR       uchUifACRptType;                /* Report Type Save Area */

extern CONST TCHAR FARCONST aszMldAC9[][64];
/* extern CONST UCHAR FARCONST aszMldAC20[][64]; */
/* extern CONST UCHAR FARCONST aszMldAC63[][64]; */
/* extern CONST UCHAR FARCONST aszMldAC68[][64]; */
//extern CONST TCHAR FARCONST aszMldAC71[][64];
//extern CONST TCHAR FARCONST	aszMldAC71_AddSettings[][64];
//extern CONST TCHAR FARCONST aszMldAC71_AddSettingsTitle[][64];
/* extern CONST UCHAR FARCONST aszMldAC114[][64]; */
//extern CONST TCHAR FARCONST aszMldAC116[][64];
//extern CONST TCHAR FARCONST aszMldAC124E[][64];
//extern CONST TCHAR FARCONST aszMldAC124R[][64];
extern CONST TCHAR FARCONST aszMldAC152[][64];
extern CONST TCHAR FARCONST aszMldAC153[][64];
extern CONST TCHAR FARCONST aszMldAC160[][64];
/* extern CONST UCHAR FARCONST aszMldAC161[][64]; */

/**
;========================================================================
            P R O T O T Y P E    D E C L A R A T I O N s                        
;========================================================================
**/
#if defined DISPLAY_MAIN
/*******************************************
* Prottype name of LCD 
********************************************/
/*VOID  MLDLCDPrtDouble(UCHAR *pszDest, USHORT   usDestLen, UCHAR *pszSource); *//* this function belong to Print*/

/* SHORT MldDispItem(VOID  *pItem, USHORT usStatus); */
VOID  MLDLCDMldRptSupHeader( MAINTHEADER *pData );
VOID  MLDLCDMldRptSupTrailer( MAINTTRAILER *pData );
VOID  MLDLCDMldRptSupFormTrailer(MAINTTRAILER *pData); 
/* SHORT MLDLCDMldIRptItemOther(USHORT usScroll); */
VOID  MLDLCDMldSupItoa( UCHAR uchData, TCHAR auchString[] );
SHORT MLDLCDMldRptSupGCF( RPTGUEST *pData );
SHORT MLDLCDMldRptSupCashWaitFile( VOID *pData );
SHORT MLDLCDMldRptSupRegFin( RPTREGFIN *pItem);
SHORT MLDLCDMldRptSupHourlyAct( RPTHOURLY *pItem);
SHORT MLDLCDMldRptSupDEPTFile( RPTDEPT *pItem, USHORT usStatus);
SHORT  MLDLCDMldRptSupPLUFile( RPTPLU *pData, USHORT usStatus);
SHORT  MLDLCDMldRptSupCPNFile( RPTCPN *pData, USHORT usStatus);
SHORT  MLDLCDMldRptSupETKFile( RPTEMPLOYEE *pData );
SHORT  MLDLCDMldRptSupEJ( RPTEJ *pItem);
SHORT MLDLCDMldRptSupServiceTime( RPTSERVICE *pItem);
SHORT MLDLCDMldRptSupProgRpt( RPTPROGRAM *pItem);
VOID MLDLCDMldRptSupMakeString(RPTEMPLOYEE *pData );
VOID  MLDLCDMldRptSupErrorCode( MAINTERRORCODE *pData );
VOID  MLDLCDMldPause(VOID); 
/*SHORT MLDLCDMldPkeyCheck(KEYMSG *pData);*/
SHORT MLDLCDMldContinueCheck(VOID); 
/*SHORT MLDLCDMldCkeyCheck(KEYMSG *pData);*/
VOID  MLDLCDMldRptSupTrans( MAINTTRANS *pData, USHORT usStatus );
VOID MLDLCDMldParaCASDisp(MAINTCASHIERIF *pDept);
/* VOID MLDLCDMldParaCashNoDisp(PARACASHIERNO *pCash, USHORT usClear); */
VOID MLDLCDMldParaPLUDisp(PARAPLU *pPlu, USHORT usStatus);
VOID MLDLCDMldParaDEPTDisp(PARADEPT *pDept);
VOID MLDLCDMldParaMenuPluDisp(PARAMENUPLUTBL *pMep);
VOID MLDLCDMldParaTaxDisp( MAINTTAXTBL *pTax );
VOID MLDLCDMldParaEmpNoDisp(PARAEMPLOYEENO *pEmp, USHORT usClear);
VOID MLDLCDMldMaintETKFl( MAINTETKFL *pData, USHORT usClear);
VOID MLDLCDMldParaOEPDisp(PARAOEPTBL *pOep);
VOID MLDLCDMldParaCPNDisp(PARACPN *pCpn, USHORT usStatus);
VOID MLDLCDMldParaPPI(PARAPPI *pPPI, USHORT usClear);


#endif

/* SHORT MldDispItem(VOID  *pItem, USHORT usStatus); */
VOID  MldRptSupHeader( MAINTHEADER *pData );
VOID  MldRptSupTrailer( MAINTTRAILER *pData );
VOID  MldRptSupFormTrailer(MAINTTRAILER *pData); 
/* SHORT MldIRptItemOther(USHORT usScroll); */
SHORT MldRptSupGCF( RPTGUEST *pData );
SHORT MldRptSupOpeStatus( MAINTOPESTATUS *pData );
SHORT MldRptSupCashWaitFile( VOID *pData );
SHORT MldRptSupRegFin( RPTREGFIN *pItem);
SHORT MldRptSupHourlyAct( RPTHOURLY *pItem);
SHORT MldRptSupDEPTFile( RPTDEPT *pItem, USHORT usStatus);
SHORT  MldRptSupPLUFile( RPTPLU *pData, USHORT usStatus);
SHORT  MldRptSupCPNFile( RPTCPN *pData, USHORT usStatus);
SHORT  MldRptSupETKFile( RPTEMPLOYEE *pData );
SHORT  MldRptSupEJ( RPTEJ *pItem);
SHORT  MldRptSupConfig(RPTEJ *pData);
SHORT MldRptSupServiceTime( RPTSERVICE *pItem);
SHORT MldRptSupProgRpt( RPTPROGRAM *pItem);
VOID MldRptSupMakeString(RPTEMPLOYEE *pData );
VOID  MldRptSupErrorCode( MAINTERRORCODE *pData );
VOID  MldPause(VOID); 
/*SHORT MldPkeyCheck(KEYMSG *pData);*/
SHORT MldContinueCheck(VOID); 
/*SHORT MldCkeyCheck(KEYMSG *pData);*/
VOID  MldRptSupTrans( MAINTTRANS *pData, USHORT usStatus );
VOID MldParaCASDisp(MAINTCASHIERIF *pDept);
/* VOID MldParaCashNoDisp(PARACASHIERNO *pCash, USHORT usClear); */
VOID MldParaPLUDisp(PARAPLU *pPlu, USHORT usStatus);
VOID MldParaDEPTDisp(PARADEPT *pDept);
VOID MldParaMenuPluDisp(PARAMENUPLUTBL *pMep);
VOID MldParaTaxDisp( MAINTTAXTBL *pTax );
VOID MldParaEmpNoDisp(PARAEMPLOYEENO *pEmp, USHORT usClear);
VOID MldMaintETKFl( MAINTETKFL *pData, USHORT usClear);
VOID MldParaOEPDisp(PARAOEPTBL *pOep);
VOID MldParaCPNDisp(PARACPN *pCpn, USHORT usStatus);
VOID MldParaPPI(PARAPPI *pPPI, USHORT usClear);

/**** End of File ****/
