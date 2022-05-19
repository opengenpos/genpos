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
* Title       : Hosts IP/Port Assignment Control Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG51.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG99Function()   : Program No.99 Entry Mode
*               UifPG99IssueRpt()   : Program No.99 Issue Report 
*               UifPG99EnterAddr()  : Program No.99 Enter Address Mode 
*               UifPG99EnterData()  : Program No.99 Enter Data Mode 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Nov-26-99:00.00.01    :M.Ozawa    : initial                                   
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
#include <uifprog.h>
#include <uifpgequ.h>

#include "uifpgex.h"                        /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifPG99Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.99 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG99Init[] = {{UifPG99IssueRpt, CID_PG99ISSUERPT},
                                    {UifPG99EnterAddr, CID_PG99ENTERADDR},
                                    {UifPG99EnterData, CID_PG99ENTERDATA},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG99Accept[] = {{UifPG99EnterAddr, CID_PG99ENTERADDR},
                                      {UifPG99EnterData, CID_PG99ENTERDATA},
                                      {UifPG99GoToReboot, CID_PG99GOTOREBOOT},
                                      {NULL,             0                }};


SHORT UifPG99Function(KEYMSG *pKeyMsg) 
{
    PARAKDSIP   ParaKdsIp;
    
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG99Init);                                    /* Open Next Function */
        
        /* Display Address 1 of This Function  */

        ParaKdsIp.uchStatus = 0;                                      /* Set W/ Amount Status */
        ParaKdsIp.uchAddress = 1;                                     /* Set Address 1 */
        ParaKdsIp.uchMajorClass = CLASS_MAINTHOSTSIP;                    /* Set Major Class */

        MaintHostsIpRead(&ParaKdsIp);                                  /* Execute Read Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG99Accept);                                  /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                                  /* execute Default Procedure */
    }
    return(SUCCESS);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG99IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.99 Issue Report 
*===============================================================================
*/
    


SHORT UifPG99IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintHostsIpReport();                   /* Execute Report Procedure */
            UieExit(aPGEnter);                          /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG99EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.99 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG99EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAKDSIP       ParaKdsIp;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaKdsIp.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaKdsIp.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                ParaKdsIp.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Slip Feed Control Procedure */
            
            ParaKdsIp.uchMajorClass = CLASS_MAINTHOSTSIP;  /* Set Major Class */

            if ((sError = MaintHostsIpRead(&ParaKdsIp)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG51Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG99EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.99 Enter Data Mode  
*===============================================================================
*/
    

SHORT UifPG99EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAKDSIP       ParaKdsIp;
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT12);  /* Start Echo Back and Set Input Digit */
        UieOpenSequence(aszSeqPGFSC_P1);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT12) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaKdsIp.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaKdsIp.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                _tcsncpy(ParaKdsIp.aszInputData, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
                //memcpy(ParaKdsIp.aszInputData, pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
				ParaKdsIp.parakdsIPPort.uchIpAddress[0] = pKeyMsg->SEL.INPUT.uchLen;
                
				//SR 14 ESMITH
				//ParaKdsIp.ausIPAddress[0] = pKeyMsg->SEL.INPUT.uchLen;
            }

            /* Execute Write Slip Feed Control Procedure */
            
            ParaKdsIp.uchMajorClass = CLASS_MAINTHOSTSIP;   /* Set Major Class */

            if ((sError = MaintHostsIpWrite(&ParaKdsIp)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG51Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG99GoToReboot(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.99 Issue Report 
*===============================================================================
*/
    


SHORT UifPG99GoToReboot(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintHostsIpReboot();                       /* Execute Reboot Procedure */
            UieExit(aPGEnter);                          /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/****** End of Source ******/  
    
    
