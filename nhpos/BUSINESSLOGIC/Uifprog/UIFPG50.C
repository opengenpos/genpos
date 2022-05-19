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
* Title       : Assignment Terminal No. Installing Shared Printer Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG50.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG50Function()   : Program No.50 Entry Mode
*               UifPG50IssueRpt()   : Program No.50 Issue Report 
*               UifPG50EnterAddr()  : Program No.50 Enter Address Mode 
*               UifPG50EnterData()  : Program No.50 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-23-93:01.00.12    :J.IKEDA    : initial                        
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
#include <uifprog.h>
#include <uifpgequ.h>

#include "uifpgex.h"                        /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifPG50Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.50 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG50Init[] = {{UifPG50IssueRpt, CID_PG50ISSUERPT},
                                    {UifPG50EnterAddr, CID_PG50ENTERADDR},
                                    {UifPG50EnterData, CID_PG50ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG50Accept[] = {{UifPG50EnterAddr, CID_PG50ENTERADDR},
                                      {UifPG50EnterData, CID_PG50ENTERDATA},
                                      {NULL,             0                }};


SHORT UifPG50Function(KEYMSG *pKeyMsg) 
{
    
    PARASHAREDPRT  SharedPrt;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG50Init);                                    /* Open Next Function */

        /* Display Address 1 of This Function */

        SharedPrt.uchStatus = 0;                                        /* Set W/ Amount Status */
        SharedPrt.uchAddress = 1;                                      /* Set Address 1 */
        SharedPrt.uchMajorClass = CLASS_PARASHRPRT;                     /* Set Major Class */

        MaintSharedPrtRead(&SharedPrt);                                 /* Execute Read Shred Printer Assignment Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG50Accept);                                  /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG50IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.50 Issue Report 
*===============================================================================
*/
    


SHORT UifPG50IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintSharedPrtReport();                     /* Execute Report Procedure */
            UieExit(aPGEnter);                          /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG50EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.50 Enter Address Mode 
*===============================================================================
*/

SHORT UifPG50EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASHAREDPRT   SharedPrt;                


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SharedPrt.uchStatus = 0;                            /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SharedPrt.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SharedPrt.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Shared Printer Assignment Procedure */
            
            SharedPrt.uchMajorClass = CLASS_PARASHRPRT;         /* Set Major Class */

            if ((sError = MaintSharedPrtRead(&SharedPrt)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG50Function() */
            }
            return(sError);                                         /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG50EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.50 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG50EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASHAREDPRT   SharedPrt;
                           
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SharedPrt.uchStatus = 0;                            /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SharedPrt.uchStatus |= MAINT_WITHOUT_DATA;      /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SharedPrt.uchTermNo = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write Shared Printer Assignment Procedure */
            
            /* SharedPrt.uchMajorClass = CLASS_PARASHRPRT; Set Major Class */
                                   
            if ((sError = MaintSharedPrtWrite(&SharedPrt)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG50Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}
