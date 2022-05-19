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
* Title       : Tax/Service/Reciprocal Rate Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC127.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC127Function()      : A/C No.127 Function Entry Mode
*               UifAC127EnterAddr()     : A/C No.127 Enter Address Mode
*               UifAC127EnterData1()    : A/C No.127 Enter Data1 Mode
*               UifAC127EnterData2()    : A/C No.127 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
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
#include <uifsup.h>
#include <uifpgequ.h>
#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT     UifAC127Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.127 Function Entry Mode  
*===============================================================================
*/
/* Define Next Function at UIM_INIT */
UIMENU FARCONST aChildAC127Init[] = {{UifAC127EnterAddr, CID_AC127ENTERADDR},
                                     {UifAC127EnterData1, CID_AC127ENTERDATA1},
                                     {NULL,               0                 }};

/* Define Next Function at UIM_ACCEPTED */
UIMENU FARCONST aChildAC127Accept[] = {{UifAC127EnterAddr, CID_AC127ENTERADDR},
                                       {UifAC127EnterData2, CID_AC127ENTERDATA2},
                                       {NULL,               0                 }};

SHORT   UifAC127Function(KEYMSG *pKeyMsg)
{
    PARATAXRATETBL      ParaTaxRate;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC127Init);                           /* Open Next Function */

        /* Display Address 1 of This Function */
        ParaTaxRate.uchStatus = 0;                              /* Set W/ Amount Status */
        ParaTaxRate.uchAddress = 1;                             /* Set Address 1 */
        ParaTaxRate.uchMajorClass = CLASS_PARATAXRATETBL;       /* Set Major Class */
        MaintTaxRateRead(&ParaTaxRate);                             
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC127Accept);                         /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }

    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC127EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.127 Enter Address Mode 
*===============================================================================
*/
SHORT   UifAC127EnterAddr(KEYMSG *pKeyMsg)
{
    SHORT               sError;
    PARATAXRATETBL      ParaTaxRate;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                  /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaTaxRate.uchStatus = 0;                                  /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaTaxRate.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaTaxRate.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaTaxRate.uchMajorClass = CLASS_PARATAXRATETBL;           /* Set Major Class */
            if ((sError = MaintTaxRateRead(&ParaTaxRate)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC127Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC127EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.127 Enter Data1 Mode 
*===============================================================================
*/
SHORT   UifAC127EnterData1(KEYMSG *pKeyMsg)
{
    SHORT           sError;
    PARATAXRATETBL  ParaTaxRate;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                        /* Register Key Sequence */
        return(SUCCESS);                                                        

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount Case */
                MaintTaxRateReport();                                           /* Execute Report Procedure */
                UieExit(aACEnter);                                              /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                            /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT6) {                   /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaTaxRate.uchMajorClass = CLASS_PARATAXRATETBL;               Set Major Class */
                ParaTaxRate.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaTaxRate.ulTaxRate = ( ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintTaxRateWrite(&ParaTaxRate)) == SUCCESS) {    /* Read Function Completed */
                    UieAccept();                                                /* Return to UifAC127Function() */
                }
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC127EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.127 Enter Data2 Mode 
*===============================================================================
*/
SHORT   UifAC127EnterData2(KEYMSG *pKeyMsg)
{
    SHORT               sError;
    PARATAXRATETBL      ParaTaxRate;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT6) {                       /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaTaxRate.uchMajorClass = CLASS_PARATAXRATETBL;                   Set Major Class */
                ParaTaxRate.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaTaxRate.ulTaxRate = ( ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintTaxRateWrite(&ParaTaxRate)) == SUCCESS) {    
                    UieAccept();                                                /* Return to UifAC127Function() */
                }
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/* --- End of Source --- */
