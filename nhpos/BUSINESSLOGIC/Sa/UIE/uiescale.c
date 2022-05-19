/*
*===========================================================================
*
*   Copyright (c) 1992-1998, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Scale Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieScale.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieReadScale() - read a weight from scale
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date      : Ver.     : Name       : Description
* Nov-26-92 : 01.00.01 : O.Nakada   : Initial
* Dec-08-92 : 01.00.03 : O.Nakada   : Modify power down recovery in
*                                     UieReadScale().
* Dec-15-92 : 01.00.05 : O.Nakada   : Add skip function.
* Jan-07-93 : 01.00.07 : O.Nakada   : Add power down recovery in skip function.
* Sep-16-93 : F1       : O.Nakada   : Divided the User Interface Engine.
* Mar-15-95 : G0       : O.Nakada   : Modified all modules, correspond to
*                                     hospitality release 3.0 functions.
* Jul-10-95 : G1       : O.Nakada   : Modified exception log and key track.
* May-27-97 : G10      : O.Nakada   : Supports NCR 7870, 7875 and 7880
*                                     scanner/scale using single cable.
* Sep-17-98 : G12      : O.Nakada   : Supports 6710 and 6720 scale.
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include "pif.h"
#include "log.h"
#include "fsc.h"
#include "uie.h"
#include "uiel.h"
#include "uielio.h"
#include "uieio.h"
#include "uietrack.h"
#include "BLFWif.h"


/*                                                                                
*===========================================================================
*   General Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieScaleInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieScaleInit(VOID)
{
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieReadScale(UIE_SCALE DLLARG *pData);
*    Output:    pData - address of scale data
*
** Return:      0 >= successful
*               0 <  failer
*===========================================================================
*/
SHORT DLLENTRY UieReadScale(UIE_SCALE DLLARG *pData)
{
	SHORT	sResult;
	BLFWIF	Weight;

	sResult = BlFwIfScaleReadWeight(&Weight);

	pData->uchUnit = Weight.uchUnit;
	pData->uchDec  = Weight.uchDec;
	pData->ulData  = Weight.ulData;

	return sResult;
}


/* ======================================= */
/* ========== End of UieScale.C ========== */
/* ======================================= */
