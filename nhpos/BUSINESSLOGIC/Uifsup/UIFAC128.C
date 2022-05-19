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
* Program Name: UIFAC128.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC128Function()       : A/C No.128 Function Entry Mode
*               UifAC128EnterAddr()      : A/C No.128 Enter Address Mode
*               UifAC128EnterData1()     : A/C No.128 Enter Data1 Mode
*               UifAC128EnterData2()     : A/C No.128 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Mar-08-93:00.00.01    :M.Ozawa    : initial                                   
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

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "uifsup.h"
#include "uifpgequ.h"
#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC128Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.128 Function Entry Mode  
*===============================================================================
*/
/* Define Next Function at UIM_INIT */
UIMENU FARCONST aChildAC128Init[] = {{UifAC128EnterAddr, CID_AC128ENTERADDR},
                                    {UifAC128EnterData1, CID_AC128ENTERDATA1},
                                    {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED */
UIMENU FARCONST aChildAC128Accept[] = {{UifAC128EnterAddr, CID_AC128ENTERADDR},
                                      {UifAC128EnterData2, CID_AC128ENTERDATA2},
                                      {NULL,              0                 }};

SHORT UifAC128Function(KEYMSG *pKeyMsg) 
{
    PARAMISCPARA    ParaMiscPara;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC128Init);                               /* Open Next Function */

        /* Display Address 1 of This Function */
        ParaMiscPara.uchStatus = 0;                                 /* Set W/ Amount Status */
        ParaMiscPara.uchAddress = MISC_FOODDENOMI_ADR;              /* Set Address 1 */
        ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;            /* Set Major Class */
        MaintMiscRead(&ParaMiscPara);                               /* Execute Read HALO Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC128Accept);                             /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC128EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.128 Enter Address Mode 
*===============================================================================
*/
SHORT UifAC128EnterAddr(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARAMISCPARA    ParaMiscPara;

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
            ParaMiscPara.uchStatus = 0;                                 /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaMiscPara.uchStatus |= MAINT_WITHOUT_DATA;           /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaMiscPara.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;            /* Set Major Class */
            if ((sError = MaintMiscRead(&ParaMiscPara)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC128Function() */
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
**  Synopsis: SHORT UifAC128EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.128 Enter Data1 Mode 
*===============================================================================
*/

SHORT UifAC128EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARAMISCPARA    ParaMiscPara;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintMiscReport();                                          /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > 7) {                        /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaMiscTbl.uchMajorClass = CLASS_PARAMISCPARA;              Set Major Class */
                ParaMiscPara.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaMiscPara.ulMiscPara = pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintMiscWrite(&ParaMiscPara)) == SUCCESS) {  /* Read Function Completed */
                    UieAccept();                                            /* Return to UifAC128Function() */
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
**  Synopsis: SHORT UifAC128EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.128 Enter Data2 Mode 
*===============================================================================
*/
SHORT UifAC128EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    PARAMISCPARA    ParaMiscPara;

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

                if (pKeyMsg->SEL.INPUT.uchLen > 7) {                        /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                /* ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;            Set Major Class */
                ParaMiscPara.uchStatus = 0;                                 /* Set W/ Amount Status */
                ParaMiscPara.ulMiscPara = pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintMiscWrite(&ParaMiscPara)) == SUCCESS) {          
                    UieAccept();                                            /* Return to UifAC128Function() */
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

/* --- End of Source File --- */