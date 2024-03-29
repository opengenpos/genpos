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
* Title       : Cashier Read Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC21.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs1 /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC21Function()       : A/C No.21 Function Entry Mode
*               UifAC21EnterData()      : A/C No.21 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-08-92:00.00.01    :K.You      : initial                                   
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
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uireg.h>
#include <uifpgequ.h>
#include <mldmenu.h>
#include <mldsup.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
*=============================================================================
**  Synopsis: SHORT UifAC21Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.21 Function Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */
UIMENU FARCONST aChildAC21Init[] = {{UifAC21EnterData, CID_AC21ENTERDATA},
                                    {NULL,             0                }};

UIMENU FARCONST aChildAC21EnterDataType[] = {{UifAC21EnterDataType, CID_AC21ENTERDATATYPE},
                                             {NULL,                0                   }};

UIMENU FARCONST aChildAC21EnterUserType[] = {{UifAC21EnterUserType, CID_AC21ENTERUSERTYPE},
											{NULL,					0					}};

SHORT UifAC21Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* V3.3 */
		if (!RptPTDChkFileExist(FLEX_CAS_ADR)) 
		{                 /* Not PTD Case */
				uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;           /* Set Data Type to Own Save Area */
				UieNextMenu(aChildAC21EnterUserType);                            /* Open Next Function */
		} else 
		{
				UieNextMenu(aChildAC21EnterDataType);
		}
		
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC21EnterData(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.21 Enter Data Mode 
*===============================================================================
*/
SHORT UifAC21EnterData(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */

        MaintDisp(AC_CASH_READ_RPT,                 /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  uchUifACDataType,                 /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Clear Lead Through */
            MaintDisp(AC_CASH_READ_RPT,             /* A/C Number */
                      0,                            /* Descriptor */
                      0,                            /* Page Number */
                      uchUifACDataType,             /* PTD Type */
                      0,                            /* Report Type */
                      0,                            /* Reset Type */
                      0L,                           /* Amount Data */
                      LDT_NUM_ADR);                 /* "Number Entry" Lead Through Address */


            /* Check 0 Data and Digit */
            if (!pKeyMsg->SEL.INPUT.lData || (pKeyMsg->SEL.INPUT.uchLen > UIFREG_MAX_ID_DIGITS)) {      

                MaintDisp(AC_CASH_READ_RPT,                 /* A/C Number */
                          0,                                /* Descriptor */
                          0,                                /* Page Number */
                          uchUifACDataType,                 /* PTD Type */
                          0,                                /* Report Type */
                          0,                                /* Reset Type */
                          0L,                               /* Amount Data */
                          LDT_NUM_ADR);                     /* "Number Entry" Lead Through Address */

                return(LDT_KEYOVER_ADR);
            }
			
			// determine if user exists  ***PDINU *
			if( sError = RptChkCashierNo(pKeyMsg->SEL.INPUT.lData) ) 
			{
				return( sError );
            }

            if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
				sError = RptCashierRead(CLASS_TTLCURDAY, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, 0);
            } else {    /* V3.3 */
                sError = RptCashierRead(CLASS_TTLCURPTD, RPT_IND_READ, (ULONG)pKeyMsg->SEL.INPUT.lData, 0);
            }
            UieExit(aACEnter);                                                       
            if (sError == RPT_ABORTED) {                        /* Aborted by User */
                sError = SUCCESS;
            }
            return(sError);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }                                                                   
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC21EnterUserType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.21 Enter Data Type Mode V3.3
*===============================================================================
*/
SHORT UifAC21EnterUserType(KEYMSG *pKeyMsg) 
{
	SHORT	sError;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(TypeSelectReport6);

        MaintDisp(AC_CASH_READ_RPT,                  /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_FUNCTYPE_ADR);                 /* "Select Daily or PTD Report" Lead Through Address */

        return(SUCCESS);
 
	case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Input Data Range */
            if (pKeyMsg->SEL.INPUT.lData != RPT_TYPE_ALL && pKeyMsg->SEL.INPUT.lData != RPT_TYPE_IND) {
                return(LDT_KEYOVER_ADR);
            }
            
            /* Set Data Type to Own Save Area */
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {                       /* W/o Amount */
                if (uchUifACDataType == RPT_TYPE_DALY) {                                /* Daily Report Case */
					// save the currently selected display/print option, generate an html version
					// of the report and then restore the user selected display/print option to
					// generate the report to the user selected device.
					UCHAR   uchUifACRptOnOffMldSave = uchUifACRptOnOffMld;
					FILE    *fpFile = ItemOpenHistorialReportsFolder (RPTREGFIN_FOLDER_ANY, AC_CASH_READ_RPT, CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0, 0);

                    if (fpFile) {
                        sError = ItemGenerateAc21Report(CLASS_TTLCASHIER, CLASS_TTLCURDAY, RPT_ALL_READ, fpFile, 0);
                        ItemCloseHistorialReportsFolder(fpFile);
                    }
					uchUifACRptOnOffMld = uchUifACRptOnOffMldSave;
                    sError = RptCashierRead(CLASS_TTLCURDAY, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                } else {    /* V3.3 */
                    sError = RptCashierRead(CLASS_TTLCURPTD, RPT_ALL_READ, 0, 0);                         /* Execute All Read Report */
                }
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                        /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }else
			{
				UieNextMenu(aChildAC21Init);                            /* Open Next Function */
			}
            
            UieNextMenu(aChildAC21Init);
            return(SUCCESS);
        
        /* default:
            break; */
        }
        /* break; */
    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC21EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.21 Enter Data Type Mode V3.3
*===============================================================================
*/
SHORT UifAC21EnterDataType(KEYMSG *pKeyMsg) 
{

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */

        UifACPGMLDDispCom(DaylyPTDSelect);

        MaintDisp(AC_CASH_READ_RPT,                  /* A/C Number */
                  0,                                /* Descriptor */
                  0,                                /* Page Number */
                  0,                                /* PTD Type */
                  0,                                /* Report Type */
                  0,                                /* Reset Type */
                  0L,                               /* Amount Data */
                  LDT_SLCDORP_ADR);                 /* "Select Daily or PTD Report" Lead Through Address */

        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                   /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Input Data Range */
            if (pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY && pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD) {
                return(LDT_KEYOVER_ADR);
            }
            
            /* Set Data Type to Own Save Area */
            uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            
            UieNextMenu(aChildAC21EnterUserType);
            return(SUCCESS);
        
        /* default:
            break; */
        }
        /* break; */
                                                                
    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*** End of File ****/
