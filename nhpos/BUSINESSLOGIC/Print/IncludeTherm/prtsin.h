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
* Title       : Print module Include file used in print module (Super & Prog)               
* Category    : Print, NCR 2170 US Hospitality Application
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
* Jun-23-93 : 01.00.12 : J.IKEDA    : Initial                                   
* Oct-05-93 : 02.00.01 : J.IKEDA    : Adds PrtThrmSupMaintETKFl()
*           :          :            : PrtThrmSupMakeETKFl()
* Aug-08-99 : 03.05.00 : M.Teraki   : Remove WAITER_MODEL
* Dec-19-02:          :R.Chambers : SR 15 for Software Security
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

/*
============================================================================
            A R G U M E N T   D E C L A R A T I O N s
============================================================================
*/

extern TCHAR aszPrThrmInTime[7 + 1];
extern TCHAR aszPrThrmOutTime[7 + 1];
extern TCHAR aszPrThrmInAMPM[2 + 1];
extern TCHAR aszPrThrmOutAMPM[2 + 1];
extern TCHAR aszPrThrmDate[5 + 1];
extern TCHAR aszPrThrmWorkTime[5 + 1 + 7 + 1];  /* R3.1 */


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

/*--- Thermal Printer ---*/                              /* FILE NAME  */
                                                       
VOID PrtThrmSupMnemData( VOID *pData );                  /* prsmnet.c  */
VOID PrtThrmSupComData( VOID *pData );                   /* prsdatat.c */
VOID PrtThrmSupMDC( VOID *pData );                       /* prsmdct.c  */
VOID PrtThrmSupDEPT( PARADEPT *pData );                  /* prsdeptt.c */                        
VOID PrtThrmSupPLU( PARAPLU *pData );                    /* prsplut.c  */
VOID PrtThrmSupPLUNoMenu( PARAPLUNOMENU *pData );        /* prspmeut.c */
VOID PrtThrmSupRound( PARAROUNDTBL *pData );             /* prsrnd.c   */
VOID PrtThrmSupFSC( PARAFSC *pData );                    /* prsfsct.c  */
VOID PrtThrmSupHourly( PARAHOURLYTIME *pData );          /* prshourt.c */
VOID PrtThrmSupFlexMem( PARAFLEXMEM *pData );            /* prsflext.c */
VOID PrtThrmSupStoRegNo( PARASTOREGNO *pData );          /* prsstot.c  */
VOID PrtThrmSupSupLevel( PARASUPLEVEL *pData );          /* prsslvlt.c */
VOID PrtThrmSupCurrency( PARACURRENCYTBL *pData );       /* prsfct.c   */
VOID PrtThrmSupTaxRate( PARATAXRATETBL *pData);          /* prstxrtt.c */
VOID PrtThrmSupTtlKeyCtl( PARATTLKEYCTL *pData);         /* prskctlt.c */
VOID PrtThrmSupCashierNo( MAINTCASHIERIF *pData );		 /* prscast.c  */
VOID PrtThrmSupTaxTbl( MAINTTAXTBL *pData);              /* prstxtbt.c */
VOID PrtThrmSupMenuPLU( MAINTMENUPLUTBL *pData );        /* prsmenpt.c */
VOID PrtThrmSupOutputLine( MAINTPRINTLINE *pData );      /* prscomt.c  */
VOID PrtThrmSupHeader( MAINTHEADER *pData );             /* prscomt.c  */
VOID PrtThrmSupTrailer( MAINTTRAILER *pData );           /* prscomt.c  */
VOID PrtThrmSupTrans( MAINTTRANS *pData );               /* prstrnt.c  */
VOID PrtThrmSupDspfline( MAINTDSPFLINE *pData );         /* prstrnt.c  */
VOID PrtThrmSupErrorCode( MAINTERRORCODE *pData );       /* prserrt.c  */
VOID PrtThrmSupRegFin( RPTREGFIN *pData );               /* prsregft.c */
VOID PrtThrmSupHourlyAct( RPTHOURLY *pData );            /* prshorpt.c */
VOID PrtThrmSupDEPTFile( RPTDEPT *pData );               /* prsderpt.c */
VOID PrtThrmSupPLUFile( RPTPLU *pData );                 /* prsplrpt.c */
VOID PrtThrmSupCashWaitFile( VOID *pData );              /* prscwrpt.c */
VOID PrtThrmSupWFClose( VOID *pData );                   /* prswclst.c */
VOID PrtThrmSupGCF( RPTGUEST *pData );                   /* prsgcft.c  */
VOID PrtThrmSupGCFClose( RPTGUEST *pData );              /* prsgcfct.c */
VOID PrtThrmSupGCNo( PARAGUESTNO *pData );               /* prsgcnot.c */
VOID PrtThrmSupRJFeed( MAINTFEED *pData );               /* prsfeedt.c */
VOID PrtThrmSupBcas( MAINTBCAS *pData );                 /* prsbcast.c */
VOID PrtThrmSupOpeStatus( MAINTOPESTATUS *pData );       /* prsopstt.c */
VOID PrtThrmSupTare( PARATARE *pData);                   /* prstaret.c */
VOID PrtThrmSupEmployeeNo( PARAEMPLOYEENO *pData );      /* prsempt.c  */
VOID PrtThrmSupETKFile( RPTEMPLOYEE *pData );            /* prsetkt.c  */
VOID PrtThrmSupETKFileJCSummary( PRPTJCSUMMARY pData );  /* prsetkt.c for SR07 */
VOID PrtSupEJ( RPTEJ *pData );                           /* prsejt.c   */
VOID PrtThrmSupPresetAmount( PARAPRESETAMOUNT *pData );  /* prspamtt.c */
VOID PrtThrmSupPigRule( PARAPIGRULE *pData );            /* prspigt.c  */
VOID PrtThrmSupMaintETKFl( MAINTETKFL *pData );          /* prsetkf.c  */
VOID PrtThrmSupTally( PARACPMTALLY *pData );             /* prstlyt.c  */
VOID  PrtThrmSupCpm( MAINTCPM *pData );                  /* prscpmt.c  */

VOID PrtThrmSupFormTrailer( MAINTTRAILER *pData );       /* prscomt.c  */
VOID PrtEJSupFormTrailer( MAINTTRAILER *pData );         /* prscomt.c  */
VOID PrtSupItoa( UCHAR uchData, TCHAR auchString[] );    /* prscomt.c  */
VOID PrtThrmSupMakeString(RPTEMPLOYEE *pData);           /* prsetkt.c  */
VOID PrtThrmSupMakeETKFl(MAINTETKFL *pData);             /* prsetkft.c */

VOID PrtThrmSupOepData( PARAOEPTBL *pData);              /* prsdatat.c */
VOID  PrtThrmSupCPN( PARACPN *pData );                   /* prscpnt.c  */
VOID  PrtThrmSupCPNFile( RPTCPN *pData );                /* prscprpt.c */

VOID PrtThrmSupFxDrive( PARAFXDRIVE *pData );            /* Prsdrvtt.c */
VOID PrtThrmSupCstrData( MAINTCSTR *pData );             /* prscstrt.c */

VOID PrtThrmSupPPI( PARAPPI *pData );                    /* prsppit.c  */
VOID PrtThrmSupServiceTime( RPTSERVICE *pData );         /* prsservt.c */
VOID PrtThrmSupDispPara( PARADISPPARA *pData );          /* prsdspst.c */

VOID PrtThrmSupDeptNoMenu( PARADEPTNOMENU *pData );      /* prsdmeut.c, 2172 */
VOID PrtThrmSupTOD(PARATOD *pData);                      /* prstodt.c, 2172 */
VOID  PrtThrmSupKdsIp( PARAKDSIP *pData );               /* prskdst.c  2172 */
VOID PrtThermSupAge( PARABOUNDAGE *pData );              /* prsaget.c 2172 */
VOID PrtThermSupRest( PARARESTRICTION *pData );          /* prsrestt.c 2172 */

VOID  PrtSupLoanPickup(VOID *pTran, MAINTLOANPICKUP *pData);     /* Saratoga */
VOID  PrtSupLoanPickup_VL(VOID *pTran, MAINTLOANPICKUP *pData);
VOID  PrtSupLoanPickup_SP(VOID *pTran, MAINTLOANPICKUP *pData);
USHORT PrtChkLoanPickupAdr(UCHAR uchMajorClass, UCHAR uchMinorClass);
VOID  PrtThrmSupMiscPara(PARAMISCPARA *pData);

VOID  PrtSupVLHead(MAINTSPHEADER *pData);
VOID  PrtSupVLTrail(VOID *pTran, MAINTSPHEADER *pData);
VOID  PrtSupVLLoanPickup(VOID *pTran, MAINTLOANPICKUP *pData);
VOID  PrtSupVLForQty(MAINTLOANPICKUP *pData);
VOID  PrtSupVLForeignTend(MAINTLOANPICKUP *pData);
VOID  PrtSupVLModifier(VOID *pTran, USHORT usModifier, USHORT usReasonCode);
VOID  PrtVLMnemAmt(USHORT usAdr, DCURRENCY lAmount, BOOL fsType);

VOID  PrtSupLoanPickup_TH(MAINTLOANPICKUP *pData);
VOID  PrtSupForeignTender_TH(CONST MAINTLOANPICKUP *pData);
VOID  PrtSupLoanPickup_EJ(MAINTLOANPICKUP *pData);
VOID  PrtSupForeignTender_EJ(MAINTLOANPICKUP *pData);
VOID  PrtThrmSupMiscPara(PARAMISCPARA *pData);
VOID  PrtThrmSupUnlockNo( PARAUNLOCKNO *pData );

/***** END of DEFINITION *****/
