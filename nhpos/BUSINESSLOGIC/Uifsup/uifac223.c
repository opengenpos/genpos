/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Individual Fiancial File Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC223.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC223Function()       : A/C No.223 Function Entry Mode
*               UifAC223EnterData()      : A/C No.223 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Dec-05-95:03.01.00    : M.Ozawa   : initial                                   
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
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */



/*
*=============================================================================
**  Synopsis: SHORT UifAC223Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.223 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC223Init[] = {{UifAC223EnterData, CID_AC223ENTERDATA},
                                    {NULL,             0                }};



SHORT UifAC223Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC223Init);                            /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}



/*
*=============================================================================
**  Synopsis: SHORT UifAC223EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.223 Enter Data Mode 
*===============================================================================
*/
    



SHORT UifAC223EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT   sError;                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_IND_READ_RPT,                  /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
                                                                    
            MaintDisp(AC_IND_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      0,                            /* PTD Type */
                      0,                            /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      0);                           /* Disable Lead Through */

            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                sError = RptIndFinDayRead(CLASS_TTLCURDAY,
                                        RPT_ALL_READ,
                                        0,                          
                                        0);                         /* Execute All Read Report */
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                        /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }

            /* Check 0 Data and Digit */

            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {      

                MaintDisp(AC_IND_READ_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          0,                                /* PTD Type */
                          0,                                /* Report Type */
                          0,                                /* Reset Type */
                          0L,                               /* Amount Data */
                          LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

                return(LDT_KEYOVER_ADR);
            }

            /* Execute Individual Read */

            sError = RptIndFinDayRead(CLASS_TTLCURDAY,
                                    RPT_IND_READ,
                                    ( USHORT)pKeyMsg->SEL.INPUT.lData,
                                    0);
            UieExit(aACEnter);                                                       
            if (sError == RPT_ABORTED) {                        /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }                                                                   
}
