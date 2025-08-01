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
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1997-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Loan/Pickup Amount Print
* Category    : Print, NCR 2170 GP R2.0 Application
* Program Name: PrSLPJ_.C
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtSupLoanPickup_EJ() : prints loan/pickup (receipt & journal)
*               PrtSupForeignTender_EJ() : prints loan/pickup (receipt & journal)
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Apr-04-97 : 02.00.05 :T.Yatuhasi  : initial
* Feb-19-00 : 01.00.00 :hrkato      : Saratoga
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

#if defined(POSSIBLE_DEAD_CODE)

// NOTE: The following functions are used with PrtSupLoanPickup()
//       functionality. The source code for PrtSupForeignTender_TH()
//       and PrtSupForeignTender_EJ() have been moved to source file prslpt.c
//       where the functions for PrtSupLoanPickup() use them.
//           Richard Chambers, Jun-11-2025


/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "rfl.h"
/* #include <uie.h> */
#include "regstrct.h"
#include "transact.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "csttl.h"
#include "csop.h"
#include "report.h"
#include "pmg.h"
#include "prtrin.h"
#include "prtsin.h"

/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/


/*
*===========================================================================
** Format  : VOID  PrtSupForeignTender_TH(MAINTLOANPICKUP *pData);
*
*   Input  : MAINTLOANPICKUP      *pData     -loan/pick up data
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints foreign tender loan/pickup
*===========================================================================
*/
 VOID    PrtSupForeignTender_TH(CONST MAINTLOANPICKUP* pData)


/*
*===========================================================================
** Format  : VOID  PrtSupForeignTender_EJ(MAINTLOANPICKUP *pData);
*
*   Input  : MAINTLOANPICKUP      *pData     -loan/pick up data
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function prints foreign tender loan/pickup
*===========================================================================
*/
VOID PrtSupForeignTender_EJ(MAINTLOANPICKUP* pData)

#endif
/* --- End of Source File --- */