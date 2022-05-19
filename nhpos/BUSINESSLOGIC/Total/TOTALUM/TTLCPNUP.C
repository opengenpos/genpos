/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : TOTAL, Total Update module Subroutine                         
* Category    : TOTAL, NCR 2170 US Hospitality Application         
* Program Name: Ttlcpnup.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*           SHORT TtlCpnupdate()   - Update Coupon Total
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Mar-07-95:03.00.00: hkato     : R3.0
* Nov-16-95:03.00.01:T.Nakahata : Bug Fixed (not affect at Training Operation)
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
#include	<tchar.h>
#include <memory.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <csttl.h>
#include <ttl.h>    
#include <regstrct.h>
#include <transact.h>
#include "ttltum.h"
#include <appllog.h>

/*
*============================================================================
**Synopsis:     VOID TtlCpnupdate(USHORT usStatus,
*                                 UCHAR  uchCpnNo, LONG lAmount, SHORT sCounter)
*
*    Input:     USHORT  usStatus    - Coupon Status
*               UCHAR   uchCpnNo    - Coupon Number
*               LONG    lAmount     - Update Amount
*               SHORT   sCounter    - Update Counter
*
*   Output:      Nothing
*    InOut:     Nothing
*
** Return:       Nothing
*
** Description  This function updates the Coupon Total in Work Area.
*
*============================================================================
*/
VOID TtlCpnupdate(USHORT usStatus, UCHAR uchCpnNo, DCURRENCY lAmount, SHORT sCounter)
{
    UCHAR   uchOffset;

   /* === Fix a glitch (11/16/95, Rel3.1) BEGIN === */
    if (usStatus & TTL_TUP_TRAIN) { /* Is operator training ? */
        return;
    }
   /* === Fix a glitch (11/16/95, Rel3.1) END === */

    uchOffset = uchCpnNo - (UCHAR)1;
    if (uchOffset >= FLEX_CPN_MAX) {
        TtlLog(MODULE_TTL_TUM_CPN, TTL_SIZEOVER, uchCpnNo);
                                    /* Record Log */
        uchTtlCpnStatus |= TTL_UPCPN_ERR;
        return;
    }

/* Current Daily Coupon Update */
    TtlCpnDayWork.CpnDayTotal[uchOffset].lAmount += lAmount; 
                                    /* Update Coupon Total Amount */
    TtlCpnDayWork.CpnDayTotal[uchOffset].sCounter += sCounter;
                                    /* Update Coupon Total Counter */ 
    TtlUpTotalFlag |= TTL_UPTOTAL_CPN;
                                    /* Set Coupon Updated Flag */
/* Current PTD Coupon Update */
    TtlCpnPTDWork.CpnPTDTotal[uchOffset].lAmount += lAmount; 
                                    /* Update Coupon Total Amount */
    TtlCpnPTDWork.CpnPTDTotal[uchOffset].sCounter += sCounter;
                                    /* Update Coupon Total Counter */ 
}

/* End of File */
