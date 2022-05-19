/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file used in print module (Super & Prog)               
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtSIn.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-12-92 : 00.00.01 : K.Nakajima :                                    
* Nov-24-92 : 01.00.00 : J.Ikeda    : Adds PrtSupTare() Prototype                                   
* Nov-26-92 : 01.00.00 : K.You      : Adds PrtSupDEPTFile_s,
*           :          :            :  PrtSupGCF_s,PrtSupMenuPLU_s,
*           :          :            :  PrtSupPLUFile_s,PrtSupPLU_s.                                   
* Jun-21-93 : 01.00.12 : J.IKEDA    : Adds PrtSupWFClose(), PrtSupGCFClose()
* Jun-30-93 : 01.00.12 : J.IKEDA    : Adds PrtSupETKFile(), PrtSupEmployeeNo()
*           :          :            : PrtSupMakeData()
*           :          :            : Adds PrtSupETKFile_s(), 
*           :          :            : PrtSupMakeData_s()
* Jul-02-93 : 01.00.12 : K.You      : Adds PrtSupPresetAmount()
* Jul-12-93 : 01.00.12 : M.Yamamoto : Add PrtSupPigRule()
* Oct-06-93 : 02.00.01 : J.IKEDA    : Add PrtMaintETKFl()  PrtMaintETKFl_s()
*           :          :            : PrtSupMakeEtkFl()
*           :          :            : ARGUMENT DECLARATION
* Nov-15-93 : 02.00.02 : M.Yamamoto : Add Prstlyt.c
* Mar-01-95 : 03.00.00 : H.Ishida   : Add PrtSupOepData
* Mar-02-95 : 03.00.00 : H.Ishida   : Add PrtSupCPN
* Aug-11-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Dec-03-99 : 01.00.00 : hrkato     : Saratoga
* Jan-07-00 : 01.00.00 : H.Endo     : Add Include prtRJ.h for Printer RJ
* Nov-27-02 :          : R.Chambers : Add PrtSupETKFileJCSummary for LePeeps
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
/* endo udpate(add) 2000/01/07 */
#include <prtRJ.h>
/*
============================================================================
            A R G U M E N T   D E C L A R A T I O N s
============================================================================
*/

extern TCHAR aszPrtInTime[7 + 1];
extern TCHAR aszPrtOutTime[7 + 1];
extern TCHAR aszPrtInAMPM[2 + 1];
extern TCHAR aszPrtOutAMPM[2 + 1];
extern TCHAR aszPrtDate[5 + 1];
extern TCHAR aszPrtWorkTime[5 + +1 + 7 + 1];    /* R3.1 */

/*
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/

#define PRT_MAX_PERCENT  60000          



/*
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
*/

VOID PrtSupMnemData( VOID *pData );
VOID PrtSupComData( VOID *pData );
VOID PrtSupMDC( VOID *pData ); 
VOID PrtSupDEPT( PARADEPT *pData );
VOID PrtSupPLU( PARAPLU *pData );
VOID PrtSupPLUNoMenu( PARAPLUNOMENU *pData );
VOID PrtSupRound( PARAROUNDTBL *pData );
VOID PrtSupFSC( PARAFSC *pData ); 
VOID PrtSupHourly( PARAHOURLYTIME *pData );
VOID PrtSupFlexMem( PARAFLEXMEM *pData );
VOID PrtSupStoRegNo( PARASTOREGNO *pData );
VOID PrtSupSupLevel( PARASUPLEVEL *pData );
VOID PrtSupCurrency( PARACURRENCYTBL *pData );
VOID PrtSupTaxRate( PARATAXRATETBL *pData );
VOID PrtSupTtlKeyCtl( PARATTLKEYCTL *pData );
VOID PrtSupCashierNo( MAINTCASHIERIF *pItem );
VOID PrtSupTaxTbl( MAINTTAXTBL *pData );
VOID PrtSupMenuPLU( MAINTMENUPLUTBL *pData );
VOID PrtSupHeader( MAINTHEADER *pData ); 
VOID PrtSupTrailer( MAINTTRAILER *pData );
VOID PrtSupTrans( MAINTTRANS *pData );
VOID PrtSupErrorCode( MAINTERRORCODE *pData );
VOID PrtSupRegFin( RPTREGFIN *pData );
VOID PrtSupHourlyAct( RPTHOURLY *pData );
VOID PrtSupDEPTFile( RPTDEPT *pData );
VOID PrtSupPLUFile( RPTPLU *pData );
VOID PrtSupCashWaitFile( VOID *pData );
VOID PrtSupWFClose( VOID *pData );
VOID PrtSupGCF( RPTGUEST *pData );
VOID PrtSupGCFClose( RPTGUEST *pData );
VOID PrtSupGCNo( PARAGUESTNO *pData );
VOID PrtSupRJFeed( MAINTFEED *pData );
VOID PrtSupBcas( MAINTBCAS *pData );
VOID PrtSupOpeStatus( MAINTOPESTATUS *pData );
VOID PrtSupTare( PARATARE *pData );
VOID PrtSupEmployeeNo( PARAEMPLOYEENO *pData );
VOID PrtSupETKFile( RPTEMPLOYEE *pData );
VOID PrtSupETKFileJCSummary( PRPTJCSUMMARY  pData );  
VOID PrtSupPresetAmount( PARAPRESETAMOUNT *pData );
VOID PrtSupPigRule(PARAPIGRULE *pData);
VOID PrtSupMaintETKFl( MAINTETKFL *pData );  
VOID PrtSupTally(PARACPMTALLY *pData);
VOID  PrtSupCpm(MAINTCPM *pCpm);
USHORT PrtSupChkType( USHORT usPrintControl );   
VOID  PrtSupItoa( UCHAR uchData, TCHAR auchString[] );
VOID PrtSup1stTrailer( MAINTTRAILER *pData ); 
VOID PrtSup2ndTrailer( MAINTTRAILER *pData ); 
VOID PrtSup3rdTrailer( MAINTTRAILER *pData ); 
VOID PrtSupMakeData(RPTEMPLOYEE *pData); 
VOID PrtSupMakeEtkFl(MAINTETKFL *pData); 

VOID PrtSupPLU_s( PARAPLU *pData );
VOID PrtSupDEPTFile_s( RPTDEPT *pData );
VOID PrtSupGCF_s( RPTGUEST *pData );
VOID PrtSupMenuPLU_s( MAINTMENUPLUTBL *pData );
VOID PrtSupPLUFile_s( RPTPLU *pData );
VOID PrtSupETKFile_s( RPTEMPLOYEE *pData );
VOID PrtSupMaintETKFl_s( MAINTETKFL *pData );  
VOID PrtSupOepData( PARAOEPTBL *pData);  
VOID PrtSupCPN( PARACPN *pData);  
VOID  PrtSupCPNFile( RPTCPN *pData ); 
VOID  PrtSupCPNFile_s( RPTCPN *pData ); 

VOID PrtSupFxDrive( PARAFXDRIVE *pData );         /* For A/C162 */
VOID PrtSupCstrData( MAINTCSTR *pData );
VOID PrtSupPPI( PARAPPI *pData );
VOID PrtSupServiceTime( RPTSERVICE *pData );
VOID  PrtSupDispPara( PARADISPPARA *pData );
VOID PrtSupDeptNoMenu( PARADEPTNOMENU *pData ); /* 2172 */
VOID PrtSupTOD(PARATOD *pData);                 /* 2172 */
VOID  PrtSupKdsIp( PARAKDSIP *pData );          /* 2172 */
VOID PrtSupAge( PARABOUNDAGE *pData ); /* 2172 */
VOID PrtSupRest( PARARESTRICTION *pData ); /* 2172 */
VOID  PrtSupLoanPickup(VOID *pTran, MAINTLOANPICKUP *pData);        /* Saratoga */
VOID  PrtSupLoanPickup_RJ(MAINTLOANPICKUP *pData,UCHAR usFuncType);
VOID  PrtSupForeignTender_RJ(MAINTLOANPICKUP *pData);
VOID  PrtSupLoanPickup_VL(VOID *pTran, MAINTLOANPICKUP *pData);
VOID  PrtSupLoanPickup_SP(VOID *pTran, MAINTLOANPICKUP *pData);
UCHAR PrtChkLoanPickupAdr(UCHAR uchMajorClass, UCHAR uchMinorClass);
VOID  PrtSupMiscPara(PARAMISCPARA *pData);                          /* Saratoga */

VOID  PrtSupVLHead(MAINTSPHEADER *pData);
VOID  PrtSupVLTrail(VOID *pTran, MAINTSPHEADER *pData);
VOID  PrtSupVLLoanPickup(VOID *pTran, MAINTLOANPICKUP *pData);
VOID  PrtSupVLForQty(MAINTLOANPICKUP *pData);
VOID  PrtSupVLForeignTend(MAINTLOANPICKUP *pData);
VOID  PrtSupVLModifier(VOID *pTran, USHORT usModifier, USHORT usReasonCode);
VOID  PrtVLMnemAmt(USHORT usAdr, DCURRENCY lAmount, BOOL fsType);


/* --- End of Header File --- */