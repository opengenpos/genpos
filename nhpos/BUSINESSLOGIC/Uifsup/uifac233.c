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
* Title       : Individual Financial File Reset Report Module  
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC233.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC233Function()       : A/C No.233 Function Entry Mode
*               UifAC233EnterRstType()   : A/C No.233 Enter Reset Type Mode
*               UifAC233EnterRptType()   : A/C No.233 Enter Report Type Mode
*               UifAC233EnterData()      : A/C No.233 Enter Terminal No. Mode
*               UifAC233EnterKey()       : A/C No.233 Reset Confirm Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Dec-05-95:03.01.00    : M.Ozawa   : initial                                   
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
*=============================================================================
**  Synopsis: SHORT UifAC233Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.233 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC233Init[] = {{UifAC233EnterRstType, CID_AC233ENTERRSTTYPE},
                                    {NULL,                0                   }};



SHORT UifAC233Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC233Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC233EnterRstType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.233 Enter Reset Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC233EnterRstType[] = {{UifAC233EnterRptType, CID_AC233ENTERRPTTYPE},
                                            {NULL,               0                   }};




SHORT UifAC233EnterRstType(KEYMSG *pKeyMsg) 
{
    
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(ResetTypeSelect);

        MaintDisp(AC_IND_RESET_RPT,                /* A/C Number */
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

            if (!pKeyMsg->SEL.INPUT.uchLen) {           /* W/o Input Data */                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }       
        
            /* Check Input Data */

            if ((pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_PRT) ||      /* Only Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RST_PRT)  ||      /* Reset and Print Type */
                (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ONLY_RST)) {      /* Reset and Print Type */
                uchUifACRstType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC233EnterRstType);                    /* Open Next Function */
                return(SUCCESS);
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
**  Synopsis: SHORT UifAC233EnterRptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.233 Enter Report Type Mode 
*===============================================================================
*/
    

/* Define Next Function */

UIMENU FARCONST aChildAC233EnterRptType1[] = {{UifAC233EnterKey, CID_AC233ENTERKEY},
                                             {NULL,            0               }};


UIMENU FARCONST aChildAC233EnterRptType2[] = {{UifAC233EnterData, CID_AC233ENTERDATA},
                                             {NULL,             0                }};


SHORT UifAC233EnterRptType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(TypeSelectReport2);

        MaintDisp(AC_IND_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOTYPE_ADR);                /* "Request Report Type Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {           /* W/o Input Data */                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }       
        
            /* Check Input Data */
            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {             /* Reset All Type */
                uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;                 /* Set Report Type to Save Area */
                if (uchUifACRstType != ( UCHAR)RPT_TYPE_ONLY_PRT) {     /* Not Only Print Type */ 
                    if ((sError = RptIndFinLock(RPT_ALL_LOCK, 0)) != SUCCESS) {
                        UieExit(aACEnter);                              /* Return to UifACEnter() */
                        return(sError);
                    } 
                    UieNextMenu(aChildAC233EnterRptType1);               /* Open Next Function */
                    return(SUCCESS);
                } else {                                                /* Only Print Type */
                    /* Clear Lead Through */
                    UifACPGMLDClear();

                    MaintDisp(AC_IND_RESET_RPT,                        /* A/C Number */
                              0,                                        /* Descriptor */
                              0,                                        /* Page Number */
                              0,                                        /* PTD Type */
                              uchUifACRptType,                          /* Report Type */
                              uchUifACRstType,                          /* Reset Type */
                              0L,                                       /* Amount Data */
                              0);                                       /* Disable Lead Through */

                    sError = RptIndFinRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0, RPT_ONLY_PRT_RESET);        /* Read Saved File */
                    UieExit(aACEnter);                                  /* Return to UifACEnter() */
                    if (sError == RPT_ABORTED) {                        /* Aborted by User */
                        sError = SUCCESS;
                    }
                    return(sError);
                }
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_IND) {      /* Individual Reset Type */
                uchUifACRptType = ( UCHAR)RPT_TYPE_IND;                 /* Set Report Type to Save Area */
                UieNextMenu(aChildAC233EnterRptType2);                   /* Open Next Function */
                return(SUCCESS);
            } else {
                return(LDT_KEYOVER_ADR);
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC233EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.233 Enter Terminal No. Mode 
*===============================================================================
*/
SHORT UifAC233EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_IND_RESET_RPT,                /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {                   
                return(LDT_KEYOVER_ADR);
            }

            if (uchUifACRstType == ( UCHAR)RPT_TYPE_ONLY_PRT) {             /* Only Print Reset Type */
 
                /* Clear Lead Through */
                UifACPGMLDClear();

                MaintDisp(AC_IND_RESET_RPT,                /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          0,                                /* PTD Type */
                          uchUifACRptType,                  /* Report Type */
                          uchUifACRstType,                  /* Reset Type */
                          0L,                               /* Amount Data */
                          0);                               /* Disable Lead Through */

                sError = RptIndFinDayRead(CLASS_TTLSAVDAY, RPT_IND_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, RPT_ONLY_PRT_RESET);  /* Read Saved File */
                UieExit(aACEnter);                                              /* Return to UifACEnter() */
                if (sError == RPT_ABORTED) {                                    /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }

            if ((sError = RptIndFinLock(RPT_IND_LOCK, ( USHORT)pKeyMsg->SEL.INPUT.lData)) == SUCCESS) {
                usUifACRptTermNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;
                UieNextMenu(aChildAC233EnterRptType1);                       /* Open Next Function */
                return(SUCCESS);
            }
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC233EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.233 Reset Confirm Mode 
*===============================================================================
*/
SHORT UifAC233EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC3EnterKey);         /* Register Key Sequence */

        UifACPGMLDClear();

        MaintDisp(AC_IND_RESET_RPT,                /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  uchUifACRstType,                  /* Reset Type */
                  ulUifACRptCashierNo,              /* Cashier Number */
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

            MaintDisp(AC_IND_RESET_RPT,                                /* A/C Number */
                      0,                                                /* Descriptor */
                      0,                                                /* Page Number */
                      0,                                                /* PTD Type */
                      uchUifACRptType,                                  /* Report Type */
                      uchUifACRstType,                                  /* Reset Type */
                      0L,                                               /* Amount Data */
                      0);                                               /* Disable Lead Through */

            if (uchUifACRptType == ( UCHAR)RPT_TYPE_ALL) {              /* All Reset Type */
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {      /* Reset and Print Type */
                    sError = RptIndFinDayReset(RPT_ALL_RESET, RPT_PRT_RESET, 0);  /* Reset and Print All Cashier File */
                    if (sError == SUCCESS) {
                        sError = RptIndFinRead(CLASS_TTLSAVDAY, RPT_ALL_READ, 0, RPT_PRT_RESET);     /* Read Saved File */
                    }
                } else {                                                    /* Only Reset Type */
                    sError = RptIndFinDayReset(RPT_ALL_RESET, RPT_ONLY_RESET, 0);      /* Only Reset All Cashier File */
                }
                RptIndFinUnLock();
            } else if (uchUifACRptType == ( UCHAR)RPT_TYPE_IND) {           /* Individual Reset Type */
                if (uchUifACRstType == ( UCHAR)RPT_TYPE_RST_PRT) {          /* Reset and Print Type */
                    sError = RptIndFinDayReset(RPT_IND_RESET, RPT_PRT_RESET, usUifACRptTermNo);                      /* Reset and Print All Cashier File */
                    if (sError == SUCCESS) {
                        sError = RptIndFinDayRead(CLASS_TTLSAVDAY, RPT_IND_READ, usUifACRptTermNo, RPT_PRT_RESET);   /* Read Saved File */
                    }
                } else {                                                    /* Only Reset Type */
                    sError = RptIndFinDayReset(RPT_IND_RESET, RPT_ONLY_RESET, usUifACRptTermNo);                     /* Only Reset All Cashier File */
                }
                RptIndFinUnLock();
            }
            UieExit(aACEnter);                                              /* Return to UifACEnter() */
            if (sError == RPT_ABORTED) {                                    /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        case FSC_CANCEL:
            RptIndFinUnLock();
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

