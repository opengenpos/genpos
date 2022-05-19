/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
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
* Title       : Print module Include file used in printer(Thermal) module
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtRJ.H
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
*
** NCR2172 **
*
* Jan-07-00 : 01.00.00 : H.Endo     : 
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

// The following define test influences the compiling of the
// printer library. There are two types of printers
// that were supported by NHPOS now GenPOS back in the
// days of the NCR 2170 point of sale terminal.
//  - Receipt/Journal printer which had a printed journal (PRINTER_RJ)
//  - Receipt printer only which had an electronic journal (PRINTER_THERMAL)
// These defines are specified on the compiler command line so that
// they are defined with a compiler option on the command line and not
// through a preprocessor directive in a source file.
//   Richard Chambers, Nov-10-2020

#ifdef PRINTER_THERMAL
#ifndef PRTTHERMAL_H_HEADER_INCLUDE
#define PRTTHERMAL_H_HEADER_INCLUDE


#define   PrtPrintTran	        ThermPrtPrintTran
#define   PrtPrintDemand        ThermPrtPrintDemand
#define   PrtPrintSummaryReceipt    ThermPrtPrintSummaryReceipt
#define   PrtPrintSplit         ThermPrtPrintSplit
#define   PrtInitialize         ThermPrtInitialize

/* -- PrCItem_.c -- */
#define   PrtPrintLine          ThermPrtPrintLine
#define   PrtPrintLineImmediate          ThermPrtPrintLineImmediate
#define   PrtPrintItem          ThermPrtPrintItem
#define   PrtPrintCpEpt         ThermPrtPrintCpEpt
#define   PrtDispItem           ThermPrtDispItem
#define   PrtCheckTenderMedia   ThermPrtCheckTenderMedia
#define   PrtGetStatus          ThermPrtGetStatus
#define   PrtDispItemForm       ThermPrtDispItemForm

/* -- PrCShr_.c -- */
#define   PrtCheckAbort         ThermPrtCheckAbort
#define   PrtChangeFont         ThermPrtChangeFont
#define   PrtShrPartialSend     ThermPrtShrPartialSend
#define   PrtShrInitReverse     ThermPrtShrInitReverse
#define   PrtShrEnd             ThermPrtShrEnd
#define   PrtCheckShr           ThermPrtCheckShr
#define   PrtShrTermLock        ThermPrtShrTermLock
#define   PrtShrTermUnLock      ThermPrtShrTermUnLock

#endif
#endif