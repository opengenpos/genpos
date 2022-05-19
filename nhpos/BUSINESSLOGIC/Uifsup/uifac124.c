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
* Title       : Tax Table #1 Programming Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC124.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                       
*               UifAC124Function()      : A/C No.124 Function Entry Mode
*               UifAC124EnterRate()     : A/C No.124 Enter Rate Mode
*               UifAC124EnterAmount1()  : A/C No.124 Enter Amount1 Mode
*               UifAC124EnterTax()      : A/C No.124 Enter Tax Mode
*               UifAC124EnterKey()      : A/C No.124 Enter EC Key Mode
*               UifAC124EnterAmount2()  : A/C No.124 Enter Amount2 Mode
*               UifAC124EnterTest()     : A/C No.124 Enter Test Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jul-13-92:00.00.01    :K.You      : initial                                   
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
#include <pif.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifpgequ.h>
#include <uifsup.h>
#include <cvt.h>
#include <display.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */


/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/


/* Define Key Sequence */

UISEQ FARCONST aszSeqAC124EnterKey[] = {FSC_AC, FSC_EC, FSC_CANCEL, 0};
UISEQ FARCONST aszSeqAC124EnterRate[] = {FSC_AC, FSC_VOID, FSC_CANCEL, 0};



/*
*=============================================================================
**  Synopsis: SHORT UifAC124Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC124Init[] = {{UifAC124EnterRate, CID_AC124ENTERRATE},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC124Accept[] = {{UifAC124EnterAmount2, CID_AC124ENTERAMOUNT2},
                                       {NULL,                 0                    }};


SHORT UifAC124Function(KEYMSG *pKeyMsg) 
{


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:                                          
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC124ENTERKEY:
        case CID_AC124ENTERTEST:
            UieNextMenu(aChildAC124Accept);                 /* Open Next Function */
            usUifACControl = 0;                             /* Clear Control Save Area */
            return(SUCCESS);

        default:
            UieNextMenu(aChildAC124Init);                   /* Open Next Function */
            MaintTaxTblInit(CLASS_MAINTTAXTBL1);            /* Initialize Tax Table Work Area */
            MaintTaxTblReadNo1();                           /* Read Tax Table */
            usUifACControl = 0;                             /* Clear Control Save Area */
            return(SUCCESS);
        }
        /* break */

    case UIM_ACCEPTED:                                          
        UieNextMenu(aChildAC124Accept);                     /* Open Next Function */
        usUifACControl = 0;                                 /* Clear Control Save Area */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                      /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC124EnterRate(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Enter Rate Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC124EnterRate[] = {{UifAC124EnterAmount1, CID_AC124ENTERAMOUNT1},
                                          {NULL,                 0                    }};


SHORT UifAC124EnterRate(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;
    MAINTREDISP     ReDisplay;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:                                                      
        UieOpenSequence(aszSeqAC124EnterRate);                      /* Register Key Sequence */
        return(SUCCESS);
                                                                    
    case UIM_ACCEPTED:
        UieAccept();                                                        /* Return to UifAC124EnterFunction() */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl = 0;                                                 /* Clear Control Save Area */
     
        /* Reset All Descriptor W/o ALPHA */

        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            break;

        case FSC_AC:

            /* Check Delete Procedure */

            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Case */
                    MaintTaxTblDeleteNo1();
                    UieExit(aACEnter);                                      /* Return to UifACEnter() */
                    return(SUCCESS);
                } else {                                                    /* W/ Amount Case */
                    return(LDT_SEQERR_ADR);
                }
            }

            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                MaintTaxTblReportNo1();                                     /* Execute Report Function */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            if ((pKeyMsg->SEL.INPUT.uchLen != UIF_DIGIT6) &&
                (pKeyMsg->SEL.INPUT.uchLen != UIF_DIGIT5)) {                  /* Not 6,5 Digit Input */
                return(LDT_KEYOVER_ADR);                                
            }

            /* Execute Rate Input Procedure */

            MaintTaxTbl.uchStatus = 0;                                       
            MaintTaxTbl.uchInteger = ( UCHAR)(pKeyMsg->SEL.INPUT.lData/10000L);
            MaintTaxTbl.uchNumerator = ( UCHAR)((pKeyMsg->SEL.INPUT.lData
                                                - 10000L*MaintTaxTbl.uchInteger)/100L);
            MaintTaxTbl.uchDenominator = ( UCHAR)(pKeyMsg->SEL.INPUT.lData
                                                  - 10000L*MaintTaxTbl.uchInteger
                                                  - 100L*MaintTaxTbl.uchNumerator);
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL1;                  
            if ((sError = MaintTaxRateEditNo1(&MaintTaxTbl)) == SUCCESS) {
                UieNextMenu(aChildAC124EnterRate);                                                 
            }
            return(sError);
        
        case FSC_VOID:
            /*  Check W/ Amount */

            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
               return(LDT_SEQERR_ADR);
            }
            if (usUifACControl & UIF_VOID) {                                /* VOID Key Already Put */
                return(LDT_SEQERR_ADR);
            } else {
                usUifACControl |= UIF_VOID;                                 /* Set VOID Key Put Status */

                /* Set VOID Descriptor */

                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
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
**  Synopsis: SHORT UifAC124EnterAmount1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Enter Amount1 Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC124EnterAmount1[] = {{UifAC124EnterTax, CID_AC124ENTERTAX},
                                             {NULL,             0                }};


SHORT UifAC124EnterAmount1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
                            


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC124EnterRate);                      /* Register Key Sequence */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieAccept();                                                        /* Return to UifAC124EnterRate() */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;

        case FSC_VOID:
            return(LDT_SEQERR_ADR);

        case FSC_AC:                                                  
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT5) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTaxTbl.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                MaintTaxTbl.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                MaintTaxTbl.ulEndTaxableAmount = ( ULONG)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Input Taxable Amount Procedure */
            
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL1;                 /* Set Major Class */
            if ((sError = MaintTaxableAmountEditNo1(&MaintTaxTbl)) == SUCCESS) { 
                UieNextMenu(aChildAC124EnterAmount1);                       /* Open Next Function() */
            }
            return(sError);

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
**  Synopsis: SHORT UifAC124EnterTax(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Enter Rate1 Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC124EnterTax[] = {{UifAC124EnterKey, CID_AC124ENTERKEY},
                                         {NULL,             0                }};


SHORT UifAC124EnterTax(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC124EnterKey);
        return(SUCCESS);
 
    case UIM_ACCEPTED:
        UieOpenSequence(aszSeqAC124EnterKey);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;

        case FSC_EC:
            return(LDT_SEQERR_ADR);

        case FSC_AC:                                                  
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTaxTbl.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                MaintTaxTbl.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                MaintTaxTbl.usTaxAmount = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Input Tax Amount Procedure */
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL1;

            if ((sError = MaintTaxAmountEditNo1(&MaintTaxTbl)) == SUCCESS) { 
                UieAccept();                                                
            } else if (sError == MAINT_2WDC_OVER) {                         /* Tax Table Over 2WDC Limit */ 
                UieNextMenu(aChildAC124EnterTax);                           /* Open Next Function */
                return(SUCCESS);
            } else if (sError == LDT_FLFUL_ADR) {                           /* File Full Case */
                UieAccept();                                                
            }
            return(sError);

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
**  Synopsis: SHORT UifAC124EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Enter Key Mode 
*===============================================================================
*/

/* Define Previous Function */

UIMENU FARCONST aParent124EnterKey[] = {{UifAC124Function, CID_AC124FUNCTION},
                                        {NULL,             0                }};

/* Define Next Function */

UIMENU FARCONST aChildAC124EnterKey[] = {{UifAC124EnterTest, CID_AC124ENTERTEST},
                                         {NULL,              0                 }};
    

SHORT UifAC124EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
    MAINTREDISP     ReDisplay;
                            

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC124EnterKey);                               /* Register Key Sequence */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieAccept();                                                        /* Return to UifAC124EnterTax() */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl = 0;                                     /* Initialize Uif Control Save Area */
     
        /* Reset All Descriptor W/o ALPHA */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);                                        

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;

        case FSC_AC:                                                  
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                MaintTaxTblTmpReportNo1();
                UieNextMenu(aChildAC124EnterKey);                           
                return(SUCCESS);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT5) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTaxTbl.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                MaintTaxTbl.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                MaintTaxTbl.ulEndTaxableAmount = ( ULONG)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Input Tax Amount Procedure */
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL1;                 /* Set Major Class */
            if (usUifACControl & UIF_EC) {                                  /* Error Correct Case */
                if ((sError = MaintTaxableAmountErrorNo1(&MaintTaxTbl)) == SUCCESS) {
                    usUifACControl &= ~UIF_EC;                                  /* Reset EC Key Put Status */
        
                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieExit(aParent124EnterKey);                            /* Return to UifAC124Function() */
                }
            } else {
                if ((sError = MaintTaxableAmountEditNo1(&MaintTaxTbl)) == SUCCESS) {
                    UieAccept();
                }
            }
            return(sError);

        case FSC_EC:
            /*  Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
               return(LDT_SEQERR_ADR);
            }
            if (usUifACControl & UIF_EC) {                                  /* VOID Key Already Put */
                return(LDT_SEQERR_ADR);                  
            } else {
                usUifACControl |= UIF_EC;                                   /* Set VOID Key Put Status */
              
                /* Set VOID(E/C) Descriptor */
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
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
**  Synopsis: SHORT UifAC124EnterAmount2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Enter Amount2 Mode 
*===============================================================================
*/
SHORT UifAC124EnterAmount2(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
    MAINTREDISP     ReDisplay;
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC124EnterKey);                       /* Register Key Sequence */
        return(SUCCESS);                                            
        
        case UIM_ACCEPTED:
        UieAccept();                                                /* Return to UifAC124EnterFunction() */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl = 0;                                         /* Initialize Uif Control Save Area */                                

        /* Reset All Descriptor W/o ALPHA */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;

        case FSC_AC:                                                  
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT5) {           /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTaxTbl.uchStatus = 0;                              /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                MaintTaxTbl.uchStatus |= MAINT_WITHOUT_DATA;        /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                MaintTaxTbl.ulEndTaxableAmount = ( ULONG)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Input Tax Amount Procedure */
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL1;         /* Set Major Class */
            if (usUifACControl & UIF_EC) {                          /* Error Correct Case */
                sError = MaintTaxableAmountErrorNo1(&MaintTaxTbl);
                if (sError == SUCCESS) {
                    usUifACControl &= ~UIF_EC;                      /* Reset EC Key Put Status */

                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieAccept();                                    /* Return to UifAC124Function() */
                }                                                       
            } else {
                sError = MaintTaxableAmountEditNo1(&MaintTaxTbl);
                if (sError == SUCCESS) {
                    UieNextMenu(aChildAC124EnterAmount1);           /* Open Next Function */
                }                                                       
            }
            return(sError);

        case FSC_EC:
            /*  Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount */
               return(LDT_KEYOVER_ADR);
            }
            if (usUifACControl & UIF_EC) {                          /* EC Key Already Put */
                return(LDT_SEQERR_ADR);                  
            } else {
                usUifACControl |= UIF_EC;                           /* Set EC Key Put Status */
        
                /* Set VOID(E/C) Descriptor */
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
                return(SUCCESS);
            }

        /* default:
            break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifAC124EnterTest(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.124 Enter Test Mode 
*===============================================================================
*/
SHORT UifAC124EnterTest(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
    MAINTREDISP     ReDisplay;
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC124EnterKey);                               /* Register Key Sequence */
        return(SUCCESS);

    case UIM_CANCEL:
        usUifACControl = 0;                                         /* Initialize Uif Control Save Area */                                

        /* Reset All Descriptor W/o ALPHA */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
        MaintReDisp(&ReDisplay);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            MaintMakeAbortKey();
            break;

        case FSC_AC:                                                  
            /* Check W/ Amount */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                MaintTaxTblWriteNo1();                                    
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);                        /* Execute Exit Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT5) {                   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            MaintTaxTbl.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Input Case */
                MaintTaxTbl.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                        /* W/ Amount Input Case */
                MaintTaxTbl.ulEndTaxableAmount = ( ULONG)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Input Tax Amount Procedure */
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL1;                 /* Set Major Class */
            if (usUifACControl & UIF_EC) {                                  /* Error Correct Case */
                if ((sError = MaintTaxableAmountErrorNo1(&MaintTaxTbl)) == SUCCESS) {

                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieExit(aParent124EnterKey);                            /* Return to UifAC124Function() */
                }
            } else {
                sError = MaintTaxTblCalNo1(&MaintTaxTbl);
                UieReject();
            }
            return(sError);

        case FSC_EC:
            /*  Check W/ Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount */
               return(LDT_KEYOVER_ADR);
            }
            if (usUifACControl & UIF_EC) {                                  /* EC Key Already Put */
                return(LDT_SEQERR_ADR);                  
            } else {
                usUifACControl |= UIF_EC;                                   /* Set EC Key Put Status */

                /* Set VOID(E/C) Descriptor */
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_VOID;
                MaintReDisp(&ReDisplay);
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


