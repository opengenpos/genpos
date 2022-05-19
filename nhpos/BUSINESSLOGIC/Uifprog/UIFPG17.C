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
* Title       : Hourly Activity Time Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG17.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG17Function()   : Program No.17 Entry Mode
*               UifPG17IssueRpt()   : Program No.17 Issue Report 
*               UifPG17EnterAddr()  : Program No.17 Enter Address Mode 
*               UifPG17EnterData()  : Program No.17 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-23-92:00.00.01    :K.You      : initial                                   
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
**  Synopsis: SHORT UifPG17Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.17 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG17Init[] = {{UifPG17IssueRpt, CID_PG17ISSUERPT},
                                    {UifPG17EnterAddr, CID_PG17ENTERADDR},
                                    {UifPG17EnterData, CID_PG17ENTERDATA},
                                    {NULL,             0             }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG17Accept[] = {{UifPG17EnterAddr, CID_PG17ENTERADDR},
                                      {UifPG17EnterData, CID_PG17ENTERDATA},
                                      {NULL,             0                }};


SHORT UifPG17Function(KEYMSG *pKeyMsg) 
{
    
    MAINTHOURLYTIME     HourlyTimeData;
                        

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG17Init);                                    /* Open Next Function */

        /* Display Address 1 of This Function */

        HourlyTimeData.uchStatus = 0;                                   /* Set W/ Amount Status */
        HourlyTimeData.uchAddress = 1;                                  /* Set Address 1 */
        HourlyTimeData.uchMajorClass = CLASS_PARAHOURLYTIME;            /* Set Major Class */

        MaintHourlyTimeRead(&HourlyTimeData);                          
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG17Accept);                                  /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG17IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.17 Issue Report 
*===============================================================================
*/
    


SHORT UifPG17IssueRpt(KEYMSG *pKeyMsg) 
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
            MaintHourlyTimeReport();                /* Execute Report Procedure */
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
**  Synopsis: SHORT UifPG17EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.17 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG17EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    MAINTHOURLYTIME     HourlyTimeData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            HourlyTimeData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                HourlyTimeData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                HourlyTimeData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Hourly Activity Time Procedure */
            
            HourlyTimeData.uchMajorClass = CLASS_PARAHOURLYTIME;    /* Set Major Class */

            if ((sError = MaintHourlyTimeRead(&HourlyTimeData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG17Function() */
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
**  Synopsis: SHORT UifPG17EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.17 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG17EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    MAINTHOURLYTIME     HourlyTimeData;   
                       

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            HourlyTimeData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                HourlyTimeData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                HourlyTimeData.usHourTimeData = ( USHORT)(pKeyMsg->SEL.INPUT.lData);
            }

            /* Execute Write Hourly Activity Time Procedure */
            
            /* HourlyTimeData.uchMajorClass = CLASS_PARAHOURLYTIME;    Set Major Class */

            if ((sError = MaintHourlyTimeWrite(&HourlyTimeData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifPG17Function() */
            }
            return(sError);                                         /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}


    
    
    
