/*mhmh
**********************************************************************
**                                                                  **
**  Title:      The time control on Platform Interface              **
**                                                                  **
**  File Name:  PIFTIME.C                                           **
**                                                                  **
**  Categoly:   Poratable Platform Interface of PC Interface        **
**                                                                  **
**  Abstruct:   This module provides the time control functions of  **
**              Platform Interface.                                 **
**              This module provides the following functions:       **
**                                                                  **
**                PifSleep():           Wait for a time             **
**                PifGetDateTime():     Get Date & Time             **
**                PifSetDateTime():     Set Date & Time             **
**                                                                  **
**              But, the "PifSetDateTime()" function don't provided **
**              the feature.                                        **
**                                                                  **
**  Compiler's options: /c /ACw /W4 /Zp /Ot /G1s                    **
**                                                                  **
**********************************************************************
**                                                                  **
**  Modification History:                                           **
**                                                                  **
**      Ver.      Date        Name      Description                 **
**      1.00    Aug/17/92   T.Kojima    Initial release             **
**                                                                  **
**********************************************************************
mhmh*/

/*******************\
*   Include Files   *
\*******************/
#include    "XINU.H"    /* XINU */
#define     PIFXINU     /* switch for "PIF.H" */
#include    "PIF.H"     /* Portable Platform Interface */
#include    "LOG.H"     /* Fault Codes Definition */
#include    "PIFXINU.H" /* Platform Interface */
#include    <time.h>    /* for getting the current date & time  */

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSleep(USHORT usMsec);              **
**              usMsec:     sleep time (unit: mS)                   **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: stops the current thread for the specified time.    **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifSleep(USHORT usMsec)
{
    XinuSleepm(usMsec);

    return;    
}

/*fhfh
**************************************************************************
**                                                                      **
**  Synopsis:   VOID PIFENTRY PifGetDateTime(DATE_TIME FAR *pDateTime); **
**              pDateTime:   pointer to the structure of date and time. **
**                                                                      **
**  Return:     None                                                    **
**                                                                      **
** Description: gets the current date and time.                         **
**                                                                      **
**************************************************************************
fhfh*/
VOID PIFENTRY PifGetDateTime(DATE_TIME FAR *pDateTime)
{
    time_t  Time;
    struct  tm  *pdt;

    /* get the current date & time  */
    time(&Time);
    pdt = localtime(&Time);

    pDateTime->usSec   = (USHORT)pdt->tm_sec; 
    pDateTime->usMin   = (USHORT)pdt->tm_min; 
    pDateTime->usHour  = (USHORT)pdt->tm_hour; 
    pDateTime->usMDay  = (USHORT)pdt->tm_mday; 
    pDateTime->usMonth = (USHORT)(pdt->tm_mon + 1); 
    pDateTime->usYear  = (USHORT)(pdt->tm_year + 1900); 
    pDateTime->usWDay  = (USHORT)pdt->tm_wday; 

    return;    
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifSetDateTime                        **
**                              (CONST DATE_TIME FAR *pDateTime);   **
**              pDateTime:  pointer to the structure of date and    **
**                          time                                    **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: sets the clock for the specified date and time.     **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifSetDateTime(CONST DATE_TIME FAR *pDateTime)
{
    return;

    pDateTime = pDateTime;
}
