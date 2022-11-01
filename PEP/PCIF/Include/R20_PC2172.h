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
#ifndef _INC_R20_PC2172
#define _INC_R20_PC2172

#ifdef __cplusplus
extern "C" {
#endif
/*************************** Add R3.0 *************************/

#if     WIN32 && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#include <TCHAR.H> //Included for UNICODE
#include "ecr.h"
#include "r20_plu.h"		/* ###ADD Saratoga */
#include "paraequ.h"
#include "r20_para.h"
#include "r20_pcstbfcc.h"
#include "r20_pcstbpar.h"
#include "r20_csop.h"
#include "r20_pcstbopr.h"
#include "csttl.h"
#include "r20_pcstbttl.h"
#include "r20_cscas.h"
#include "r20_pcstbcas.h"
#include "csgcs.h"
#include "r20_pcstbgcf.h"
#include "r20_isp.h"
#include "r20_pcstbisp.h"
#include "regstrct.h"
#include "transact.h"
#include "maint.h"
#include "pcif.h"
#include "ej.h"
#include "r20_pcstbej.h"
#include "r20_csetk.h"
#include "r20_pcstbetk.h"
#include "r20_cpm.h"
#include "r20_ept.h"
#include "r20_pcstbcpm.h"

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
