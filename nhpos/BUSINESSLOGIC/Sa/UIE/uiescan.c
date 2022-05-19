/*
*===========================================================================
*
*   Copyright (c) 1992-1997, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Scanner Module
* Category    : User Interface Enigne, 2170 System Application
* Program Name: UieScan.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     The provided function names are as follows:
*
*                   UieNotonFile() - judgement enable/disable
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial
* Sep-30-92  00.01.05  O.Nakada     Modify scanner open data in
*                                   UieScanner().
* May-25-93  C1        O.Nakada     Removed input device control functions.
* Jun-05-93  C1        O.Nakada     Supports NCR 7852, 7870 and 7890.
* Mar-15-95  G0        O.Nakada     Modified all modules, correspond to
*                                   hospitality release 3.0 functions.
* May-29-97  G10       O.Nakada     Supports NCR 7870, 7875 and 7880
*                                   scanner/scale using single cable.
* Jul-04-97  G10'      O.Nakada     Modified scale zero weight control 
*                                   in UieScannerScale().
*
*** NCR 2172 ***
* Sep-29-99  01.00.00  M.Teraki     Initial
* Apr-04-00            M.Teraki     Modified to fix a bug. in UiePutScannerData()
*                                   'UIE_ENA_MSR' -> 'UIE_ENA_SCANNER'
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
#include <string.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "fsc.h"
#include "uie.h"
#include "uiel.h"
#include "uieio.h"
#include "uiering.h"
#include "BLFWif.h"

//      An Overview of Scanners Supported by GenPOS  FSC_SCANNER, PERIPHERAL_DEVICE_SCANNER and SCF_TYPE_SCANNER
//
// Two kinds of scanners are supported. The first is the older NCR scanner or scanner/scale
// which is a legacy from the NCR 2170 and NCR 7448 with NHPOS. The second is the newer
// handheld keyboard wedge type of scanner such as the Symbol hand held scanner.
//
// For older NCR scanner or scanner/scale see CUieScan::UieScannerThread() which contains the
// necessary logic to read in from a COM port scanner data and to then parse it in order to
// read the bar code digits.
//
// For newer USB keyboard wedge type scanners such as the Symbol, that logic is in 
// function CDeviceEngine::ConvertKey(). See also UIE_VIRTUALKEY_EVENT_SCANNER
//
// For the older NCR scanner, the scanner must be provisioned to transform the raw digits of
// the bar code label into an acceptable format for the CUieScan class. This means that:
//   - Enable SANYO ID Characters must be turned on (analyses bar code as to type and adds letter prefix)
//   - provide a single prefix character with the STX (0x02) or Start of Transmission character
//   - provide a single suffix character with the ETX (0x03) or End of Transmission character
//
// For the newer USB keyboard wedge scanner, the scanner must be provisioned to provide a prefix
// character and a suffix character so that the keyboard wedge functionality can isolate the bar
// code digits from the incoming stream of data.
//   - provide a single prefix character of capital letter M (0x4D)
//   - provide a single suffic character of carriage return (0x0D)

/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
STATIC  UIECOM  UieScanner1Status =
                    {1,                      /* Device ID             */
                     0,                      /* Port ID               */
                     -1,                     /* Port Handle           */
                     UIE_SCANNER_UNKNOWN,    /* Type                  */
                     0,                      /* Request Status        */
                     PIF_COM_PROTOCOL_NON,   /* non Protocol          */
                     UIE_SCANNER1_BAUD,      /* 9600 Baud             */
                     UIE_SCANNER1_BYTE,      /* Odd, 1 Stop, 7 Data   */
                     PIF_COM_FORMAT_TEXT,    /* Text Format           */
                     UIE_SCANNER_SCALE_NONE, /* phase of scale        */
                     0xFFFF,                 /* exclusive semaphore   */
                     0xFFFF,                 /* synchronous semaphore */
                     NULL,                   /* scale data            */
                     0};                     /* scale result code     */

STATIC  UIECOM  UieScanner2Status =
                    {2,                      /* Device ID             */
                     0,                      /* Port ID               */
                     -1,                     /* Port Handle           */
                     UIE_SCANNER_UNKNOWN,    /* Type                  */
                     0,                      /* Request Status        */
                     PIF_COM_PROTOCOL_NON,   /* non Protocol          */
                     UIE_SCANNER1_BAUD,      /* 9600 Baud             */
                     UIE_SCANNER1_BYTE,      /* Odd, 1 Stop, 7 Data   */
                     PIF_COM_FORMAT_TEXT,    /* Text Format           */
                     UIE_SCANNER_SCALE_NONE, /* phase of scale        */
                     0xFFFF,                 /* exclusive semaphore   */
                     0xFFFF,                 /* synchronous semaphore */
                     NULL,                   /* scale data            */
                     0};                     /* scale result code     */

STATIC  UIECOM  *pUieScannerScale = NULL;    /* scale interface       */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID UieScannerInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID UieScannerInit(VOID)
{
}


/*
*===========================================================================
** Synopsis:    USHORT DLLENTRY UieNotonFile(USHORT usControl)
*               usControl - new scanner status
*                   UIE_ENABLE  - not-on-file disable
*                   UIE_DISABLE - normal mode
*
** Return:      previous scanner status
*               UIE_ENABLE  - scanner not-on-file disable
*               UIE_DISABLE - normal mode
*
** Description: 
*
*               Not-On-File is a message sent to the scanner to trigger
*               a Not On File tone or beep to be played by the scanner.
*               This tone is used to inform the operator that the bar code
*               scanned is not on file or does not exist in the PLU database.
*               The scanner doesn't know if a PLU number exists or not and
*               merely provides the scanned bar code. The POS software has
*               to do the lookup and handle whether the bar code is on file
*               or not.
*===========================================================================
*/
USHORT DLLENTRY UieNotonFile(USHORT usControl)
{
    USHORT  usPrevious;
	BOOL	bResult;

    /* ----- get scanner previous status ----- */
    if (fchUieDevice & UIE_ENA_SCANNER) {                           /* enable scanner */
        usPrevious = UIE_DISABLE;                                   /* normal mode */
    } else {                                                        /* disable scanner */
        usPrevious = UIE_ENABLE;                                    /* scanner not-on-file disable */
    }

    if (usControl == UIE_ENABLE) {                                  /* not-on-file, scanner disable */
        fchUieDevice &= ~UIE_ENA_SCANNER;

        /* ----- reset request status of soft reset and disable ----- */
        UieScanner1Status.fchRequest &= ~(UIE_SCANNER_REQ_ENA | UIE_SCANNER_REQ_DIS);
        UieScanner2Status.fchRequest &= ~(UIE_SCANNER_REQ_ENA | UIE_SCANNER_REQ_DIS);

        /* ----- set request status of not-on-file disable ----- */
        UieScanner1Status.fchRequest |= UIE_SCANNER_REQ_NOF_ENA;
        UieScanner2Status.fchRequest |= UIE_SCANNER_REQ_NOF_ENA;

		bResult = BlFwIfScannerNOF(usControl);						/* 02/03/01 */
    } else {                                                        /* soft reset */
        fchUieDevice |= UIE_ENA_SCANNER;

        UieScannerEnable();
    }

    return (usPrevious);
}


/*
*===========================================================================
** Synopsis:    VOID UieScannerEnable(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID UieScannerEnable(VOID)
{
	BOOL	bResult;

    /* ----- reset request status of not-on-file disable or disable ----- */
    UieScanner1Status.fchRequest &= ~(UIE_SCANNER_REQ_NOF_ENA |
                                      UIE_SCANNER_REQ_DIS);
    UieScanner2Status.fchRequest &= ~(UIE_SCANNER_REQ_NOF_ENA |
                                      UIE_SCANNER_REQ_DIS);

    /* ----- set request status of soft reset ----- */
    UieScanner1Status.fchRequest |=  UIE_SCANNER_REQ_ENA;
    UieScanner2Status.fchRequest |=  UIE_SCANNER_REQ_ENA;

	bResult = BlFwIfDeviceControl(BLFWIF_SCANNER, TRUE);
}


/*
*===========================================================================
** Synopsis:    VOID UieScannerDisable(VOID)
*
** Return:      nothing
*
** Description: 
*===========================================================================
*/
VOID UieScannerDisable(VOID)
{
	BOOL	bResult;

    /* ----- reset request status of not-on-file disable or soft reset ----- */
    UieScanner1Status.fchRequest &= ~UIE_SCANNER_REQ_ENA;
    UieScanner2Status.fchRequest &= ~UIE_SCANNER_REQ_ENA;

    /* ----- set request status of disable ----- */
    UieScanner1Status.fchRequest |= UIE_SCANNER_REQ_DIS;
    UieScanner2Status.fchRequest |= UIE_SCANNER_REQ_DIS;

	bResult = BlFwIfDeviceControl(BLFWIF_SCANNER, FALSE);
}


/*
*===========================================================================
** Synopsis:    LONG UiePutScannerData(ULONG ulLength,
*                                      CONST UCHAR *pData,
*                                      VOID *pResult);
*
*     Input:    ulLength - length of data from pData
*               pData    - SCANNER data from external module
*
*    Output:    pResult - reserved
*
** Return:      reserved
*
** Description: Accept SCANNER data and write to the ring buffer
*===========================================================================
*/
LONG UiePutScannerData(ULONG ulLength, CONST UCHAR *pData, VOID *pResult)
{
    FSCTBL  Ring;
    LONG	lRet = -1;	/* return value to caller */

    /* --- if SCANNER is disabled, ignore data --- */

    if (!(fchUieDevice & UIE_ENA_SCANNER)) {
        return lRet;
    }

    /* --- write data into ring buffer --- */

    Ring.uchMajor = FSC_SCANNER;
    Ring.uchMinor = 0;

    UieWriteRingBuf(Ring, pData, (USHORT)ulLength);

    return lRet;
}

/* ====================================== */
/* ========== End of UieScan.C ========== */
/* ====================================== */
