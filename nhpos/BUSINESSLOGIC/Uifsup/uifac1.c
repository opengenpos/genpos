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
* Title       : Guest Check No. Assignment Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC1.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC1Function()    : AC No.1 Function Entry Mode 
*               UifAC1EnterData()   : AC No.1 Enter Data Mode   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-16-92: K.You     : initial                                   
*          :           :                                    
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

#include "uifsupex.h"

/*
*=============================================================================
**  Synopsis:    SHORT UifAC1Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.1 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC1EnterData[] = {{UifAC1EnterData,CID_AC1ENTERDATA},
                                        {NULL,           0               }};

    
SHORT UifAC1Function(KEYMSG *pKeyMsg) 
{
    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC1EnterData);            /* Open Next Function */

        MaintDisp(AC_GCNO_ASSIN,                    /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC1EnterData(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.1 Enter Data Mode    
*===============================================================================
*/

    

SHORT UifAC1EnterData(KEYMSG *pKeyMsg) 
{
    

    SHORT           sError;
    PARAGUESTNO     ParaGCNo;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:                                              
            /* Digit Check */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {  
                return(LDT_KEYOVER_ADR);
            }

            ParaGCNo.uchStatus = 0;                         /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                ParaGCNo.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                ParaGCNo.usGuestNumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Write Guest Check No. Procedure */
            
            ParaGCNo.uchMajorClass = CLASS_PARAGUESTNO;     /* Set Major Class */

            sError = MaintGuestNoWrite(&ParaGCNo);
            if ((sError != LDT_SEQERR_ADR) && (sError != LDT_KEYOVER_ADR)) {
                UieExit(aACEnter);                          /* Return to UifACEnter() */
            }
            return(sError);                                 /* Set Success or Input Data Error */

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}




    
    
    
