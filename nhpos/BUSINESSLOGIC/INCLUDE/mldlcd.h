/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline display module Include file used by user of this
* Category    : Multiline display, NCR 2170 US Hospitarity Application
* Program Name: mldlcd.h
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
*
** NCR2172 **
*
* Jan-14-00 : 01.00.00 : H.Endo     : 
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

//  This file contains macro definitions to translate the standard MLD function
//  names into a set of unique names for the LCD display.
//  The include file mld2x20.h contains the defines for DISPTYPE == 2
//  which are used to translate the standard MLD function names into a
//  set of unique names for the 2x20 display.
//
//  The display type is a compiler command line argument as part of the Properties set
//  in Visual Studio IDE with the C/C++ Preprocessor defines. See Preprocessor Definitions
//  in the Properties dialog for library Mldscl20, Mlddisp and MldDisp20.
//
//  This is a holdover from the NCR 2170 terminal and NCR 7448 terminal
//  when NHPOS, now GenPOS, allowed the use of either a 2x20 display
//  or an LCD monitor type of display.
//
//  The 2x20 display is a user interface display which contains two lines
//  each line having 20 characters. Because it was a cheaper display, customers
//  would choose that display over the more expensive LCD screen type of display.
//
//  Since the 2x20 display is technologically obsolete for modern point of sale
//  systems, this architecture should be updated to eliminate the 2x20 library
//  and functionality.
//
//  As a side note, a similar approach was used with the receipt printer library
//  which supported either a receipt printer with electronic journal or
//  a receipt printer and journal printer pair of printers. The older technology
//  of a receipt printer and journal printer pair was used with the NCR 2170
//  so the NHPOS software supported it.
//     Richard Chambers, Nov-09-2020

#if DISPTYPE ==1                    // LCD screen display type. See comments above.

#ifndef DISPLAYLCD_HEADER_DEFINED
#define DISPLAYLCD_HEADER_DEFINED

#define MldCheckSize                  MLDLCDMldCheckSize
#define MldChkAndCreFile              MLDLCDMldChkAndCreFile
#define MldChkAndDelFile              MLDLCDMldChkAndDelFile
/*#define MldCkeyCheck                  MLDLCDMldCkeyCheck*/
#define MldContinueCheck              MLDLCDMldContinueCheck
#define MldCreateFile                 MLDLCDMldCreateFile
#define MldCreatePopUp                MLDLCDMldCreatePopUp
#define MldDeletePopUp                MLDLCDMldDeletePopUp
#define MldForceDeletePopUp           MLDLCDMldForceDeletePopUp
#define MldDispCursor                 MLDLCDMldDispCursor
#define MldDispGCFQualifier           MLDLCDMldDispGCFQualifier
#define MldDispGuestNum               MLDLCDMldDispGuestNum
#define MldDispTableNum               MLDLCDMldDispTableNum
#define MldDispItem                   MLDLCDMldDispItem
#define MldDisplayInit                MLDLCDMldDisplayInit
#define MldDispSeatNum                MLDLCDMldDispSeatNum
#define MldDispSubTotal               MLDLCDMldDispSubTotal
#define MldDispWaitKey                MLDLCDMldDispWaitKey
#define MldDownCursor                 MLDLCDMldDownCursor
#define MldECScrollWrite              MLDLCDMldECScrollWrite
#define MldECScrollWrite2             MLDLCDMldECScrollWrite2
#define MldGetCursorDisplay           MLDLCDMldGetCursorDisplay
#define MldGetMldStatus               MLDLCDMldGetMldStatus
#define MldInit                       MLDLCDMldInit
#define MldIRptItemOther              MLDLCDMldIRptItemOther
#define MldMaintETKFl                 MLDLCDMldMaintETKFl
#define MldMoveCursor                 MLDLCDMldMoveCursor
#define MldParaCASDisp                MLDLCDMldParaCASDisp
#define MldParaCPNDisp                MLDLCDMldParaCPNDisp
#define MldParaDEPTDisp               MLDLCDMldParaDEPTDisp
#define MldParaEmpNoDisp              MLDLCDMldParaEmpNoDisp
#define MldParaMenuPluDisp            MLDLCDMldParaMenuPluDisp
#define MldParaOEPDisp                MLDLCDMldParaOEPDisp
#define MldParaPLUDisp                MLDLCDMldParaPLUDisp
#define MldParaPPI                    MLDLCDMldParaPPI
#define MldParaTaxDisp                MLDLCDMldParaTaxDisp
#define MldPause                      MLDLCDMldPause
/*#define MldPkeyCheck                  MLDLCDMldPkeyCheck*/
#define MldPopUpDownCursor            MLDLCDMldPopUpDownCursor
#define MldPopUpGetCursorDisplay      MLDLCDMldPopUpGetCursorDisplay
#define MldPopUpRedisplay             MLDLCDMldPopUpRedisplay
#define MldPopUpUpCursor              MLDLCDMldPopUpUpCursor
/*#define MldPrintf                     MLDLCDMldPrintf*/
/*#define MldPrintfAttr                 MLDLCDMldPrintfAttr*/
#define MldPutAllGcfToScroll          MLDLCDMldPutAllGcfToScroll
#define MldPutAllGcfToScroll2         MLDLCDMldPutAllGcfToScroll2
/*#define MldPutCurTransToScroll        MLDLCDMldPutCurTransToScroll*/
/*#define MldPutCurTransToScroll2       MLDLCDMldPutCurTransToScroll2*/
#define MldPutGcfToScroll             MLDLCDMldPutGcfToScroll
#define MldPutGcfToScroll2            MLDLCDMldPutGcfToScroll2
#define MldRedisplayToScroll          MLDLCDMldRedisplayToScroll
#define MldPutSplitTransToScroll      MLDLCDMldPutSplitTransToScroll
#define MldPutSplitTransToScroll2     MLDLCDMldPutSplitTransToScroll2
#define MldPutTransToScroll           MLDLCDMldPutTransToScroll
#define MldPutTransToScroll2          MLDLCDMldPutTransToScroll2
#define MldPutTrnToScrollReverse      MLDLCDMldPutTrnToScrollReverse
#define MldPutTrnToScrollReverse2     MLDLCDMldPutTrnToScrollReverse2
#define MldQueryCurScroll             MLDLCDMldQueryCurScroll
#define	MldSetMoveCursor			  MLDLCDMldSetMoveCursor
#define	MldGetVosWinHandle			  MLDLCDMldGetVosWinHandle
#define MldQueryMoveCursor            MLDLCDMldQueryMoveCursor
#define MldRefresh                    MLDLCDMldRefresh
#define MldRptSupCashWaitFile         MLDLCDMldRptSupCashWaitFile
#define MldRptSupCPNFile              MLDLCDMldRptSupCPNFile
#define MldRptSupDEPTFile             MLDLCDMldRptSupDEPTFile
#define MldRptSupEJ                   MLDLCDMldRptSupEJ
#define MldRptSupErrorCode            MLDLCDMldRptSupErrorCode
#define MldRptSupETKFile              MLDLCDMldRptSupETKFile
#define MldRptSupFormTrailer          MLDLCDMldRptSupFormTrailer
#define MldRptSupGCF                  MLDLCDMldRptSupGCF
#define MldRptSupHeader               MLDLCDMldRptSupHeader
#define MldRptSupHourlyAct            MLDLCDMldRptSupHourlyAct
#define MldRptSupMakeString           MLDLCDMldRptSupMakeString
#define MldRptSupPLUFile              MLDLCDMldRptSupPLUFile
#define MldRptSupProgRpt              MLDLCDMldRptSupProgRpt
#define MldRptSupRegFin               MLDLCDMldRptSupRegFin
#define MldRptSupServiceTime          MLDLCDMldRptSupServiceTime
#define MldRptSupTrailer              MLDLCDMldRptSupTrailer
#define MldRptSupTrans                MLDLCDMldRptSupTrans
#define MldScrollClear                MLDLCDMldScrollClear
#define MldScrollDown                 MLDLCDMldScrollDown
#define MldScrollFileWrite            MLDLCDMldScrollFileWrite
#define MldScrollFileWriteSpl         MLDLCDMldScrollFileWriteSpl
#define MldScrollFileDelete           MLDLCDMldScrollFileDelete
#define MldScrollFileDeleteExecute    MLDLCDMldScrollFileDeleteExecute
#define MldScrollUp                   MLDLCDMldScrollUp
#define MldScrollWrite                MLDLCDMldScrollWrite
#define MldScrollWrite2               MLDLCDMldScrollWrite2
#define MldSetCursor                  MLDLCDMldSetCursor
#define MldSetDescriptor              MLDLCDMldSetDescriptor
#define MldSetMode                    MLDLCDMldSetMode
#define MldString                     MLDLCDMldString
#define MldStringAttr                 MLDLCDMldStringAttr
#define MldUpCursor                   MLDLCDMldUpCursor
#define MldUpdatePopUp                MLDLCDMldUpdatePopUp
#define UifACPGMLDClear               MLDLCDUifACPGMLDClear
#define UifACPGMLDDispCom             MLDLCDUifACPGMLDDispCom
#endif
#endif
/* end of file*/
