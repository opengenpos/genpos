/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Display String (with Attribute) Function
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosStrA.C
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
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
#include <memory.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#define  __EventSubSystem
#include "vos.h"
#include "vosl.h"

#include "EvsL.h"
/*
*===========================================================================
*   General Declarations
*===========================================================================
*/

// The VosParamArray is used to hold the button information that is
// sent to Framework.  In other words, a message is sent to Framework
// with the address of a VosParamArray item and the VosParamArray item
// provides the additional information that Framework needs to create 
// a button.  This allows us to have multiple outstanding button requests
// queued up against Framework without worrying about overwriting request
// information.  That said, this array needs to be sized large enough so
// that if Framework is not keeping up, that new button requests do not
// overwrite outstanding button requests that are waiting to be processed
// by Framework.  Remember that the Framework thread priority is a touch
// lower than some other threads.  Best size seems to be around 80 but
// that is because the largest OEP window size is currently 64 buttons.
//
// Sep-15-2012  Richard Chambers
//    With new uses for OEP windows such as for Preauth Batch selection for Amtrak
//    the previous size of 80 is insufficient.  So we are modifing this to be
//    large enough for a single day's transactions.
//    Following are to help find this area in future:
//      MldDispOrderEntryPromptText() - following is limitation on max number OEP items
//      MldUpdatePopUp() - following is limitation on max number OEP items
static VOSPARAM VosParamArray[2000];
static int      indxVosParamFree = -1;

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/


/*
*===========================================================================
** Synopsis:    VOID VosStringButton(USHORT usHandle, UCHAR *puchString,
*                                  USHORT usLength, USHORT usRow);
*     Input:    usReal     - real handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               usRow      - row in the display window to place the button.
*
** Return:      nothing
*
** Description: This function informs the Framework that a button needs to be
*               created in a particular window with a particular text on the
*               button face.
*===========================================================================
*/
VOID VosStringButton(USHORT usReal, TCHAR *puchString, USHORT usLength, USHORT usRow)
{
    VOSWINDOW   *pWin;
	ULONG       wParam = 1;

    /* --- check window handle --- */
	if (indxVosParamFree == -1) {
		memset (VosParamArray, 0, sizeof(VosParamArray));
		indxVosParamFree = 0;
	}

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_4);
		return;
    }

    pWin   = &aVosWindow[usReal];           /* set window address */
    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_5);
		return;
    }

	indxVosParamFree++;
	if (indxVosParamFree >= sizeof(VosParamArray)/sizeof(VosParamArray[0])) {
		indxVosParamFree = 0;
	}

	_tcsncpy (VosParamArray[indxVosParamFree].tchVosString, puchString, 63);
	VosParamArray[indxVosParamFree].tchVosString[63] = 0;   // ensure end of string exists.
	VosParamArray[indxVosParamFree].usRow = usRow;
    __vEvsPutParam_VOSWINDOW (pWin, wParam, VosParamArray + indxVosParamFree);

}

/*
*===========================================================================
** Synopsis:    VOID VosExecStringAttrColumn(USHORT usHandle, UCHAR *puchString,
*                                      USHORT usLength, ULONG *plAttr);
*     Input:    usHandle   - real handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               plAttr    - character attribute
*
** Return:      nothing
*===========================================================================
*/
static VOID VosExecStringAttrColumn(USHORT usReal, TCHAR *puchString, USHORT usLength,
                       ULONG *plAttr)
{
	//usCRow, usCCol are Current Column and Current Row
	//usERow, usECol are End Column and End Row
    USHORT      usPos, usCRow, usCCol, usERow, usECol, usOff, usShow;
    TCHAR       uchChar;
    TCHAR       *puchBuf;
    VOSWINDOW   *pWin;
	USHORT		usDispScrollUp = 0, usDispSpace = 0, usDeletedSpace = 0;

    pWin     = &aVosWindow[usReal];

    puchBuf  = pWin->puchBuf;
    usCRow   = pWin->CharBase.usRow + pWin->Cursor.usRow;
    usCCol   = pWin->CharBase.usCol + pWin->Cursor.usCol;
    usERow   = pWin->CharBase.usRow + pWin->CharSize.usRow - 1;
    usECol   = pWin->CharBase.usCol + pWin->CharSize.usCol - 1;
    usShow   = 0;

	if (pWin->usState & VOS_STATE_WBUTTONS) {
		VosStringButton (usReal, puchString, usLength, pWin->Cursor.usRow);
		pWin->Cursor.usRow++;
		return;
	}

    /* --- check current cursor position --- */

    if (! (pWin->usState & VOS_STATE_CTYPE) &&  /* invisible cursor     */
        usCRow == usERow &&                 /* bottom side              */
		usCCol >  usECol &&
        *puchString != _T('\n') &&              /* not newline              */
        *puchString != _T('\r') &&              /* not carriage return      */
        *puchString != _T('\b')) {              /* not back space           */
        VosExecScrollUp(pWin, 1);           /* scroll up                */
        usCCol = pWin->CharBase.usCol;      /* cursor for left side     */;
    }

    /* --- format display buffer of VOS --- */

    for (usPos = 0; usPos < usLength; usPos++) {
        uchChar = *(puchString + usPos);

        /* --- check PifDisplayAttr() compatible mode --- */

        switch (uchChar) {
        case _T('\n'):                          /* newline                  */
            if (usShow) {                   /* show rest buffer         */
                VosDisplayRow(pWin, usCRow);/* display current row      */
                usShow = 0;                 /* reset show flag          */
				usDeletedSpace = 0;
            }
            usCRow++;                       /* cursor for next row      */
            usCCol = pWin->CharBase.usCol;  /* cursor for left side     */
            if (usCRow <= usERow) {         /* in box                   */
                break;                      /* next ...                 */
            }
            VosExecScrollUp(pWin, 1);       /* scroll up                */
            usCRow = usERow;                /* cursor for buttom side   */
            break;                          /* next ...                 */

        case _T('\r'):                          /* carriage return          */
            usCCol = pWin->CharBase.usCol;  /* cursor for left side     */
			usDeletedSpace = 0;
            break;                          /* next ...                 */

        case _T('\b'):                          /* back space               */
            if (usCCol != pWin->CharBase.usCol) {   /* not left side    */
                usCCol--;                   /* cursor for left          */
                break;                      /* next ...                 */
            }
            if (usCRow != pWin->CharBase.usRow) {    /* not top side    */
                usCRow--;                   /* cursor for 1 up          */
                usCCol = usECol;            /* cursor for right side    */
                break;                      /* next ...                 */
            }
            break;                          /* next ...                 */

        default:                            /* other character          */
            usOff = (usCRow * pWin->PhySize.usCol + usCCol) * 2;
			//if the length of the string is equal to the end column
			//of the text control and that last character is a space
			//then the line has already been formated to take into account
			//the double wide characters
			if((usLength == usECol + 1) && *(puchString + usECol) == 0x20){
				*(pWin->puchBuf + usOff    ) = uchChar;
				*(pWin->puchBuf + usOff + 1) = (UCHAR)*plAttr;
				plAttr++;
			//if the length of the string is greater than the width
			//of one row then the whole buffer is being sent(not just one line)
			//and there are two spaces in a row and we need to remove spaces
			//to reformat the buffer to account for double width character
			//then don't increment the Current Column and don't place
			//the current character(space) into the buffer for the screen
			}else if( usLength >= (usECol + 1) && (uchChar == 0x20 
					&& *(puchString + usPos + 1) == 0x20
					&& usDispSpace)){
				usDispSpace--;
				usDeletedSpace++;

				break;
			//otherwise(normal) no spaces need to be removed because
			//the double width characters are already accounted for or
			//there are no double width characters
			}else{
				*(pWin->puchBuf + usOff    ) = uchChar;
				*(pWin->puchBuf + usOff + 1) = (UCHAR)*plAttr;
				plAttr++;
			}


			if ((usCCol + usDeletedSpace) < usECol) {         /* not right side           */
                usCCol++;                   /* cursor for next column   */
                usShow = 1;                 /* set show flag            */

				//chinese characters in unicode range 4e00 to 9fa5
				if(uchChar >= 0x4E00 && uchChar <= 0x9FA5 ){
					usDispSpace++;
				}
                break;                      /* next ...                 */
            }

            VosDisplayRow(pWin, usCRow);    /* display current row      */
            usShow = 0;                     /* reset show flag          */

            if (usCRow != usERow) {         /* not bottom side          */
                usCRow++;                   /* cursor for next row      */
                usCCol = pWin->CharBase.usCol;  /* cursor for left side */
				usDeletedSpace = 0;
                break;                      /* next ...                 */
            }

            if (! (pWin->usState & VOS_STATE_CTYPE) &&  /* invisible cursor */
                usPos + 1 >= usLength) {    /* end of string            */
                usCCol++;                   /* set range error          */
                break;                      /* next ...                 */
            }

            VosExecScrollUp(pWin, 1);       /* scroll up                */
            usCCol = pWin->CharBase.usCol;  /* cursor for left side     */
			usDeletedSpace = 0;
            break;                          /* next ...                 */
        }
    }
	
	if (usShow) {                           /* show rest buffer         */
        VosDisplayRow(pWin, usCRow);        /* display current dow      */
    }
	    /* --- set cursor position --- */
	//the column position needs to be adjusted for the number of characters(spaces
	//that were deleted

	pWin->Cursor.usCol = usDeletedSpace + usCCol - pWin->CharBase.usCol;

    pWin->Cursor.usRow = usCRow - pWin->CharBase.usRow;
    VosExecCurPos(pWin);
}


/*
*===========================================================================
** Synopsis:    VOID VosStringAttr(USHORT usHandle, UCHAR *puchString,
*                                  USHORT usLength, UCHAR uchAttr);
*     Input:    usHandle   - user handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               uchAttr    - character attribute
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosStringAttr(USHORT usHandle, TCHAR *puchString, USHORT usLength,
                   UCHAR uchAttr)
{
    USHORT      usReal;
    VOSWINDOW   *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;                  /* real window handle */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_1);
		return;
    }

    pWin   = &aVosWindow[usReal];           /* set window address */
    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_2);
		return;
    }

    VosExecWindowActive(usReal, FALSE);
    VosExecStringAttr(usReal, puchString, usLength, uchAttr, FALSE);

    PifReleaseSem(usVosApiSem);
}

/*
*===========================================================================
** Synopsis:    VOID VosStringAttr(USHORT usHandle, UCHAR *puchString,
*                                  USHORT usLength, UCHAR uchAttr);
*     Input:    usHandle   - user handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               uchAttr    - character attribute
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosStringAttrColumn(USHORT usHandle, TCHAR *puchString, USHORT usLength,
					  ULONG *plAttr)
{
    USHORT      usReal;
    VOSWINDOW   *pWin;

    PifRequestSem(usVosApiSem);

    /* --- check window handle --- */
    usReal = usHandle - 1;                  /* real window handle */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_3);
		return;
    }

    pWin   = &aVosWindow[usReal];           /* set window address */
    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_4);
		return;
    }

    VosExecWindowActive(usReal, FALSE);
    VosExecStringAttrColumn(usReal, puchString, usLength, plAttr);

    PifReleaseSem(usVosApiSem);
}

/*
*===========================================================================
** Synopsis:    VOID VosStringLabel(USHORT usHandle, UCHAR *puchString,
*                                  USHORT usLength, USHORT usRow);
*     Input:    usReal     - real handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               usRow      - row in the display window to place the label.
*
** Return:      nothing
*
** Description: This function informs the Framework that a label needs to be
*               created in a particular window with a particular text on the
*               label face.
*               See CFrameworkWndText::OnEvsPutParam() in Framework.
*===========================================================================
*/
VOID VosStringLabel(USHORT usReal, TCHAR *puchString, USHORT usLength, USHORT usRow)
{
    VOSWINDOW   *pWin;
	ULONG       wParam = 4;

    /* --- check window handle --- */
	if (indxVosParamFree == -1) {
		memset (VosParamArray, 0, sizeof(VosParamArray));
		indxVosParamFree = 0;
	}

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_4);
		return;
    }

    pWin   = &aVosWindow[usReal];           /* set window address */
    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_5);
		return;
    }

	indxVosParamFree++;
	if (indxVosParamFree >= sizeof(VosParamArray)/sizeof(VosParamArray[0])) {
		indxVosParamFree = 0;
	}

	_tcsncpy (VosParamArray[indxVosParamFree].tchVosString, puchString, 63);
	VosParamArray[indxVosParamFree].tchVosString[63] = 0;   // ensure end of string exists.
	VosParamArray[indxVosParamFree].usRow = usRow;
    __vEvsPutParam_VOSWINDOW (pWin, wParam, VosParamArray + indxVosParamFree);

}


/*
*===========================================================================
** Synopsis:    VOID VosShowWindow(USHORT usReal);
*
*     Input:    usReal     - real handle of window
*
** Return:      nothing
*
** Description: This function informs the Framework that a window needs to
*               be shown using ShowWindow (SW_SHOW).
*===========================================================================
*/
__declspec( dllexport ) VOID VosShowWindow(USHORT usHandle)
{
    VOSWINDOW   *pWin;
	ULONG       wParam = 2;
	USHORT      usReal;

    PifRequestSem(usVosApiSem);

	if (indxVosParamFree == -1) {
		memset (VosParamArray, 0, sizeof(VosParamArray));
		indxVosParamFree = 0;
	}

    /* --- check window handle --- */
    usReal = usHandle - 1;                  /* real window handle */

    if (usReal >= VOS_NUM_WINDOW) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_6);
		return;
    }

    pWin   = &aVosWindow[usReal];           /* set window address */
    if (! (pWin->usState & VOS_STATE_USE)) {    /* free handle    */
		PifReleaseSem(usVosApiSem);
        PifAbort(MODULE_VOS_STRINGATTR, FAULT_INVALID_ENVIRON_7);
		return;
    }

    __vEvsPutParam_VOSWINDOW (pWin, wParam, 0);

    PifReleaseSem(usVosApiSem);
}


/*
*===========================================================================
** Synopsis:    VOID VosExecStringAttr(USHORT usHandle, UCHAR *puchString,
*                                      USHORT usLength, UCHAR uchAttr,
*                                      USHORT fsControl);
*     Input:    usHandle   - real handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               uchAttr    - character attribute
*
** Return:      nothing
*===========================================================================
*/
VOID VosExecStringAttr(USHORT usReal, TCHAR *puchString, USHORT usLength,
                       UCHAR uchAttr, USHORT fsControl)
{
    VOSWINDOW   *pWin;
	//usCRow, usCCol are Current Column and Current Row
	//usERow, usECol are End Column and End Row
    USHORT      usPos, usCRow, usCCol, usERow, usECol, usOff, usShow;
	USHORT		usDispScrollUp = 0, usDispSpace = 0, usDeletedSpace = 0;
    TCHAR       uchChar;
    TCHAR       *puchBuf;
	UCHAR		uchAttr2;

    pWin     = &aVosWindow[usReal];

    puchBuf  = pWin->puchBuf;
    usCRow   = pWin->CharBase.usRow + pWin->Cursor.usRow;
    usCCol   = pWin->CharBase.usCol + pWin->Cursor.usCol;
    usERow   = pWin->CharBase.usRow + pWin->CharSize.usRow - 1;
    usECol   = pWin->CharBase.usCol + pWin->CharSize.usCol - 1;
    usShow   = 0;
    uchAttr2 = uchAttr;

	if (pWin->usState & VOS_STATE_WBUTTONS) {
		VosStringButton (usReal, puchString, usLength, pWin->Cursor.usRow);
		pWin->Cursor.usRow++;
		return;
	}

    /* --- check current cursor position --- */

    if (! (pWin->usState & VOS_STATE_CTYPE) &&  /* invisible cursor     */
        usCRow == usERow &&                 /* bottom side              */
		usCCol >  usECol &&
        *puchString != _T('\n') &&              /* not newline              */
        *puchString != _T('\r') &&              /* not carriage return      */
        *puchString != _T('\b')) {              /* not back space           */
        VosExecScrollUp(pWin, 1);           /* scroll up                */
        usCCol = pWin->CharBase.usCol;      /* cursor for left side     */;
    }


    /* --- format display buffer of VOS --- */

    for (usPos = 0; usPos < usLength; usPos++) {
        uchChar = *(puchString + usPos);

        /* --- check PifDisplayAttr() compatible mode --- */

        if (fsControl) {                    /* use attribute in string  */
            usPos++;
            uchAttr2 = *(puchString + usPos) | uchAttr;
        }
        
        switch (uchChar) {
        case _T('\n'):                          /* newline                  */
            if (usShow) {                   /* show rest buffer         */
                VosDisplayRow(pWin, usCRow);/* display current row      */
                usShow = 0;                 /* reset show flag          */
				usDeletedSpace = 0;
            }
            usCRow++;                       /* cursor for next row      */
            usCCol = pWin->CharBase.usCol;  /* cursor for left side     */
            if (usCRow <= usERow) {         /* in box                   */
                break;                      /* next ...                 */
            }
            VosExecScrollUp(pWin, 1);       /* scroll up                */
            usCRow = usERow;                /* cursor for buttom side   */
            break;                          /* next ...                 */

        case _T('\r'):                          /* carriage return          */
            usCCol = pWin->CharBase.usCol;  /* cursor for left side     */
			usDeletedSpace = 0;
            break;                          /* next ...                 */

        case _T('\b'):                          /* back space               */
            if (usCCol != pWin->CharBase.usCol) {   /* not left side    */
                usCCol--;                   /* cursor for left          */
                break;                      /* next ...                 */
            }
            if (usCRow != pWin->CharBase.usRow) {    /* not top side    */
                usCRow--;                   /* cursor for 1 up          */
                usCCol = usECol;            /* cursor for right side    */
                break;                      /* next ...                 */
            }
            break;                          /* next ...                 */

        default:                            /* other character          */
            usOff = (usCRow * pWin->PhySize.usCol + usCCol) * 2;
			//if the length of the string is equal to the end column
			//of the text control and that last character is a space
			//then the line has already been formated to take into account
			//the double wide characters
			if((usLength == usECol + 1) && *(puchString + usECol) == 0x20){
				*(pWin->puchBuf + usOff    ) = uchChar;
				*(pWin->puchBuf + usOff + 1) = uchAttr2;
			//if the length of the string is greater than the width
			//of one row then the whole buffer is being sent(not just one line)
			//and there are two spaces in a row and we need to remove spaces
			//to reformat the buffer to account for double width character
			//then don't increment the Current Column and don't place
			//the current character(space) into the buffer for the screen
			}else if( usLength >= (usECol + 1) && (uchChar == 0x20 
					&& *(puchString + usPos + 1) == 0x20
					&& usDispSpace)){
				usDispSpace--;
				usDeletedSpace++;

				break;
			//otherwise(normal) no spaces need to be removed because
			//the double width characters are already accounted for or
			//there are no double width characters
			}else {
				*(pWin->puchBuf + usOff    ) = uchChar;
				*(pWin->puchBuf + usOff + 1) = uchAttr2;
			}

			if ((usCCol + usDeletedSpace) < usECol) {         /* not right side           */
                usCCol++;                   /* cursor for next column   */
                usShow = 1;                 /* set show flag            */

				//chinese characters in unicode range 4e00 to 9fa5
				if(uchChar >= 0x4E00 && uchChar <= 0x9FA5 ){
					usDispSpace++;
				}
                break;                      /* next ...                 */
            }

            VosDisplayRow(pWin, usCRow);    /* display current row      */
            usShow = 0;                     /* reset show flag          */

            if (usCRow != usERow) {         /* not bottom side          */
                usCRow++;                   /* cursor for next row      */
                usCCol = pWin->CharBase.usCol;  /* cursor for left side */
				usDeletedSpace = 0;
                break;                      /* next ...                 */
            }

            if (! (pWin->usState & VOS_STATE_CTYPE) &&  /* invisible cursor */
                usPos + 1 >= usLength) {    /* end of string            */
                usCCol++;                   /* set range error          */
                break;                      /* next ...                 */
            }

            VosExecScrollUp(pWin, 1);       /* scroll up                */
            usCCol = pWin->CharBase.usCol;  /* cursor for left side     */
			usDeletedSpace = 0;
            break;                          /* next ...                 */
        }
    }
	
	if (usShow) {                           /* show rest buffer         */
        VosDisplayRow(pWin, usCRow);        /* display current dow      */
    }
	    /* --- set cursor position --- */
	//the column position needs to be adjusted for the number of characters(spaces
	//that were deleted

	pWin->Cursor.usCol = usDeletedSpace + usCCol - pWin->CharBase.usCol;

    pWin->Cursor.usRow = usCRow - pWin->CharBase.usRow;
    VosExecCurPos(pWin);
}

/*
*===========================================================================
** Synopsis:    VOID VosStringAttr(USHORT usHandle, UCHAR *puchString,
*                                  USHORT usLength, UCHAR uchAttr);
*     Input:    usHandle   - user handle of window
*               puchString - address of string buffer
*               usLength   - length of string buffer
*               uchAttr    - character attribute
*
** Return:      nothing
*===========================================================================
*/
__declspec( dllexport ) VOID VosStringEdit(USHORT usHandle, TCHAR *puchString, USHORT usLength, USHORT usType)
{
    VOSWINDOW   *pWin;
    TCHAR       *puchBuf;
	USHORT      usCRow, usCCol, usERow, usECol, usShow, usOff;
	USHORT		usDispScrollUp = 0, usDispSpace = 0, usDeletedSpace = 0;
	USHORT		usReal;
	
	/* --- check window handle --- */
	usReal = usHandle - 1;                  /* real window handle */
    pWin   = &aVosWindow[usReal];           /* set window address */
	
    PifRequestSem(usVosApiSem);

	puchBuf  = pWin->puchBuf;
    usCRow   = pWin->CharBase.usRow + pWin->Cursor.usRow;
    usCCol   = pWin->CharBase.usCol + pWin->Cursor.usCol;
    usERow   = pWin->CharBase.usRow + pWin->CharSize.usRow - 1;
    usECol   = pWin->CharBase.usCol + pWin->CharSize.usCol - 1;
    usShow   = 0;

    usOff = (usCRow * pWin->PhySize.usCol + usCCol) * 2;

	// if handle range error or handle is free then error
    if (usReal >= VOS_NUM_WINDOW || ! (pWin->usState & VOS_STATE_USE)) {
		PifReleaseSem(usVosApiSem);
        PifAbort (MODULE_VOS_STRINGATTR, FAULT_INVALID_HANDLE);
    }

    VosExecWindowActive (usReal, FALSE);

	switch(usType)
	{
		case 1:
			memcpy (puchString, (pWin->puchBuf + usOff), usLength);
			break;

		case 2:
			memcpy ((pWin->puchBuf + usOff), puchString, usLength);
			break;

		case 3:
			memcpy (puchString, pWin->puchBuf, usLength);
			break;

		case 4:
			memcpy (pWin->puchBuf, puchString, usLength);
			break;

		default:
			break;
	}
	
	VosDisplayRow(pWin, usCRow);
    PifReleaseSem(usVosApiSem);
}

/* ====================================== */
/* ========== End of VosStrA.C ========== */
/* ====================================== */


