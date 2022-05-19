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
* Title       : Tax Table #2 Programming Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFAC125.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                       
*               UifAC125Function()      : A/C No.125 Function Entry Mode
*               UifAC125EnterRate()     : A/C No.125 Enter Rate Mode
*               UifAC125EnterAmount1()  : A/C No.125 Enter Amount1 Mode
*               UifAC125EnterTax()      : A/C No.125 Enter Tax Mode
*               UifAC125EnterKey()      : A/C No.125 Enter EC Key Mode
*               UifAC125EnterAmount2()  : A/C No.125 Enter Amount2 Mode
*               UifAC125EnterTest()     : A/C No.125 Enter Test Mode
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

UISEQ FARCONST aszSeqAC125EnterKey[] = {FSC_AC, FSC_EC, FSC_CANCEL, 0};
UISEQ FARCONST aszSeqAC125EnterRate[] = {FSC_AC, FSC_VOID, FSC_CANCEL, 0};



/*
*=============================================================================
**  Synopsis: SHORT UifAC125Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC125Init[] = {{UifAC125EnterRate, CID_AC125ENTERRATE},
                                     {NULL,              0                 }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC125Accept[] = {{UifAC125EnterAmount2, CID_AC125ENTERAMOUNT2},
                                       {NULL,                 0                    }};


SHORT UifAC125Function(KEYMSG *pKeyMsg) 
{


    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:                                          
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC125ENTERKEY:
        case CID_AC125ENTERTEST:
            UieNextMenu(aChildAC125Accept);                 /* Open Next Function */
            usUifACControl = 0;                             /* Clear Control Save Area */
            return(SUCCESS);

        default:
            UieNextMenu(aChildAC125Init);                   /* Open Next Function */
            MaintTaxTblInit(CLASS_MAINTTAXTBL2);             /* Initialize Tax Table Work Area */
            MaintTaxTblReadNo2();                           /* Read Tax Table */
            usUifACControl = 0;                             /* Clear Control Save Area */
            return(SUCCESS);
        }
        /* break */

    case UIM_ACCEPTED:                                          
        UieNextMenu(aChildAC125Accept);                     /* Open Next Function */
        usUifACControl = 0;                                 /* Clear Control Save Area */
        return(SUCCESS);

    default:
        return(UifACDefProc(pKeyMsg));                      /* execute Default Procedure */
    }
}


/*
*=============================================================================
**  Synopsis: SHORT UifAC125EnterRate(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Enter Rate Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC125EnterRate[] = {{UifAC125EnterAmount1, CID_AC125ENTERAMOUNT1},
                                          {NULL,                 0                    }};


SHORT UifAC125EnterRate(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;
    MAINTREDISP     ReDisplay;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:                                                      
        UieOpenSequence(aszSeqAC125EnterRate);                      /* Register Key Sequence */
        return(SUCCESS);
                                                                    
    case UIM_ACCEPTED:
        UieAccept();                                                        /* Return to UifAC125EnterFunction() */
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
                    MaintTaxTblDeleteNo2();
                    UieExit(aACEnter);                                      /* Return to UifACEnter() */
                    return(SUCCESS);
                } else {                                                    /* W/ Amount Case */
                    return(LDT_SEQERR_ADR);
                }
            }

            /* Check W/ Amount */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount */
                MaintTaxTblReportNo2();                                     /* Execute Report Function */
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
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL2;                  
            if ((sError = MaintTaxRateEditNo2(&MaintTaxTbl)) == SUCCESS) {
                UieNextMenu(aChildAC125EnterRate);                                                 
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
**  Synopsis: SHORT UifAC125EnterAmount1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Enter Amount1 Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC125EnterAmount1[] = {{UifAC125EnterTax, CID_AC125ENTERTAX},
                                             {NULL,             0                }};


SHORT UifAC125EnterAmount1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
                            


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC125EnterRate);                      /* Register Key Sequence */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieAccept();                                                        /* Return to UifAC125EnterRate() */
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
            
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL2;                 /* Set Major Class */
            if ((sError = MaintTaxableAmountEditNo2(&MaintTaxTbl)) == SUCCESS) { 
                UieNextMenu(aChildAC125EnterAmount1);                       /* Open Next Function() */
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
**  Synopsis: SHORT UifAC125EnterTax(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Enter Rate1 Mode 
*===============================================================================
*/

    
/* Define Next Function */

UIMENU FARCONST aChildAC125EnterTax[] = {{UifAC125EnterKey, CID_AC125ENTERKEY},
                                         {NULL,             0                }};


SHORT UifAC125EnterTax(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC125EnterKey);
        return(SUCCESS);
 
    case UIM_ACCEPTED:
        UieOpenSequence(aszSeqAC125EnterKey);
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
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL2;

            if ((sError = MaintTaxAmountEditNo2(&MaintTaxTbl)) == SUCCESS) { 
                UieAccept();                                                
            } else if (sError == MAINT_2WDC_OVER) {                         /* Tax Table Over 2WDC Limit */ 
                UieNextMenu(aChildAC125EnterTax);                           /* Open Next Function */
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
**  Synopsis: SHORT UifAC125EnterKey(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Enter Key Mode 
*===============================================================================
*/

/* Define Previous Function */

UIMENU FARCONST aParent125EnterKey[] = {{UifAC125Function, CID_AC125FUNCTION},
                                        {NULL,             0                }};

/* Define Next Function */

UIMENU FARCONST aChildAC125EnterKey[] = {{UifAC125EnterTest, CID_AC125ENTERTEST},
                                         {NULL,              0                 }};
    

SHORT UifAC125EnterKey(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
    MAINTREDISP     ReDisplay;
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC125EnterKey);                               /* Register Key Sequence */
        return(SUCCESS);

    case UIM_ACCEPTED:
        UieAccept();                                                        /* Return to UifAC125EnterTax() */
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
                MaintTaxTblTmpReportNo2();
                UieNextMenu(aChildAC125EnterKey);                           
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
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL2;                 /* Set Major Class */
            if (usUifACControl & UIF_EC) {                                  /* Error Correct Case */
                if ((sError = MaintTaxableAmountErrorNo2(&MaintTaxTbl)) == SUCCESS) {
                    usUifACControl &= ~UIF_EC;                                  /* Reset EC Key Put Status */
        
                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieExit(aParent125EnterKey);                            /* Return to UifAC125Function() */
                }
            } else {
                if ((sError = MaintTaxableAmountEditNo2(&MaintTaxTbl)) == SUCCESS) {
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
**  Synopsis: SHORT UifAC125EnterAmount2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Enter Amount2 Mode 
*===============================================================================
*/
SHORT UifAC125EnterAmount2(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
    MAINTREDISP     ReDisplay;
                            
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC125EnterKey);                       /* Register Key Sequence */
        return(SUCCESS);                                            
        
        case UIM_ACCEPTED:
        UieAccept();                                                /* Return to UifAC125EnterFunction() */
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
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL2;         /* Set Major Class */
            if (usUifACControl & UIF_EC) {                          /* Error Correct Case */
                sError = MaintTaxableAmountErrorNo2(&MaintTaxTbl);
                if (sError == SUCCESS) {
                    usUifACControl &= ~UIF_EC;                      /* Reset EC Key Put Status */

                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieAccept();                                    /* Return to UifAC125Function() */
                }                                                       
            } else {
                sError = MaintTaxableAmountEditNo2(&MaintTaxTbl);
                if (sError == SUCCESS) {
                    UieNextMenu(aChildAC125EnterAmount1);           /* Open Next Function */
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
**  Synopsis: SHORT UifAC125EnterTest(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.125 Enter Test Mode 
*===============================================================================
*/
SHORT UifAC125EnterTest(KEYMSG *pKeyMsg) 
{
    SHORT           sError;
    MAINTTAXTBL     MaintTaxTbl;   
    MAINTREDISP     ReDisplay;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqAC125EnterKey);                               /* Register Key Sequence */
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
                MaintTaxTblWriteNo2();                                    
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
            MaintTaxTbl.uchMajorClass = CLASS_MAINTTAXTBL2;                 /* Set Major Class */
            if (usUifACControl & UIF_EC) {                                  /* Error Correct Case */
                if ((sError = MaintTaxableAmountErrorNo2(&MaintTaxTbl)) == SUCCESS) {
                    usUifACControl &= ~UIF_EC;                              /* Reset EC Key Put Status */

                    /* Reset All Descriptor W/o ALPHA */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLRWOALPH;
                    MaintReDisp(&ReDisplay);
                    UieExit(aParent125EnterKey);                            /* Return to UifAC125Function() */
                }
            } else {
                sError = MaintTaxTblCalNo2(&MaintTaxTbl);
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
