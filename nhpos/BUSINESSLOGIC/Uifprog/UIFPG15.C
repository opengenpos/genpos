/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Preset Amount Cash Tender Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application         
* Program Name: UIFPG15.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                                                             
*               UifPG15Function()       : Program No.15 Function Entry Mode
*               UifPG15EnterAddr()      : Program No.15 Enter Address Mode
*               UifPG15EnterData()      : Program No.15 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-02-93: 01.00.12   : K.You     : initial                                   
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
#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifProg.h>
#include <uifpgequ.h>
                                            
#include "uifpgex.h"                        /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifPG15Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: ProgramNo.15 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG15Init[] = {{UifPG15IssueRpt, CID_PG15ISSUERPT},
                                    {UifPG15EnterAddr, CID_PG15ENTERADDR},
                                    {UifPG15EnterData, CID_PG15ENTERDATA},
                                    {NULL,               0              }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG15Accept[] = {{UifPG15EnterAddr, CID_PG15ENTERADDR},
                                      {UifPG15EnterData, CID_PG15ENTERDATA},
                                      {NULL,               0              }};


SHORT UifPG15Function(KEYMSG *pKeyMsg) 
{

                                            
    PARAPRESETAMOUNT    ParaPresetAmount;   
                                              

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:                                                  
        UieNextMenu(aChildPG15Init);                                /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaPresetAmount.uchStatus = 0;                             /* Set W/ Amount Status */
        ParaPresetAmount.uchAddress = 1;                            /* Set Address 1 */
        ParaPresetAmount.uchMajorClass = CLASS_PARAPRESETAMOUNT;    /* Set Major Class */

        MaintPresetAmountRead(&ParaPresetAmount);                             
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG15Accept);                              /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG15IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.15 Issue Report 
*===============================================================================
*/
    


SHORT UifPG15IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {            /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintPresetAmountReport();              /* Execute Report Procedure */
            UieExit(aPGEnter);                      /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG15EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: ProgramNo.15 Enter Address Mode 
*===============================================================================
*/

SHORT UifPG15EnterAddr(KEYMSG *pKeyMsg) 
{
                        
    SHORT               sError;
    PARAPRESETAMOUNT    ParaPresetAmount;
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:                                              
        UieOpenSequence(aszSeqPGFSC_P2);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */
                                                                                    
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
                                                                        
            ParaPresetAmount.uchStatus = 0;                             /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaPresetAmount.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaPresetAmount.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaPresetAmount.uchMajorClass = CLASS_PARAPRESETAMOUNT;           /* Set Major Class */
            if ((sError = MaintPresetAmountRead(&ParaPresetAmount)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifPG15Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG15EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: ProgramNo.15 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG15EnterData(KEYMSG *pKeyMsg) 
{
                    
    SHORT               sError;
    PARAPRESETAMOUNT    ParaPresetAmount;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);                                                        

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaPresetAmount.uchStatus = 0;                                 /* Set W/ Amount Status */
            ParaPresetAmount.ulAmount = ( ULONG)pKeyMsg->SEL.INPUT.lData;
            if ((sError = MaintPresetAmountWrite(&ParaPresetAmount)) == SUCCESS) {    /* Read Function Completed */
                UieAccept();                                                /* Return to UifPG15Function() */
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/****** End of Source ******/
