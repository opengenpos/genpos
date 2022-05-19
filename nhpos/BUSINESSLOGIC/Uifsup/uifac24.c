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
* Title       : Hourly Activity Report Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC24.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC24Function()       : A/C No.24 Function Entry Mode
*               UifAC24EnterDataType()  : A/C No.24 Enter Data Type Mode
*               UifAC24EnterRptType()   : A/C No.24 Enter Report Type Mode
*               UifAC24EnterData1()     : A/C No.24 Enter Start Time Mode
*               UifAC24EnterData2()     : A/C No.24 Enter End Time Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-10-92:00.00.01    :K.You      : initial                                   
* Jul-26-95:03.00.03    :M.Ozawa    : Modify for Report on LCD
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
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC24Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.24 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */
UIMENU FARCONST aChildAC24Init[] = {{UifAC24EnterDataType, CID_AC24ENTERDATATYPE},
                                    {NULL,                 0                    }};

SHORT UifAC24Function(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieNextMenu(aChildAC24Init);                            /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC24EnterDataType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.24 Enter Data Type Mode 
*===============================================================================
*/
    
/* Define Next Function */
UIMENU FARCONST aChildAC24EnterDataType[] = {{UifAC24EnterRptType, CID_AC24ENTERRPTTYPE},
                                             {NULL,                0                   }};

SHORT UifAC24EnterDataType(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        UifACPGMLDDispCom(DaylyPTDSaveSelect);
        MaintDisp(AC_HOURLY_READ_RPT, 0, 0, 0, 0, 0, 0L, LDT_SLCDORP_ADR);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* allow AC key for PTD, V3.3 */
            /* return(LDT_SEQERR_ADR); */

        case FSC_NUM_TYPE:
            /* Check W/ Amount */
            if (!(pKeyMsg->SEL.INPUT.uchLen)) {                 /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Check Input Data Range */
			switch (pKeyMsg->SEL.INPUT.lData) {
				case RPT_TYPE_DALY:
				case RPT_TYPE_PTD:
				case RPT_TYPE_DALYSAVE:
				case RPT_TYPE_PTDSAVE:
					break;

				default:
					return(LDT_KEYOVER_ADR);
					break;
			}
            
            /* Set Data Type to Own Save Area */
            uchUifACDataType = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            UieNextMenu(aChildAC24EnterDataType);
            return(SUCCESS);
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC24EnterRptType(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.24 Enter Report Type Mode 
*===============================================================================
*/
    
/* Define Next Function */
UIMENU FARCONST aChildAC24EnterRptType[] = {{UifAC24EnterData1, CID_AC24ENTERDATA1},
                                            {NULL,              0                 }};


SHORT UifAC24EnterRptType(KEYMSG *pKeyMsg) 
{
    SHORT   sError;                        

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC23EnterDataType);   /* Register Key Sequence */
        UifACPGMLDDispCom(TypeSelectReport3);
        MaintDisp(AC_HOURLY_READ_RPT, 0, 0, uchUifACDataType, 0, 0, 0L, LDT_REPOTYPE_ADR);
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            return(LDT_SEQERR_ADR);

        case FSC_AC:
            /* Check W/ Amount */
            if (!(pKeyMsg->SEL.INPUT.uchLen)) {                 /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }

            if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_ALL) {             /* All Read Report Case */
                /* Clear Lead Through */
                uchUifACRptType = RPT_TYPE_ALL;     /* Set Report Type to Own Save Area */
                UifACPGMLDClear();
                MaintDisp(AC_HOURLY_READ_RPT, 0, 0, uchUifACDataType, RPT_TYPE_ALL, 0, 0L, 0);
				switch (uchUifACDataType) {
					case RPT_TYPE_DALY:             /* AC24 Daily Function Case */
						sError = RptHourlyDayRead(CLASS_TTLCURDAY, RPT_ALL_READ);
						break;
					case RPT_TYPE_PTD:              /* AC24 PTD Function Case */
						sError = RptHourlyPTDRead(CLASS_TTLCURPTD, RPT_ALL_READ);
						break;
					case RPT_TYPE_DALYSAVE:         /* AC24 Previous or Saved Daily Function Case */
						sError = RptHourlyDayRead(CLASS_TTLSAVDAY, RPT_ALL_READ);
						break;
					case RPT_TYPE_PTDSAVE:          /* AC24 Previous or Saved PTD Function Case */
						sError = RptHourlyPTDRead(CLASS_TTLSAVPTD, RPT_ALL_READ);
						break;
				}
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                            /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            } else if (pKeyMsg->SEL.INPUT.lData == RPT_TYPE_HOURANGE) {     /* Range Read Report Case */
                UieNextMenu(aChildAC24EnterRptType);                        
                return(SUCCESS);
            } else {
                return(LDT_KEYOVER_ADR);
            }
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC24EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.24 Enter Start Time Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC24EnterData1[] = {{UifAC24EnterData2, CID_AC24ENTERDATA2},
                                          {NULL,              0                 }};


SHORT UifAC24EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
    RPTHOURLY   RptHourly;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        uchUifACRptType = RPT_TYPE_HOURANGE;     /* Set Report Type to Own Save Area */
        UifACPGMLDClear();
        MaintDisp(AC_HOURLY_READ_RPT, 0, 0, uchUifACDataType, RPT_TYPE_HOURANGE, RPT_HOUR_STSTIME, 0L, LDT_DATA_ADR);
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                                           /* W/o Amount */
                return(LDT_SEQERR_ADR);
            }
        
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            RptHourly.StartTime.usHour =  ( USHORT)(pKeyMsg->SEL.INPUT.lData/100L);
            RptHourly.StartTime.usMin =  ( USHORT)(pKeyMsg->SEL.INPUT.lData%100L);
            RptHourly.uchMajorClass = CLASS_TTLHOURLY;
            RptHourly.uchMinorClass = CLASS_TTLHOURLY_STSTIME;
            if ((sError = RptHourlyDaySet(&RptHourly)) == SUCCESS) {         
                UieNextMenu(aChildAC24EnterData1);                          /* Open Next Function */                                                    
            }
            return(sError);

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC24EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.24 Enter End Time Mode 
*===============================================================================
*/
SHORT UifAC24EnterData2(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
    RPTHOURLY   RptHourly;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);            /* Register Key Sequence */
        UifACPGMLDClear();
        MaintDisp(AC_HOURLY_READ_RPT, 0, 0, uchUifACDataType, RPT_TYPE_HOURANGE, RPT_HOUR_ENDTIME, 0L, LDT_DATA_ADR);
        return(SUCCESS);
                                                                            
    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {                               /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            RptHourly.uchStatus = 0;

            if (!pKeyMsg->SEL.INPUT.uchLen) {                                           /* W/o Amount */
                RptHourly.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                RptHourly.StartTime.usHour = ( USHORT)(pKeyMsg->SEL.INPUT.lData/100L);
                RptHourly.StartTime.usMin = ( USHORT)(pKeyMsg->SEL.INPUT.lData%100L);
            }
            RptHourly.uchMajorClass = CLASS_TTLHOURLY;
            RptHourly.uchMinorClass = CLASS_TTLHOURLY_ENDTIME;
            if ((sError = RptHourlyDaySet(&RptHourly)) == SUCCESS) {

                /* Clear Lead Through */
                UifACPGMLDClear();
                MaintDisp(AC_HOURLY_READ_RPT, 0, 0, uchUifACDataType, RPT_TYPE_HOURANGE, 0, 0L, 0);

				switch (uchUifACDataType) {
					case RPT_TYPE_DALY:             /* AC24 Daily Function Case */
						sError = RptHourlyDayRead(CLASS_TTLCURDAY, RPT_RANGE_READ);
						break;
					case RPT_TYPE_PTD:              /* AC24 PTD Function Case */
						sError = RptHourlyPTDRead(CLASS_TTLCURPTD, RPT_RANGE_READ);
						break;
					case RPT_TYPE_DALYSAVE:         /* AC24 Previous or Saved Daily Function Case */
						sError = RptHourlyDayRead(CLASS_TTLSAVDAY, RPT_RANGE_READ);
						break;
					case RPT_TYPE_PTDSAVE:          /* AC24 Previous or Saved PTD Function Case */
						sError = RptHourlyPTDRead(CLASS_TTLSAVPTD, RPT_RANGE_READ);
						break;
				}
                UieExit(aACEnter);
                if (sError == RPT_ABORTED) {                                /* Aborted by User */
                    sError = SUCCESS;
                }
                return(sError);
            }
            return(sError);

        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/**** End of File ****/
