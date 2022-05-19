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
* Title       : Emergent Guest Check File Close Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC103.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC103Function()      : A/C No.103 Function Entry Mode 
*               UifAC103EnterData()     : A/C No.103 Enter Data Mode  
*               UifAC103ExitFunction()  : A/C No.103 Exit Function Mode  
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
* Nov-26-92:01.00.00    :K.You      : Display GC No.                                    
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

#include "uifsupex.h"

/*
*=============================================================================
**  Synopsis:    SHORT UifAC103Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.103 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC103Init[] = {{UifAC103EnterData,CID_AC103ENTERDATA},
                                     {NULL,             0                 }};

    
SHORT UifAC103Function(KEYMSG *pKeyMsg) 
{
    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC103Init);               /* Open Next Function */

        MaintDisp(AC_EMG_GCFL_CLS,                  /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* " Number Entry " Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC103EnterData(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.103 Enter Data Mode  
*===============================================================================
*/


/* Define Next Function */

UIMENU FARCONST aChildAC103EnterData[] = {{UifAC103ExitFunction, CID_AC103EXITFUNCTION},
                                          {NULL,                 0                    }};

static USHORT       usUifGCNumber = 0;                  /* GC No. Save Area */

SHORT UifAC103EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
    USHORT  usGCNumber;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            /* W/ Amount Check */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Digit Check */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {       /* Over Digit, R3.3 */
                return(LDT_KEYOVER_ADR);
            }

            /* Check GC Number and Execute */
            if ((sError = MaintChkGCNo(( ULONG)pKeyMsg->SEL.INPUT.lData, &usGCNumber)) == SUCCESS) {
                usUifGCNumber = (USHORT)pKeyMsg->SEL.INPUT.lData;           /* Save GC Number to Own Save Area */
                usUifACRptGcfNo = usGCNumber;                               /* Save GC Number to Own Save Area */
                UieNextMenu(aChildAC103EnterData);
                return(SUCCESS);
            }
            /* UieExit(aACEnter);                                               Return to UifACEnter() */
            return(sError);

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC103ExitFunction(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.103 Exit Function Mode  
*===============================================================================
*/
SHORT UifAC103ExitFunction(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        
        MaintDisp(AC_EMG_GCFL_CLS,                  /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  usUifGCNumber,                    /* GC Number */
                  LDT_RESET_ADR);                   /* " Lead Through for Reset Report " Lead Through Address */
        
        return(SUCCESS);

    case UIM_INPUT:
        /* W/ Amount Check */
        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            sError = RptGuestResetByGuestCheck (RPT_OPEN_RESET, usUifACRptGcfNo);   /* Execute Close File Function */
            UieExit(aACEnter);                                                
            return(sError);    

        case FSC_CANCEL:
            UieExit(aACEnter);                                                
            return(SUCCESS);    

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}



    
    
    
