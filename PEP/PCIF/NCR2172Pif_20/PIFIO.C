/*mhmh
**********************************************************************
**                                                                  **
**  Title:      The other communication on Platform Interface       **
**                                                                  **
**  File Name:  PIFIO.C                                             **
**                                                                  **
**  Categoly:   Poratable Platform Interface of PC Interface        **
**                                                                  **
**  Abstruct:   This module provides the other communication        **
**              functions of Platform Interface.                    **
**              This module provides the following functions:       **
**                                                                  **
**                PifGetChar():         Get Key Data and Keylock    **
**                                      Position                    **
**                PifControlKeyboard(): Control Keyboard            **
**                PifGetKeyLock():      Get Keylock Position        **
**                PifGetWaiter():       Get Waiter ID               **
**                PifDisplayString():   Display String              **
**                PifDisplayAttr():     Display String with         **
**                                      Attributes                  **
**                PifLightDescr():      Light Descriptor            **
**                PifOpenDrawer():      Open Drawer                 **
**                PifDrawerStatus():    Get Drawer Status           **
**                PifBeep():            Play Note                   **
**                PifPlayStanza():      Play Stanza                 **
**                PifToneVolume():      Adjust Tone Volume          **
**                PifReadMsr():         Read MSR                    **
**                                                                  **
**              But, the all function don't provided the feature.   **
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

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifGetChar(CHARDATA FAR *pInputChar); **
**              pInputChar: pointer to the structure of key or key  **
**                          lock data                               **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: Get Character from Keyboard get one key data from   **
**              the keyboard.                                       **
**                                                                  **
***********************************************************************
fhfh*/
VOID PIFENTRY PifGetChar(CHARDATA FAR *pInputChar)
{
    return;

    pInputChar = pInputChar;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifControlKeyboard(USHORT usFunc);    **
**              usFunc: function ID                                 **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: enable and disable the keyboard.                    **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifControlKeyboard(USHORT usFunc)
{
    return;

    usFunc = usFunc;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   UCHAR PIFENTRY PifGetKeyLock(VOID);                 **
**                                                                  **
**  Return:     Key Lock Position                                   **
**                                                                  **
** Description: returns the current key lock position.              **
**                                                                  **
**********************************************************************
fhfh*/
UCHAR PIFENTRY PifGetKeyLock(VOID)
{
    return(0);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   SHORT PIFENTRY PifGetWaiter(VOID);                  **
**                                                                  **
**  Return:     0 (Waiter ID)                                       **
**                                                                  **
** Description: returns the current waiter ID.                      **
**                                                                  **
**********************************************************************
fhfh*/
SHORT PIFENTRY PifGetWaiter(VOID)
{
    return(0);
}

/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   VOID PIFENTRY PifDisplayString(USHORT usDispId,             **
**                                             USHORT usRow,                **
**                                             USHORT usColumn,             **
**                                             CONST UCHAR FAR *puchString, **
**                                             USHORT usChars,              **
**                                             UCHAR uchAttr);              **
**              usDispId:   display ID (operator/customer)                  **
**              usRow:      row position                                    **
**              usColumn:   column position                                 **
**              puchString: pointer to character string                     **
**              usChars:    number of character                             **
**              uchAttr:    display attribute                               **
**                                                                          **
**  Return:     None                                                        **
**                                                                          **
** Description: displays the string according as each argument.             **
**                                                                          **
******************************************************************************
fhfh*/
VOID PIFENTRY PifDisplayString(USHORT usDispId, USHORT usRow, USHORT usColumn,
                               CONST UCHAR FAR *puchString, USHORT usChars,
                               UCHAR uchAttr)
{
    return;

    usDispId   = usDispId;
    usRow      = usRow;
    usColumn   = usColumn;
    puchString = puchString;
    usChars    = usChars;
    uchAttr    = uchAttr;
}

/*fhfh
******************************************************************************
**                                                                          **
**  Synopsis:   VOID PIFENTRY PifDisplayAttr(USHORT usDispId,               **
**                                           USHORT usRow,                  **
**                                           USHORT usColumn,               **
**                                           CONST UCHAR FAR *puchString,   **
**                                           USHORT usChars);               **
**              usDispId:   display ID (operator/customer)                  **
**              usRow:      row position                                    **
**              usColumn:   column position                                 **
**              puchString: pointer to character and attribute string       **
**              usChars:    number of character                             **
**                                                                          **
**  Return:     None                                                        **
**                                                                          **
** Description: displays the string according as the specified display      **
**              attributes by chracter unit.                                **
**                                                                          **
******************************************************************************
fhfh*/
VOID PIFENTRY PifDisplayAttr(USHORT usDispId, USHORT usRow, USHORT usColumn,
                             CONST UCHAR FAR *puchString, USHORT usChars)
{
    return;

    usDispId   = usDispId;
    usRow      = usRow;
    usColumn   = usColumn;
    puchString = puchString;
    usChars    = usChars;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifLightDescr(USHORT usDispId,        **
**                                          USHORT usDescrId,       **
**                                          UCHAR uchAttr);         **
**              usDispId:   display ID (operator/customer)          **
**              usDescrId:  descriptor position                     **
**              uchAttr:    attribute                               **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: turns on, turns off and blinks the specified        **
**              descriptor.                                         **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifLightDescr(USHORT usDispId, USHORT usDescrId, UCHAR uchAttr)
{
    return;

    usDispId  = usDispId;
    usDescrId = usDescrId;
    uchAttr   = uchAttr;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifOpenDrawer(USHORT usDrawerId);   **
**              usDrawerId: drawer ID (1 or 2)                      **
**                                                                  **
**  Return:     PIF_DRAWER_NOT_PROVIDED                             **
**                                                                  **
** Description: opens the specified drawer.                         **
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifOpenDrawer(USHORT usDrawerId)
{
    return(PIF_DRAWER_NOT_PROVIDED);
    
    usDrawerId = usDrawerId;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHORT PIFENTRY PifDrawerStatus(USHORT usDrawerId); **
*               usDrawerId: drawer ID (1 or 2)                      **
**                                                                  **
**  Return:     PIF_DRAWER_CLOSED                                   **
**                                                                  **
** Description: returns the status of specified drawer.             **
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifDrawerStatus(USHORT usDrawerId)
{
    return(PIF_DRAWER_CLOSED);
    
    usDrawerId = usDrawerId;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifBeep(USHORT usMsec);               **
*               usMsec: duration of beep (unit: mS)                 **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: Beep sounds the beep for the specified duration.    **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifBeep(USHORT usMsec)
{
    return;

    usMsec = usMsec;
}


/*fhfh
**************************************************************************
**                                                                      **
**  Synopsis:   VOID PIFENTRY PifPlayStanza(CONST STANZA FAR *pStanza); **
**              pStanza:    pointer to the structure of a pairs of      **
**                          duration and frequency                      **
**                                                                      **
**  Return:     None                                                    **
**                                                                      **
** Description: sounds the beep according as the structure.             **
**                                                                      **
**************************************************************************
fhfh*/
VOID PIFENTRY PifPlayStanza(CONST STANZA FAR *pStanza)
{
    return;

    pStanza = pStanza;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifToneVolume(USHORT usVolume);       **
**              usVolume:   volume (0 to 7)                         **
**                                                                  **
**  Return:     None                                                **
**                                                                  **
** Description: adjusts the volume to the specified volume.         **
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifToneVolume(USHORT usVolume)
{
    return;

    usVolume = usVolume;
}

/*fhfh
**************************************************************************
**                                                                      **
**  Synopsis:   SHORT PIFENTRY PifReadMsr(MSR_BUFFER FAR *pMsrBuffer);  **
**              pMarBuffer:  pointer to the input buffer                **
**                                                                      **
**  Return:     PIF_ERROR_MSR_NOT_PROVIDED                              **
**                                                                      **
** Description: Read MSR reads the buffer of the MSR                    **
**                                                                      **
**************************************************************************
fhfh*/
SHORT PIFENTRY PifReadMsr(MSR_BUFFER FAR *pMsrBuffer)
{
    return(PIF_ERROR_MSR_NOT_PROVIDED);

    pMsrBuffer = pMsrBuffer;
}
