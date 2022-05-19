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
* Title       : PLU Group Summary Read Report Module
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC30.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               UifAC31Function()       : A/C No.30 Function Entry Mode
*               UifAC31EnterDataType()  : A/C No.30 Enter Data Type Mode
*               UifAC31EnterRptType()   : A/C No.30 Enter Report Type Mode
*               UifAC31EnterData()      : A/C No.30 Enter Data Mode
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* May-08-95: 03.00.00   : T.Satoh   : initial
* Jul-27-95: 03.00.03   : M.Ozawa   : Modify to report on Scroll Display
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
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s
;=============================================================================
*/

UIMENU FARCONST
aChildAC31EnterDataType[] = {{UifAC31EnterRptType, CID_AC31ENTERRPTTYPE},
                             {NULL,                0                   }};

UIMENU FARCONST
aChildAC31Init[]          = {{UifAC31EnterDataType, CID_AC31ENTERDATATYPE},
                             {NULL,                 0                    }};

UIMENU FARCONST
aChildAC31EnterRptType[]  = {{UifAC31EnterData, CID_AC31ENTERDATA},
                             {NULL,           0                  }};
/* reduce key data, V3.3
UISEQ FARCONST
aszSeqAC30EnterDataType[] = {FSC_NUM_TYPE, FSC_AC, 0};
*/

UISEQ FARCONST
aszSeqAC31EnterData[] = {FSC_AC, FSC_CANCEL, 0};

/*
*=============================================================================
**  Synopsis: SHORT UifAC31Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.30 Function Entry
*==============================================================================
*/

SHORT UifAC31Function(KEYMSG *pKeyMsg)
{
    switch(pKeyMsg->uchMsg){
        case UIM_INIT:
            /*
             * Case : PTD is not Existed
             */
            if(!RptPTDChkFileExist(FLEX_CPN_ADR)){
                /*
                 * Set Data Type to Own Save Area
                 */
                uchUifACDataType = (UCHAR)RPT_TYPE_DALY;
                UieNextMenu(aChildAC31EnterDataType);
            }
            /*
             * Case : PTD is Existed
             */
            else{
                UieNextMenu(aChildAC31Init);
            }
            return(SUCCESS);

        default:
            return(UifACDefProc(pKeyMsg));     /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC31EnterDataType(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: IF PTD file is Existed, then get DATA TYPE.
*==============================================================================
*/

SHORT UifAC31EnterDataType(KEYMSG *pKeyMsg)
{
    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC31EnterDataType);
            /* UieOpenSequence(aszSeqAC30EnterDataType); */

            UifACPGMLDDispCom(DaylyPTDSelect);

            MaintDisp(AC_CPN_READ_RPT,      /* A/C Number                   */
                      0, 0, 0, 0, 0, 0L,
                      LDT_SLCDORP_ADR);     /* "Select Daily or PTD Report" */
            return(SUCCESS);

        case UIM_INPUT:
            switch (pKeyMsg->SEL.INPUT.uchMajor) {
                case FSC_AC:
                    /* allow AC key for PTD, V3.3 */
                    /* return(LDT_SEQERR_ADR); */

                case FSC_NUM_TYPE:
                    /* Without anything keys */
                    if(!pKeyMsg->SEL.INPUT.uchLen){
                        return(LDT_SEQERR_ADR);
                    }

                    /* Check Input Data Range */
                    if(pKeyMsg->SEL.INPUT.lData != RPT_TYPE_DALY &&
                                   pKeyMsg->SEL.INPUT.lData != RPT_TYPE_PTD){
                        return(LDT_KEYOVER_ADR);
                    }

                    /* Set Data Type to Own Save Area */
                    uchUifACDataType = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    UieNextMenu(aChildAC31EnterDataType);
                    return(SUCCESS);

                /* default: break; */
                }
                /* break; */

        default:
            return(UifACDefProc(pKeyMsg));   /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC31EnterRptType(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: IF PTD file is Existed, or DATA TYPE already inputed,
*                 then get REPORT TYPE.
*==============================================================================
*/

SHORT UifAC31EnterRptType(KEYMSG *pKeyMsg)
{
    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC31EnterDataType);
            /* UieOpenSequence(aszSeqAC30EnterDataType); */

            UifACPGMLDDispCom(TypeSelectReport6);

            MaintDisp(AC_PLUGROUP_READ_RPT,       /* A/C Number                  */
                      0, 0,
                      uchUifACDataType,      /* PTD Type                    */
                      0, 0, 0L,
                      LDT_REPOTYPE_ADR);     /* "Request Report Type Entry" */
            return(SUCCESS);

        case UIM_ACCEPTED:
            UieNextMenu(aChildAC31EnterRptType);
            return(SUCCESS);

        case UIM_INPUT:
            switch (pKeyMsg->SEL.INPUT.uchMajor) {
                case FSC_NUM_TYPE:
                    return(LDT_SEQERR_ADR);

                case FSC_AC:
                    /* Without anything keys */
                    if(!pKeyMsg->SEL.INPUT.uchLen){
                        return(LDT_SEQERR_ADR);
                    }

                    switch(pKeyMsg->SEL.INPUT.lData){
                         /************************/
                         /* All Read Report Case */
                         /************************/
                         case RPT_TYPE_ALL:
                            /* Clear Lead Through */

                             uchUifACRptType = ( UCHAR)RPT_TYPE_ALL;     /* Set Report Type to Own Save Area */

                             UifACPGMLDClear();

                             MaintDisp(AC_PLUGROUP_READ_RPT,     /* A/C Number   */
                                       0, 0,
                                       uchUifACDataType,    /* PTD Type     */
                                       (UCHAR)RPT_TYPE_ALL, /* Report Type  */
                                       0, 0L, 0);

                             /* Daily Report */
                             if(uchUifACDataType == RPT_TYPE_DALY){
                                 RptPLUGroupRead(CLASS_TTLCURDAY,RPT_ALL_READ,0);
                             }
                             /* PTD Report */
                             else{
                                 RptPLUGroupRead(CLASS_TTLCURPTD,RPT_ALL_READ,0);
                             }

                             UieExit(aACEnter);   /* Return to UifACEnter() */
                             return(SUCCESS);

                        /*******************************/
                        /* Individual Read Report Case */
                        /*******************************/
                        case RPT_TYPE_IND:
                            uchUifACRptType = (UCHAR)RPT_TYPE_IND;

                            UifACPGMLDClear();

                            UieNextMenu(aChildAC31EnterRptType);
                            return(SUCCESS);

                        default:
                            return(LDT_KEYOVER_ADR);
                    }
                /* default: break; */
            }
            /* break; */

        default:
            return(UifACDefProc(pKeyMsg));     /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC31EnterData(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: IF REPORT TYPE is INDIVIDUAL PRINT, then get Coupon Number.
*==============================================================================
*/

SHORT UifAC31EnterData(KEYMSG *pKeyMsg)
{
    SHORT   sError;

    switch(pKeyMsg->uchMsg) {
        case UIM_INIT:
            UieOpenSequence(aszSeqAC31EnterData);

            MaintDisp(AC_PLUGROUP_READ_RPT,                  /* A/C Number       */
                      CLASS_MAINTDSPCTL_10NPG,          /* Set Minor Class for 10N10D */
                      0,
                      uchUifACDataType,                 /* PTD Type         */
                      uchUifACRptType,                  /* Report Type      */
                      0, 0L,
                      LDT_PLUGROUPNO_ADR);                    /* "Enter PLU Group #" */
            return(SUCCESS);

        case UIM_INPUT:
            switch (pKeyMsg->SEL.INPUT.uchMajor) {
            case FSC_CANCEL:
                if(uchRptOpeCount & RPT_HEADER_PRINT){          /* Not First Operation */
                    MaintMakeAbortKey();
                }
                break;

			case FSC_AC:

				// Without anything keys or Over 3 digit
                if((pKeyMsg->SEL.INPUT.uchLen < 0) ||
                   (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3)){
                    return(LDT_SEQERR_ADR);
                }
				if(pKeyMsg->SEL.INPUT.uchLen == 0)
				{
					MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);   /* Print Trailer */
					UieExit(aACEnter);		/* Return to UifACEnter() */
					return(SUCCESS);
				}
                // Check 0 Data
                if(pKeyMsg->SEL.INPUT.lData == 0){
                    return(LDT_KEYOVER_ADR);
                }

                /* Clear Lead Through */

                 uchUifACRptType = ( UCHAR)RPT_TYPE_IND;     /* Set Report Type to Own Save Area */

                 UifACPGMLDClear();

                 MaintDisp(AC_PLUGROUP_READ_RPT,     /* A/C Number   */
                           0, 0,
                           uchUifACDataType,    /* PTD Type     */
                           uchUifACRptType, /* Report Type  */
                           0, 0L, 0);

                 /* Daily Report */
                 if(uchUifACDataType == RPT_TYPE_DALY){
					sError = RptPLUGroupRead(CLASS_TTLCURDAY,RPT_TYPE_IND,(UCHAR)pKeyMsg->SEL.INPUT.lData);

					MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);   /* Print Trailer */
					UieExit(aACEnter);		/* Return to UifACEnter() */
					return(SUCCESS);
                 }
                 /* PTD Report */
                 else{
                     sError = RptPLUGroupRead(CLASS_TTLCURPTD,RPT_TYPE_IND,(UCHAR)pKeyMsg->SEL.INPUT.lData);
                 }

                 UieAccept();
                 return(sError);

                /*
				UieAccept();
                /* Aborted by User
                if(sError == RPT_ABORTED){
                    sError = SUCCESS;
                }
                return(sError);
				*/
        }

        default:
            return(UifACDefProc(pKeyMsg));     /* Execute Default Procedure */
    }
}

/**** End of File ****/

