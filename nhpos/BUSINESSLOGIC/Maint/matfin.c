/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintFin Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATFIN.C
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstract: This function controls right "FINALIZE" in supervisor/program mode. 
*
*           The provided function names are as follows:  
*
*               VOID MaintFin( VOID )
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :   Name    : Description
* May-04-92 :          : J.Ikeda   : initial                                   
* Dec-28-99 :          : hrkato     : Saratoga
* Apr-04-15 : 02.02.01 : R.Chambers : remove clearing of global MaintWork in MaintFinExt()
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
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include <tchar.h>
#include <string.h>

#include "ecr.h"
#include "paraequ.h"
#include "para.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "maint.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "maintram.h"

/*
*===========================================================================
** Synopsis:    VOID MaintFin(UCHAR uchMinorClass)
*               
*     Input:    uchMinorClass
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*               
*
** Description: This function controls right "FINALIZE" in supervisor/program mode.
*===========================================================================
*/

VOID MaintFinExt(UCHAR uchMinorClass, USHORT usPrtControl)
{
    /* check maintenance/report operational counter */
    if ((uchMaintOpeCo != MAINT_1STOPERATION) ||
        (uchRptOpeCount & RPT_HEADER_PRINT)) {  /* header is printed */

		MaintMakeTrailerExt (uchMinorClass, usPrtControl);
    }

    /* --- Saratoga --- 
    if (MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTLOAN || 
        MaintWork.LoanPickup.uchMajorClass == CLASS_MAINTPICKUP) {
        if (MaintWork.LoanPickup.HeadTrail.usPrintControl & SLIP) {
            MaintSPTrailCtl(&MaintWork.LoanPickup.HeadTrail);
        }
    }
*/
    uchMaintOpeCo = MAINT_1STOPERATION;
    uchRptOpeCount = 0;
    usMaintACNumber = 0;                        /* Saratoga */
}

VOID MaintFin(UCHAR uchMinorClass)
{
	MaintFinExt(uchMinorClass, (PRT_JOURNAL | PRT_RECEIPT));
}

/* --- End of Source File --- */