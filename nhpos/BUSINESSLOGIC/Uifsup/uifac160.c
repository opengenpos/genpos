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
* Title       : Order Entry Table Data Set Module
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC160.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               UifAC160Function()       : A/C No.160 Function Entry Mode
*               UifAC160EnterAddr()      : A/C No.160 Enter Address Mode
*               UifAC160EnterData()      : A/C No.160 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
*    -  -95:00.00.00    :M.Waki     : initial
* Apr-14-95:03.00.00    :T.Satoh    : Adds clear MLD
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
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */




/*
*=============================================================================
**  Synopsis: SHORT UifAC160Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.160 Function Entry Mode
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC160Init[] = {{UifAC160EnterAddr, CID_AC160ENTERADDR},
                                    {NULL,              0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC160Accept[] = {{UifAC160EnterData, CID_AC160ENTERDATA},
                                      {NULL,              0                 }};


SHORT UifAC160Function(KEYMSG *pKeyMsg)
{

    switch (pKeyMsg->uchMsg) {

    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC160ENTERDATA:
            UieNextMenu(aChildAC160Init);                               /* Open Next Function */
            return(SUCCESS);

        default:
            usUifACControl = 0;                                         /* Clear Control Save Area */
            UieNextMenu(aChildAC160Init);                               /* Open Next Function */
            return(SUCCESS);
        }

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC160ENTERADDR:
            usUifACControl |= STS_OEP_ACCESS;                               /* STS_ON = !0 already existed ??*/
            UieNextMenu(aChildAC160Accept);                         /* Open Next Function */
            return(SUCCESS);
        default:
            return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
        }

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC160EnterAddr(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
**  Description: A/C No.160 Enter Address Mode
*===============================================================================
*/

SHORT UifAC160EnterAddr(KEYMSG *pKeyMsg)
{

    SHORT           sError;
    PARAOEPTBL  ParaOepTbl;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:

        UifACPGMLDClear();                              /* Clear MLD */

        /* "enter table address" display */
        MaintDisp(AC_OEP_MAINT,                         /* A/C Number */
                  0,                                    /* Set Minor Class for 10N10D */
                  0,                                    /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_TBLNUM_ADR);                         /* "Number Entry" Lead Through Address */

        UieOpenSequence(aszSeqACFSC_AC);                          /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaOepTbl.uchStatus = 0;                                   /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                if (!usUifACControl) {                                  /* */
                    MaintOepReport();                                  /* Execute Report Procedure */
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                    UieExit(aACEnter);                                  /* Return to UifACEnter() */
                    return(SUCCESS);
                } else {
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                    UieExit(aACEnter);                                  /* Return to UifACEnter() */
                    return(SUCCESS);
                }
            } else {                                                    /* W/ Amount Input Case */
                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {
                    return(LDT_KEYOVER_ADR);                            /* key input error */
                }
                if (pKeyMsg->SEL.INPUT.lData < 1L) {
                    return(LDT_KEYOVER_ADR);                            /* key input error */
                }
            }
            ParaOepTbl.uchTblNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            ParaOepTbl.uchTblAddr = 1;
            ParaOepTbl.uchMajorClass = CLASS_PARAOEPTBL;              /* Set Major Class */
            ParaOepTbl.uchStatus = MAINT_STS_TBLNUMBER;             /* Set Status Table Number In */
            if ((sError = MaintOepRead(&ParaOepTbl)) == SUCCESS) {
                UieAccept();                                            /* Return to UifAC160Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */

        }

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC160EnterData(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.160 Enter Data Mode
*===============================================================================
*/
UISEQ FARCONST aszSeqAC160EnterDataType[] = {FSC_NUM_TYPE, FSC_AC, 0};
UIMENU FARCONST aAC160Func[] = {{UifAC160Function,CID_AC160FUNCTION},
                               {NULL,      0          }};


SHORT UifAC160EnterData(KEYMSG *pKeyMsg)
{

    SHORT           sError;
    PARAOEPTBL  ParaOepTbl;



    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC160EnterDataType);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            if (pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
              /* Check Digit */

                if ((pKeyMsg->SEL.INPUT.lData > 10L) || (pKeyMsg->SEL.INPUT.lData < 1L)){
                    return(LDT_KEYOVER_ADR);                            /* key input error */
                }
                ParaOepTbl.uchMajorClass = CLASS_PARAOEPTBL;             /* Set Major Class */
                ParaOepTbl.uchStatus = MAINT_STS_TBLADDRESS;             /* Set Status Table Address In */
                ParaOepTbl.uchTblAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintOepRead(&ParaOepTbl)) == SUCCESS) {   /* Read Function Completed */
                    UieReject();                                         /* Shift Current Func. to Active Func. */
                    return(SUCCESS);
                }
                return(sError);
            }else {             /* W/O Data */
                ParaOepTbl.uchMajorClass = CLASS_PARAOEPTBL;              /* Set Major Class */
                ParaOepTbl.uchStatus = MAINT_STS_TBLADDINC;              /* Set Status Table Address In */
                if ((sError = MaintOepRead(&ParaOepTbl)) == SUCCESS) {   /* Read Function Completed */
                    UieReject();                                         /* Shift Current Func. to Active Func. */
                    return(SUCCESS);
                }
                return(sError);
            }

        case FSC_AC:
            if (pKeyMsg->SEL.INPUT.uchLen) {                               /* W/ Amount Case */
              /* Check Digit */

                if ((pKeyMsg->SEL.INPUT.lData > 99L) || (pKeyMsg->SEL.INPUT.lData < 0L)){
                    return(LDT_KEYOVER_ADR);                            /* key input error */
                }
                ParaOepTbl.uchMajorClass = CLASS_PARAOEPTBL;             /* Set Major Class */
                ParaOepTbl.uchStatus = MAINT_STS_TBLDATA;             /* Set Status Table Data In */
                ParaOepTbl.uchOepData[0] = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintOepWrite(&ParaOepTbl)) == SUCCESS) {   /* Read Function Completed */
                    ParaOepTbl.uchMajorClass = CLASS_PARAOEPTBL;              /* Set Major Class */
                    ParaOepTbl.uchStatus = MAINT_STS_TBLADDINC;             /* Set Status Table Address In */
                    if ((sError = MaintOepRead(&ParaOepTbl)) == SUCCESS) {   /* Read Function Completed */
                        UieReject();                                     /* Shift Current Func. to Active Func. */
                        return(SUCCESS);
                    }
                }
                
                return(sError);

            }else {
                UieExit(aAC160Func);                                            /* Return to UifAC160Function() */
                return(SUCCESS);
            }
        default:
            return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

