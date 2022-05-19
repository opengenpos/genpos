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
* Title       : Set Sales Promotion Message Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application           
* Program Name: UIFAC88.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC88Function()   : A/C No.88 Entry Mode
*               UifAC88EnterALPHA() : A/C No.88 Enter Mnemonics Data Mode
*               UifAC88IssueRpt()   : A/C No.88 Issue Report
*               UifAC88EnterAddr()  : A/C No.88 Enter Address Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
*          :            :           :
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
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>                       
#include <cvt.h>                            /* Key Code Table */

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT UifAC88Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.88 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC88Init[] = {{UifACShift, CID_ACSHIFT},
                                    {UifAC88EnterALPHA, CID_AC88ENTERALPHA},
                                    {UifACChangeKB1, CID_ACCHANGEKB1},
                                    {NULL,           0            }};


/* Define Next Function at UIM_ACCEPTED from CID_ACCHANGEKB1 */

UIMENU FARCONST aChildAC88Accept1[] = {{UifAC88IssueRpt, CID_AC88ISSUERPT},
                                       {UifAC88EnterAddr, CID_AC88ENTERADDR},
                                       {NULL,             0             }};


/* Define Next Function at UIM_ACCEPTED from CID_ACCHANGEKB2 */

UIMENU FARCONST aChildAC88Accept2[] = {{UifAC88EnterAddr, CID_AC88ENTERADDR},
                                       {NULL,             0             }};


/* Define Next Function at UIM_ACCEPTED from CID_AC88ENTERALPHA,CID_AC88ENTERADDR */

UIMENU FARCONST aChildAC88Accept3[] = {{UifACShift, CID_ACSHIFT},
                                       {UifAC88EnterALPHA, CID_AC88ENTERALPHA},
                                       {UifACChangeKB2, CID_ACCHANGEKB2},
                                       {NULL,           0            }};



SHORT UifAC88Function(KEYMSG *pKeyMsg) 
{

    PARAPROMOTION   ProSalesData;


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        ProSalesData.uchStatus = 0;                             /* Set W/ Amount Status */
        ProSalesData.uchAddress = 1;                            /* Set Address 1 */
        ProSalesData.uchMajorClass = CLASS_PARAPROMOTION;       /* Set Major Class */
        MaintPromotionRead(&ProSalesData);                      /* Execute Read Transaction Mnemonics Procedure */

        UieNextMenu(aChildAC88Init);                            /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_ACCHANGEKB1:
            UieNextMenu(aChildAC88Accept1);                     /* Open Next Function */
            break;

        case CID_ACCHANGEKB2:
            UieNextMenu(aChildAC88Accept2);                     /* Open Next Function */
            break;

        default:                                                /* CID_AC88ENTERALPHA */
            UieNextMenu(aChildAC88Accept3);                     /* CID_AC88ENTERADDR */
            break;                                              
        }
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC88EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.88 Enter Mnemonics Data Mode 
*===============================================================================
*/
    


SHORT UifAC88EnterALPHA(KEYMSG *pKeyMsg) 
{

    SHORT           sError;
    PARAPROMOTION   ProSalesData;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P1);                            /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, PARA_PROMOTION_LEN);          /* Define Display Format and Max. Input Data */


        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            ProSalesData.uchStatus = 0;                           /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                ProSalesData.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                memset(ProSalesData.aszProSales,                    /* Full NULL to Own Buffer */
                       '\0',
                       (PARA_PROMOTION_LEN+1) * sizeof(TCHAR));

                _tcsncpy(ProSalesData.aszProSales,                    /* Copy Input Mnemonics to Own Buffer */
                       pKeyMsg->SEL.INPUT.aszKey,
                       pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(ProSalesData.aszProSales,                    /* Copy Input Mnemonics to Own Buffer */
                //       pKeyMsg->SEL.INPUT.aszKey,
                //       pKeyMsg->SEL.INPUT.uchLen);
            }

            ProSalesData.uchMajorClass = CLASS_PARAPROMOTION;       /* Set Major Class */

            if ((sError = MaintPromotionWrite(&ProSalesData)) == SUCCESS) {    
                UieAccept();                                        /* Return to UifAC88Function() */
            }
            return(sError);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC88IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error 
*               
**  Description: Program No.88 Enter Address Mode 
*===============================================================================
*/
    

SHORT UifAC88IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
            case FSC_AC:
                MaintPromotionReport();                 /* Execute Report Procedure */
                UieExit(aACEnter);                      /* Return to UifACEnter() */
                return(SUCCESS);
            
            /* default:
                  break; */

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC88EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error 
*               
**  Description: Program No.88 Enter Address Mode 
*===============================================================================
*/

/* Define Key Sequence */

UISEQ FARCONST aszSeqAC88EnterAddr[] = {FSC_NUM_TYPE, FSC_AC, 0};

    

SHORT UifAC88EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAPROMOTION   ProSalesData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC88EnterAddr);               /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);

        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ProSalesData.uchStatus = 0;                     /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ProSalesData.uchStatus |= MAINT_WITHOUT_DATA;           /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ProSalesData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ProSalesData.uchMajorClass = CLASS_PARAPROMOTION;           /* Set Major Class */

            if ((sError = MaintPromotionRead(&ProSalesData)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC88Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}




    
    
    
