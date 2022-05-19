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
* Title       : ETK File Maintenance Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC153.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC153Function()       : A/C 153 Function Entry Mode
*               UifAC153EnterEmpNo()     : A/C 153 Enter Employee No./Exit Function Mode
*               UifAC153EnterData()      : A/C 153 Enter Data Mode
*               UifAC153CancelFunction() : A/C 153 Cancel Function
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Sep-30-93: 02.00.01   : J.IKEDA   : Initial                                     
* Jul-28-95: 03.00.03   : M.Ozawa   : Modify for LCD display
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
#include <pif.h>
#include <fsc.h>
#include <csetk.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifpgequ.h>
#include <uifsup.h>
#include <cvt.h>
#include <mldmenu.h>
#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC153EnterData[] = {FSC_NUM_TYPE, FSC_TIME_IN, FSC_TIME_OUT, FSC_AC, 0};

/*
*=============================================================================
**     Synopsis: SHORT UifAC153Function(KEYMSG *pKeyMsg) 
*
*         Input: *pKeyMsg        : Pointer to Structure for Key Message  
*        Output: nothing
*
*        Return: SUCCESS         : Successful 
*                LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.153 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT (Initial, from CID_AC153ENTERDATA) */

UIMENU FARCONST aChildAC153Init[] = {{UifAC153EnterEmpNo,     CID_AC153ENTEREMPNO},
                                     {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                     {NULL,                   0                      }};


SHORT UifAC153Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDClear();                              /* Clear MLD */

        UieNextMenu(aChildAC153Init);           /* open next function */                       
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));          /* execute default procedure */
    }
}

/*
*=============================================================================
**     Synopsis: SHORT UifAC153EnterEmpNo(KEYMSG *pKeyMsg) 
*
*         Input: *pKeyMsg        : Pointer to Structure for Key Message  
*        Output: nothing                         
*        Return: SUCCESS         : Successful
*                LDT_SEQERR_ADR  : Sequence Error
*                LDT_KEYOVER_ADR : Wrong Data Error
*                LDT_NTINFL_ADR  : Not In File         (convert ETK_NOT_IN_FILE)
*                LDT_PROHBT_ADR  : Prohibit Operation  (convert ETK_FILE_NOT_FOUND)
*
**  Description: A/C No.153 Enter Employee Number/Exit Function Mode 
*===============================================================================
*/

/* Define Next Function at UIM_INPUT and at UIM_ACCEPTED from CID_AC153ENTERDATA */

UIMENU FARCONST aChildAC153NxtFunc[] = {{UifAC153EnterData,      CID_AC153ENTERDATA},
                                        {UifAC153CancelFunction, CID_AC153CANCELFUNCTION},
                                        {NULL,                   0                      }};

SHORT UifAC153EnterEmpNo(KEYMSG *pKeyMsg) 
{

    SHORT      sError;
    PARAETKFL  ParaEtkFl;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:

        MaintDisp(AC_ETK_MAINT,                 /* A/C number */
                  CLASS_MAINTDSPCTL_10NPG,      /* set minor class for 10N10D */
                  0,                            /* page number */
                  0,                            /* PTD type */
                  0,                            /* report type */
                  0,                            /* reset type */
                  0L,                           /* amount data */
                  LDT_NUM_ADR);                 /* "Number Entry" lead through address */

        UieOpenSequence(aszSeqACFSC_AC);        /* register key sequence */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC153NxtFunc);         /* open next function */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:                            /* Employee No. entry */

            /* check data length */

            if (!pKeyMsg->SEL.INPUT.uchLen) {   /* without amount */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);    /* execute finalize procedure */
                RptETKUnLock();                 /* unlock ETK file */
                UieExit(aACEnter);              /* return to UifACEnter() */
                return(SUCCESS);
            } 

            /* check digits */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT9) {   /* over digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaEtkFl.uchMajorClass = CLASS_PARAETKFILE;    /* set major class */
            ParaEtkFl.ulEmployeeNo = (ULONG)pKeyMsg->SEL.INPUT.lData; /* set employee number */

            /* check error status */

            if ((sError = MaintETKFileRead(&ParaEtkFl)) == SUCCESS) {
                UieNextMenu(aChildAC153NxtFunc);
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));          /* execute default procedure */
    }
}

/*
*=============================================================================
**     Synopsis: SHORT UifAC153EnterData(KEYMSG *pKeyMsg) 
*
*         Input: *pKeyMsg        : Pointer to Structure for Key Message  
*        Output: nothing                         
*        Return: SUCCESS         : Successful
*                LDT_SEQERR_ADR  : Sequence Error
*                LDT_KEYOVER_ADR : Wrong Data Error
*                LDT_NTINFL_ADR  : Not In File         (convert ETK_NOT_IN_FILE)
*                LDT_PROHBT_ADR  : Prohibit Operation  (convert ETK_FILE_NOT_FOUND)
*
**  Description: A/C No.153 Enter Data Mode 
*===============================================================================
*/

/* Define Next Function at UIM_INPUT */

UIMENU FARCONST aParentAC153Func[] = {{UifAC153Function, CID_AC153FUNCTION},
                                      {NULL,             0                }};


SHORT UifAC153EnterData(KEYMSG *pKeyMsg)
{

    SHORT      sError;
    PARAETKFL  ParaEtkFl;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC153EnterData);  /* register key sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:                      /* Block No. entry */

            /* check digits */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* over digit */
                return(LDT_KEYOVER_ADR);
            }                                               
            ParaEtkFl.uchStatus = 0;            /* set W/ amount status */

            /* check length */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o amount case */
                ParaEtkFl.uchStatus |= MAINT_WITHOUT_DATA;  /* set W/o amount status */
            } else {                                        /* W/ amount case */
                ParaEtkFl.usBlockNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;  /* set Block No. */
            }
            ParaEtkFl.uchMajorClass = CLASS_PARAETKFILE;    /* set major class */
            ParaEtkFl.uchMinorClass = CLASS_PARAETKFILE_NUMTYPE;    /* set minor class */

            /* check error status */

            if ((sError = MaintETKFileWrite(&ParaEtkFl)) == SUCCESS) {
                UieAccept();                    /* return to UifAC153EnterEmpNo() */            
            }
            return(sError);                     

        case FSC_TIME_IN:                       /* Date/Time-in Entry */
        
            /* check data length */

            if (!pKeyMsg->SEL.INPUT.uchLen) {   /* without amount */
                return(LDT_SEQERR_ADR);
            } 

            /* check digits */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT8) {   /* over digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaEtkFl.uchMajorClass = CLASS_PARAETKFILE;    /* set major class */
            ParaEtkFl.uchMinorClass = CLASS_PARAETKFILE_TIMEIN; /* set minor class */
            ParaEtkFl.ulDateTimeIn = ( ULONG)pKeyMsg->SEL.INPUT.lData;  /* set Date/Time-in */
            ParaEtkFl.uchStatus = 0;            /* set W/ amount status */

            /* check error status */

            if ((sError = MaintETKFileWrite(&ParaEtkFl)) == SUCCESS) {
                UieAccept();                    /* return to UifAC153EnterEmpNo() */            
            }
            return(sError);                     

        case FSC_TIME_OUT:                      /* Time-out Entry */
        
            /* check data length */

            if (!pKeyMsg->SEL.INPUT.uchLen) {   /* without amount */
                return(LDT_SEQERR_ADR);
            } 

            /* check digits */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT8) {   /* over digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaEtkFl.uchMajorClass = CLASS_PARAETKFILE;    /* set major class */
            ParaEtkFl.uchMinorClass = CLASS_PARAETKFILE_TIMEOUT; /* set minor class */
            ParaEtkFl.ulTimeOut = ( ULONG)pKeyMsg->SEL.INPUT.lData;  /* set Time-out */
            ParaEtkFl.uchStatus = 0;            /* set W/ amount status */

            /* check error status */

            if ((sError = MaintETKFileWrite(&ParaEtkFl)) == SUCCESS) {
                UieAccept();                    /* return to UifAC153EnterEmpNo() */            
            }
            return(sError);                     

        case FSC_AC:                            /* Job Code */

            /* check length */

            if (!pKeyMsg->SEL.INPUT.uchLen) {   /* W/o Amount Case */
                MaintETKFilePrint();            /* print new ETK file */
                UieExit(aParentAC153Func);      /* return to UifAC153Function() */
                return(SUCCESS);
            }

            /* check digits */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* over digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaEtkFl.uchMajorClass = CLASS_PARAETKFILE;    /* set major class */
            ParaEtkFl.uchMinorClass = CLASS_PARAETKFILE_JOBCODE;        /* set minor class */
            ParaEtkFl.uchJobCode = ( UCHAR)pKeyMsg->SEL.INPUT.lData;    /* set Job Code */
            ParaEtkFl.uchStatus = 0;            /* set W/ amount status */

            /* check error status */

            if ((sError = MaintETKFileWrite(&ParaEtkFl)) == SUCCESS) {
                UieAccept();                    /* return to UifAC153EnterEmpNo() */            
            }
            return(sError);                     

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));          /* execute default procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**     Synopsis: SHORT UifAC153CancelFunction(KEYMSG *pKeyMsg) 
*
*         Input: *pKeyMsg        : Pointer to Structure for Key Message  
*        Output: nothing                         
*        Return: SUCCESS         : Successful
*                LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 153 cancel function mode 
*===============================================================================
*/
    
SHORT UifAC153CancelFunction(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_CANCEL);    /* register key sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ amount */
                return(LDT_SEQERR_ADR);
            }
            MaintETKFilePrint();                    /* print new ETK file */
            RptETKUnLock();                         /* Unlock ETK file */
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);    /* execute finalize procedure */
            UieExit(aACEnter);                      /* return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));              /* execute default procedure */
    }
}

/***** End of File ****/
