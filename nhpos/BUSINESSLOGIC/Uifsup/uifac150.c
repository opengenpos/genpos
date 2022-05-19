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
* Title       : Employee Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC150.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC150Function()       : A/C No.21 Function Entry Mode
*               UifAC150EnterData()      : A/C No.21 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-08-92:00.00.01    : M.Yamamoto: initial
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
#include <uireg.h>
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */



/*
*=============================================================================
**  Synopsis: SHORT UifAC150Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.150 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC150Init[] = {{UifAC150EnterData, CID_AC150ENTERDATA},
                                    {NULL,             0                }};



SHORT UifAC150Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
    case UIM_ACCEPTED:
        UieNextMenu(aChildAC150Init);                            /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC150EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.150 Enter Data Mode 
*===============================================================================
*/
UIMENU FARCONST aChildAC150DataFunc[] = {{UifAC150EnterData1, CID_AC150ENTERDATA1},
                                        {NULL,             0                    }};

SHORT UifAC150EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(TypeSelectReport6);

        MaintDisp(AC_ETK_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOTYPE_ADR);                /* "Report Type" Lead Through Address */

        return(SUCCESS);
        break;
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            UifACPGMLDClear();

            MaintDisp(AC_ETK_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      0,                            /* PTD Type */
                      0,                            /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                UieAccept();                                                /* Return to UifAC68Function() */                   
                return(LDT_SEQERR_ADR);
            }

            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS)) {
                UieAccept();                                         /* Return to UifAC68Function() */
                return(LDT_KEYOVER_ADR);
            }

            if (pKeyMsg->SEL.INPUT.lData == RPT_ALL_READ) {
                sError = RptETKRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0);    /* Execute All Read Report */
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                        /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);

            } else if (pKeyMsg->SEL.INPUT.lData == RPT_IND_READ) {
                UieNextMenu(aChildAC150DataFunc);                /* Open Next Function */
                return (SUCCESS);
            } else {
                UieAccept();                                         /* Return to UifAC68Function() */
                return(LDT_KEYOVER_ADR);
            }
        case FSC_CANCEL:
        default:
                return(UifACDefProc(pKeyMsg));                   /* Execute Default Procedure */
                break;
        }
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC150DataFunc);                /* Open Next Function */
        return(SUCCESS);
        break;

    default:
        return(UifACDefProc(pKeyMsg));                   /* Execute Default Procedure */
    }                                                                   
}
/*
*=============================================================================
**  Synopsis: SHORT UifAC150EnterData1(KEYMSG *pKeyMsg) 
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
SHORT UifAC150EnterData1(KEYMSG *pKeyMsg)
{
    SHORT   sError;
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_ETK_READ_RPT,                  /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
        break;

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            /* Execute Individual Read */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT9)) {
                return(LDT_KEYOVER_ADR);
            }
            sError = RptETKRead(CLASS_TTLCURDAY, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, 0);
            if ((sError == RPT_ABORTED) || (sError == RPT_END)
                || (sError == SUCCESS)) { /* Aborted by User */
                UieExit(aACEnter);                             
                sError = SUCCESS;
            }
            return (sError);
        default:
            return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
            break;
        }
    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}
