/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Misc. Parameter Set/Read Module
* Category    : User Interface For Supervisor, NCR 2170 US GP Application         
* Program Name: UIFAC208.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC208Function()       : A/C No.208 Function Entry Mode
*               UifAC208EnterAddr()      : A/C No.208 Enter Address Mode
*               UifAC208EnterData1()     : A/C No.208 Enter Data1 Mode
*               UifAC208EnterData2()     : A/C No.208 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* FEB-23-94:00.00.01    :M.INOUE    : initial                                   
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
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifAC208Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.208 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC208Init[] = {{UifAC208EnterAddr, CID_AC208ENTERADDR},
                                    {UifAC208EnterData1, CID_AC208ENTERDATA1},
                                    {NULL,              0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC208Accept[] = {{UifAC208EnterAddr, CID_AC208ENTERADDR},
                                      {UifAC208EnterData2, CID_AC208ENTERDATA2},
                                      {NULL,              0                 }};


SHORT UifAC208Function(KEYMSG *pKeyMsg) 
{


    PARABOUNDAGE    ParaBoundAge;   
                    

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC208Init);                               /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaBoundAge.uchStatus = 0;                                 /* Set W/ Amount Status */
        ParaBoundAge.uchAddress = 1;                                /* Set Address 1 */
        ParaBoundAge.uchMajorClass = CLASS_PARABOUNDAGE;            /* Set Major Class */

        MaintBoundAgeRead(&ParaBoundAge);                           /* Execute Read HALO Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC208Accept);                             /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC208EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.208 Enter Address Mode 
*===============================================================================
*/

SHORT UifAC208EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT            sError;
    PARABOUNDAGE     ParaBoundAge;
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                          /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaBoundAge.uchStatus = 0;                                 /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaBoundAge.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaBoundAge.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaBoundAge.uchMajorClass = CLASS_PARABOUNDAGE;            /* Set Major Class */
            if ((sError = MaintBoundAgeRead(&ParaBoundAge)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC208Function() */
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
**  Synopsis: SHORT UifAC208EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.208 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC208EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARABOUNDAGE    ParaBoundAge;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintBoundAgeReport();                                          /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                if (pKeyMsg->SEL.INPUT.lData > 0xFF) {                     /* Over ushort */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaMiscTbl.uchMajorClass = CLASS_PARABOUNDAGE;              Set Major Class */
                ParaBoundAge.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaBoundAge.uchAgePara = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintBoundAgeWrite(&ParaBoundAge)) == SUCCESS) {  /* Read Function Completed */
                    UieAccept();                                            /* Return to UifAC208Function() */
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
**  Synopsis: SHORT UifAC208EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.208 Enter Data2 Mode 
*===============================================================================
*/
    

SHORT UifAC208EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARABOUNDAGE    ParaBoundAge;
                   


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

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                if (pKeyMsg->SEL.INPUT.lData > 0xFF) {                     /* Over ushort */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaBoundAge.uchMajorClass = CLASS_PARAAGEPARA;            Set Major Class */
                ParaBoundAge.uchStatus = 0;                                 /* Set W/ Amount Status */
                ParaBoundAge.uchAgePara = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintBoundAgeWrite(&ParaBoundAge)) == SUCCESS) {          
                    UieAccept();                                            /* Return to UifAC208Function() */
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
/******* end of uifac208.c *******/
