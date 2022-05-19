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
* Title       : Set Pig Rule Table.
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC130.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC130Function()      : A/C No.130 Function Entry Mode
*               UifAC130EnterAddr()     : A/C No.130 Enter Address Mode
*               UifAC130EnterData1()    : A/C No.130 Enter Data1 Mode
*               UifAC130EnterData2()    : A/C No.130 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
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
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifAC130Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.130 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC130Init[] = {{UifAC130EnterAddr, CID_AC130ENTERADDR},
                                     {UifAC130EnterData1, CID_AC130ENTERDATA1},
                                     {NULL,               0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC130Accept[] = {{UifAC130EnterAddr, CID_AC130ENTERADDR},
                                       {UifAC130EnterData2, CID_AC130ENTERDATA2},
                                       {NULL,               0                 }};


SHORT UifAC130Function(KEYMSG *pKeyMsg) 
{


    PARAPIGRULE      ParaPigRule;   
                                              

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC130Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaPigRule.uchStatus = 0;                                  /* Set W/ Amount Status */
        ParaPigRule.uchAddress = 1;                                 /* Set Address 1 */
        ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;              /* Set Major Class */
        ParaPigRule.uchMinorClass = CLASS_PARAPIGRULE_COLIMIT;      /* Set Minor Class */

        MaintPigRuleRead(&ParaPigRule);                             
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC130Accept);                             /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC130EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.130 Enter Address Mode 
*===============================================================================
*/

SHORT UifAC130EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    PARAPIGRULE         ParaPigRule;
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                  /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */
                                                                       
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaPigRule.uchStatus = 0;                                  /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaPigRule.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaPigRule.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;           /* Set Major Class */
            if ((sError = MaintPigRuleRead(&ParaPigRule)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC130Function() */
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
**  Synopsis: SHORT UifAC130EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.130 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC130EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAPIGRULE  ParaPigRule;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                        /* Register Key Sequence */
        return(SUCCESS);                                                        

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Case */
                MaintPigRuleReport();                                           /* Execute Report Procedure */
                UieExit(aACEnter);                                              /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                            /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {                   /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;               Set Major Class */
                ParaPigRule.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaPigRule.ulPigRule = ( ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintPigRuleWrite(&ParaPigRule)) == SUCCESS) {    /* Read Function Completed */
                    UieAccept();                                                /* Return to UifAC130Function() */
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
**  Synopsis: SHORT UifAC130EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.130 Enter Data2 Mode 
*===============================================================================
*/
    

SHORT UifAC130EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    PARAPIGRULE      ParaPigRule;
                        


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {                       /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaPigRule.uchMajorClass = CLASS_PARAPIGRULE;                   Set Major Class */
                ParaPigRule.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaPigRule.ulPigRule = ( ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintPigRuleWrite(&ParaPigRule)) == SUCCESS) {    
                    UieAccept();                                                /* Return to UifAC130Function() */
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

