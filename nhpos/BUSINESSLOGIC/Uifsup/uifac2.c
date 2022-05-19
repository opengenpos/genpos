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
* Title       : Guest Check File Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC2.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC2Function()        : A/C No.2 Function Entry Mode
*               UifAC2EnterType()       : A/C No.2 Enter Type Mode
*               UifAC2EnterData1()      : A/C No.2 Enter Data 1 Mode
*               UifAC2EnterData2()      : A/C No.2 Enter Data 2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-08-92:00.00.01    :K.You      : initial                                   
* Jul-26-95:03.00.03    :M.Ozawa    : Modify for Report on LCD
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
**  Synopsis: SHORT UifAC2Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.2 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC2Init[] = {{UifAC2EnterType, CID_AC2ENTERTYPE},
                                   {NULL,            0               }};



SHORT UifAC2Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC2Init);                             /* Open Next Function */
        return(SUCCESS);                                        

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC2EnterType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.2 Enter Type Mode 
*===============================================================================
*/
    
/* Define Next Function */

UIMENU FARCONST aChildAC2EnterType1[] = {{UifAC2EnterData1, CID_AC2ENTERDATA1},
                                         {NULL,             0                }};

UIMENU FARCONST aChildAC2EnterType2[] = {{UifAC2EnterData2, CID_AC2ENTERDATA2},
                                         {NULL,             0                }};



SHORT UifAC2EnterType(KEYMSG *pKeyMsg) 
{
    
    SHORT   sError;                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        UifACPGMLDDispCom(TypeSelectReport1);

        MaintDisp(AC_GCFL_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOTYPE_ADR);                /* "Request Report Type Entry" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {      
        case FSC_AC:
            /* Check W/ Amount */

            if (!(pKeyMsg->SEL.INPUT.uchLen)) {             /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
        
            /* Check Input Data */

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {             /* All Read Type */

                uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;     /* Set Report Type to Own Save Area */

                /* Clear Lead Through */

                UifACPGMLDClear();

                MaintDisp(AC_GCFL_READ_RPT,                             /* A/C Number */
                          0,                                            /* Descriptor */
                          0,                                            /* Page Number */
                          0,                                            /* PTD Type */
                          ( UCHAR)RPT_TYPE_ALL,                         /* Report Type */
                          0,                                            /* Reset Type */
                          0L,                                           /* Amount Data */
                          0);                                           /* Disable Lead Through */

                sError = RptGuestRead(RPT_ALL_READ, 0);
                UieExit(aACEnter);                                      /* Return to UifACEnter() */
                if (sError == RPT_ABORTED) {                            /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_IND) {      /* Individual Read Type */
                UieNextMenu(aChildAC2EnterType1);                       /* Open Next Function */
                return(SUCCESS);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_WAIIND) {   /* Individual Read by Waiter Type */
                UieNextMenu(aChildAC2EnterType2);                       /* Open Next Function */
                return(SUCCESS);
            } else {
                return(LDT_KEYOVER_ADR);                                /* Return Wrong Error */
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }                                                       
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC2EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.2 Enter Data 1 Mode 
*===============================================================================
*/
    


SHORT UifAC2EnterData1(KEYMSG *pKeyMsg) 
{

    SHORT   sError;
    USHORT  usGCNumber;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        uchUifACRptType = ( UCHAR)RPT_TYPE_IND;     /* Set Report Type to Own Save Area */

        UifACPGMLDClear();

        MaintDisp(AC_GCFL_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  ( UCHAR)RPT_TYPE_IND,             /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT7) {               /* Over Digit, R3.3 */
                return(LDT_KEYOVER_ADR);
            }

            /* Check GC Number and Execute */

            if ((sError = MaintChkGCNo(( ULONG)pKeyMsg->SEL.INPUT.lData, &usGCNumber)) != SUCCESS) {
                return(sError);
            }

            /* Clear Lead Through */

            UifACPGMLDClear();

            MaintDisp(AC_GCFL_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      ( UCHAR)RPT_TYPE_IND,             /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            sError = RptGuestRead(RPT_IND_READ, usGCNumber);
            UieExit(aACEnter);                                          /* Return to UifACEnter() */
            if (sError == RPT_ABORTED) {                                /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC2EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.2 Enter Data 2 Mode 
*===============================================================================
*/
    


SHORT UifAC2EnterData2(KEYMSG *pKeyMsg) 
{
                            
    SHORT   sError;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        uchUifACRptType = ( UCHAR)RPT_TYPE_WAIIND;     /* Set Report Type to Own Save Area */

        UifACPGMLDClear();

        MaintDisp(AC_GCFL_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  ( UCHAR)RPT_TYPE_WAIIND,          /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_WTNO_ADR);                    /* "Enter Waiter #" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        /* Check W/ Amount */

        if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
            return(LDT_SEQERR_ADR);
        }
        
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check 0 Data and Digit */

            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)) {              
                return(LDT_KEYOVER_ADR);
            }
            /* Clear Lead Through */

            UifACPGMLDClear();

            MaintDisp(AC_GCFL_READ_RPT,                 /* A/C Number */
                      0,                                /* Descriptor */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      ( UCHAR)RPT_TYPE_WAIIND,          /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      0);                               /* Disable Lead Through */

            sError = RptGuestRead(RPT_WAITER_READ, ( USHORT)pKeyMsg->SEL.INPUT.lData);
            UieExit(aACEnter);                                          /* Return to UifACEnter() */
            if (sError == RPT_ABORTED) {                                /* Aborted by User */
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

/**** End of File ****/
