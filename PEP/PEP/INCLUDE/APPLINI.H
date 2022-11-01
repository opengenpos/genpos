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
* Program Name: APPLINI.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Sep-21-92:00.00.01:H.Yamaguchi: initial                                   
* Jun-51-93:00.00.01:H.Yamaguchi: Adds struct "APPLINI"                                    
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

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/

/* Device Information Flag */

#define APPL_INIT_ALL           0           /* Initialize all device      */
#define APPL_AUDIBLE_TONE       0x0001      /* AC 169 (Set up Tone value) */

/*
*---------------------------------------------------------------------------
*   Define Return Code 
*---------------------------------------------------------------------------
*/

/* User interface error code */

#define APPL_SUCCESS         0              /* Success     */
#define APPL_ERROR_FAIL     -1              /* Failed      */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/

typedef    struct {
    USHORT    usParaF;      /* Other parameter */
    USHORT    usStatus;     /* Status */
}APPLINI;

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
SHORT  ApplSetupDevice(USHORT usDevice);    /* Setup all or a degignate device */
SHORT  Appl_AudibleTone(VOID);              /* Setup audible tone */

/*========================================
    external work
========================================*/
extern  APPLINI ApplTrace;                 /* Halt Log */

/*========== END OF DEFINITION ============*/
