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
* Title       : Set Menu Shift Page Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATSTPG.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               MaintSetPage()      : Set and Display Menu Shift Page    
*               MaintGetPage()      : Get Menu Shift Page    
*               MaintSetPageOnly()  : Set Menu Shift Page    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-29-92:00.00.01    :K.You      : initial                                   
*          :            :                                    
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
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>

#include "maintram.h"


/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

UCHAR  uchMaintMenuPage;                        /* Menu Shift Page Save Area */
                                                

/*
*======================================================================================
*
**   Synopsis:  SHORT MaintSetPage( MAINTSETPAGE *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTSETPAGE  
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_KEYOVER_ADR : Wrong Data
*               
**  Description: Check/Set Menu Shift Page and Display.  
*======================================================================================
*/

SHORT MaintSetPage( MAINTSETPAGE *pData )
{


    /* Check W/o Amount Case */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* W/o Amount Case */
        uchMaintMenuPage++;
        if (uchMaintMenuPage > MAX_PAGE_NO) {               
            uchMaintMenuPage = 1;                           /* Initialize Menu Page */
        }
    } else {                                                /* W/ Amount Case */
        if (pData->uchPageNumber == 0 || pData->uchPageNumber > MAX_PAGE_NO) {  /* Over Digit */
            return(LDT_KEYOVER_ADR);                                            
        }

        /* Save Menu Page Data */
    
        uchMaintMenuPage = pData->uchPageNumber;

    }

    /* Display Menu Page */

    MaintDisp(pData->uchACNumber,                   /* A/C Number */
              CLASS_MAINTDSPCTL_10NPG,              /* Set Minor Class for 10N10D */
              uchMaintMenuPage,                     /* Page Number */
              pData->uchPTDType,                    /* PTD Type */
              pData->uchRptType,                    /* Report Type */
              pData->uchRstType,                    /* Reset Type */
              pData->ulAmount,                      /* Amount Data */
              pData->uchLeadAddr);                  /* Lead Through Address */

    return(SUCCESS);

}
/*
*======================================================================================
*
**   Synopsis:  UCHAR MaintGetPage( VOID )
*
*       Input:  Nothing  
*      Output:  Nothing
*
**  Return:     Menu Page 
*               
**  Description: Get Menu Shift Page  
*======================================================================================
*/

UCHAR MaintGetPage( VOID )
{

    return( uchMaintMenuPage );

}

/*
*======================================================================================
*
**   Synopsis:  SHORT MaintSetPageOnly( MAINTSETPAGE *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for MAINTSETPAGE  
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_KEYOVER_ADR : Wrong Data
*               
**  Description: Check and Set Menu Shift Page.  
*======================================================================================
*/

SHORT MaintSetPageOnly( MAINTSETPAGE *pData )
{


    /* Check W/o Amount Case */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* W/o Amount Case */
        uchMaintMenuPage++;
        if (uchMaintMenuPage > MAX_PAGE_NO) {               
            uchMaintMenuPage = 1;                           /* Initialize Menu Page */
        }
    } else {                                                /* W/ Amount Case */
        if (pData->uchPageNumber == 0 || pData->uchPageNumber > MAX_PAGE_NO) {  /* Over Digit */
            return(LDT_KEYOVER_ADR);                                            
        }

        /* Save Menu Page Data */
    
        uchMaintMenuPage = pData->uchPageNumber;

    }

    return(SUCCESS);

}
