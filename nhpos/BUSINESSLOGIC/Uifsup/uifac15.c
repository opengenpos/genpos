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
* Title       : Supervisor Secret Number Change Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC15.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC15Function()   : AC No.15 Function Entry Mode 
*               UifAC15EnterData()  : AC No.15 Enter Data Mode   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-08-92: K.You     : initial                                   
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
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC15EnterKey[] = {FSC_AC, FSC_CANCEL, 0};



/*
*=============================================================================
**  Synopsis:    SHORT UifAC15Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.15 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC15EnterData[] = {{UifAC15EnterData, CID_AC15ENTERDATA},
                                         {NULL,             0                }};

    
SHORT UifAC15Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC15EnterData);           /* Open Next Function */

        MaintDisp(AC_SUPSEC_NO,                     /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount */
                  0);                               /* Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC15EnterData(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.15 Enter Data Mode    
*===============================================================================
*/
SHORT UifAC15EnterData(KEYMSG *pKeyMsg) 
{
    SHORT           sError = SUCCESS;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:                                                  
        UieOpenSequence(aszSeqAC15EnterKey);                        /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_NO, UIF_DIGIT9);                       /* Stop Echo Back and Set Input Digit */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:                                              

            /* Check W/o Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                /* W/o Amount Input Case */
                return(LDT_SEQERR_ADR);
            }

            /* Check Supervisor No. Unmatch and Set Secret Number */
            sError = MaintClearSetPin(( ULONG)(pKeyMsg->SEL.INPUT.lData), 0);  

			/* Make Header and Trailer */
			MaintHeaderCtl(AC_SUPSEC_NO, RPT_ACT_ADR);
			MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

            /* break */

        case FSC_CANCEL:
            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT10);         /* Start Echo Back and Set Max Input Digit */
            UieExit(aACEnter);                                      /* Return to UifACEnter() */
            return(sError);

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

    



    
    
    
