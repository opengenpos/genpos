/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : UI for setting HOTEL ID                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application
* Program Name: UIFPG54.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPG54Function()   : Program No.54 Entry Mode
*               UifPG54EnterALPHA() : Program No.54 Enter Mnemonics Data Mode
*               UifPG54IssueRpt()   : Program No.54 Issue Report
*               UifPG54EnterAddr()  : Program No.54 Enter Address Mode
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Sep-30-93:00.00.01    :M.Yamamoto : initial                                   
*          :            :           :
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
#include <pif.h>
#include <uie.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifprog.h>
#include <uifpgequ.h>                       
#include <cvt.h>                            /* Key Code Table */
#include "rfl.h"
#include "uifpgex.h"                        /* Unique Extern Header for UI */

static UCHAR uchUifpgAdr;                   /* memory for type # */

/*
*=============================================================================
**  Synopsis: SHORT UifPG54Function(KEYMSG *pKeyMsg) 
*               
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               
**  Description: Program No.54 Entry Mode  
*===============================================================================
*/

/* Define Next Function at UIM_INIT */

UIMENU FARCONST aChildPG54Init[] = {{UifPGShift, CID_PGSHIFT},
                                    {UifPG54EnterALPHA, CID_PG54ENTERALPHA},
                                    {UifPGChangeKB1, CID_PGCHANGEKB1},
                                    {NULL,           0              }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB1 */

UIMENU FARCONST aChildPG54Accept1[] = {{UifPG54IssueRpt, CID_PG54ISSUERPT},
                                       {UifPG54EnterAddr, CID_PG54ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PGCHANGEKB2 */

UIMENU FARCONST aChildPG54Accept2[] = {{UifPG54EnterAddr, CID_PG54ENTERADDR},
                                       {NULL,             0                }};


/* Define Next Function at UIM_ACCEPTED from CID_PG54ENTERALPHA,CID_PG54ENTERADDR */

UIMENU FARCONST aChildPG54Accept3[] = {{UifPGShift, CID_PGSHIFT},
                                       {UifPG54EnterALPHA, CID_PG54ENTERALPHA},
                                       {UifPGChangeKB2, CID_PGCHANGEKB2},
                                       {NULL,           0              }};

SHORT UifPG54Function(KEYMSG *pKeyMsg) 
{

    PARAHOTELID  HotelId;

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        /* Display Address 1 of This Function */

        HotelId.uchStatus = 0;                             /* Set W/ Amount Status */
        uchUifpgAdr = HotelId.uchAddress = 1;               /* Set Address 1 */
        HotelId.uchMajorClass = CLASS_PARAHOTELID;         /* Set Major Class */
        MaintHotelIdRead(&HotelId);                        /* Execute Read Adjective Mnemonics Procedure */

        UieNextMenu(aChildPG54Init);                       /* Open Next Function */
        break;

    case UIM_ACCEPTED:
        switch (pKeyMsg->SEL.usFunc) {
        case CID_PGCHANGEKB1:
            UieNextMenu(aChildPG54Accept1);                /* Open Next Function */
            break;

        case CID_PGCHANGEKB2:
            UieNextMenu(aChildPG54Accept2);                /* Open Next Function */
            break;

        default:                                                /* CID_PG54ENTERALPHA */
            UieNextMenu(aChildPG54Accept3);                     /* CID_PG54ENTERADDR */
            break;
        }
        break;

    default:
        return(UifPGDefProc(pKeyMsg));                          /* execute Default Procedure */
    }
    return(SUCCESS);
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG54EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.54 Enter Mnemonics Data Mode
*===============================================================================
*/
    


SHORT UifPG54EnterALPHA(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAHOTELID     HotelId;   
    UCHAR           uchI;


    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P1);                        /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW1_AN, MAX_HOTELID_SIZE);        /* Define Display Format and Max. Input Data */

        /* Set KB Mode to ALPHA Mode */

        UifACChgKBType(UIF_ALPHA_KB);
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P1:
            HotelId.uchStatus = 0;                             /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                  /* W/o Amount Input Case */
                HotelId.uchStatus |= MAINT_WITHOUT_DATA;       /* Set W/o Amount Status */
            } else {                                           /* W/ Amount Input Case */
                if (pKeyMsg->SEL.INPUT.aszKey[0] == 0x12) { /* W wide Character */
                    return (LDT_KEYOVER_ADR);
                }
                memset(HotelId.aszMnemonics,                   /* Full NULL to Own Buffer */
                       '\0',
                       sizeof(HotelId.aszMnemonics));
                if (uchUifpgAdr == HOTEL_PDT_ADR) {
                    tcharnset(HotelId.aszMnemonics,               /* Full 0 to Own Buffer */
                           _T('0'),
                           (PARA_PDT_LEN));
                }
                if (uchUifpgAdr == HOTEL_ID_ADR) {
                    if (pKeyMsg->SEL.INPUT.uchLen > PARA_HOTELID_LEN) {
                        return (LDT_KEYOVER_ADR);
                    } 
                    memmove(HotelId.aszMnemonics,
                        &pKeyMsg->SEL.INPUT.aszKey,
                        MAX_HOTELID_SIZE * sizeof(TCHAR));
                } else if (uchUifpgAdr == HOTEL_APVL_CODE_ADR) {
                    if (pKeyMsg->SEL.INPUT.uchLen > PARA_APVL_CODE_LEN) {
                        return (LDT_KEYOVER_ADR);
                    } 
                    memmove(HotelId.aszMnemonics,
                        &pKeyMsg->SEL.INPUT.aszKey,
                        MAX_HOTELID_SIZE * sizeof(TCHAR));
                } else {
                    for (uchI = 0; uchI < pKeyMsg->SEL.INPUT.uchLen; uchI++) {
                        if (pKeyMsg->SEL.INPUT.aszKey[uchI] < '0' || 
                            pKeyMsg->SEL.INPUT.aszKey[uchI] > '9') {
                            return LDT_KEYOVER_ADR;
                        }
                    }
                    if (uchUifpgAdr == HOTEL_SLD_ADR) {
                        if (pKeyMsg->SEL.INPUT.lData > 9999 ||
                            pKeyMsg->SEL.INPUT.lData < 0) {
                            return (LDT_KEYOVER_ADR);
                        }
                        memmove(HotelId.aszMnemonics,
                               &pKeyMsg->SEL.INPUT.aszKey,
                               PARA_SLD_LEN * sizeof(TCHAR));
                    } else if (uchUifpgAdr == HOTEL_PDT_ADR) {
                        if (pKeyMsg->SEL.INPUT.uchLen > 10 ) {
                            return (LDT_KEYOVER_ADR);
                        }
                        memmove(HotelId.aszMnemonics,
                               &pKeyMsg->SEL.INPUT.aszKey,
                               pKeyMsg->SEL.INPUT.uchLen  * sizeof(TCHAR));
                    } else if (HOTEL_TEND1_ADR <= uchUifpgAdr || 
                           uchUifpgAdr <= HOTEL_TEND20_ADR) {
                        if (pKeyMsg->SEL.INPUT.lData > 9 ||
                            pKeyMsg->SEL.INPUT.lData < 0) {
                            return (LDT_KEYOVER_ADR);
                        }
                        memmove(HotelId.aszMnemonics,
                               &pKeyMsg->SEL.INPUT.aszKey,
                               PARA_TEND_LEN * sizeof(TCHAR));
                    }
                } 
            }

            /* Execute Write Tansaction Mnemonic Procedure */
            
            /* HotelId.uchMajorClass = CLASS_PARAHOTELID;         Set Major Class */

            if ((sError = MaintHotelIdWrite(&HotelId)) == SUCCESS) {
                uchUifpgAdr++;
                if (uchUifpgAdr > HOTELID_ADR_MAX) {
                    uchUifpgAdr = 1;
                }
                UieAccept();                                        /* Return to UifPG54Function() */
            }
            return(sError);                                        

        /* default:
              break; */

        }
        /* break; */

    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPG54IssueRpt(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.54 Issue Report 
*===============================================================================
*/
    

SHORT UifPG54IssueRpt(KEYMSG *pKeyMsg) 
{
    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P3);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */

        if (pKeyMsg->SEL.INPUT.uchLen) {            /* W/ Amount Case */
            return(LDT_KEYOVER_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
            case FSC_P3:
                MaintHotelIdReport();              /* Execute Report Procedure */
                UieExit(aPGEnter);                  /* Return to UifPGEnter() */
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
**  Synopsis: SHORT UifPG54EnterAddr(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: Program No.54 Enter Address Mode 
*===============================================================================
*/
SHORT UifPG54EnterAddr(KEYMSG *pKeyMsg) 
{
    
    SHORT           sError;
    PARAHOTELID    HotelId;   

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Check Digit */

            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {           /* Over Digit */
               return(LDT_KEYOVER_ADR);
            }

            HotelId.uchStatus = 0;                                  /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                HotelId.uchStatus |= MAINT_WITHOUT_DATA;            /* Set W/o Amount Status */
            } else {                                                /* W/ Amount Input Case */
                uchUifpgAdr = HotelId.uchAddress = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Read Adjective Mnemonics Procedure */
            
            HotelId.uchMajorClass = CLASS_PARAHOTELID;              /* Set Major Class */

            if ((sError = MaintHotelIdRead(&HotelId)) == SUCCESS) {
                if (!pKeyMsg->SEL.INPUT.uchLen) {                       /* W/o Amount Input Case */
                    uchUifpgAdr++;
                    if (uchUifpgAdr > HOTELID_ADR_MAX) {
                        uchUifpgAdr = 1;
                    }
                } else {                                                /* W/ Amount Input Case */
                    uchUifpgAdr = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
                }
                UieAccept();                                        /* Return to UifPG54Function() */
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




    
    
    
