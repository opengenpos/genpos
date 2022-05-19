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
* Program Name: UIFAC151.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC151Function()       : A/C No.18 Function Entry Mode
*               UifAC151EnterRstType()   : A/C No.18 Enter Reset Type Mode
*               UifAC151EnterKey()       : A/C No.18 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-26-93:00.00.01    : M.Yamamoto: initial
*          :            :           :                         
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
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC151Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.151 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC151Init[] = {{UifAC151EnterRstType, CID_AC151ENTERRSTTYPE},
                                    {NULL,                0                   }};



SHORT UifAC151Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC151Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC151EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.151 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC151EnterRstType[] = {{UifAC151EnterKey, CID_AC151ENTERKEY},
                                            {NULL,            0               }};




SHORT UifAC151EnterRstType(KEYMSG *pKeyMsg) 
{
    
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_ETK_RESET_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                /* "Request Function Type Entry" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {      
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
    
            /* Check Input Data */

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT) {        /* Only Print Type */

                /* Clear Lead Through */

                UifACPGMLDClear();

                MaintDisp(AC_ETK_RESET_RPT,                             /* A/C Number */
                          0,                                            /* Descriptor */
                          0,                                            /* Page Number */
                          0,                                            /* PTD Type */
                          uchUifACRptType,                                            /* Report Type */
                          uchUifACRstType,                              /* Reset Type */
                          0L,                                           /* Amount Data */
                          0);                                           /* Disable Lead Through */

                sError = RptETKRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0,
                           (UCHAR)pKeyMsg->SEL.INPUT.lData);
                    UieExit(aACEnter);                                      /* Return to UifACEnter() */
                    if (sError == RPT_ABORTED) {                        /* Aborted by User */
                        sError = SUCCESS;
                    }
                    return(sError);
            } else if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) || /* Reset and Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {      /* Only Reset Type */
                if ((sError = RptETKLock()) == SUCCESS) {
                    uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData; /* Save Reset Type */
                    UieNextMenu(aChildAC151EnterRstType);                /* Open Next Function */
                } else {
                    UieExit(aACEnter);                                  /* Return to UifACEnter() */
                }
                return(sError);
            } else {
                return(LDT_KEYOVER_ADR);
            }

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
**  Synopsis: SHORT UifAC151EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.151 Reset Confirm Mode 
*===============================================================================
*/

SHORT UifAC151EnterKey(KEYMSG *pKeyMsg) 
{
    
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);         /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_ETK_RESET_RPT,                 /* A/C Number */
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

            MaintDisp(AC_ETK_RESET_RPT,                       /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      uchUifACRstType,                  /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            sError = RptETKReset(RPT_ALL_RESET, uchUifACRstType);
            
            RptETKUnLock();
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            if (sError == RPT_END) {                                 /* All Reset Incompleted */
                sError = SUCCESS;
            }
            return(sError);

        case FSC_CANCEL:
            if (uchUifACRstType != ( UCHAR)RPT_TYPE_ONLY_PRT) {             /* Reset Case */
                RptETKUnLock();
            }
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
/*====== End of Source ======*/


