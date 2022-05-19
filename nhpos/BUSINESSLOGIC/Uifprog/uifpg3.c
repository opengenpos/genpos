/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : FSC Assignment Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application        
* Program Name: UIFPG3.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG3Function()    : Program No.3 Entry Mode
*               UifPG3IssueRpt()    : Program No.3 Issue Report 
*               UifPG3EnterAddr()   : Program No.3 Enter Address Mode 
*               UifPG3EnterData1()  : Program No.3 Enter Major FSC Data 
*               UifPG3EnterData2()  : Program No.3 Enter Minor FSC Data 
*                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-19-92:00.00.01    :K.You      : initial                                   
* Nov-28-95:03.01.00    :M.Ozawa    : modifiy for direct plu no entry
* Jan-26-98:03.01.08    :M.Ozawa    : Add MaintPLUNoMenuReplace() at Prog.3 entry
* Jan-18-00:01.00.00    :hrkato     : Saratoga
*
*** GenPOS
* Oct-11-17 : 02.02.02 : R.Chambers   : initialize variables when defined, localize variables.
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

#include "ecr.h"
#include "uie.h"
#include "fsc.h"
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "uifprog.h"
#include "uifpgequ.h"                       

#include "uifpgex.h"                        /* Unique Extern Header for UI */

/* Define Key Sequence */

UISEQ FARCONST  aszSeqPG3FSC_P1[] = {FSC_P1, FSC_P2, FSC_SCANNER, 0};

/*
*=============================================================================
**  Synopsis: SHORT UifPG3Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.3 Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG3Init1[] = {{UifPG3IssueRpt, CID_PG3ISSUERPT},
                                    {UifPG3EnterPage, CID_PG3ENTERPAGE},
                                    {UifPG3EnterAddr, CID_PG3ENTERADDR},
                                    {UifPG3EnterData1, CID_PG3ENTERDATA1},
                                    {NULL,             0                }};

/* Define Next Function at UIM_INIT from CID_PG3ENTERDATA1 */

UIMENU FARCONST aChildPG3Init2[] = {{UifPG3EnterData2, CID_PG3ENTERDATA2},
                                    {NULL,             0                }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildPG3Accept[] = {{UifPG3EnterPage, CID_PG3ENTERPAGE},
                                     {UifPG3EnterAddr, CID_PG3ENTERADDR},
                                     {UifPG3EnterData1, CID_PG3ENTERDATA1},
                                     {NULL,             0                }};


SHORT UifPG3Function(KEYMSG *pKeyMsg) 
{
	PARAFSC     FSCData = {0};   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PG3ENTERDATA1:                             /* UieExit() from UifPG3EnterData1() */
            UieNextMenu(aChildPG3Init2);                    /* Open Next Function */
            break;

        default:                                            /* Initialize */
            /* relocate PLU No., 1/26/98 */
            MaintDeptNoMenuReplace();
            MaintPLUNoMenuReplace();
            UieNextMenu(aChildPG3Init1);                    /* Open Next Function */

            /* Display Address 1 of This Function */
            FSCData.uchStatus = 0;                          /* Set W/ Amount Status */
            FSCData.uchPageNo = 1;                          /* Set Page 1 */
            FSCData.uchAddress = 1;                         /* Set Address 1 */
            FSCData.uchMajorClass = CLASS_PARAFSC;          /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_INIT;     /* Set Minor Class */
            MaintFSCRead(&FSCData);                         /* Execute Read FSC Procedure */
            break;
        }
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildPG3Accept);                       /* Open Next Function */
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                      /* execute Default Procedure */
        break;
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG3IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program No.3 Issue Report 
*===============================================================================
*/
SHORT UifPG3IssueRpt(KEYMSG *pKeyMsg) 
{
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */
        if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P3:
            MaintFSCReport();                   /* Execute Report Procedure */
            UieExit(aPGEnter);                  /* Return to UifPGEnter() */
            return(SUCCESS);

        /* default:
              break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG3EnterPage(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.3 Enter Page Mode 
*===============================================================================
*/
SHORT UifPG3EnterPage(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	PARAFSC     FSCData = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P4);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P4:
            /* Check Digit */
            if ((pKeyMsg->SEL.INPUT.lData > FSC_MAX_PAGE) ||
                ((pKeyMsg->SEL.INPUT.lData < FSC_MIN_PAGE) &&
                 (pKeyMsg->SEL.INPUT.uchLen != 0))) {/* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            FSCData.uchStatus = 0;                              /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                FSCData.uchStatus |= MAINT_WITHOUT_DATA;        /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                FSCData.uchPageNo = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read FSC Procedure */
            FSCData.uchMajorClass = CLASS_PARAFSC;              /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_PAGE;         /* Set MInor Class */

            if ((sError = MaintFSCRead(&FSCData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG3Function() */
            }
            return(sError);
            
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
**  Synopsis: SHORT UifPG3EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.3 Enter Address Mode 
*===============================================================================
*/
SHORT UifPG3EnterAddr(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	PARAFSC     FSCData = {0};   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                        /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.lData > 255L) {              /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            FSCData.uchStatus = 0;                              /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                FSCData.uchStatus |= MAINT_WITHOUT_DATA;        /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                FSCData.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read FSC Procedure */
            FSCData.uchMajorClass = CLASS_PARAFSC;              /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_ADDRESS;      /* Set MInor Class */

            if ((sError = MaintFSCRead(&FSCData)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG3Function() */
            }
            return(sError);
            
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
**  Synopsis: SHORT UifPG3EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
**  Description: Program No.3 Enter Major FSC Data   
*===============================================================================
*/
/* Define Parent Function */

UIMENU FARCONST aParentPG3Func[] = {{UifPG3Function, CID_PG3FUNCTION},
                                    {NULL,           0              }};

SHORT UifPG3EnterData1(KEYMSG *pKeyMsg) 
{
    SHORT       sError;
	PARAFSC     FSCData = {0};   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            FSCData.uchStatus = 0;                                  /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                FSCData.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                FSCData.uchMajorFSCData= ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Edit FSC Procedure */
            FSCData.uchMajorClass = CLASS_PARAFSC;                  /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_MAJOR;            /* Set Minor Class */
            if (FSCData.uchMajorFSCData + UIE_FSC_BIAS == FSC_KEYED_PLU) {
                UieCtrlDevice(UIE_ENA_SCANNER);
            }
            if ((sError = MaintFSCEdit(&FSCData)) == SUCCESS) {     /* Minor FSC not Exist Case */
                MaintFSCWrite();                                    
                UieAccept();                                        /* Return to UifPG3Function() */
                return(SUCCESS);
            } else if (sError == MAINT_FSC_EXIST) {                 /* Extend FSC Exist Case */
                UieExit(aParentPG3Func);                            /* Return to UifPG3Function() */
                return(SUCCESS);
            } else {
                return(sError);                                     /* Set Input Data Error */
            }

        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG3EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*
**  Description: Program No.3 Enter Minor Data  
*===============================================================================
*/
SHORT UifPG3EnterData2(KEYMSG *pKeyMsg) 
{
    UCHAR       uchMajor, uchMinor;
    SHORT       sError;
    PARAFSC     FSCData = {0};   
    USHORT      usLen;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                        /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* Check Digit */
#if 0
            if (pKeyMsg->SEL.INPUT.lData > 0xFFFF) {                /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
#endif
            FSCData.uchStatus = 0;                                  /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                FSCData.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                FSCData.uchMinorFSCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                usLen = (SHORT)pKeyMsg->SEL.INPUT.uchLen;
                if (usLen > PLU_MAX_DIGIT) usLen = PLU_MAX_DIGIT;
                /* Add 2172 Rel1.0 */
                _tcsncpy(FSCData.PluNo.aszPLUNumber, pKeyMsg->SEL.INPUT.aszKey, usLen);
                FSCData.usDeptNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Write FSC Procedure */
            FSCData.uchMajorClass = CLASS_PARAFSC;                  /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_MINOR;            /* Set Minor Class */
            if ((sError = MaintFSCEdit(&FSCData)) == SUCCESS) {     /* Invalid Data Case */     
                MaintFSCWrite();        
                UieAccept();                                        /* Return to UifPG3Function() */
            }
            return(sError);                                         /* Set Success or Input Data Error */
            
        case FSC_P2:
            MaintGetFSC(&uchMajor, &uchMinor);
            
            /* saratoga */
            if (uchMajor == FSC_KEYED_DEPT - UIE_FSC_BIAS) {
                if (pKeyMsg->SEL.INPUT.lData > 0xFF) {                /* Over Digit */
                    return(LDT_KEYOVER_ADR);
                }
                FSCData.uchStatus = 0;                                  /* Set W/ Amount Status */

                if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                    FSCData.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
                } else {                                                /* W/ Amount Input Case */
                    FSCData.uchMinorFSCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                }

                /* Write FSC Procedure */
                FSCData.uchMajorClass = CLASS_PARAFSC;                  /* Set Major Class */
                FSCData.uchMinorClass = CLASS_PARAFSC_MINOR2;            /* Set Minor Class */
                if ((sError = MaintFSCEdit(&FSCData)) == SUCCESS) {     /* Invalid Data Case */     
                    MaintFSCWrite();        
                    UieAccept();                                        /* Return to UifPG3Function() */
                }
                return(sError);                                         /* Set Success or Input Data Error */
            }
            
            if (uchMajor != FSC_KEYED_PLU - UIE_FSC_BIAS) {
                return(LDT_SEQERR_ADR);
            }

            /* Check Digit */
            FSCData.uchStatus = 0;                                  /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                FSCData.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                FSCData.uchMinorFSCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                usLen = (SHORT)pKeyMsg->SEL.INPUT.uchLen;
                if (usLen > PLU_MAX_DIGIT) usLen = PLU_MAX_DIGIT;
                /* Add 2172 Rel1.0 */
                _tcsncpy(FSCData.PluNo.aszPLUNumber, pKeyMsg->SEL.INPUT.aszKey, usLen);
            }

            /* Write FSC Procedure */
            FSCData.uchMajorClass = CLASS_PARAFSC;                  /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_MINOR2;            /* Set Minor Class */
            if ((sError = MaintFSCEdit(&FSCData)) == SUCCESS) {     /* Invalid Data Case */     
                MaintFSCWrite();        
                UieAccept();                                        /* Return to UifPG3Function() */
            }
            return(sError);                                         /* Set Success or Input Data Error */
            
        case FSC_SCANNER:
			//  WARNING: the following few lines of source code could result in a buffer overflow
			//  ERROR    if the scanned in bar code is more than 14 digits.
			//  ERROR    this source really should be doing a label analysis with RflLabelAnalysis()
			//  ERROR    and using struct LABELANALYSIS with the scanned in bar code.
			//
			//           See also functions ItemSalesLabelProc(), ItemOtherLabelProc().
			//
			//           There is also the question of whether this logic should be used in other places
			//           where FSC_SCANNER input with a PLU label should also be used to allow for RSS-14.
			//             Richard Chambers, Oct-16-2017, GenPOS Rel 2.2.2
			//
#pragma message("  *** WARNING: inconsistency with RSS-14 bar code and error with more than 14 digits.\z")
            if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT77/*Changed for RSS14 UIF_DIGIT14*/) ||  /* Over digit */
               (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                return(LDT_KEYOVER_ADR);
            }

            FSCData.uchMinorFSCData = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            /* Add 2172 Rel1.0 */
            _tcsncpy(FSCData.PluNo.aszPLUNumber, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, PLU_MAX_DIGIT);
            FSCData.uchStatus = 0;                                    /* Set W/ Amount Status */
            FSCData.uchStatus |= MAINT_SCANNER_INPUT;
            
            /* Write FSC Procedure */
            FSCData.uchMajorClass = CLASS_PARAFSC;                  /* Set Major Class */
            FSCData.uchMinorClass = CLASS_PARAFSC_MINOR;            /* Set Minor Class */

            if ((sError = MaintFSCEdit(&FSCData)) == SUCCESS) {     /* Invalid Data Case */     
                MaintFSCWrite();        
                UieAccept();                                        /* Return to UifPG3Function() */
            }
            if (sError == SUCCESS) {
                UieCtrlDevice(UIE_DIS_SCANNER);
            }

            return(sError);                                         /* Set Success or Input Data Error */
            
        /* default:
              break; */
        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}

/* --- End of Source File --- */