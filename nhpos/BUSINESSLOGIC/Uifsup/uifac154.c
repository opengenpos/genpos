/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ETK Labor Cost Value Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC154.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC154Function()       : A/C No.154 Function Entry Mode
*               UifAC154EnterAddr()      : A/C No.154 Enter Address Mode
*               UifAC154EnterData1()     : A/C No.154 Enter Data1 Mode
*               UifAC154EnterData2()     : A/C No.154 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Dec-20-95:03.01.00    :M.Ozawa    : initial          
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC154Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.154 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC154Init[] = {{UifAC154EnterAddr, CID_AC154ENTERADDR},
                                    {UifAC154EnterData1, CID_AC154ENTERDATA1},
                                    {NULL,              0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC154Accept[] = {{UifAC154EnterAddr, CID_AC154ENTERADDR},
                                      {UifAC154EnterData2, CID_AC154ENTERDATA2},
                                      {NULL,              0                 }};


SHORT UifAC154Function(KEYMSG *pKeyMsg) 
{
	PARALABORCOST     ParaLaborCost = {0};   
                    
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC154Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */
        ParaLaborCost.uchMajorClass = CLASS_PARALABORCOST;            /* Set Major Class */
        ParaLaborCost.uchAddress = LABOR_COST1_ADR;                   /* Set Address 1 */
        ParaLaborCost.uchStatus = 0;                                  /* Set W/ Amount Status */

        MaintLaborCostRead(&ParaLaborCost);                           /* Execute Read Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC154Accept);                               /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC154EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.154 Enter Address Mode to allow for the entry of a job code
*                from 1 to the max number.
*===============================================================================
*/
SHORT UifAC154EnterAddr(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
	PARALABORCOST   ParaLaborCost = {0};
                    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                          /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.lData > MAX_LABORCOST_SIZE) {
                return(LDT_KEYOVER_ADR);
            }

            ParaLaborCost.uchMajorClass = CLASS_PARALABORCOST;            /* Set Major Class */
            ParaLaborCost.uchStatus = 0;                                  /* Set W/ Amount Status */
            if (pKeyMsg->SEL.INPUT.uchLen == 0) {                         /* W/o Amount Input Case */
                ParaLaborCost.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                      /* W/ Amount Input Case */
                ParaLaborCost.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            if ((sError = MaintLaborCostRead(&ParaLaborCost)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC86Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC154EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.154 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC154EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
	PARALABORCOST   ParaLaborCost = {0};
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen == 0) {                       /* W/o Amount Case */
                MaintLaborCostReport();                                     /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                    /* W/ Amount Case */
                if (pKeyMsg->SEL.INPUT.lData > 65535L) {                    /* Check number Digits, Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                ParaLaborCost.uchMajorClass = CLASS_PARALABORCOST;          /* Set Major Class */
                ParaLaborCost.uchStatus = 0;                                /* Set W/ Amount Status */
                ParaLaborCost.usLaborCost = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintLaborCostWrite(&ParaLaborCost)) == SUCCESS) {   /* Read Function Completed */
                    UieAccept();                                            /* Return to UifAC154Function() */
                }
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC154EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.154 Enter Data2 Mode 
*===============================================================================
*/
SHORT UifAC154EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
	PARALABORCOST   ParaLaborCost = {0};
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen == 0) {                       /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                    /* W/ Amount Case */
                if (pKeyMsg->SEL.INPUT.lData > 65535L) {                    /* Check number Digits, Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                ParaLaborCost.uchMajorClass = CLASS_PARALABORCOST;          /* Set Major Class */
                ParaLaborCost.uchStatus = 0;                                /* Set W/ Amount Status */
                ParaLaborCost.usLaborCost = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintLaborCostWrite(&ParaLaborCost)) == SUCCESS) {          
                    UieAccept();                                            /* Return to UifAC86Function() */
                }
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */

    }
}
