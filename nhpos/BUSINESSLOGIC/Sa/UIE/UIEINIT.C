/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1993       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Check Sequence for Initialize Module
* Category    : User Interface Enigne, NCR 2170 High Level System Application
* Program Name: UIEINIT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Meduim/Large Model
* Options     : /c /A[M|Lw] /W4 /G[1|2]s /Os /Zap
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UieInitialize() : Initialize Main Moudule
*                   UieInitWork() : Initialize of Work Area
*                   UieCreateSem() : Create Semaphore
*                   UieCreateThread() : Create Thread
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date       Ver.      Name         Description
* Sep-16-93  F1        O.Nakada     Divided the User Interface Engine.
* Mar-15-95  G0        O.Nakada     Removed the uieinit.h include file.
*
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
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include "ecr.h"
#include "uie.h"
#include "uiel.h"
#include "uieseq.h"
#include "uiedialo.h"

                                                                                
/*
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
extern  VOID    UieInitCheckSequence(VOID);


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieInitialize(CONST CVTTBL FAR *pCvt,
*                                           CONST FSCTBL FAR *pFsc,
*                                           UCHAR uchMax);
*     Input:    pCvt   - Conversion Table
*               pFsc   - FSC Table
*               uchMax - Max Mode Lock Position
*
**  Return:     nothing
*
** Description: Initialize to User Interface Engine.
*               - Work Area
*               - Semaphore
*               - Mode Key Position
*               - Deawer Status                             
*               - Input Device Thread
*===========================================================================
*/
VOID DLLENTRY UieInitialize(CONST CVTTBL FAR * FAR *pCvt,
                            CONST FSCTBL FAR * FAR *pFsc, UCHAR uchMax)
{
    UieInitCheckSequence();

    UieInitialize2(pCvt, pFsc, uchMax);
}


/*
*===========================================================================
** Synopsis:    VOID UieInitCheckSequence(VOID)
*
** Return:      nothing
*
** Deslription: Initialize User Interface Engine Work Area
*===========================================================================
*/
VOID UieInitCheckSequence(VOID)
{
	UieRetry.fchStatus = (UIE_ACCEPT |          /* Set to Accept Flag */
                          UIE_CLEAR_RETRY_BUF); /* Set to Clear Retry Buffer Flag */

    iuchUieRootFunc[UIE_NORMAL] = UIE_LINK_EMPTY;
}



/* ---------- End of UIEINIT.C ---------- */
