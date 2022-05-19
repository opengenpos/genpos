/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 2000            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : BroadCast/Request to DownLoad PLU Parameter Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC66.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC66Function()   : A/C No.66 Function Entry Mode 
*               UifAC66EnterKey()   : A/C No.66 Enter AC Key Mode   
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
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <csttl.h>
#include <ttl.h>

#include "uifsupex.h"

VOID UifACSupCloseFile(VOID);

/*
*=============================================================================
**  Synopsis:    SHORT UifAC66Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.66 Function Entry Mode   
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC66EnterKey[] = {{UifAC66EnterKey, CID_AC66ENTERKEY},
                                        {NULL,            0               }};

SHORT UifAC66Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC66EnterKey);            /* Open Next Function */

        MaintDisp(AC_PLU_DOWN,                      /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_CLRABRT_ADR);                 /* AC or Abort */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC66EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.66 Enter AC Key Mode     
*===============================================================================
*/
SHORT   UifAC66EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT               sError=0;
    SYSCONFIG CONST FAR *pSysConfig;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        pSysConfig = PifSysConfig();                    /* get system config */
        if (pSysConfig->ausOption[UIE_TEST_OPTION8] != UIE_TEST_PASS) {         /* Debug Option */
            return(LDT_SEQERR_ADR);
        }

        /* W/o Amount Check */
        if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            MaintDisp(AC_PLU_DOWN,                          /* A/C Number */
                      0,                                    /* Descriptor */
                      0,                                    /* Page Number */
                      0,                                    /* PTD Type */
                      0,                                    /* Report Type */
                      0,                                    /* Reset Type */
                      0L,                                   /* Amount Data */
                      LDT_COMUNI_ADR);                      /* " During Comm. " Lead Through Address */

            UifACSupCloseFile();
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(sError);                                 /* Return Success or Error Status */

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}


/*
*==============================================================================
**  Synopsis: SHORT UifAC66EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.66 Enter AC Key Mode     
*===============================================================================
*/
VOID    UifACSupCloseFile(VOID) 
{
    USHORT  i;

    for (i=0; i<PIF_MAX_FILE; i++) {

        PifCloseFile(i);  /* close all handle of PIF */
    }
    TtlPLUCloseDB();    /* close plu total data base */

}
    
/* --- End of Source File --- */