/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Beverage Dispenser Parameter Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application        
* Program Name: UIFAC137.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifAC137Function()      : A/C No.137 Function Entry Mode
*               UifAC137EnterAddr()     : A/C No.137 Enter Address Mode 
*               UifAC137EnterData1()    : A/C No.137 Enter Data Mode 1
*               UifAC137EnterData2()    : A/C No.137 Enter Data Mode 2
*               UifAC137EnterData3()    : A/C No.137 Enter Data Mode 3
*                                     
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Feb-02-95:03.01.00    : M.Ozawa   : initial
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
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>

#include "uifsupex.h"                       /* Unique Extern Header for UI */

/*
*=============================================================================
**  Synopsis: SHORT UifAC137Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               
**  Description: A/C No.137 Function Entry Mode  
*===============================================================================
*/


/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildAC137Init1[] = {{UifAC137EnterAddr, CID_AC137ENTERADDR},
                                    {UifAC137EnterData1, CID_AC137ENTERDATA1},
                                    {NULL,             0             }};

/* Define Next Function at UIM_INIT from CID_AC137ENTERDATA1 */

UIMENU FARCONST aChildAC137Init2[] = {{UifAC137EnterData2, CID_AC137ENTERDATA2},
                                    {NULL,             0             }};

/* Define Next Function at UIM_ACCEPTED */

UIMENU FARCONST aChildAC137Accept[] = {{UifAC137EnterAddr, CID_AC137ENTERADDR},
                                     {UifAC137EnterData3, CID_AC137ENTERDATA3},
                                     {NULL,             0             }};


SHORT UifAC137Function(KEYMSG *pKeyMsg) 
{
    
    PARADISPPARA     ParaDispPara;   

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_AC137ENTERDATA1:                                 /* UieExit() from UifPG2EnterData1() */
        case CID_AC137ENTERDATA3:                                 /* UieExit() from UifPG2EnterData3() */
            UieNextMenu(aChildAC137Init2);                        /* Open Next Function */
            break;

        default:                                                /* Initialize */
            UieNextMenu(aChildAC137Init1);                        /* Open Next Function */

            /* Display Address 1 of This Function */

            ParaDispPara.uchStatus = 0;                          /* Set W/ Amount Status */
            ParaDispPara.usAddress = 1;                          /* Set Address 1 */
            ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;     /* Set Major Class */

            MaintDispParaRead(&ParaDispPara); 
            break;
        }
        break;

    case UIM_ACCEPTED:
        UieNextMenu(aChildAC137Accept);                           /* Open Next Function */
        break;

    default:
        return(UifACDefProc(pKeyMsg));                          /* execute Default Procedure */
        break;
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC137EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               
**  Description:  
*===============================================================================
*/
    

SHORT UifAC137EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARADISPPARA    ParaDispPara;   


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_NUM_TYPE);                          /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_NUM_TYPE:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaDispPara.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaDispPara.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                ParaDispPara.usAddress = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Flexible Memory Procedure */
            
            ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;      /* Set Major Class */

            if ((sError = MaintDispParaRead(&ParaDispPara)) == SUCCESS) {    
                UieAccept();                                    /* Return to UifPG2Function() */
            }
            return(sError);                                     /* Set Success or Input Data Error */
            
        /* default:
              break;  */
    }
    /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                          /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC137EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               LDT_FLFUL_ADR   : File Full
*
**  Description: A/C No.137 Enter Data Mode after UIM_INIT  
*===============================================================================
*/
    

/* Define Parent Function */

UIMENU FARCONST aParentAC137Func[] = {{UifAC137Function, CID_AC137FUNCTION},
                                    {NULL,           0              }};


SHORT UifAC137EnterData1(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARADISPPARA    ParaDispPara;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintDispParaReport();                                      /* Execute Report Procedure */
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.lData > 0xFFFFL) {
                return(LDT_KEYOVER_ADR);
            }
            ParaDispPara.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaDispPara.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                ParaDispPara.usPluNumber= ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Edit Record Procedure */
            
            ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;                /* Set Major Class */
            ParaDispPara.uchMinorClass = CLASS_PARADISPPARA_PLUNO;          /* Set Major Class */

            if ((sError = MaintDispParaEdit(&ParaDispPara)) == SUCCESS) {   /* Adjective Exist Case */
                MaintDispParaWrite(); 
                UieAccept();                                                /* Return to UifAC137Function() */
                return(SUCCESS);
            } else if (sError == MAINT_ADJECTIVE_EXIST) {                     /* Adjective Exist Case */
                UieExit(aParentAC137Func);                                    /* Return to UifAC137Function() */
                return(SUCCESS);
            } else {
                return(sError);                                             /* Set Input Data Error */
            }
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC137EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               LDT_FLFUL_ADR   : File Full
*
**  Description: A/C No.137 Enter Data Mode after UIM_ACCEPTED  
*===============================================================================
*/
    


SHORT UifAC137EnterData2(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARADISPPARA    ParaDispPara;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }
            ParaDispPara.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaDispPara.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                ParaDispPara.uchAdjective = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Write PTD Flag Procedure */
            
            ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;                 /* Set Major Class */
            ParaDispPara.uchMinorClass = CLASS_PARADISPPARA_ADJECTIVE;       /* Set Major Class */

            if ((sError = MaintDispParaEdit(&ParaDispPara)) == SUCCESS) {     /* Invalid Data Case */     
                MaintDispParaWrite();
                UieAccept();                            /* Return to UifAC137Function() */
                return(SUCCESS);
            }
            
        /* default:
              break; */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                      /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC137EnterData3(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADDR : Sequence Error
*               LDT_FLFUL_ADR   : File Full
*
**  Description: A/C No.137 Enter Data Mode after UIM_ACCEPT
*===============================================================================
*/
    

SHORT UifAC137EnterData3(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARADISPPARA    ParaDispPara;   
                    

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            if (!pKeyMsg->SEL.INPUT.uchLen) {                               /* W/o Amount Case */
                MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                UieExit(aACEnter);                                          /* Return to UifACEnter() */
                return(SUCCESS);
            }

            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.lData > 0xFFFFL) {
                return(LDT_KEYOVER_ADR);
            }
            ParaDispPara.uchStatus = 0;                          /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                   /* W/o Amount Input Case */
                ParaDispPara.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                            /* W/ Amount Input Case */
                ParaDispPara.usPluNumber= ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Edit Record Procedure */
            
            ParaDispPara.uchMajorClass = CLASS_PARADISPPARA;                /* Set Major Class */
            ParaDispPara.uchMinorClass = CLASS_PARADISPPARA_PLUNO;          /* Set Major Class */

            if ((sError = MaintDispParaEdit(&ParaDispPara)) == SUCCESS) {   /* Adjective Exist Case */
                MaintDispParaWrite();
                UieAccept();                                                /* Return to UifAC137Function() */
                return(SUCCESS);
            } else if (sError == MAINT_ADJECTIVE_EXIST) {                     /* Adjective Exist Case */
                UieExit(aParentAC137Func);                                    /* Return to UifAC137Function() */
                return(SUCCESS);
            } else {
                return(sError);                                             /* Set Input Data Error */
            }
            
        /* default:
              break;  */
        }
        /* break; */

    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}

/**** End of File ****/
