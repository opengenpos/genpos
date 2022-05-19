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
* Title       : Adjust Tone Volume Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC169.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC169Function()      : A/C No.169 Function Entry Mode
*               UifAC169EnterAddr()     : A/C No.169 Enter Address Mode
*               UifAC169EnterData1()    : A/C No.169 Enter Data1 Mode
*               UifAC169EnterData2()    : A/C No.169 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-21-92:00.00.01    :K.You      : initial                                   
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
**  Synopsis: SHORT UifAC169Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.169 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC169Init[] = {{UifAC169EnterAddr, CID_AC169ENTERADDR},
                                    {UifAC169EnterData1, CID_AC169ENTERDATA1},
                                    {NULL,               0                  }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC169Accept[] = {{UifAC169EnterAddr, CID_AC169ENTERADDR},
                                      {UifAC169EnterData1, CID_AC169ENTERDATA1},
                                      {NULL,               0                  }};


SHORT UifAC169Function(KEYMSG *pKeyMsg) 
{


    PARATONECTL     ParaTone;   
                    

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC169Init);                           /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaTone.uchStatus = 0;                                 /* Set W/ Amount Status */
        ParaTone.uchAddress = 1;                                /* Set Address 1 */
        ParaTone.uchMajorClass = CLASS_PARATONECTL;             /* Set Major Class */

        MaintToneRead(&ParaTone);                               /* Execute Read HALO Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC169Accept);                         /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC169EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.169 Enter Address Mode 
*===============================================================================
*/

SHORT UifAC169EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATONECTL     ParaTone;
                        


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                          /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaTone.uchStatus = 0;                                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaTone.uchStatus |= MAINT_WITHOUT_DATA;               /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaTone.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaTone.uchMajorClass = CLASS_PARATONECTL;                 /* Set Major Class */
            if ((sError = MaintToneRead(&ParaTone)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC169Function() */
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
**  Synopsis: SHORT UifAC169EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.169 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC169EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARATONECTL     ParaTone;
                   


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

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaTone.uchMajorClass = CLASS_PARATONECTL;              Set Major Class */
                ParaTone.uchStatus = 0;                                     /* Set W/ Amount Status */
                ParaTone.uchToneCtl = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintToneWrite(&ParaTone)) == SUCCESS) {      /* Read Function Completed */
                    UieAccept();                                            /* Return to UifAC169Function() */
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


