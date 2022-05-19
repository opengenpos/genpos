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
* Title       : Coupon All Reset Report Module
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC40.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC40Function()       : A/C No.40 Function Entry Mode
*               UifAC40EnterDataType()  : A/C No.40 Enter Data Type Mode
*               UifAC40EnterRstType()   : A/C No.40 Enter Reset Type Mode
*               UifAC40EnterKey()       : A/C No.40 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* May-08-95: 03.00.00   : T.Satoh   : initial
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

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s
;=============================================================================
*/

/* Define Next Function */
UIMENU FARCONST
aChildAC40EnterDataType[] = {{UifAC40EnterRstType, CID_AC40ENTERRSTTYPE},
                            {NULL,                0                   }};

UIMENU FARCONST
aChildAC40Init[]          = {{UifAC40EnterDataType, CID_AC40ENTERDATATYPE},
                             {NULL,                 0                    }};

UIMENU FARCONST
aChildAC40EnterRstType[]  = {{UifAC40EnterKey, CID_AC40ENTERKEY},
                             {NULL,            0               }};
/* reduce key data, V3.3
UISEQ FARCONST aszSeqAC40EnterDataType[] = {FSC_NUM_TYPE, FSC_AC, 0};
UISEQ FARCONST aszSeqAC40EnterKey[]      = {FSC_AC, FSC_CANCEL, 0};
*/
/*
*=============================================================================
**  Synopsis: SHORT UifAC40Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.40 Function Entry Mode  
*==============================================================================
*/

SHORT UifAC40Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg){
        case UIM_INIT:
            /*
             * PTD File is not Existed
             */
            if(!RptPTDChkFileExist(FLEX_CPN_ADR)){
                /* Set Data Type to Own Save Area */
                uchUifACDataType = (UCHAR)RPT_TYPE_DALY;
                UieNextMenu(aChildAC40EnterDataType);
            }
            /*
             * PTD File is Existed
             */
            else{
                UieNextMenu(aChildAC40Init);
            }
            return(SUCCESS);

        default:
            return(UifACDefProc(pKeyMsg));    /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC40EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.40 Enter Data Type Mode 
*==============================================================================
*/

SHORT UifAC40EnterDataType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC23EnterDataType);
            /* UieOpenSequence(aszSeqAC40EnterDataType); */

            UifACPGMLDDispCom(DaylyPTDSelect);

            MaintDisp(AC_CPN_RESET_RPT,     /* A/C Number                   */
                      0, 0, 0, 0, 0, 0L,
                      LDT_SLCDORP_ADR);     /* "Select Daily or PTD Report" */
            return(SUCCESS);

        case UIM_INPUT:
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_AC:
                    /* allow AC key for PTD, V3.3 */
                    /* return(LDT_SEQERR_ADR); */

                case FSC_NUM_TYPE:
                    /* Check with aomething keys */
                    if(!pKeyMsg->SEL.INPUT.uchLen){
                        return(LDT_SEQERR_ADR);
                    }

                    /* Check Input Data Range */
                    if(pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY &&
                                   pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD){
                        return(LDT_KEYOVER_ADR);
                    }

                    /* Set Data Type to Own Save Area */
                    uchUifACDataType = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    UieNextMenu(aChildAC40EnterDataType);
                    return(SUCCESS);

                /* default: break; */
            }
            /* break; */

        default:
            return(UifACDefProc(pKeyMsg));     /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC40EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.40 Enter Reset Type Mode 
*==============================================================================
*/

SHORT UifAC40EnterRstType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC23EnterDataType);
            /* UieOpenSequence(aszSeqAC40EnterDataType); */

            UifACPGMLDDispCom(ResetTypeSelect);

            MaintDisp(AC_CPN_RESET_RPT,    /* A/C Number                    */
                      0, 0,
                      uchUifACDataType,    /* PTD Type                      */
                      0, 0, 0L,
                      LDT_FUNCTYPE_ADR);   /* "Request Function Type Entry" */
            return(SUCCESS);

        case UIM_INPUT:
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_NUM_TYPE:
                    return(LDT_SEQERR_ADR);

                case FSC_AC:
                    /* Check with something keys */
                    if(!pKeyMsg->SEL.INPUT.uchLen){
                        return(LDT_SEQERR_ADR);
                    }

                    /* Input Data is Only Print Type */
                    if(pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT){

                        /* Clear Lead Through */

                        UifACPGMLDClear();

                        MaintDisp(AC_CPN_RESET_RPT,/* A/C Number */
                                  0, 0,
                                  uchUifACDataType,/* PTD Type */
                                  0,
                                  (UCHAR)RPT_TYPE_ONLY_PRT,/* Reset Type */
                                  0L, 0);

                        /* Daily File Type */
                        if(uchUifACDataType == (UCHAR)RPT_TYPE_DALY){
                            if((sError = RptCpnRead(CLASS_TTLSAVDAY,
                                          RPT_ALL_READ,0)) != SUCCESS){
                                return(sError);
                            }
                        }
                        else{
                            if((sError = RptCpnRead(CLASS_TTLSAVPTD,
                                          RPT_ALL_READ,0)) != SUCCESS){
                                return(sError);
                            }
                        }

                        UieExit(aACEnter);        /* Return to UifACEnter() */
                        return(SUCCESS);
                    }
                    else
                    /* Reset and Print Type OR Only Reset Type */
                    if((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT) ||
                       (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)){
                        if((sError = RptCpnLock()) == SUCCESS){
                            /* Save Reset Type */
                            uchUifACRstType = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                            UieNextMenu(aChildAC40EnterRstType);
                        }
                        else{
                            UieExit(aACEnter);    /* Return to UifACEnter() */
                        }
                        return(sError);
                    }
                    else{
                        return(LDT_KEYOVER_ADR);
                    }

                /* default: break; */
            }
            /* break; */

        default:
            return(UifACDefProc(pKeyMsg));    /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC40EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.40 Reset Confirm Mode 
*==============================================================================
*/

SHORT UifAC40EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError; 

    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC3EnterKey);
            /* UieOpenSequence(aszSeqAC40EnterKey); */

            UifACPGMLDClear();

            MaintDisp(AC_CPN_RESET_RPT,       /* A/C Number                 */
                      CLASS_MAINTDSPCTL_10N,  /* 10N10D Display             */
                      0,
                      uchUifACDataType,  /* PTD Type                        */
                      0,
                      uchUifACRstType,   /* Reset Type                      */
                      0L,
                      LDT_RESET_ADR);    /* "Lead Through for Reset Report" */
            return(SUCCESS);
                                                                            
        case UIM_INPUT:
            /* Check With something keys */
            if(pKeyMsg->SEL.INPUT.uchLen){
                return(LDT_SEQERR_ADR);
            }

            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_AC:
                    /* Clear Lead Through */

                    UifACPGMLDClear();

                    MaintDisp(AC_CPN_RESET_RPT,         /* A/C Number       */
                              0, 0,
                              uchUifACDataType,         /* PTD Type         */
                              0,
                              uchUifACRstType,          /* Reset Type       */
                              0L, 0);

                    /* Daily File Type */
                    if(uchUifACDataType == (UCHAR)RPT_TYPE_DALY){
                        /* Reset and Print Type  */
                        if(uchUifACRstType == (UCHAR)RPT_TYPE_RST_PRT){
                            if((sError=RptCpnReset(CLASS_TTLSAVDAY,RPT_PRT_RESET))==SUCCESS){
                                RptCpnRead(CLASS_TTLSAVDAY,
                                              RPT_ALL_READ,0);
                            }
                        }
                        else{
                            sError = RptCpnReset(CLASS_TTLSAVDAY,RPT_ONLY_RESET);
                        }
                    }
                    else{
                        /* Reset and Print Type  */
                        if(uchUifACRstType == (UCHAR)RPT_TYPE_RST_PRT){
                            if((sError=RptCpnReset(CLASS_TTLSAVPTD,RPT_PRT_RESET))==SUCCESS){
                                RptCpnRead(CLASS_TTLSAVPTD,
                                              RPT_ALL_READ,0);
                            }
                        }
                        else{
                            sError = RptCpnReset(CLASS_TTLSAVPTD,RPT_ONLY_RESET);
                        }
                    }

                    RptCpnUnLock();
                    UieExit(aACEnter);           /* Return to UifACEnter() */
                    return(sError);

                case FSC_CANCEL:
                    RptCpnUnLock();
                    UieExit(aACEnter);           /* Return to UifACEnter() */
                    return(SUCCESS);

                /* default: break; */
            }
            /* break; */

        default:
            return(UifACDefProc(pKeyMsg));    /* Execute Default Procedure */
    }
}
