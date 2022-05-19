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
* Title       : Manual Alternative Kitchen Printer Assignment Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC6.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC6Function()        : A/C No.6 Function Entry Mode
*               UifAC6EnterAddr()       : A/C No.6 Enter Address Mode
*               UifAC6EnterData1()      : A/C No.6 Enter Data1 Mode
*               UifAC6EnterData2()      : A/C No.6 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-29-92:00.00.01    :K.You      : initial                                   
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
#include <pif.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifAC6Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.6 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC6Init[] = {{UifAC6EnterAddr, CID_AC6ENTERADDR},
                                   {UifAC6EnterData1, CID_AC6ENTERDATA1},
                                   {NULL,             0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC6Accept[] = {{UifAC6EnterAddr, CID_AC6ENTERADDR},
                                     {UifAC6EnterData2, CID_AC6ENTERDATA2},
                                     {NULL,             0                 }};


SHORT UifAC6Function(KEYMSG *pKeyMsg) 
{


    PARAALTKITCH    ParaAltKith;   


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC6Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaAltKith.uchStatus = 0;                                  /* Set W/ Amount Status */
        ParaAltKith.uchSrcPtr = 1;                                  /* Set Address 1 */
        ParaAltKith.uchMajorClass = CLASS_PARAMANUALTKITCH;         /* Set Major Class */

        MaintManuAltKitchRead(&ParaAltKith);                       
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC6Accept);                               /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC6EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.6 Enter Address Mode 
*===============================================================================
*/

SHORT UifAC6EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAALTKITCH    ParaAltKith;
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                  /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaAltKith.uchStatus = 0;                                  /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaAltKith.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaAltKith.uchSrcPtr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaAltKith.uchMajorClass = CLASS_PARAMANUALTKITCH;         /* Set Major Class */
            if ((sError = MaintManuAltKitchRead(&ParaAltKith)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC6Function() */
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
**  Synopsis: SHORT UifAC6EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.6 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC6EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAALTKITCH    ParaAltKith;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                       /* W/o Amount Case */
                MaintManuAltKitchReport();                                          /* Execute Report Procedure */
                UieExit(aACEnter);                                                  /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                                /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {                       /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaAltKith.uchMajorClass = CLASS_PARACTLTBLMENU;                   Set Major Class */
                ParaAltKith.uchStatus = 0;
                ParaAltKith.uchDesPtr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintManuAltKitchWrite(&ParaAltKith)) == SUCCESS) {   
                    UieAccept();                                                /* Return to UifAC6Function() */
                }                                                               
            }
            return(sError);

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC6EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.6 Enter Data2 Mode 
*===============================================================================
*/
    

SHORT UifAC6EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAALTKITCH    ParaAltKith;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintManuAltKitchFin();                                     /* Send Manual Alt. Kitch. Prt. */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaAltKith.uchMajorClass = CLASS_PARACTLTBLMENU;                   Set Major Class */
                ParaAltKith.uchStatus = 0;  /* W/ amount status */
                ParaAltKith.uchDesPtr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintManuAltKitchWrite(&ParaAltKith)) == SUCCESS) {   
                    UieAccept();                                            /* Return to UifAC6Function() */
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

