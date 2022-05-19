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
* Title       : Media Flash Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC123.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC123Function()       : AC No.123 Function Entry Mode 
*               UifAC123EnterDataType()  : AC No.123 Enter Data Type   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-09-92: K.You     : initial                                   
* Jul-26-95: M.Ozawa   : Modify for Report on LCD
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
*=============================================================================
**  Synopsis:    SHORT UifAC123Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.123 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC123EnterDataType[] = {{UifAC123EnterDataType,CID_AC123ENTERDATATYPE},
                                              {NULL,                 0                    }};

    
SHORT UifAC123Function(KEYMSG *pKeyMsg) 
{
    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC123EnterDataType);      /* Open Next Function */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_MEDIA_RPT,                     /* A/C Number */
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
**  Synopsis: SHORT UifAC123EnterDataType(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: AC No.123 Enter Data Type    
*===============================================================================
*/


SHORT UifAC123EnterDataType(KEYMSG *pKeyMsg) 
{
    
    SHORT   sError;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:                                              
            if (pKeyMsg->SEL.INPUT.uchLen) {                            /* W/ Amount Input Case */
                if ((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_DALY) ||
                    (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_PTD)) {

                    uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;

                    UifACPGMLDClear();

                    MaintDisp(AC_MEDIA_RPT,                             /* A/C Number */
                              0,                                        /* Descriptor */
                              0,                                        /* Page Number */
                              ( UCHAR)pKeyMsg->SEL.INPUT.lData,         /* PTD Type */
                              0,                                        /* Report Type */
                              0,                                        /* Reset Type */
                              0L,
                              0);                               /* Disable Lead Through */
                } else {
                    return(LDT_KEYOVER_ADR);                            /* Return Wrong Data Error */
                }
                if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_DALY) {        /* Daily Function Case */
                    sError = RptRegFinDayRead(CLASS_TTLCURDAY, RPT_MEDIA_READ);
                } else {                                                /* PTD Function Case */
                    sError = RptRegFinPTDRead(CLASS_TTLCURPTD, RPT_MEDIA_READ);
                } 
                UieExit(aACEnter);
                return(sError);
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
