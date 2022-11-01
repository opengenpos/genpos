/*mhmh
**********************************************************************
**                                                                  **
**  Title:      PopUp to error message                              **
**                                                                  **
**  File Name:  STUBPOP.C                                           **
**                                                                  **
**  Categoly:   PC stub                                             **
**                                                                  **
**  Abstruct:   This module provides the  function of popup of      **
**              error message.                                      **
**              This module provides the following functions:       **
**                                                                  **
**                StubPopUp():      PopUp to error message          **
**                                                                  **
**  Compiler's options: /c /ACw /W4 /Zp /Ot /G1s                    **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:                                           **
**                                                                  **
**      Ver.      Date        Name      Description                 **
**      1.00    Sep/13/92   T.Kojima    Initial release             **
**                                                                  **
**********************************************************************
mhmh*/

/*******************\
|   Include Files   |
\*******************/
#include    <windows.h>
#include    "ecr.h"
//#include    "pif.h"
# include   "r20_pif.h"
#include    "csstbfcc.h"
#include    "stbpopup.h"

/*******************\
|   Error Message   |
\*******************/
char achErrMasterDown[]  = "Master Down!";
char achErrBMasterDown[] = "B-Master Down!";
char achErrBusy[]        = "Terminal Busy!";
char achErrTimeout[]     = "Timeout!";
char achErrCom[]         = "Communication Error!";

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   int StubPopUp(int iError)                           **
**                                                                  **
**  Return:     TRUE:   depressed cancel bottun                     **
**              FALSE:  depressed retry bottun                      **
**                                                                  **
** Description: This function output the popup.                     **
**                                                                  **
**********************************************************************
fhfh*/
int StubPopUp(int iError)
{
    HWND    hwnd;
    char    *pchErr;
    WORD    wType = MB_RETRYCANCEL | MB_ICONEXCLAMATION;

    switch(iError) {
    case STUB_M_DOWN:
        pchErr = achErrMasterDown;
        break;

    case STUB_BM_DOWN:
        pchErr = achErrBMasterDown;
        break;

    case STUB_BUSY_MULTI:
    case STUB_BUSY:
        pchErr = achErrBusy;
        break;

    case STUB_TIME_OUT:
        pchErr = achErrTimeout;
        break;

    default:
        pchErr = achErrCom;
    }

    hwnd = GetActiveWindow();
    
    if (MessageBox(hwnd, pchErr, "Warnning!", wType) == IDCANCEL) {
        return(TRUE);
    } else {
        return(FALSE);
    }
}

