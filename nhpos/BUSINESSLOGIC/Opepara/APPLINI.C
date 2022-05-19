/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Application Initialize Module                         
* Category    : NCR 2170 US Hospitality Application         
* Program Name: APPLINI.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*             Applini()            : Initialize device 
*              Appl_AudibleTone()  : Audible Tone( AC 169)
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Sep-18-92:00.00.01:H.Yamaguchi: initial                                   
*          :        :           :                                    
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
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <pif.h>
#include <log.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <Applini.h>

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
                                                                                
APPLINI ApplTrace;                 /* Halt Log */
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    SHORT  ApplSetupDevice(USHORT usDevice)
*     Input:    usDevice   
*     Output:   nothing
*     InOut:    nothing
*
** Return:      SUCCESS or FAIL
*
** Description: Initialize device.
*===========================================================================
*/
SHORT  ApplSetupDevice(USHORT usDevice)
{
    SHORT   sError;

    sError = APPL_SUCCESS;

    switch (usDevice) {

    case  APPL_INIT_ALL :               /* Initialize all device */

        sError = Appl_AudibleTone();    /* Setup tone value */
        break;
    
    case  APPL_AUDIBLE_TONE :           /* AC 169 (Set up Tone value) */

        sError = Appl_AudibleTone();    /* Setup tone valu */
        break;

    default :
        break;
   }
   return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT  Appl_AudibleTone(VOID)
*     Input:    nothing   
*     Output:   nothing
*     InOut:    nothing
*
** Return:      SUCCESS or FAIL
*
** Description: Initialize audible tone device.
*===========================================================================
*/
SHORT  Appl_AudibleTone(VOID)
{
    PARATONECTL  Tone;

    Tone.uchMajorClass = CLASS_PARATONECTL;
    Tone.uchAddress    = 1;
    CliParaRead(&Tone);                       /* Read parameter     */
    if (Tone.uchToneCtl == MAX_TONE_VOL) {
        Tone.uchToneCtl = 0x04;
    }
    PifToneVolume((USHORT)Tone.uchToneCtl);   /* Setup tone value   */
    return(APPL_SUCCESS);
}


/*====== End of Source File ======*/
