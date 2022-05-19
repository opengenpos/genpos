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
* Title       : Set Date and Time Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC17.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC17Function()       : A/C No.17 Function Entry Mode
*               UifAC17EnterAddr()      : A/C No.17 Enter Address Mode
*               UifAC17EnterData1()     : A/C No.17 Enter Data1 Mode
*               UifAC17EnterData2()     : A/C No.17 Enter Data2 Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-01-92:00.00.01    :K.You      : initial                                   
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
**  Synopsis: SHORT UifAC17Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.17 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU aChildAC17Init[] = {{UifAC17EnterAddr, CID_AC17ENTERADDR},
                                    {UifAC17EnterData1, CID_AC17ENTERDATA1},
                                    {NULL,             0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU aChildAC17Accept[] = {{UifAC17EnterAddr, CID_AC17ENTERADDR},
                                      {UifAC17EnterData2, CID_AC17ENTERDATA2},
                                      {NULL,             0                 }};


SHORT UifAC17Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC17Init);                                /* Open Next Function */
        MaintTODInit();                                             /* Initialize Maintenance Work */      
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC17Accept);                              /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC17EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.17 Enter Address Mode 
*===============================================================================
*/
SHORT UifAC17EnterAddr(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTTOD    MaintTOD = { 0 };
                    
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

            MaintTOD.uchStatus = 0;                                     /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                MaintTOD.uchStatus |= MAINT_WITHOUT_DATA;               /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                MaintTOD.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            MaintTOD.uchMajorClass = CLASS_MAINTTOD;                    /* Set Major Class */
            MaintTOD.uchMinorClass = CLASS_MAINTTOD_READ;               /* Set Minor Class */
            if ((sError = MaintTODEdit(&MaintTOD)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC17Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
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
**  Synopsis: SHORT UifAC17EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.17 Enter Data1 Mode 
*===============================================================================
*/
SHORT UifAC17EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTTOD    MaintTOD = { 0 };
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                MaintTODWrite();
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT6) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTOD.uchStatus = 0;                                         /* Set W/ Amount Status */
            MaintTOD.uchMajorClass = CLASS_MAINTTOD;                        /* Set Major Class */
            MaintTOD.uchMinorClass = CLASS_MAINTTOD_WRITE;                  /* Set Minor Class */
            MaintTOD.lTODData = pKeyMsg->SEL.INPUT.lData;
            MaintTOD.uchStrLen = pKeyMsg->SEL.INPUT.uchLen;
            if ((sError = MaintTODEdit(&MaintTOD)) == SUCCESS) {         
                UieAccept();                                                /* Return to UifAC17Function() */
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
**  Synopsis: SHORT UifAC17EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.17 Enter Data2 Mode 
*===============================================================================
*/
SHORT UifAC17EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	MAINTTOD    MaintTOD = { 0 };
                   
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                MaintTODWrite();
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT6) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTOD.uchStatus = 0;                                         /* Set W/ Amount Status */
            MaintTOD.uchMajorClass = CLASS_MAINTTOD;                        /* Set Major Class */
            MaintTOD.uchMinorClass = CLASS_MAINTTOD_WRITE;                  /* Set Minor Class */
            MaintTOD.lTODData = pKeyMsg->SEL.INPUT.lData;
            MaintTOD.uchStrLen = pKeyMsg->SEL.INPUT.uchLen;
            if ((sError = MaintTODEdit(&MaintTOD)) == SUCCESS) {         
                UieAccept();                                                /* Return to UifAC17Function() */
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

