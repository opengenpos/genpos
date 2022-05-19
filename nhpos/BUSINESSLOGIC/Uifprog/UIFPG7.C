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
* Title       : Store/Register No. Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG7.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG7Function()   : Program No.7 Entry Mode
*               UifPG7IssueRpt()   : Program No.7 Issue Report 
*               UifPG7EnterAddr()  : Program No.7 Enter Address Mode 
*               UifPG7EnterData()  : Program No.7 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-19-92:00.00.01    :K.You      : initial                                   
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
#include <uifprog.h>
#include <uifpgequ.h>

#include "uifpgex.h"                        /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifPG7Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.7 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG7Init[] = {{UifPG7IssueRpt, CID_PG7ISSUERPT},
                                   {UifPG7EnterAddr, CID_PG7ENTERADDR},
                                   {UifPG7EnterData, CID_PG7ENTERDATA},
                                   {NULL,            0               }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG7Accept[] = {{UifPG7EnterAddr, CID_PG7ENTERADDR},
                                     {UifPG7EnterData, CID_PG7ENTERDATA},
                                     {NULL,            0               }};


SHORT UifPG7Function(KEYMSG *pKeyMsg) 
{
    
    PARASTOREGNO   StoRegData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG7Init);                                /* Open Next Function */

        /* Display Address 1 of This Function */

        StoRegData.uchStatus = 0;                                  /* Set W/ Amount Status */
        StoRegData.uchAddress = 1;                                 /* Set Address 1 */
        StoRegData.uchMajorClass = CLASS_PARASTOREGNO;             /* Set Major Class */

        MaintStoRegNoRead(&StoRegData);                            /* Execute Read Store/Reg No. Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG7Accept);                              /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                             /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG7IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.7 Issue Report  
*===============================================================================
*/
    


SHORT UifPG7IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintStoRegNoReport();                  /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG7EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_KEYOVER_ADR : wrong Data Error
*               
**  Description: Program No.7 Enter Address Mode  
*===============================================================================
*/
    


SHORT UifPG7EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASTOREGNO    StoRegData;   
                           


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
            StoRegData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                StoRegData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                StoRegData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Store/Reg No. Procedure */
            
            StoRegData.uchMajorClass = CLASS_PARASTOREGNO;      /* Set Major Class */

            if ((sError = MaintStoRegNoRead(&StoRegData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG7Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG7EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.7 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG7EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASTOREGNO    StoRegData;   
                        

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {                               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            StoRegData.uchStatus = 0;                                                   /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                                           /* W/o Amount Input Case */
                StoRegData.uchStatus |= MAINT_WITHOUT_DATA;                             /* Set W/o Amount Status */
            } else {                                                                    /* W/ Amount Input Case */
                StoRegData.usStoreNo = ( USHORT)(pKeyMsg->SEL.INPUT.lData/1000L);       /* Set Store No. */
                StoRegData.usRegisterNo = ( USHORT)(pKeyMsg->SEL.INPUT.lData%1000L);    /* Set Register No. */
            }

            /* Execute Write Store/Reg No. Procedure */
            
            /* StoRegData.uchMajorClass = CLASS_PARASTOREGNO;       Set Major Class */

            if ((sError = MaintStoRegNoWrite(&StoRegData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG7Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}


    
    
    
