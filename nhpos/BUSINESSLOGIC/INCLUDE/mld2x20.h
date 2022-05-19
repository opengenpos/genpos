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
* Program Name: mld2x20.h
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
//  names into a set of unique names for the 2x20 display.
//  The include file mldlcd.h contains the defines for DISPTYPE == 1
//  which are used to translate the standard MLD function names into a
//  set of unique names for the LCD display.
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


#if DISPTYPE ==2                    // 2x20 display type. See comments above.

#ifndef DISPLAY2X20_HEADER_DEFINED
#define DISPLAY2X20_HEADER_DEFINED

#define aszMldStorage1Temp					 MLD2X20aszMldStorage1Temp
#define aszMldStorage1File                   MLD2X20aszMldStorage1File
#define aszMldStorage1Index                  MLD2X20aszMldStorage1Index
#define auchMldNoDispPLUHigh                 MLD2X20auchMldNoDispPLUHigh
#define auchMldNoDispPLULow                  MLD2X20auchMldNoDispPLULow
#define MldAddChk2                           MLD2X20MldAddChk2
#define MldCalStoSize                        MLD2X20MldCalStoSize
#define MldCancel                            MLD2X20MldCancel
#define MldCheckConsolidation                MLD2X20MldCheckConsolidation
#define MldCheckSize                         MLD2X20MldCheckSize
#define MldCheckSpecialStatus                MLD2X20MldCheckSpecialStatus
#define MldChkAndCreFile                     MLD2X20MldChkAndCreFile
#define MldChkAndDelFile                     MLD2X20MldChkAndDelFile
#define MldChkDisplayPluNo                   MLD2X20MldChkDisplayPluNo
#define MldChkStorageSize                    MLD2X20MldChkStorageSize
#define MldCopyGcfToStorage                  MLD2X20MldCopyGcfToStorage
#define MldCoupon                            MLD2X20MldCoupon
#define MldCreateFile                        MLD2X20MldCreateFile
#define MldCreateIndexFile                   MLD2X20MldCreateIndexFile
#define MldCreatePopUp                       MLD2X20MldCreatePopUp
#define MldDeletePopUp                       MLD2X20MldDeletePopUp
#define MldForceDeletePopUp                  MLD2X20MldForceDeletePopUp
#define MldDeptPLU                           MLD2X20MldDeptPLU
#define MldDispCondiment                     MLD2X20MldDispCondiment
#define MldDispCursor                        MLD2X20MldDispCursor
#define MldDispEndMessage                    MLD2X20MldDispEndMessage
#define MldDispGCFQualifier                  MLD2X20MldDispGCFQualifier
#define MldDispGuestNum                      MLD2X20MldDispGuestNum
#define MldDispItem                          MLD2X20MldDispItem
#define MldDisplayInit                       MLD2X20MldDisplayInit
#define MldDispOEPChildPlu                   MLD2X20MldDispOEPChildPlu
#define MldDispOEPLeadthrough                MLD2X20MldDispOEPLeadthrough
#define MldDispOrderEntryPrompt2             MLD2X20MldDispOrderEntryPrompt2
#define MldDispSeatNum                       MLD2X20MldDispSeatNum
#define MldDispSubTotal                      MLD2X20MldDispSubTotal
#define MldDispWaitKey                       MLD2X20MldDispWaitKey
#define MldDownCursor                        MLD2X20MldDownCursor
#define MldECScrollWrite                     MLD2X20MldECScrollWrite
#define MldECScrollWrite2                    MLD2X20MldECScrollWrite2
#define MldExpandFile                        MLD2X20MldExpandFile
#define MldForeignTender                     MLD2X20MldForeignTender
#define MldGetCursorDisplay                  MLD2X20MldGetCursorDisplay
#define MldGetMldStatus                      MLD2X20MldGetMldStatus
#define MldGetTrnStoToMldSto                 MLD2X20MldGetTrnStoToMldSto
#define MldInit                              MLD2X20MldInit
#define MldIRptItemOther                     MLD2X20MldIRptItemOther
#define MldIsSalesIDisc                      MLD2X20MldIsSalesIDisc
#define MldItemAffection                     MLD2X20MldItemAffection
#define MldItemCoupon                        MLD2X20MldItemCoupon
#define MldItemDisc                          MLD2X20MldItemDisc
#define MldItemMisc                          MLD2X20MldItemMisc
#define MldItemRegDisc                       MLD2X20MldItemRegDisc
#define MldItemSales                         MLD2X20MldItemSales
#define MldItemTender                        MLD2X20MldItemTender
#define MldItemTotal                         MLD2X20MldItemTotal
#define MldItemTransOpen                     MLD2X20MldItemTransOpen
#define MldLocalPutGcfToScroll               MLD2X20MldLocalPutGcfToScroll
#define MldLocalPutTransToScroll             MLD2X20MldLocalPutTransToScroll
#define MldLocalScrollWrite                  MLD2X20MldLocalScrollWrite
#define MldMoveCursor                        MLD2X20MldMoveCursor
#define MldPopUpDownCursor                   MLD2X20MldPopUpDownCursor
#define MldPopUpGetCursorDisplay             MLD2X20MldPopUpGetCursorDisplay
#define MldPopUpRedisplay                    MLD2X20MldPopUpRedisplay
#define MldPopUpUpCursor                     MLD2X20MldPopUpUpCursor
#define MldPutAllGcfToScroll                 MLD2X20MldPutAllGcfToScroll
#define MldPutAllGcfToScroll2                MLD2X20MldPutAllGcfToScroll2
#define MldPutGcfToScroll                    MLD2X20MldPutGcfToScroll
#define MldPutGcfToScroll2                   MLD2X20MldPutGcfToScroll2
#define MldRedisplayToScroll                 MLD2X20MldRedisplayToScroll
#define MldPutSplitTransToScroll             MLD2X20MldPutSplitTransToScroll
#define MldPutSplitTransToScroll2            MLD2X20MldPutSplitTransToScroll2
#define MldPutTransToScroll                  MLD2X20MldPutTransToScroll
#define MldPutTransToScroll2                 MLD2X20MldPutTransToScroll2
#define MldPutTrnToScrollReverse             MLD2X20MldPutTrnToScrollReverse
#define MldPutTrnToScrollReverse2            MLD2X20MldPutTrnToScrollReverse2
#define MldQueryCurScroll                    MLD2X20MldQueryCurScroll
#define	MldSetMoveCursor					 MLD2X20MldSetMoveCursor
#define	MldGetVosWinHandle					 MLD2X20MldGetVosWinHandle
#define MldQueryMoveCursor                   MLD2X20MldQueryMoveCursor
#define MldReadFile                          MLD2X20MldReadFile
#define MldRefresh                           MLD2X20MldRefresh
#define MldReorder                           MLD2X20MldReorder
#define MldResetItemDiscTmpStore             MLD2X20MldResetItemDiscTmpStore
#define MldResetItemSalesQtyPrice            MLD2X20MldResetItemSalesQtyPrice
#define MldResetSpecialStatus                MLD2X20MldResetSpecialStatus
#define MldScrollFileWrite                   MLD2X20MldScrollFileWrite
#define MldScrollWrite                       MLD2X20MldScrollWrite
#define MldScrollWrite2                      MLD2X20MldScrollWrite2
#define MldSetCondiment                      MLD2X20MldSetCondiment
#define MldSetCursor                         MLD2X20MldSetCursor
#define MldSetDescriptor                     MLD2X20MldSetDescriptor
#define MldSetDrive3ScrollSystem             MLD2X20MldSetDrive3ScrollSystem
#define MldSetItemSalesQtyPrice              MLD2X20MldSetItemSalesQtyPrice
#define MldSetMode                           MLD2X20MldSetMode
#define MldSetNoun                           MLD2X20MldSetNoun
#define MldSetPrecheckSystem                 MLD2X20MldSetPrecheckSystem
#define MldSetSalesConsolidation             MLD2X20MldSetSalesConsolidation
#define MldSetSalesCouponQTY                 MLD2X20MldSetSalesCouponQTY
#define MldSetScrollSymbol                   MLD2X20MldSetScrollSymbol
#define MldSetSpecialStatus                  MLD2X20MldSetSpecialStatus
#define MldSetSuperModeSystem                MLD2X20MldSetSuperModeSystem
#define MldStoAffect                         MLD2X20MldStoAffect
#define MldStoAppendIndex                    MLD2X20MldStoAppendIndex
#define MldStoCloseFile                      MLD2X20MldStoCloseFile
#define MldStoCoupon                         MLD2X20MldStoCoupon
#define MldStoDisc                           MLD2X20MldStoDisc
#define MldStoMisc                           MLD2X20MldStoMisc
#define MldStoMulti                          MLD2X20MldStoMulti
#define MldStoNormal                         MLD2X20MldStoNormal
#define MldStoOpen                           MLD2X20MldStoOpen
#define MldStoOpenFile                       MLD2X20MldStoOpenFile
#define MldStorageInit                       MLD2X20MldStorageInit
#define MldStoSalesPreviousCondiment         MLD2X20MldStoSalesPreviousCondiment
#define MldStoSales                          MLD2X20MldStoSales
#define MldStoSetupPrtIdx                    MLD2X20MldStoSetupPrtIdx
#define MldStoTender                         MLD2X20MldStoTender
#define MldStoTotal                          MLD2X20MldStoTotal
#define MldString                            MLD2X20MldString
#define MldTempRestore                       MLD2X20MldTempRestore
#define MldTempStore                         MLD2X20MldTempStore
#define MldTender                            MLD2X20MldTender
#define MldTmpDiscRestore                    MLD2X20MldTmpDiscRestore
#define MldTmpSalesRestore                   MLD2X20MldTmpSalesRestore
#define MldTotal                             MLD2X20MldTotal
#define MldUpCursor                          MLD2X20MldUpCursor
#define MldUpdatePopUp                       MLD2X20MldUpdatePopUp
#define MldWriteFile                         MLD2X20MldWriteFile
#define MldScrollFileWriteSpl                MLD2X20MldScrollFileWriteSpl
#define MldScrollFileDelete                  MLD2X20MldScrollFileDelete
#define MldScrollFileDeleteExecute           MLD2X20MldScrollFileDeleteExecute
#define UifACPGMLDClear                      MLD2X20UifACPGMLDClear
#define UifACPGMLDDispCom                    MLD2X20UifACPGMLDDispCom
#endif
#endif

/* end of file */
