/*************************************************************************
 *
 * DeviceEngine.cpp
 *
 * $Workfile:: DeviceEngine.cpp                                          $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: DeviceEngine.cpp                                           $
 *
 * Dec-05-16: 02.02.02 : R.Chambers  : FRAMEWORK_IOCTL_SCALE_READWEIGHT now returns status of UIE_SCALE_NOT_PROVIDE if no opened.
************************************************************************/

#include "stdafx.h"
#include <tchar.h>
#include "Framework.h"
#include "SCF.h"
#include "OPOS.h"
#include "OPOSCash.h"
#include "OPOSDisp.h"
#include "OPOSPtr.h"
#include "OPOSCoin.h"
#include "BlFWif.h"
#include "uie.h"
#include "pmg.h"
#include "appllog.h"
#include "paraequ.h"
#include "para.h"
#include "ecr.h"
#include "evs.h"
#include "DeviceEngine.h"

#include "httpmain.h"

#include "DisplayBitMap.h"

// Following define is used to turn licence key file check on or off
// To turn off, comment out or change value from 1 to 0
//#define  DEVICE_LOADLICENSEKEYFILE_ON   1

// following includes and defines needed for WMI access for BIOS serial number
#define _WIN32_DCOM
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#include "MsxmlWrapper.h"
#include "Crypto.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern HINSTANCE  hResourceDll = 0;
USHORT printingLogoType;

//===========    CDeviceMultipleDeviceMap  =====================

unsigned short  CDeviceMultipleDeviceMap::m_usDeviceMapArray[3] = {0, 0x01, 0x02};

CDeviceMultipleDeviceMap::CDeviceMultipleDeviceMap (MultipleDeviceMapBits bit)
{
	SetDevice(bit);
}

CDeviceMultipleDeviceMap::~CDeviceMultipleDeviceMap ()
{
	m_usDeviceMap = 0;
}



//===========    CDeviceWinPrinter  =====================

USHORT CDeviceWinPrinter::usReceiptPrintCounter = 0;

CDeviceWinPrinter::CDeviceWinPrinter (TargetPrinterType  tpType)
{
	tpPrinterType = tpType;
	m_tpObjectStatus = PrintUnavailable;
	m_oldFont = 0;
	iYRow = 0;
	iXColumn = 0;
	bFilterEscSeq = true;

#if 0
	memset(&m_logfont, 0, sizeof(LOGFONT));       // zero out structure
	m_logfont.lfHeight = 12;                      // request a 12-pixel-height font
	strcpy(m_logfont.lfFaceName, "Arial");        // request a face name "Arial"
	m_cfont.CreateFontIndirect(&m_logfont);       // create the font
#endif
}

CDeviceWinPrinter::~CDeviceWinPrinter ()
{
	m_tpObjectStatus = PrintUnavailable;
	if (m_oldFont != 0) {
		SelectObject (m_oldFont);
		m_fontText.DeleteObject ();
		m_oldFont = 0;
	}
}

int CDeviceWinPrinter::StartPage()
{
	if (m_tpObjectStatus == PrintUnavailable)
		return 0;

	if (m_tpObjectStatus == PrintOpen) {
		EndPage();
	}
	//		return 1;

	m_tpObjectStatus = PrintOpen;
	iYRow = 0;
	iXColumn = 0;

	usReceiptPrintCounter++;  usReceiptPrintCounter &= 0x01ff;

	TCHAR  tcBuffer[64] = { 0 };
	_stprintf (tcBuffer, _T("Receipt %d"), usReceiptPrintCounter);

	char  aszErrorBuffer[128] = { 0 };

	DOCINFO docinfo = { 0 };

	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = tcBuffer;

	int iRet = StartDoc(&docinfo);

	if (iRet <= 0)
	{
		sprintf(aszErrorBuffer, "DeviceEngine.cpp: CDeviceWinPrinter::StartPage()  Windows Driver Error:: Failed StartDoc() %d", iRet);
		NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
	}

	iRet = CDC::StartPage();
	if (iRet <= 0)
	{
		sprintf(aszErrorBuffer, "DeviceEngine.cpp: CDeviceWinPrinter::StartPage()  Windows Driver Error:: Failed CDC::StartPage() %d", iRet);
		NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
	}

	return iRet;
}

int CDeviceWinPrinter::EndPage()
{
	if (m_tpObjectStatus == PrintUnavailable)
		return 0;

	int iRet = 0;

	m_tpObjectStatus = PrintClosed;
	iYRow = 0;
	iXColumn = 0;

	int iError = CDC::EndPage();

	char aszErrorBuffer[128] = { 0 };
	if (iError <= 0)
	{
		sprintf(aszErrorBuffer, "DeviceEngine.cpp: CDeviceWinPrinter::EndPage()  Windows Driver Error:: Failed CDC::EndPage() %d", iError);
		NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
	}
	iError = CDC::EndDoc();
	if (iError <= 0)
	{
		sprintf(aszErrorBuffer, "DeviceEngine.cpp: CDeviceWinPrinter::EndPage()  Windows Driver Error:: Failed CDC::EndDoc() %d", iError);
		NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
	}
	return iError;
}

int CDeviceWinPrinter::PrintLine (LPTSTR ptchBuffer, ULONG  ulLength)
{
	const LONG  lLineMax = 42;

	if (m_tpObjectStatus == PrintUnavailable)
		return 0;

	TCHAR PrintBuffer[256] = { 0 };

	if (ulLength < 1)
		return 1;
	if (ulLength >= sizeof(PrintBuffer) / sizeof(PrintBuffer[0]))
		ulLength = sizeof(PrintBuffer) / sizeof(PrintBuffer[0]) - 1;

	_tcsncpy_s(PrintBuffer, 255, ptchBuffer, ulLength);
	PrintBuffer[ulLength] = 0;

	if (m_tpObjectStatus == PrintClosed)
	{
		StartPage();
		iYRow = 0;
		iXColumn = 0;
	}

	TEXTMETRIC myTextMetric;
	int horRez;
	horRez = GetDeviceCaps(HORZRES);
	GetOutputTextMetrics(&myTextMetric);
	int i, j;

	// Now lets check to see if we are near the end of a page for the output device.
	// If we are at the end of a page, then we will start a new page.
	int  iDeviceVertHeight = GetDeviceCaps(VERTRES);

	if (iYRow + myTextMetric.tmHeight + 5 >= iDeviceVertHeight) {
		EndPage();
		StartPage();
		iYRow = 0;
		iXColumn = 0;
	}

	tpPrinterCode = PrintCode_SingleHighWide;
	iXColumn = 0;

	for (i = 0, j = 0; i < ulLength; )
	{
		if (PrintBuffer[i] == 255)
			PrintBuffer[i] = _T(' ');

		if (PrintBuffer[i] == 0x0D || PrintBuffer[i] == 0x0A) {
			// We have found the end of line sequence so go to next line.
			if (i - j > 0)
			{
				// Need to do the font changes here based on value of tpPrinterCode
				// before we actually do the TextOut.
				// When incrementing iXColumn, need to also consider whether
				// the tmAveCharWidth indicates the actual width of the characters.
				TextOut(iXColumn, iYRow, PrintBuffer + j, i - j);
			}
			i++;
			if (PrintBuffer[i] == 0x0A) {
				i++;
			}
			j = i;
			iYRow += (myTextMetric.tmHeight + 5);
			iXColumn = 0;
			tpPrinterCode = PrintCode_SingleHighWide;
			if (i >= ulLength)
				break;
		}
		else if (bFilterEscSeq && PrintBuffer[i] == 0x1b)
		{
			// We have found the beginning on an escape sequence.
			// Lets process the escape sequence emulating the OPOS command.
			if (i - j > 0)
			{
				// Need to do the font changes here based on value of tpPrinterCode
				// before we actually do the TextOut.
				// When incrementing iXColumn, need to also consider whether
				// the tmAveCharWidth indicates the actual width of the characters.
				TextOut(iXColumn, iYRow, PrintBuffer + j, i - j);
				CSize mySize = GetTextExtent(PrintBuffer + j, i - j);
				iXColumn += mySize.cx;
			}
			i++; j = i;
			if (PrintBuffer[i] == _T('|'))
			{
				// There are basically two different types of commands.
				// Some have digits preceeding the actual command and
				// others do not.
				i++;
				if (PrintBuffer[i] >= _T('0') && PrintBuffer[i] <= _T('9'))
				{
					j = i;    // remember where the digit string begins
					while (PrintBuffer[i] >= _T('0') && PrintBuffer[i] <= _T('9'))
					{
						i++;
					}
					if (PrintBuffer[i] == _T('P'))
					{
						// It is a page cut command.  EndPage () will do
						// a page cut for us.
						EndPage();
						iYRow = 0;
						iXColumn = 0;
						i++; j = i;
					}
					else if (PrintBuffer[i] == _T('B'))
					{
						// It is a print logo command (PRT_LOGO) and we will ignore this
						iYRow += (myTextMetric.tmHeight + 5);
						iXColumn = 0;
						i++; j = i;
					}
					else if (PrintBuffer[i] == _T('C'))
					{
						// 4 = handle double high and double wide characters
						// 2 = handle single high and double wide characters
						int linecnt = _ttoi(PrintBuffer + j);
						switch (linecnt) {
						case 4:
							tpPrinterCode = PrintCode_DoubleHighWide;
							break;
						case 2:
							tpPrinterCode = PrintCode_DoubleWide;
							break;
						default:
							tpPrinterCode = PrintCode_SingleHighWide;
							break;
						}
						i++; j = i;    // step to after the font change command ends
					}
					else if (PrintBuffer[i] == _T('l') && PrintBuffer[i + 1] == _T('F'))
					{
						// It is a line feed command and the number of lines to feed
						// is specified before the lF string.
						int linecnt = _ttoi(PrintBuffer + j);
						while (linecnt > 0)
						{
							TextOut(iXColumn, iYRow, _T(" "), 1);
							iYRow += (myTextMetric.tmHeight + 5);
							iXColumn = 0;
							linecnt--;
						}
						i += 2; j = i;  // step to after the line feed command ends
					}
					else if (PrintBuffer[i] == _T('f') && PrintBuffer[i + 1] == _T('P')) {
						// It is a paper cut command and the percentage of paper width to cut
						// is specified before the fP string.
						// EndPage () will do a page cut for us.
						EndPage();
						iYRow = 0;
						iXColumn = 0;
						tpPrinterCode = PrintCode_SingleHighWide;
						i += 2; j = i;  // step to after the line feed command ends
					}
				}
				else
				{
					TCHAR  tcPrintOption = PrintBuffer[i];
					i++;

					TCHAR  tcPrintCommand = PrintBuffer[i];
					if (tcPrintCommand == _T('B'))
					{
						// It is a print logo command (PRT_LOGO) and we will ignore this
						iYRow += (myTextMetric.tmHeight + 5);
						iXColumn = 0;
						i++; j = i;
					}
					else if (tcPrintCommand == _T('A'))
					{
						// It is an alignment command and tcPrintOption contains the option.
						//    r means right alignment
						//    l means left alignment
						//    c means centered alignment

						i++; j = i;

						// We need to figure out the line length of the remaining
						// text.  This is complicated by the fact that the remaining
						// unwritten characters may in fact be double wide characters
						// or there may be other escape sequences in the character buffer.
						// Therefor, we will process the rest of the line to calculate
						// the number of tmAveCharWidth characters remaining.  If we find
						// any double wide characters, they will count as two characters.
						int linelen = i;
						int charwidth = 1;
						int linecurlen = 0;
						int  iLineSizeTempIndex = 0;
						TCHAR  tchLineSizeTemp[64] = { 0 };

						while (linelen < ulLength)
						{
							if (PrintBuffer[linelen] == 0x0D || PrintBuffer[linelen] == 0x0A) {
								break;
							}

							if (PrintBuffer[linelen] == 0x1b)
							{
								if (PrintBuffer[linelen + 1] == _T('|'))
								{
									// if we find another alignment command then break.
									if (PrintBuffer[linelen + 3] == _T('A'))
										break;

									linelen += 2;
									if (PrintBuffer[linelen + 1] == _T('C'))
									{
										if (PrintBuffer[linelen] == _T('4'))
											charwidth = 1;   // tpPrinterCode = PrintCode_DoubleHighWide;
										else if (PrintBuffer[linelen] == _T('2'))
											charwidth = 1;  // tpPrinterCode = PrintCode_DoubleWide;
										else
											charwidth = 1;  // tpPrinterCode = PrintCode_SingleHighWide;
										linelen += 2;
									}
								}
								else {
									linelen += 3;
								}
							}
							else
							{
								tchLineSizeTemp[iLineSizeTempIndex] = PrintBuffer[linelen];
								iLineSizeTempIndex++;
								linecurlen += charwidth;
								linelen++;
							}
						}

						// We will now calculate the length of the remaining text for this line.
						// We do the scaling in order to make sure we are properly doing an
						// align right or a centering operation.
						//
						// WARNING:  This calculation works for a font size of 8 however other
						//           font sizes may require a different value for iScaleValue below.
						//           See CreatePointFont() function below when processing SCF_OPOS_NAME_WIN_PRINTER.
						//           For font size of 8, a value of 32 seems to work best.
						//           For font size of 9, a value of 30 seems to work best.
						const int iScaleValue = 30;
						tchLineSizeTemp[iLineSizeTempIndex] = 0;

						if (tcPrintOption == _T('r') && linecurlen > 0)
						{
							// int iLineLength = (42 * (myTextMetric.tmAveCharWidth * iScaleValue)) / 24;
							LONG lLineLength = lLineMax * myTextMetric.tmAveCharWidth;
							CSize mySize = GetTextExtent(tchLineSizeTemp, iLineSizeTempIndex);
							if (lLineLength - mySize.cx > 0) {
								iXColumn = lLineLength - mySize.cx;
							}
						}
						else if (tcPrintOption == _T('l'))
						{
							iXColumn = 0;
						}
						else if (tcPrintOption == _T('c'))
						{
							CSize mySize = GetTextExtent(tchLineSizeTemp, iLineSizeTempIndex);
							LONG lLineLength = lLineMax * myTextMetric.tmAveCharWidth;
							iXColumn = lLineLength / 2 - mySize.cx / 2;
						}
					}
				}
			}
			else
			{
				j = i; i++;
			}
		}
		else {
			i++;
		}
	}

	if (i - j > 0)
	{
		BOOL bRet = TextOut(iXColumn, iYRow, PrintBuffer + j, i - j);
		iYRow += (myTextMetric.tmHeight + 5);
		iXColumn = 0;
	}
	return 1;
}

//=======================================================


/////////////////////////////////////////////////////////////////////////////
// Local Definition
/////////////////////////////////////////////////////////////////////////////


// for Keyboard Interface

#define DE_TABLE_END        0x00
#define DE_NOT_CONVERTED    0x0000
#define DE_WEDGE_STREAM     0x0100

// parser macros for a value of lParam

#define KEY_REPEAT_COUNT(l)     (LOWORD(l) & 0x00FF)        // repeat counts
#define KEY_SCAN_CODE(l)        (HIWORD(l) & 0x00FF)        // scan code
#define IS_EXTENDED_KEY(l)      (HIWORD(l) & KF_EXTENDED)   // is extended key ?
#define IS_BREAK_KEY(l)         (HIWORD(l) & KF_UP)         // is break key ?

typedef struct _CONVERT_KEY              // Convert Key Code Table
{
    UCHAR   uchSrc;                         // Source Key Code
    UCHAR   uchShiftCntl;                   // Convert Code (if entered SHIFT&CONTROL)
    UCHAR   uchShift;                       // Convert Code (if entered SHIFT key)
    UCHAR   uchCntl;                        // Convert Code (if entered CONTROL key)
    UCHAR   uchAlt;                         // Convert Code (if entered ALT(MENU))
    UCHAR   uchNormal;                      // Convert Code
} CONVERT_KEY, *PCONVERT_KEY;

/*
	Table for converting a key press from a standard PC 102 key keyboard
	typically used with debugging environment.

	This table is setup in a matrix format duplicating the 7448 Micromotion
	key board that has a standard layout of 11 keys across and 16 keys down.

	The 16 rows down are divided into 4 regions each region has the
	same keys but the regions are differentiated by whether the
	Control key and/or the Shift key is also pressed.

	Keys are numbered in numerical order begining with key number 1 in
	the top left corner thrugh key number 165 in the bottom right corner.
		The regions from the top are:
		the Control + Shift region (rows 1 thru 4)
		the Shift region (rows 5 thru 8)
		the Control region (rows 9 thru 12)
		the normal key region (rows 13 thru 16)

	The key layout for each region using this keypress table looks like:
		1  2  3  4  5  6  7  8  9  0  -
		A  B  C  D  E  F  G  H  I  J  K
		L  M  N  O  P  Q  R  S  T  U  V
		W  X  Y  Z  [  ]  ;  ,  .  /  \

	Basically this means that key number 1 (first key in the
	first row) is mapped to the character '1' with both the Control
	and the Shift keys depressed.  We will see the character '1' come
	in with the Control and the Shift flags both set and this character
	will be translated to be key number 1 in the top left corner of the
	keyboard.

	If the letter 'L' is pressed with the Shift key depressed, then we
	will see the character 'L' with the Shift flag set and we will
	translate this to physical key number 67 (row 7, column 1).

*/
CONVERT_KEY aConvertKeyPCStd[] =
{
//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x31, 0x6C, 0x98, 0xC4, 0x00, 0xDE},         // '1'
    {0x32, 0x6D, 0x99, 0xC5, 0x00, 0xDF},         // '2'
    {0x33, 0x6E, 0x9A, 0xC6, 0x00, 0x52},         // '3'
    {0x34, 0x6F, 0x9B, 0xC7, 0x00, 0x53},         // '4'
    {0x35, 0x70, 0x9C, 0xC8, 0x00, 0x54},         // '5'
    {0x36, 0x71, 0x9D, 0xC9, 0x00, 0x37},         // '6'
    {0x37, 0x72, 0x9E, 0xCA, 0x00, 0x38},         // '7'
    {0x38, 0x73, 0x9F, 0xCB, 0x00, 0x39},         // '8'
    {0x39, 0x74, 0xA0, 0xCC, 0x00, 0x55},         // '9'
    {0x30, 0x75, 0xA1, 0xCD, 0x00, 0x56},         // '0'
    {0xBD, 0x76, 0xA2, 0xCE, 0x00, 0x57},         // '-'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x41, 0x77, 0xA3, 0xCF, 0x00, 0xE0},         // 'A'
    {0x42, 0x78, 0xA4, 0xD0, 0x00, 0xE1},         // 'B'
    {0x43, 0x79, 0xA5, 0xD1, 0x00, 0x58},         // 'C'
    {0x44, 0x7A, 0xA6, 0xD2, 0x00, 0x59},         // 'D'
    {0x45, 0x7B, 0xA7, 0xD3, 0x00, 0x5A},         // 'E'
    {0x46, 0x7C, 0xA8, 0xD4, 0x00, 0x34},         // 'F'
    {0x47, 0x7D, 0xA9, 0xD5, 0x00, 0x35},         // 'G'
    {0x48, 0x7E, 0xAA, 0xD6, 0x00, 0x36},         // 'H'
    {0x49, 0x7F, 0xAB, 0xD7, 0x00, 0x5B},         // 'I'
    {0x4A, 0x80, 0xAC, 0xD8, 0x00, 0x5C},         // 'J'
    {0x4B, 0x81, 0xAD, 0xD9, 0x00, 0x5D},         // 'K'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x4C, 0x82, 0xAE, 0xDA, 0x00, 0xE2},         // 'L'
    {0x4D, 0x83, 0xAF, 0xDB, 0x00, 0xE3},         // 'M'
    {0x4E, 0x84, 0xB0, 0x40, 0x00, 0x5E},         // 'N'
    {0x4F, 0x85, 0xB1, 0x41, 0x00, 0x5F},         // 'O'
    {0x50, 0x86, 0xB2, 0x42, 0x00, 0x60},         // 'P'
    {0x51, 0x87, 0xB3, 0x43, 0x00, 0x31},         // 'Q'
    {0x52, 0x88, 0xB4, 0x44, 0x00, 0x32},         // 'R'
    {0x53, 0x89, 0xB5, 0x45, 0x00, 0x33},         // 'S'
    {0x54, 0x8A, 0xB6, 0x46, 0x00, 0x61},         // 'T'
    {0x55, 0x8B, 0xB7, 0x47, 0x00, 0x62},         // 'U'  101Key
    {0x56, 0x8C, 0xB8, 0x48, 0x00, 0x63},         // 'V'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x57, 0x8D, 0xB9, 0xDC, 0x00, 0xE4},         // 'W'
    {0x58, 0x8E, 0xBA, 0xDD, 0x00, 0xE5},         // 'X'
    {0x59, 0x8F, 0xBB, 0x49, 0x00, 0x64},         // 'Y'
    {0x5A, 0x90, 0xBC, 0x4A, 0x00, 0x65},         // 'Z'
    {0xDB, 0x91, 0xBD, 0x4B, 0x00, 0x66},         // '['
    {0xDD, 0x92, 0xBE, 0x4C, 0x00, 0x30},         // ']'
    {0xBA, 0x93, 0xBF, 0x4D, 0x00, 0x67},         // ';'  101Key
    {0xBB, 0x93, 0xBF, 0x4D, 0x00, 0x67},         // ';'  106Key
    {0xBC, 0x94, 0xC0, 0x4E, 0x00, 0x68},         // ','  COMMA
    {0xBE, 0x95, 0xC1, 0x4F, 0x00, 0x69},         // '.'  PERIOD
    {0xBF, 0x96, 0xC2, 0x50, 0x00, 0x6A},         // '/'
    {0xDC, 0x97, 0xC3, 0x51, 0x00, 0x6B},         // '\'  101Key
    {0xE2, 0x97, 0xC3, 0x51, 0x00, 0x6B},         // '\'  106Key

    // Special Asign
	//    Phys - S&C -  S  -  C  - Menu - Norm
    {VK_INSERT,  0x30, 0x30, 0x30, 0x00, 0x30},
    {VK_END,     0x31, 0x31, 0x31, 0x00, 0x31},
    {VK_DOWN,    0x32, 0x32, 0x32, 0x00, 0x32},
    {VK_NEXT,    0x33, 0x33, 0x33, 0x00, 0x33},
    {VK_LEFT,    0x34, 0x34, 0x34, 0x00, 0x34},
    {VK_CLEAR,   0x35, 0x35, 0x35, 0x00, 0x35},
    {VK_RIGHT,   0x36, 0x36, 0x36, 0x00, 0x36},
    {VK_HOME,    0x37, 0x37, 0x37, 0x00, 0x37},
    {VK_UP,      0x38, 0x38, 0x38, 0x00, 0x38},
    {VK_PRIOR,   0x39, 0x39, 0x39, 0x00, 0x39},
    {VK_RETURN,  0x69, 0x69, 0x69, 0x00, 0x69},   // 'Enter' -> 'P1'
    {VK_ADD,     0x66, 0x66, 0x66, 0x00, 0x66},   // '+'     -> 'P2'
    {VK_SUBTRACT,0x54, 0x54, 0x54, 0x00, 0x54},   // '-'     -> 'P3'
    {VK_MULTIPLY,0x55, 0x55, 0x55, 0x00, 0x55},   // '*'     -> 'P4'
    {VK_DIVIDE,  0x68, 0x68, 0x68, 0x00, 0x68},   // '/'     -> 'P5'
    {VK_DELETE,  0x6B, 0x6B, 0x6B, 0x00, 0x6B},   // 'Del'   -> 'A/C'
    {VK_ESCAPE,  0x43, 0x43, 0x43, 0x00, 0x43},   // 'Esc'   -> 'Clear'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {VK_F1, 0x03, 0x03, 0x03, 0x00, 0x03},        // LOCK MODE
    {VK_F2, 0x04, 0x04, 0x04, 0x00, 0x04},        // REG MODE
    {VK_F3, 0x05, 0x05, 0x05, 0x00, 0x05},        // SUPER MODE
    {VK_F4, 0x06, 0x06, 0x06, 0x00, 0x06},        // PROG MODE

    {DE_TABLE_END, 0x00, 0x00, 0x00, 0x00, 0x00}
};

CONVERT_KEY aConvertKeyBigTicket[] =
{
	// Special key strokes are identified by 0xFn where n = 0 to F.
	// the least significant nibble, n, is used as an index into a
	// special function table to handle that keystroke.

	// First special key stroke is for the Big Ticket key lock

    {0x7C, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0},         // '/'  Keyboard mounted key lock
//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x31, 0x6C, 0x98, 0xC4, 0x00, 0x31},         // '1'
    {0x32, 0x6D, 0x99, 0xC5, 0x00, 0x32},         // '2'
    {0x33, 0x6E, 0x9A, 0xC6, 0x00, 0x33},         // '3'
    {0x34, 0x6F, 0x9B, 0xC7, 0x00, 0x34},         // '4'
    {0x35, 0x70, 0x9C, 0xC8, 0x00, 0x35},         // '5'
    {0x36, 0x71, 0x9D, 0xC9, 0x00, 0x36},         // '6'
    {0x37, 0x72, 0x9E, 0xCA, 0x00, 0x37},         // '7'
    {0x38, 0x73, 0x9F, 0xCB, 0x00, 0x38},         // '8'
    {0x39, 0x74, 0xA0, 0xCC, 0x00, 0x39},         // '9'
    {0x30, 0x75, 0xA1, 0xCD, 0x00, 0x30},         // '0'
    {0xBD, 0x76, 0xA2, 0xCE, 0x00, 0x57},         // '-'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x41, 0x77, 0xA3, 0xCF, 0x00, 0x41},         // 'A'
    {0x42, 0x78, 0xA4, 0xD0, 0x00, 0x42},         // 'B'
    {0x43, 0x79, 0xA5, 0xD1, 0x00, 0x43},         // 'C'
    {0x44, 0x7A, 0xA6, 0xD2, 0x00, 0x44},         // 'D'
    {0x45, 0x7B, 0xA7, 0xD3, 0x00, 0x45},         // 'E'
    {0x46, 0x7C, 0xA8, 0xD4, 0x00, 0x46},         // 'F'
    {0x47, 0x7D, 0xA9, 0xD5, 0x00, 0x47},         // 'G'
    {0x48, 0x7E, 0xAA, 0xD6, 0x00, 0x48},         // 'H'
    {0x49, 0x7F, 0xAB, 0xD7, 0x00, 0x49},         // 'I'
    {0x4A, 0x80, 0xAC, 0xD8, 0x00, 0x4A},         // 'J'
    {0x4B, 0x81, 0xAD, 0xD9, 0x00, 0x4B},         // 'K'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x4C, 0x82, 0xAE, 0xDA, 0x00, 0x4C},         // 'L'
    {0x4D, 0x83, 0xAF, 0xDB, 0x00, 0x4D},         // 'M'
    {0x4E, 0x84, 0xB0, 0x40, 0x00, 0x4E},         // 'N'
    {0x4F, 0x85, 0xB1, 0x41, 0x00, 0x4F},         // 'O'
    {0x50, 0x86, 0xB2, 0x42, 0x00, 0x50},         // 'P'
    {0x51, 0x87, 0xB3, 0x43, 0x00, 0x51},         // 'Q'
    {0x52, 0x88, 0xB4, 0x44, 0x00, 0x52},         // 'R'
    {0x53, 0x89, 0xB5, 0x45, 0x00, 0x53},         // 'S'
    {0x54, 0x8A, 0xB6, 0x46, 0x00, 0x54},         // 'T'
    {0x55, 0x8B, 0xB7, 0x47, 0x00, 0x55},         // 'U'  101Key
    {0x56, 0x8C, 0xB8, 0x48, 0x00, 0x56},         // 'V'

//  Phys - S&C -  S  -  C  - Menu - Norm
    {0x57, 0x8D, 0xB9, 0xDC, 0x00, 0x57},         // 'W'
    {0x58, 0x8E, 0xBA, 0xDD, 0x00, 0x58},         // 'X'
    {0x59, 0x8F, 0xBB, 0x49, 0x00, 0x59},         // 'Y'
    {0x5A, 0x90, 0xBC, 0x4A, 0x00, 0x5A},         // 'Z'
    {0xDB, 0x91, 0xBD, 0x4B, 0x00, 0x5B},         // '['
    {0xDD, 0x92, 0xBE, 0x4C, 0x00, 0x5D},         // ']'
    {0xBA, 0x93, 0xBF, 0x4D, 0x00, 0xBA},         // ';'  101Key
    {0xBB, 0x93, 0xBF, 0x4D, 0x00, 0xBB},         // ';'  106Key
    {0xBC, 0x94, 0xC0, 0x4E, 0x00, 0xBC},         // ','  COMMA
    {0xBE, 0x95, 0xC1, 0x4F, 0x00, 0xBE},         // '.'  PERIOD
    {0xBF, 0x96, 0xC2, 0x50, 0x00, 0xBF},         // '/'
    {0xDC, 0x97, 0xC3, 0x51, 0x00, 0xDC},         // '\'  101Key
    {0xE2, 0x97, 0xC3, 0x51, 0x00, 0xE2},         // '\'  106Key

    // Special Asign
	//    Phys - S&C -  S  -  C  - Menu - Norm
	{VK_ESCAPE,  0x43, 0x43, 0x43, 0x00, 0x74},
    {VK_INSERT,  0x30, 0x30, 0x30, 0x00, 0x75},
    {VK_END,     0x31, 0x31, 0x31, 0x00, 0x76},
    {VK_DOWN,    0x32, 0x32, 0x32, 0x00, 0x77},
    {VK_NEXT,    0x33, 0x33, 0x33, 0x00, 0x78},
    {VK_LEFT,    0x34, 0x34, 0x34, 0x00, 0x79},
    {VK_CLEAR,   0x35, 0x35, 0x35, 0x00, 0x7A},
    {VK_RIGHT,   0x36, 0x36, 0x36, 0x00, 0x7B},
    {VK_HOME,    0x37, 0x37, 0x37, 0x00, 0x7C},
    {VK_UP,      0x38, 0x38, 0x38, 0x00, 0x7D},
    {VK_PRIOR,   0x39, 0x39, 0x39, 0x00, 0x7E},
    {VK_DIVIDE,  0x68, 0x68, 0x68, 0x00, 0x81},
    {VK_DELETE,  0x6B, 0x6B, 0x6B, 0x00, 0x82},


	//68 Key Portion, these keys can be assigned, however the above keys cannot.

	//  Row 1 keys 1 thru 4
    //           SHIFT CTRL  MENU  NONE

    {VK_F10, 0x00, 0x5B, 0x5B, 0x00, 0x72},         // Ctrl & F10
    {VK_F1, 0x00, 0x61, 0x5C, 0x00, 0x5F},         // Ctrl & F1
    {VK_F2, 0x00, 0x62, 0x5D, 0x00, 0x60},         // Ctrl & F2
    {VK_F3, 0x00, 0x00, 0x5E, 0x00, 0x65},         // Ctrl & F3

	//  Row 2 keys 5 thru 10
    //           SHIFT CTRL  MENU  NONE
    {0x70, 0x00, 0x61, 0x5C, 0x00, 0x5F},         // F1
    {0x54, 0x00, 0x62, 0x5D, 0x00, 0x60},         // F2
    {VK_F1, 0x00, 0x61, 0x5C, 0x00, 0x5F},         // Shift & F1
    {VK_MULTIPLY,0x55, 0x55, 0x55, 0x00, 0x62},	  // *
	{VK_SUBTRACT,0x54, 0x54, 0x54, 0x00, 0x63},	  // -
    {0x73, 0x00, 0x64, 0x00, 0x00, 0x66},         // Shift & F4

	//  Row 3 keys 11 thru 17
    //           SHIFT CTRL  MENU  NONE
    {VK_F3, 0x00, 0x00, 0x5E, 0x00, 0x65},         // F3
    {0x73, 0x00, 0x64, 0x00, 0x00, 0x66},         // F4
    {0x67, 0x00, 0x37, 0x00, 0x00, 0x37},         // '7'  - Fixed key
    {0x68, 0x00, 0x38, 0x00, 0x00, 0x38},         // '8'  - Fixed key
    {0x69, 0x00, 0x39, 0x00, 0x00, 0x39},         // '9'  - Fixed key
    {0x54, 0x00, 0x62, 0x5D, 0x00, 0x60},         // Shift & F2
    {0x74, 0x00, 0x68, 0x00, 0x00, 0x69},         // Shift & F5

	//  Row 4 keys 18 thru 24
    //           SHIFT CTRL  MENU  NONE
    {0x74, 0x00, 0x68, 0x00, 0x00, 0x69},         // F5
    {0x75, 0x00, 0x6C, 0x00, 0x00, 0x6A},         // F6
    {VK_NUMPAD4, 0x00, 0x00, 0x00, 0x00, 0x34},   // '4'  - Fixed key
    {VK_NUMPAD5, 0x00, 0x00, 0x00, 0x00, 0x35},   // '5'  - Fixed key
    {VK_NUMPAD6, 0x00, 0x00, 0x00, 0x00, 0x36},   // '6'  - Fixed key
    {0x6B, 0x00, 0x00, 0x00, 0x00, 0x6B},         //  +
    {0x75, 0x00, 0x6C, 0x00, 0x00, 0x6A},         // Shift & F6

	//  Row 5 keys 27 thru 31
    //           SHIFT CTRL  MENU  NONE
    {0x76, 0x00, 0x70, 0x00, 0x00, 0x6D},         // F7
    {0x77, 0x00, 0x73, 0x00, 0x00, 0x6E},         // F8
    {VK_NUMPAD1, 0x00, 0x00, 0x00, 0x00, 0x31},   // '1'  - Fixed key
    {VK_NUMPAD2, 0x00, 0x00, 0x00, 0x00, 0x32},   // '2'  - Fixed key
    {VK_NUMPAD3, 0x00, 0x00, 0x00, 0x00, 0x33},   // '3'  - Fixed key
    {0x0D, 0x00, 0x00, 0x00, 0x00, 0x6F},         // Return
    {0x76, 0x00, 0x70, 0x00, 0x00, 0x6D},         // Shift & F7

	//  Row 6 keys 32 thru 37
    //           SHIFT CTRL  MENU  NONE
    {0x78, 0x75, 0x00, 0x00, 0x00, 0x71},         // F9
    {0x79, 0x00, 0x5B, 0x5B, 0x00, 0x72},         // F10
    {VK_NUMPAD0, 0x00, 0x00, 0x00, 0x00, 0x30},   // '0'
    {VK_NUMPAD0, 0x00, 0x00, 0x00, 0x00, 0x30},   // '0'
    {0x6E, 0x00, 0x00, 0x00, 0x00, 0x75},         // '.'
    {0x0D, 0x00, 0x00, 0x00, 0x00, 0x6F},         // Return
    {0x77, 0x00, 0x73, 0x00, 0x00, 0x6E},         // Shift & F8

    {DE_TABLE_END, 0x00, 0x00, 0x00, 0x00, 0x00}
};
#if 0
// Removed by Richard Chambers as appears to no longer be needed
// Was used as part of Win CE emulator testing.  Number pad entries
// were turned into a numeric index into this table to provide a
// specific keypress.
UCHAR auchKeyNumber[] =
{
    0x00,
    // Key number: 1 to 176
    0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
    0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2,
    0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD,
    0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8,
    0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3,
    0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE,
    0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9,
    0xDA, 0xDB, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0xDC, 0xDD, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51,
    0xDE, 0xDF, 0x52, 0x53, 0x54, 0x37, 0x38, 0x39, 0x55, 0x56, 0x57,
    0xE0, 0xE1, 0x58, 0x59, 0x5A, 0x34, 0x35, 0x36, 0x5B, 0x5C, 0x5D,
    0xE2, 0xE3, 0x5E, 0x5F, 0x60, 0x31, 0x32, 0x33, 0x61, 0x62, 0x63,
    0xE4, 0xE5, 0x64, 0x65, 0x66, 0x30, 0x67, 0x68, 0x69, 0x6A, 0x6B,
    // Key Lock: 177 to 180
    0x03, 0x04, 0x05, 0x06
};

#endif


/*
	Keypress conversion table for the NCR Wedge 64 key keyboard.

	The source key codes are received from the keyboard via a MSG Windows
	message.  The wParam member contains the source key code.

	The source key code is translated to a UIE conversion table offset by finding
	the source key code in the first column of the table and then selecting the
	appropriate column of the CONVERT_KEY table depending on the key state.

	The UIE conversion table offset value is returned by the function ConvertKey ()
	below which performs the table lookup and source key to UIE conversion table
	offset translation.

	The UIE conversion table offset is used within the UIE subsystem, by function
	UieConvertFsc (), to then convert the key press into either a key press, type
	UIE_AN_KEY, or a function code, type UIE_FUNC_KEY.


	The MSG Windows message contains the following things of interest:
		message which contains WM_KEYDOWN
		wParam which contains the virtual key code
		lParam which contains the key data
			bits  0 - 15 -> repeat count for this message
			bits 16 - 23 -> scan code sent by the keyboard
			bit 24       -> if set is extended key from 101 or 102 key keyboard
			bit 30       -> previous key state

	The keys are in the table beginning with key 1 in the top left corner,
	going across the keyboard row of keys to key 8 in the top right corner.
	Then the next row is specified.

	See the user guide for NCR 5932 Wedge Keyboards, BD20-1369-A

	The keys F11 through F20 are seen as shift plus F1 through F10.
	The keys F21 through F23 are seen as control plus F1 through F3.

	Since this table is for an NCR 5932 Wedge 64 key keyboard, the range
	of values for the body of the table is from 0x40 through 0x79 because
	the max number of keys is 64 and 10 of those are fixed keys used
	for the numeric pad.

	This table must agree with the tables provided with Uifreg, Uifprog, and Uifsup
	in that the offset provided in the body of the table must correspond to the
	correct position in the CVTTBL such as CvtReg5932_68 [].
*/
CONVERT_KEY aConvertKeyWedge68[] =
{
	// Special key strokes are identified by 0xFn where n = 0 to F.
	// the least significant nibble, n, is used as an index into a
	// special function table to handle that keystroke.

	// First special key stroke is for the Wedge 68 key key lock
    {0x7C, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0},         // '/'  Keyboard mounted key lock

	//  Row 1 keys 1 thru 8
    //      S&C SHIFT CTRL  MENU  NONE
    {0xBF, 0x00, 0x00, 0x00, 0x00, 0x40},         // '/'
    {0x41, 0x00, 0x00, 0x00, 0x00, 0x41},         // 'A'
    {0x42, 0x00, 0x00, 0x00, 0x00, 0x42},         // 'B'
    {0x43, 0x00, 0x00, 0x00, 0x00, 0x43},         // 'C'
    {0x44, 0x00, 0x00, 0x00, 0x00, 0x44},         // 'D'
    {0x45, 0x00, 0x00, 0x00, 0x00, 0x45},         // 'E'
    {0x46, 0x00, 0x00, 0x00, 0x00, 0x46},         // 'F'
    {0x47, 0x00, 0x00, 0x00, 0x00, 0x47},         // 'G'

	//  Row 2 keys 9 thru 16
    //           SHIFT CTRL  MENU  NONE
    {0xDC, 0x00, 0x00, 0x00, 0x00, 0x48},         // '\'
    {0x48, 0x00, 0x00, 0x00, 0x00, 0x49},         // 'H'
    {0x49, 0x00, 0x00, 0x00, 0x00, 0x4A},         // 'I'
    {0x4A, 0x00, 0x00, 0x00, 0x00, 0x4B},         // 'J'
    {0x4B, 0x00, 0x00, 0x00, 0x00, 0x4C},         // 'K'
    {0x4C, 0x00, 0x00, 0x00, 0x00, 0x4D},         // 'L'
    {0x4D, 0x00, 0x00, 0x00, 0x00, 0x4E},         // 'M'
    {0x4E, 0x00, 0x00, 0x00, 0x00, 0x4F},         // 'N'

	//  Row 3 keys 17 thru 24
    //           SHIFT CTRL  MENU  NONE
    {0xBC, 0x00, 0x00, 0x00, 0x00, 0x50},         // '''
    {0x4F, 0x00, 0x00, 0x00, 0x00, 0x51},         // 'O'
    {0x50, 0x00, 0x00, 0x00, 0x00, 0x52},         // 'P'
    {0x51, 0x00, 0x00, 0x00, 0x00, 0x53},         // 'Q'  - Fixed key Clear
    {0x52, 0x00, 0x00, 0x00, 0x00, 0x54},         // 'R'
    {0x53, 0x00, 0x00, 0x00, 0x00, 0x55},         // 'S'
    {0x54, 0x00, 0x00, 0x00, 0x00, 0x56},         // 'T'
    {0x55, 0x00, 0x00, 0x00, 0x00, 0x57},         // 'U'

	//  Row 4 keys 25 thru 32
    //           SHIFT CTRL  MENU  NONE
    {0xBD, 0x00, 0x58, 0x00, 0x00, 0x00},         // '_'
    {0x70, 0x00, 0x5B, 0x00, 0x00, 0x59},         // F1 or F11
    {0x71, 0x00, 0x63, 0x00, 0x00, 0x5A},         // F2 or F12
    {0x70, 0x00, 0x5B, 0x00, 0x00, 0x59},         // F11 or F1
    {0x6A, 0x00, 0x00, 0x00, 0x00, 0x5C},         // '*'
    {0x6D, 0x00, 0x00, 0x00, 0x00, 0x5D},         // '-'
    {0x73, 0x00, 0x5E, 0x00, 0x00, 0x62},         // F14 or F4
    {0x56, 0x00, 0x00, 0x00, 0x00, 0x5F},         // 'V'

	//  Row 5 keys 33 thru 41
    //           SHIFT CTRL  MENU  NONE
    {0xDE, 0x00, 0x60, 0x00, 0x00, 0x00},         // '"'
    {0x72, 0x00, 0x00, 0x00, 0x00, 0x61},         // F3 or F13
    {0x73, 0x00, 0x5E, 0x00, 0x00, 0x62},         // F4 or F14
    {0x67, 0x00, 0x37, 0x00, 0x00, 0x37},         // '7'  - Fixed key
    {0x37, 0x00, 0x37, 0x00, 0x00, 0x37},         // '7'  - Fixed key
    {0x68, 0x00, 0x38, 0x00, 0x00, 0x38},         // '8'  - Fixed key
    {0x38, 0x00, 0x38, 0x00, 0x00, 0x38},         // '8'  - Fixed key
    {0x69, 0x00, 0x39, 0x00, 0x00, 0x39},         // '9'  - Fixed key
    {0x39, 0x00, 0x39, 0x00, 0x00, 0x39},         // '9'  - Fixed key
    {0x71, 0x00, 0x63, 0x00, 0x00, 0x5A},         // F22
    {0x74, 0x00, 0x64, 0x00, 0x00, 0x67},         // F15 or F5
    {0x57, 0x00, 0x00, 0x00, 0x00, 0x65},         // 'W'

	//  Row 6 keys 42 thru 50
    //           SHIFT CTRL  MENU  NONE
    {0xBA, 0x00, 0x66, 0x00, 0x00, 0x00},         // ':'
    {0x74, 0x00, 0x64, 0x00, 0x00, 0x67},         // F5 or F15
    {0x75, 0x00, 0x6A, 0x00, 0x00, 0x68},         // F6 or F16
    {0x64, 0x00, 0x34, 0x00, 0x00, 0x34},         // '4'  - Fixed key
    {0x34, 0x00, 0x34, 0x00, 0x00, 0x34},         // '4'  - Fixed key
    {0x65, 0x00, 0x35, 0x00, 0x00, 0x35},         // '5'  - Fixed key
    {0x35, 0x00, 0x35, 0x00, 0x00, 0x35},         // '5'  - Fixed key
    {0x66, 0x00, 0x36, 0x00, 0x00, 0x36},         // '6'  - Fixed key
    {0x36, 0x00, 0x36, 0x00, 0x00, 0x36},         // '6'  - Fixed key
    {0x6B, 0x00, 0x00, 0x00, 0x00, 0x69},         // 'Y'
    {0x75, 0x00, 0x6A, 0x00, 0x00, 0x68},         // F6 or F16
    {0x58, 0x00, 0x00, 0x00, 0x00, 0x6B},         // 'X'

	//  Row 7 keys 51 thru 59
    //           SHIFT CTRL  MENU  NONE
    {0x08, 0x00, 0x00, 0x00, 0x00, 0x6C},         // bksp
    {0x76, 0x00, 0x70, 0x00, 0x00, 0x6D},         // F7 or F17
    {0x77, 0x00, 0x76, 0x00, 0x00, 0x6E},         // F8 or F18
    {0x61, 0x00, 0x31, 0x00, 0x00, 0x31},         // '1'  - Fixed key
    {0x31, 0x00, 0x31, 0x00, 0x00, 0x31},         // '1'  - Fixed key
    {0x62, 0x00, 0x32, 0x00, 0x00, 0x32},         // '2'  - Fixed key
    {0x32, 0x00, 0x32, 0x00, 0x00, 0x32},         // '2'  - Fixed key
    {0x63, 0x00, 0x33, 0x00, 0x00, 0x33},         // '3'  - Fixed key
    {0x33, 0x00, 0x33, 0x00, 0x00, 0x33},         // '3'  - Fixed key
    {0x0D, 0x00, 0x00, 0x00, 0x00, 0x6F},         //
    {0x76, 0x00, 0x70, 0x00, 0x00, 0x6D},         // F17 or F7
    {0x59, 0x00, 0x00, 0x00, 0x00, 0x71},         // 'Y'

	//  Row 8 keys 60 thru 68
    //           SHIFT CTRL  MENU  NONE
    {0x20, 0x00, 0x00, 0x00, 0x00, 0x72},         // ' '
    {0x78, 0x75, 0x00, 0x00, 0x00, 0x73},         // F9 or F19
    {0x79, 0x00, 0x00, 0x00, 0x00, 0x74},         // F10 or F20
    {0x60, 0x00, 0x30, 0x30, 0x30, 0x30},         // '0'
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x30},         // '0'
    {0x2E, 0x00, 0x00, 0x00, 0x00, 0x75},         // '.'
    {0x6E, 0x00, 0x00, 0x00, 0x00, 0x75},         // '.'
    {0x90, 0x00, 0x00, 0x00, 0x00, 0x75},         //
    {0x0D, 0x00, 0x00, 0x00, 0x00, 0x6F},         // CR
    {0x77, 0x00, 0x76, 0x00, 0x00, 0x6E},         // F18 or F8
    {0x5A, 0x00, 0x00, 0x00, 0x00, 0x77},         // 'Z'

    //            SHIFT CTRL  MENU  NONE
    {VK_F1, 0x00, 0x03, 0x03, 0x03, 0x03},        // LOCK MODE
    {VK_F2, 0x00, 0x04, 0x04, 0x04, 0x04},        // REG MODE
    {VK_F3, 0x00, 0x05, 0x05, 0x05, 0x05},        // SUPER MODE
    {VK_F4, 0x00, 0x06, 0x06, 0x06, 0x06},        // PROG MODE

    {DE_TABLE_END, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/*
	Keypress conversion table for the NCR Wedge 78 key keyboard.

	The NCR Wedge 78 key keyboard is a simple keyboard with 6 rows of keys
	each row containing 13 keys.  All of the keys are the same dimensions.

	The keys are in the table beginning with key 1 in the top left corner,
	going across the keyboard row of keys to key 13 in the top right corner.
	Then the next row is specified.

	See the user guide for NCR 5932 Wedge Keyboards, BD20-1369-A

	The keys F11 through F20 are seen as shift plus F1 through F10.
	The keys F21 through F23 are seen as control plus F1 through F3.

*/
CONVERT_KEY aConvertKeyWedge78[] =
{
	// Special key strokes are identified by 0xFn where n = 0 to F.
	// the least significant nibble, n, is used as an index into a
	// special function table to handle that keystroke.

	// First special key stroke is for the Wedge 78 key key lock
    {0x7C, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0},         // '/'  Keyboard mounted key lock

	//  Row 1 keys 1 thru 13
    //      S&C SHIFT CTRL  MENU  NONE
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x40},         // '/'
    {0x31, 0x00, 0x00, 0x00, 0x00, 0x41},         // '1'
    {0x32, 0x00, 0x00, 0x00, 0x00, 0x42},         // '2'
    {0x33, 0x00, 0x00, 0x00, 0x00, 0x43},         // '3'
    {0x34, 0x00, 0x00, 0x00, 0x00, 0x44},         // '4'
    {0x35, 0x00, 0x00, 0x00, 0x00, 0x45},         // '5'
    {0x36, 0x00, 0x00, 0x00, 0x00, 0x46},         // '6'
    {0x37, 0x00, 0x00, 0x00, 0x00, 0x47},         // '7'
    {0x38, 0x00, 0x00, 0x00, 0x00, 0x48},         // '8'
    {0x39, 0x00, 0x00, 0x00, 0x00, 0x49},         // '9'
    {0x30, 0x00, 0x00, 0x00, 0x00, 0x4A},         // '9'
    {0x70, 0x00, 0x00, 0x00, 0x00, 0x4B},         //
    {0x71, 0x00, 0x00, 0x00, 0x00, 0x4C},         // '0'

	//  Row 2 keys 14 thru 26
    //      S&C SHIFT CTRL  MENU  NONE
    {0x1B, 0x00, 0x00, 0x00, 0x00, 0x4D},         // 'A'
    {0x41, 0x00, 0x00, 0x00, 0x00, 0x4E},         // 'A'
    {0x42, 0x00, 0x00, 0x00, 0x00, 0x4F},         // 'B'
    {0x43, 0x00, 0x00, 0x00, 0x00, 0x50},         // 'C'
    {0x44, 0x00, 0x00, 0x00, 0x00, 0x51},         // 'D'
    {0x45, 0x00, 0x00, 0x00, 0x00, 0x52},         // 'E'
    {0x46, 0x00, 0x00, 0x00, 0x00, 0x53},         // 'F'
    {0x90, 0x00, 0x00, 0x00, 0x00, 0x54},         // 'W'
    {0x6F, 0x00, 0x00, 0x00, 0x00, 0x55},         // '5'
    {0x6A, 0x00, 0x00, 0x00, 0x00, 0x56},         // 'X'
    {0x6D, 0x00, 0x00, 0x00, 0x00, 0x57},         // '2'
    {0x72, 0x00, 0x00, 0x00, 0x00, 0x58},         // 'Y'
    {0x73, 0x00, 0x00, 0x00, 0x00, 0x59},         // '.'

	//  Row 3 keys 17 thru 39
    //      S&C SHIFT CTRL  MENU  NONE
    {0x09, 0x00, 0x00, 0x00, 0x00, 0x5A},         // 'B'
    {0x47, 0x00, 0x00, 0x00, 0x00, 0x5B},         // 'G'
    {0x48, 0x00, 0x00, 0x00, 0x00, 0x5C},         // 'H'
    {0x49, 0x00, 0x00, 0x00, 0x00, 0x5D},         // 'I'
    {0x4A, 0x00, 0x00, 0x00, 0x00, 0x5E},         // 'J'
    {0x4B, 0x00, 0x00, 0x00, 0x00, 0x5F},         // 'K'
    {0x08, 0x00, 0x00, 0x00, 0x00, 0x60},         // '8'
    {0x67, 0x00, 0x00, 0x00, 0x00, 0x61},         // ':'
    {0x68, 0x00, 0x00, 0x00, 0x00, 0x62},         // '5'
    {0x69, 0x00, 0x00, 0x00, 0x00, 0x63},         // bksp
    {0x6B, 0x00, 0x00, 0x00, 0x00, 0x64},         // '2'
    {0x74, 0x00, 0x00, 0x00, 0x00, 0x65},         // ' '
    {0x75, 0x00, 0x00, 0x00, 0x00, 0x66},         // '.'

	//  Row 4 keys 40 thru 52
    //      S&C SHIFT CTRL  MENU  NONE
    {0x11, 0x00, 0x00, 0x00, 0x00, 0x67},         // 'C'
    {0x4C, 0x00, 0x00, 0x00, 0x00, 0x68},         // 'L'
    {0x4D, 0x00, 0x00, 0x00, 0x00, 0x69},         // 'M'
    {0x4E, 0x00, 0x00, 0x00, 0x00, 0x6A},         // 'N'
    {0x4F, 0x00, 0x00, 0x00, 0x00, 0x6B},         // 'O'
    {0x50, 0x00, 0x00, 0x00, 0x00, 0x6C},         // 'P'
    {0x14, 0x00, 0x00, 0x00, 0x00, 0x6D},         // '9'
    {0x64, 0x00, 0x00, 0x00, 0x00, 0x6E},         // 'a'
    {0x65, 0x00, 0x00, 0x00, 0x00, 0x6F},         // '6'
    {0x66, 0x00, 0x00, 0x00, 0x00, 0x70},         // F7 or F17
    {0x7F, 0x00, 0x00, 0x00, 0x00, 0x71},         // NONE
    {0x76, 0x00, 0x00, 0x00, 0x00, 0x72},         // F9 or F19
    {0x77, 0x00, 0x00, 0x00, 0x00, 0x73},         //

	//  Row 5 keys 53 thru 66
    //      S&C SHIFT CTRL  MENU  NONE
    {0x10, 0x00, 0x00, 0x00, 0x00, 0x74},         // 'D'
    {0x51, 0x00, 0x00, 0x00, 0x00, 0x75},         // 'Q'
    {0x52, 0x00, 0x00, 0x00, 0x00, 0x76},         // 'R'
    {0x53, 0x00, 0x00, 0x00, 0x00, 0x77},         // 'S'
    {0x54, 0x00, 0x00, 0x00, 0x00, 0x78},         // 'T'
    {0x55, 0x00, 0x00, 0x00, 0x00, 0x79},         // 'U'
    {0x91, 0x00, 0x00, 0x00, 0x00, 0x7A},         // '9'
    {0x61, 0x00, 0x00, 0x00, 0x00, 0x7B},         // 'a'
    {0x62, 0x00, 0x00, 0x00, 0x00, 0x7C},         // 'b'
    {0x63, 0x00, 0x00, 0x00, 0x00, 0x7D},         // 'c'
    {0x13, 0x00, 0x00, 0x00, 0x00, 0x7E},         // '3'
    {0x78, 0x00, 0x00, 0x00, 0x00, 0x7F},         // F10 or F20
    {0x79, 0x00, 0x00, 0x00, 0x00, 0x80},         // CR

	//  Row 6 keys 67 thru 78
    //      S&C SHIFT CTRL  MENU  NONE
    {0x12, 0x00, 0x00, 0x00, 0x00, 0x81},         // 'E'
    {0x56, 0x00, 0x00, 0x00, 0x00, 0x82},         // 'V'
    {0x57, 0x00, 0x00, 0x00, 0x00, 0x83},         // 'W'
    {0x58, 0x00, 0x00, 0x00, 0x00, 0x84},         // 'X'
    {0x59, 0x00, 0x00, 0x00, 0x00, 0x85},         // 'Y'
    {0x5A, 0x00, 0x00, 0x00, 0x00, 0x86},         // 'Z'
    {0xDC, 0x00, 0x00, 0x00, 0x00, 0x87},         // F22
    {0x60, 0x00, 0x00, 0x00, 0x00, 0x88},         // '4'
    {0x20, 0x00, 0x00, 0x00, 0x00, 0x89},         // ' '
    {0x6E, 0x00, 0x00, 0x00, 0x00, 0x8A},         // '1'
    {0x0D, 0x00, 0x00, 0x00, 0x00, 0x8B},         //
    {0x7A, 0x00, 0x00, 0x00, 0x00, 0x8C},         // '0'
    {0x7B, 0x00, 0x00, 0x00, 0x00, 0x8D},         // F18 or F8

    //            SHIFT CTRL  MENU  NONE
    {VK_F1, 0x00, 0x03, 0x03, 0x03, 0x03},        // LOCK MODE
    {VK_F2, 0x00, 0x04, 0x04, 0x04, 0x04},        // REG MODE
    {VK_F3, 0x00, 0x05, 0x05, 0x05, 0x05},        // SUPER MODE
    {VK_F4, 0x00, 0x06, 0x06, 0x06, 0x06},        // PROG MODE

    {DE_TABLE_END, 0x00, 0x00, 0x00, 0x00, 0x00}
};

#if 0
// Removed by Richard Chambers as appears to no longer be needed
// Was used as part of Win CE emulator testing.  Number pad entries
// were turned into a numeric index into this table to provide a
// specific keypress.
UCHAR auchKeyNumberWedge68[] =
{
    0x00,
    // Key number: 1 to 68
    0x40, 0x41, 0x42, 0x43, 0x44,		0x45, 0x46, 0x47,
    0x48, 0x49, 0x4A, 0x4B, 0x4C,		0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54,		0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5B, 0x5C,		0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x37, 0x38, 0x39, 0x63, 0x64, 0x65,
    0x66, 0x67, 0x68, 0x34, 0x35, 0x36, 0x69, 0x6A, 0x6B,
    0x6C, 0x6D, 0x6E, 0x31, 0x32, 0x33, 0x6F, 0x70, 0x71,
    0x72, 0x73, 0x74, 0x30,		  0x75,		  0x76,	0x77,
    // Key Lock:
    0x03, 0x04, 0x05, 0x06
};
#endif


/*
	Key press table for the 7448 Micromotion keyboard.

	The 7448 Micromotion keyboard is a membrane type of keyboard.

	The physical appearance is of a large matrix of keys 11 keys across
	and 16 keys down.

	Keys are numbered in numerical order begining with key number 1 in
	the top left corner thrugh key number 176 in the bottom right corner.

 */

CONVERT_KEY aConvertKeyMicro[] =
{
    //           SHIFT CTRL  MENU  NONE
    {0x31, 0x00, 0x77, 0x00, 0x00, 0x6C},        // '1'
    {0x32, 0x00, 0x8C, 0x00, 0x00, 0x6D},        // '2'
    {0x33, 0x00, 0x79, 0x00, 0x00, 0x6E},        // '3'
    {0x34, 0x00, 0x7A, 0x00, 0x00, 0x6F},        // '4'
    {0x35, 0x00, 0x7B, 0x00, 0x00, 0x70},        // '5'
    {0x36, 0x00, 0xB5, 0x00, 0x00, 0x71},        // '6'
    {0x37, 0x00, 0x7C, 0x00, 0x00, 0x72},        // '7'
    {0x38, 0x00, 0x98, 0x00, 0x00, 0x73},        // '8'
    {0x39, 0x00, 0x7E, 0x00, 0x00, 0x74},        // '9'
    {0x30, 0x00, 0x7F, 0x00, 0x00, 0x75},        // '0'
    {VK_F1, 0x00, 0x00, 0x64, 0xB4, 0xDC},        //
    {VK_F2, 0x00, 0x00, 0x66, 0xAA, 0xDD},
    {VK_F3, 0x00, 0x4E, 0x6A, 0xAB, 0xDE},
    {VK_F4, 0x00, 0x55, 0x57, 0x76, 0xDF},
    {VK_F5, 0x00, 0x69, 0x6B, 0xCF, 0xE0},
    {VK_F6, 0x00, 0x56, 0xAF, 0xDA, 0xE1},
    {VK_F7, 0x00, 0x5C, 0xB0, 0xDB, 0xE2},
    {VK_F8, 0x00, 0x62, 0xB1, 0x40, 0xE3},
    {VK_F9, 0x00, 0x58, 0xB2, 0x41, 0xE4},
    {VK_F10,0x00, 0x5A, 0xB3, 0x42, 0xE5},
    {VK_F11,0x00, 0x00, 0x00, 0x00, 0x4A},
    {VK_F12,0x00, 0x00, 0x00, 0x00, 0x4D},

    //           SHIFT CTRL  MENU  NONE
    {0xBA, 0x00, 0xA7, 0x00, 0x00, 0xA6},        // VK_OEM_1
    {0xBF, 0x00, 0x96, 0x00, 0x00, 0xA0},        // VK_OEM_2
    {0xC0, 0x00, 0xB8, 0x00, 0x00, 0xB7},        // VK_OEM_3
    {0xDB, 0x00, 0x5D, 0x00, 0x00, 0xA8},        // VK_OEM_4
    {0xDC, 0x00, 0x50, 0x00, 0x00, 0xA1},        //
    {0xDD, 0x00, 0x63, 0x00, 0x00, 0xA9},        // VK_OEM_6
    {0xDE, 0x00, 0x78, 0x00, 0x00, 0x7D},        // VK_OEM_7
    {0xBB, 0x00, 0xA5, 0x00, 0x00, 0x81},        // VK_OEM_PLUS
    {0xBD, 0x00, 0xB6, 0x00, 0x00, 0x80},        // VK_OEM_MINUS
    {0xBC, 0x00, 0xA3, 0x00, 0x00, 0x97},        // VK_OEM_COMMA
    {0xBE, 0x00, 0xA4, 0x00, 0x00, 0xA2},        // VK_OEM_PERIOD

    //                    SHIFT CTRL  MENU  NONE
    {VK_TAB,        0x00, 0x00, 0x00, 0x00, 0xAE},
#ifdef _WIN32_WCE
    {VK_DBE_SBCSCHAR,0x00, 0x00, 0x00, 0x00, 0x00},   // VK_0xF3
#endif
	{VK_DELETE,     0x00, 0x00, 0x00, 0x00, 0x43},    //
    {VK_INSERT,     0x00, 0x00, 0x00, 0x00, 0x49},    //
    {VK_PRIOR,      0x00, 0x00, 0x00, 0x00, 0x4B},    //
    {VK_SPACE,      0x00, 0x00, 0x00, 0x00, 0x4C},    //
    {VK_MULTIPLY,   0x00, 0x00, 0x00, 0x00, 0x4F},    //
    {VK_BACK,       0x00, 0x00, 0x00, 0x00, 0x51},    //
    {VK_ESCAPE,     0x00, 0x00, 0x00, 0x00, 0x52},    //
    {VK_END,        0x00, 0x00, 0x00, 0x00, 0x53},    //
    {VK_NEXT,       0x00, 0x00, 0x00, 0x00, 0x54},    //
    {VK_UP,         0x00, 0x00, 0x00, 0x00, 0x59},    //
    {VK_SUBTRACT,   0x00, 0x00, 0x00, 0x00, 0x5B},    //
    {VK_LEFT,       0x00, 0x00, 0x00, 0x00, 0x5E},    //
    {VK_HOME,       0x00, 0x00, 0x00, 0x00, 0x5F},    //
    {VK_RIGHT,      0x00, 0x00, 0x00, 0x00, 0x60},    //
    {VK_ADD,        0x00, 0x00, 0x00, 0x00, 0x61},    //
    {VK_DOWN,       0x00, 0x00, 0x00, 0x00, 0x65},    //
    {VK_DECIMAL,    0x00, 0x00, 0x00, 0x00, 0x67},    //
    {VK_RETURN,     0x00, 0x00, 0x00, 0x00, 0x68},    //

    //                 SHIFT CTRL  MENU  NONE
    {VK_NUMPAD1, 0x00, 0x00, 0x00, 0x00, 0x31},       //
    {VK_NUMPAD2, 0x00, 0x00, 0x00, 0x00, 0x32},       //
    {VK_NUMPAD3, 0x00, 0x00, 0x00, 0x00, 0x33},       //
    {VK_NUMPAD4, 0x00, 0x00, 0x00, 0x00, 0x34},       //
    {VK_NUMPAD5, 0x00, 0x00, 0x00, 0x00, 0x35},       //
    {VK_NUMPAD6, 0x00, 0x00, 0x00, 0x00, 0x36},       //
    {VK_NUMPAD7, 0x00, 0x00, 0x00, 0x00, 0x37},       //
    {VK_NUMPAD8, 0x00, 0x00, 0x00, 0x00, 0x38},       //
    {VK_NUMPAD9, 0x00, 0x00, 0x00, 0x00, 0x39},       //
    {VK_NUMPAD0, 0x00, 0x00, 0x00, 0x00, 0x30},       //

    //           SHIFT CTRL  MENU  NONE
    {0x41, 0x00, 0x8D, 0x00, 0xAC, 0xC4},        // 'A'
    {0x42, 0x00, 0x9D, 0x00, 0xAD, 0xD4},        // 'B'
    {0x43, 0x00, 0x9B, 0x00, 0xC3, 0xD2},        // 'C'
    {0x44, 0x00, 0x8F, 0x00, 0xCD, 0xC6},        // 'D'
    {0x45, 0x00, 0x84, 0x00, 0xCE, 0xBB},        // 'E'
    {0x46, 0x00, 0x90, 0x00, 0xD7, 0xC7},        // 'F'
    {0x47, 0x00, 0x91, 0x00, 0xD8, 0xC8},        // 'G'
    {0x48, 0x00, 0x92, 0x00, 0xD9, 0xC9},        // 'H'
    {0x49, 0x00, 0x89, 0x00, 0x44, 0xC0},        // 'I'
    {0x4A, 0x00, 0x93, 0x00, 0x45, 0xCA},        // 'J'
    {0x4B, 0x00, 0x94, 0x00, 0x46, 0xCB},        // 'K'
    {0x4C, 0x00, 0x95, 0x00, 0x47, 0xCC},        // 'L'
    {0x4D, 0x00, 0x9F, 0x00, 0x48, 0xD6},        // 'M'
    {0x4E, 0x00, 0x9E, 0x00, 0x00, 0xD5},        // 'N'
    {0x4F, 0x00, 0x8A, 0x00, 0x00, 0xC1},        // 'O'
    {0x50, 0x00, 0x8B, 0x00, 0x00, 0xC2},        // 'P'
    {0x51, 0x00, 0x82, 0x00, 0x00, 0xB9},        // 'Q'
    {0x52, 0x00, 0x85, 0x00, 0x00, 0xBC},        // 'R'
    {0x53, 0x00, 0x8E, 0x00, 0x00, 0xC5},        // 'S'
    {0x54, 0x00, 0x86, 0x00, 0x00, 0xBD},        // 'T'
    {0x55, 0x00, 0x88, 0x00, 0x00, 0xBF},        // 'U'
    {0x56, 0x00, 0x9C, 0x00, 0x00, 0xD3},        // 'V'
    {0x57, 0x00, 0x83, 0x00, 0x00, 0xBA},        // 'W'
    {0x58, 0x00, 0x9A, 0x00, 0x00, 0xD1},        // 'X'
    {0x59, 0x00, 0x87, 0x00, 0x00, 0xBE},        // 'Y'
    {0x5A, 0x00, 0x99, 0x00, 0x00, 0xD0},        // 'Z'
    {0x61, 0x00, 0x8D, 0x00, 0xAC, 0xC4},        // 'a'
    {0x62, 0x00, 0x9D, 0x00, 0xAD, 0xD4},        // 'b'
    {0x63, 0x00, 0x9B, 0x00, 0xC3, 0xD2},        // 'c'
    {0x64, 0x00, 0x8F, 0x00, 0xCD, 0xC6},        // 'd'
    {0x65, 0x00, 0x84, 0x00, 0xCE, 0xBB},        // 'e'
    {0x66, 0x00, 0x90, 0x00, 0xD7, 0xC7},        // 'f'
    {0x67, 0x00, 0x91, 0x00, 0xD8, 0xC8},        // 'g'
    {0x68, 0x00, 0x92, 0x00, 0xD9, 0xC9},        // 'h'
    {0x69, 0x00, 0x89, 0x00, 0x44, 0xC0},        // 'i'
    {0x6A, 0x00, 0x93, 0x00, 0x45, 0xCA},        // 'j'
    {0x6B, 0x00, 0x94, 0x00, 0x46, 0xCB},        // 'k'
    {0x6C, 0x00, 0x95, 0x00, 0x47, 0xCC},        // 'l'
    {0x6D, 0x00, 0x9F, 0x00, 0x48, 0xD6},        // 'm'
    {0x6E, 0x00, 0x9E, 0x00, 0x00, 0xD5},        // 'n'
    {0x6F, 0x00, 0x8A, 0x00, 0x00, 0xC1},        // 'o'
    {0x70, 0x00, 0x8B, 0x00, 0x00, 0xC2},        // 'p'
    {0x71, 0x00, 0x82, 0x00, 0x00, 0xB9},        // 'q'
    {0x72, 0x00, 0x85, 0x00, 0x00, 0xBC},        // 'r'
    {0x73, 0x00, 0x8E, 0x00, 0x00, 0xC5},        // 's'
    {0x74, 0x00, 0x86, 0x00, 0x00, 0xBD},        // 't'
    {0x75, 0x00, 0x88, 0x00, 0x00, 0xBF},        // 'u'
    {0x76, 0x00, 0x9C, 0x00, 0x00, 0xD3},        // 'v'
    {0x77, 0x00, 0x83, 0x00, 0x00, 0xBA},        // 'w'
    {0x78, 0x00, 0x9A, 0x00, 0x00, 0xD1},        // 'x'
    {0x79, 0x00, 0x87, 0x00, 0x00, 0xBE},        // 'y'
    {0x7A, 0x00, 0x99, 0x00, 0x00, 0xD0},        // 'z'

    {DE_TABLE_END, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/*
	This table is for converting 7448 Conventional keyboard key presses
	to the BusinessLogic scan code table format.

	The 7448 Conventional Keyboard is setup in a matrix with 10 keys
	across and 8 keys down.  Typically the 0 key is a double key in
	which one of the keys in a pair is deadened and a double key cap
	is used to make the 0 key easy to find and press.

	Keys are numbered in numerical order going across the keyboard beginning
	with key number 1 in the top left corner through key number 10 in the top
	right corner and so down to key number 71 in the lower left corner and
	key number 80 in the bottom right corner.

	Since this table is for an NCR 7448 terminal Conventional keyboard, the range
	of values for the body of the table is from 0x40 to 0x85 because
	the max number of keys is 80 and 10 of those are fixed keys used
	for the numeric pad.

 */
CONVERT_KEY aConvertKey80[] =
{
	// Row 1 keys 01 thru 10 and Row 2 keys 11 thru 20
	// Row 2 is Row 1 shifted that is with shift key depressed with
	// the exception that key number 8 of row 2 is a VK_MULTIPLY rather
	// than the character '8'.
    //                 SHIFT CTRL  MENU  NONE
    {0x31,        0x00, 0x7C, 0x00, 0x00, 0x72},   // '1'
    {0x32,        0x00, 0x7D, 0x00, 0x00, 0x73},   // '2'
    {0x33,        0x00, 0x7E, 0x00, 0x00, 0x74},   // '3'
    {0x34,        0x00, 0x7F, 0x00, 0x00, 0x75},   // '4'
    {0x35,        0x00, 0x80, 0x00, 0x00, 0x76},   // '5'
    {0x36,        0x00, 0x81, 0x00, 0x00, 0x77},   // '6'
    {0x37,        0x00, 0x82, 0x00, 0x00, 0x78},   // '7'
    {0x38,        0x00, 0x00, 0x00, 0x00, 0x79},   // '8' - Row 1 only
    {VK_MULTIPLY, 0x00, 0x00, 0x00, 0x00, 0x83},   // numeric multiply (0x6A) - Row 2 only
    {0x39,        0x00, 0x84, 0x00, 0x00, 0x7A},   // '9'
    {0x30,        0x00, 0x85, 0x00, 0x00, 0x7B},   // '0'

	// Row 3 keys 21 thru 30
    //                 SHIFT CTRL  MENU  NONE
    {0x51,       0x00, 0x00, 0x00, 0x00, 0x40},    // 'q'
    {0x57,       0x00, 0x00, 0x00, 0x00, 0x41},    // 'w'
    {0x45,       0x00, 0x00, 0x00, 0x00, 0x42},    // 'e'
    {0x52,       0x00, 0x00, 0x00, 0x00, 0x43},    // 'r'
    {0x54,       0x00, 0x00, 0x00, 0x00, 0x44},    // 't'
    {0x59,       0x00, 0x00, 0x00, 0x00, 0x45},    // 'y'
    {0x55,       0x00, 0x00, 0x00, 0x00, 0x46},    // 'u'
    {VK_F3,      0x00, 0x00, 0x00, 0x00, 0x47},    // F3 key (0x72)
    {0x49,       0x00, 0x00, 0x00, 0x00, 0x48},    // 'i'
    {0x4F,       0x00, 0x00, 0x00, 0x00, 0x6C},    // 'o'

	// Row 4 keys 31 thru 40
    //                 SHIFT CTRL  MENU  NONE
    {0x41,       0x00, 0x00, 0x00, 0x00, 0x49},    // 'a'
    {0x53,       0x00, 0x00, 0x00, 0x00, 0x4A},    // 's'
    {0x44,       0x00, 0x00, 0x00, 0x00, 0x4B},    // 'd'
    {0x46,       0x00, 0x00, 0x00, 0x00, 0x4C},    // 'f'
    {0x47,       0x00, 0x00, 0x00, 0x00, 0x4D},    // 'g'
    {0x48,       0x00, 0x00, 0x00, 0x00, 0x4E},    // 'h'
    {0x4A,       0x00, 0x00, 0x00, 0x00, 0x4F},    // 'j'
    {VK_F4,      0x00, 0x00, 0x00, 0x00, 0x50},    // F4 key (0x73)
    {0x4B,       0x00, 0x00, 0x00, 0x00, 0x51},    // 'k'
    {0x50,       0x00, 0x00, 0x00, 0x00, 0x6D},    // 'p'

	// Row 5 keys 41 thru 50
    //                 SHIFT CTRL  MENU  NONE
    {0x5A,       0x00, 0x00, 0x00, 0x00, 0x52},    // 'z'
    {0x58,       0x00, 0x00, 0x00, 0x00, 0x53},    // 'x'
    {0x43,       0x00, 0x00, 0x00, 0x00, 0x54},    // 'c'
    {VK_NUMPAD7, 0x00, 0x00, 0x00, 0x00, 0x37},    // numeric 7
    {VK_NUMPAD8, 0x00, 0x00, 0x00, 0x00, 0x38},    // numeric 8
    {VK_NUMPAD9, 0x00, 0x00, 0x00, 0x00, 0x39},    // numeric 9
    {0x56,       0x00, 0x00, 0x00, 0x00, 0x55},    // 'v'
    {VK_F5,      0x00, 0x00, 0x00, 0x00, 0x56},    // F5 key (0x74)
    {0x42,       0x00, 0x00, 0x00, 0x00, 0x57},    // 'b'
    {0x4C,       0x00, 0x00, 0x00, 0x00, 0x6E},    // 'l'

	// Row 6 keys 51 thru 60
    //                 SHIFT CTRL  MENU  NONE
    {VK_HOME,    0x00, 0x00, 0x00, 0x00, 0x58},    // Home key (0x24)
    {VK_PRIOR,   0x00, 0x00, 0x00, 0x00, 0x59},    // Page up key (0x21)
    {VK_NEXT,    0x00, 0x00, 0x00, 0x00, 0x5A},    // Page down key (0x22)
    {VK_NUMPAD4, 0x00, 0x00, 0x00, 0x00, 0x34},    // numeric 4
    {VK_NUMPAD5, 0x00, 0x00, 0x00, 0x00, 0x35},    // numeric 5
    {VK_NUMPAD6, 0x00, 0x00, 0x00, 0x00, 0x36},    // numeric 6
    {VK_TAB,     0x00, 0x00, 0x00, 0x00, 0x5B},    // Tab key (0x09)
    {VK_F6,      0x00, 0x00, 0x00, 0x00, 0x5C},    // F6 key (0x75)
    {0x4E,       0x00, 0x00, 0x00, 0x00, 0x5D},    // 'n'
    {0x4D,       0x00, 0x00, 0x00, 0x00, 0x6F},    // 'm'

	// Row 7 keys 61 thru 70
    //                 SHIFT CTRL  MENU  NONE
    {VK_F1,      0x00, 0x00, 0x00, 0x00, 0x5E},    //  F1 key (0x70)
    {VK_UP,      0x00, 0x00, 0x00, 0x00, 0x5F},    // cursor up key (0x26)
    {VK_F2,      0x00, 0x00, 0x00, 0x00, 0x60},    // F2 key (0x71)
    {VK_NUMPAD1, 0x00, 0x00, 0x00, 0x00, 0x31},    // numeric 1
    {VK_NUMPAD2, 0x00, 0x00, 0x00, 0x00, 0x32},    // numeric 2
    {VK_NUMPAD3, 0x00, 0x00, 0x00, 0x00, 0x33},    // numeric 3
    {VK_SPACE,   0x00, 0x00, 0x00, 0x00, 0x61},    // space (0x20)
    {VK_F7,      0x00, 0x00, 0x00, 0x00, 0x62},    // F7 key (0x76)
    {0xBC,       0x00, 0x00, 0x00, 0x00, 0x63},    // VK_OEM_COMMA
    {VK_DELETE,  0x00, 0x00, 0x00, 0x00, 0x70},    // Delete key (0x2E)

	// Row 8 keys 71 thru 80
    //                 SHIFT CTRL  MENU  NONE
    {VK_LEFT,    0x00, 0x00, 0x00, 0x00, 0x64},    // Left cursor (0x25)
    {VK_DOWN,    0x00, 0x00, 0x00, 0x00, 0x65},    // Down cursor (0x28)
    {VK_RIGHT,   0x00, 0x00, 0x00, 0x00, 0x66},    // Right cursor (0x27)
    {VK_NUMPAD0, 0x00, 0x00, 0x00, 0x00, 0x30},    // numeric 0 (0x60)
												   // Usually dead key double capped with VK_NUMPAD0. Is it F9?
    {VK_DECIMAL, 0x00, 0x00, 0x00, 0x00, 0x68},    // numeric decimal (0x6E)
    {VK_RETURN,  0x00, 0x00, 0x00, 0x00, 0x69},    // return (0x0D)
    {VK_F8,      0x00, 0x00, 0x00, 0x00, 0x6A},    // F8 key (0x77)
    {VK_BACK,    0x00, 0x00, 0x00, 0x00, 0x6B},    // backspace (0x08)
    {VK_ESCAPE,  0x00, 0x00, 0x00, 0x00, 0x71},    // escape key (0x1B)

	// Unused key press codes on the NCR 7448 Conventional keyboard
    //                  SHIFT CTRL  MENU  NONE
    {VK_F9,      0x00, 0x00, 0x00, 0x00, 0x00},    // F9 key (0x78)
    {VK_F10,     0x00, 0x00, 0x00, 0x00, 0x00},    // F10 key (0x79)
    {VK_F11,     0x00, 0x00, 0x00, 0x00, 0x00},    // F11 key (0x7A)
    {VK_F12,     0x00, 0x00, 0x00, 0x00, 0x00},    // F12 key (0x7B)

    {DE_TABLE_END, 0x00, 0x00, 0x00, 0x00, 0x00}
};

#if 0
// Removed by Richard Chambers as appears to no longer be needed
// Was used as part of Win CE emulator testing.  Number pad entries
// were turned into a numeric index into this table to provide a
// specific keypress.
UCHAR auchKeyNumber80[] =
{
    0x00,
    // Key number: 1 to 80
    0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B,
    0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x6C,
    0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x6D,
    0x52, 0x53, 0x54, 0x37, 0x38, 0x39, 0x55, 0x56, 0x57, 0x6E,
    0x58, 0x59, 0x5A, 0x34, 0x35, 0x36, 0x5B, 0x5C, 0x5D, 0x6F,
    0x5E, 0x5F, 0x60, 0x31, 0x32, 0x33, 0x61, 0x62, 0x63, 0x70,
    0x64, 0x65, 0x66, 0x30, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x71,
    // Key Lock:
    0x03, 0x04, 0x05, 0x06
};
#endif

// for Device Interface

typedef struct _PDataInfo
{
    DWORD   dwType;
    VOID*   pvData;
    DWORD   dwLength;
} DataInfo, *PDataInfo;

// for call back interface

__declspec(dllexport)
DWORD CALLBACK BlInterface2(DWORD dwCommand, VOID* pvData, DWORD dwLength);

static CDeviceWinPrinter dcPrinter;
static TCHAR uchPrintType = PMG_NONE_PRINTER;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

# pragma comment(lib, "wbemuuid.lib")

class WmiClass {
public:
	WmiClass() : m_pEnum(0), m_pclsObj(0), m_hrLastResult(0), m_uReturn(1) {}
	WmiClass(IWbemServices *pSvc) : m_pEnum(0), m_pclsObj(0), m_hrLastResult(0), m_uReturn(1) {m_pSvc = pSvc;}
	~WmiClass() { if (m_pclsObj) {m_pclsObj->Release(); m_pclsObj = 0;} if (m_pEnum) { m_pEnum->Release(); m_pEnum = 0;} }
	HRESULT ExecQuery (wchar_t *pQuery, IEnumWbemClassObject **pEnum = 0);
	int  StartEnum (HRESULT &hr);
	HRESULT GetProp (wchar_t *pProp, VARIANT &vtProp);
	bool IsValidProp () { return m_uReturn != 0 && !FAILED(m_hrLastResult); }
	HRESULT  GetMethod (wchar_t *pMethod);
	HRESULT  ExecLastMethod (void);

public:
	HRESULT  m_hrLastResult;
	ULONG    m_uReturn;

private:
	wchar_t  m_LastMethodName[256];
	wchar_t  m_LastObjectPath[512];
	IEnumWbemClassObject *m_pEnum;
	IWbemClassObject *m_pclsObj;
	IWbemServices *m_pSvc;

	IWbemClassObject *m_pMethodParmsIn;
	IWbemClassObject *m_pMethodParmsOut;

};

HRESULT WmiClass::ExecQuery (wchar_t *pQuery, IEnumWbemClassObject **pEnum /* = 0 */)
{
	m_hrLastResult = m_pSvc->ExecQuery(
		bstr_t("WQL"), 
		bstr_t(pQuery),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		NULL,
		&m_pEnum);

	if (FAILED(m_hrLastResult)) {
		m_pEnum = 0;
	}
	if (pEnum) {
		*pEnum = m_pEnum;
	}
	return m_hrLastResult;
}

int  WmiClass::StartEnum (HRESULT &hr)
{
	hr = m_hrLastResult = m_pEnum->Next(WBEM_INFINITE, 1, &m_pclsObj, &m_uReturn);
	if (m_uReturn != 0 && !FAILED(m_hrLastResult)) {
	}

	return 0;
}

HRESULT  WmiClass::GetProp (wchar_t *pProp, VARIANT &vtProp)
{
	m_hrLastResult = m_pclsObj->Get(pProp, 0, &vtProp, 0, 0);
	return m_hrLastResult;
}

HRESULT  WmiClass::GetMethod (wchar_t *pMethod)
{
	m_hrLastResult = m_pclsObj->GetMethod(pMethod, 0, &m_pMethodParmsIn, &m_pMethodParmsOut);
	if (!FAILED(m_hrLastResult)) {
		wcsncpy (m_LastMethodName, pMethod, 250);
	}
	return m_hrLastResult;
}

HRESULT  WmiClass::ExecLastMethod (void)
{
	m_hrLastResult = m_pSvc->ExecMethod(
		bstr_t(m_LastObjectPath), 
		bstr_t(m_LastMethodName),
		0, 
		NULL,
		NULL,
		NULL,
		NULL);
	return m_hrLastResult;
}

static long long llConvertAddress (wchar_t *pString)
{
	long long  llAddress = 1;

	for (int i = 0; pString[i]; i++) {
		if (pString[i] >= L'0' && pString[i] <= L'9') {
			llAddress <<= 4;
			llAddress += pString[i] - L'0';
		} else if (pString[i] >= L'A' && pString[i] <= L'F') {
			llAddress <<= 4;
			llAddress += pString[i] - L'A' + 10;
		} else if (pString[i] >= L'a' && pString[i] <= L'f') {
			llAddress <<= 4;
			llAddress += pString[i] - L'a' + 10;
		}
	}

	return llAddress;
}

static SHORT CDeviceConfigCheckSerialNumber (TCHAR *tcsSerialNo)
{
    HRESULT hres;
    IWbemLocator *pLoc = NULL;

	*tcsSerialNo = 0;    // initialze return string to empty string

	hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        return 1;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
 
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
        pLoc->Release();     
        return 2;                // Program has failed.
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();     
        return 3;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
//        bstr_t("SELECT * FROM Win32_SystemEnclosure"),
//        bstr_t("SELECT * FROM Win32_BaseBoard"),
//        bstr_t("SELECT * FROM Win32_BIOS"),
    ULONG uReturn = 0;
   
	SHORT  sRetStatus = -100;
	{
		WmiClass   myClass (pSvc);
		hres = myClass.ExecQuery (L"SELECT * FROM Win32_BIOS");
		if (FAILED(hres)) {
			pSvc->Release();
			pLoc->Release();
			return 4;               // Program has failed.
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		for (myClass.StartEnum (hres); myClass.IsValidProp(); myClass.StartEnum(hres)) {
			VARIANT vtProp;

			// Get the value of the Name property
			hres = myClass.GetProp(L"SerialNumber", vtProp);
			if (!FAILED(hres)) {
				switch (vtProp.vt) {
					case VT_BSTR:
						_tcscpy (tcsSerialNo, vtProp.bstrVal);
						sRetStatus = 0;
						break;
				}
			}
			VariantClear(&vtProp);
		}
	}

	//---------------------------------------------------------
	//---------------------------------------------------------
	// read the MAC address of the terminal in case we need
	// to derive a pseudo-serial number.
	long long llMacAddress = 0;
	{
		WmiClass   myClass (pSvc);
		hres = myClass.ExecQuery (L"SELECT * FROM Win32_NetworkAdapter");
		if (FAILED(hres)) {
			pSvc->Release();
			pLoc->Release();
			return 4;               // Program has failed.
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------
		SHORT  sRetStatusTwo = -100;
		for (myClass.StartEnum (hres); myClass.IsValidProp(); myClass.StartEnum(hres)) {
			VARIANT vtProp;

			// Get the value of the Name property
			hres = myClass.GetProp(L"PNPDeviceID", vtProp);
			if (!FAILED(hres)) {
				wchar_t *pPciVen = L"PCI\\VEN";
				switch (vtProp.vt) {
					case VT_BSTR:
						if (wcsncmp (pPciVen, vtProp.bstrVal, sizeof(pPciVen)) == 0) {
							hres = myClass.GetProp(L"MACAddress", vtProp);
							if (!FAILED(hres)) {
								switch (vtProp.vt) {
									case VT_BSTR:
										llMacAddress = llConvertAddress (vtProp.bstrVal);
										sRetStatusTwo = 0;
										break;
								}
							}
						}
						break;
 				}
			}
			VariantClear(&vtProp);
		}
	}


    // Cleanup
    // ========
    pSvc->Release();
    pLoc->Release();

	// if the search for a serial number in the BIOS did not provide
	// something then lets see if the NCR Retail Systems Manager has
	// put a serial number into the Windows Registry.
	if (sRetStatus != 0) {
		ULONG  ulCount = 0;
		TCHAR  lpszValue[256] = {0};

		ScfGetRsmValue (L"SerialNumber", &ulCount, lpszValue);
		if (ulCount > 0) {
			_tcscpy (tcsSerialNo, lpszValue);
			sRetStatus = 0;
		}
	}
	return sRetStatus;
}

BOOL CDeviceEngine::ValidateKeyCode (char *sKeyHash, wchar_t *sKeyCode)
{
	int      i, j;
	UCHAR    xBuffer[128];

	// init the cryptographic hash with the provided hash key code.
	// if we reuse this hash each time then we will get the same crypto results.
	CCrypto  myCrypto;
	myCrypto.RflConstructEncryptByteStringClass(sKeyHash);

	memset (xBuffer, 0, sizeof(xBuffer));
	for (i = 0, j = 0; sKeyCode[i]; j++) {
		if (sKeyCode[i] >= '0' && sKeyCode[i] <= '9') {
			xBuffer[j] = (sKeyCode[i] - '0') & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		} else if (sKeyCode[i] >= 'A' && sKeyCode[i] <= 'F') {
			xBuffer[j] = (sKeyCode[i] - 'A' + 10) & 0x0f;
			xBuffer[j] <<= 4;
			i++;
		}
		if (sKeyCode[i] >= '0' && sKeyCode[i] <= '9') {
			xBuffer[j] |= (sKeyCode[i] - '0') & 0x0f;
			i++;
		} else if (sKeyCode[i] >= 'A' && sKeyCode[i] <= 'F') {
			xBuffer[j] |= (sKeyCode[i] - 'A' + 10) & 0x0f;
			i++;
		}
	}

#if 1
	char yBuffer[64];

	myCrypto.RflDecryptSerialNoString((UCHAR *)yBuffer, xBuffer);
	for (i = 0; yBuffer[i]; i++) {
		m_ValidatedString[i] = yBuffer[i];
	}
	m_ValidatedString[i] = 0;
#else
	myCrypto.RflDecryptByteString (xBuffer, j, 128);

	for (i = 0; i < j; i++) {
		m_ValidatedString[i] = xBuffer[i];
	}
#endif
	return TRUE;
}

int CDeviceEngine::BlFwIfLicStatus (VOID)
{
	return m_licStatus;
}

int CDeviceEngine::LoadLicenseKeyFile (void)
{
	CMsxmlWrapper  m_Msxml;
	char  myXmlBuffer[4096];
	memset (myXmlBuffer, 0, sizeof(myXmlBuffer));

	m_licStatus = UNREGISTERED_COPY;

	m_LicSerialNumberString[0] = 0;
	m_LicResellerIdString[0] = 0;
	m_LicKeyCodeString[0] = 0;

	// Open the license file that was dropped and then use MSXML XML parser
	// to pull the information from the XML text.
	SHORT shLicenseFile = PifOpenFile (_T("LICENKEY"), "ro");
	if (shLicenseFile < 0) {
		int  iLength = 0;
		char xmlBuffer[4096];

		// Generate the XML text used for the license file.
		_tcsncpy (m_LicSerialNumberString, m_tcsSerialNo.GetBuffer(), 48);
		m_tcsSerialNo.ReleaseBuffer();
		iLength = sprintf (xmlBuffer, "<licensepayload>\r\n");
		iLength += sprintf (xmlBuffer+iLength, "\t<serialnumber>%S</serialnumber>\r\n", m_LicSerialNumberString);
		iLength += sprintf (xmlBuffer+iLength, "\t<resellerid>%S</resellerid>\r\n", m_LicResellerIdString);
		iLength += sprintf (xmlBuffer+iLength, "\t<keycode>%S</keycode>\r\n", m_LicKeyCodeString);
		iLength += sprintf (xmlBuffer+iLength, "</licensepayload>");
		shLicenseFile = PifOpenFile (_T("LICENKEY"), "wn");
		PifWriteFile (shLicenseFile, xmlBuffer, iLength * sizeof(xmlBuffer[0]));
		PifCloseFile (shLicenseFile);

		return -1;
	}

	ULONG  ulActualBytes = 0;
	PifReadFile (shLicenseFile, myXmlBuffer, sizeof(myXmlBuffer)/sizeof(myXmlBuffer[0]), &ulActualBytes);
	PifCloseFile (shLicenseFile);

	m_Msxml.LoadText (myXmlBuffer);

	int  iRetStatus = -2;

	if (m_Msxml.SelectNode (L"//body")) {
		m_Msxml.SelectNodeGettext(L"//licensepayload/serialnumber", m_LicSerialNumberString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/resellerid", m_LicResellerIdString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/keycode", m_LicKeyCodeString, 120);
		iRetStatus = 0;
	} else if (m_Msxml.SelectNode (L"//licensepayload")) {
		m_Msxml.SelectNodeGettext(L"//licensepayload/serialnumber", m_LicSerialNumberString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/resellerid", m_LicResellerIdString, 63);
		m_Msxml.SelectNodeGettext(L"//licensepayload/keycode", m_LicKeyCodeString, 120);
		iRetStatus = 0;
	}

	if (iRetStatus == 0) {
		int   i;
		char  xValidatedString[128];

		ValidateKeyCode (CRYPTO_KEYCODE_HASH_VALUE, m_LicKeyCodeString);
		for (i = 0; i < m_tcsSerialNo.GetLength (); i++) {
			xValidatedString[i] = m_tcsSerialNo[i];
		}
		xValidatedString[i] = 0;

		if (strncmp (xValidatedString, m_ValidatedString, strlen(xValidatedString)) == 0) {
			iRetStatus = 1;
			m_licStatus = REGISTERED_COPY;
		} else {
			iRetStatus = -3;
		}
	}

	return iRetStatus;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Constructor
//
// Prototype:   void        CDeviceEngine::CDeviceEngine();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceEngine::CDeviceEngine()
{
    // initialize

    m_bOpened           = FALSE;
	m_bStartDoc = FALSE;
	m_bFirstKeylock     = FALSE;
    // m_pDeviceKeylock    = NULL;
    // m_pDeviceDrawer     = NULL;
    m_pDeviceScanner1   = NULL;
    m_pDeviceScanner2   = NULL;
    m_pDeviceScale      = NULL;
	m_pDevicePrinter = NULL;
	m_myAppForMessages  = NULL;
	m_ulPrintCount		= 0;
	m_nConvertKeyStateMachine = 0;
	m_nConvertKeyStateMachineIndex = 0;
	m_bVirtualKeyProcessingEnabled = TRUE;

	m_pHttpServerProc = 0;

	m_pOPOSControl = new COPOSControl;
	m_pZebraControl = new IZebraPrintCtl;  // Create the COleDispatchDriver control for the Zebra Printer interface
	m_bZebraControlDispatch = FALSE;       // m_pZebraControl->CreateDispatch() not yet called
	m_mZebraControlDevices.SetDevice(CDeviceMultipleDeviceMap::NO_DEVICE);           // indicate not devices provisioned on Zebra Printer Control yet.
	memset(&m_flfLogoIndex, 0x00, sizeof(m_flfLogoIndex));

    // create a sync. object

    ::InitializeCriticalSection(&m_csData);

    // send lock mode event to business logic

//  MakeKeylockModeEvent();
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Constructor
//
// Prototype:   void        CDeviceEngine::~CDeviceEngine();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

CDeviceEngine::~CDeviceEngine()
{
    // close each devices
    Close();

    // clear device data
    RemoveAllData();

	try {
		if (m_pOPOSControl) {
			delete m_pOPOSControl;
			m_pOPOSControl = 0;
		}
	}
	catch ( ...) {
		TRACE2("%S(%d): Error CDeviceEngine() destructor delete OPOS control\n", __FILE__, __LINE__);
	}

	if (m_pZebraControl) {
		delete m_pZebraControl;

		m_pZebraControl = 0;
		m_bZebraControlDispatch = FALSE;       // m_pZebraControl->CreateDispatch() not yet called
		m_mZebraControlDevices.SetDevice(CDeviceMultipleDeviceMap::NO_DEVICE);    // indicate not devices provisioned on Zebra Printer Control yet.
	}

	if (m_pHttpServerProc)	{
		delete m_pHttpServerProc;
		m_pHttpServerProc = 0;
	}

    ::DeleteCriticalSection(&m_csData);
}


//////////////////////////////////////////////////////////////////////
// Public Functions
//////////////////////////////////////////////////////////////////////

int CDeviceEngine::Device_SCF_TYPE_CDISPLAY_Setup (PSCINFO pInfo, TCHAR * pchDLL)
{
	{
		CHAR  xBuff[256];
		sprintf(xBuff, "Setting up Customer Display %S  0x%x", pchDLL, pInfo->dwType);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	if (m_pOPOSControl && m_pOPOSControl->m_hWnd) {
		if (m_pOPOSControl->m_lineDisplay.GetState() == OPOS_S_CLOSED) {
			LONG lOposStatus;

			if ((lOposStatus = m_pOPOSControl->m_lineDisplay.Open(pchDLL)) == OPOS_SUCCESS) {
				BOOL bRet = FALSE;
				LONG lCodePage;

				lOposStatus = m_pOPOSControl->m_lineDisplay.ClaimDevice(500);
				if (lOposStatus != OPOS_SUCCESS) {
					char  xBuff[128];
					sprintf(xBuff, "m_lineDisplay.ClaimDevice(500) failed: %d 0x%x.", lOposStatus, lOposStatus);
					NHPOS_ASSERT_TEXT(0, xBuff);
					PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_DEVICE_CLAIM_ERR);
					PifLog(MODULE_LINE_NO(MODULE_FRAMEWORK), (USHORT)__LINE__);
				}
				m_pOPOSControl->m_lineDisplay.SetDeviceEnabled(TRUE);
				m_pOPOSControl->m_lineDisplay.ClearText();

				lCodePage = m_pOPOSControl->m_lineDisplay.GetCharacterSet();
#if 1
				{
					char xBuff[128];
					sprintf(xBuff, "==NOTE:  m_lineDisplay.GetCharacterSet() lCodePage = %d", lCodePage);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
#else
				NHPOS_ASSERT(lCodePage > 400);
				NHPOS_ASSERT(lCodePage > 990);
				NHPOS_ASSERT(lCodePage != DISP_CS_UNICODE);
				NHPOS_ASSERT(lCodePage != DISP_CS_ASCII);
				NHPOS_ASSERT(lCodePage != DISP_CS_ANSI);
#endif

				// Automatically detect whether a display is an APA customer line display
				// or not.  This check is done to support the older APA line displays which
				// accept ANSI and not UNICODE.  If UNICODE is displayed to the device or if
				// the device SetCharacterSet () is used to set it to display UNICODE then all
				// that will be seen on the display is blocks and other indications of a bad
				// display character set.
				// The NHPOS_ASSERTs are being used to print the information in case the detection
				// logic needs to change in the future.
				m_pOPOSControl->m_lineDisplay.m_bUnicodeSupport = (_tcsstr(pchDLL, _T("APA")) == 0);
				NHPOS_ASSERT(m_pOPOSControl->m_lineDisplay.m_bUnicodeSupport);

				if (m_pOPOSControl->m_lineDisplay.m_bUnicodeSupport) {
					lCodePage = BlFwIfGetCodePage();
					lCodePage = DISP_CS_UNICODE;  // Standard Unicode character set non-APA display
					m_pOPOSControl->m_lineDisplay.SetCharacterSet(lCodePage);
				}
				else {
					lCodePage = DISP_CS_ANSI;  // Ansi character set such as for APA display
					m_pOPOSControl->m_lineDisplay.SetCharacterSet(lCodePage);
				}
				lCodePage = m_pOPOSControl->m_lineDisplay.GetCharacterSet();
#if 1
				{
					char xBuff[128];
					sprintf(xBuff, "==NOTE:  m_lineDisplay.GetCharacterSet() lCodePage = %d", lCodePage);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
#else
				NHPOS_ASSERT(lCodePage > 400);
				NHPOS_ASSERT(lCodePage > 990);
				NHPOS_ASSERT(lCodePage != DISP_CS_UNICODE);
				NHPOS_ASSERT(lCodePage != DISP_CS_ASCII);
				NHPOS_ASSERT(lCodePage != DISP_CS_ANSI);
#endif
			}
			else
			{
				char  xBuff[128];

				sprintf(xBuff, "SCF_TYPE_CDISPLAY: m_lineDisplay.Open(pchDLL) failed %d 0x%x.", lOposStatus, lOposStatus);
				NHPOS_ASSERT_TEXT(0, xBuff);
				PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_DISPLAY_OPEN_ERR);
			}
		}
		else {
			NHPOS_ASSERT_TEXT(0, "m_lineDisplay.GetState() != OPOS_S_CLOSED");
		}
	}
	else
	{
		// In order to test the APA display, we will set this to false
		// if there is a problem with accessing the line display control
		if (m_pOPOSControl && m_pOPOSControl->m_hWnd) {
			m_pOPOSControl->m_lineDisplay.m_bUnicodeSupport = FALSE;
		}
		NHPOS_ASSERT(m_pOPOSControl);
		NHPOS_ASSERT(m_pOPOSControl->m_hWnd);
		PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_DISPLAY_STATE_ERR);
	}

	return 0;
}

int CDeviceEngine::Device_SCF_TYPE_PRINTER_Setup(PSCINFO pInfo, TCHAR * pchDevice, TCHAR * pchDLL)
{
	BOOL    bResult;
	DWORD   dwType;

	{
		CHAR  xBuff[256];
		sprintf(xBuff, "Setting up printer %S  0x%x", pchDLL, pInfo->dwType);
		NHPOS_NONASSERT_TEXT(xBuff);
	}

	if (0 == _tcscmp(pchDLL, SCF_OPOS_NAME_WIN_PRINTER))
	{
		// Get the default printer.
		CPrintDialog dlg(FALSE);
		dlg.GetDefaults();

		// Is a default printer set up?
		HDC hdcPrinter = dlg.GetPrinterDC();

		if (hdcPrinter)
		{
			// Create a CDC and attach it to the default printer.
			dcPrinter.Attach(hdcPrinter);
			dcPrinter.m_tpObjectStatus = CDeviceWinPrinter::PrintClosed;

			// Font size may affect line length calculation in CDeviceWinPrinter::PrintLine().
			dcPrinter.m_fontText.CreatePointFont(80, _T("Courier New"), CDC::FromHandle(hdcPrinter));
			dcPrinter.m_oldFont = dcPrinter.SelectObject(&dcPrinter.m_fontText);

			uchPrintType = PMG_WIN_PRINTER;
		}
		else
		{
			PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_PRINTER_GETDC_ERR);
		}
	}
	else if (0 == _tcscmp(pchDLL, SCF_COM_NAME_PRINTER)) {
		if (m_pDevicePrinter == NULL)
		{
			CDevicePrinter*   pDevice;

			{
				CHAR  xBuff[256];
				sprintf(xBuff, "Setting up a printer %S  0x%x", pchDLL, pInfo->dwType);
				NHPOS_NONASSERT_TEXT(xBuff);
			}

			// Make a call to ScfGetActiveDevice(),and ScfGetParameters()
			// to retrieve the provisioned COM port and baud rate - TLDJR
			CString csCOMPort;
			CString csBaudRate;
			TCHAR   atchDeviceName[SCF_USER_BUFFER] = { 0 };
			TCHAR   atchDllName[SCF_USER_BUFFER] = { 0 };
			TCHAR   portname[32] = { 0 };
			TCHAR   baudRate[32] = { 0 };
			DWORD   dwCount, dwDataType;
			int     dwRet;

			// Get the Active Printer, COM port and and baud rate
			dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, atchDeviceName, atchDllName);
			dwRet = ScfGetParameter(atchDeviceName, SCF_DATANAME_PORT, &dwDataType, portname, sizeof(portname), &dwCount);
			csCOMPort = _T("COM") + (CString)portname += _T(":");
			dwRet = ScfGetParameter(atchDeviceName, SCF_DATANAME_BAUD, &dwDataType, baudRate, sizeof(baudRate), &dwCount);
			csBaudRate = baudRate;
			long baud = _ttoi(csBaudRate);

			pDevice = new CDevicePrinter(this);
			bResult = pDevice->Open(pchDevice, pchDLL);
			dwType = bResult ? pDevice->GetDeviceType() : 0;

			if (dwType == SCF_TYPE_PRINTER) {
				m_pDevicePrinter = pDevice;
				uchPrintType = PMG_COM_PRINTER;
			}
			else
			{
				delete pDevice;
				PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_SCALE_SPEC_ERR);
			}
		}
	}
	else if (!_tcscmp(pchDLL, SCF_ZEBRA_NAME_PRINTER))
	{
		// Initialize the page width to 350 dots which is about 2 inches, set the
		// initial font (two values) and line height, and set the extra intercharacter spacing to 0.
		//TCHAR   *tcZebraPrinterInitString = _T("! U1 PAGE-WIDTH 350\r\n! U1 SETLP 5 0 18\r\n! U1 SETSP 0\r\n! U1 SETLP-TIMEOUT 8\r\n");
		TCHAR   *tcZebraPrinterInitString = _T("! U1 PAGE-WIDTH 400\r\n! U1 SETLP 5 0 18\r\n! U1 SETSP 0\r\n! U1 SETLP-TIMEOUT 8\r\n");

		/* NOTICE!!!
		When working with the Zebra Printer(s) ,you may recieve an error in the windows
		event log, concerning "failed bluetooth authentication". If this occurs you'll
		need to use a program called "Label Vista" to configure the bluetooth settings
		of the Zebra printer (You may need to set a bluetooth pin/passkey). a copy of
		Label Vista can be found on the N:\ drive in the "ZebraPrinters" folder
		( N:\Setups_SDKs_Installers\ZebraPrinters ). A 5-pin micro USB connector must be used to
		connect the zebra printer to the PC when using Label Vista. - TLDR
		*/
		if (!m_pZebraControl) {
			m_pZebraControl = new IZebraPrintCtl;
		}

		if (m_pZebraControl && !m_bZebraControlDispatch) {
			m_bZebraControlDispatch = m_pZebraControl->CreateDispatch(_T("ZebraPrint.ZebraPrintCtl"));
			if (m_bZebraControlDispatch) {
				// Make a call to ScfGetActiveDevice(),and ScfGetParameters()
				// to retrieve the provisioned COM port and baud rate - TLDJR
				CString csCOMPort;
				CString csBaudRate;
				TCHAR   atchDeviceName[SCF_USER_BUFFER];
				TCHAR   atchDllName[SCF_USER_BUFFER];
				TCHAR   portname[32];
				TCHAR   baudRate[32];
				DWORD   dwCount, dwDataType;
				int     dwRet;

				memset(&atchDeviceName, 0x00, sizeof(atchDeviceName));
				memset(&atchDllName, 0x00, sizeof(atchDllName));

				// Get the Active Printer, COM port and and baud rate
				dwRet = ScfGetActiveDevice(SCF_TYPENAME_PRINTER, &dwCount, atchDeviceName, atchDllName);
				dwRet = ScfGetParameter(atchDeviceName, SCF_DATANAME_PORT, &dwDataType, portname, sizeof(portname), &dwCount);
				csCOMPort = "COM" + (CString)portname += ":";
				dwRet = ScfGetParameter(atchDeviceName, SCF_DATANAME_BAUD, &dwDataType, baudRate, sizeof(baudRate), &dwCount);
				csBaudRate = baudRate;
				long baud = _ttoi(csBaudRate);
				m_pZebraControl->SerialConnection(csCOMPort.GetBuffer(csCOMPort.GetLength()), baud);

				m_pZebraControl->Print(tcZebraPrinterInitString);
				uchPrintType = PMG_ZEBRA_PRINTER;
				m_mZebraControlDevices.SetDevice(CDeviceMultipleDeviceMap::PRINTER_DEVICE);           // indicate PRINTER devices provisioned on Zebra Printer Control
			}
			else {
				NHPOS_ASSERT_TEXT(0, "SCF_TYPE_PRINTER: m_pZebraControl->CreateDispatch(_T(\"ZebraPrint.ZebraPrintCtl\") failed.");
			}
		}
		else if (m_pZebraControl && m_bZebraControlDispatch) {
			uchPrintType = PMG_ZEBRA_PRINTER;
			m_mZebraControlDevices.SetDevice(CDeviceMultipleDeviceMap::PRINTER_DEVICE);           // indicate PRINTER devices provisioned on Zebra Printer Control
		}
	}
	else if (m_pOPOSControl && m_pOPOSControl->m_hWnd)
	{
		if (m_pOPOSControl->m_OPOSPrinter.GetState() == OPOS_S_CLOSED) {
			LONG  lOposStatus = 0;

			if ((lOposStatus = m_pOPOSControl->m_OPOSPrinter.Open(pchDLL)) == OPOS_SUCCESS)
			{
				m_pOPOSControl->m_OPOSPrinter.SetPowerNotify(OPOS_PN_ENABLED);
				if (m_pOPOSControl->m_OPOSPrinter.ClaimDevice(500) != OPOS_SUCCESS) {
					PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_DEVICE_CLAIM_ERR);
					PifLog(MODULE_LINE_NO(MODULE_FRAMEWORK), (USHORT)__LINE__);
				}
				m_pOPOSControl->m_OPOSPrinter.SetDeviceEnabled(TRUE);
#if 0
				// The following can be used to pull the OPOS printer Capability measurements if curious.
				// Normally this would not be in a release build but may be helpful for designer testing
				{
					LONG  lValue;
					char  xBuff[128];

					lValue = m_pOPOSControl->m_OPOSPrinter.GetRecLineSpacing();
					sprintf(xBuff, "GetRecLineSpacing(): lValue = %d", lValue);
					NHPOS_NONASSERT_TEXT(xBuff);

					lValue = m_pOPOSControl->m_OPOSPrinter.GetRecLetterQuality();
					sprintf(xBuff, "GetRecLetterQuality(): lValue = %d", lValue);
					NHPOS_NONASSERT_TEXT(xBuff);

					lValue = 1;
					m_pOPOSControl->m_OPOSPrinter.SetRecLetterQuality(lValue);
					sprintf(xBuff, "SetRecLetterQuality(): lValue = %d", lValue);
					NHPOS_NONASSERT_TEXT(xBuff);

					lValue = m_pOPOSControl->m_OPOSPrinter.GetRecLineHeight();
					sprintf(xBuff, "GetRecLineHeight(): lValue = %d", lValue);
					NHPOS_NONASSERT_TEXT(xBuff);

					lValue = m_pOPOSControl->m_OPOSPrinter.GetRecLineWidth();
					sprintf(xBuff, "GetRecLineWidth(): lValue = %d", lValue);
					NHPOS_NONASSERT_TEXT(xBuff);

					lValue = m_pOPOSControl->m_OPOSPrinter.GetRecLineChars();
					sprintf(xBuff, "GetRecLineChars(): lValue = %d", lValue);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
#endif
				{
					char  xBuff[128];
					LONG  lRetValue;

					lRetValue = m_pOPOSControl->m_OPOSPrinter.GetCapRecBarCode();
					sprintf(xBuff, "   GetCapRecBarCode status = %d", lRetValue);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

				//asign the print type, this type defines which type of print commands to go into, either
				//windows or opos types.
				uchPrintType = PMG_OPOS_PRINTER;
			}
			else
			{
				char  xBuff[128];

				sprintf(xBuff, "SCF_TYPE_PRINTER: m_OPOSPrinter.Open(pchDLL) failed %d 0x%x.", lOposStatus, lOposStatus);
				NHPOS_ASSERT_TEXT(0, xBuff);
				int lOposResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
				int lOposResultCodeExt = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
				sprintf(xBuff, "                  ResultCode %d 0x%x  ResultCodeExtended  %d 0x%x.", lOposResultCode, lOposResultCode, lOposResultCodeExt, lOposResultCodeExt);
				NHPOS_ASSERT_TEXT(0, xBuff);
				PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_PRINTER_OPEN_ERR);
			}
		}
		else {
			NHPOS_ASSERT_TEXT(0, "SCF_TYPE_PRINTER: m_OPOSPrinter.GetState() != OPOS_S_CLOSED.");
		}
	}
	else
	{
		NHPOS_ASSERT_TEXT(0, "SCF_TYPE_PRINTER: Unknown printer.");
		PifLog(MODULE_FRAMEWORK, LOG_EVENT_FW_PRINTER_SPEC_ERR);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Open a input devices
//
// Prototype:   BOOL        CDeviceEngine::Open();
//
// Inputs:      nothing
//
// Outputs:     BOOL        bResult;    - states
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::Open()
{
    BOOL    bResult;
    DWORD   dwResult;
    DWORD   dwType;
    DWORD   dwCount;
	TCHAR   achDevice[SCF_USER_BUFFER], myVersionNumber[24] = {0};
    TCHAR   achDLL[SCF_USER_BUFFER];
    PSCINFO pInfo;
    static  SCINFO scInfo[] = { { SCF_TYPENAME_KEYLOCK,     SCF_TYPE_KEYLOCK    },
                                { SCF_TYPENAME_MSR,         SCF_TYPE_MSR        },
                                { SCF_TYPENAME_MSR1,        SCF_TYPE_MSR1       },
                                { SCF_TYPENAME_MSR2,        SCF_TYPE_MSR2       },
                                { SCF_TYPENAME_SCANNER,     SCF_TYPE_SCANNER    },
                                { SCF_TYPENAME_SCALE,       SCF_TYPE_SCALE      },
                                { SCF_TYPENAME_SERVERLOCK,  SCF_TYPE_SERVERLOCK },
                                { SCF_TYPENAME_CDISPLAY,    SCF_TYPE_CDISPLAY   },
                                { SCF_TYPENAME_PRINTER,     SCF_TYPE_PRINTER    },
                                { SCF_TYPENAME_DRAWER1,     SCF_TYPE_DRAWER1     },
								{ SCF_TYPENAME_DRAWER2,     SCF_TYPE_DRAWER2    },
                                { SCF_TYPENAME_COIN,		SCF_TYPE_COIN	    },
                                { SCF_TYPENAME_SIGCAP1,		SCF_TYPE_SIGCAP1    },
                                { SCF_TYPENAME_PINPAD1,		SCF_TYPE_PINPAD1    },
								{ NULL,                     0                   } };

    // already opened ?
    if (m_bOpened)
    {
        return FALSE;                       // exit ...
    }

	{
		TCHAR  tcsSerialNo[128];
		CDeviceConfigCheckSerialNumber (tcsSerialNo);
		m_tcsSerialNo = tcsSerialNo;

		sprintf ((char *)tcsSerialNo, "==NOTE: BIOS Serial Number %S", (LPCWSTR)m_tcsSerialNo);
		NHPOS_NONASSERT_TEXT((char *)tcsSerialNo);
	}

	{
		// Pin the Pif.dll module so that the PIF library will stay in memory until
		// GenPOS actually exits. We have seen cases with running GenPOS in Debug mode
		// in the Debugger in which it appears that the DETACH functionality of Pif.dll
		// is invoked before the GenPOS application is actually ready to exit.
		//
		// What we see is a kind of pause after doing a Stop debugging from the Debug menu.
		// Then the VS 2005 debugger seems to detach. However the GenPOS process, FrameWork,
		// remains in memory, recompile fails due to dlls being in use, and the process can not
		// be terminated using Task Manager. The workstation must be rebooted to clear the process.
		// We have tried to attach the debugger to the process again and the attach is refused.
		// It appears the Framework process is in a state such that it is dead however a
		// debugger is still attached nor can it be terminated.
		//
		// It appears from several debug sessions that what is happening is that the Framework
		// app is waiting on an event and the Piff.dll is detached, along with its data structures
		// and memory before the event is triggered. The result appears to be that Framework is
		// then waiting on an event that will never happen and the event data structures are no
		// longer valid. It would seem that Windows would handle this but it appears that it doesn't.
		//
		// The GET_MODULE_HANDLE_EX_FLAG_PIN flag causes the specified dll to be pinned
		// in memory.
		//    The module stays loaded until the process is terminated, no matter
		//    how many times FreeLibrary is called.
		// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms683200(v=vs.85).aspx
		//
		//      Richard Chambers, Mar-12-2018
		// See also PifFinalize().
		HMODULE  hMod;
		BOOL     bPin = ::GetModuleHandleEx (GET_MODULE_HANDLE_EX_FLAG_PIN, L"pif.dll", &hMod);
		NHPOS_ASSERT(bPin);
		NHPOS_ASSERT(hMod);
	}

	// The following #if is used to determine whether this build is to be
	// software protected with license key or not.
#if defined(DEVICE_LOADLICENSEKEYFILE_ON) && (DEVICE_LOADLICENSEKEYFILE_ON == 1)
	m_licStatus = UNREGISTERED_COPY;
	LoadLicenseKeyFile();
#else
	m_licStatus = REGISTERED_COPY;
#endif
	NHPOS_ASSERT_TEXT((m_licStatus == REGISTERED_COPY), "==NOTE:  Unregistered copy.");

    // setup call back function
    bResult = BlSetFrameworkFunc(CMD_DEVICEENGINE_FUNC, BlInterface2);

	// mark as opened
	m_bOpened = TRUE;

	// initialize business logic
	hResourceDll = AfxGetResourceHandle(); //Get resource information

	LoadString(hResourceDll, FILE_ID_VER, myVersionNumber, sizeof(myVersionNumber)); // Load Version Information, to be used later for conversion

	if (m_pOPOSControl == NULL) {
		char xBuff[128];
		sprintf(xBuff, "**ERROR:  m_pOPOSControl == NULL DeviceEngine setup aborted.");
		NHPOS_NONASSERT_NOTE("**ERROR", xBuff);
//		bResult = BlInitialize(myVersionNumber); //pass in version information to Business Logic.
//		return FALSE;
	}

	try {
		m_pOPOSControl->Create(IDD_BASE_DIALOG);
	}
	catch (CException* e)
	{
		char xBuff[128];
		sprintf(xBuff, "**ERROR:  m_pOPOSControl->Create(IDD_BASE_DIALOG) DeviceEngine setup aborted.");
		NHPOS_NONASSERT_NOTE("**ERROR", xBuff);
		e->Delete();
//		bResult = BlInitialize(myVersionNumber); //pass in version information to Business Logic.
//		return FALSE;
	}

	// check to see if the OPOS control is available and loaded properly.
	// if the window handle is null then there is a problem.
	if (m_pOPOSControl->m_hWnd == 0) {
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_CONTROL_CREATE_ERR);
	}

    for (pInfo = scInfo; pInfo->pszName != NULL; pInfo++)
    {
        // clear buffer

        memset(achDevice, '\0', sizeof(achDevice));
        memset(achDLL,    '\0', sizeof(achDLL));

        // get active device information

        dwResult = ScfGetActiveDevice(pInfo->pszName, &dwCount, achDevice, achDLL);

        if (dwResult != SCF_SUCCESS || dwCount == 0)
        {
            continue;                       // next loop ...
        }

        TCHAR*  pchDevice = achDevice;
        TCHAR*  pchDLL    = achDLL;

        while (dwCount-- && *pchDLL)
        {
            if (pInfo->dwType == SCF_TYPE_KEYLOCK)
            {
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_KEYLOCK_STATE_ERR);
            }
			else if (pInfo->dwType == SCF_TYPE_DRAWER1)
            {
				{
					CHAR  xBuff[256];
					sprintf(xBuff, "Setting up Drawer 1 %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
				if(m_pOPOSControl && m_pOPOSControl->m_hWnd && m_pOPOSControl->m_cashDrawer.GetState() == OPOS_S_CLOSED)
				{
					if(m_pOPOSControl->m_cashDrawer.Open(pchDLL) == OPOS_SUCCESS)
					{
						if (m_pOPOSControl->m_cashDrawer.ClaimDevice(500) != OPOS_SUCCESS) {
							PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_DEVICE_CLAIM_ERR);
							PifLog (MODULE_LINE_NO(MODULE_FRAMEWORK), (USHORT)__LINE__);
						}
						m_pOPOSControl->m_cashDrawer.SetDeviceEnabled(TRUE);
					}
					else {
						NHPOS_ASSERT_TEXT(0, "SCF_TYPE_DRAWER1: m_pOPOSControl->m_cashDrawer.Open(pchDLL) != OPOS_SUCCESS");
					}
				}
				else {
					NHPOS_ASSERT_TEXT(0, "SCF_TYPE_DRAWER1: mm_pOPOSControl->m_cashDrawer.GetState() == OPOS_S_CLOSED");
				}
			}else if (pInfo->dwType == SCF_TYPE_DRAWER2)
			{
				{
					CHAR  xBuff[256];
					sprintf(xBuff, "Setting up Drawer 2 %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
				if (m_pOPOSControl && m_pOPOSControl->m_hWnd && m_pOPOSControl->m_cashDrawer2.GetState() == OPOS_S_CLOSED){
					if(m_pOPOSControl->m_cashDrawer2.Open(pchDLL) == OPOS_SUCCESS){
						if (m_pOPOSControl->m_cashDrawer2.ClaimDevice(500) != OPOS_SUCCESS) {
							PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_DEVICE_CLAIM_ERR);
							PifLog (MODULE_LINE_NO(MODULE_FRAMEWORK), (USHORT)__LINE__);
						}
						m_pOPOSControl->m_cashDrawer2.SetDeviceEnabled(TRUE);
					}
					else {
						NHPOS_ASSERT_TEXT(0, "SCF_TYPE_DRAWER2: m_pOPOSControl->m_cashDrawer.Open(pchDLL) != OPOS_SUCCESS");
					}
				}
				else {
					NHPOS_ASSERT_TEXT(0, "SCF_TYPE_DRAWER2: mm_pOPOSControl->m_cashDrawer.GetState() == OPOS_S_CLOSED");
				}
            }
            else if (pInfo->dwType == SCF_TYPE_MSR || pInfo->dwType == SCF_TYPE_MSR1 || pInfo->dwType == SCF_TYPE_MSR2)
            {
				CHAR  xBuff[256];
				sprintf (xBuff, "REMOVED: Setting up an MSR %S, 0x%x", pchDLL, pInfo->dwType);
				NHPOS_NONASSERT_TEXT(xBuff);

				// Removed built-in MSR handling source code as part of
				// removing legacy source in preparation for first upload of
				// OpenGenPOS into GitHub.
				// We are removing legacy MSR as part of eliminating possiblity
				// of being used as an PCI-DSS In Scope Application which requires
				// assessment and meeting PCI standards for account holder information.
				//		OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers

			}
			else if (pInfo->dwType == SCF_TYPE_SIGCAP1)
			{
				{
					CHAR  xBuff[256];
					sprintf (xBuff, "REMOVED: Setting up a SIGCAP %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

            }
			else if (pInfo->dwType == SCF_TYPE_PINPAD1)
			{
				{
					CHAR  xBuff[256];
					sprintf (xBuff, "REMOVED: Setting up a PIN pad %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

            }
            else if ((pInfo->dwType & SCF_TYPE_SCANNER) != 0 && ((m_pDeviceScanner1 == NULL) || (m_pDeviceScanner2 == NULL)))
            {
                CDeviceScanner* pDevice;

				{
					CHAR  xBuff[256];
					sprintf (xBuff, "Setting up a scanner %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

                pDevice = new CDeviceScanner(this);
                bResult = pDevice->Open(pchDevice, pchDLL);
                dwType  = bResult ? pDevice->GetDeviceType() : 0;

                if (dwType == SCF_TYPE_SCANNER)
                {
                    if (m_pDeviceScanner1 == NULL) {
                        m_pDeviceScanner1 = pDevice;
                    }
                    else {
                        m_pDeviceScanner2 = pDevice;
                    }
                }
                else if (dwType == (SCF_TYPE_SCANNER | SCF_TYPE_SCALE))
                {
                    if (m_pDeviceScanner1 == NULL) {
                        m_pDeviceScanner1 = pDevice;
                    }
                    else {
                        m_pDeviceScanner2 = pDevice;
                    }

                    if (m_pDeviceScale == NULL) m_pDeviceScale = (CDeviceScale*)pDevice;
                }
                else
				{
                    delete pDevice;
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SCANNER_SPEC_ERR);
				}
            }
            else if (pInfo->dwType == SCF_TYPE_SCALE && m_pDeviceScale == NULL)
            {
                CDeviceScale*   pDevice;

				{
					CHAR  xBuff[256];
					sprintf (xBuff, "Setting up a scale %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

                pDevice = new CDeviceScale(this);
                bResult = pDevice->Open(pchDevice, pchDLL);
                dwType  = bResult ? pDevice->GetDeviceType() : 0;

                if (dwType == SCF_TYPE_SCALE)
                    m_pDeviceScale = pDevice;
                else
				{
                    delete pDevice;
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SCALE_SPEC_ERR);
				}
            }
            else if (pInfo->dwType == SCF_TYPE_SERVERLOCK)
            {
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_SERVRLOCK_SPEC_ERR);
            }
			else if (pInfo->dwType == SCF_TYPE_CDISPLAY) {
				Device_SCF_TYPE_CDISPLAY_Setup(pInfo, pchDLL);
			}
			else if (pInfo->dwType == SCF_TYPE_PRINTER) {
				Device_SCF_TYPE_PRINTER_Setup(pInfo, pchDevice, pchDLL);
			} else if (pInfo->dwType == SCF_TYPE_COIN)
			{
				{
					CHAR  xBuff[256];
					sprintf (xBuff, "Setting up coin %S  0x%x", pchDLL, pInfo->dwType);
					NHPOS_NONASSERT_TEXT(xBuff);
				}

				if(m_pOPOSControl && m_pOPOSControl->m_hWnd)
				{
					if(m_pOPOSControl->m_OPOSCoin.GetState() == OPOS_S_CLOSED)
					{
						if(m_pOPOSControl->m_OPOSCoin.Open(pchDLL) == OPOS_SUCCESS)
						{
							if (m_pOPOSControl->m_OPOSCoin.ClaimDevice(500) != OPOS_SUCCESS) {
								PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_DEVICE_CLAIM_ERR);
								PifLog (MODULE_LINE_NO(MODULE_FRAMEWORK), (USHORT)__LINE__);
							}
							m_pOPOSControl->m_OPOSCoin.SetDeviceEnabled(TRUE);
						}
						else
						{
							NHPOS_ASSERT_TEXT(0, "m_OPOSCoin.Open(pchDLL) failed.");
							PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_COIN_OPEN_ERR);
						}
					}
					else
					{
						NHPOS_ASSERT_TEXT(0, "m_OPOSCoin.GetState() != OPOS_S_CLOSED");
						PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_COIN_STATE_ERR);
					}
				} else {
				}
			}
			else {
				NHPOS_ASSERT_TEXT(0, "Unknown device Type.");
			}

            // move pointer
            pchDevice += _tcslen(pchDevice) + 1;
            pchDLL    += _tcslen(pchDLL)    + 1;
        }
    }

	{
		TCHAR   atchDeviceName[SCF_USER_BUFFER];
		TCHAR   atchDllName[SCF_USER_BUFFER];
		TCHAR   *pDeviceName;
		TCHAR   *pDllName;
		DWORD   dwRet, dwCount;

		pDeviceName = atchDeviceName;
		pDllName    = atchDllName;

		dwRet = ScfGetActiveDevice(SCF_TYPENAME_HTTPSERVER, &dwCount, pDeviceName, pDllName);

		if ((dwRet == SCF_SUCCESS) && (dwCount)) {
			// initialize the http server
			if (!m_pHttpServerProc)
			{
				m_pHttpServerProc = new CServerProc (TEXT("httpserver.dll"));

				if (m_pHttpServerProc) {
					if (m_pHttpServerProc->LoadDllProcs())
					{
						if (m_pHttpServerProc->myInterface.m_pfnStartThread) {
							BlInitHttp(&m_pHttpServerProc->myInterface);
							NHPOS_NONASSERT_NOTE("==STATE", "==STATE: Tiny HTTP server started.");
						}
					}
					else {
						memset (&(m_pHttpServerProc->myInterface), 0, sizeof(m_pHttpServerProc->myInterface));
						PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_LOAD_HTTP_DLL);
						NHPOS_ASSERT_TEXT(0, "==ERROR: Tiny HTTP server start failed. DLL load.");
					}
				} else {
					NHPOS_ASSERT_TEXT(0, "**ERROR: new CServerProc() failed.");
				}
			}
		}
		else {
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_HTTP_DLL_NOLOAD);
		}

		dwRet = ScfGetActiveDevice(SCF_TYPENAME_CONNENGINEMFC, &dwCount, pDeviceName, pDllName);
		if ((dwRet == SCF_SUCCESS) && (dwCount)) {
			// initialize the Connection Engine as a server
			if (!m_pHttpServerProc)
			{

#if 0
				if (m_pHttpServerProc->LoadDllProcs())
				{
					if (m_pHttpServerProc->myInterface.m_pfnStartThread) {
						BlInitHttp(&m_pHttpServerProc->myInterface);
					}
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_CONNENGINE_START);
				}
				else {
					memset (&(m_pHttpServerProc->myInterface), 0, sizeof(m_pHttpServerProc->myInterface));
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_CONNENGINE_FAILED);
				}
#endif
			}
		}
		else {
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_CONNENGINE_UNUSED);
		}
	}

	bResult = BlInitialize(myVersionNumber); //pass in version information to Business Logic.
											 // send lock mode event to business logic
    MakeKeylockModeEvent();

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Close the input devices
//
// Prototype:   BOOL        CDeviceEngine::Close();
//
// Inputs:      nothing
//
// Outputs:     BOOL        bResult;    - states
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::Close()
{
    // not opened ?
    if (! m_bOpened)
    {
        return FALSE;                       // exit ...
    }

	// Removed MSR defince close and release of resources.
	//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers

    // free the scanner device ?
    if (m_pDeviceScanner1 != NULL)
    {
        if ((CDeviceSS*)m_pDeviceScale == (CDeviceSS*)m_pDeviceScanner1)
            m_pDeviceScale = NULL;
        m_pDeviceScanner1->Close();
        delete m_pDeviceScanner1;
        m_pDeviceScanner1 = NULL;
    }

    if (m_pDeviceScanner2 != NULL)
    {
        if ((CDeviceSS*)m_pDeviceScale == (CDeviceSS*)m_pDeviceScanner2)
            m_pDeviceScale = NULL;
        m_pDeviceScanner2->Close();
        delete m_pDeviceScanner2;
        m_pDeviceScanner2 = NULL;
    }

    // free the scale device ?
    if (m_pDeviceScale)
    {
        m_pDeviceScale->Close();
        delete m_pDeviceScale;
        m_pDeviceScale = NULL;
    }

    // free the cash drawer device
    if (m_pOPOSControl && m_pOPOSControl->m_hWnd) {
		try {
			if (m_pOPOSControl->m_cashDrawer.GetState() != OPOS_S_CLOSED)
			{
				m_pOPOSControl->m_cashDrawer.SetDeviceEnabled(FALSE);
				m_pOPOSControl->m_cashDrawer.ReleaseDevice();
				m_pOPOSControl->m_cashDrawer.Close();  // Commented out as it appears to result in an Access Violation in MFC42.dll for some reason.
			}
		}
		catch ( ... ) {
			TRACE2("%S(%d): Exception Cashdrawer 1 caught.  Continuing.\n", __FILE__, __LINE__);
		}

		try {
			if (m_pOPOSControl->m_cashDrawer2.GetState() != OPOS_S_CLOSED)
			{
				m_pOPOSControl->m_cashDrawer2.SetDeviceEnabled(FALSE);
				m_pOPOSControl->m_cashDrawer2.ReleaseDevice();
				m_pOPOSControl->m_cashDrawer2.Close();  // Commented out as it appears to result in an Access Violation in MFC42.dll for some reason.
			}
		}
		catch ( ... ) {
			TRACE2("%S(%d): Exception Cashdrawer 2 caught.  Continuing.\n", __FILE__, __LINE__);
		}

		if (m_pOPOSControl->m_lineDisplay.GetState() != OPOS_S_CLOSED)
		{
			m_pOPOSControl->m_lineDisplay.SetDeviceEnabled(FALSE);
			m_pOPOSControl->m_lineDisplay.ReleaseDevice();
			m_pOPOSControl->m_lineDisplay.Close();
		}

		if (m_pOPOSControl->m_OPOSPrinter.GetState() != OPOS_S_CLOSED)
		{
			m_pOPOSControl->m_OPOSPrinter.SetDeviceEnabled(FALSE);
			m_pOPOSControl->m_OPOSPrinter.ReleaseDevice();
			m_pOPOSControl->m_OPOSPrinter.Close();
		}

		if (m_pOPOSControl->m_OPOSCoin.GetState() != OPOS_S_CLOSED)
		{
			m_pOPOSControl->m_OPOSCoin.SetDeviceEnabled(FALSE);
			m_pOPOSControl->m_OPOSCoin.ReleaseDevice();
			m_pOPOSControl->m_OPOSCoin.Close();
		}
	}

    // mark as closed
    m_bOpened = FALSE;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    call back function
//
// Prototype:   BOOL        CDeviceEngine::BlInterface(dwCommand, pvData, dwLength);
//
// Inputs:      DWORD       dwCommand;  -
//              VOID*       pvData;     -
//              DWORD       dwLength;   -
//
// Outputs:     BOOL        bResult;    -
//
/////////////////////////////////////////////////////////////////////////////

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS ) {
		return NULL;
	}

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) * bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

BOOL CDeviceEngine::BlInterface(DWORD dwCommand, VOID* pvData, DWORD dwLength)
{
    BOOL    bResult = 0;
	PFRAMEWORK_IO_PRINTER pData;
	LONG lRes, lExRes, lRet =0;
	SHORT  sRetryCount = 1;  // retry loop count for printer retry and failures
	static int x = 0, y = 0;
//	TCHAR singleModeStr[] = _T("ESC[2ST;5;0!v");    // removed as not used. also would ESC to be 0x1B, escape character, instead.  RJC 11/13/2020
	LONG  lPrintStation = 0;
	LONG  lPrintType = 0;
	char aszErrorBuffer[156];

    // which command ?

    switch (dwCommand)
    {
    case FRAMEWORK_IOCTL_CONTROL:
    {
        PFRAMEWORK_IOCTL_STATE  pReq;

        if (dwLength < sizeof(FRAMEWORK_IOCTL_STATE))
            return FALSE;                   // exit ...

        pReq = (PFRAMEWORK_IOCTL_STATE)pvData;

        switch (pReq->dwDevice)
        {
        case FRAMEWORK_KEYLOCK:
//            bResult = m_pDeviceKeylock ? m_pDeviceKeylock->Enable(pReq->bState) : FALSE;
            bResult = FALSE;
            break;

        case FRAMEWORK_DRAWER:
//            bResult = m_pDeviceDrawer ? m_pDeviceDrawer->Enable(pReq->bState) : FALSE;
            bResult = FALSE;
            break;

        case FRAMEWORK_MSR:
            bResult = FALSE;
            break;

        case FRAMEWORK_SCANNER:
            bResult = FALSE;

            if (m_pDeviceScanner1 != NULL) {
                bResult = m_pDeviceScanner1->Enable(pReq->bState);
            }

            if (m_pDeviceScanner2 != NULL) {
                bResult = m_pDeviceScanner2->Enable(pReq->bState);
            }

            break;
        case FRAMEWORK_SCALE:
            bResult = m_pDeviceScale ? m_pDeviceScale->Enable(pReq->bState) : FALSE;
            break;
        case FRAMEWORK_SERVERLOCK:
            break;
        default:
            return FALSE;                   // exit ...
        }

        if (bResult)
            pReq->bState = bResult;
        break;
    }

	case FRAMEWORK_IOCTL_VERSION_INFO:
	{
        PFRAMEWORK_IOCTL_STATE  pReq;
		CString					myVersion;

        if (dwLength < sizeof(FRAMEWORK_IOCTL_STATE))
            return FALSE;                   // exit ...

        pReq = (PFRAMEWORK_IOCTL_STATE)pvData;

        switch (pReq->dwDevice)
        {
        case FRAMEWORK_KEYLOCK:
            break;

        case FRAMEWORK_DRAWER:
			if (m_pOPOSControl && m_pOPOSControl->m_hWnd)
				myVersion = m_pOPOSControl->m_cashDrawer.GetControlObjectDescription();
            break;

        case FRAMEWORK_MSR:
            break;

        case FRAMEWORK_SCANNER:	//Not OPOS
            break;

        case FRAMEWORK_SCALE:	//Not OPOS
            break;

        case FRAMEWORK_SERVERLOCK:	//Not OPOS
            break;

		case FRAMEWORK_PRINTER:
			pData = (PFRAMEWORK_IO_PRINTER)pvData;
			lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
			lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

			if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER)) {
				myVersion = m_pOPOSControl->m_OPOSPrinter.GetControlObjectDescription();
			}
			break;

		case FRAMEWORK_CDISPLAY:
			if (m_pOPOSControl && m_pOPOSControl->m_hWnd)
				myVersion = m_pOPOSControl->m_lineDisplay.GetControlObjectDescription();
			break;

		case FRAMEWORK_COIN:
			if (m_pOPOSControl && m_pOPOSControl->m_hWnd)
				myVersion = m_pOPOSControl->m_OPOSCoin.GetControlObjectDescription();
			break;

        default:
            return FALSE;                   // exit ...
        }

		_tcsncpy (pReq->Version, myVersion, sizeof(pReq->Version)/sizeof(pReq->Version[0]));

		return TRUE;
	}

    case FRAMEWORK_IOCTL_DRAWER_OPEN:
    {
        PFRAMEWORK_IO_DRAWER_OPEN   pReq;

		bResult = FALSE;
        if (dwLength >= sizeof(FRAMEWORK_IO_DRAWER_OPEN)) {
			pReq = (PFRAMEWORK_IO_DRAWER_OPEN)pvData;
			m_myAppForMessages->PostThreadMessage (WM_APP_DEVICEENGINE_IOCTL, FRAMEWORK_IOCTL_DRAWER_OPEN, pReq->dwNumber);
			Sleep (50);
			bResult = TRUE;
		}
		break;
    }

    case FRAMEWORK_IOCTL_DRAWER_STATUS:
    {
        PFRAMEWORK_IO_DRAWER_STATUS pRes;
		BL_NOTIFYDATA   BlNotify;
		_DEVICEINPUT*   pDeviceInput;
		USHORT			ausNotify[2] = { BLFWIF_DRAWER_CLOSED, BLFWIF_DRAWER_CLOSED};

		if (dwLength < sizeof(FRAMEWORK_IO_DRAWER_STATUS))
            return FALSE;                   // exit ...

        pRes = (PFRAMEWORK_IO_DRAWER_STATUS)pvData;
        pRes->dwDrawerA = BLFWIF_DRAWER_CLOSED;
        pRes->dwDrawerB = BLFWIF_DRAWER_CLOSED;

        if(m_pOPOSControl && m_pOPOSControl->m_hWnd) {
			bResult = m_pOPOSControl->m_lCashDrawer1Status;
			if (bResult == 1)
			{
				pRes->dwDrawerA = BLFWIF_DRAWER_OPENED;
				ausNotify[0] = BLFWIF_DRAWER_OPENED;
			}
		}

        if(m_pOPOSControl && m_pOPOSControl->m_hWnd) {
			bResult = m_pOPOSControl->m_lCashDrawer2Status;
			if (bResult == 1)
			{
				pRes->dwDrawerB = BLFWIF_DRAWER_OPENED;
				ausNotify[1] = BLFWIF_DRAWER_OPENED;
			}
		}

		//we will now send the information we
		//have gathered from te opos controls and send the information
		//into the UieDrawer area.
		pDeviceInput = &BlNotify.u.Data;

		pDeviceInput->ulDeviceId = BL_DEVICE_DATA_DRAWER;
		pDeviceInput->ulLength = sizeof(USHORT) * 2;
		pDeviceInput->pvAddr = ausNotify;

		BlNotify.ulType = BL_INPUT_DATA;
		BlNotifyData(&BlNotify, NULL);

		return TRUE;
        break;
    }

	case FRAMEWORK_IOCTL_DRAWER_CLOSE:
	{
		if (m_pOPOSControl && m_pOPOSControl->m_hWnd)
		{
			try {
				if (m_pOPOSControl->m_cashDrawer.GetState() != OPOS_S_CLOSED)
				{
					m_pOPOSControl->m_cashDrawer.SetDeviceEnabled(FALSE);
					m_pOPOSControl->m_cashDrawer.ReleaseDevice();
					// WARNING:  Some types of cash drawer provisioning require this Close()
					//           and for others it can cause an application crash with an
					//           Access Violation in the MFC42.dll.  With the setup that US Customs
					//           is using, this Close() is required.  If it is not done then
					//           an AC999 to shutdown the terminal will hang and CPU utilization will
					//           jump between 95% and 98%.  It appears that the Cash Drawer Service Object
					//           refuses to terminate gracefully unless a Close() is done on the OPOS object.
					m_pOPOSControl->m_cashDrawer.Close();
				}
			}
			catch ( ...) {
				TRACE2("%S(%d): Exception Cashdrawer 1 caught.  Continuing.\n", __FILE__, __LINE__);
			}

			try {
				if (m_pOPOSControl->m_cashDrawer2.GetState() != OPOS_S_CLOSED)
				{
					m_pOPOSControl->m_cashDrawer2.SetDeviceEnabled(FALSE);
					m_pOPOSControl->m_cashDrawer2.ReleaseDevice();
					m_pOPOSControl->m_cashDrawer2.Close();
				}
			}
			catch ( ...) {
				TRACE2("%S(%d): Exception Cashdrawer 2 caught.  Continuing.\n", __FILE__, __LINE__);
			}
		}
		break;
	}

//#endif
    case FRAMEWORK_IOCTL_SCANNER_NOF:
    {
        PFRAMEWORK_IO_SCANNER_NOF   pReq;

        if (dwLength < sizeof(FRAMEWORK_IO_SCANNER_NOF))
            return FALSE;                   // exit ...

        pReq    = (PFRAMEWORK_IO_SCANNER_NOF)pvData;

        bResult = FALSE;
        if (m_pDeviceScanner1 != NULL) {
            bResult = m_pDeviceScanner1->NotOnFile(pReq->bState);
        }

        if (m_pDeviceScanner2 != NULL) {
            bResult = m_pDeviceScanner2->NotOnFile(pReq->bState);
        }

        if (bResult)
            pReq->bState = bResult;
        break;
    }

    case FRAMEWORK_IOCTL_SCALE_READWEIGHT:
    {
		DEVICEIO_SCALE_DATA         Data = {0};
        PFRAMEWORK_IO_SCALE_DATA    pRes = (PFRAMEWORK_IO_SCALE_DATA)pvData;

        if (dwLength < sizeof(FRAMEWORK_IO_SCALE_DATA))
            return FALSE;                   // exit ...

		Data.sStatus = UIE_SCALE_NOT_PROVIDE;    // initialize to scale not provided then see if this should change.
		bResult = TRUE;

        if (m_pDeviceScale) m_pDeviceScale->ReadWeight(&Data);

		pRes->sStatus = Data.sStatus;
        pRes->uchDec  = Data.uchDec;
        pRes->uchUnit = Data.uchUnit;
        pRes->ulData  = Data.ulData;
        break;
    }

	case FRAMEWORK_IO_LINEDISPLAY_WRITE:
	{
        PFRAMEWORK_IO_DISPLAY_DATA   pData;

	    if (dwLength < sizeof(FRAMEWORK_IO_DISPLAY_DATA))
            return FALSE;                   // exit ...

        pData    = (PFRAMEWORK_IO_DISPLAY_DATA)pvData;
        if(m_pOPOSControl){
			if ((m_pOPOSControl->m_ulLineDisplayFlags & LINEDISPLAY_FLAG_SCROLLING) == 0) {
				if(m_pOPOSControl->m_lineDisplay.DisplayTextAt(pData->nRow, pData->nColumn, pData->lpuchBuffer, DISP_DT_NORMAL) == OPOS_SUCCESS)
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}
			}
			else {
				if(m_pOPOSControl->m_lineDisplay.DisplayText(pData->lpuchBuffer, DISP_DT_NORMAL) == OPOS_SUCCESS)
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}
			}
		}
        break;
    }

	case FRAMEWORK_IO_LINEDISPLAY_GETSO:
	{
		PFRAMEWORK_IO_DISPLAY_DATA pData;
		CString myString;

		if(dwLength < sizeof(FRAMEWORK_IO_DISPLAY_DATA))
			return FALSE;

		pData = (PFRAMEWORK_IO_DISPLAY_DATA)pvData;

		pData->nColumn = 20;
		pData->nRow  = 4;
		if (m_pOPOSControl && m_pOPOSControl->m_hWnd) {
			pData->nColumn = m_pOPOSControl->m_lineDisplay.GetDeviceColumns();
			pData->nRow  = m_pOPOSControl->m_lineDisplay.GetDeviceRows();
		}

#if 0
		myString = m_pOPOSControl->m_lineDisplay.GetDeviceName();
		memcpy(pData->tchDeviceDescription, myString.GetBuffer(myString.GetLength()), (myString.GetLength() * sizeof(TCHAR)));
#else
		if (m_pOPOSControl && m_pOPOSControl->m_lineDisplay.m_bUnicodeSupport) {
			_tcscpy (pData->tchDeviceDescription, _T("Unicode"));
		}
		else {
			_tcscpy (pData->tchDeviceDescription, _T("LineDisplayAPA"));
		}
#endif
	}
		break;

	case FRAMEWORK_IOCTL_PRINTER_TYPE:
		{
			pData = (PFRAMEWORK_IO_PRINTER)pvData;

			lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
			lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);
			pData->ptchBuffer = &uchPrintType;

			bResult = TRUE;
		}
		break;

	case FRAMEWORK_IOCTL_PRINTER_TRANS_START:
		{
			BOOL	bAsyncMode = true;

			pData = (PFRAMEWORK_IO_PRINTER)pvData;
			lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
			lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

			//Just in case load the newest Logo Information
			this->PrinterLoadLogoIndexInformation();

			if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
			{
				if(BlFwIfMDCCheck(MDC_DBLSIDEPRINT_ADR,ODD_MDC_BIT3))
				{
					//Begin transaction printing, this is used for double sided printers,
					m_pOPOSControl->m_OPOSPrinter.TransactionPrint(PTR_S_RECEIPT,PTR_TP_TRANSACTION);
					m_pOPOSControl->m_OPOSPrinter.SetAsyncMode(bAsyncMode);

					if(lPrintStation == PRT_REGULARPRINTING && (pData->lColumns > 0))
					{
						this->PrinterChangePrintSide(PRT_DBLSIDE_BACK);
						printingLogoType = (USHORT)this->PrinterPrintLogo(pData->lColumns, PRT_DBLSIDE_BACK);
						this->PrinterChangePrintSide(PRT_DBLSIDE_FRONT);
					}
				}
				bResult = TRUE;
			}
			else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
			{
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_TRANS_START: PMG_COM_PRINTER transaction start needed.");
			}
			else {
				// if not a recognized printer then break out of the loop.
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_TRANS_START: Unrecognized type of printer.");
				bResult = FALSE;
			}
		}
		break;

	case FRAMEWORK_IOCTL_PRINTER_TRANS_END:
	{
		LONG resultCode;
		INT bmpHeight = 0;
		TCHAR tcsCommandString[] = _T("\x1B|%dlF");
		TCHAR myString[10];
		this->PrinterLoadLogoIndexInformation();
		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		memset(&myString, 0, sizeof(myString));

		if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
		{
			if(BlFwIfMDCCheck(MDC_DBLSIDEPRINT_ADR,ODD_MDC_BIT3))
			{

				//  This is a call to a function that looks into the header information
				//  for the bitmap that is being displayed last, and retrieves the height
				//  of the image.  From that a calculation is made for a percentage of the
				//  total height, and extra padding is added to the end of the receipt
				//  to allign the image correctly.   ***PDINU
				if (((bmpHeight = this->LoadImageInformation(printingLogoType)) != 0) && (lPrintStation == PRT_REGULARPRINTING) )
				{
					wsprintf(myString, tcsCommandString, ((bmpHeight*2) / 100));
					m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT, myString);
				}

				m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT, _T("\x1B|75fP")); //Receipt Paper cut 75% see Opos Help Doc

				resultCode = m_pOPOSControl->m_OPOSPrinter.TransactionPrint(PTR_S_RECEIPT,PTR_TP_NORMAL	);

				if(resultCode == OPOS_SUCCESS)
				{
					if(lPrintStation == PRT_REGULARPRINTING)
					{
						this->PrinterPrintLogo(pData->lColumns, PRT_DBLSIDE_FRONT);
						m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT, _T("\x1B|75fP")); //Receipt Paper cut 75% see Opos Help Doc
					}
				}else
				{
					PrinterCheckStatus (pData, resultCode);
				}
			}else
			{
				m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT, _T("\x1B|75fP")); //Receipt Paper cut 75% see Opos Help Doc
			}
		}
		else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
		{
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_TRANS_END: PMG_COM_PRINTER transaction end needed.");
		}
		else {
			// if not a recognized printer then break out of the loop.
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_TRANS_END: Unrecognized type of printer.");
			bResult = FALSE;
		}
		break;
	}

	case FRAMEWORK_IOCTL_PRINTER_INIT_LOADLOGO:
		this->PrinterLoadLogoIndexInformation();
		break;

	case FRAMEWORK_IOCTL_PRINTER_WRITE:
		bResult = DevicePrinterWrite (pvData, sizeof(FRAMEWORK_IO_PRINTER));
		break;

	case FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS:
        if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
            return FALSE;                   // exit ...

		bResult = DevicePrinterColumnStatus (pvData, sizeof(FRAMEWORK_IO_PRINTER));
		break;

	case FRAMEWORK_IOCTL_PRINTER_WAIT:
		{
			pData = (PFRAMEWORK_IO_PRINTER)pvData;
			lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
			lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

			if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
			{
				m_pOPOSControl->m_OPOSPrinter.SetFlagWhenIdle(TRUE);

				while(m_pOPOSControl->m_lPrinter1Status != PTR_SUE_IDLE)
				{
					PifSleep(500);
				}
			}
			else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
			{
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_WAIT: PMG_COM_PRINTER needed.");
			}
			else {
				// if not a recognized printer then break out of the loop.
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_WAIT: Unrecognized type of printer.");
				bResult = FALSE;
			}
		}
		break;

	case FRAMEWORK_IOCTL_PRINTER_BEG_INS:
		if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
			return FALSE;                   // exit ...

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
		{
			do
			{
				lRes = m_pOPOSControl->m_OPOSPrinter.BeginInsertion(pData->lTimeout);
				if(lRes == OPOS_E_TIMEOUT)
				{
					pData->dwErrorCode = DEVICEIO_PRINTER_MSG_VSLPINS_ADR;
					if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
 						BlFwPmgCallBack(pData->dwErrorCode, 0);
					}
					Sleep(50);
				}
			}while(lRes == OPOS_E_TIMEOUT);
		}
		else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
		{
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_BEG_INS: PMG_COM_PRINTER needed.");
		}
		else {
			// if not a recognized printer then break out of the loop.
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_BEG_INS: Unrecognized type of printer.");
			bResult = FALSE;
		}
		break;

	case FRAMEWORK_IOCTL_PRINTER_END_INS:
		if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
			return FALSE;                   // exit ...

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		do
		{
			if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
			{
				lRes = m_pOPOSControl->m_OPOSPrinter.EndInsertion();
				if(lRes == OPOS_SUCCESS)
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
					lRes = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
					if(lRes == OPOS_E_EXTENDED)
					{
						lExRes = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
						if(lExRes == OPOS_EPTR_SLP_EMPTY)
						{
							pData->dwErrorCode = DEVICEIO_PRINTER_MSG_VSLPINS_ADR;
						}
						else if(lExRes == OPOS_EPTR_COVER_OPEN)
						{
							pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
							lRes = 0;//reset the error to break out of loop
							//because printer no longer in insertion mode after
							//cover is opened
						}
						else
						{
							pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
						}
						if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
							BlFwPmgCallBack(pData->dwErrorCode, 0);
						}
						m_pOPOSControl->m_OPOSPrinter.BeginInsertion(2000);//start insertion again no slip was found
					}
				}
			}
			else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
			{
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_END_INS: PMG_COM_PRINTER needed.");
			}
			else {
				// if not a recognized printer then break out of the loop.
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_END_INS: Unrecognized type of printer.");
				bResult = FALSE;
				break;
			}
			Sleep(50);
			sRetryCount--;
		} while((sRetryCount > 0) && lRes == OPOS_E_EXTENDED);
		break;

	case FRAMEWORK_IOCTL_PRINTER_BEG_REM:
		if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
			return FALSE;                   // exit ...

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
		{
			lRet = m_pOPOSControl->m_OPOSPrinter.BeginRemoval(pData->lTimeout);
			if( lRet == OPOS_E_TIMEOUT)
			{
				NHPOS_ASSERT_TEXT(0,"PRINTER RELEASE SLIP TIMEOUT");
			}
		}
		else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
		{
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_BEG_REM: PMG_COM_PRINTER needed.");
		}
		else {
			// if not a recognized type of printer then break out.of loop.
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_BEG_REM: Unrecognized type of printer.");
			bResult = FALSE;
			break;
		}
		break;

	case FRAMEWORK_IOCTL_PRINTER_END_REM:
		if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
			return FALSE;                   // exit ...

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		do
		{
			if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
			{
				lRes = m_pOPOSControl->m_OPOSPrinter.EndRemoval();
				if(lRes == OPOS_SUCCESS)
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
					lRes = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
					if(lRes == OPOS_E_EXTENDED)
					{
						lExRes = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
						if(lExRes == OPOS_EPTR_SLP_FORM)
						{
							pData->dwErrorCode = DEVICEIO_PRINTER_MSG_VSLPREMV_ADR;
						}
						else
						{
							pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
						}
					}
				}
			}
			else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
			{
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_END_REM: PMG_COM_PRINTER needed.");
			}
			else {
				// if not a recognized type of printer then break out.of loop.
				NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_END_REM: Unrecognized type of printer.");
				bResult = FALSE;
				break;
			}
			Sleep(50);
			sRetryCount--;
		}while((sRetryCount > 0) && lRes == OPOS_E_EXTENDED);
		break;

	case FRAMEWORK_IOCTL_COIN_DISPENSE:

		PFRAMEWORK_IO_COIN_DATA pCoinData;
		LONG	lRet;

		if(dwLength < sizeof(FRAMEWORK_IO_COIN_DATA))
			return FALSE;

		pCoinData = (PFRAMEWORK_IO_COIN_DATA)pvData;
		{
			lRet = m_pOPOSControl->m_OPOSCoin.DispenseChange(pCoinData->lDispenseAmount);
			if(lRet == OPOS_SUCCESS)
			{
				pCoinData->sReturnCode = UIE_SUCCESS;
				return TRUE;
			}
			else if(lRet == OPOS_E_ILLEGAL)
			{
				pCoinData->sReturnCode = UIE_COIN_ERROR_RANGE;
				return FALSE;
			}
			else
			{
				pCoinData->sReturnCode = UIE_COIN_ERROR_COM;
				return FALSE;
			}
		}
		break;

	case FRAMEWORK_IOCTL_PRINTER_FONT: //ESMITH EJ FONT
	{
		NHPOS_ASSERT(dwLength == sizeof(FRAMEWORK_IO_PRINTER));
		CString strLineCharList;
		CArray<long, long>	lColumnArray;
		int nPos = 0, nFit = 0;

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl) {
			bResult = TRUE;
			pData->bSlipStationExist = m_pOPOSControl->m_OPOSPrinter.GetCapSlpPresent();
			switch(lPrintStation){
			case PTR_S_RECEIPT:
				strLineCharList= m_pOPOSControl->m_OPOSPrinter.GetRecLineCharsList();
				break;
			case PTR_S_JOURNAL:
				strLineCharList = m_pOPOSControl->m_OPOSPrinter.GetJrnLineCharsList();
				break;
			case PTR_S_SLIP:
				strLineCharList = m_pOPOSControl->m_OPOSPrinter.GetSlpLineCharsList();
				break;
			default:
				bResult = FALSE;
				return bResult;
				break;
			}

			if(!strLineCharList.IsEmpty())
			{
				switch (pData->usFontType){
				case PRT_FONT_A:
					nPos = strLineCharList.Find(_T(','));

					if(nPos != -1)
					{
						pData->lColumns = _ttol(strLineCharList.Left(nPos));
					}
					else
					{
						bResult = FALSE;
						return bResult;
					}
					break;

				case PRT_FONT_B:
					if(strLineCharList.Find(PMG_THERMAL_CHAR_B) != -1) //change back to B
					{
						pData->lColumns = PMG_THERMAL_CHAR_B;
					}
					else
					{
						/* ===============================================
							Find the value closest to PMG_THERMAL_CHAR_B
						   =============================================== */
						while( (nPos = strLineCharList.Find(_T(','))) != -1)
						{
							lColumnArray.Add(_ttol(strLineCharList.Left(nPos)));
							strLineCharList.Delete (0, nPos);
							strLineCharList.TrimLeft(_T(','));

						}

						lColumnArray.Add(_ttol(strLineCharList));

						for(int i=0; i < lColumnArray.GetSize(); i++)
						{
							if(PMG_THERMAL_CHAR_B > lColumnArray[i])
								nFit++;
						}

						if(nFit > 0)
						{
							if(nFit >= lColumnArray.GetSize())
							{
								pData->lColumns = lColumnArray[nFit-1];
							}
							else
							{
								pData->lColumns = lColumnArray[nFit];
							}
						}
						else
						{
							pData->lColumns = lColumnArray[0];
						}
					}
					break;

				default:
					bResult = FALSE;
					return bResult;
					break;
				}

				switch(lPrintStation){
				case PTR_S_RECEIPT:
					m_pOPOSControl->m_OPOSPrinter.SetRecLineChars(pData->lColumns);
					break;
				case PTR_S_JOURNAL:
					m_pOPOSControl->m_OPOSPrinter.SetJrnLineChars(pData->lColumns);
					break;
				case PTR_S_SLIP:
					m_pOPOSControl->m_OPOSPrinter.SetSlpLineChars(pData->lColumns);
					break;
				default:
					bResult = FALSE;
					return bResult;
					break;
				}
			}
			else
			{
				bResult = FALSE;
				return bResult;
			}
		}
		else if( (lPrintType == 0 || lPrintType == PMG_ZEBRA_PRINTER) && m_pZebraControl && m_bZebraControlDispatch) {
			bResult = TRUE;
			return bResult;
		}
		else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
		{
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_FONT: PMG_COM_PRINTER needed.");
		}
		else {
			// if not a recognized type of printer then break out.of loop.
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_FONT: Unrecognized type of printer.");
			break;
		}
		break;
	}

	case FRAMEWORK_IOCTL_PRINTER_GETSETCAP: // LONG  PmgGetSetCap( USHORT usPrtType, USHORT usPrtCap, LONG lValue)
	{
		NHPOS_ASSERT(dwLength == sizeof(FRAMEWORK_IO_PRINTER));
		int nPos = 0, nFit = 0;

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl) {
			int     iStation;
			USHORT  usPrtCap = pData->usFontType;
			LONG    lValue = pData->lColumns;
			struct  {
				LONG    lPrintStation;
				USHORT  usPrtCap;
				LONG  (COPOSPOSPrinter::*pPrtCapFuncGet) ();
				VOID  (COPOSPOSPrinter::*pPrtCapFuncSet) (long);
			} MyStationCapArray[] = {
				{PTR_S_RECEIPT, PRT_CAP_LINE_SPACING, &(COPOSPOSPrinter::GetRecLineSpacing), &(COPOSPOSPrinter::SetRecLineSpacing)},
				{PTR_S_RECEIPT, PRT_CAP_LINE_HEIGHT, &(COPOSPOSPrinter::GetRecLineHeight), &(COPOSPOSPrinter::SetRecLineHeight)},
				{PTR_S_SLIP, PRT_CAP_LINE_SPACING, &(COPOSPOSPrinter::GetSlpLineSpacing), &(COPOSPOSPrinter::SetSlpLineSpacing)},
				{PTR_S_SLIP, PRT_CAP_LINE_HEIGHT, &(COPOSPOSPrinter::GetSlpLineHeight), &(COPOSPOSPrinter::SetSlpLineHeight)},
				{0, 0, 0}
			};

			bResult = TRUE;
			pData->bSlipStationExist = m_pOPOSControl->m_OPOSPrinter.GetCapSlpPresent();
			for (iStation = 0; MyStationCapArray[iStation].pPrtCapFuncGet; iStation++) {
				if (MyStationCapArray[iStation].lPrintStation == lPrintStation && MyStationCapArray[iStation].usPrtCap == (usPrtCap & PRT_CAP_MASK)) {
					pData->lTimeout = (m_pOPOSControl->m_OPOSPrinter.*(MyStationCapArray[iStation].pPrtCapFuncGet))();
					if ((usPrtCap & PRT_CAP_GET_ONLY) == 0) {
						(m_pOPOSControl->m_OPOSPrinter.*(MyStationCapArray[iStation].pPrtCapFuncSet))(lValue);
					}
					break;
				}
			}

			if (MyStationCapArray[iStation].pPrtCapFuncGet == 0) {
				bResult = FALSE;
				return bResult;
			}
		}
		else if( (lPrintType == 0 || lPrintType == PMG_ZEBRA_PRINTER) && m_pZebraControl && m_bZebraControlDispatch) {
			bResult = TRUE;
			return bResult;
		}
		else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
		{
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_GETSETCAP: PMG_COM_PRINTER needed.");
		}
		else {
			// if not a recognized type of printer then break out.of loop.
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_GETSETCAP: Unrecognized type of printer.");
			break;
		}
		break;
	}

	case FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE: // LONG  PmgGetSetCap( USHORT usPrtType, USHORT usPrtCap, LONG lValue)
	{
		NHPOS_ASSERT(dwLength == sizeof(FRAMEWORK_IO_PRINTER));
		int nPos = 0, nFit = 0;

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		pData->fbStatus = 0;
		pData->m_ulPrinter1StatusMap = 0;

		if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl) {
			LONG    lLineWidth = 2, lLineHeight = 40, lRetValue;
			LONG    lAlignment, lTextPosition;

			bResult = TRUE;
			pData->bSlipStationExist = m_pOPOSControl->m_OPOSPrinter.GetCapSlpPresent();

			SetdwErrorCode (pData);
			lRetValue = m_pOPOSControl->m_OPOSPrinter.GetCapRecBarCode();
			switch(lPrintStation){
				case PTR_S_RECEIPT:
					lLineWidth = m_pOPOSControl->m_OPOSPrinter.GetRecLineWidth();
					lLineHeight = m_pOPOSControl->m_OPOSPrinter.GetRecLineHeight();
					lRetValue = m_pOPOSControl->m_OPOSPrinter.GetCapRecBarCode();
					if (lLineWidth < 2) {
						lLineWidth = 2;
					}
					if (lLineHeight < 1) {
						lLineHeight = 40;
					}
					break;
				case PTR_S_SLIP:
					lLineWidth = m_pOPOSControl->m_OPOSPrinter.GetSlpLineWidth();
					lLineHeight = m_pOPOSControl->m_OPOSPrinter.GetSlpLineHeight();
					lRetValue = m_pOPOSControl->m_OPOSPrinter.GetCapSlpBarCode();
					break;
				default:
					bResult = FALSE;
					lRetValue = 0;      // indicate that bar code is not supported for this printer
					break;
			}

			if (lRetValue) {
				LONG   lSymbology = 0;
				TCHAR  *ptchBuffer = pData->ptchBuffer;
				TCHAR   tchTempBuffer[64];

				switch (pData->ulSymbology & 0x0000ffff) {
					case PRT_BARCODE_CODE_CODE128C:
						lSymbology = PTR_BCS_Code128_Parsed;
						{
							int    iCount = 0;
							TCHAR  *ptchOrig = pData->ptchBuffer;
							TCHAR  *ptchNew = tchTempBuffer;
							*ptchNew = 105;   // indicate starting Code128C
							ptchNew++;
							int iLen = _tcslen(ptchOrig);
							ptchNew = tchTempBuffer;
							*ptchNew = '{';  ptchNew++;
							*ptchNew = 'C';  ptchNew++;
							if ((iLen & 1) != 0) {
								// odd number of characters so we will need to prepend a zero to the string
								*ptchNew = '0';  ptchNew++;
							}
							for (; *ptchOrig; ptchOrig++) {
								if (*ptchOrig >= '0' && *ptchOrig <= '9') {
									*ptchNew = *ptchOrig;  ptchNew++;
								}
							}
							*ptchNew = 0;
						}
						ptchBuffer = tchTempBuffer;
						break;

					case PRT_BARCODE_CODE_CODE128:
						lSymbology = PTR_BCS_Code128;
						// we need to convert from ASCII to the Code128 character set
						// see http://www.idautomation.com/code128faq.html
						// see http://en.wikipedia.org/wiki/Code_128
						{
							int    iCount = 0;
							TCHAR  *ptchOrig = pData->ptchBuffer;
							TCHAR  *ptchNew = tchTempBuffer;
							*ptchNew = 103;   // indicate starting Code128A which is our default
							ptchNew++;
							for (; *ptchOrig; ptchOrig++) {
								if (*ptchOrig < 95) {
									iCount++;
									if (*ptchOrig >= 32)
										*ptchNew = *ptchOrig - 32;
									else
										*ptchNew = *ptchOrig + 64;
									ptchNew++;
								}
							}
							*ptchNew = 0;
						}
						ptchBuffer = tchTempBuffer;
						break;

					case PRT_BARCODE_CODE_CODE39:
						lSymbology = PTR_BCS_Code39;
						break;

					case PRT_BARCODE_CODE_CODEUPCA:
						lSymbology = PTR_BCS_UPCA;
						break;

					case PRT_BARCODE_CODE_CODEEAN13:
						lSymbology = PTR_BCS_EAN13;
						break;

					case PRT_BARCODE_CODE_CODEEAN128:
						lSymbology = PTR_BCS_EAN128;
						break;

					default:
						lSymbology = PTR_BCS_Code39;
						break;
				}

				switch (pData->ulAlignment & FRAMEWORK_IO_PRINTER_TEXT_MASK_LCR) {
					case FRAMEWORK_IO_PRINTER_TEXT_LEFT:
						lAlignment = PTR_BC_LEFT;
						break;
					case FRAMEWORK_IO_PRINTER_TEXT_CENTER:
						lAlignment = PTR_BC_CENTER;
						break;
					case FRAMEWORK_IO_PRINTER_TEXT_RIGHT:
						lAlignment = PTR_BC_RIGHT;
						break;
					default:
						lAlignment = PTR_BC_LEFT;
						break;
				}

				switch (pData->ulAlignment & FRAMEWORK_IO_PRINTER_TEXT_MASK_NAB) {
					case FRAMEWORK_IO_PRINTER_TEXT_NONE:
						lTextPosition = PTR_BC_TEXT_NONE;
						break;
					case FRAMEWORK_IO_PRINTER_TEXT_ABOVE:
						lTextPosition = PTR_BC_TEXT_ABOVE;
						break;
					case FRAMEWORK_IO_PRINTER_TEXT_BELOW:
						lTextPosition = PTR_BC_TEXT_BELOW;
						break;
					default:
						lTextPosition = PTR_BC_TEXT_BELOW;
						break;
				}

				// The printer supports bar code printing.
				lRetValue = m_pOPOSControl->m_OPOSPrinter.PrintBarCode(lPrintStation, ptchBuffer, lSymbology, lLineHeight, lLineWidth, lAlignment, lTextPosition);
				bResult = TRUE;
				if (lRetValue != 0) {
					char  xBuff[128];

					sprintf(xBuff, "PrintBarCode sta = %d  status = %d  print %S", lPrintStation, lRetValue, pData->ptchBuffer);
					NHPOS_NONASSERT_TEXT(xBuff);
					sprintf(xBuff, "        lSymbology = %d  lLineHeight = %d  lLineWidth %d  lAlignment %d", lSymbology, lLineHeight, lLineWidth, lAlignment);
					NHPOS_NONASSERT_TEXT(xBuff);
				}
			}
			if (pData->fbStatus & PRT_STAT_HW_ERROR) {
				if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
					BlFwPmgCallBack(pData->dwErrorCode, 0);
				}
			}
		}
		else if( (lPrintType == 0 || lPrintType == PMG_ZEBRA_PRINTER) && m_pZebraControl && m_bZebraControlDispatch) {
			bResult = TRUE;
		}
		else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
		{
			// how do we print a bar code with direct to printer?
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE: PMG_COM_PRINTER needs barcode print.");
		}
		else {
			// if not a recognized type of printer then break out.of loop.
			NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_PRINTBARCODE: Unrecognized type of printer.");
			break;
		}
		break;
	}

	case FRAMEWORK_IOCTL_PRINTER_STATUS:
		bResult = DevicePrinterStatus (pvData, sizeof(FRAMEWORK_IO_PRINTER));
		break;
#if 0
			m_myAppForMessages->PostThreadMessage (WM_APP_DEVICEENGINE_IOCTL, FRAMEWORK_IOCTL_PRINTER_STATUS, (LPARAM)pvData);
			bResult = TRUE;
			break;
#endif


//windows drivers
	case FRAMEWORK_IOCTL_WINPRINT_WRITE:
	{
		char aszErrorBuffer[156] = { 0 };
		int iError;

		if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
            return FALSE;

		pData = (PFRAMEWORK_IO_PRINTER)pvData;
		lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
		lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

		pData->dwErrorCode = 0;

		iError = dcPrinter.PrintLine(pData->ptchBuffer, pData->ulLength);
		if (iError < 0)
		{
			sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_WRITE  Windows Driver Error:: Failed PrintLine() %d m_bStartDoc = %d ", iError, m_bStartDoc);
			NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
		}

	  //save the original rectangle before we
	  //perform all sorts of header/footer adjustments
	/*
		CFont myNewFont, myOldFont;
		CFont *pOldFont;

		memset(&myNewFont, 0x00, sizeof(myNewFont));
		memset(&myOldFont, 0x00, sizeof(myOldFont));

		pOldFont = dcPrinter.GetCurrentFont();


		myNewFont.CreateFont((myTextMetric.tmHeight * 1.5),
													0,                         // nWidth
													0,                         // nEscapement
													0,                         // nOrientation
													FW_NORMAL,                 // nWeight
													FALSE,                     // bItalic
													FALSE,                     // bUnderline
													0,                          // strike out
													ANSI_CHARSET,               // character set
													OUT_STRING_PRECIS,          // out precision
													CLIP_DEFAULT_PRECIS,        // clip precision
													DEFAULT_QUALITY,            // quality
													FIXED_PITCH | FF_DONTCARE,  // pitch and family
													_T("Courier New"));         // face name

		dcPrinter.SelectObject(myNewFont);
		dcPrinter.TextOut(x,y,pData->ptchBuffer, pData->ulLength);
		horRez = dcPrinter.GetDeviceCaps(HORZRES);
		dcPrinter.GetTextMetrics(&myTextMetric);
		y += (myTextMetric.tmHeight + 5);
		dcPrinter.SelectObject(pOldFont);*/

		break;
	}

	case FRAMEWORK_IOCTL_WINPRINT_START:
		{
			int iError;
#if 1
			// Start a page.
			if ((iError = dcPrinter.StartPage()) <= 0)
			{
				sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_START  Windows Driver Error:: Failed StartPage() %d", iError);
				NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
				bResult = FALSE;
				iError = dcPrinter.AbortDoc();
				if (iError <= 0)
				{
					sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_START  Windows Driver Error:: Failed AbortDoc() %d", iError);
					NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
				}
			}
			else {
				bResult = TRUE;
			}
#else
			DOCINFO docinfo = { 0 };

			docinfo.cbSize = sizeof(docinfo);
			docinfo.lpszDocName = _T("GenPOS: Printing");

		    // If it fails, complain and exit gracefully.
		    if ((iError = dcPrinter.StartDoc(&docinfo)) <= 0)
			{
		        sprintf (aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_START  Windows Driver Error:: Failed StartDoc() %d", iError);
				NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
				bResult = FALSE;
			}
			else
			{
				// Start a page.
				if ((iError = dcPrinter.StartPage()) <= 0)
				{
					sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_START  Windows Driver Error:: Failed StartPage() %d", iError);
					NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
					bResult = FALSE;
					iError = dcPrinter.AbortDoc();
					if (iError <= 0)
					{
						sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_START  Windows Driver Error:: Failed AbortDoc() %d", iError);
						NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
					}
				}
				else {
					bResult = TRUE;
				}
			}
#endif
			m_bStartDoc = bResult;
		}
		break;

	case FRAMEWORK_IOCTL_WINPRINT_END:
		{
			m_bStartDoc = FALSE;
			int iError = dcPrinter.EndPage();
			if (iError < 0)
			{
				sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_END  Windows Driver Error:: Failed EndPage() %d", iError);
				NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
			}
			iError = dcPrinter.EndDoc();
			if (iError < 0)
			{
				sprintf(aszErrorBuffer, "DeviceEngine.cpp: FRAMEWORK_IOCTL_WINPRINT_END  Windows Driver Error:: Failed EndDoc() %d", iError);
				NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
			}
		}
		break;

	case FRAMEWORK_IOCTL_COM_MSR_READ:
		bResult = 0;
		break;

	case FRAMEWORK_IOCTL_COM_VIRTUAL_KEYBOARD:
		{
			PFRAMEWORK_IO_VIRTUALKEY_DATA    pRes;

			pRes = (PFRAMEWORK_IO_VIRTUALKEY_DATA)pvData;
#if 0
			if (pRes->usOptions == FRAMEWORK_IO_VIRTUALKEY_READ_ENABLE) {
				m_nConvertKeyStateMachine = 0;  // Reset the state machine in case some stuff has been partially processed.
				m_bVirtualKeyProcessingEnabled = (pRes->usOptions == FRAMEWORK_IO_VIRTUALKEY_READ_ENABLE);
			}
#endif
			m_nConvertKeyStateMachine = 0;  // Reset the state machine in case some stuff has been partially processed.

		}
		break;

//	case FRAMEWORK_IOCTL_PINPAD_READ:
//		this functionality is done in function SHORT BlFwIfReadPinPad (PBLFMSRIF pMsrTrackData)
//      in BusinessLogic and not through the DeviceEngine interface.

// Removed PinPad, Signature Capture, and MSR read as part of eliminating
// handling of account holder information in order to maintain PCI-DSS Out Of Scope
// status for OpenGenPOS.
//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers

	case FRAMEWORK_IOCTL_PINPAD_START_READ:
		bResult = 0;
		break;

	case FRAMEWORK_IOCTL_SIGNATURECAPTURE_CAPTURE:
		bResult = FALSE;
		break;

	case FRAMEWORK_IOCTL_SIGNATURECAPTURE_READ:
		bResult = FALSE;
		break;

	case FRAMEWORK_IOCTL_SIGNATURECAPTURE_CUSTOMERDISPLAY:
		bResult = 0;
		break;

	case FRAMEWORK_IOCTL_SIGNATURECAPTURE_BITMAP:
		bResult = 0;
		break;

	case FRAMEWORK_IOCTL_MULTI_MSR_READ:
		NHPOS_ASSERT_TEXT(0, "m_pZebraControl->Open() failed");
		bResult = 0;
		break;

	case FRAMEWORK_IOCTL_MULTI_MSR_EXIST:
		bResult = 0;
		break;

	case FRAMEWORK_IOCTL_LIC_DETAILS: // LONG  PmgGetSetCap( USHORT usPrtType, USHORT usPrtCap, LONG lValue)
		{
			AXDATA *pReq = (AXDATA *)pvData;

			pReq->licStatus = m_licStatus;
		}
		break;

    default:                                // unknown command ...
        return FALSE;                       // exit ...
    }

    // exit ...

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::PreTranslateMessage(pMsg);
//
// Inputs:      MSG         pMsg;       -
//
// Outputs:     BOOL        bResult;    - states
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::PreTranslateMessage(MSG* pMsg)
{
    BOOL    bResult;

    if (pMsg->wParam != VK_F24)
    {
		// keyboard data ?
        bResult = KeyboardDataMessage(pMsg);
    }
    else
    {
		// device data ?
        bResult = DeviceDataMessage(pMsg);
    }
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::KeyboardDataMessage(pMsg);
//
// Inputs:      MSG         pMsg;       -
//
// Outputs:     BOOL        bResult;    - states
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::KeyboardDataMessage(MSG* pMsg)
{
    BOOL            bValidData;
    USHORT          uchConvertCode;
    BL_NOTIFYDATA   BlNotify;
    CHARDATA        CharaData;
	VIRTUALKEY_BUFFER  VirtualKeyData;
    CString         sDebugMsg;
	static int      iBufferit = 0;
	static MSG      msgarray[50];
	 MSG      xmsgarray[50];
	int             xiBufferit = 0;

	CharaData.uchFlags = 0;

    // is not down key ?
	if (pMsg->message != WM_CHAR) {
		if (pMsg->message != WM_KEYDOWN &&
			pMsg->message != WM_SYSKEYDOWN)
		{
			return FALSE;                       // exit ...
		}
		if (m_nConvertKeyStateMachine >= 10) {
			return FALSE;
		}
	}
// remove the following else as it causes Keyboard Wedge Interface MSR to not function reliably.
//	else if (m_nConvertKeyStateMachine < 10) {
//		return FALSE;
//	}

	xiBufferit = iBufferit;
	if (xiBufferit) {
		msgarray[xiBufferit] = *pMsg;
		xiBufferit++;
		if (xiBufferit >= sizeof(msgarray)/sizeof(msgarray[0])) xiBufferit = 1;
		iBufferit = xiBufferit;
		memcpy (xmsgarray, msgarray, sizeof(xmsgarray));
	}
    // initialize
    bValidData   = FALSE;

    // Algolism for Key Number Input Mode.
    uchConvertCode = ConvertAlgorismForKeyInput(pMsg, &VirtualKeyData);

    if (uchConvertCode == DE_WEDGE_STREAM)
	{
		return TRUE;      // we are queuing up data like for a virtual scanner so just indicate we have handled this character
	} else if (uchConvertCode != DE_NOT_CONVERTED)
    {
		bValidData = TRUE;
		BlNotify.u.Data.ulDeviceId = BL_DEVICE_DATA_KEY;
		BlNotify.u.Data.ulLength   = sizeof(CharaData);
		BlNotify.u.Data.pvAddr     = &CharaData;
		// The default action is that we are a 7448 terminal so
		// we just take the message as is.  Even if we are a touch terminal
		// or a terminal using the NCR 5932 Wedge keyboard, we still need
		// to modify the CharaData.uchASCII only if its a pseudo key lock event.
        CharaData.uchASCII = uchConvertCode;
		// With the increased FSC table size for Touch screen, we need to set uchScan to 0 as
		// this value is used in the table offset calculations in function UieConvertFsc ().
        CharaData.uchScan  = 0;
#if defined(_WIN32_WCE_EMULATION) || defined(XP_PORT)
#ifdef TOUCHSCREEN
		// Lets check to see if the key code is a character keypress
		// or is it a special such as a key lock position change or other
		// data stream from a wedge type of keyboard such as the NCR 5932 Wedge 68 Key.
		// The NCR 5932 Wedge 68 key keyboard sends a sequence of characters which are then
		// turned into a single keylock event.  Since the code looks for the keylock change
		// in the uchScan (code knows something special because uchASCII is zero), we have to
		// put the uchConvertCode (the converted code which indicates position of keylock) into
		// the uchScan variable.
		if (uchConvertCode >= PIF_KEYLOCK && uchConvertCode <= PIF_KEYLOCK + 6) {
			CharaData.uchASCII = 0;
			CharaData.uchScan  = uchConvertCode;
		} else if (uchConvertCode == PIF_VIRTUALKEYEVENT) {
			// This is a virtual keyboard type of event.
			BlNotify.u.Data.ulDeviceId = BL_DEVICE_DATA_VIRTUALKEY;
			BlNotify.u.Data.ulLength   = sizeof(VirtualKeyData);
			BlNotify.u.Data.pvAddr     = &VirtualKeyData;
		}
#else
		// if we are not compiled as touchscreen then we are compiled for Win CE
		// Emulator for testing under Emulator with Visual eMbedded C++ IDE.
		// Convert a function key (F1 through F4) press into a key lock event.
       CharaData.uchASCII = (pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F4) ?
            0 : uchConvertCode;
#endif //Touchscreen
#endif//_WIN32_WCE_EMULATION || XP_PORT


#ifdef _DEBUG
        UCHAR auchASKII[2];

        auchASKII[0] = CharaData.uchASCII;
        auchASKII[1] = '\0';
        sDebugMsg.Format(
            _T("ASCII=0x%x/%S, SCAN=0x%x\n"),
            CharaData.uchASCII,
            auchASKII,
            CharaData.uchScan);
        OutputDebugString((LPCTSTR)sDebugMsg);
#endif
    }

    if (bValidData)
    {
        // notify data to business logic

        BlNotify.ulType = BL_INPUT_DATA;
        BlNotifyData(&BlNotify, NULL);

        return TRUE;                        // TRUE
    }

    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    Convert Key Code
//
// Prototype:   unsigned char CDeviceEngine::ConvertKey(pMsg);
//
// Inputs:      MSG*    pMsg;           - Message
//
// Outputs:     UCHAR   uchConvertCode;
//                      =  DE_NOT_CONVERTED : not changed
//                      != DE_NOT_CONVERTED : converted key code
//
/////////////////////////////////////////////////////////////////////////////

USHORT CDeviceEngine::ConvertKey(MSG *pMsg, VIRTUALKEY_BUFFER  *pVirtualKeyData)
{
    USHORT          uchConvertCode = DE_NOT_CONVERTED;
    PCONVERT_KEY    pConvertKey = 0;
    short           fShiftKey;
    short           fControlKey;
    short           fAltKey;
	short			fCapitalKey;
	int             nKeyboardValue = 0;
#if 0
	// The following provide some basic data stream examples for some types of
	// virtual keyboard devices.  These can be used to emulate these devices.
	static TCHAR    FreedomPayGoodSwipe[] = {
		17, 79, 49, 55, 49, 55, 48, 50, 57, 56, 188, 32, 222, 188, 222,
		69, 48, 48, 55, 48, 48, 48, 48, 49, 69,  65, 53,  70,  67,  52, 54, 17, 78, 13};
	static TCHAR    FreedomPayBadSwipe[] = {
		17, 79, 49, 55, 49, 55, 48, 50, 57, 56, 188, 32, 222, 188, 222,
		69, 48, 48, 55, 48, 48, 48, 48, 50, 52,  57, 49,  55,  51,  51, 50, 17, 78, 13};
#endif

    nKeyboardValue = PifSysConfig()->uchKeyType;                    /* get system config */
	if ((PifSysConfig()->usInterfaceDetect & SYSCONFIG_IF_EEPT_DLL1) != 0) {
		nKeyboardValue |= KEYBOARD_VIRTUAL;
	}

	if ((PifSysConfig()->usInterfaceDetect & SYSCONFIG_IF_WEDGE_ON) != 0) {
		nKeyboardValue |= KEYBOARD_VIRTUAL;
	}

	// In the case of the keyboard type being touch, then we want to allow
	// the user to use a standard keyboard to do mnemonic changes gracefully
	// so if we be touch and we have received a keyboard message, then just
	// return informing everything else that we have not done a conversion.
	// Device Engine will then let Windows know that Windows has to deal with
	// the message itself.
	// In other places, we will look for WM_CHAR messages which Windows will
	// have translated for us.
    if (nKeyboardValue == KEYBOARD_TOUCH)		 // If we be touch then just return unconverted
    {
        return (DE_NOT_CONVERTED);
    }

    // Check to see if we have received an extended keypress such as shift key.
	// Remember that we are in pre-translation so every key message will be
	// received here including shift, alt (menu), and control keypresses.

	//SR 303 Conventional 78 key keyboard
	//we allow shift, control, and menu buttons to be pressed
	//for the 78 keyboard. JHHJ
    if (((nKeyboardValue & KEYBOARD_VIRTUAL) == 0) &&
		(LOWORD(pMsg->wParam) == VK_SHIFT ||
        LOWORD(pMsg->wParam) == VK_CONTROL ||
        LOWORD(pMsg->wParam) == VK_MENU) && (nKeyboardValue != KEYBOARD_WEDGE_78))
    {
        return (DE_NOT_CONVERTED);
    }

	// Handle the case that we are the touchscreen.
	// The standard is that the wParam will indicate key change as follows
	//		0x03 - Lock mode
	//		0x04 - Register mode
	//		0x05 - Supervisor mode
	//		0x06 - Program mode
    if (((nKeyboardValue & KEYBOARD_VIRTUAL) == 0) && pMsg->wParam >= 0x03 && pMsg->wParam <= 0x06)
    {
        uchConvertCode = pMsg->wParam - 0x03 + PIF_KEYLOCK;
		return (0x03);
    }

	// Determine which key conversion table to use based on what
	// our current keyboard type is.  The keyboard type is a parameter
	// that is specified through SysConfig.

    pConvertKey = 0;
    if (nKeyboardValue == KEYBOARD_MICRO)			 // Micromotion KB (176)
    {
        pConvertKey = &aConvertKeyMicro[0];
    }
    else if (nKeyboardValue == KEYBOARD_CONVENTION)  // Conventional KB (80)
    {
        pConvertKey = &aConvertKey80[0];
    }
    else if (nKeyboardValue == KEYBOARD_WEDGE_68)    // NCR 5932 Wedge 64 key keyboard
    {
		pConvertKey = &aConvertKeyWedge68[0];
    }
    else if (nKeyboardValue == KEYBOARD_WEDGE_78)    // NCR 5932 Wedge 78 key keyboard
    {
		pConvertKey = &aConvertKeyWedge78[0];
    }
    else if (nKeyboardValue == KEYBOARD_PCSTD_102)   // Standard 102 key PC keyboard
    {
		pConvertKey = &aConvertKeyPCStd[0];
    }
	else if (nKeyboardValue == KEYBOARD_TICKET)
	{
		pConvertKey = &aConvertKeyBigTicket[0];
	}

	if (((nKeyboardValue & KEYBOARD_VIRTUAL) != 0) && m_bVirtualKeyProcessingEnabled)
	{
		// The following state machine case is the first step in processing
		// a virtual keyboard type of data stream.  Since this may actually be
		// shared by several different types of virtual keyboard devices, we
		// differentiate between them by looking at the first character of the
		// data stream.  Depending on the first character we will process the
		// data stream in this state machine for the proper device.
#if 0
		// To use the hardcoded swipe strings, change the decimal 17 to a keyboard
		// character such as a carriage return (decimal 13) as in this example:
		if (m_nConvertKeyStateMachine == 0 && LOWORD(pMsg->wParam) == 13) {
#endif
		if (m_nConvertKeyStateMachine == 0 && LOWORD(pMsg->wParam) == 17) {
			// The protocol as provided by FreedomPay is composed of
			// some 34 characters.  The first character is a decimal 17
			// and the last three characters are decimal 17, 78, and 13.
			// This means that both the first character of the data
			// stream is a 17 and a 17 is also near the end so we check
			// to see if the state machine is not started when processing
			// a character of decimal 17 to mean the beginning of a data stream.
			// KEYBOARD_VIRTUAL, VIRTUALKEY_BUFFER type of data stream to process
			m_nConvertKeyStateMachine = 10;  // this is a FreedomPay data stream, VIRTUALKEY_EVENT_FREEDOMPAY
			m_nConvertKeyStateMachineIndex = 16;
			memset(m_usConvertKeyBuffer,0,sizeof(m_usConvertKeyBuffer));
		}
		else if (m_nConvertKeyStateMachine == 0 && LOWORD(pMsg->wParam) == _T('M')) {
			// This is a Mobile Demand PC Symbol scanner attachment
			m_nConvertKeyStateMachine = 12;  // this is a scanner data stream, UIE_VIRTUALKEY_EVENT_SCANNER
			m_nConvertKeyStateMachineIndex = 16;
			memset(m_usConvertKeyBuffer,0,sizeof(m_usConvertKeyBuffer));
		}
		// Removed the Mobile Demand PC Symbol MSR attachment.
		// this is an MSR data stream, UIE_VIRTUALKEY_EVENT_MSR
		//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers
	}
	else if (m_bVirtualKeyProcessingEnabled) {
			__debugbreak();  // prep for ARM build _asm int 3;
	}

    fShiftKey   = GetKeyState(VK_SHIFT) & 0x8000;
    fControlKey = GetKeyState(VK_CONTROL) & 0x8000;
    fAltKey     = GetKeyState(VK_MENU) & 0x8000;

	// The following state machine is used with the NCR 5932 Wedge keyboard
	// to handle key lock change events.
	// The NCR 5932 Wedge keyboard with key lock will send a 4 character keypress data
	// stream that looks like 0x7C 0xC0 [rlsx] 0x0D where [rlsx] stands for either
	// the 'r', 'l', 's', or 'x' characters indicating Register, Lock, Supervisor, or
	// EX mode respectively.
	if (m_nConvertKeyStateMachine) {
		m_usConvertKeyBuffer [m_nConvertKeyStateMachine] = LOWORD(pMsg->wParam);
		uchConvertCode = DE_WEDGE_STREAM;
		switch (m_nConvertKeyStateMachine) {
		case 1:
			m_usConvertKeyBuffer [m_nConvertKeyStateMachine] = LOWORD(pMsg->wParam);
			if (m_usConvertKeyBuffer [m_nConvertKeyStateMachine] != 0xC0)
				m_nConvertKeyStateMachine = 0;
			else
				m_nConvertKeyStateMachine++;
			break;
		case 2:
			m_usConvertKeyBuffer [m_nConvertKeyStateMachine] = LOWORD(pMsg->wParam);
			m_nConvertKeyStateMachine++;
			break;
		case 3:
			m_usConvertKeyBuffer [m_nConvertKeyStateMachine] = LOWORD(pMsg->wParam);
			if (m_usConvertKeyBuffer [m_nConvertKeyStateMachine] != 0x0d)
				;   // do nothing and just drop through as its a bad message
			else {
				m_nConvertKeyStateMachine = 0;
				if (m_usConvertKeyBuffer[2] == 0x4C)   // Lock mode indicated
					return (PIF_KEYLOCK + PIF_NCRKLOCK_MODE_LOCK);
				else if (m_usConvertKeyBuffer[2] == 0x52)  // Register Mode indicated
					return (PIF_KEYLOCK + PIF_NCRKLOCK_MODE_REG);
				else if (m_usConvertKeyBuffer[2] == 0x53)  // Supervisor mode indicated
					return (PIF_KEYLOCK + PIF_NCRKLOCK_MODE_SUP);
				else if (m_usConvertKeyBuffer[2] == 0x58)  // Ex mode indicated so change to Program Mode
					return (PIF_KEYLOCK + PIF_NCRKLOCK_MODE_PROG);
			}
			m_nConvertKeyStateMachine = 0;
			break;

		case 10:  // this is a FreedomPay data stream, VIRTUALKEY_EVENT_FREEDOMPAY
			if (! m_bVirtualKeyProcessingEnabled)  // if virtual key processing is disabled then ignore.
				break;
#if 0
#pragma message("                                            =====  ***   Virtual keyboard emulation turned on    ***  =====")
#if !defined(_DEBUG)
				CauseCompilerError++;
#endif
				// following will allow us to emulate a FreedomPay card swipe without the hardware.
				// if you want to do a bad card swipe then use the FreedomPayBadSwipe array instead
				// of the FreedomPayGoodSwipe array.
				// Setup the buffer with the data so that it will just flow naturally into
				// the code to process a received data stream.
				{
					TCHAR  *pSwipeData = FreedomPayGoodSwipe;
					int     iSwipeDataBytes = sizeof(FreedomPayGoodSwipe);
					char    cJunk = 0x10011;  // create compiler warning.

					m_nConvertKeyStateMachineIndex = 16;
					memcpy (&m_usConvertKeyBuffer [m_nConvertKeyStateMachineIndex], pSwipeData, iSwipeDataBytes);
					m_nConvertKeyStateMachineIndex += sizeof(FreedomPayGoodSwipe)/sizeof(FreedomPayGoodSwipe[0]) - 1;
					m_usConvertKeyBuffer [m_nConvertKeyStateMachine] = 13;
				}
#endif
			if (m_usConvertKeyBuffer [m_nConvertKeyStateMachine] != 13) {
				// Data string termination character not yet found so put this
				// character into the buffer.
				m_usConvertKeyBuffer [m_nConvertKeyStateMachineIndex] = LOWORD(pMsg->wParam);
				m_nConvertKeyStateMachineIndex++;
			}
			else {
				m_usConvertKeyBuffer [m_nConvertKeyStateMachineIndex] = LOWORD(pMsg->wParam);
				m_nConvertKeyStateMachineIndex++;
				memset (pVirtualKeyData, 0, sizeof(VIRTUALKEY_BUFFER));
				pVirtualKeyData->usKeyEventType = VIRTUALKEY_EVENT_FREEDOMPAY;
				// transfer the received data stream to the virtual key buffer
				// The FreedomPay RFID fob data contains a 25 character credentials sequence that
				// is provided by the RFID reader embedded into a string of characters.  The first
				// character is the beginning of data character followed by 25 credential characters
				// followed by an end of data character, checksum character, and end of message character.

				// Testing with a new sample FreedomPay RFID reader we are getting a string that is different
				// from what we expect.  Instead of a comma, we are seeing a character with a value of
				// decimal 188.
				// To work around this we are transforming the data into an acceptable
				// FreedomPay Creditials data string.
				// Richard Chambers, Feb-20-2008
				{
					int i = 0;
					for (i = 0; i < 25; i++) {
						pVirtualKeyData->u.freedompay_data.auchTrack2[i] = m_usConvertKeyBuffer[18 + i];
						if (pVirtualKeyData->u.freedompay_data.auchTrack2[i] == 188) {
							pVirtualKeyData->u.freedompay_data.auchTrack2[i] = ',';
						}
					}
					pVirtualKeyData->u.freedompay_data.uchLength2 = i;
				}

				uchConvertCode = PIF_VIRTUALKEYEVENT;
				m_nConvertKeyStateMachine = 0;
			}
			break;

		case 12:
			// This is a Mobile Demand wedge device.
			// This is the Symbol scanner attachment a USB device
			// that is using a wedge keyboard type of interface.
			if (pMsg->message != WM_CHAR)
				return (DE_NOT_CONVERTED);

			if (m_usConvertKeyBuffer [m_nConvertKeyStateMachine] != _T('\r')) {
				// Data string termination character not yet found so put this
				// character into the buffer.
				if (LOWORD(pMsg->wParam) >= _T(' ')) {
					m_usConvertKeyBuffer [m_nConvertKeyStateMachineIndex] = LOWORD(pMsg->wParam);
					m_nConvertKeyStateMachineIndex++;
				}
			}
			else {
				m_usConvertKeyBuffer [m_nConvertKeyStateMachineIndex] = LOWORD(pMsg->wParam);
				memset (pVirtualKeyData, 0, sizeof(VIRTUALKEY_BUFFER));
				pVirtualKeyData->usKeyEventType = VIRTUALKEY_EVENT_SCANNER;
				{
					int i = 0;
					for (i = 17; i < m_nConvertKeyStateMachineIndex; i++) {
						pVirtualKeyData->u.scanner_data.auchStr[i - 17] = m_usConvertKeyBuffer[i];
					}
					pVirtualKeyData->u.scanner_data.uchStrLen = i - 17;
				}
				uchConvertCode = PIF_VIRTUALKEYEVENT;
				m_nConvertKeyStateMachine = 0;
			}
			break;

			// This is a Mobile Demand wedge device.
			// This is the Magtek mag strip card reader a USB device
			// that is using a wedge keyboard type of interface.
			//
			// Removed the Mobile Demand PC Symbol MSR attachment.
			// this is an MSR data stream, UIE_VIRTUALKEY_EVENT_MSR
			//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers

			//  fall through and handle as default

		default:
			m_nConvertKeyStateMachine = 0;
			uchConvertCode = DE_NOT_CONVERTED;
			break;
		}
		return (uchConvertCode);
	}

    // Search the Convert Table if we have one.
	// If we have a virtual keyboard but this is not a guard character
	// indicating a data stream from a virtual keyboard device
	// we have to be mindful that this may be a touchscreen thus
	// we may not have a conversion table so we just return as not
	// converted if there is no conversion table.

	if (pConvertKey == 0)
		return (DE_NOT_CONVERTED);

	for ( ; pConvertKey->uchSrc != LOWORD(pMsg->wParam); pConvertKey++)
	{
		if (pConvertKey->uchSrc == DE_TABLE_END)
		{
			return (DE_NOT_CONVERTED);
		}
	}

	if(nKeyboardValue != KEYBOARD_WEDGE_78)
	{
		if (fAltKey)                                 // ALT(MENU) + XX
		{
			uchConvertCode = pConvertKey->uchAlt;
		}
		else if (fShiftKey && fControlKey)           // SHIFT + CONTROL + XX
		{
			uchConvertCode = pConvertKey->uchShiftCntl;
		}
		else if (fShiftKey)                          // SHIFT + XX
		{
			uchConvertCode = pConvertKey->uchShift;
		}
		else if (fControlKey)                        // CONTROL + XX
		{
			uchConvertCode = pConvertKey->uchCntl;
		}
		else
		{
			uchConvertCode = pConvertKey->uchNormal; // XX -> Normal key press
		}
	}else
	{
		//The 78 key keyboard detects its own built-in
		//MSR reader, however the MSR reader used
		//also sends off a key message as a button pressed
		//so we need to ignore this press. JHHJ
		fCapitalKey = GetKeyState(VK_CAPITAL) & 0x8000;
		if((pConvertKey->uchSrc == 0x14) && fCapitalKey)
		{
			return (DE_NOT_CONVERTED);
		}

		uchConvertCode = pConvertKey->uchNormal; // XX -> Normal key press
	}


	if (uchConvertCode == 0)
	{
		return (DE_NOT_CONVERTED);
	}

	TRACE(_T("VK=%d(0x%x), ConvertCode=%d(0x%x), Shift=%d, Control=%d, Alt=%d\n"),
		LOWORD(pMsg->wParam), LOWORD(pMsg->wParam),
		(int)uchConvertCode, (int)uchConvertCode,
		(int)fShiftKey,
		(int)fControlKey,
		(int)fAltKey);

	// This character represents the first character in an NCR 5932 Wedge
	// keyboard keylock sequence.  We have a state machine above we use to finish
	// parsing out the keylock sequence.  This code initializes the state machine
	// so that we will interpret the remaining characters the NCR 5932 has
	// wedged into the keyboard data stream as a keylock change.
	if (uchConvertCode >= 0xF0 && uchConvertCode <= 0xFF) {
		m_nConvertKeyStateMachine = 1;
		m_usConvertKeyBuffer [0] = LOWORD(pMsg->wParam);
		uchConvertCode = DE_NOT_CONVERTED;
	}

    return (uchConvertCode);
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:    Convert Key Code
//
// Prototype:   UCHAR   CDeviceEngine::ConvertAlgorismForKeyInput(pMsg);
//
// Inputs:      MSG*    pMsg;       - message
//
// Outputs:     UCHAR   uchConvertCode;
//                      =  DE_NOT_CONVERTED : not changed
//                      != DE_NOT_CONVERTED : converted key code
//
/////////////////////////////////////////////////////////////////////////////

USHORT CDeviceEngine::ConvertAlgorismForKeyInput(MSG *pMsg, VIRTUALKEY_BUFFER  *pVirtualKeyData)
{
    return ConvertKey(pMsg, pVirtualKeyData);
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   VOID        CDeviceEngine::MakeKeylockModeEvent();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID CDeviceEngine::MakeKeylockModeEvent()
{
    BL_NOTIFYDATA   BlNotify;
    _DEVICEINPUT    *pDeviceInput;
    CHARDATA        CharaData;

    // Initialize
    pDeviceInput = &BlNotify.u.Data;

    // Make Lockmode Event
	CharaData.uchFlags = 0;
    CharaData.uchASCII = 0;

	//Touchscreen Can "Start" in any mode
	//No keylock to change the mode
	//buttons control which mode terminal is in
	CharaData.uchScan = 0x04;	//Register Mode
    //CharaData.uchScan  = 0x03;      // LOCK MODE

    // setup notify data
    pDeviceInput->ulDeviceId = BL_DEVICE_DATA_KEY;
    pDeviceInput->ulLength   = sizeof(CharaData);
    pDeviceInput->pvAddr     = &CharaData;
    BlNotify.ulType          = BL_INPUT_DATA;

    // notify data
    BlNotifyData(&BlNotify, NULL);

    m_bFirstKeylock = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::DeviceDataMessage(pMsg);
//
// Inputs:      MSG         pMsg;       -
//
// Outputs:     BOOL        bResult;    - states
//
// Description: This function handles a message from a device thread which
//              indicates that the device has data which should be retrieved.
//              This is used by non-OPOS type devices providing a way for
//              devices, especially directly connected input devices such as
//              scanners to be handled by a separate thread which then informs
//              the main application window when there is some kind of device
//              data that needs to be retrieved.
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::DeviceDataMessage(MSG* pMsg)
{
    BOOL            bResult, bDone = FALSE;
    LONG            nStatus;
    DWORD           dwType;
    DWORD           dwLength;
    UCHAR           auchDevice[512], auchNotify[512];

    // is break key ?
    if (pMsg->message != WM_KEYDOWN)
    {
        return FALSE;                       // exit ...
    }

    for (;;)
    {
        // get a data from device
        dwLength = sizeof(auchDevice);
        bResult  = RemoveData(&dwType, auchDevice, &dwLength);

        if (! bResult)
        {
            return bDone;                   // exit ...
        }

        BOOL            bValid = FALSE;     // assume bad status
        BL_NOTIFYDATA   Notify;

        // which device ?
        switch (dwType)
        {
        case SCF_TYPE_KEYLOCK:
            bValid = ConvertKeylockData((CHARDATA*)auchNotify, (PDEVICEIO_KEYLOCK_POSITION)auchDevice, dwLength);
            Notify.u.Data.ulDeviceId = BL_DEVICE_DATA_KEY;
            Notify.u.Data.ulLength   = sizeof(CHARDATA);
            break;

        case SCF_TYPE_DRAWER1:
//            bValid = ConvertDrawerData(
//                        (USHORT*)auchNotify,
//                        (PDEVICEIO_DRAWER_STATUS)auchDevice,
//                        dwLength);
//            Notify.u.Data.ulDeviceId = BL_DEVICE_DATA_DRAWER;
//            Notify.u.Data.ulLength   = sizeof(USHORT) * 2;
            break;

        case SCF_TYPE_MSR:
			// used for direct connect type MSRs such as the MSR attached to the DT310 tablet
			// which communicates through a COM port rather than through a keyboard wedge interface.
            bValid = ConvertMSRData((MSR_BUFFER*)auchNotify, (PDEVICEIO_MSR_DATA)auchDevice, dwLength);
            Notify.u.Data.ulDeviceId = BL_DEVICE_DATA_MSR;
            Notify.u.Data.ulLength   = sizeof(MSR_BUFFER);
            break;

        case SCF_TYPE_SCANNER:     // FSC_SCANNER, PERIPHERAL_DEVICE_SCANNER
            bValid = ConvertScannerData(auchNotify, auchDevice, dwLength);
            memcpy(auchNotify, auchDevice, dwLength);
            bValid = dwLength ? TRUE : FALSE;
            Notify.u.Data.ulDeviceId = BL_DEVICE_DATA_SCANNER;
            Notify.u.Data.ulLength   = (ULONG)dwLength;
            break;

        case SCF_TYPE_SERVERLOCK:
            bValid = ConvertServerLockData((SHORT*)auchNotify, (PDEVICEIO_SERVERLOCK_NUMBER)auchDevice, dwLength);
            /* Notify.u.Data.ulDeviceId = BL_DEVICE_DATA_KEY; */
            Notify.u.Data.ulDeviceId = BL_DEVICE_DATA_SERVERLOCK;
            Notify.u.Data.ulLength   = sizeof(SHORT);
            break;

        default:
            ASSERT(FALSE);
            break;
        }

        // valid data ?

        if (m_bFirstKeylock && bValid)
        {
            Notify.ulType        = BL_INPUT_DATA;
            Notify.u.Data.pvAddr = auchNotify;
            nStatus              = BlNotifyData(&Notify, NULL);
        }

        bDone = TRUE;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::ConvertKeylockData(pBl,
//                              pDevice, dwLength);
//
// Inputs:
//
// Outputs:     BOOL        bResult;    -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::ConvertKeylockData(CHARDATA* pBl,
    PDEVICEIO_KEYLOCK_POSITION pDevice, DWORD dwLength)
{
    DWORD   dwPifPosition;

    // valid data length ?
    if (dwLength != sizeof(DEVICEIO_KEYLOCK_POSITION))
    {
        return FALSE;                       // exit ...
    }

    // which position ?
	// These positions are based on the NCR 7448 terminal keylock.
	// Some other configurations such as RealPOS with the NCR 5932 Wedge keyboard
	// may not allow for all of these positions.

    switch (pDevice->dwPosition)
    {
		case NCRKLOCK_POSITION_1:
			dwPifPosition = PIF_NCRKLOCK_MODE_EX;		// Ex keylock position
			break;
		case NCRKLOCK_POSITION_2:
			dwPifPosition = PIF_NCRKLOCK_MODE_LOCK;		// Lock keylock position
			break;
		case NCRKLOCK_POSITION_3:
			dwPifPosition = PIF_NCRKLOCK_MODE_REG;		// Register keylock position
			break;
		case NCRKLOCK_POSITION_4:
			dwPifPosition = PIF_NCRKLOCK_MODE_SUP;		// Supervisor keylock position
			break;
		case NCRKLOCK_POSITION_5:
			dwPifPosition = PIF_NCRKLOCK_MODE_PROG;		// Program keylock position
			break;
		default:										// unknown position
			return FALSE;								// exit ...
    }

    // make up business logic data

    pBl->uchASCII = 0;
    pBl->uchScan  = (UCHAR)(PIF_KEYLOCK + dwPifPosition);

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::ConvertDrawerData(pBl,
//                              pDevice, dwLength);
//
// Inputs:
//
// Outputs:     BOOL        bResult;    -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::ConvertDrawerData(USHORT* pBl,
    PDEVICEIO_DRAWER_STATUS pDevice, DWORD dwLength)
{
    // valid data length ?

    if (dwLength != sizeof(DEVICEIO_DRAWER_STATUS))
    {
        return FALSE;                       // exit ...
    }

    // make up business logic data

    *(pBl + 0) = (USHORT)pDevice->dwDrawerA;
    *(pBl + 1) = (USHORT)pDevice->dwDrawerB;

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::ConvertMSRData(pBl,
//                              pDevice, dwLength);
//
// Inputs:
//
// Outputs:     BOOL        bResult;    -
//
// Description: This function is used to convert a DEVICEIO_MSR_DATA data
//              into an MSR_BUFFER data buffer.  The track status of each
//              track is examined and if there is data for that track then
//              we copy that data over to the MSR_BUFFER data struct.
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::ConvertMSRData(MSR_BUFFER* pBl, PDEVICEIO_MSR_DATA pDevice, DWORD dwLength)
{
    BOOL    bResult = FALSE;                // assume bad status

    // valid data length ?
    if (dwLength != sizeof(DEVICEIO_MSR_DATA))
    {
        return FALSE;                       // exit ...
    }

    // clear buffer
    memset(pBl, '\0', sizeof(MSR_BUFFER));

    // is available track 1 data ?
    if (pDevice->sStatus1 == DEVICEIO_MSR_SUCCESS)
    {
        pBl->uchLength1 = pDevice->sLength1 <= PIF_LEN_TRACK1 ? pDevice->sLength1 : 0;
        memcpy(pBl->auchTrack1, pDevice->uchTrack1, pBl->uchLength1 * sizeof(pDevice->uchTrack1[0]));
        bResult = pBl->uchLength1 ? TRUE : bResult;
    }

    // is available track 2 data ?
    if (pDevice->sStatus2 == DEVICEIO_MSR_SUCCESS)
    {
        pBl->uchLength2 = pDevice->sLength2 <= PIF_LEN_TRACK2 ? pDevice->sLength2 : 0;
        memcpy(pBl->auchTrack2, pDevice->uchTrack2, pBl->uchLength2 * sizeof(pDevice->uchTrack2[0]));
        bResult = pBl->uchLength2 ? TRUE : bResult;
    }

    // is available track 3 data ?
    if (pDevice->sStatus3 == DEVICEIO_MSR_SUCCESS)
    {
        pBl->uchLengthJ = pDevice->sLength3 <= PIF_LEN_TRACKJ ? pDevice->sLength3 : 0;
        memcpy(pBl->auchTrackJ, pDevice->uchTrack3, pBl->uchLengthJ * sizeof(pDevice->uchTrack3[0]));
        bResult = pBl->uchLengthJ ? TRUE : bResult;
    }

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::ConvertScannerData(pBl,
//                              pDevice, dwLength);
//
// Inputs:
//
// Outputs:     BOOL        bResult;    -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::ConvertScannerData(UCHAR* pBl, UCHAR* pDevice,
    DWORD dwLength)
{
    memcpy(pBl, pDevice, dwLength);

    // exit ...

    return (BOOL)dwLength;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::ConvertServerLockData(pBl,
//                              pDevice, dwLength);
//
// Inputs:
//
// Outputs:     BOOL        bResult;    -
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::ConvertServerLockData(SHORT* pBl,
    PDEVICEIO_SERVERLOCK_NUMBER pDevice, DWORD dwLength)
{
    // valid data length ?

    if (dwLength != sizeof(DEVICEIO_SERVERLOCK_NUMBER))
    {
        return FALSE;                       // exit ...
    }

    *pBl = (SHORT)pDevice->dwNumber;

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::AddData(dwType, pvData, nLength);
//
// Inputs:      MSG         pMsg;       -
//
// Outputs:     BOOL        bResult;    - states
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::AddData(DWORD dwType, VOID* pvData, DWORD dwLength)
{
    PDataInfo   pInfo;

    // allocate a memory and copy the data

    pInfo           = new DataInfo;
    pInfo->dwType   = dwType;
    pInfo->dwLength = dwLength;
    pInfo->pvData   = new UCHAR [dwLength];
    memcpy(pInfo->pvData, pvData, dwLength);

    // is available window object ?  If so, lets check to see if we are the
	// foreground window.  If not, then lets make ourselves the foreground window.
    if (AfxGetApp()->m_pMainWnd && AfxGetApp()->m_pMainWnd->m_hWnd)
    {
        if (::GetForegroundWindow() != AfxGetApp()->m_pMainWnd->m_hWnd)
        {
            ::SetForegroundWindow(AfxGetApp()->m_pMainWnd->m_hWnd);
//          ::SetWindowPos(
//              AfxGetApp()->m_pMainWnd->m_hWnd,
//              HWND_TOP,
//              0,
//              0,
//              0,
//              0,
//              SWP_NOMOVE | SWP_NOSIZE);
        }
    }

   ::EnterCriticalSection(&m_csData);

    // add a data to queue
    m_lstData.AddTail(pInfo);

    // notify device data event to windows
    //::keybd_event(VK_F24, 0, 0,               0);
    //::keybd_event(VK_F24, 0, KEYEVENTF_KEYUP, 0);
    if (AfxGetApp()->m_pMainWnd && AfxGetApp()->m_pMainWnd->m_hWnd)
    {
        HWND hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;

        /* generate VK_F24 as device data trigger only if main window is generated. */
        ::PostMessage(hWnd, WM_KEYDOWN, VK_F24, 0x00000001);
        ::PostMessage(hWnd, WM_KEYUP,   VK_F24, 0xC0000001);
    }

    ::LeaveCriticalSection(&m_csData);

    // exit ...

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   BOOL        CDeviceEngine::RemoveData(pdwType, pvData, pdwLength);
//
// Inputs:      MSG         pMsg;       -
//
// Outputs:     BOOL        bResult;    - states
//
/////////////////////////////////////////////////////////////////////////////

BOOL CDeviceEngine::RemoveData(DWORD* pdwType, VOID* pvData, DWORD* pdwLength)
{
    PDataInfo   pInfo = NULL;               // assume no data

    ::EnterCriticalSection(&m_csData);

    // get a data from queue
    if (! m_lstData.IsEmpty())
    {
        pInfo = (PDataInfo)m_lstData.RemoveHead();
    }

    ::LeaveCriticalSection(&m_csData);

    // no data ?
    if (! pInfo)
    {
		*pdwType   = 0;
		*pdwLength = 0;
        return FALSE;                       // exit ...
    }

    // copy the label data
	if (pInfo->dwLength > *pdwLength)
		pInfo->dwLength = *pdwLength;

    *pdwType   = pInfo->dwType;
    *pdwLength = pInfo->dwLength;
    memcpy(pvData, pInfo->pvData, pInfo->dwLength);

    // free the memory
    delete [] pInfo->pvData;
    delete pInfo;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//
// Prototype:   VOID        CDeviceEngine::RemoveAllData();
//
// Inputs:      nothing
//
// Outputs:     nothing
//
/////////////////////////////////////////////////////////////////////////////

VOID CDeviceEngine::RemoveAllData()
{
    PDataInfo   pInfo;

    ::EnterCriticalSection(&m_csData);

    while (! m_lstData.IsEmpty())
    {
        pInfo = (PDataInfo)m_lstData.RemoveHead();
        delete [] pInfo->pvData;
        delete pInfo;
    }

    ::LeaveCriticalSection(&m_csData);
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    call back function
//
// Prototype:   DWORD       BlInterface2(dwCommand, pvData, dwLength);
//
// Inputs:      DWORD       dwCommand;  -
//              VOID*       pvData;     -
//              DWORD       dwLength;   -
//
// Outputs:     DWORD       dwResult;   -
//
/////////////////////////////////////////////////////////////////////////////

__declspec(dllexport)
DWORD CALLBACK BlInterface2(DWORD dwCommand, VOID* pvData, DWORD dwLength)
{
    BOOL            bResult;

#ifndef DEVENG_REPLACEMENT
    CFrameworkApp*  pApp;
    CDeviceEngine*  pEngine;

    // get the object
    pApp    = (CFrameworkApp*)AfxGetApp();
    pEngine = pApp ? pApp->GetDeviceEngine() : NULL;

    // valid object ?
    if (pEngine)
    {
        bResult = pEngine->BlInterface (dwCommand, pvData, dwLength);
    }
    else
    {
        ASSERT(FALSE);
        bResult = FALSE;
    }
#else
	


#endif

    // exit ...
    return (DWORD)bResult;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction                             Saratoga
//////////////////////////////////////////////////////////////////////

static SHORT  UieCopyFileToFile (TCHAR *tcsDestFileName, TCHAR *tcsSourceFileName)
{
	UCHAR	auchStoreBuffer[2048];
	ULONG	ulBytesWritten= 0;
	ULONG	ulActualBytesRead;
	SHORT   sDestFileHandle, sSourceFileHandle;

	sDestFileHandle = PifOpenFile (tcsDestFileName, "xw");
	sSourceFileHandle = PifOpenFile (tcsSourceFileName, "or");

	if (sDestFileHandle >= 0 && sSourceFileHandle >= 0) {
		do {
			PifReadFile (sSourceFileHandle, auchStoreBuffer, sizeof(auchStoreBuffer), &ulActualBytesRead);
			PifWriteFile (sDestFileHandle, auchStoreBuffer, ulActualBytesRead);
		} while(ulActualBytesRead == sizeof(auchStoreBuffer));

	}

	if (sDestFileHandle >= 0) {
		PifCloseFile (sDestFileHandle);
	}

	if (sSourceFileHandle >= 0) {
		PifCloseFile (sSourceFileHandle);
	}

	return SUCCESS;
	
}

BOOL CDeviceEngine::PrinterLoadLogoIndexInformation()
//We are now using a logo index file, which stores all of
//the logos filenames for use in pep, as well as information
//on which logo is to be used for a special logo, and which one will
//be used as the standard logo.
{
	DWORD dwFileSize = 0;
	TCHAR tcsCommandString[] = _T("C:/FlashDisk/NCR/Saratoga/Database/PARAMLO%d%s");
	TCHAR myString[258];
	memset(&myString, 0x00, sizeof(myString));

	CFile myLogoFileIndex;
	CFileException e;

	memset (&this->m_flfLogoIndex, 0, sizeof(this->m_flfLogoIndex));

	if (! (m_pOPOSControl && m_pOPOSControl->m_hWnd && m_pOPOSControl->m_OPOSPrinter.GetState() != OPOS_S_CLOSED) ) {
		NHPOS_NONASSERT_TEXT("CDeviceEngine::PrinterLoadLogoIndexInformation(): OPOS control issue.");
		return FALSE;
	}

	if(!myLogoFileIndex.Open(_T("C:\\FlashDisk\\NCR\\Saratoga\\Database\\PARAMLID"),CFile::modeRead,&e))
	{
		e.GetErrorMessage(myString,256,NULL);
		NHPOS_ASSERT_TEXT(0, myString);
		return FALSE;
	}
	try {
		dwFileSize = myLogoFileIndex.GetLength ();
	}
	catch (CFileException *e) {
		char  xBuff[256];
		sprintf (xBuff, "CDeviceEngine::PrinterLoadLogoIndexInformation(): myLogoFileIndex.GetLength () threw exception %d.", e->m_cause);
		NHPOS_ASSERT_TEXT(0, xBuff);
		e->Delete();
	}

	if (dwFileSize < sizeof(this->m_flfLogoIndex)) {
		myLogoFileIndex.Close();
#if defined(_DEBUG)
		NHPOS_ASSERT_TEXT(0,"CDeviceEngine::PrinterLoadLogoIndexInformation(): PARAMLID file size too small.");
#endif
		return FALSE;
	}

	myLogoFileIndex.Read(&this->m_flfLogoIndex,sizeof(this->m_flfLogoIndex));
	myLogoFileIndex.Close();

	const SYSCONFIG *pSysConfig = PifSysConfig();                    /* get system config */

	//we load all of  our logos into the service object so that we dont have to do it multiple
	//times JHHJ
	int i;
	LONG  lStatus = 0, lextResultCode = 0;

	for(i = 0; i < 9; i++)
	{
		TCHAR tcsCommandString[] = _T("C:/FlashDisk/NCR/Saratoga/Database/PARAMLO%d%s");

		if (pSysConfig->tcsLogoExtension[0]) {
			// an extension is specified so lets copy the standard files to a new file with extension.
			TCHAR  tcsDestFile[64], tcsSourceFile[64];
			wsprintf (tcsDestFile, _T("PARAMLO%d%s"), (i + 1), pSysConfig->tcsLogoExtension);
			wsprintf (tcsSourceFile, _T("PARAMLO%d"), (i + 1));
			UieCopyFileToFile (tcsDestFile, tcsSourceFile);
		}

		wsprintf(myString, tcsCommandString, (i + 1), pSysConfig->tcsLogoExtension);
		lStatus = m_pOPOSControl->m_OPOSPrinter.SetBitmap ((i + 1), PTR_S_RECEIPT, myString, PTR_BM_ASIS, PTR_BM_CENTER);
		lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
		{
			char xBuff[256];
			sprintf (xBuff, "PrinterLoadLogoIndexInformation(): lStatus = %d  0x%x lextResultCode = %d 0x%x  i = %d", lStatus, lStatus, lextResultCode, lextResultCode, i);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
		PifSleep(pSysConfig->usLoadLogoDelay);
	}
	//Load the last logo alone, because we have to load it into PARAMLA instead of PARAML10
	wsprintf(myString, _T("C:\\FlashDisk\\NCR\\Saratoga\\Database\\PARAMLOA%s"), pSysConfig->tcsLogoExtension);
	lStatus = m_pOPOSControl->m_OPOSPrinter.SetBitmap (i + 1, PTR_S_RECEIPT, myString, PTR_BM_ASIS, PTR_BM_CENTER);
	lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
		{
			char xBuff[256];
			sprintf (xBuff, "PrinterLoadLogoIndexInformation(): lStatus = %d  0x%x  lextResultCode = %d 0x%x  i = 10", lStatus, lStatus, lextResultCode, lextResultCode);
			NHPOS_NONASSERT_TEXT(xBuff);
		}
	PifSleep(pSysConfig->usLoadLogoDelay);

	NHPOS_NONASSERT_TEXT("Printer logos loaded.");
	if(this->m_flfLogoIndex.uchStatus & LOGO_STAT_PERCENTAGE)
	{
		PARAFLEXMEM paraFlexMem = {0};

		paraFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
		paraFlexMem.uchAddress = FLEX_GC_ADR;
		BlFwIfParaRead(&paraFlexMem);
	}

	return TRUE;
}

LONG CDeviceEngine::PrinterChangePrintSide(SHORT sType)
//this function is for double sided printers, basically
//we are using the Double Sided mode with full control.
//we manually change the side to print on instead of
//having just a static image on a side. JHHJ
{
	USHORT usLen;
	TCHAR myString[256*2], myTemp[100];
	//the commandline below is for switching sides to print on
	TCHAR tcsFormatString[] = _T("\x1B[2ST;6;%d!v");

	memset(&myString, 0x00, sizeof(myString));
	memset(&myTemp, 0x00,sizeof(myTemp));

	usLen = (_tcslen(_T("\x1B[2ST;6;x!v"))); //Double sided print code to

	memset(&myString, 0x00, sizeof(myString));

	_tcscat(myString, _T("\x1B|"));
	_ultot(usLen, &myTemp[0], 10);
	_tcscat(myString, myTemp);
	_tcscat(myString, _T("E"));
	wsprintf(myTemp, tcsFormatString, sType);
	_tcscat(myString,myTemp);
	return m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT, myString);
}

LONG CDeviceEngine::PrinterPrintLogo(ULONG ulTransNo, BOOL blFreq)
{
	LONG    lResultCode = 0;
	LONG    lextResultCode = 0;
	TCHAR   myString[256*2];
	TCHAR   tcsFormatString[] = _T("\x1B|%dB"); //printer code for printing a bitmap
	SHORT	lRandNumber;
	char    xBuff[128];

	if (blFreq && (this->m_flfLogoIndex.uchStatus & LOGO_STAT_PERCENTAGE))
	{
		lRandNumber = myRandNumber (1, 100);			// We want a random number between 1-100
		try {
			if (lRandNumber <= (USHORT)this->m_flfLogoIndex.ulSpecialPercent){
				//we assign the logo that is assigned for being the special one
				NHPOS_ASSERT_TEXT((this->m_flfLogoIndex.uchSpecialLogo < 20), "==NOTE: PrinterPrintLogo() logo index out of range.");
				wsprintf(myString,tcsFormatString, this->m_flfLogoIndex.uchSpecialLogo);
				lResultCode = m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT,myString);
				if (lResultCode != OPOS_SUCCESS) {
					if(lResultCode == OPOS_E_EXTENDED)
					{
						lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
					}
					sprintf (xBuff, "**WARNING: PrinterPrintLogo() lResultCode %d, lextResultCode %d index %d", lResultCode, lextResultCode, this->m_flfLogoIndex.uchSpecialLogo);
					NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
				}
				return SPECIAL_LOGO;
			}else
			{
				//we assign the logo that is to be standard.
				if(this->m_flfLogoIndex.uchStandardLogo)
				{
					NHPOS_ASSERT_TEXT((this->m_flfLogoIndex.uchStandardLogo < 20), "==NOTE: PrinterPrintLogo() logo index out of range.");
					wsprintf(myString,tcsFormatString, this->m_flfLogoIndex.uchStandardLogo);
					lResultCode = m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT,myString);
					if (lResultCode != OPOS_SUCCESS) {
						if(lResultCode == OPOS_E_EXTENDED)
						{
							lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
						}
						sprintf (xBuff, "**WARNING: PrinterPrintLogo() lResultCode %d  lextResultCode %d index %d", lResultCode, lextResultCode, this->m_flfLogoIndex.uchStandardLogo);
						NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
					}
				}
				//feed 5 lines after the picture is printed, for good measure JHHJ
//					m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT, _T("\x1B|5lF"));
				return STANDARD_LOGO;
			}
		}
		catch (...) {
			lResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
			if(lResultCode == OPOS_E_EXTENDED)
			{
				lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
			}
			sprintf (xBuff, "**WARNING: catch PrinterPrintLogo() lResultCode %d, lextResultCode %d index %d ", lResultCode, lextResultCode, this->m_flfLogoIndex.uchStandardLogo);
			NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
		}
	}else
	{
		try {
			//we assign the logo that is to be standard.
			if(this->m_flfLogoIndex.uchStandardLogo)
			{
				NHPOS_ASSERT_TEXT((this->m_flfLogoIndex.uchStandardLogo < 20), "==NOTE: PrinterPrintLogo() logo index out of range.");
				wsprintf(myString,tcsFormatString, this->m_flfLogoIndex.uchStandardLogo);
				lResultCode = m_pOPOSControl->m_OPOSPrinter.PrintNormal(PTR_S_RECEIPT,myString);
				if (lResultCode != OPOS_SUCCESS) {
					if(lResultCode == OPOS_E_EXTENDED)
					{
						lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
					}
					sprintf (xBuff, "**WARNING: catch PrinterPrintLogo() lResultCode %d  lextResultCode %d index %d", lResultCode, lextResultCode, this->m_flfLogoIndex.uchStandardLogo);
					NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
				}
				return STANDARD_LOGO;
			}
		}
		catch (...) {
			lResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
			if(lResultCode == OPOS_E_EXTENDED)
			{
				lextResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
			}
			sprintf (xBuff, "**WARNING: catch PrinterPrintLogo() lResultCode %d  lextResultCode %d index %d ", lResultCode, lextResultCode, this->m_flfLogoIndex.uchStandardLogo);
			NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
		}
	}

	return NO_SPECIFIED_LOGO;

}

SHORT CDeviceEngine::myRandNumber (USHORT low, USHORT high)
{
	USHORT lRange = high - low;
	ULONG lRandNumber;

	lRandNumber = rand ();
	lRandNumber = lRandNumber * 100L * (lRange + 1) / (RAND_MAX + 1) / 100L;

	return (USHORT)lRandNumber;
}

// This function checks to see if the activeX controlles returned
// any error messages.   ***PDINU
VOID CDeviceEngine::PrinterCheckStatus (PFRAMEWORK_IO_PRINTER pData, LONG resultCode)
{
	LONG  extResultCode;
	char aszErrorBuffer[128];

	if (!m_pOPOSControl) return;

	resultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
	sprintf (aszErrorBuffer, "BlInterface (): Error Printing:: Result Code: %d", resultCode);
	NHPOS_ASSERT_TEXT(0, aszErrorBuffer);

	if(resultCode == OPOS_E_EXTENDED)
	{
		extResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
		memset(aszErrorBuffer,'\0',sizeof(aszErrorBuffer));
		sprintf (aszErrorBuffer, "BlInterface (): Error Printing:: Extended Code: %d", extResultCode);
		NHPOS_ASSERT_TEXT(0, aszErrorBuffer);
		if(extResultCode == OPOS_EPTR_SLP_EMPTY)
		{
			pData->dwErrorCode = DEVICEIO_PRINTER_MSG_VSLPINS_ADR;
		}
		else if(extResultCode == OPOS_EPTR_REC_EMPTY)
		{
			pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PAPEROUT_ADR;
		}
		else if(extResultCode == OPOS_EPTR_COVER_OPEN)
		{
			pData->dwErrorCode = DEVICEIO_PRINTER_MSG_COVEROPEN_ADR;
		}
		else
		{
			pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
		}
		if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
			BlFwPmgCallBack(pData->dwErrorCode, 0);
		}
	}
	else /*if(resultCode != OPOS_E_ILLEGAL)*/
	{
		pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
		if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
 			BlFwPmgCallBack(pData->dwErrorCode, 0);
		}
	}
	Sleep(50);
}

//  The following funciton looks into the header information of the logo files
//  and determines the maximum length of the logo being displayed last.  This
//  funtion was created to add extra padding to the end of a receipt so the
//  image is aligned correctly.   ***PDINU
LONG CDeviceEngine::LoadImageInformation(USHORT printingLogoType)
{
	LONG    lRetValue = 0;
	TCHAR   myString[256];
	TCHAR   tcsCommandString[] = _T("C:\\FlashDisk\\NCR\\Saratoga\\Database\\PARAMLO%d.BMP");
	memset(&myString, 0x00, sizeof(myString));

	USHORT	usLen = _tcslen(tcsCommandString);
	CFile   myBmp;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;

	if (printingLogoType == STANDARD_LOGO)
		wsprintf(myString, tcsCommandString, this->m_flfLogoIndex.uchStandardLogo);
	else if (printingLogoType == SPECIAL_LOGO)
		wsprintf(myString, tcsCommandString, this->m_flfLogoIndex.uchSpecialLogo);
	else
		return 0;

	if( !myBmp.Open( myString, CFile::modeRead) )
		return 0;

	// Read file header
	if (myBmp.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader)) {
		// File type should be 'BM'
		if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B')) {
			if (myBmp.Read((LPSTR)&bmiHeader, sizeof(bmiHeader)) != sizeof(bmiHeader)) {
				lRetValue = bmiHeader.biHeight;
			}
		}
	}

	return lRetValue;
}

SHORT CDeviceEngine::SetdwErrorCode (VOID* pvData)
{
	PFRAMEWORK_IO_PRINTER  pData = (PFRAMEWORK_IO_PRINTER)pvData;
	LONG                   lOposPrinterStatus = 0;

	pData->dwErrorCode = 0;
	pData->extResultCode = pData->resultCode = 0;
	pData->m_ulPrinter1StatusMap = pData->fbStatus = 0;
	// bit PRT_STAT_ERROR is used to indicate NOT an error condition for some reason.
	pData->fbStatus |= (PRT_STAT_ERROR | PRT_STAT_INIT_END);
	if (m_pOPOSControl) {
		if( OPOS_S_ERROR == (pData->resultCode = m_pOPOSControl->m_OPOSPrinter.GetState()))
		{
			pData->fbStatus = (PRT_STAT_HW_ERROR | PRT_STAT_POWER_OFF);
			pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
		} else {
			if( OPOS_SUE_POWER_ONLINE != (lOposPrinterStatus = m_pOPOSControl->m_OPOSPrinter.GetPowerState()))	{
				pData->fbStatus = (PRT_STAT_HW_ERROR | PRT_STAT_POWER_OFF);
				pData->dwErrorCode = DEVICEIO_PRINTER_MSG_POWEROFF_ADR;
			}
		}

		// We have noticed that if there is a printer power off error then the following
		// status check logic does not seem to correctly detect that there is a problem.
		// The GetResultCode() and GetResultCodeExtended() functions both seem to return
		// a value of zero which makes the logic think that there is in fact not a problem
		// with the printer when there is.
		//    Richard Chambers, Sep-13-2018
		if (pData->dwErrorCode != DEVICEIO_PRINTER_MSG_POWEROFF_ADR) {
			pData->m_ulPrinter1StatusMap = m_pOPOSControl->m_ulPrinter1StatusMap;
			// we can not necessarily depend on status events for some printers such as the Zebra printer for Amtrak
			// this needs some thought.  what we have seen is that NCR printers are reliable in their status events.
			if (pData->m_ulPrinter1StatusMap != 0) {
				// bit PRT_STAT_ERROR is used to indicate NOT an error condition for some reason.
				pData->fbStatus &= ~(PRT_STAT_ERROR | PRT_STAT_INIT_END);
				pData->fbStatus |= PRT_STAT_HW_ERROR;
				if (m_pOPOSControl->m_ulPrinter1StatusMap & COPOSControl::PRINTERSTATUSMAP_COVEROPEN) {
					pData->dwErrorCode = DEVICEIO_PRINTER_MSG_COVEROPEN_ADR;
				} else if (m_pOPOSControl->m_ulPrinter1StatusMap & COPOSControl::PRINTERSTATUSMAP_PAPEROUT) {
					pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PAPEROUT_ADR;
				} else {
					pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
				}
			}
			{
				static LONG  lsResultCode = 0, lsExtResultCode = 0;
				static int   iCount = 0;
				LONG         lResultCode, lExtResultCode;

				lResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
				lExtResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
#if 0
			// Remove this log as it generates a lot of recurring ASSRTLOG logs to little value.
			//    Richard Chambers Feb-11-2017 while working on Amtrak Stored Transaction field issue and
			//                                 reviewing logs from the field.
				if ((pData->fbStatus & PRT_STAT_HW_ERROR) != 0) {
					char  xBuff[256];
					sprintf (xBuff, "==TEST: GetState %d  %d  fbStatus 0x%x dwErrorCode %d StatusMap 0x%x GetResult %d  %d", pData->resultCode, pData->extResultCode, pData->fbStatus, pData->dwErrorCode, pData->m_ulPrinter1StatusMap, lResultCode, lExtResultCode);
					NHPOS_NONASSERT_NOTE("==TEST", xBuff);
				}
#endif
				lsResultCode = lResultCode;
				lsExtResultCode = lExtResultCode;
				if (lResultCode == OPOS_SUCCESS && (pData->fbStatus & PRT_STAT_HW_ERROR) != 0) {
					// We have seen an error in the Amtrak field in which GenPOS will display a printer error when
					// the printer shows green LEDs and there does not seem to be any printer hardware issue.
					// Investigation indicates that there are times when the Zebra printer used by Amtrak will send
					// a set of events of paper out followed almost immediately by paper ok.  This source is a
					// work around for that field issue.  Richard Chambers, Nov 18, 2014
					pData->fbStatus |= (PRT_STAT_ERROR | PRT_STAT_INIT_END);
					pData->fbStatus &= ~PRT_STAT_HW_ERROR;
					pData->dwErrorCode = 0;
					pData->m_ulPrinter1StatusMap = (m_pOPOSControl->m_ulPrinter1StatusMap &= ~COPOSControl::PRINTERSTATUSMAP_PAPEROUT);
#if 0
				// Remove this log as it generates a lot of recurring ASSRTLOG logs to little value.
				//    Richard Chambers Feb-11-2017 while working on Amtrak Stored Transaction field issue and
				//                                 reviewing logs from the field.
					NHPOS_NONASSERT_NOTE ("==NOTE", "==NOTE: lResultCode == OPOS_SUCCESS, pData->fbStatus reset.");
#endif
				}
			}
		}
	}
	return 0;
}


// CDeviceEngine::DevicePrinterWrite() to handle case FRAMEWORK_IOCTL_PRINTER_WRITE:
BOOL CDeviceEngine::DevicePrinterWrite (VOID *pvData, DWORD dwLength)
{
	static int          x = 0, y = 0;
	static DATE_TIME    dtSaveDateTime = {0};
	PFRAMEWORK_IO_PRINTER pData = (PFRAMEWORK_IO_PRINTER)pvData;
	LONG       lRet = 0;
	LONG       lPrintStation = 0;
	LONG       lPrintType = 0;
    BOOL       bResult = 0, bPrintAssert = 1;
	BOOL       bUsePrintImmediate = FALSE;
//	TCHAR      singleModeStr[] = _T("ESC[2ST;5;0!v");    // removed as not used. also would ESC to be 0x1B, escape character, instead.  RJC 11/13/2020
	DATE_TIME  dtCurrentDateTime;
	char       aszErrorBuffer[128];

	memset(aszErrorBuffer,'\0',sizeof(aszErrorBuffer));
    if (dwLength < sizeof(FRAMEWORK_IO_PRINTER))
        return FALSE;                   // exit ...

	bResult = TRUE;

	pData->resultCode = pData->extResultCode = pData->dwErrorCode = 0;
	lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
	lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);

	pData->fbStatus = 0;
	pData->m_ulPrinter1StatusMap = 0;
	if (lPrintType == PMG_OPOS_IMMEDIATE) {
		lPrintType = PMG_OPOS_PRINTER;
		bUsePrintImmediate = TRUE;
	}
	if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl){
		LONG    lOposPrinterStatus = 0;
		USHORT  usBlFwRetStat = 0;

		SetdwErrorCode (pData);

		PifGetDateTime(&dtCurrentDateTime);
		bPrintAssert = (memcmp (&dtSaveDateTime, &dtCurrentDateTime, sizeof(DATE_TIME)) != 0);
		dtSaveDateTime = dtCurrentDateTime;
		if (pData->fbStatus & PRT_STAT_HW_ERROR) {
			if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
				BlFwPmgCallBack(pData->dwErrorCode, 0);
			}
			bResult = FALSE;
		} else {
			try {
				if (bUsePrintImmediate) {
					pData->resultCode = m_pOPOSControl->m_OPOSPrinter.PrintImmediate(lPrintStation, pData->ptchBuffer);
				} else {
					pData->resultCode = m_pOPOSControl->m_OPOSPrinter.PrintNormal(lPrintStation, pData->ptchBuffer);
				}
			}
			catch (...) {
				char  xBuff[128];

				pData->resultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
				if(pData->resultCode == OPOS_E_EXTENDED)
				{
					pData->extResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
				}
				sprintf (xBuff, "**WARNING: catch DevicePrinterWrite() lResultCode %d  lextResultCode %d", pData->resultCode, pData->extResultCode);
				NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
			}
			if (pData->resultCode == OPOS_E_ILLEGAL)
			{
				try {
					// Possibly because printer buffer was not open for writing.  See OPOS documentation.  ***PDINU
					bResult = BlInterface(FRAMEWORK_IOCTL_PRINTER_TRANS_START, pvData, dwLength);
					if (bUsePrintImmediate) {
						pData->resultCode = m_pOPOSControl->m_OPOSPrinter.PrintImmediate(lPrintStation, pData->ptchBuffer);
					} else {
						pData->resultCode = m_pOPOSControl->m_OPOSPrinter.PrintNormal(lPrintStation, pData->ptchBuffer);
					}
				}
				catch (...) {
					char  xBuff[128];

					pData->resultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
					if(pData->resultCode == OPOS_E_EXTENDED)
					{
						pData->extResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
					}
					sprintf (xBuff, "**WARNING: catch DevicePrinterWrite() lResultCode %d  lextResultCode %d", pData->resultCode, pData->extResultCode);
					NHPOS_NONASSERT_NOTE("**WARNING", xBuff);
				}
			}
			
			if(pData->resultCode == OPOS_SUCCESS)
			{
				bResult = TRUE;
				// bit PRT_STAT_ERROR is used to indicate NOT an error condition for some reason.
				pData->fbStatus = (PRT_STAT_ERROR | PRT_STAT_INIT_END);
				pData->dwErrorCode = DEVICEIO_PRINTER_MSG_SUCCESS_ADR;
				BlFwPmgCallBack(pData->dwErrorCode, 1);
//				m_pOPOSControl->m_ulPrinter1StatusMap &= ~(COPOSControl::PRINTERSTATUSMAP_COVEROPEN | COPOSControl::PRINTERSTATUSMAP_COVEROPEN);
				m_pOPOSControl->m_ulPrinter1ResultMap = 0;
			} else
			{
				bResult = FALSE;
				pData->fbStatus = (PRT_STAT_HW_ERROR | PRT_STAT_POWER_OFF);
				pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
				pData->resultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCode();
				if(pData->resultCode == OPOS_E_EXTENDED)
				{
					pData->extResultCode = m_pOPOSControl->m_OPOSPrinter.GetResultCodeExtended();
					if(pData->extResultCode == OPOS_EPTR_SLP_EMPTY)
					{
						pData->dwErrorCode = DEVICEIO_PRINTER_MSG_VSLPINS_ADR;
					}
					else if(pData->extResultCode == OPOS_EPTR_REC_EMPTY)
					{
						pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PAPEROUT_ADR;
						pData->m_ulPrinter1StatusMap |= COPOSControl::PRINTERSTATUSMAP_PAPEROUT;
						m_pOPOSControl->m_ulPrinter1ResultMap |= COPOSControl::PRINTERSTATUSMAP_COVEROPEN;
					}
					else if(pData->extResultCode == OPOS_EPTR_COVER_OPEN)
					{
						pData->dwErrorCode = DEVICEIO_PRINTER_MSG_COVEROPEN_ADR;
						pData->m_ulPrinter1StatusMap |= COPOSControl::PRINTERSTATUSMAP_COVEROPEN;
						m_pOPOSControl->m_ulPrinter1ResultMap |= COPOSControl::PRINTERSTATUSMAP_COVEROPEN;
					}
					else
					{
						pData->dwErrorCode = DEVICEIO_PRINTER_MSG_PRTTBL_ADR;
						m_pOPOSControl->m_ulPrinter1ResultMap |= COPOSControl::PRINTERSTATUSMAP_CHEKPRINTER;
					}
					if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
						BlFwPmgCallBack(pData->dwErrorCode, 0);
					}
				}
				if (bPrintAssert) {
					sprintf (aszErrorBuffer, "## BlInterface (): Error Printing:: Result Code: %d  Extended Code: %d  dwErrorCode: %d", pData->resultCode, pData->extResultCode, pData->dwErrorCode);
					NHPOS_NONASSERT_TEXT(aszErrorBuffer);
				}
				Sleep(50);
			}
		}
	}
	else if (lPrintType == PMG_COM_PRINTER && m_pDevicePrinter)
	{
		UCHAR  tempBuffer[128] = { 0 };
		DWORD  dwLength = 0, dwWritten = 0;
		DEVICEIO_PRINTER_WRITE  data = { 0 };

		switch (PMG_GET_PRT_STATION(pData->lPrintStation)) {
		case PTR_S_RECEIPT:
			data.wPrintStation = PMG_PRT_RECEIPT;    // translate from OPOS definitions to device definitions for printer station
			break;
		case PTR_S_SLIP:
			data.wPrintStation = PMG_PRT_SLIP;    // translate from OPOS definitions to device definitions for printer station
			break;
		}

		for (int i = 0; i < pData->ulLength; i++) {
			tempBuffer[i] = pData->ptchBuffer[i];
		}
		data.lpData = (LPBYTE)tempBuffer;
		data.wLength = pData->ulLength;
		m_pDevicePrinter->Write(tempBuffer, pData->ulLength, &dwWritten);
	}
	else if(lPrintType == PMG_ZEBRA_PRINTER && m_pZebraControl && m_bZebraControlDispatch){
		TCHAR prtBuff[5];
		prtBuff[0] = _T('\x1B');
		prtBuff[1] = _T('C');
		prtBuff[2] = _T('U');
		prtBuff[3] = _T('T');
		prtBuff[4] = 0x00;

		// if pData->ptchbuffer equals ESC+ |75P (the cut sequence)
		// then print whatever is in the buffer, otherwise, just
		// concatenate it.
		CString csTemp = pData->ptchBuffer;
		//AfxMessageBox(CString("\x1B|75P"));

		if (csTemp.Compare(prtBuff) == 0)
		{
    		//m_pZebraControl->Print(pData->ptchBuffer);
			csZebraBuffer += CString("\r\n");
			m_pZebraControl->Print(_T("! U1 PAGE-WIDTH 400\r\n! U1 SETLP 5 0 18\r\n! U1 SETSP 0\r\n! U1 SETLP-TIMEOUT 8\r\n"));

			m_pZebraControl->Print(csZebraBuffer);
			csZebraBuffer = "";				
		}
		else 
		{
			csZebraBuffer += CString("\r\n") += csTemp;
		}
	}
	else {
		// if not a recognized printer then break out of the loop.
		NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_WRITE: Unrecognized type of printer.");
		bResult = FALSE;
	}

	return bResult;
}
// CDeviceEngine::DevicePrinterColumnStatus() to handle case FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS:
BOOL CDeviceEngine::DevicePrinterColumnStatus (VOID * pvData, DWORD dwLength)
{
	static int x = 0, y = 0;
	PFRAMEWORK_IO_PRINTER pData = (PFRAMEWORK_IO_PRINTER)pvData;
	LONG   lRet = 0;
	LONG  lPrintStation = 0;
	LONG  lPrintType = 0;
    BOOL    bResult = 0;
	SHORT  sRetryCount = 1;  // retry loop count for printer retry and failures
//	TCHAR singleModeStr[] = _T("ESC[2ST;5;0!v");    // removed as not used. also would ESC to be 0x1B, escape character, instead.  RJC 11/13/2020

	pData = (PFRAMEWORK_IO_PRINTER)pvData;
	lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
	lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);
	pData->fbStatus = 0;
	pData->m_ulPrinter1StatusMap = 0;

	if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl){
		bResult = TRUE;

		SetdwErrorCode (pData);

		pData->bSlipStationExist = m_pOPOSControl->m_OPOSPrinter.GetCapSlpPresent();
		switch(lPrintStation){
		case PTR_S_RECEIPT:
			pData->lColumns = m_pOPOSControl->m_OPOSPrinter.GetRecLineChars();
			break;
		case PTR_S_JOURNAL:
			pData->lColumns = m_pOPOSControl->m_OPOSPrinter.GetJrnLineChars();
			break;
		case PTR_S_SLIP:
			pData->lColumns = m_pOPOSControl->m_OPOSPrinter.GetSlpLineChars();
			break;
		default:
			bResult = FALSE;
			return bResult;
			break;
		}
	}
	else if (lPrintType == PMG_COM_PRINTER && m_pDevicePrinter)
	{
		// following Warning to ASSRTLOG disabled as is just noise.  Oct-26-2023 RJC
		// NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS: PMG_COM_PRINTER needs.");
		bResult = TRUE;
		pData->bSlipStationExist = FALSE;
		switch (lPrintStation) {
		case PTR_S_RECEIPT:
			pData->lColumns = PMG_THERMAL_CHAR_D;
			break;
		case PTR_S_JOURNAL:
			pData->lColumns = 0;
			break;
		case PTR_S_SLIP:
			pData->lColumns = 0;
			break;
		default:
			bResult = FALSE;
			return bResult;
			break;
		}
	}
	else if (lPrintType == PMG_ZEBRA_PRINTER && m_pZebraControl && m_bZebraControlDispatch) {
		bResult = TRUE;
		pData->bSlipStationExist = FALSE;
		switch (lPrintStation) {
		case PTR_S_RECEIPT:
			pData->lColumns = PMG_THERMAL_CHAR_D;
			break;
		case PTR_S_JOURNAL:
			pData->lColumns = 0;
			break;
		case PTR_S_SLIP:
			pData->lColumns = 0;
			break;
		default:
			bResult = FALSE;
			return bResult;
			break;
		}
	}
	else 
	{
		NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS: Unknown Printer type.");
	}

	return bResult;
}

// CDeviceEngine::DevicePrinterStatus() to handle case FRAMEWORK_IOCTL_PRINTER_STATUS:
BOOL CDeviceEngine::DevicePrinterStatus (VOID * pvData, DWORD dwLength)
{
	PFRAMEWORK_IO_PRINTER pData = (PFRAMEWORK_IO_PRINTER)pvData;
    BOOL    bResult = 0;
	LONG   lRet = 0;
	SHORT  sRetryCount = 1;  // retry loop count for printer retry and failures
	static int x = 0, y = 0;
//	TCHAR singleModeStr[] = _T("ESC[2ST;5;0!v");    // removed as not used. also would ESC to be 0x1B, escape character, instead.  RJC 11/13/2020
	LONG  lPrintStation = 0;
	LONG  lPrintType = 0;

	NHPOS_ASSERT(dwLength == sizeof(FRAMEWORK_IO_PRINTER));
	lPrintStation = PMG_GET_PRT_STATION(pData->lPrintStation);
	lPrintType = PMG_GET_PRT_TYPE(pData->lPrintStation);
	USHORT usRet;

	pData->fbStatus = 0;
	pData->m_ulPrinter1StatusMap = 0;

	if((lPrintType == 0 || lPrintType == PMG_OPOS_PRINTER) && m_pOPOSControl)
	{
		LONG  lOposPrinterStatus = 0;

		SetdwErrorCode (pData);
		bResult = FALSE;
		if ((pData->fbStatus & PRT_STAT_HW_ERROR) == 0) {
			bResult = TRUE;
			// bit PRT_STAT_ERROR is used to indicate NOT an error condition for some reason.
			pData->fbStatus |= (PRT_STAT_ERROR | PRT_STAT_INIT_END);
			pData->dwErrorCode = DEVICEIO_PRINTER_MSG_SUCCESS_ADR;
			BlFwPmgCallBack(pData->dwErrorCode, 1);

			switch(lPrintStation) {
			case PTR_S_RECEIPT:
			case PTR_S_JOURNAL:
				// For receipt and journal printing, when getting
				// the status of the printer, we initially just
				// want to test if the printer is on. If so we allow
				// the print to proceed and catch any other errors
				// in the print method.
				break;

			case PTR_S_SLIP:
			case PMG_PRT_SLIP:
				usRet = m_pOPOSControl->m_OPOSPrinter.GetSlpEmpty();
				if(!usRet)
				{
					pData->fbStatus |= PRT_STAT_FORM_IN;     // Indicate that a form is in the slip station
				}else
				{
					pData->fbStatus &= ~PRT_STAT_FORM_IN;    // Indicate that a form is not in the slip station
				}
				break;

			default:
				pData->fbStatus = PRT_STAT_POWER_OFF;        // Indicate that the unknown device is powered off
				break;
			}
		}
		if ((pData->fbStatus & PRT_STAT_HW_ERROR) != 0) {
			if ((pData->lPrintStation & PMG_PRT_REPORT_ERROR) == 0) {
				BlFwPmgCallBack(pData->dwErrorCode, 0);
			}
		}
	}
	else if ((lPrintType == 0 || lPrintType == PMG_COM_PRINTER) && m_pDevicePrinter)
	{
		// how do we print a bar code with direct to printer?
		NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_STATUS: PMG_COM_PRINTER needs.");
	}
	else if( (lPrintType == 0 || lPrintType == PMG_ZEBRA_PRINTER) && m_pZebraControl && m_bZebraControlDispatch) {
		LONG  lZebraPrinterStatus = m_pZebraControl->GetPrinterStatus();
		switch(lPrintStation){
			case PTR_S_RECEIPT:
			case PTR_S_JOURNAL:
				// For receipt and journal printing, when getting
				// the status of the printer, we initially just
				// want to test if the printer is on. If so we allow
				// the print to proceed and catch any other errors
				// in the print method.
				if( ZEBRA_NORESPONSE_MASK == lZebraPrinterStatus)
				{
					pData->fbStatus |= (PRT_STAT_ERROR | 0x20);
					bResult = FALSE;
				}else
				{
					bResult = TRUE;
				}
				break;

			case PTR_S_SLIP:
			case PMG_PRT_SLIP:
				pData->fbStatus |= (PRT_STAT_ERROR | 0x20);
				bResult = FALSE;
				break;

			default:
				bResult = FALSE;
				break;
		}
	} else if (lPrintType == 0 || lPrintType == PMG_WIN_PRINTER) {
		bResult = TRUE;
	} else {
		// if not a recognized type of printer then break out.of loop.
		NHPOS_ASSERT_TEXT(0, "FRAMEWORK_IOCTL_PRINTER_STATUS: Unrecognized type of printer.");
		bResult = TRUE;
		pData->fbStatus = PRT_STAT_POWER_OFF;
	}

	return bResult;
}
// This function is used to handle messages to DeviceEngine that involve the
// OPOS control and other types of Active-X controls that use the COMM object model
// and hence must be accessed from the main message pump thread and not from some
// other thread.
//
// This is first used with cash drawer as that is where we have seen an immediate problem.
// However other OPOS type devices may need to be added into this function.
// It is used by sending an WM_APP_DEVICEENGINE_IOCTL using the PostThreadMessage() function
//      m_myAppForMessages->PostThreadMessage (WM_APP_DEVICEENGINE_IOCTL, FRAMEWORK_IOCTL_DRAWER_OPEN, pReq->dwNumber);
//
SHORT  CDeviceEngine::HandleIOCTL (WPARAM wParam, LPARAM lParam)
{
	BOOL  bResult = FALSE;

	if (wParam == FRAMEWORK_IOCTL_DRAWER_OPEN) {
		switch(lParam){
		case 1:
			if (!m_pOPOSControl || !m_pOPOSControl->m_hWnd) {
				bResult = TRUE;
			} else if (m_pOPOSControl->m_cashDrawer.GetState() != OPOS_S_CLOSED){
				bResult = (m_pOPOSControl->m_cashDrawer.OpenDrawer() == OPOS_SUCCESS);
			}
			break;
		case 2:
			if (!m_pOPOSControl || !m_pOPOSControl->m_hWnd) {
				bResult = TRUE;
			}
			else if(m_pOPOSControl->m_cashDrawer2.GetState() != OPOS_S_CLOSED){
				bResult = (m_pOPOSControl->m_cashDrawer2.OpenDrawer() == OPOS_SUCCESS);
			}
			break;

		default:
			break;
		}
	} else if (wParam == FRAMEWORK_IOCTL_PRINTER_STATUS) {
		bResult = DevicePrinterStatus ((VOID *)lParam, sizeof(FRAMEWORK_IO_PRINTER));
	} else if (wParam == FRAMEWORK_IOCTL_PRINTER_WRITE) {
		bResult = DevicePrinterWrite ((VOID *)lParam, sizeof(FRAMEWORK_IO_PRINTER));
	} else if (wParam == FRAMEWORK_IOCTL_PRINTER_COLUMN_STATUS) {
		bResult = DevicePrinterColumnStatus ((VOID *)lParam, sizeof(FRAMEWORK_IO_PRINTER));
	}

	BlFwIfDeviceMessageSystemSendWait ((VOID *)lParam);

	return bResult;
}