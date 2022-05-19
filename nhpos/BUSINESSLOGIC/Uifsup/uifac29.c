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
* Title       : PLU Sales Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC29.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC29Function()               : A/C No.29 Function Entry Mode
*               UifAC29EnterDataType()          : A/C No.29 Enter Data Type Mode
*               UifAC29EnterRptType()           : A/C No.29 Enter Report Type Mode
*               UifAC29EnterDEPT1()             : A/C No.29 Enter DEPT 1 Mode
*               UifAC29EnterRptCode1()          : A/C No.29 Enter Report Code 1 Mode
*               UifAC29EnterPLU1()              : A/C No.29 Enter PLU 1 Mode
*               UifAC29EnterDEPT2()             : A/C No.29 Enter DEPT 2 Mode
*               UifAC29EnterRptCode2()          : A/C No.29 Enter Report Code 2 Mode
*               UifAC29EnterPLU2()              : A/C No.29 Enter PLU 2 Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name   : Description
* Jul-09-92:00.00.01  :K.You     : initial                                   
* Jul-21-93:01.00.12  : J.IKEDA  : Adds Direct Menu Shift Key in UifAC29EnterPLU1()     
*          :          :          : & UifAC29EnterPLU2() 
* Jul-22-93:01.00.12  : J.IKEDA  : Adds MaintDisp() for KEYED DEPT in UifAC29EnterDEPT1()
* Jul-26-95:03.00.03  : M.Ozawa  : Adds Report on Display
* Dec-10-99:01.00.00  : hrkato   : Saratoga
** GenPOS Rel 2.x
* Nov-02-16:02.02.02  : R.Chambers  : allow use of AC key to enter PLU number like PLU key.
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
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
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
UIMENU CONST aChildAC29EnterDataType[] = {{UifAC29EnterRptType, CID_AC29ENTERRPTTYPE},
                                          {NULL,                0                   }};

/* Define Key Sequence */
UISEQ CONST aszSeqAC29EnterDEPT1[] = {FSC_KEYED_DEPT, FSC_AC, 0};
UISEQ CONST aszSeqAC29EnterDEPT2[] = {FSC_KEYED_DEPT, FSC_AC, FSC_CANCEL, 0};
UISEQ CONST aszSeqAC29EnterPLU1[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, FSC_PLU,
                                    FSC_KEYED_PLU, FSC_AC, FSC_SCANNER, FSC_E_VERSION, FSC_VIRTUALKEYEVENT, 0};
UISEQ CONST aszSeqAC29EnterPLU2[] = {FSC_MENU_SHIFT, FSC_D_MENU_SHIFT, FSC_PLU,
                                    FSC_KEYED_PLU, FSC_AC, FSC_SCANNER, FSC_E_VERSION, FSC_VIRTUALKEYEVENT, FSC_CANCEL, 0};
UISEQ CONST aszSeqAC29EnterRptCode2[] = {FSC_AC, FSC_CANCEL, 0};

/*
*=============================================================================
**  Synopsis: SHORT UifAC29Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */
UIMENU CONST aChildAC29Init[] = {{UifAC29EnterDataType, CID_AC29ENTERDATATYPE},
                                 {NULL,                 0                    }};


SHORT UifAC29Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Check PTD Exist or Not */
        if (!RptPTDChkFileExist(FLEX_PLU_ADR)) {                /* Not PTD Case */
            uchUifACDataType = (UCHAR)RPT_TYPE_DALY;            /* Set Data Type to Own Save Area */
            UieNextMenu(aChildAC29EnterDataType);
        } else {
            UieNextMenu(aChildAC29Init);                        /* Open Next Function */
        }
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter Data Type Mode 
*===============================================================================
*/
SHORT UifAC29EnterDataType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        UifACPGMLDDispCom(DaylyPTDSelect);
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_SLCDORP_ADR);                 /* "Select Daily or PTD Report" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Input Data Range */
            if (pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY && pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD) {
                return(LDT_KEYOVER_ADR);
            }
            
            /* Set Data Type to Own Save Area */
            uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            UieNextMenu(aChildAC29EnterDataType);
            return(SUCCESS);
        }

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterRptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter Report Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU CONST aChildAC29EnterRptType2[] = {{UifAC29EnterDEPT1, CID_AC29ENTERDEPT1},
                                          {NULL,              0                 }};

UIMENU CONST aChildAC29EnterRptType3[] = {{UifAC29EnterRptCode1, CID_AC29ENTERRPTCODE1},
                                          {NULL,                 0                    }};

UIMENU CONST aChildAC29EnterRptType4[] = {{UifAC29EnterPLU1, CID_AC29ENTERPLU1},
                                          {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED from CID_AC29ENTERDEPT1 */
UIMENU CONST aChildAC29EnterRptType5[] = {{UifAC29EnterDEPT2, CID_AC29ENTERDEPT2},
                                          {NULL,              0                 }};
 
/* Define Next Function at UIM_ACCEPTED from CID_AC29ENTERRPTCODE1 */
UIMENU CONST aChildAC29EnterRptType6[] = {{UifAC29EnterRptCode2, CID_AC29ENTERRPTCODE2},
                                          {NULL,                 0                    }};
 
/* Define Next Function at UIM_ACCEPTED from CID_AC29ENTERPLU1 */
UIMENU CONST aChildAC29EnterRptType7[] = {{UifAC29EnterPLU2, CID_AC29ENTERPLU2},
                                          {NULL,             0                }};
 

SHORT UifAC29EnterRptType(KEYMSG *pKeyMsg) 
{
	TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1] = {0};
    SHORT   sError;                            

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        UifACPGMLDDispCom(TypeSelectReport5);
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOTYPE_ADR);                /* "Request Report Type Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC29ENTERDEPT1:                                /* Accepted from CID_AC29ENTERDEPT1 */
        case CID_AC29ENTERDEPT2:                                /* Accepted from CID_AC29ENTERDEPT2 */
            UieNextMenu(aChildAC29EnterRptType5);               /* Open Next Function */
            return(SUCCESS);

        case CID_AC29ENTERRPTCODE1:                             /* Accepted from CID_AC29ENTERRPTCODE1 */
        case CID_AC29ENTERRPTCODE2:                             /* Accepted from CID_AC29ENTERRPTCODE2 */
            UieNextMenu(aChildAC29EnterRptType6);               /* Open Next Function */
            return(SUCCESS);

        default:                                                /* Accepted from CID_AC29ENTERPLU1 */
            UieNextMenu(aChildAC29EnterRptType7);               /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            return(LDT_SEQERR_ADR);

        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {     /* All Read Report Case */

                /* Control Display */
                uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;     /* Set Report Type to Own Save Area */
                UifACPGMLDClear();
                MaintDisp(AC_PLUSALE_READ_RPT,                  /* A/C Number */
                          0,                                    /* Descriptor */
                          0,                                    /* Page Number */
                          uchUifACDataType,                     /* PTD Type */
                          ( UCHAR)RPT_TYPE_ALL,                 /* Report Type */
                          0,                                    /* Reset Type */
                          0L,                                   /* Amount Data */
                          0);                                   /* Lead Through Address */

                if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                    sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0, auchPLUNumber);
                } else {                                                                /* PTD Report Case */
                    sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0, auchPLUNumber);
                }
                UieExit(aACEnter);                      
                return(sError);

            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_DEPTIND) {      /* Individual Read Report by DEPT */
                uchUifACRptType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;         /* Set Report Type to Own Save Area */
                UieNextMenu(aChildAC29EnterRptType2);                       /* Open Next Function */  
                return(SUCCESS);                                                    
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_RPTCODEIND) {   /* Individual Read Report by Report Code */ 
                uchUifACRptType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;         /* Set Report Type to Own Save Area */
                UieNextMenu(aChildAC29EnterRptType3);                       /* Open Next Function */  
                return(SUCCESS);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_INDPLU) {       /* Individual Read Report */ 
                uchUifACRptType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;         /* Set Report Type to Own Save Area */
                UieNextMenu(aChildAC29EnterRptType4);                       /* Open Next Function */  
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
**  Synopsis: SHORT UifAC29EnterDEPT1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter DEPT 1 Mode 
*===============================================================================
*/
SHORT UifAC29EnterDEPT1(KEYMSG *pKeyMsg) 
{
	TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1] = {0};
    SHORT   sError;
    USHORT  usDept;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterDEPT1);      /* Register Key Sequence */
        UifACPGMLDClear();
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {       /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {   /* Saratoga */
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_DEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_DEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
            }

            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

            if (sError == SUCCESS) {
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
            } else if (sError == RPT_ABORTED) {
                sError = SUCCESS;
                UieExit(aACEnter);
            }
            return(sError);

        case FSC_KEYED_DEPT:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                        /* Over Digit */
                return(LDT_SEQERR_ADR);
            }
            /* Clear Lead Through */
            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDept)) != SUCCESS) {
                return(sError);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_DEPT_READ, usDept, 0, auchPLUNumber);
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_DEPT_READ, usDept, 0, auchPLUNumber);
            }

            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

            if (sError == SUCCESS) {                                                      
                UieAccept();                                    /* Return to UifACEnterRptType() */
            } else if (sError == RPT_ABORTED) {
                sError = SUCCESS;
                UieExit(aACEnter);
            }
            return(sError);
        }

    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterRptCode1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter Report Code 1 Mode 
*===============================================================================
*/
SHORT UifAC29EnterRptCode1(KEYMSG *pKeyMsg) 
{
	TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1] = {0};
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        UifACPGMLDClear();
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOCD_ADR);                  /* "Request Report Code # Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_CODE_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_CODE_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
            }

            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_REPOCD_ADR);                  /* "Request Report Code # Entry" Lead Through Address */

            if (sError == SUCCESS) {                                                      
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
            } else if (sError == RPT_ABORTED) {
                sError = SUCCESS;
                UieExit(aACEnter);
            }
            return(sError);
        }

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterPLU1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter PLU 1 Mode 
*===============================================================================
*/
SHORT UifAC29EnterPLU1(KEYMSG *pKeyMsg) 
{
	TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1+1+1] = {0};
	UCHAR			uchModStat;
    SHORT           sError;
	MAINTSETPAGE    SetPageNo = {0};
    MAINTPLU        MaintPLU = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterPLU1);       /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);
        UifACPGMLDClear();
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  MaintGetPage(),                   /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLUSALE_READ_RPT;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_MENU_SHIFT:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SetPageNo.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
                SetPageNo.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                SetPageNo.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;                   /* Set Major Class */
            SetPageNo.uchACNumber = AC_PLUSALE_READ_RPT;                    /* Set Major Class */
            SetPageNo.uchPTDType = uchUifACDataType;
            SetPageNo.uchRptType = uchUifACRptType;
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL  *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                                /* Shift Cur. Func. to Act. Func. */ 
            }
            return(sError);

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;               /* Set Major Class */
            SetPageNo.uchACNumber = AC_PLUSALE_READ_RPT;                /* Set Major Class */
            SetPageNo.uchPTDType = uchUifACDataType;
            SetPageNo.uchRptType = uchUifACRptType;
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            SetPageNo.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;      /* set page number */
            SetPageNo.uchStatus = 0;                                    /* Set W/ Amount Status */
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL  *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                            /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_VIRTUALKEYEVENT:
			if (UieVirtualKeyToPhysicalDev (UIE_VIRTUALKEY_EVENT_SCANNER, pKeyMsg) < 0) {
				return(LDT_KEYOVER_ADR);
			}
//            break;  // drop through to the scanner code.

		case FSC_SCANNER:                                   /* Saratoga */
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }
            _tcsncpy(auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            if (uchUifACDataType == RPT_TYPE_DALY) {                            /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            }
            if (sError == SUCCESS) {          
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
            }
            return(sError);

        case FSC_E_VERSION:                         /* Saratoga */
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchMinorClass = CLASS_PARAPLU_SET_EV;     /* Set Minor Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PLUSALE_READ_RPT;
            MaintPLU.uchEVersion = 1;               /* Set E-Version */
            return(MaintPLUModifier(&MaintPLU));

        case FSC_AC:
//            return(LDT_SEQERR_ADR);   allow use of the AC or Action Code key to enter a PLU number
        case FSC_PLU:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14)) {                           
                return(LDT_KEYOVER_ADR);
            }
            _tcscpy(auchPLUNumber, pKeyMsg->SEL.INPUT.aszKey);                   /* Saratoga */
            if (uchUifACDataType == RPT_TYPE_DALY) {                            /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            }
            if (sError == SUCCESS) {          
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
            }
            return(sError);

        case FSC_KEYED_PLU:
            /* Check W/O Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_KEYOVER_ADR);
            }

            if ((sError = MaintGetPLUNo(pKeyMsg->SEL.INPUT.uchMinor, 
                auchPLUNumber, &uchModStat)) != SUCCESS) {
                return(sError);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                    /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);   /* Saratoga */
            } else {                                                    /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            }
            if (sError == SUCCESS) {
                UieAccept();                                        /* Return to UifACEnterRptType() */
            }
            return(sError);
        }

    case UIM_CANCEL:
        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLUSALE_READ_RPT;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        /* break */
    
    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterDEPT2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter DEPT 2 Mode 
*===============================================================================
*/
SHORT UifAC29EnterDEPT2(KEYMSG *pKeyMsg) 
{
	TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1] = {0};
    SHORT   sError;
    USHORT  usDept;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterDEPT2);      /* Register Key Sequence */
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount */
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);
                UieExit(aACEnter);                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {   /* Saratoga */
                return(LDT_KEYOVER_ADR);                                                
            }

            /* Clear Lead Through */
            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACDataType == RPT_TYPE_DALY) {                    /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_DEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);                  /* Saratoga */
            } else {                                                    /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_DEPT_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);                  /* Saratoga */
            }

            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

            if (sError == SUCCESS) {                                                      
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
            } else if (sError == RPT_ABORTED) {
                sError = SUCCESS;
                UieExit(aACEnter);
            }
            return(sError);

        case FSC_KEYED_DEPT:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                        /* Over Digit */
                return(LDT_SEQERR_ADR);
            }

            /* Clear Lead Through */
            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if ((sError = MaintGetDEPTNo(pKeyMsg->SEL.INPUT.uchMinor, &usDept)) != SUCCESS) {
                return(sError);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                    /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_DEPT_READ, usDept, 0, auchPLUNumber);                  /* Saratoga */
            } else {                                                    /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_DEPT_READ, usDept, 0, auchPLUNumber);                  /* Saratoga */
            }

            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_DEPTNO_ADR);                  /* "Request Department No. Entry" Lead Through Address */

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
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterRptCode2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter Report Code 2 Mode 
*===============================================================================
*/
SHORT UifAC29EnterRptCode2(KEYMSG *pKeyMsg) 
{
	TCHAR   auchPLUNumber[PLU_MAX_DIGIT+1+1] = {0};
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterRptCode2);   /* Register Key Sequence */
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOCD_ADR);                  /* "Request Report Code # Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);    /* Saratoga */
                UieExit(aACEnter);
                return(SUCCESS);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            /* Clear Lead Through */
            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_CODE_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_CODE_READ, (USHORT)pKeyMsg->SEL.INPUT.lData, 0, auchPLUNumber);
            }

            MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      uchUifACDataType,                 /* PTD Type */
                      uchUifACRptType,                  /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_REPOCD_ADR);                  /* "Request Report Code # Entry" Lead Through Address */

            if (sError == SUCCESS) {                                                      
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
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
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC29EnterPLU2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.29 Enter PLU 2 Mode 
*===============================================================================
*/
SHORT UifAC29EnterPLU2(KEYMSG *pKeyMsg) 
{
	TCHAR           auchPLUNumber[PLU_MAX_DIGIT+1+1+1] = {0};
	UCHAR			uchMod;
    SHORT           sError;                    
    MAINTSETPAGE    SetPageNo = {0};
    MAINTPLU        MaintPLU = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC29EnterPLU2);       /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);
        MaintDisp(AC_PLUSALE_READ_RPT,              /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                  MaintGetPage(),                   /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  uchUifACRptType,                  /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLUSALE_READ_RPT;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_MENU_SHIFT:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SetPageNo.uchStatus = 0;                                       
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
                SetPageNo.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                SetPageNo.uchPageNumber = (UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;                   /* Set Major Class */
            SetPageNo.uchACNumber = AC_PLUSALE_READ_RPT;                    /* Set Major Class */
            SetPageNo.uchPTDType = uchUifACDataType;
            SetPageNo.uchRptType = uchUifACRptType;
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL   *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                                /* Shift Current Func. to Active Func. */ 
            }
            return(sError);

        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            SetPageNo.uchMajorClass = CLASS_MAINTSETPAGE;               /* Set Major Class */
            SetPageNo.uchACNumber = AC_PLUSALE_READ_RPT;                
            SetPageNo.uchPTDType = uchUifACDataType;
            SetPageNo.uchRptType = uchUifACRptType;
            SetPageNo.uchLeadAddr = LDT_NUM_ADR;
            SetPageNo.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;      /* set page number */
            SetPageNo.uchStatus = 0;                                    /* Set W/ Amount Status */
            if ((sError = MaintSetPage(&SetPageNo)) == SUCCESS) {
				FSCTBL   *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                UieReject();                                            /* Shift Current Func. to Active Func. */
            }
            return(sError);

        case FSC_SCANNER:                                   /* Saratoga */
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }
            _tcsncpy(auchPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen);
            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);  /* Saratoga */
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);  /* Saratoga */
            }
            if (sError == SUCCESS) {          
                UieAccept();
            }
            return(sError);

        case FSC_E_VERSION:                         /* Saratoga */
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                return(LDT_KEYOVER_ADR);
            }
            MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
            MaintPLU.uchMinorClass = CLASS_PARAPLU_SET_EV;     /* Set Minor Class */
            MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
            MaintPLU.usACNumber = AC_PLUSALE_READ_RPT;
            MaintPLU.uchEVersion = 1;               /* Set E-Version */
            return(MaintPLUModifier(&MaintPLU));

        case FSC_AC:
            /* Check Digit and if nothing entered execute finalize procedure */
            if (pKeyMsg->SEL.INPUT.uchLen == 0) {                               /* W/o Amount */
                RptPLUDayRead(CLASS_TTLIGNORE, RPT_FIN_READ, 0, 0, auchPLUNumber);        /* Execute Finalize Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
//            } else {
//                return(LDT_SEQERR_ADR);    allow use of AC or Action Code key to enter a PLU number
            }
        case FSC_PLU:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14)) {                  
                return(LDT_KEYOVER_ADR);
            }
            _tcscpy(auchPLUNumber, pKeyMsg->SEL.INPUT.aszKey);
            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);  /* Saratoga */
            } else {                                                                /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);  /* Saratoga */
            }
            if (sError == SUCCESS) {          
                UieAccept();                                    /* Return to UifAC29EnterRptType() */
            }
            return(sError);

        case FSC_KEYED_PLU:
            /* Check W/O Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
                return(LDT_KEYOVER_ADR);
            }

            if ((sError = MaintGetPLUNo(pKeyMsg->SEL.INPUT.uchMinor, 
                auchPLUNumber, &uchMod)) != SUCCESS) {
                return(sError);
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                        /* Daily Report Case */
                sError = RptPLUDayRead(CLASS_TTLCURDAY, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            } else {                                                        /* PTD Report Case */
                sError = RptPLUPTDRead(CLASS_TTLCURPTD, RPT_PLUIND_READ, 0, 0, auchPLUNumber);
            }
            if (sError == SUCCESS) {
                UieAccept();                                        /* Return to UifACEnterRptType() */
            }
            return(sError);

        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;
        }

    case UIM_CANCEL:
        MaintPLU.uchMajorClass = CLASS_PARAPLU; /* Set Major Class */
        MaintPLU.uchMinorClass = CLASS_PARAPLU_RESET_EV;     /* Set Minor Class */
        MaintPLU.uchStatus = 0;                 /* Set W/ Amount Status */
        MaintPLU.usACNumber = AC_PLUSALE_READ_RPT;    /* Set A/C Number */
        MaintPLU.uchEVersion = 0;               /* Set E-Version */
        MaintPLUModifier(&MaintPLU);
        /* break */
    
    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/**** End of File ****/