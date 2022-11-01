/*mhmh
**********************************************************************
**                                                                  **
**  Title:      The other functions on Platform Interface           **
**                                                                  **
**  File Name:  PIFOTHER.C                                          **
**                                                                  **
**  Categoly:   Poratable Platform Interface of PC Interface        **
**                                                                  **
**  Abstruct:   This module provides the other functions of         **
**              Platform Interface.                                 **
**              This module provides the following functions:       **
**                                                                  **
**                PifXinuInit():    Initialize Platform Interface   **
**                PifLog():         Log Exception                   **
**                                                                  **
**                LibMain():        initialize DLL                  **
**                                                                  **
**              But, the function "PifLog()" don't provided the     **
**              feature.                                            **
**                                                                  **
**  Compiler's options: /c /ACw /W4 /Zp /Ot /G1s                    **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:                                           **
**                                                                  **
**      Ver.      Date        Name      Description                 **
**      1.00    Aug/17/92   T.Kojima    Initial release             **
**              Jun/15/98   O.Nakada    Win32                       **
**                                                                  **
**********************************************************************
mhmh*/

/*******************\
*   Include Files   *
\*******************/
#ifndef	WIN32
#include    <windows.h>
#endif
#include    "XINU.H"    /* XINU */
#define     PIFXINU     /* switch for "PIF.H" */
#include    "PIF.H"     /* Portable Platform Interface */
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */

/****************************\
*   Global Static Variables *|
\****************************/
/* device number of drivers */
SHORT   sDev_net;       /* net  */
SHORT   sDev_yam;       /* yam  */

/* system configuration         */
SYSCONFIG   SysConfig;

SHORT	fsNetStatus;	/* network control, 2172 */

#ifndef	WIN32
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   int FAR PASCAL LibMain (HANDLE hInstance,           **
**                                      WORD wDataSeg,              **
**                                      WORD wHeapSize,             **
**                                      LPSTR lpszCmdLine)          **
**                                                                  **
**  Return:     1                                                   **
**                                                                  **
** Description: This function initializes the DLL.                  **
**                                                                  **
**********************************************************************
fhfh*/
int FAR PASCAL LibMain (HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
{
    /* initialize   */
    if (wHeapSize > 0)  UnlockData(0);

    /* initialize device */
    XinuSysInit();

    /* initialize Platform Interface */
    PifXinuInit(FALSE);

    return(1);

    hInstance   = hInstance;
    wDataSeg    = wDataSeg;
    lpszCmdLine = lpszCmdLine;
}
/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID FAR PASCAL WEP(int nParameter);                **
**                                                                  **
** Description: termination function                                **
**                                                                  **
**********************************************************************
fhfh*/
VOID FAR PASCAL WEP(int nParameter)
{
    return;

    nParameter = nParameter;
}
#endif	/* WIN32 */

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifXinuInit(BOOL fOsType);                     **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: This function initializes the Platform Interface.   **
**                                                                  **
**********************************************************************
fhfh*/
VOID PifXinuInit(BOOL fOsType)
{
    /* get YAM device number    */
    sDev_yam = XinuGetDev("yam");

    /* set local address    */
    SysConfig.auchLaddr[0] = 0xc0;
    SysConfig.auchLaddr[1] = 1;
    SysConfig.auchLaddr[2] = 0;
    SysConfig.auchLaddr[3] = 1;

    /* get NET device number    */
    sDev_net = XinuGetDev("net");

    /* initialize network 
    XinuNetStart(SysConfig.auchLaddr);
    */
    return;
    
    fOsType = fOsType;
}

/////////////////////////////////////////////////////////////////////////////
/*
**********************************************************************
**                                                                  **
**  Synopsis:   SYSCONFIG CONST FAR * PIFENTRY PifSysConfig(VOID);  **
**                                                                  **
**  Return:     pointer to the structure of system configuration    **
**                                                                  **
** Description: Get System Configuration returns the pointer to the **
**              structure of system configuration preserved by      **
**              system.                                             **
**                                                                  **
**********************************************************************
fhfh*/
SYSCONFIG CONST FAR *PIFENTRY PifSysConfig(VOID)
{
    return((SYSCONFIG FAR *)&SysConfig);
}
