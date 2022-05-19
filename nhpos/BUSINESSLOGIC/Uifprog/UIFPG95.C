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
* Title       : Unlock Number for NHPOS Unlock Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG95.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG95Function()   : Program No.95 Entry Mode
*               UifPG95IssueRpt()   : Program No.95 Issue Report 
*               UifPG95EnterAddr()  : Program No.95 Enter Address Mode 
*               UifPG95EnterData()  : Program No.95 Enter Data Mode 
*
* This file contains the source code to implement the P95 interface to allow
* the user to set the Serial Number and the Unlock code for a terminal.
* This code is being implemented to support the Software Security functionality
* beginning with Release 1.4 of NHPOS.
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Dec-19-02:00.00.01    :R. Chambers: initial                                   
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
#include <windows.h>
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
#include "BlFWif.h"

/*
*=============================================================================
**  Synopsis: SHORT UifPG95Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.95 Entry Mode  

	We are using the following conventions for address in Prog 95.
	Address 1 is used to enter the serial number of the terminal.
		We expect that the user will enter the serial number without dashes.
	Address 2 is used to enter the unlock code the customer will get from
		NCR.

	Additional addresses will be used in the future to allow the locking of
	additional pieces of Neighborhood POS.

*===============================================================================
*/
/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG95Init[] = {{UifPG95IssueRpt, CID_PG95ISSUERPT},
                                   {UifPG95EnterAddr, CID_PG95ENTERADDR},
                                   {UifPG95EnterData, CID_PG95ENTERDATA},
                                   {NULL,            0               }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG95Accept[] = {{UifPG95EnterAddr, CID_PG95ENTERADDR},
                                     {UifPG95EnterData, CID_PG95ENTERDATA},
                                     {NULL,            0               }};


/* The new UifPG95Function calls the nag screen
   and allows the user to register NHPOS */
/*
SHORT UifPG95Function(KEYMSG *pKeyMsg) 
{

}
*/

SHORT UifPG95Function(KEYMSG *pKeyMsg) 
{
#ifdef VBOLOCK
    // call the Nag Screen...if it's needed
	if (BlFwIfLicStatus() == REGISTERED_COPY)
		MessageBox(0,_T("Your product is already registered. Press P5 to return to the Program Menu!"),_T("Program 95"), MB_OK);
	else
	{
		//BlFwIfNagScreen();
		TCHAR szUlCode[128];
		TCHAR szStatus[128];

		BlFwIfLicReg(szUlCode,szStatus);
		MessageBox(0,_T("Press P5 to return to the Program Menu"),_T("Program 95"), MB_OK);
	}
    return(SUCCESS);
    
#else	
  
    PARAUNLOCKNO   UnlockData;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildPG95Init);                                 /* Open Next Function */

        /* Display Address 1 of This Function */
		BlFwIfLicStatus();

        UnlockData.uchStatus = 0;                                 /* Set W/ Amount Status */
        UnlockData.uchAddress = 1;                                /* Set Address 1 */
        UnlockData.uchMajorClass = CLASS_PARAUNLOCKNO;          /* Set Major Class */

        MaintUnlockNoRead(&UnlockData);                         /* Execute Read Security No Procedure */
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG95Accept);                               /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                              /* execute Default Procedure */
    }
    return(SUCCESS);
#endif
}


/*
*=============================================================================
**  Synopsis: SHORT UifPG95IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.6 Issue Report  
*===============================================================================
*/
    


SHORT UifPG95IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {            /* W/ Amount Case */
            return(LDT_SEQERR_ADR);                 
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintUnlockNoReport();                /* Execute Report Procedure */
            UieExit(aPGEnter);                      /* Return to UifPGEnter() */
            return(SUCCESS);
        
        /* default:
            break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG95EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful                 
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.6 Enter Address Mode 
*===============================================================================
*/
    


SHORT UifPG95EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAUNLOCKNO   SecurityData;   
                    


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);                        
            }
            SecurityData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SecurityData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                SecurityData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Security No Procedure */
            
            SecurityData.uchMajorClass = CLASS_PARAUNLOCKNO;  /* Set Major Class */

            if ((sError = MaintUnlockNoRead(&SecurityData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG95Function() */
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
**  Synopsis: SHORT UifPG95EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.6 Enter Data Mode 
*===============================================================================
*/
    

SHORT UifPG95EnterData(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAUNLOCKNO  SecurityData;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT24);  /* Start Echo Back and Set Input Digit */
        UieOpenSequence(aszSeqPGFSC_P1);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT24) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            SecurityData.uchStatus = 0;                         /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                SecurityData.uchStatus |= MAINT_WITHOUT_DATA;   /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
				int xLen = pKeyMsg->SEL.INPUT.uchLen;
				if (xLen > PARA_UNLOCK_LEN)
					xLen = PARA_UNLOCK_LEN;
				_tcsncpy (SecurityData.aszUnlockNo, pKeyMsg->SEL.INPUT.aszKey, xLen);
				SecurityData.aszUnlockNo[xLen] = '\0';
            }

            /* Execute Write Security No Procedure */
            
            /* SecurityData.uchMajorClass = CLASS_PARAUNLOCKNO;   Set Major Class */

            if ((sError = MaintUnlockNoWrite(&SecurityData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG95Function() */
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


    
    
    
