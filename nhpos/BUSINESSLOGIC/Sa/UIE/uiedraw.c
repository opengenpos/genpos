/*
*===========================================================================
*
*   Copyright (c) 1992-1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Drawer Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieDraw.C
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieOpenDrawer() - open drawer
*                   UieDrawerComp() - update status of drawer compulsory
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*
* Date      : Ver.     : Name       : Description
* Mar-20-92 : 00.00.01 : O.Nakada   : Initial
* Jan-19-93 : 00.00.01 : O.Nakada   : Modify UieCheckDrawerStatus() function.
* May-25-93 : C1       : O.Nakada   : Removed input device control functions.
* Mar-15-95 : G0       : O.Nakada   : Modified all modules, correspond to
*                                   : hospitality release 3.0 functions.
* Sep-05-95 : G2       : O.Nakada   : Modified UieReadDrawer() function.
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Apr-18-00 :          : M.Teraki   : Modified UieOpenDrawer()
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
* Nov-09-15  GenPOS 2.2 R.Chambers  Removed unneeded call to PifReleaseSem(usUieSyncNormal)
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
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "pif.h"
#include "scf.h"
#include "uie.h"
#include "uiel.h"
#include "uieio.h"
#include "UIELIO.H"
#include "uietrack.h"
#include "BLFWif.h"
#include "paraequ.h"
#include "uiering.h" //JHHJ
#include "para.h"
#include "fsc.h"   
#include "csstbpar.h"

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
static  UCHAR   fchUieDrawer[UIE_NUM_DRAWER] = {0, 0};            /* status of drawer */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieOpenDrawer(USHORT usNo);
*     Input:    usNo - drawer number
*
** Return:      TRUE  - drawer opened
*               FALSE - open error
*===========================================================================
*/
SHORT  UieOpenDrawer(USHORT usNo)
{
    UCHAR fchOldData;

    /* --- at first, update drawer opened flag --- */
    fchOldData = fchUieDrawer[usNo - 1];
    fchUieDrawer[usNo - 1] |= (UIE_DRAWER_ENGINE | UIE_DRAWER_APPL);   /* engine opened, appl. opened  */

    if (BlFwIfDrawerOpen(usNo) != BLFWIF_DRAWER_OPENED)
    {
        /* --- if failed, clearset drawer opened flag --- */
        fchUieDrawer[usNo - 1] = fchOldData;
        return FALSE;
    }

    /* --- wait for complete drawer opening --- */

//    do {
//        PifSleep(UIE_DRAWER_TOPEN);         /* sleep current thread  */
//    } while (BlFwIfDrawerStatus(usNo) == BLFWIF_DRAWER_OPENING);

    return TRUE;
}


/*
*===========================================================================
** Synopsis:    SHORT DLLENTRY UieDrawerComp(BOOL fStatus);
*     Input:    fStatus - drawer comulsory flag
*
** Return:      nothing
*===========================================================================
*/
VOID  UieDrawerComp(BOOL fStatus)
{
    UieSetDrawerComp(0, fStatus);
    UieSetDrawerComp(1, fStatus);
}


/*
*===========================================================================
** Synopsis:    VOID DLLENTRY UieSetStatusDrawer(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID  UieSetStatusDrawer(VOID)
{
    fchUieDrawer[0] |= UIE_DRAWER_APPL;
    fchUieDrawer[1] |= UIE_DRAWER_APPL;
}


/*
*===========================================================================
** Synopsis:    VOID UieDrawerInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieDrawerInit(VOID)
{
	USHORT		i;

    /* --- initialization of drawer status --- */
	for(i = 0; i < 2; i++)
	{
        fchUieDrawer[i] |= UIE_DRAWER_APPL;      /* appl opened            */
        fchUieDrawer[i] &= ~UIE_DRAWER_COMP;     /* reset comulsory drawer */
        fchUieDrawer[i] &= ~UIE_DRAWER_ENGINE;
    }
}


/*
*===========================================================================
** Synopsis:    SHORT UieSetDrawerComp(USHORT usNo, BOOL fStatus);
*     Input:    usNo    - drawer number (0 or 1)
*               fStatus - drawer comulsory flag
*
** Return:      nothing
*===========================================================================
*/
VOID UieSetDrawerComp(USHORT usNo, BOOL fStatus)
{
    if (fStatus == UIE_ENABLE) {                /* drawer compulsory     */
        fchUieDrawer[usNo] |= UIE_DRAWER_COMP;  /* set compulsory flag   */
    } else {
        fchUieDrawer[usNo] &= ~UIE_DRAWER_COMP; /* reset compulsory flag */
    }
}


/*
*===========================================================================
** Synopsis:    VOID UieCheckDrawerStatus(VOID);
*
** Return:      nothing
*
** Description: As for this function, when the mode change happens,
*               Call will be done. And, if Drawer opens, UieDrawer() will
*               be caused.
*===========================================================================
*/
SHORT UieCheckDrawerStatus(USHORT usNum)
{
	if(fchUieDrawer[usNum -1] & UIE_DRAWER_ENGINE)
	{
		return BLFWIF_DRAWER_OPENED;
	}else
	{
		return BLFWIF_DRAWER_CLOSED;
	}
}


/*
*===========================================================================
** Synopsis:    SHORT UieReadDrawer(KEYMSG *pMsg);
*     InOut:    pMsg - address of input message
*
** Return:      UIE_SUCCESS - drawer closed
*               UIE_ERROR   - no message
*===========================================================================
*/
SHORT UieReadDrawer(KEYMSG *pMsg)
{
    USHORT  usNo;
	ULONG	ulStatus[2] = {0};

	BlFwIfDrawerStatus(ulStatus);

    for (usNo = 0; usNo < UIE_NUM_DRAWER; usNo++) {
        /* --- check mismatch drawer status --- */
        if (! (fchUieDrawer[usNo] & UIE_DRAWER_ENGINE) &&   /* closed for engine */
               fchUieDrawer[usNo] & UIE_DRAWER_APPL) {      /* opened for appl   */
            
            fchUieDrawer[usNo] &= ~UIE_DRAWER_APPL;         /* reset appl flag   */

            /* --- check device status --- */
            if (! (fchUieDevice & UIE_ENA_DRAWER)) {        /* disable drawer    */
                continue;                                   /* next ...          */
            }
            
            /* --- make message of drawer closed --- */
            memset(pMsg, 0, sizeof(KEYMSG));
            pMsg->uchMsg     = UIM_DRAWER;                  /* message ID        */
            pMsg->SEL.usFunc = usNo + 1;                    /* drawer no.        */
            
            UieTracking(UIE_TKEY_DRAWER, (UCHAR)(usNo + 1), 0);
            return (UIE_SUCCESS);
        }
    }

	PifSleep(20);
    return (UIE_ERROR);
}


/*
*===========================================================================
** Synopsis:    SHORT UieDrawerCompulsory(VOID);
*
** Return:      UIE_SUCCESS - compulsory drawer
*               UIE_ERROR   - not compulsory drawer
*===========================================================================
*/
SHORT UieDrawerCompulsory(FSCTBL *pFsc)
{
	/* compulsory drawer */
    if (((fchUieDrawer[0] & UIE_DRAWER_APPL) && (fchUieDrawer[0] & UIE_DRAWER_COMP)) ||
        ((fchUieDrawer[1] & UIE_DRAWER_APPL) && (fchUieDrawer[1] & UIE_DRAWER_COMP))) {
		
		// if we are in a control string then do not worry about the drawer state
		// at this time.  An error dialog will cause problems with the control string.
		if (UieMacro.fsControl & UIE_MACRO_RUNNING)
			return UIE_ERROR;

		//SR 901, allow a user to us the print on demand key,
		//even if the cash drawer is open, but only print on demand. JHHJ
		if(CliParaMDCCheckField(MDC_DRAWER2_ADR, MDC_PARAM_BIT_C) && (pFsc->uchMajor == FSC_PRT_DEMAND))
		{
			return UIE_ERROR;
		}
        return (UIE_SUCCESS);
    } else {
        return (UIE_ERROR);
    }
}


/*
*===========================================================================
** Synopsis:    LONG UiePutDrawerData(ULONG ulLength,
*                                         CONST USHORT *pusData,
*                                         VOID *pResult);
*
*     Input:    ulLength - length of data from pData
*               pusData  - data from external module
*
*    Output:    pResult - reserved
*
** Return:      reserved
*
** Description: Accept cash drawer status data and clear the ring buffer if the
*               drawer is in use and is open.  We clear the user interface ring buffer
*               in case the user has pressed other keys while waiting for the
*               Clear on Tender to execute.
*               This function is in a polling loop and each time it is called, the
*               cash drawer status is checked.  It is also called during the processing
*               of error messages such as Close Drawer error when the drawer
*               is open for so long the Close Cash Drawer time expires.
*===========================================================================
*/
LONG  UiePutDrawerData(ULONG ulLength, CONST USHORT *pusData, VOID *pResult)
{  
    if (ulLength != sizeof(USHORT) * 2)
    {
        return -1;
    }

    if (*(pusData + 0) == BLFWIF_DRAWER_CLOSED)
    {
        fchUieDrawer[0] &= ~UIE_DRAWER_ENGINE;          /* reset open flag     */
    }

    if (*(pusData + 1) == BLFWIF_DRAWER_CLOSED)
    {
        fchUieDrawer[1] &= ~UIE_DRAWER_ENGINE;          /* reset open flag     */
    }
    
	/* is there a drawer still open?   */
    if ((fchUieDrawer[0] & UIE_DRAWER_ENGINE) || (fchUieDrawer[1] & UIE_DRAWER_ENGINE))
    {
        /*This condition was necessary because when the drawer is opened for the Clear Display
		at Tender option, the Operator may still be able to press the keypad, but the keys pressed
		should not be remembered.  This is done due to the location of the function call to clear the display.
		There is no way to check key presses there, so the ring buffer is purged of the information here*/
		//SR156 Clear Display Option If the drawer is a Compulsory Drawer
		if(CliParaMDCCheckField(MDC_CLEAR_ON_TENDER, MDC_PARAM_BIT_C) && (CliParaMDCCheckField( MDC_DRAWER_ADR, MDC_PARAM_BIT_C ) == 0) )
		{
			/*This condition is added to check if there is a drawer at all,
			if there is no drawer, it skips this call, because there is no need to clear
			the ring buffer.*/
			if((PifSysConfig()->uchDrawer1 != DRAWER_NONE) || (PifSysConfig()->uchDrawer2 != DRAWER_NONE))
			{
				UieClearRingBuf();
			}
		}
		// eliminate call to PifReleaseSem(usUieSyncNormal) as it is not needed and incrementing semaphore unnecessarily.
		//     Richard Chambers Nov-09-2015
    }

    return -1;
}

/* ====================================== */
/* ========== End of UieDraw.C ========== */
/* ====================================== */
