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
* Title       : Flexible Memory Assignment Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application        
* Program Name: UIFPG2.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG2Function()    : Program No.2 Entry Mode
*               UifPG2IssueRpt()    : Program No.2 Issue Report 
*               UifPG2EnterAddr()   : Program No.2 Enter Address Mode 
*               UifPG2EnterData1()  : Program No.2 Enter Data Mode 1
*               UifPG2EnterData2()  : Program No.2 Enter Data Mode 2
*                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-19-92:00.00.01    :K.You      : initial
* Feb-28-95:03.00.00    :H.Ishida   : Chg. Digint of Program Address
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
**  Synopsis: SHORT UifPG2Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               
**  Description: Program No.2 Entry Mode 
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG2Init1[] = {{UifPG2IssueRpt, CID_PG2ISSUERPT},
                                    {UifPG2EnterAddr, CID_PG2ENTERADDR},
                                    {UifPG2EnterData1, CID_PG2ENTERDATA1},
                                    {NULL,             0             }};

/* Define Next Function at UIM_INIT from CID_PG2ENTERDATA1 */

UIMENU FARCONST aChildPG2Init2[] = {{UifPG2EnterData2, CID_PG2ENTERDATA2},
                                    {NULL,             0             }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG2Accept[] = {{UifPG2EnterAddr, CID_PG2ENTERADDR},
                                     {UifPG2EnterData1, CID_PG2ENTERDATA1},
                                     {NULL,             0             }};


SHORT UifPG2Function(KEYMSG *pKeyMsg) 
{
    
    PARAFLEXMEM     FlexMemData;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PG2ENTERDATA1:                                 /* UieExit() from UifPG2EnterData1() */
            UieNextMenu(aChildPG2Init2);                        /* Open Next Function */
            break;

        default:                                                /* Initialize */
            UieNextMenu(aChildPG2Init1);                        /* Open Next Function */

            /* Display Address 1 of This Function */

            FlexMemData.uchStatus = 0;                          /* Set W/ Amount Status */
            FlexMemData.uchAddress = FLEX_DEPT_ADR;             /* Set Address 1 */
            FlexMemData.uchMajorClass = CLASS_PARAFLEXMEM;      /* Set Major Class */

            MaintFlexMemRead(&FlexMemData);                     /* Execute Read Flexible Memory Procedure */
            break;
        }
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG2Accept);                           /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                          /* execute Default Procedure */
        break;
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG2IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.2 Issue Report 
*===============================================================================
*/
    

SHORT UifPG2IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintFlexMemReport();               /* Execute Report Procedure */
            UieExit(aPGEnter);                  /* Return to UifPGEnter() */
            return(SUCCESS);

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG2EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               
**  Description:  
*===============================================================================
*/
    

SHORT UifPG2EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAFLEXMEM     FlexMemData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            FlexMemData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                FlexMemData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                FlexMemData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Flexible Memory Procedure */
            
            FlexMemData.uchMajorClass = CLASS_PARAFLEXMEM;      /* Set Major Class */

            if ((sError = MaintFlexMemRead(&FlexMemData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG2Function() */
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
**  Synopsis: SHORT UifPG2EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               LDT_FLFUL_ADR   : File Full
*
**  Description: Program No.2 Enter Data Mode after UIM_INIT  
*===============================================================================
*/
    

/* Define Parent Function */

UIMENU FARCONST aParentPG2Func[] = {{UifPG2Function, CID_PG2FUNCTION},
                                    {NULL,           0              }};


SHORT UifPG2EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAFLEXMEM     FlexMemData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT6) {       /* Over Digit, Saratoga */
                return(LDT_SEQERR_ADR);
            }
            FlexMemData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                FlexMemData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                FlexMemData.ulRecordNumber = (ULONG)pKeyMsg->SEL.INPUT.lData; /* 2172 */
                /* FlexMemData.usRecordNumber= ( USHORT)pKeyMsg->SEL.INPUT.lData; */
            }

            /* Edit Flexible Memory Record Procedure */
            
            FlexMemData.uchMajorClass = CLASS_PARAFLEXMEM;                  /* Set Major Class */
            FlexMemData.uchMinorClass = CLASS_PARAFLEXMEM_RECORD;           /* Set Major Class */

            if ((sError = MaintFlexMemEdit(&FlexMemData)) == SUCCESS) {     /* PTD Flag not Exist Case */
                if ((sError = MaintFlexMemWrite()) == SUCCESS) {            /* Create File Complete Case */
                    UieAccept();                                            /* Return to UifPG2Function() */
                    return(SUCCESS);
                } else {
                    return(sError);                                         
                }
            } else if (sError == MAINT_PTD_EXIST) {                         /* PTD Flag Exist Case */
                UieExit(aParentPG2Func);                                    /* Return to UifPG2Function() */
                return(SUCCESS);
            } else {
                return(sError);                                             /* Set Input Data Error */
            }
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG2EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               LDT_FLFUL_ADR   : File Full
*
**  Description: Program No.2 Enter Data Mode after UIM_ACCEPTED  
*===============================================================================
*/
    


SHORT UifPG2EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAFLEXMEM     FlexMemData;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            FlexMemData.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                FlexMemData.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                FlexMemData.uchPTDFlag = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Write PTD Flag Procedure */
            
            FlexMemData.uchMajorClass = CLASS_PARAFLEXMEM;                  /* Set Major Class */
            FlexMemData.uchMinorClass = CLASS_PARAFLEXMEM_PTD;              /* Set Major Class */

            if ((sError = MaintFlexMemEdit(&FlexMemData)) == SUCCESS) {     /* Invalid Data Case */     
                if ((sError = MaintFlexMemWrite()) == SUCCESS) {            /* Create File Complete Case */
                    UieAccept();                            /* Return to UifPG2Function() */
                    return(SUCCESS);
                } else {
                    UieAccept();                            /* Return to UifPG2Function() */
                    return(sError);                  
                }
            } else {
                return(sError);                             /* Set Input Data Error */
            }
            
        /* default:
              break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}

/* --- End of Source File --- */