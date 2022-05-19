/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Department Sales Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC26.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC26Function()               : A/C No.26 Function Entry Mode
*               UifAC26EnterDataType()          : A/C No.26 Enter Data Type Mode
*               UifAC26EnterRptType()           : A/C No.26 Enter Report Type Mode
*               UifAC26EnterMDEPT1()            : A/C No.26 Enter Major DEPT 1 Mode
*               UifAC26EnterDEPT1()             : A/C No.26 Enter DEPT 1 Mode
*               UifAC26EnterMDEPT2()            : A/C No.26 Enter Major DEPT 2 Mode
*               UifAC26EnterDEPT2()             : A/C No.26 Enter DEPT 2 Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-09-92:00.00.01    :K.You      : initial                                   
* Jul-26-95:03.00.03    :M.Ozawa    : Modify for Report on LCD
* Dec-10-99:01.00.00    :hrkato     : Saratoga
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2  R.Chambers   Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#include <mldsup.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/
    
/* Define Next Function */
UIMENU CONST aChildAC26EnterDataType[] = {{UifAC26EnterRptType, CID_AC26ENTERRPTTYPE},
                                             {NULL,                0                   }};

/* Define A/C Key Sequence */
UISEQ CONST aszSeqAC26EnterMDEPT2[] = {FSC_AC, FSC_CANCEL, 0};
UISEQ CONST aszSeqAC26EnterDEPT1[] = {FSC_KEYED_DEPT, FSC_AC, 0};
UISEQ CONST aszSeqAC26EnterDEPT2[] = {FSC_KEYED_DEPT, FSC_AC, FSC_CANCEL, 0};


/*
*=============================================================================
**  Synopsis: SHORT UifAC26Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */
UIMENU CONST aChildAC26Init[] = {{UifAC26EnterDataType, CID_AC26ENTERDATATYPE},
                                 {NULL,                 0                    }};


SHORT UifAC26Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Check PTD Exist or Not */
        if (!RptPTDChkFileExist(FLEX_DEPT_ADR)) {               /* Not PTD Case */
            uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;           /* Set Data Type to Own Save Area */
            UieNextMenu(aChildAC26EnterDataType);
        } else {
            UieNextMenu(aChildAC26Init);                            /* Open Next Function */
        }
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC26EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Enter Data Type Mode 
*===============================================================================
*/
SHORT UifAC26EnterDataType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        UifACPGMLDDispCom(DaylyPTDSelect);
        MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, 0, 0, 0, 0L, LDT_SLCDORP_ADR);                 /* "Select Daily or PTD Report" Lead Through Address */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);                                
            }

            /* Check Input Data Range */
            if (pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY && pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD) {
                return(LDT_KEYOVER_ADR);
            }
            
            /* Set Data Type to Own Save Area */
            uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            
            UieNextMenu(aChildAC26EnterDataType);
            return(SUCCESS);
        }

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC26EnterRptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Enter Report Type Mode 
*===============================================================================
*/

    
/* Define Next Function */
UIMENU CONST aChildAC26EnterRptType2[] = {{UifAC26EnterMDEPT1, CID_AC26ENTERMDEPT1},
                                          {NULL,               0                  }};


UIMENU CONST aChildAC26EnterRptType3[] = {{UifAC26EnterDEPT1, CID_AC26ENTERDEPT1},
                                          {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC26ENTERMDEPT1 */
UIMENU CONST aChildAC26EnterRptType4[] = {{UifAC26EnterMDEPT2, CID_AC26ENTERMDEPT2},
                                          {NULL,               0                     }};
 
/* Define Next Function at UIM_ACCEPTED from CID_AC26ENTERDEPT1 */
UIMENU CONST aChildAC26EnterRptType5[] = {{UifAC26EnterDEPT2, CID_AC26ENTERDEPT2},
                                          {NULL,              0                     }};
 

SHORT UifAC26EnterRptType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        UifACPGMLDDispCom(TypeSelectReport4);
        MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, 0, 0, 0L, LDT_REPOTYPE_ADR);  /* "Request Report Type Entry" Lead Through Address */
        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC26ENTERMDEPT1:
        case CID_AC26ENTERMDEPT2:
            UieNextMenu(aChildAC26EnterRptType4);               /* Open Next Function */
            return(SUCCESS);

        default:                                                /* CID_AC26ENTERDEPT1 case */
            UieNextMenu(aChildAC26EnterRptType5);               /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            return(LDT_SEQERR_ADR);

        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {             /* All Read Report Case */
                uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;     /* Set Report Type to Own Save Area */
                /* Control Display */
                UifACPGMLDClear();
                MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, ( UCHAR)RPT_TYPE_ALL, 0, 0L, 0);                                           /* Lead Through Address */

                if (uchUifACDataType == RPT_TYPE_DALY) {                /* Daily Report Case */
                    sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0);
                } else {                                                                /* PTD Report Case */
                    sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0);
                }
                UieExit(aACEnter);
                return(sError);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_MDEPTIND) {
                uchUifACRptType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;     /* Set Report Type to Own Save Area */
                UieNextMenu(aChildAC26EnterRptType2);                   /* Open Next Function */  
                return(SUCCESS);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_INDDEPT) {
                uchUifACRptType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;     /* Set Report Type to Own Save Area */
                UieNextMenu(aChildAC26EnterRptType3);                   /* Open Next Function */  
                return(SUCCESS);
            } else {
                return(LDT_KEYOVER_ADR);
            }
        }

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC26EnterMDEPT1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Enter Major DEPT 1 Mode 
*===============================================================================
*/
SHORT UifAC26EnterMDEPT1(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        UifACPGMLDClear();
        MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, uchUifACRptType, 0, 0L, LDT_MDEPT_ADR);   /* "Request Major Department No. Entry" Lead Through Address */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                           
                return(LDT_KEYOVER_ADR);
            }
            /* Clear Lead Through */
            MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, uchUifACRptType, 0, 0L, 0);  /* Disable  Lead Through */

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_MDEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);  /* Saratoga */
            } else {                                                                /* PTD Report Case */
                sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_MDEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);  /* Saratoga */
            }

            MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, uchUifACRptType, 0, 0L, LDT_MDEPT_ADR);  /* "Request Major Department No. Entry" Lead Through Address */
            if (sError == SUCCESS) {
                UieAccept();                                        /* Return to UifAC26EnterRptType() */
            } else if (sError == RPT_ABORTED) {
                sError = SUCCESS;
                UieExit(aACEnter);
            }
            return(sError);
        }

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC26EnterDEPT1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Enter DEPT 1 Mode 
*===============================================================================
*/
SHORT UifAC26EnterDEPT1(KEYMSG *pKeyMsg) 
{
    SHORT       sError;                    
    USHORT      usDEPTNumber;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC26EnterDEPT1);      /* Register Key Sequence */
        UifACPGMLDClear();
        MaintDisp(AC_DEPTSALE_READ_RPT, CLASS_MAINTDSPCTL_10NPG, 0, uchUifACDataType, uchUifACRptType, 0, 0L, LDT_DEPTNO_ADR);      /* "Request Department # Entry" Lead Through Address */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {       /* Saratoga */
                return(LDT_KEYOVER_ADR);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_DEPTIND_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);
            } else {                                                                /* PTD Report Case */
                sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_DEPTIND_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);
            }
            if (sError == SUCCESS) {      
                UieAccept();                                    /* Return to UifAC26EnterRptType() */
            }
            return(sError);                                 

        case FSC_KEYED_DEPT:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                        /* Over Digit */
                return(LDT_SEQERR_ADR);
            }

            /* Get Dept#,   Saratoga */
            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDEPTNumber)) != SUCCESS) {
                return(sError);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_DEPTIND_READ, usDEPTNumber);      /* Saratoga */
            } else {                                                                /* PTD Report Case */
                sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_DEPTIND_READ, usDEPTNumber);      /* Saratoga */
            }
            if (sError == SUCCESS) {                                                      
                UieAccept();
            }
            return(sError);
        }

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC26EnterMDEPT2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Enter Major DEPT 2 Mode 
*===============================================================================
*/
SHORT UifAC26EnterMDEPT2(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC26EnterMDEPT2);                                            /* Register Key Sequence */
        MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, uchUifACRptType, 0, 0L, LDT_MDEPT_ADR);  /* "Request Major Department No. Entry" Lead Through Address */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount */
                RptDEPTRead(0, RPT_FIN_READ, 0);         /* Execute Finalize Procedure */
                UieExit(aACEnter);                          /* Return to UifACEnter() */
                return(SUCCESS);
            }
                        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            /* Clear Lead Through */
            if(uchUifACRptOnOffMld == RPT_DISPLAY_OFF){
                UifACPGMLDClear();
            }

            MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, uchUifACRptType, 0, 0L, 0);   /* Disable  Lead Through */

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_MDEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);
            } else {                                                                /* PTD Report Case */
                sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_MDEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);
            }

            if(uchUifACRptOnOffMld == RPT_DISPLAY_OFF){
                UifACPGMLDClear();
            }

            MaintDisp(AC_DEPTSALE_READ_RPT, 0, 0, uchUifACDataType, uchUifACRptType, 0, 0L, LDT_MDEPT_ADR);                   /* "Request Major Department No. Entry" Lead Through Address */

            if (sError == SUCCESS) {
                UieAccept();                                    /* Return to UifACEnterRptType() */
            } else if (sError == RPT_ABORTED) {
                sError = SUCCESS;
                UieExit(aACEnter);
            }
            return(sError);

        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC26EnterDEPT2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.26 Enter DEPT 2 Mode 
*===============================================================================
*/
SHORT UifAC26EnterDEPT2(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
    USHORT  usDEPTNumber;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC26EnterDEPT2);      /* Register Key Sequence */
        MaintDisp(AC_DEPTSALE_READ_RPT, CLASS_MAINTDSPCTL_10NPG, 0, uchUifACDataType, uchUifACRptType, 0, 0L, LDT_DEPTNO_ADR);    /* "Request Department # Entry" Lead Through Address */
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                RptDEPTRead(0, RPT_FIN_READ, 0);                     /* Execute Finalize Procedure */
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {   /* Saratoga */
                return(LDT_KEYOVER_ADR);
            }
            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_DEPTIND_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);
            } else {                                                                /* PTD Report Case */
                sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_DEPTIND_READ, (USHORT)pKeyMsg->SEL.INPUT.lData);
            }
            if (sError == SUCCESS) {        
                UieAccept();                                        /* Return to UifAC26EnterRptType() */
            }
            return(sError);

        case FSC_KEYED_DEPT:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* Over Digit */
                return(LDT_SEQERR_ADR);
            }

            /* Get Dept#,   Saratoga */
            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, 
                &usDEPTNumber)) != SUCCESS) {
                return(sError);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_DEPTIND_READ, usDEPTNumber);      /* Saratoga */
            } else {                                        /* PTD Report Case */
                sError = RptDEPTRead(CLASS_TTLCURPTD, RPT_DEPTIND_READ, usDEPTNumber);      /* Saratoga */
            }
            if (sError == SUCCESS) {        
                UieAccept();                                                /* Return to UifAC26EnterRptType() */
            }
            return(sError);

        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;
        }

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/**** End of File ****/