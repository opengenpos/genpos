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
* Title       : PTD Reset Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC18.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC18Function()       : A/C No.18 Function Entry Mode
*               UifAC18EnterRstType()   : A/C No.18 Enter Reset Type Mode
*               UifAC18EnterKey()       : A/C No.18 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
* Feb-21-96:03.01.00    :M.Ozawa    : enhanced to programable eod
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
**  Synopsis: SHORT UifAC18Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.18 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC18Init[] = {{UifAC18EnterRstType, CID_AC18ENTERRSTTYPE},
                                    {NULL,                0                   }};



SHORT UifAC18Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC18Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC18EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.18 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC18EnterRstType[] = {{UifAC18EnterKey, CID_AC18ENTERKEY},
                                            {NULL,            0               }};




SHORT UifAC18EnterRstType(KEYMSG *pKeyMsg) 
{
    
                    
    SHORT   sError;
    USHORT  usNoOfRpt;
    PROGRPT_EODPTDIF aPTDRec[MAX_PROGRPT_PTD];


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_PTD_RPT,                       /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                /* "Request Function Type Entry" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {      
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
    
            /* Check Input Data */

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT) {        /* Only Print Type */

                /* Clear Lead Through */

                UifACPGMLDClear();

                MaintDisp(AC_PTD_RPT,                                   /* A/C Number */
                          0,                                            /* Descriptor */
                          0,                                            /* Page Number */
                          0,                                            /* PTD Type */
                          0,                                            /* Report Type */
                          ( UCHAR)pKeyMsg->SEL.INPUT.lData,             /* Reset Type */
                          0L,                                           /* Amount Data */
                          0);                                           /* Disable Lead Through */

                if (((sError = ProgRptGetPTDInfo(&usNoOfRpt, aPTDRec)) == PROGRPT_SUCCESS) &&
                    (usNoOfRpt)) {

                    sError = ProgRptPTDReset(( UCHAR)RPT_TYPE_ONLY_PRT, aPTDRec, usNoOfRpt);

                } else {

                   RptPTDRead(( UCHAR)RPT_TYPE_ONLY_PRT);
                   sError = SUCCESS;
                }
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                return(sError);
            } else if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) || /* Reset and Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {      /* Only Reset Type */
                if ((sError = RptEODPTDLock(AC_PTD_RPT)) == SUCCESS) {
                    uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData; /* Save Reset Type */
                    UieNextMenu(aChildAC18EnterRstType);                /* Open Next Function */
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
**  Synopsis: SHORT UifAC18EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.18 Reset Confirm Mode 
*===============================================================================
*/
    


SHORT UifAC18EnterKey(KEYMSG *pKeyMsg) 
{
    
                            
    SHORT   sError;
    USHORT  usNoOfRpt;
    PROGRPT_EODPTDIF aPTDRec[MAX_PROGRPT_PTD];


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);         /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_PTD_RPT,                       /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_RESET_ADR);                   /* "Lead Through for Reset Report"Lead Through Address */

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

            MaintDisp(AC_PTD_RPT,                       /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      uchUifACRstType,                  /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (((sError = ProgRptGetPTDInfo(&usNoOfRpt, aPTDRec)) == PROGRPT_SUCCESS) &&
                (usNoOfRpt)) {

                sError = ProgRptPTDReset(uchUifACRstType, aPTDRec, usNoOfRpt);

            } else {

                if ((sError = RptPTDReset(uchUifACRstType)) == SUCCESS) {          
                    if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type  */
                        RptPTDRead(uchUifACRstType);                                           
                    }
                }
            }
            RptEODPTDUnLock();
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            if (sError == RPT_RESET_FAIL) {                                 /* All Reset Incompleted */
                sError = SUCCESS;
            }
            return(sError);

        case FSC_CANCEL:
            if (uchUifACRstType != ( UCHAR)RPT_TYPE_ONLY_PRT) {             /* Reset Case */
                RptEODPTDUnLock();
            }
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

