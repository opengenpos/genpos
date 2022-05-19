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
* Title       : PLU Sales Restriction Table Maintenance/Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US GP Application         
* Program Name: UIFAC170.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC170Function()      : A/C No.170 Function Entry Mode
*               UifAC170EnterData1()    : A/C No.170 Enter Data1 Mode
*               UifAC170EnterData2()    : A/C No.710 Enter Data2 Mode
*               UifAC170EnterAddr()     : A/C No.170 Enter Address Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
*    Date  : Ver.Rev.   :   Name    : Description
* Apr-20-93:00.00.01    : M.Ozawa   : Initial for GP
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
#include <cvt.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC170Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.170 Function Entry Mode  
*===============================================================================
*/


UIMENU FARCONST aChildAC170Init[] = {{UifAC170EnterData1, CID_AC170ENTERDATA1},
                                      {UifAC170EnterAddr, CID_AC170ENTERADDR},
                                      {NULL,               0                  }};


UIMENU FARCONST aChildAC170Accept[] = {{UifAC170EnterData2, CID_AC170ENTERDATA2},
                                      {UifAC170EnterAddr, CID_AC170ENTERADDR},
                                      {NULL,               0                  }};

SHORT UifAC170Function(KEYMSG *pKeyMsg) 
{



    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC170Init);                /* prepare ac70 report */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC170Accept);              /* prepare next entry */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC170EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.170 Enter Address Mode 
*===============================================================================
*/

UISEQ FARCONST aszSeqAC170EnterAddr[] = { FSC_NUM_TYPE, FSC_QTY, 0};

SHORT UifAC170EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError = SUCCESS;
    PARARESTRICTION ParaRest;
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC170EnterAddr);                           /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaRest.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaRest.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaRest.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaRest.uchMajorClass = CLASS_PARARESTRICTION;
            ParaRest.uchMinorClass = CLASS_PARAREST_ADDRESS;
            if ((sError = MaintRestRead(&ParaRest)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC70Function() */
            }
            return(sError);                                             /* Set Success or Input Data Error */
            
        case FSC_QTY:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            ParaRest.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaRest.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaRest.uchField = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaRest.uchMajorClass = CLASS_PARARESTRICTION;
            ParaRest.uchMinorClass = CLASS_PARAREST_FIELD;
            if ((sError = MaintRestEdit(&ParaRest)) == SUCCESS) {
                UieAccept();
            }
            return(sError);
            break;

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
**  Synopsis: SHORT UifAC170EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.170 Enter Data1 Mode (prepare report isuue)
*===============================================================================
*/
    
UISEQ FARCONST aszSeqAC170EnterData[] = { FSC_AC, 0};

SHORT UifAC170EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError = SUCCESS;
    PARARESTRICTION ParaRest;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:

        /* read 1st address 1st field data */

        ParaRest.uchMajorClass = CLASS_PARARESTRICTION;
        ParaRest.uchMinorClass = CLASS_PARAREST_READ;
        ParaRest.uchAddress = REST_CODE1_ADR;
        ParaRest.uchField = REST_DATE_FIELD;

        MaintRestRead(&ParaRest);

        UieOpenSequence(aszSeqAC170EnterData);                              /* Register Key Sequence */
        return(sError);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintRestReport();                                          /* Issue Table Report */          
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);                        /* Execute Finalize Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {
                if ((sError = UifAC170ACCheck(pKeyMsg)) == SUCCESS) {
                    UieAccept();         /* return pararent function and set write function */
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
**  Synopsis: SHORT UifAC170EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.170 Enter Data2 Mode (prepare restriction table write)
*===============================================================================
*/
    
SHORT UifAC170EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError = SUCCESS;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC170EnterData);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                if (MaintRestChkStatus() == MAINT_DATA_INPUT) {
                    if ((sError = MaintRestWrite()) == SUCCESS){            /* Write Table to Para. Module */
                        UieAccept();
                    }
                } else {
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);                        /* Execute Finalize Procedure */
                    UieExit(aACEnter);                                          /* Return to UifACEnter() */
                    return(sError);
                }
            } else {                                                        /* W/ Amount Case */
                if ((sError = UifAC170ACCheck(pKeyMsg)) == SUCCESS) {
                    UieReject();          /* restruct key sequence & clear UIE buffer */
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
**  Synopsis: SHORT UifAC170ACCheck(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.170 Check A/C Key Data Module
*===============================================================================
*/
    
SHORT UifAC170ACCheck(KEYMSG *pKeyMsg) 
{
    
    PARARESTRICTION ParaRest;
                   
    if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {               /* Over Digit */
        return(LDT_KEYOVER_ADR);
    }

    ParaRest.uchMajorClass = CLASS_PARARESTRICTION;
    ParaRest.uchMinorClass = CLASS_PARAREST_DATA;
    ParaRest.uchStatus = 0;                                      /* Set W/ Amount Status */
    ParaRest.uchData = (UCHAR)pKeyMsg->SEL.INPUT.lData;
    return(MaintRestEdit(&ParaRest));                            /* set ac70 data to work area */
}

