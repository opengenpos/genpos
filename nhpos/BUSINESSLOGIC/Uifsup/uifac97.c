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
* Title       : PTD Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC97.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC97Function()       : A/C No.97 Function Entry Mode
*               UifAC97EnterRstType()   : A/C No.97 Enter Reset Type Mode
*               UifAC97EnterKey()       : A/C No.97 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. : Name       : Description
* Nov-11-93 : 00.00.01 : M.Yamamoto : initial                                   
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <eept.h>
#include <pif.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <cpm.h>
#include <csstbcpm.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC97Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.97 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC97Init[] = {{UifAC97EnterRstType, CID_AC97ENTERRSTTYPE},
                                    {NULL,                0                   }};



SHORT UifAC97Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC97Init);                             /* Open Next Function */
        return(SUCCESS);
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
        break;
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC97EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.97 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC97EnterRstType[] = {{UifAC97EnterKey, CID_AC97ENTERKEY},
                                            {NULL,            0               }};




SHORT UifAC97EnterRstType(KEYMSG *pKeyMsg) 
{
    
    SHORT sReturn = SUCCESS;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDDispCom(TypeSelectReport8);       /* display type on LCD */

        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        MaintDisp(AC_TALLY_CPM_EPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                /* "Request Function Type Entry" Lead Through Address */

        return(SUCCESS);
        break;

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {      
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
    
            /* Check Input Data */

            if ((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_CPM_PRT) ||
               (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_EPT_PRT)) {       /* Only Print Type */

                /* Clear Lead Through */
                MaintDisp(AC_TALLY_CPM_EPT,                             /* A/C Number */
                          0,                                            /* Descriptor */
                          0,                                            /* Page Number */
                          0,                                            /* PTD Type */
                          0,                                            /* Report Type */
                          ( UCHAR)pKeyMsg->SEL.INPUT.lData,             /* Reset Type */
                          0L,                                           /* Amount Data */
                          0);                                           /* Disable Lead Through */

                sReturn = RptTallyReadOrReset(( UCHAR)pKeyMsg->SEL.INPUT.lData);
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                return(sReturn);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_CPM_RST ||
                       pKeyMsg->SEL.INPUT.lData == RPT_TYPE_EPT_RST) {
                uchUifACRstType = ( UCHAR )pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC97EnterRstType);                /* Open Next Function */
                return(SUCCESS);
            } else {
                return(LDT_KEYOVER_ADR);
            }
            break;
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC97EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.97 Reset Confirm Mode 
*===============================================================================
*/

SHORT UifAC97EnterKey(KEYMSG *pKeyMsg) 
{

    SHORT sReturn = SUCCESS;
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);         /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_TALLY_CPM_EPT,                 /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_RESET_ADR);                   /* "Lead Through for Reset Report"Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        /* Check W/ Amount */

        if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }
        
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:

            /* Clear Lead Through */

            UifACPGMLDClear();

            MaintDisp(AC_TALLY_CPM_EPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      uchUifACRstType,                  /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            sReturn = RptTallyReadOrReset(uchUifACRstType);
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(sReturn);
            break;

        case FSC_CANCEL:
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(SUCCESS);
            break;

        /* default:
            break; */
        }
        /* break; */
        /*return(UifACDefProc(pKeyMsg));*/                                  /* Execute Default Procedure */
    }
    return(UifACDefProc(pKeyMsg)); /* ### Move (2171 for Win32) v1.0 */
}
                                                         
/****** End of Source(UifAC97.c) ******/
