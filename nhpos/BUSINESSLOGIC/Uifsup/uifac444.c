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
* Title       : Copy Total/Counter File Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC42.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC42Function()   : A/C No.42 Function Entry Mode 
*               UifAC42EnterKey()   : A/C No.42 Enter AC Key Mode   
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jul-09-92: K.You     : initial                                   
*          :           :                                    
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
#include "uifsupex.h"
#include <csstbstb.h>
#include "display.h"
#include <report.h>

/*
*=============================================================================
**  Synopsis:    SHORT UifAC444Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.444 Function Entry Mode
*                This Action Code is used to join or unjoin a disconnected
*                Satellite terminal to or from a cluster.  A disconnected
*                Satellite is a terminal that can be unjoined or disconnected
*                from a cluster and will then act as a Master terminal while
*                disconnected from the cluster.
*
*                After assumming the Master terminal role, a disconnected
*                Satellite can join an existing cluster and then upload its
*                totals to the Master terminal of the cluster.  When joined
*                to a cluster, a disconnected Satellite terminal acts like a
*                standard Satellite terminal.
*
*   Associations:
*                CID_AC444FUNCTION  -> identifier in uifsup.h
*              , AC_JOIN_CLUSTER -> Action Code identifier in paraequ.h
*                LCD_AC444_ADR  -> LCD mnemonic in paraequ.h
*                ACNo444[] -> MLDMENU in file mldmatfm.c
*                CLI_FCOPJOINCLUSTER     -> CLI code in Client Stub, CliOpJoinCluster(), SerOpJoinCluster()
*                CLI_FCOPUNJOINCLUSTER   -> CLI code in Client Stub, CliOpUnjoinCluster, SerOpUnjoinCluster()
*                CLI_FCOPTRANSFERTOTALS  -> CLI code in Client Stub, CliOpTransferTotals, SerOpTransferTotals()
*
*  Functions:    Functions used to implement the AC444 functionality.
*                OpJoinCluster()
*                SerOpJoinCluster()
*                CliOpJoinCluster()
*                CstIamDisconnectedSatellite()
*===============================================================================
*/

/* Define Next Function */

UIMENU FARCONST aChildAC444EnterKey[] = {{UifAC444EnterKey, CID_AC444ENTERKEY},
                                        {NULL,            0               }};

    
SHORT UifAC444Function(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC444EnterKey);            /* Open Next Function */

        MaintDisp(AC_JOIN_CLUSTER,                      /* A/C Number */
                  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_REPOCD_ADR);                  /* " Copy OK? " Lead Through Address */
                  
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}

/*
*==============================================================================
**  Synopsis: SHORT UifAC444EnterKey(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.444 Enter AC Key Mode     
*===============================================================================
*/


SHORT UifAC444EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT   sError;
	REGDISPMSG  DispMsgData; // used to display DSC indicator

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */

		UifACPGMLDDispCom(TypeAC444);

		MaintDisp(AC_JOIN_CLUSTER,                
			0,                               
			0,                               
			0,                               
			0,                                
			0,                               
			0L,                             
			LDT_AC_444); 

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {                          
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {// No key pressed before AC key press
                return(LDT_SEQERR_ADR);
            }			
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {// Too many keys pressed
                return(LDT_KEYOVER_ADR);
            }

			if (pKeyMsg->SEL.INPUT.lData == 1){		/* Status */
                /* Clear Lead Through */
                UifACPGMLDClear();

				MaintDisp(AC_JOIN_CLUSTER,          /* A/C Number, used in RptCheckReportOnMld() to check displayable or not. */
				  CLASS_MAINTDSPCTL_10N,            /* 10N10D Display */
				  0,                                /* Page Number */
				  0,                                /* PTD Type */
				  0,                                /* Report Type */
				  0,				                /* Reset Type */
				  0L,                               /* Amount Data */
				  0);                   /* "Lead Through for Reset Report"Lead Through Address */
				
				MaintMakeHeader(CLASS_MAINTHEADER_RPT,          /* Minor Data Class */              
						RPT_ACT_ADR,                            /* Report Name Address */
						0		,                               /* Report Name Address */
						0,                                      /* Special Mnemonics Address */
						RPT_READ_ADR,                           /* Report Name Address */
						0,                                      /* Read Type */
						AC_JOIN_CLUSTER,                        /* A/C Number */
						0,                                      /* Reset Type */
						PRT_RECEIPT|PRT_JOURNAL);   /* Print Control Bit */


				RptTerminalConnectStatus(NULL);

				MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
				UieExit(aACEnter);                              /* Return to UifACEnter() */
				return(SUCCESS);                                /* Return Success or Error Status */
			}

			if (pKeyMsg->SEL.INPUT.lData == 2){		/* Unjoin/Detach/Disconnect/Clear  */
				if (CstIamDisconnectedSatellite() != STUB_SUCCESS)
					return(LDT_PROHBT_ADR);

				if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS)
					return(LDT_PROHBT_ADR);

                /* Clear Lead Through */
                UifACPGMLDClear();

				MaintDisp(AC_JOIN_CLUSTER,                          /* A/C Number */
					0,                                    /* Descriptor */
					0,                                    /* Page Number */
					0,                                    /* PTD Type */
					0,                                    /* Report Type */
					(UCHAR)pKeyMsg->SEL.INPUT.lData,      /* Reset Type */
					0L,                                   /* Amount Data */
					LDT_COMUNI_ADR);                      /* " During Comm. " Lead Through Address */
				
				sError = MaintUnjoinCluster();            /* Execute the Unjoin command */

				// below code and comments copied from handling of D-B indicator
				// in pifmain.c PifCheckDelayBalance()
				memset(&DispMsgData, 0x00, sizeof(DispMsgData));
				//We set the class so that we can send the dummy data,
				//and trigger the discriptor to show S-F to inform the user that
				//the terminal is in store forward mode
				DispMsgData.uchMajorClass = CLASS_UIFREGDISP;
					
				DispWrite(&DispMsgData);

				UieExit(aACEnter);                        /* Return to UifACEnter() */
				return(sError);                           /* Return Success or Error Status */
			}
						
			if (pKeyMsg->SEL.INPUT.lData == 3){		/* Join/Attach/Connect/Transfer Totals */
				if (CstIamDisconnectedUnjoinedSatellite() != STUB_SUCCESS)
					return(LDT_PROHBT_ADR);
				
                /* Clear Lead Through */
                UifACPGMLDClear();

				MaintDisp(AC_JOIN_CLUSTER,                          /* A/C Number */
					0,                                    /* Descriptor */
					0,                                    /* Page Number */
					0,                                    /* PTD Type */
					0,                                    /* Report Type */
					(UCHAR)pKeyMsg->SEL.INPUT.lData,      /* Reset Type */
					0L,                                   /* Amount Data */
					LDT_COMUNI_ADR);                      /* " During Comm. " Lead Through Address */
				
				sError = MaintJoinCluster();                    /* Execute the join command */
				
				// below code and comments copied from handling of D-B indicator
				// in pifmain.c PifCheckDelayBalance()
				memset(&DispMsgData, 0x00, sizeof(DispMsgData));
				//We set the class so that we can send the dummy data,
				//and trigger the discriptor to show S-F to inform the user that
				//the terminal is in store forward mode
				DispMsgData.uchMajorClass = CLASS_UIFREGDISP;
				
				DispWrite(&DispMsgData);

				UieExit(aACEnter);                              /* Return to UifACEnter() */
				return(sError);                                 /* Return Success or Error Status */
			}
			
			return(LDT_KEYOVER_ADR);
			/* default:
			break; */
		}

    case UIM_CANCEL:
        /* break */
    
    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}