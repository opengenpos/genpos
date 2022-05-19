/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintPreModeIn Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPREMD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*              MaintPreModeInPrg() : control when mode lock is in program mode.
*              MiantPreModeInSup() : control when mode lock is in supervisor mode.   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-30-92: 01.00.00 : J.Ikeda   : initial                                   
* Dec-10-92: 01.00.00 : K.You     : Adds R/J Control                                   
* Dec-14-98: 03.03.06 : hrkato    : V3.3 FVT Comment #4
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
/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include "ecr.h"
#include "rfl.h"
#include "pif.h"
#include "uie.h"
#include "plu.h"
#include "csstbfcc.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csstbpar.h"

#include "maintram.h"

/**
==============================================================================
;                  R A M   A R E A   D E C L A R A T I O Ns                         
=============================================================================
**/

MAINTWORK MaintWork;          /* union */

UCHAR     uchMaintOpeCo;                  /* operational counter */
ULONG     ulMaintSupNumber = 0;           /* current supervisor employee number */
USHORT    usMaintSupNumberInternal = 0;
USHORT    usMaintSupLevel = 0;            /* supervisor level */
UCHAR     uchMaintMldPageNo;              /* displayed page no R3.1 */

/*
*===========================================================================
** Synopsis:    VOID MaintPreModeInPrg( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*               
*
** Description: This function controls when mode lock is in program mode. 
*
*===========================================================================
*/

VOID MaintPreModeInPrg( VOID )
{
    UCHAR           uchMajorClass;
	MAINTDSPFLINE   MaintDspFLine = { 0 };
	MAINTREDISP     ReDisplay = { 0 };

    /* clear self-work */
    memset(&MaintWork, 0, sizeof(MAINTWORK));

    /* drawer compulsory disabled */    
    UieDrawerComp(UIE_DISABLE);                  
 
    /* Display "P" Character */
    uchMaintMenuPage = 0;
    MaintDspFLine.uchMajorClass = CLASS_MAINTDSPFLINE; 
    MaintDspFLine.uchMinorClass = CLASS_MAINTPREMD_OPPROG; /* V3.3 */
    MaintDspFLine.aszMnemonics[0] = 'P';
    MaintDspFLine.aszMnemonics[1] = '\0';
    DispWrite(&MaintDspFLine);                   /* call DispSupFLine() */ 

    /* Set R/J Column */
    uchMajorClass = CLASS_ITEMMODEIN;
    PrtPrintItem(NULL, &uchMajorClass);
 
    /* Display "CLOSED" Character */
    MaintDspFLine.uchMajorClass = CLASS_MAINTDSPFLINE; 
    MaintDspFLine.uchMinorClass = CLASS_MAINTPREMD_CUS;
	RflGetLead(MaintDspFLine.aszMnemonics, LDT_CUSCLOSE_ADR); /* R2.0 */
    DispWrite(&MaintDspFLine);                   /* call DispSupFLine() */ 

    /* Reset All Descriptor */
    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
    MaintReDisp(&ReDisplay);

}

/*
*===========================================================================
** Synopsis:    VOID MaintPreModeInSup( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*               
*
** Description: This function controls when mode lock is in supervisor mode.
*===========================================================================
*/

VOID MaintPreModeInSup( VOID )
{
    UCHAR           uchMajorClass;
	MAINTDSPFLINE   MaintDspFLine = { 0 };
	MAINTREDISP     ReDisplay = { 0 };
	PARACTLTBLMENU  MenuPage = { 0 };           /* V3.3 */
    FSCTBL FAR      *pData;             /* V3.3 */

    /* clear self-work */
    memset(&MaintWork, 0, sizeof(MAINTWORK));

    /* drawer compulsory disabled */    
    UieDrawerComp(UIE_DISABLE);                  
                                              
    /* Display "SUPER" Character */

    /* --- Set Shift Page,  V3.3 --- */
    MenuPage.uchMajorClass = CLASS_PARACTLTBLMENU;
    MenuPage.uchAddress = CTLPG_STAND_ADR;
    CliParaRead(&MenuPage);
    if (MenuPage.uchPageNumber < 1 || MenuPage.uchPageNumber > MAX_PAGE_NO) {
        MenuPage.uchPageNumber = 1;
    }
    pData = (FSCTBL *)&ParaFSC[MenuPage.uchPageNumber - 1];
    UieSetFsc(pData);
    uchMaintMenuPage = MenuPage.uchPageNumber;
    /* --- Set Shift Page,  V3.3 --- */

    MaintDspFLine.uchMajorClass = CLASS_MAINTDSPFLINE; 
    MaintDspFLine.uchMinorClass = CLASS_MAINTPREMD_OP;
	RflGetLead(MaintDspFLine.aszMnemonics, LDT_SUPMSG_ADR);    /* "SUPER" for 2x20, R2.0 */
    DispWrite(&MaintDspFLine);                   /* call DispSupFLine() */ 

    /* Set R/J Column */
    uchMajorClass = CLASS_ITEMMODEIN;
    PrtPrintItem(NULL, &uchMajorClass);
 
    /* Display "CLOSED" Character */
    MaintDspFLine.uchMajorClass = CLASS_MAINTDSPFLINE;
    MaintDspFLine.uchMinorClass = CLASS_MAINTPREMD_CUS;
	RflGetLead(MaintDspFLine.aszMnemonics, LDT_CUSCLOSE_ADR);    /*  "CLOSED" for 2x20, R2.0 */
    DispWrite(&MaintDspFLine);                   /* call DispSupFLine() */ 

    /* Reset All Descriptor */
    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
    MaintReDisp(&ReDisplay);

}

/*
*======================================================================================
**   Synopsis:  USHORT MaintGetACNumber(VOID)
*
*       Input:  Nothing
*      Output:  Nothing
*
**  Return:     Menu Page
*
**  Description: Get Action Code                                        Saratoga
*======================================================================================
*/
USHORT  MaintGetACNumber(VOID)
{
    return(usMaintACNumber);
}

/* --- End of Source --- */