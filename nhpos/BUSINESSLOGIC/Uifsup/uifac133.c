/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Service Time Parameter Module 
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC133.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC133Function()       : A/C No.133 Function Entry Mode
*               UifAC133EnterAddr()      : A/C No.133 Enter Address Mode
*               UifAC133EnterData1()     : A/C No.133 Enter Data1 Mode
*               UifAC133EnterData2()     : A/C No.133 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Nov-30-95: 03.01.00   :M.Ozawa    : initial            
* Aug-27-98: 03.03.00   :hrkato     : V3.3
* Aug-16-99: 03.05.00   : K.Iwata   : V3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
**  Synopsis: SHORT UifAC133Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.133 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC133Init[] = {{UifAC133EnterAddr, CID_AC133ENTERADDR},
                                    {UifAC133EnterData1, CID_AC133ENTERDATA1},
                                    {NULL,              0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC133Accept[] = {{UifAC133EnterAddr, CID_AC133ENTERADDR},
                                      {UifAC133EnterData2, CID_AC133ENTERDATA2},
                                      {NULL,              0                 }};


SHORT UifAC133Function(KEYMSG *pKeyMsg) 
{


    PARASERVICETIME     ParaSerTime;   
                    

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC133Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaSerTime.uchStatus = 0;                                  /* Set W/ Amount Status */
        ParaSerTime.uchAddress = 1;                                 /* Set Address 1 */
        ParaSerTime.uchMajorClass = CLASS_PARASERVICETIME;          /* Set Major Class */

        MaintServiceTimeRead(&ParaSerTime);                         /* Execute Read HALO Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC133Accept);                               /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC133EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.133 Enter Address Mode 
*===============================================================================
*/

SHORT UifAC133EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASERVICETIME ParaSerTime;
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                          /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaSerTime.uchStatus = 0;                                  /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaSerTime.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaSerTime.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaSerTime.uchMajorClass = CLASS_PARASERVICETIME;          /* Set Major Class */
            if ((sError = MaintServiceTimeRead(&ParaSerTime)) == SUCCESS) {    
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
**  Synopsis: SHORT UifAC133EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.133 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC133EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASERVICETIME ParaSerTime;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintServiceTimeReport();                                   /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.lData > 65535L) {                    /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                ParaSerTime.uchMajorClass = CLASS_PARASERVICETIME;          /* Set Major Class */
                ParaSerTime.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaSerTime.usBorder = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintServiceTimeWrite(&ParaSerTime)) == SUCCESS) {   /* Read Function Completed */
                    UieAccept();                                            /* Return to UifAC133Function() */
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
**  Synopsis: SHORT UifAC133EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.133 Enter Data2 Mode 
*===============================================================================
*/
    

SHORT UifAC133EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARASERVICETIME ParaSerTime;
                   


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

                if (pKeyMsg->SEL.INPUT.lData > 65535L) {                      /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                ParaSerTime.uchMajorClass = CLASS_PARASERVICETIME;           /* Set Major Class */
                ParaSerTime.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaSerTime.usBorder = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintServiceTimeWrite(&ParaSerTime)) == SUCCESS) {          
                    UieAccept();                                            /* Return to UifAC133Function() */
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


/* --- End of Source --- */
