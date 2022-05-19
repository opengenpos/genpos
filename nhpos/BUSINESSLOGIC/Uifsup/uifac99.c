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
* Title       : EOD Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC99.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC99Function()       : A/C No.99 Function Entry Mode
*               UifAC99EnterRstType()   : A/C No.99 Enter Reset Type Mode
*               UifAC99EnterKey()       : A/C No.99 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-09-92:00.00.01    :K.You      : initial                                   
* Feb-27-96:03.01.00    :M.Ozawa    : enhanced to programable eod
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
#include <progrept.h>
#include <csprgrpt.h>
#include <mldmenu.h>
#include "appllog.h"

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT UifAC99Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.99 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC99Init[] = {{UifAC99EnterRstType, CID_AC99ENTERRSTTYPE},
                                    {NULL,                0                   }};

SHORT UifAC99Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC99Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC99EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.99 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC99EnterRstType[] = {{UifAC99EnterKey, CID_AC99ENTERKEY},
                                            {NULL,            0               }};

SHORT UifAC99EnterRstType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
    USHORT  usNoOfRpt;
    PROGRPT_EODPTDIF aEODRec[MAX_PROGRPT_EODPTD];

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        UifACPGMLDDispCom(ResetTypeSelect);
        MaintDisp(AC_EOD_RPT, 0, 0, 0, 0, 0, 0L, LDT_FUNCTYPE_ADR);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {      
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
    
			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 99);                                  // Action Code 99, End of Day
			PifLog (MODULE_LINE_NO(MODULE_PRINT_SUPPRG_ID), (USHORT)__LINE__);                       // Action Code 99, End of Day
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), (USHORT)(pKeyMsg->SEL.INPUT.lData));  // Action Code 99, End of Day

			/* Check Input Data */
            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT) {        /* Only Print Type */

                /* Clear Lead Through */
                UifACPGMLDClear();

                MaintDisp(AC_EOD_RPT, 0, 0, 0, 0, ( UCHAR)pKeyMsg->SEL.INPUT.lData, 0L, 0);

                if (((sError = ProgRptGetEODInfo(&usNoOfRpt, aEODRec)) == PROGRPT_SUCCESS) && (usNoOfRpt)) {
                    sError = ProgRptEODReset(( UCHAR)RPT_TYPE_ONLY_PRT, aEODRec, usNoOfRpt);
                } else {
                    RptEODRead(( UCHAR)RPT_TYPE_ONLY_PRT);
                    sError = SUCCESS;
                }
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                return(sError);
            } else if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) || (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {
				ProgRptLog(1);
                if ((sError = RptEODPTDLock(AC_EOD_RPT)) == SUCCESS) {
					ProgRptLog(2);
                    uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData; /* Save Reset Type */
                    UieNextMenu(aChildAC99EnterRstType);                /* Open Next Function */
                } else {
                    UieExit(aACEnter);                                  /* Return to UifACEnter() */
                }
                return(sError);
            } else {
                return(LDT_KEYOVER_ADR);
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC99EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.99 Reset Confirm Mode 
*===============================================================================
*/
SHORT UifAC99EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
    USHORT  usNoOfRpt;
    PROGRPT_EODPTDIF aEODRec[MAX_PROGRPT_EODPTD];

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);                             /* Register Key Sequence */
        UifACPGMLDClear();
        MaintDisp(AC_EOD_RPT, CLASS_MAINTDSPCTL_10N, 0, 0, 0, uchUifACRstType, 0L, LDT_RESET_ADR);                   /* "Lead Through for Reset Report"Lead Through Address */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        /* Check W/ Amount */
        if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }
        
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            UifACPGMLDClear();
            MaintDisp(AC_EOD_RPT, 0, 0, 0, 0, uchUifACRstType, 0L, 0);
			ProgRptLog(3);

			PifLog (MODULE_PRINT_SUPPRG_ID, LOG_EVENT_UIFUIE_ACTIONCODE);
			PifLog (MODULE_DATA_VALUE(MODULE_PRINT_SUPPRG_ID), 99);                                  // Action Code 99, End of Day
			PifLog (MODULE_LINE_NO(MODULE_PRINT_SUPPRG_ID), (USHORT)__LINE__);                       // Action Code 99, End of Day

			if (((sError = ProgRptGetEODInfo(&usNoOfRpt, aEODRec)) == PROGRPT_SUCCESS) && (usNoOfRpt)) {
				ProgRptLog(4);
                sError = ProgRptEODReset(uchUifACRstType, aEODRec, usNoOfRpt);
				ProgRptLog(58);
            } else {
                if ((sError = RptEODReset(uchUifACRstType)) == SUCCESS) {          
                    if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {      /* Reset and Print Type  */
                        RptEODRead(uchUifACRstType);                                           
                    }
                }
            }
            RptEODPTDUnLock();
            UieExit(aACEnter);                                          /* Return to UifACEnter() */
            if (sError == RPT_RESET_FAIL) {                             /* All Reset Incompleted */
                sError = SUCCESS;
            }
            return(sError);

        case FSC_CANCEL:
            RptEODPTDUnLock();
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

