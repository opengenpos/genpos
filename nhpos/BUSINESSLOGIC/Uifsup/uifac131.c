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
* Title       : Service Time Report Module
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC131.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC131Function()       : AC131 Function Entry Mode 
*               UifAC131EnterDataType()  : AC131 Enter Data Type   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :          : Name      : Description
* Dec-05-95:          : M.Ozawa   : Initial                                   
* Aug-27-98:          : hrkato    : V3.3
* Aug-16-99: 03.05.00 : K.Iwata   : V3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
#include <mldmenu.h>

#include "uifsupex.h"



/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */


/*
*=============================================================================
**  Synopsis:    SHORT UifAC131Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description:  
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC131EnterDataType[] = {{UifAC131EnterDataType,CID_AC131ENTERDATATYPE},
                                              {NULL,                 0                    }};

    
SHORT UifAC131Function(KEYMSG *pKeyMsg) 
{
    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC131EnterDataType);               /* Open Next Function */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_SERVICE_READ_RPT,              /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,
                  LDT_SLCDORP_ADR);                 /* Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC131EnterDataType(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description:  
*===============================================================================
*/


SHORT UifAC131EnterDataType(KEYMSG *pKeyMsg) 
{
    


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);                       /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Digit Check */

        if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
            return(LDT_KEYOVER_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:                                              
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Input Case */
                if ((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_DALY) ||
                    (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_PTD)) {

                    uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;

                    UifACPGMLDClear();

                    MaintDisp(AC_SERVICE_READ_RPT,              /* A/C Number */
                              0,                                /* Descriptor */
                              0,                                /* Page Number */
                              ( UCHAR)pKeyMsg->SEL.INPUT.lData, /* PTD Type */
                              0,                                /* Report Type */
                              0,                                /* Reset Type */
                              0L,
                              0);                               /* Disable Lead Through */
                } else {
                    return(LDT_KEYOVER_ADR);                            /* Return Wrong Data Error */
                }
                if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_DALY) {        /* Daily Function Case */
                    RptServiceDayRead(CLASS_TTLCURDAY,
                                     RPT_ALL_READ);
                } else {                                                /* PTD Function Case */
                    RptServicePTDRead(CLASS_TTLCURPTD,
                                     RPT_ALL_READ);
                }
                UieExit(aACEnter);
                return(SUCCESS);
            }
            return(LDT_SEQERR_ADR);                                    /* Return Wrong Data Error */

        /* default:
              break; */
    }

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}



/**** End of File ****/
