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
* Title       : Currency Conversion Rate Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC89.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC89Function()       : A/C No.89 Function Entry Mode
*               UifAC89EnterAddr()      : A/C No.89 Enter Address Mode
*               UifAC89EnterData1()     : A/C No.89 Enter Data1 Mode
*               UifAC89EnterData2()     : A/C No.89 Enter Data2 Mode
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
**  Synopsis: SHORT UifAC89Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.89 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC89Init[] = {{UifAC89EnterAddr, CID_AC89ENTERADDR},
                                    {UifAC89EnterData1, CID_AC89ENTERDATA1},
                                    {NULL,              0                 }};


/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC89Accept[] = {{UifAC89EnterAddr, CID_AC89ENTERADDR},
                                      {UifAC89EnterData2, CID_AC89ENTERDATA2},
                                      {NULL,              0                 }};


SHORT UifAC89Function(KEYMSG *pKeyMsg) 
{


    PARACURRENCYTBL     ParaCurrTbl;   
                                        

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC89Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */

        ParaCurrTbl.uchStatus = 0;                                  /* Set W/ Amount Status */
        ParaCurrTbl.uchAddress = 1;                                 /* Set Address 1 */
        ParaCurrTbl.uchMajorClass = CLASS_PARACURRENCYTBL;          /* Set Major Class */

        MaintCurrencyRead(&ParaCurrTbl);                            
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC89Accept);                              /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC89EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.89 Enter Address Mode 
*===============================================================================
*/

SHORT UifAC89EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    PARACURRENCYTBL     ParaCurrTbl;
                    


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

            ParaCurrTbl.uchStatus = 0;                                  /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                ParaCurrTbl.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                ParaCurrTbl.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            ParaCurrTbl.uchMajorClass = CLASS_PARACURRENCYTBL;          /* Set Major Class */
            if ((sError = MaintCurrencyRead(&ParaCurrTbl)) == SUCCESS) {    
                UieAccept();                                            /* Return to UifAC89Function() */
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
**  Synopsis: SHORT UifAC89EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.89 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC89EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    PARACURRENCYTBL     ParaCurrTbl;
                   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintCurrencyReport();                                      /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            } else {                                                        /* W/ Amount Case */
                /* Check Digit */

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT9) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }

                ParaCurrTbl.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaCurrTbl.ulForeignCurrency = ( ULONG)pKeyMsg->SEL.INPUT.lData;       

                /* ParaCurrTbl.uchMajorClass = CLASS_PARACURRENCYTBL;                   Set Major Class */
                if ((sError = MaintCurrencyWrite(&ParaCurrTbl)) == SUCCESS) {           
                    UieAccept();                                                        
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
**  Synopsis: SHORT UifAC89EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.89 Enter Data2 Mode 
*===============================================================================
*/
    

SHORT UifAC89EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT               sError;
    PARACURRENCYTBL     ParaCurrTbl;
                   


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

                if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT9) {               /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }

                ParaCurrTbl.uchStatus = 0;                                  /* Set W/ Amount Status */
                ParaCurrTbl.ulForeignCurrency = ( ULONG)pKeyMsg->SEL.INPUT.lData;       

                /* ParaCurrTbl.uchMajorClass = CLASS_PARACURRENCYTBL;           Set Major Class */
                ParaCurrTbl.ulForeignCurrency = ( ULONG)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintCurrencyWrite(&ParaCurrTbl)) == SUCCESS) {       
                    UieAccept();
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

