/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Promotional Pricing Item Table Maintenance Module 
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC71.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC71Function()       : A/C No.71 Function Entry Mode
*               UifAC71EnterData1()     : A/C No.71 Enter Data1 Mode
*               UifAC71EnterData2()     : A/C No.71 Enter Data2 Mode
*               UifAC71ErrorCorrect()   : A/C No.71 ErrorCorrect Mode
*               UifAC71CancelFunction() : A/C No.71 Cancel Function Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Nov-28-95: 03.01.00   : M.Ozawa   : initial                             
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
#include <uifpgequ.h>
#include <uifsup.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

static USHORT usLastPPINum = 0;
static USHORT usPPIStartPos = 0;

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */

UISEQ FARCONST aszSeqAC71EnterData1[] = {FSC_VOID, FSC_AC, FSC_CANCEL, 0};
UISEQ FARCONST aszSeqAC71EnterData2[] = {FSC_NUM_TYPE, FSC_AC, FSC_QTY, FSC_SCROLL_DOWN, FSC_SCROLL_UP, 0};
UISEQ FARCONST aszSeqAC71ErrorCorrect[] = {FSC_EC, 0};

UIMENU FARCONST aParentAC71Func[] = {{UifAC71Function, CID_AC71FUNCTION},
                                     {NULL,            0               }};


/*
*=============================================================================
**  Synopsis: SHORT UifAC71Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.71 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC71Init1[] = {{UifAC71EnterData1, CID_AC71ENTERDATA1},
                                     {UifAC71CancelFunction, CID_AC71CANCELFUNCTION},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED from CID_AC71ENTERDATA1 */

UIMENU FARCONST aChildAC71Accept1[] = {{UifAC71EnterData2, CID_AC71ENTERDATA2},
                                       {UifAC71ErrorCorrect, CID_AC71ERRORCORRECT},
                                       {UifAC71CancelFunction, CID_AC71CANCELFUNCTION},
                                       {NULL,              0                 }};

SHORT UifAC71Function(KEYMSG *pKeyMsg) 
{

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        usUifACControl &= ~UIF_VOID;                         /* Set VOID Key Put Status */
        UieNextMenu(aChildAC71Init1);                   /* Open Next Function */
        return(SUCCESS);

    case UIM_REJECT:
        UieNextMenu(aChildAC71Accept1);                   /* Open Next Function */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC71Accept1);                   /* Open Next Function */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC71EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.71 Enter Data1 Mode 
*===============================================================================
*/
    

SHORT UifAC71EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTPPI        PPIData;
    MAINTREDISP     ReDisplay;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UifACPGMLDClear();                              /* Clear MLD */

        MaintDisp(AC_PPI_MAINT,                         /* A/C Number */
                  0,                                    /* Set Minor Class for 10N10D */
                  0,                                    /* Page Number */
                  0,                                    /* PTD Type */
                  0,                                    /* Report Type */
                  0,                                    /* Reset Type */
                  0L,                                   /* Amount Data */
                  LDT_NUM_ADR);                         /* "Number Entry" Lead Through Address */

        UieOpenSequence(aszSeqAC71EnterData1);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_CANCEL:
        if (usUifACControl & UIF_VOID) {                        /* VOID Key Already Put */
            /* Reset All Descriptor W/o ALPHA */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
            MaintReDisp(&ReDisplay);
            usUifACControl &= ~UIF_VOID;                         /* Set VOID Key Put Status */
        }
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
			usPPIStartPos = 0;
            if (!pKeyMsg->SEL.INPUT.uchLen) {       /* W/O Amount case */
                if (!(usUifACControl & UIF_ACCESSED)){         /* Report Out */
                    MaintPPIReport();
                    MaintPPIUnLock();                           /* UnLock PPI File */
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);         /* Execute Finalize Procedure */
                    UieExit(aACEnter);
                    return(SUCCESS);
                }
                MaintPPIUnLock();                           /* UnLock PPI File */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
                UieExit(aACEnter);
                return(SUCCESS);
            }
            /* Check Digit */
			if ((pKeyMsg->SEL.INPUT.lData < A71_CODE_MIN)|| //ESMITH
				(pKeyMsg->SEL.INPUT.lData > A71_CODE_MAX)) {
            //if ((pKeyMsg->SEL.INPUT.lData < 1)||
              //  (pKeyMsg->SEL.INPUT.lData > 255L)) {
                return(LDT_KEYOVER_ADR);
            }
            if (usUifACControl & UIF_VOID) {                        /* VOID Key Already Put */
                PPIData.uchMajorClass = CLASS_MAINTCSTR;        /* Set Major Class */
                PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
                PPIData.uchPPINumber = ( USHORT)pKeyMsg->SEL.INPUT.lData;	//ESMITH
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintPPIDelete(&PPIData)) == SUCCESS) { /* Delete Function Completed */
                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieExit(aParentAC71Func);
                    usUifACControl |= UIF_ACCESSED;                     /* Already Proccessed */
                }
                return(sError);
            }

            PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
            PPIData.uchAddr = 1;                           /* set initial address */
            PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
            PPIData.uchPPINumber = ( USHORT)pKeyMsg->SEL.INPUT.lData; //ESMITH
			//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            if ((sError = MaintPPIRead(&PPIData, 1)) == SUCCESS) {             /* Read Function Completed */
                UieAccept();                                                /* Return to UifAC71Function() */
				usLastPPINum = PPIData.uchPPINumber;
            }
            usUifACControl |= UIF_ACCESSED;                     /* Already Proccessed */
            return(sError);

        case FSC_VOID:
            /* Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            if (usUifACControl & UIF_VOID) {       /* VOID Key Already Put */
                return(LDT_SEQERR_ADR);
            }
            usUifACControl |= UIF_VOID;                         /* Set VOID Key Put Status */
            /* Set VOID Descriptor */

            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
            MaintReDisp(&ReDisplay);
/*            UieReject(); */
            return(SUCCESS);
        
        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MaintPPIUnLock();                           /* UnLock PPI File */
            if (uchMaintOpeCo){
                if (!(uchMaintOpeCo & MAINT_WRITECOMPLETE)) {
                    MaintMakeAbortKey();                     /* "ABORTED" Print */
                }
            }
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC71EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.71 Enter Data2 Mode 
*===============================================================================
*/

SHORT UifAC71EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT       sError;
    MAINTPPI    PPIData;   

	SHORT		ppiLength = MaintPPICheckLength(); // PPI Enhancement - CSMALL

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC71EnterData2);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
		case FSC_SCROLL_UP:
			if (usPPIStartPos == 20 || usPPIStartPos == 0) // page 2 to 1, or page 1 to 1 (top of scroll page)	-	PPI Enhancement - CSMALL
			{
				usPPIStartPos = 0;
				PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
				PPIData.uchAddr = 1;                           /* set initial address */
				PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
				PPIData.uchPPINumber = ( USHORT)usLastPPINum;
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
				if ((sError = MaintPPIRead(&PPIData, 1)) == SUCCESS) {             /* Read Function Completed */
					UieAccept();                                                /* Return to UifAC71Function() */
				}

			} else if(usPPIStartPos == 40) // page 3 to 2
			{
				usPPIStartPos = 20;
				PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
				PPIData.uchAddr = 21;                           /* set initial address */
				PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
				PPIData.uchPPINumber = ( USHORT)usLastPPINum;
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
				if ((sError = MaintPPIRead(&PPIData, 2)) == SUCCESS) {             /* Read Function Completed */
					UieAccept();                                                /* Return to UifAC71Function() */
				}

			} else // default to page 1
			{
				usPPIStartPos = 0;
				PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
				PPIData.uchAddr = 1;                           /* set initial address */
				PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
				PPIData.uchPPINumber = ( USHORT)usLastPPINum;
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
				if ((sError = MaintPPIRead(&PPIData, 1)) == SUCCESS) {             /* Read Function Completed */
					UieAccept();                                                /* Return to UifAC71Function() */
				}

			}
            usUifACControl |= UIF_ACCESSED;                     /* Already Proccessed */
            return(sError);

		case FSC_SCROLL_DOWN:
			if (usPPIStartPos == 0)		// PPI Enhancement - CSMALL
			{
				usPPIStartPos = 20;
				PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
				PPIData.uchAddr = 20;                           /* set initial address */
				PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
				PPIData.uchPPINumber = ( USHORT)usLastPPINum;
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
				if ((sError = MaintPPIRead(&PPIData,2)) == SUCCESS) {             /* Read Function Completed */
					UieAccept();                                                /* Return to UifAC71Function() */
				}

			} else if (usPPIStartPos == 20)
			{
				usPPIStartPos = 40;
				PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
				PPIData.uchAddr = 40;                           /* set initial address */
				PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
				PPIData.uchPPINumber = ( USHORT)usLastPPINum;
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
				if ((sError = MaintPPIRead(&PPIData,3)) == SUCCESS) {             /* Read Function Completed */
					UieAccept();                                                /* Return to UifAC71Function() */
				}

			} else if (usPPIStartPos == 40) // if on screen 3 'Additional Settings' then do not scroll down
			{
				UieAccept();
			} else			// default to page 1
			{
				usPPIStartPos = 20;
				PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
				PPIData.uchAddr = 20;                           /* set initial address */
				PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
				PPIData.uchPPINumber = ( USHORT)usLastPPINum;
				//PPIData.uchPPINumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
				if ((sError = MaintPPIRead(&PPIData,1)) == SUCCESS) {             /* Read Function Completed */
					UieAccept();                                                /* Return to UifAC71Function() */
				}

			}

            usUifACControl |= UIF_ACCESSED;                     /* Already Proccessed */
            return(sError);
        case FSC_NUM_TYPE:                                                  /* Address Key Case */
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            PPIData.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                PPIData.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                PPIData.uchAddr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Edit PPI Procedure */
            
            PPIData.uchMajorClass = CLASS_PARAPPI;                      /* Set Major Class */
            PPIData.uchMinorClass = CLASS_PARAPPI_MAINT_READ;           /* Set Minor Class */

            if (usPPIStartPos != 40)	// PPI Enhancement - CSMALL
			{
				if ((sError = MaintPPIEdit(&PPIData)) == SUCCESS) {
					UieReject();
					return(SUCCESS);
				}

			} else {
				if ((sError = MaintPPIEditAddtlSettings(&PPIData)) == SUCCESS) {
					UieReject();
					return(SUCCESS);
				}
			}

            return(sError);

        case FSC_AC:                                                        /* Enter Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                /* Execute Write Procedure */
                
				if ( (usPPIStartPos == 40) || (ppiLength < 20 && usPPIStartPos == 20) )		// PPI Enhancement - CSMALL
				{
					if ((sError = MaintPPIWriteAddtlSettings()) == SUCCESS) {                                            
						UifACPGMLDClear();                              /* Clear MLD */
						UieExit(aParentAC71Func);                               /* Return to UifAC33Function() */
					}
				} else {
					if ((sError = MaintPPIWrite()) == SUCCESS) {                                            
						UifACPGMLDClear();                              /* Clear MLD */
						UieExit(aParentAC71Func);                               /* Return to UifAC33Function() */
					}
				}
                return(sError);
            } else {                                                        /* W/ Amount Case */

				if ( (usPPIStartPos == 40) || (usPPIStartPos == 20 && ppiLength < 20) )	// PPI Enhancement - CSMALL
				{
					/* Check Digit */
					PPIData.uchStatus = 0;                                  /* Initialize Status */ 
					PPIData.uchMajorClass = CLASS_PARAPPI;                  /* Set Major Class */
					PPIData.uchMinorClass = CLASS_PARAPPI_MAINT_PRICE_WRITE;      /* Set Minor Class */
					PPIData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;
					if ((sError = MaintPPIEditAddtlSettings(&PPIData)) == SUCCESS) {
						UieReject();
						return(SUCCESS);
					}

				} else {

					/* Check Digit */
					PPIData.uchStatus = 0;                                  /* Initialize Status */ 
					PPIData.uchMajorClass = CLASS_PARAPPI;                  /* Set Major Class */
					PPIData.uchMinorClass = CLASS_PARAPPI_MAINT_PRICE_WRITE;      /* Set Minor Class */
					PPIData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;
					if ((sError = MaintPPIEdit(&PPIData)) == SUCCESS) {
						UieReject();
						return(SUCCESS);
					}

				}
				return(sError);
            }

        case FSC_QTY:                                                       /* QTY Key Case */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                return(LDT_SEQERR_ADR);

            } else {                                                        /* W/ Amount Case */
                PPIData.uchStatus = 0;                                  /* Initialize Status */ 
                PPIData.uchMajorClass = CLASS_PARAPPI;                  /* Set Major Class */
                PPIData.uchMinorClass = CLASS_PARAPPI_MAINT_QTY_WRITE;      /* Set Minor Class */
                PPIData.ulInputData = (USHORT)pKeyMsg->SEL.INPUT.lData;
                if ((sError = MaintPPIEdit(&PPIData)) == SUCCESS) {
                    UieReject();
                    return(SUCCESS);
                }
                return(sError);
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC71ErrorCorrect(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.71 Error Correct Mode 
*===============================================================================
*/

SHORT UifAC71ErrorCorrect(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC71ErrorCorrect);                              /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_EC:                                                        /* Enter Key Case */
            if (pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                return(LDT_SEQERR_ADR);
            } else {                                                        /* W/ Amount Case */
                UifACPGMLDClear();                              /* Clear MLD */
                UieExit(aParentAC71Func);                                   /* Return to UifAC71Function */
                return(SUCCESS);
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifAC71CancelFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No. 71 Exit Function Mode 
*===============================================================================
*/
    

SHORT UifAC71CancelFunction(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_CANCEL);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }
            MaintPPIUnLock();                           /* UnLock PPI File */
            UifACPGMLDClear();                              /* Clear MLD */
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);            /* Execute Finalize Procedure */
            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
 
/**** End of File ****/
