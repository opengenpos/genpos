/* pifnop.c : Defines the entry point for the DLL application. */


#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "piflocal.h"

VOID   PifXinuInit(BOOL fOsType)
{
	return;

	fOsType = fOsType;
}

VOID   PifInitQueue(VOID)
{
	return;
}

VOID   PIFENTRY PifGetChar(CHARDATA FAR *pInputChar)
{
	return;

	pInputChar = pInputChar;
}

VOID   PIFENTRY PifControlKeyboard(USHORT usFunc)
{
	return;

	usFunc = usFunc;
}

SHORT  PIFENTRY PifControl(USHORT usFunc, VOID FAR *pArgList)
{
	return 0;

	usFunc = usFunc;
	pArgList = pArgList;
}

/* --- Viode I/O --- */
VOID    PIFENTRY PifVioStrAttr(USHORT usRow, USHORT usColumn,
                               CONST UCHAR FAR *puchString, USHORT usChars,
                               USHORT usPage)
{
	return;

	usRow = usRow;
	usColumn = usColumn;
	puchString = puchString;
	usChars = usChars;
	usPage = usPage;
}

VOID    PIFENTRY PifVioStr(USHORT usRow, USHORT usColumn,
                           CONST UCHAR FAR *puchString, USHORT usChars,
                           UCHAR  uchAttr, USHORT usPage)
{
	return;

	usRow = usRow;
	usColumn = usColumn;
	puchString = puchString;
	usChars = usChars;
	uchAttr = uchAttr;
	usPage = usPage;
}

VOID    PIFENTRY PifVioCurOnOff(USHORT usControl)
{
	return;

	usControl = usControl;
}

VOID    PIFENTRY PifVioCurType(UCHAR uchTop, UCHAR uchBottom)
{
	return;

	uchTop = uchTop;
	uchBottom = uchBottom;
}

VOID    PIFENTRY PifVioGetCurPos(USHORT FAR *pusRow, USHORT FAR *pusColumn,
                                 USHORT usPage)
{
	return;

	pusRow = pusRow;
	pusColumn = pusColumn;
	usPage = usPage;
}

VOID    PIFENTRY PifVioSetCurPos(USHORT usRow, USHORT usColumn,
                                 USHORT usPage)
{
	return;

	usRow = usRow;
	usColumn = usColumn;
	usPage = usPage;
}

VOID    PIFENTRY PifVioScrollUp(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr)
{
	return;

	usTopRow = usTopRow;
	usLeftCol = usLeftCol;
	usBotRow = usBotRow;
	usRightCol = usRightCol;
	usNumRow = usNumRow;
	uchAttr = uchAttr;
}

VOID    PIFENTRY PifVioScrollDown(USHORT usTopRow, USHORT usLeftCol, USHORT usBotRow,
                            USHORT usRightCol, USHORT usNumRow, UCHAR uchAttr)
{
	return;

	usTopRow = usTopRow;
	usLeftCol = usLeftCol;
	usBotRow = usBotRow;
	usRightCol = usRightCol;
	usNumRow = usNumRow;
	uchAttr = uchAttr;
}

VOID    PIFENTRY PifVioMode(USHORT usMode)
{
	return;

	usMode = usMode;
}

SHORT   PIFENTRY PifVioBackLight(USHORT usControl)
{
	return 0;

	usControl = usControl;
}

VOID    PIFENTRY PifVioActivePage(USHORT usPage)
{
	return;

	usPage = usPage;
}

VOID    PIFENTRY PifVioLoadCG(UCHAR FAR *puchFonts, UCHAR uchBytes)
{
	return;

	puchFonts = puchFonts;
	uchBytes = uchBytes;
}

