/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Vio Library
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsInit.c
* --------------------------------------------------------------------------
* Compiler    : VC++ Ver. 6.00  by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-07-99 : 1.00.00  : K.Iwata    : initial (for 2172)
*           :          :            :
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

#include <tchar.h>
#include "ecr.h"
#include "pif.h"
#include "vos.h"
#include "vosl.h"

#include "EvsL.h"


/*
*===========================================================================
** Synopsis:    VOID    EvsVioStrAttr(USHORT usRow, USHORT usColumn,
*                       UCHAR *puchString, USHORT usChars,
*                       USHORT usPage,VOID *pVosWin)
*　
*     Input:    
*               pVosWin   - VOS information
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioStrAttr(USHORT usRow, USHORT usColumn,UCHAR *puchString, USHORT usChars,
                               USHORT usPage,VOID *pVosWin){

    /* update screen */
    __vEvsUpdateData_VOSWINDOW(pVosWin);
    return;

    usRow = usRow;
    usColumn = usColumn;
    puchString = puchString;
    usChars = usChars;
    usPage = usPage;
}


/*
*===========================================================================
** Synopsis:    VOID    EvsVioCurOnOff(USHORT usControl);
*　
*     Input:    usControl - dummy
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioCurOnOff(USHORT usControl, VOID *pVosWin){

	if (pVosWin)
	{
	    __vEvsUpdateCursor_VOSWINDOW(pVosWin);
	}

    return;

    usControl = usControl;
}


/*
*===========================================================================
** Synopsis:    VOID    EvsVioCurType(UCHAR uchTop, UCHAR uchBottom);
*　
*     Input:    uchTop    - dummy
*               uchBottom - dummy
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioCurType(UCHAR uchTop, UCHAR uchBottom, VOID *pVosWin){

	if (pVosWin)
	{
	    __vEvsUpdateCursor_VOSWINDOW(pVosWin);
	}

    return;

    uchTop = uchTop;
    uchBottom = uchBottom;
}


/*
*===========================================================================
** Synopsis:    VOID    EvsVioSetCurPos(USHORT usRow, USHORT usColumn,USHORT usPage);
*　
*     Input:    
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioSetCurPos(USHORT usRow, USHORT usColumn,USHORT usPage, VOID *pVosWin){

	if (pVosWin)
	{
	    __vEvsUpdateCursor_VOSWINDOW(pVosWin);
	}

    return;

    usRow = usRow;
    usColumn = usColumn;
    usPage = usPage;
}

/*
*===========================================================================
** Synopsis:    VOID    EvsVioScrollUp(USHORT usTopRow, USHORT usLeftCol, 
*                           USHORT usBotRow,USHORT usRightCol, USHORT usNumRow, 
*                           UCHAR uchAttr);
*　
*     Input:    
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioScrollUp(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr){
    return;

    usTopRow = usTopRow;
    usLeftCol = usLeftCol;
    usBotRow = usBotRow;
    usRightCol = usRightCol;
    usNumRow = usNumRow;
    uchAttr = uchAttr;
}


/*
*===========================================================================
** Synopsis:    VOID    EvsVioScrollDown(USHORT usTopRow, USHORT usLeftCol, 
*                         USHORT usBotRow,USHORT usRightCol, USHORT usNumRow, 
*                         UCHAR uchAttr);
*　
*     Input:    
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioScrollDown(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr){
    return;

    usTopRow = usTopRow;
    usLeftCol = usLeftCol;
    usBotRow = usBotRow;
    usRightCol = usRightCol;
    usNumRow = usNumRow;
    uchAttr = uchAttr;
}


/*
*===========================================================================
** Synopsis:    VOID    EvsVioMode(USHORT usMode);
*　
*     Input:    usMode    - dummy
*
** Return:      nothing
*
*===========================================================================
*/
VOID    EvsVioMode(USHORT usMode){
    return;
    usMode = usMode;
}


/*
*===========================================================================
** Synopsis:    SHORT   EvsVioBackLight(USHORT usControl);
*　
*     Input:    usControl  - dummy
*
** Return:      0
*
*===========================================================================
*/
SHORT   EvsVioBackLight(USHORT usControl){
    return 0;
    usControl = usControl;
}



#ifdef XXXX
D:\V1_00_WinCE_EVS\EventSubSystem\VOSMISC.C 236:     PifVioStrAttr((USHORT)(pWin->PhyBase.usRow + usRow),
D:\V1_00_WinCE_EVS\EventSubSystem\VOSMISC.C 270:     PifVioStrAttr((USHORT)(pWin->PhyBase.usRow + usRow),

D:\V1_00_WinCE_EVS\EventSubSystem\vosinit.c 146:     PifVioCurOnOff(PIF_VIO_OFF);
D:\V1_00_WinCE_EVS\EventSubSystem\VOSMISC.C 232:     PifVioCurOnOff(PIF_VIO_OFF);
D:\V1_00_WinCE_EVS\EventSubSystem\VOSMISC.C 246:     PifVioCurOnOff(PIF_VIO_ON);
D:\V1_00_WinCE_EVS\EventSubSystem\VOSWDELE.C 142:    PifVioCurOnOff(PIF_VIO_OFF);        /* cursor off              */
D:\V1_00_WinCE_EVS\EventSubSystem\vosctype.c 136:    PifVioCurOnOff(PIF_VIO_ON);
D:\V1_00_WinCE_EVS\EventSubSystem\vosctype.c 143:    PifVioCurOnOff(PIF_VIO_OFF);

D:\V1_00_WinCE_EVS\EventSubSystem\vosinit.c 147:     PifVioCurType(1, 14);

D:\V1_00_WinCE_EVS\EventSubSystem\vosscur.c 146:     PifVioSetCurPos(usRow, usCol, usVosPage);
D:\V1_00_WinCE_EVS\EventSubSystem\vosinit.c 151:     PifVioSetCurPos(0, 0, usVosPage);

D:\V1_00_WinCE_EVS\EventSubSystem\vossup.c 162:         PifVioScrollUp((USHORT)(pWin->PhyBase.usRow + pWin->CharBase.usRow),

D:\V1_00_WinCE_EVS\EventSubSystem\vossdown.c 164:         PifVioScrollDown((USHORT)(pWin->PhyBase.usRow + pWin->CharBase.usRow),

D:\V1_00_WinCE_EVS\EventSubSystem\vosinit.c 127:         PifVioMode(PIF_VIO_TEXT4025);
D:\V1_00_WinCE_EVS\EventSubSystem\vosinit.c 134:         PifVioMode(PIF_VIO_TEXT8025);

D:\V1_00_WinCE_EVS\EventSubSystem\vosblig.c 118:     PifVioBackLight(PIF_VIO_ON);
D:\V1_00_WinCE_EVS\EventSubSystem\vosblig.c 122:     PifVioBackLight(PIF_VIO_OFF);

#endif
