#if POSSIBLE_DEAD_CODE
/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2172     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : PC interface, Header file for user                        
*   Category           : PC interface common                                               
*   Program Name       : PC2172.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date           :NAME               :Description
*    Nov-11-92      :H.NAKASHIMA        :Initial
*    Nov-17-93      :H.YAMAGUCHI        :ADDs cpm.h/pcstbcpm.h/ept.h
*    Mar-28-95      :O.Nakada           :ADDs R3.0
*    Jun-12-98      :O.Nakada           :Win32
*    Sep-07-98      :O.Nakada           :Delete cswai.h and pcstbwai.h
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/
/*************************** Add R3.0 *************************/
#ifndef _INC_PC2172
#define _INC_PC2172

#ifdef __cplusplus
extern "C" {
#endif
/*************************** Add R3.0 *************************/

#if     WIN32 && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#include "ecr.h"
#include "plu.h"		/* ###ADD Saratoga */
#include "paraequ.h"
#include "para.h"
#include "pcstbfcc.h"
#include "pcstbpar.h"
#include "csop.h"
#include "pcstbopr.h"
#include "csttl.h"
#include "pcstbttl.h"
#include "cscas.h"
#include "pcstbcas.h"
#include "csgcs.h"
#include "pcstbgcf.h"
#include "isp.h"
#include "pcstbisp.h"
#include "regstrct.h"
#include "transact.h"
#include "maint.h"
#include "pcif.h"
#include "ej.h"
#include "pcstbej.h"
#include "csetk.h"
#include "pcstbetk.h"
#include "cpm.h"
#include "ept.h"
#include "pcstbcpm.h"

#if     WIN32 && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif


/*************************** Add R3.0 *************************/
#ifdef __cplusplus
}
#endif

#endif                                      /* _INC_PC2172 */
/*************************** Add R3.0 *************************/
/*===== end of definition =====*/
#endif