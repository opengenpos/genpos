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
* Title       : Entry Module                         
* Category    : User Interface For Program, NCR 2170 US Hospitality Application         
* Program Name: UIFPGMOD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifPGMode()         : Program Entry Mode  
*               UifPGModeIn()       : Program Mode In  
*               UifPGEnter()        : Program Enter Mode
*               UifPGDefProc()      : Program Default Mode
*               UifPGChangeKB1()    : Change Alpha KB to Numeric KB Mode 1
*               UifPGChangeKB2()    : Change Alpha KB to Numeric KB Mode 2
*               UifPGShift()        : Execute Shift Key Operation Mode
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Aug-09-93:00.00.01    : K.You     : initial                                   
* Jan-18-00:01.00.00    : hrkato    : Saratoga
* Dec-19-02:            :R.Chambers : SR 15 for Software Security
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2  R.Chambers Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
#include <intrin.h>

#include "ecr.h"
#include "log.h"
#include "uie.h"
#include "fsc.h"
#include "pif.h"
#include "pifmain.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "csstbstb.h"
#include "plu.h"
#include "csstbfcc.h"
#include "uic.h"
#include "maint.h"
#include "mld.h"
#include "mldmenu.h"
#include "display.h"
#include "appllog.h"
#include "uifprog.h"
#include "uifsup.h"
#include "uifpgequ.h"
#include "cvt.h"                            /* Key Code Table */
#include "uifpgex.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

static USHORT          usUifPGControl;                 /* Control Data Save Area */
static SHORT           husUifPGHandle;                 /* UIC Handle Save Area */

static UCHAR           uchUifPGMenuNumber = 0;         /* Program Mode Menu Number (0~1) */
static UCHAR           uchUifPGMenuMore = 0;           /* Indicates that more pages follow this page. */
static CONST MLDMENU * *     PGCurrentMenu = 0;

UIFPGDIADATA    UifPGDiaWork;                   /* Dialog Work Area,    Saratoga */

static VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* Add R3.0 */

UCHAR UifPGMenuBack(UCHAR uchPageNumber) ;

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

/* Define UifPGEnter() Function */

UIMENU CONST aPGEnter[] = {{UifPGEnter,CID_PGENTER},         
                              {NULL,      0          }};

/* Define Key Sequence */

UISEQ CONST  aszSeqPGFSC_P1[] = {FSC_P1, FSC_P2, 
                    FSC_SCROLL_UP, FSC_SCROLL_DOWN, FSC_SCANNER, 0}; /* Saratoga */
UISEQ CONST  aszSeqPGFSC_P2[] = {FSC_P2, 0};
UISEQ CONST  aszSeqPGFSC_P3[] = {FSC_P3, 0};
UISEQ CONST  aszSeqPGFSC_P4[] = {FSC_P4, 0};
UISEQ CONST  aszSeqPGFSC_P5[] = {FSC_P5, 0};

/*
*=============================================================================
**  Synopsis: SHORT UifPGMode(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     nothing 
*               
**  Description: Program Enter Mode  
*===============================================================================
*/
    
/* Define Next Function */
UIMENU CONST aChildPGMode[] = {{UifPGModeIn,CID_PGMODEIN},
                                  {NULL,       0           }};

/* Define Key Sequence */
UISEQ CONST aszSeqPGMode[] = {FSC_P2, FSC_P5, 0};
UISEQ CONST aszSeqPGEnter[] = {FSC_P1, FSC_P2, FSC_SCROLL_UP, FSC_SCROLL_DOWN, 0};

static void UifSetKeyboardMode_Prog(void)
{
    SYSCONFIG CONST* pSysConfig = PifSysConfig();

    /* set K/B mode to neumeric mode */
    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtProgC);
    }
    else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtProgM);
    }
    else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {     // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtProgTouch);
    }
    else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtProg5932_68);
    }
    else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtProg5932_78);
    }
    else {
        __debugbreak();   // replaced _asm int 3; to allow ARM compiles for Windows on Arm
    }
}

SHORT UifPGMode(KEYMSG *pKeyMsg) 
{
    SHORT       sError = SUCCESS;

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGMode);          /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_NO, UIF_DIGIT7);   /* Stop Echo Back and Set Max Input Digit */
        UieCtrlDevice(UIE_DIS_WAITER);          /* set disable input device */
        MldRefresh();                           /* destroy previous windows */
        MaintPreModeInPrg();                    /* Execute Program Mode In Procedure */

        UieNextMenu(aChildPGMode);              /* Open Next Function */
        break;

    case UIM_QUIT:
        if (husUifPGHandle) {
            if (UicResetFlag(husUifPGHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
            }
            husUifPGHandle = 0;
        }
        MldRefresh();
        MldSetDescriptor(MLD_PRECHECK_SYSTEM);      /* change descriptor to register mode */
        UieCtrlDevice(UIE_DIS_MSR|UIE_DIS_SCANNER);                 /* disable MSR R3.1 */
        break;

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P5:
            return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 
        }
		// fall through for default processing.
    default:
        sError = UifPGDefProc(pKeyMsg);
        break;
    }
    return(sError);
}
/*
*=============================================================================
**  Synopsis: SHORT UifPGModeIn(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program Mode In    
*===============================================================================
*/
SHORT UifPGModeIn(KEYMSG *pKeyMsg) 
{
    SHORT       sError = SUCCESS;
	MAINTMODEIN ModeIn = {0};

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        if (pKeyMsg->SEL.usFunc == 0) {
            UieOpenSequence(aszSeqPGMode);          /* Register Key Sequence */
        } else {
            MaintShrTermUnLock();
            UieNextMenu(aPGEnter);
        }
        break;

    case UIM_REJECT:
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P5:
            return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 

        case FSC_P2:
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT4) {       /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            if ((husUifPGHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {  /* check transaction lock *//* ### MOD (2172 Rel1.0) */
                husUifPGHandle = 0;
                return(LDT_LOCK_ADR);
            }
            ModeIn.uchStatus = 0;               /* Set W/ Amount Status */

            if (!pKeyMsg->SEL.INPUT.uchLen) {               /* W/o Amount Input Case */
                ModeIn.uchStatus |= MAINT_WITHOUT_DATA;     /* Set W/o Amount Status */
            } else {                                        /* W/ Amount Input Case */
                ModeIn.ulModeInNumber = ( ULONG)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Program Mode In Procedure */
            if ((sError = MaintModeInPrg(&ModeIn)) == SUCCESS) {    
                MldSetDescriptor(MLD_SUPER_MODE);      /* change descriptor to super mode */
                MldSetMode(MLD_SUPER_MODE);                     /* MLD Set Program Mode */
                UieNextMenu(aPGEnter);                          /* Open Next Function */
            } else {
                if (UicResetFlag(husUifPGHandle) != UIC_SUCCESS) {  /* reset transaction lock */
                    PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                }
                husUifPGHandle = 0;
            }
            return(sError);
        }
		// fall through for default processing.
    default:
        sError = UifPGDefProc(pKeyMsg);         /* Execute Default Procedure */
        break;
    }
    return(sError);

}


/*
*=============================================================================
**  Synopsis: SHORT UifPGEnter(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Program Enter Mode 
*===============================================================================
*/

/* Define Next Function */

UIMENU CONST aChildPG1Func[] = {{UifPG1Function,CID_PG1FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG2Func[] = {{UifPG2Function,CID_PG2FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG3Func[] = {{UifPG3Function,CID_PG3FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG6Func[] = {{UifPG6Function,CID_PG6FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG7Func[] = {{UifPG7Function,CID_PG7FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG8Func[] = {{UifPG8Function,CID_PG8FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG9Func[] = {{UifPG9Function,CID_PG9FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG10Func[] = {{UifPG10Function,CID_PG10FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG15Func[] = {{UifPG15Function,CID_PG15FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG17Func[] = {{UifPG17Function,CID_PG17FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG18Func[] = {{UifPG18Function,CID_PG18FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG20Func[] = {{UifPG20Function,CID_PG20FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG21Func[] = {{UifPG21Function,CID_PG21FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG22Func[] = {{UifPG22Function,CID_PG22FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG23Func[] = {{UifPG23Function,CID_PG23FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG39Func[] = {{UifPG39Function,CID_PG39FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG46Func[] = {{UifPG46Function,CID_PG46FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG47Func[] = {{UifPG47Function,CID_PG47FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG48Func[] = {{UifPG48Function,CID_PG48FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG49Func[] = {{UifPG49Function,CID_PG49FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG67Func[] = {{UifPG67Function,CID_PG67FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG50Func[] = {{UifPG50Function,CID_PG50FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG51Func[] = {{UifPG51Function,CID_PG51FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG54Func[] = {{UifPG54Function,CID_PG54FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG57Func[] = {{UifPG57Function,CID_PG57FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG60Func[] = {{UifPG60Function,CID_PG60FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG61Func[] = {{UifPG61Function,CID_PG61FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG62Func[] = {{UifPG62Function,CID_PG62FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG65Func[] = {{UifPG65Function,CID_PG65FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG95Func[] = {{UifPG95Function,CID_PG95FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};
UIMENU CONST aChildPG99Func[] = {{UifPG99Function,CID_PG99FUNCTION},{UifPGExit,CID_PGEXIT},{NULL,0}};


SHORT UifPGEnter(KEYMSG *pKeyMsg) 
{
	SYSCONFIG CONST  *pSysConfig = PifSysConfig(); 
    SHORT             sError = SUCCESS;
	MAINTENT          Ent = {0};
    MAINTREDISP       ReDisplay = {0};
	PARATERMINALINFO  TermInfo = {0};
                
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        if (pKeyMsg->SEL.usFunc) {                      /* In case of 1st initialize, "usFunc" is 0 */
            MaintShrTermUnLock();                       /* Terminal Unlock */
        }
        MaintInit();                                    /* Clear Maint/Report Work */
        UieOpenSequence(aszSeqPGEnter);                 /* Register Key Sequence */
        UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT7);  /* Start Echo Back and Set Input Digit */
        UieModeChange(UIE_ENABLE);                      /* Enable Mode Change */

        /* Reset All Descriptor */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);
        uchUifPGMenuNumber = 0;  /* Initial Menu Number */
        /* Display Entry */
        if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
            UifPGMenu(uchUifPGMenuNumber);
        }

        MaintDisp(0, 0, 0, 0, 0, 0, 0L, 0);

        /* print out pc i/f reset log, V3.3 */
        if (uchResetLog != 0) {
            if(MaintResetLog(uchResetLog) == SUCCESS) {
                uchResetLog = 0;
            }
        }
        /* --- Print Power Down Log,    Saratoga --- */
        if (uchPowerDownLog != 0) {
            if(MaintPowerDownLog(uchPowerDownLog) == SUCCESS) {
                uchPowerDownLog = 0;
            }
        }
        break;

    case UIM_INPUT:
        /* Check Digit */
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_SCROLL_UP:
            /* Scroll Up */
            if (pKeyMsg->SEL.INPUT.uchLen) {   /* With Amount Case */
                return(LDT_SEQERR_ADR);
            }

            if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
                if (CliCheckAsMaster() != STUB_SUCCESS) {   /* Sattelite */
                    UieReject();
                    return(SUCCESS);
                }

                if(uchUifPGMenuNumber > 0) {   /* Previous Menu Page */
                    uchUifPGMenuNumber = UifPGMenuBack(uchUifPGMenuNumber);
                }
            }
            UieReject();
            return(SUCCESS);

        case FSC_SCROLL_DOWN:
            /* Scroll Down */
            if (pKeyMsg->SEL.INPUT.uchLen) {   /* With Amount Case */
                return(LDT_SEQERR_ADR);
            }

            if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
                if(uchUifPGMenuMore) {   /* Next Menu Page */
                    uchUifPGMenuNumber = uchUifPGMenuMore;
					UifPGMenu(uchUifPGMenuNumber);
                }
            }
            UieReject();
            return(SUCCESS);

        case FSC_P5:
            return(LDT_SEQERR_ADR);                     /* Return Sequence Error */ 

        case FSC_P1:
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            Ent.uchStatus = 0;                          /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {           /* W/o Amount Input Case */
                Ent.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                    /* W/ Amount Input Case */
                Ent.usSuperNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }

            /* Execute Enter Program No. Procedure */
            if ((sError = MaintEntPrg(&Ent)) == SUCCESS) {  /* Open Next Function By Program No. */
                /* check if terminal lock is successful */
                if (MaintShrTermLockProg(Ent.usSuperNo) != SUCCESS) {
                                                            /* In case of SYSTEM BUSY */
                    UieExit(aChildPGMode);                  /* Return to UifACModeIn() */
                    return(SUCCESS);
                }

                UieCtrlDevice(UIE_ENA_SCANNER); /* set scanner as enable, 2172 */
                UifACPGMLDClear();        /* Menu Clear */

                switch (Ent.usSuperNo) {
                case PG_MDC:                                /* MACHINE DEFINITION CODE */
                    UieNextMenu(aChildPG1Func);    
                    break;

				case PG_FLX_MEM:                            /* FLEXIBLE MEMORY ASSIGNMENT */
                     UieNextMenu(aChildPG2Func); 
                     break;

				case PG_FSC:                                /* FSC FOR REGULAR KEYBOARD */
					/* SR 474 JHHJ, added this so that the user cannot edit FSC
					when we are on a touch screen terminal.  The user does not need
					to change the FSC's for a touch screen because they are set to specific location
					so that FrameworkWndButton uses them properly*/
					TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
					ParaTerminalInfoParaRead (&TermInfo);
					if(TermInfo.TerminalInfo.uchMenuPageDefault >= STD_TOUCH_MENU_PAGE)
					{
						UieExit(aChildPGMode);	
						return(LDT_PROHBT_ADR);
					}
                     UieNextMenu(aChildPG3Func); 
                     break;

				case PG_SEC_PROG:                           /* SECURITY NUMBER FOR PROGRAM MODE */
                     UieNextMenu(aChildPG6Func); 
                     break;

				case PG_STOREG_NO:                          /* STORE/REGISTER No. */
                     UieNextMenu(aChildPG7Func); 
                     break;

				case PG_SUPLEVEL:                           /* SUPERVISOR NUMBER/LEVEL ASSIGNMENT */
                     UieNextMenu(aChildPG8Func); 
                     break;

                case PG_ACT_SEC:                            /* ACTION CODE SECURITY */
                     UieNextMenu(aChildPG9Func); 
                     break;

                case PG_TRAN_HALO:                          /* TRANSACTION HALO */
                     UieNextMenu(aChildPG10Func); 
                     break;

                case PG_PRESET_AMT:                         /* PRESET AMOUNT CASH TENDER */
                     UieNextMenu(aChildPG15Func); 
                     break;

                case PG_HOURLY_TIME:                        /* HOURLY ACTIVITY TIME */
                     UieNextMenu(aChildPG17Func);
                     break;

                case PG_SLIP_CTL:                           /* SLIP PRINTER FEED CONTROL */
                     UieNextMenu(aChildPG18Func);
                     break;

                case PG_TRAN_MNEMO:                         /* TRANSACTION MNEMONICS */
                     UieNextMenu(aChildPG20Func);
                     break;

                case PG_LEAD_MNEMO:                         /* LEAD THROUGH MNEMONICS */
                     UieNextMenu(aChildPG21Func);
                     break;

                case PG_RPT_NAME:                           /* REPORT NAME */
                     UieNextMenu(aChildPG22Func);
                     break;

                case PG_SPEC_MNEMO:                         /* SPECIAL MNEMONICS */
                     UieNextMenu(aChildPG23Func);
                     break;

                case PG_PCIF:                               /* PC INTERFACE */
                     UieNextMenu(aChildPG39Func);
                     break;

                case PG_ADJ_MNEMO:                          /* ADJECTIVE MNEMONICS */
                     UieNextMenu(aChildPG46Func);
                     break;

                case PG_PRT_MNEMO:                          /* PRINT MODIFIER MNEMONICS */
                     UieNextMenu(aChildPG47Func);
                     break;

                case PG_MAJDEPT_MNEMO:                      /* MAJOR DEPARTMENT MNEMONICS */
                     UieNextMenu(aChildPG48Func);
                     break;

                case PG_AUTO_KITCH:                         /* AUTO ALTERNATIVE KITCHEN PRINTER ASSIGNMENT */
                     UieNextMenu(aChildPG49Func);
                     break;

				case PG_SHR_PRT:                            /* ASSIGNMENT TERMINAL NO.INSTALLING SHARED PRINTER */
                     UieNextMenu(aChildPG50Func);
                     break;

                case PG_KDS_IP:                            /* KDS IP ADDRESS ASSIGNMENT, 2172 */
                     UieNextMenu(aChildPG51Func);
                     break;

                case PG_HOTELID:                            /* HOTEL ID AND SLD */
                     UieNextMenu(aChildPG54Func);
                     break;

				case PG_CHA24_MNEMO:                        /* 24 CHRACTERS MNEMONICS */
                     UieNextMenu(aChildPG57Func);
                     break;

                case PG_TTLKEY_TYP:                         /* TOTAL KEY TYPE ASSIGNMENT */
                     UieNextMenu(aChildPG60Func);
                     break;

				case PG_TTLKEY_CTL:                         /* TOTAL KEY CONTROL */
                     UieNextMenu(aChildPG61Func);
                     break;

                case PG_TEND_PARA:                         /* TENDER PARAMETER, V3.3 */
                     UieNextMenu(aChildPG62Func);
                     break;

                case PG_MLD_MNEMO:                         /* MLD MNEMONICS, V3.3 */
                     UieNextMenu(aChildPG65Func);
                     break;

				case PG_HOSTS_IP:                          /* HOSTS IP ADDRESS, 2172 */
                    UieNextMenu(aChildPG99Func);
                     break;

                case PG_UNLOCK_NO:                          /* SECURITY UNLOCK CODE */
                    UieNextMenu(aChildPG95Func);
                     break;

				case PG_AUTO_CPN:
					 UieNextMenu(aChildPG67Func);
					 break;

                default:
                     break;
                }
                UieModeChange(UIE_DISABLE);                 /* Disable Mode Change */
            }
            return(sError);
        }
		// fall through for default processing.
    default:
        sError = UifPGDefProc(pKeyMsg);                     /* Execute Default Procedure */
        break;
    }
    return(sError);

    
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifPGExit(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Exit Program Number    
*===============================================================================
*/
SHORT UifPGExit(KEYMSG *pKeyMsg) 
{
    SYSCONFIG CONST     *pSysConfig = PifSysConfig();
    SHORT               sError = SUCCESS;
	MAINTREDISP         ReDisplay = {0};

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P5);          /* Register Key Sequence */
        break;

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P5:
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ Amount ?  */
                return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 
            }
            MaintFin(CLASS_MAINTTRAILER_PRTPRG);    /* Execute Finalize Procedure */
            usUifPGControl &= ~UIF_SHIFT_PAGE;      /* Reset Shift Page Bit */

#if 1
            // use a single function to see the keyboard mode to Numeric mode
            // we want single place for this code for future changes.
            //    Richard Chambers, Apr-06-2023
            UifSetKeyboardMode_Prog();
#else 
            /* Recover KB Mode to Numeric Mode */
			if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {       // 7448 Conventional Type
                UieSetCvt(CvtProgC);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
                UieSetCvt(CvtProgM);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {     // Touchscreen terminal uses Micromotion tables
                UieSetCvt(CvtProgTouch);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
                UieSetCvt(CvtProg5932_68);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
                UieSetCvt(CvtProg5932_78);                         
			} else {
				_asm int 3;
			}
#endif
            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT7);              /* Start Echo Back and Set Input Digit */

            /* Reset Descriptor */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);

            UieExit(aPGEnter);                      /* Return to UifPGEnter() */
            return(SUCCESS);
        }
		// fall through for default processing.
    default:
        sError = UifPGDefProc(pKeyMsg);         /* Execute Default Procedure */
        break;
    }
    return(sError);
}


/*
*=============================================================================
**  Synopsis: SHORT UifPGDefProc(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS : Successful 
*               
**  Description: Program Default Mode 
*===============================================================================
*/
SHORT UifPGDefProc(KEYMSG *pKeyMsg) 
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();     /* get system config */
	MAINTREDISP     ReDisplay = {0};
   
    switch (pKeyMsg->uchMsg) {
/*
    case UIM_REDISPLAY:
        MaintReDisp();                               ReDisplay Previous Date
        return(SUCCESS); 
*/
    case UIM_CANCEL:
        return(SUCCESS);

    case UIM_QUIT:
        if (pPifSaveFarData != NULL) {               /* Simulator */
            (*pPifSaveFarData)();                           
        } 
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P5:                
            if (pKeyMsg->SEL.INPUT.uchLen) {        /* W/ Amount ?  */
                return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 
            }
            MaintFin(CLASS_MAINTTRAILER_PRTPRG);    /* Execute Finalize Procedure */

            if (pPifSaveFarData != NULL) {               /* save parameter, saratoga */
                (*pPifSaveFarData)();                           
            } 

            usUifPGControl &= ~UIF_SHIFT_PAGE;      /* Reset Shift Page Bit */

#if 1
            // use a single function to see the keyboard mode to Numeric mode
            // we want single place for this code for future changes.
            //    Richard Chambers, Apr-06-2023
            UifSetKeyboardMode_Prog();
#else 
            /* Recover KB Mode to Numeric Mode */
            if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {       // 7448 Conventional Type
                UieSetCvt(CvtProgC);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
                UieSetCvt(CvtProgM);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {     // Touchscreen terminal uses Micromotion tables
                UieSetCvt(CvtProgTouch);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
                UieSetCvt(CvtProg5932_68);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
                UieSetCvt(CvtProg5932_78);                         
			} else {
				_asm int 3;
			}
#endif

            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT7);              /* Start Echo Back and Set Input Digit */

            /* Reset Descriptor */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);
			if(pSysConfig->uchKBMode != UIF_NEUME_KB)
			{
				UieSetKBMode (UIF_NEUME_KB);
			}

            UieExit(aPGEnter);                      /* Return to UifPGEnter() */
            return(SUCCESS);

        case FSC_RECEIPT_FEED:
            MaintFeedRec(MAINT_RECKEYDEF_FEED);     /* Execute Receipt Feed */
            UieKeepMessage();                       /* Keep Input Data */
            return(SUCCESS);

        case FSC_JOURNAL_FEED:
            MaintFeedJou();                         /* Execute Journal Feed */
            UieKeepMessage();                       /* Keep Input Data */
            return(SUCCESS);

        /* print out pc i/f reset log, V3.3 */
        case FSC_RESET_LOG:                         // in UifPGDefProc(): call MaintResetLog() to issue PCIF reset log.  see IspWriteResetLog()
            if (uchResetLog != 0) {
                if(MaintResetLog(uchResetLog) == SUCCESS) {
                    uchResetLog = 0;
                }
            }
            return(SUCCESS);

        /* --- Print Power Down Log,    Saratoga --- */
        case FSC_POWER_DOWN:
            if (uchPowerDownLog != 0) {
                uchPowerDownLog = pKeyMsg->SEL.INPUT.uchMinor;
                if (MaintPowerDownLog(uchPowerDownLog) == SUCCESS) {
                    uchPowerDownLog = 0;
                }
            }
            return(SUCCESS);
        }
		// fall through for default processing.
    default:
        return(UieDefProc(pKeyMsg));                /* Execute Uie Default Procedure */
    }
}

/*
*=============================================================================
**  Synopsis: SHORT UifPGChangeKB1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Change Alpha KB to Numeric KB Mode 1 
*===============================================================================
*/
SHORT UifPGChangeKB1(KEYMSG *pKeyMsg) 
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();     /* get system config */
	MAINTREDISP     ReDisplay = {0};
                    
    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P4);            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
		/* Check Digit */
		if (pKeyMsg->SEL.INPUT.uchLen) {                /* W/ Amount Case */
			return(LDT_SEQERR_ADR);
		}
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
		case FSC_P4:
#if 1
            // use a single function to see the keyboard mode to Numeric mode
            // we want single place for this code for future changes.
            //    Richard Chambers, Apr-06-2023
            UifSetKeyboardMode_Prog();
#else 
            /* Set KB Mode to Neumeric Mode */
			if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {       // 7448 Conventional Type
				UieSetCvt(CvtProgC);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
				UieSetCvt(CvtProgM);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {     // Touchscreen terminal uses Micromotion tables
				UieSetCvt(CvtProgTouch);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
				UieSetCvt(CvtProg5932_68);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
				UieSetCvt(CvtProg5932_78);                         
			} else {
				_asm int 3;
			}
#endif

			UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT7);              /* Start Echo Back and Set Input Digit */
	        
			ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
			ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
			MaintReDisp(&ReDisplay);
	        
			UieAccept();                                                /* Return to UifPG20Function() */
			return(SUCCESS);
		}
		// fall through for default processing.
    default:
        return(UifPGDefProc(pKeyMsg));              /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: SHORT UifPGChangeKB2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Change Alpha KB to Numeric KB Mode 2  
*===============================================================================
*/
SHORT UifPGChangeKB2(KEYMSG *pKeyMsg) 
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();     /* get system config */
	MAINTREDISP     ReDisplay = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P4);                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
		/* Check Digit */
		if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount Case */
			return(LDT_SEQERR_ADR);
		}
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
		case FSC_P4:
#if 1
            // use a single function to see the keyboard mode to Numeric mode
            // we want single place for this code for future changes.
            //    Richard Chambers, Apr-06-2023
            UifSetKeyboardMode_Prog();
#else 
            /* Set KB Mode to Neumeric Mode */
			if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {    /* Conventional Type */
				UieSetCvt(CvtProgC);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
				UieSetCvt(CvtProgM);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {	   // Touchscreen terminal uses Micromotion tables
				UieSetCvt(CvtProgTouch);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
				UieSetCvt(CvtProg5932_68);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
				UieSetCvt(CvtProg5932_78);                         
			} else {
				_asm int 3;
			}
#endif

			UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT7);              /* Start Echo Back and Set Input Digit */
	        
			ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
			ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
			MaintReDisp(&ReDisplay);
	        
			UieAccept();                                                /* Return to UifPG20Function() */
			return(SUCCESS);
		}
		// fall through for default processing.
    default:
        return(UifPGDefProc(pKeyMsg));                  /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: SHORT UifPGShift(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Execute Shift Key Operation Mode 
*===============================================================================
*/
SHORT UifPGShift(KEYMSG *pKeyMsg) 
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();     /* get system config */

	switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqPGFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
		switch (pKeyMsg->SEL.INPUT.uchMajor) {
		case FSC_P2:
			/* Set KB Mode to Neumeric Mode */
			if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION){		/* Conventional */
				if (usUifPGControl & UIF_SHIFT_PAGE) {                  /* Now is Shift Page Case */
					UieSetCvt(CvtAlphC1);                               /* Set Standard Alpha Conventional KB Table */
					usUifPGControl &= ~UIF_SHIFT_PAGE;                  /* Reset Shift Page Bit */
				} else {
					UieSetCvt(CvtAlphC2);                               /* Set Shift Alpha Conventional KB Table */      
					usUifPGControl |= UIF_SHIFT_PAGE;                   /* Set Shift Page Bit */
				}
				UieKeepMessage();                                       /* Keep Input Data for UI Engine */
				return(SUCCESS);
			} else if(pSysConfig->uchKeyType == KEYBOARD_WEDGE_68){ /* Wedge 64 type */
				if (usUifPGControl & UIF_SHIFT_PAGE) {                  /* Now is Shift Page Case */
					UieSetCvt(CvtAlph5932_68_1);                               /* Set Standard Alpha Conventional KB Table */
					usUifPGControl &= ~UIF_SHIFT_PAGE;                  /* Reset Shift Page Bit */
				} else {
					UieSetCvt(CvtAlph5932_68_2);                               /* Set Shift Alpha Conventional KB Table */      
					usUifPGControl |= UIF_SHIFT_PAGE;                   /* Set Shift Page Bit */
				}
				UieKeepMessage();                                       /* Keep Input Data for UI Engine */
				return(SUCCESS);
			} else if(pSysConfig->uchKeyType == KEYBOARD_WEDGE_78){ /* Wedge 78 type */
				if (usUifPGControl & UIF_SHIFT_PAGE) {                  /* Now is Shift Page Case */
					UieSetCvt(CvtAlph5932_78_1);                               /* Set Standard Alpha Conventional KB Table */
					usUifPGControl &= ~UIF_SHIFT_PAGE;                  /* Reset Shift Page Bit */
				} else {
					UieSetCvt(CvtAlph5932_78_2);                               /* Set Shift Alpha Conventional KB Table */      
					usUifPGControl |= UIF_SHIFT_PAGE;                   /* Set Shift Page Bit */
				}
				UieKeepMessage();                                       /* Keep Input Data for UI Engine */
				return(SUCCESS);
			} else {													/* Micromotion Type */
				return(LDT_SEQERR_ADR);                         
			}
		}
		// fall through for default processing.
    default:
        return(UifPGDefProc(pKeyMsg));                              /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: VOID UifPGMenu(UCHAR MenuPageNumber) 
*
*       Input:  MenuPageNumber  :    MenuPageNumber
*       Output: nothing
*
**  Return: nothing
*               
**  Description: Program Mode Code Menu Display
*===============================================================================
*/
VOID UifPGMenu(UCHAR uchPageNumber) 
{
	SYSCONFIG CONST * pSysConfig = PifSysConfig(); 
	PARATERMINALINFO TermInfo = {0};

	/* SR 474 JHHJ, added this so that the user cannot edit FSC
	when we are on a touch screen terminal.  The user does not need
	to change the FSC's for a touch screen because they are set to specific location
	so that FrameworkWndButton uses them properly.
	This code removes the MLD for Program 3*/
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);

	if(TermInfo.TerminalInfo.uchMenuPageDefault < STD_TOUCH_MENU_PAGE)
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			PGCurrentMenu = PGEntryMenu + uchPageNumber;
		} else {
			PGCurrentMenu = PGEntryMenuSatt + uchPageNumber;
		}
	} else
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			PGCurrentMenu = PGEntryMenuTouch + uchPageNumber;
		} else {
			PGCurrentMenu = PGEntryMenuSattTouch + uchPageNumber;
		}
	}

	uchUifPGMenuMore = UifACPGMLDDispCom(PGCurrentMenu);
	if (uchUifPGMenuMore)
		uchUifPGMenuMore += uchPageNumber;
}

UCHAR UifPGMenuBack(UCHAR uchPageNumber) 
{
	SYSCONFIG CONST * pSysConfig = PifSysConfig(); 
	PARATERMINALINFO TermInfo = {0};

	/* SR 474 JHHJ, added this so that the user cannot edit FSC
	when we are on a touch screen terminal.  The user does not need
	to change the FSC's for a touch screen because they are set to specific location
	so that FrameworkWndButton uses them properly.
	This code removes the MLD for Program 3*/
	TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);

	if (uchPageNumber > 0)
		uchPageNumber--;

	if(TermInfo.TerminalInfo.uchMenuPageDefault < STD_TOUCH_MENU_PAGE)
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			while (uchPageNumber > 0 && PGEntryMenu[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			PGCurrentMenu = PGEntryMenu + uchPageNumber;
		} else {
			while (uchPageNumber > 0 && PGEntryMenuSatt[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			PGCurrentMenu = PGEntryMenuSatt + uchPageNumber;
		}
	} else
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			while (uchPageNumber > 0 && PGEntryMenuTouch[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			PGCurrentMenu = PGEntryMenuTouch + uchPageNumber;
		} else {
			while (uchPageNumber > 0 && PGEntryMenuSattTouch[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			PGCurrentMenu = PGEntryMenuSattTouch + uchPageNumber;
		}
	}

	uchUifPGMenuMore = UifACPGMLDDispCom(PGCurrentMenu);
	if (uchUifPGMenuMore)
		uchUifPGMenuMore += uchPageNumber;

	return uchPageNumber;
}
/*
*===========================================================================
** Synopsis:    SHORT   UifPGDiaMnem(UIFPGDIADATA *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Customer Name Entry.                            Saratoga
*===========================================================================
*/
UIMENU CONST aChildPGDiaMnem[] = {{UifPGDiaMnemEntry, CID_DIAPGMNEM}, {NULL, 0}};/* Saratoga */

SHORT   UifPGDiaMnem(UIFPGDIADATA *pData)
{
    SYSCONFIG CONST     *pSysConfig = PifSysConfig();
    SHORT               sRetStatus;
    USHORT              usCtrlDevice;

    /* set K/B mode to alpha mode */
    UieEchoBack(UIE_ECHO_ROW0_AN, 20);

    /* Reset Single/Double Wide Position */
    /*    UieEchoBackWide(255, 0);*/
    //UieEchoBackWide(1, 19);                                     /* RFC, 01/30/2001 */
    
    if (pSysConfig->uchOperType == DISP_2X20) {
        UieEchoBackWide(1, 19);
    } else if (pSysConfig->uchOperType == DISP_LCD) {
        UieEchoBackWide(1,39);   // LCD
    }
    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtAlphC1);                                   /* set standard conventional K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {     // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_68_1);                                   /* set 5932 Wedge 64 K/B table */
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_78_1);                                   /* set 5932 Wedge 78 K/B table */
	} else {
        __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}

/*    UieSetFsc(NULL);                                            * set FSC table pointer to NULL */
    UieCreateDialog();                                          /* create dialog */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    UieInitMenu(aChildPGDiaMnem);

    for (;;) {
		KEYMSG   Data = {0};

        UieGetMessage(&Data, 0x0000);                                   /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);                   /* check key */
        if (Data.SEL.INPUT.uchMajor == FSC_P5) {                /* cancel ? */
            break;
        }
        if (sRetStatus == UIPG_DIA_SUCCESS) {
            *pData = UifPGDiaWork;
            sRetStatus = SUCCESS;       
            break;
        }
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieDeleteDialog();                                          /* delete dialog */
    
#if 1
    // use a single function to see the keyboard mode to Numeric mode
    // we want single place for this code for future changes.
    //    Richard Chambers, Apr-06-2023
    UifSetKeyboardMode_Prog();
#else 
    /* set K/B mode to neumeric mode */
    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtProgC);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtProgM);                         
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {     // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtProgTouch);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtProg5932_68);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtProg5932_78);                         
	} else {
		_asm int 3;
	}
#endif

/*    UieSetFsc((FSCTBL FAR *)&PGFsc);      * set FSC table pointer */
    UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT7);
    return(sRetStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT UifPGDiaMnemEntry(KEYMSG *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Customer Name Entry.
*===========================================================================
*/
SHORT   UifPGDiaMnemEntry(KEYMSG *pData)
{
    switch (pData->uchMsg) {
    case UIM_INPUT:
        switch (pData->SEL.INPUT.uchMajor) {
        case FSC_P1:
            /* check double wide */
            if (pData->SEL.INPUT.uchLen == 0) {
                return(LDT_KEYOVER_ADR);
            }
/*          if (strchr(pData->SEL.INPUT.aszKey, 0x12) != 0) {        double wide exist
                return(LDT_KEYOVER_ADR);
            }
*/          memset(&UifPGDiaWork, '\0', sizeof(UifPGDiaWork));
            UifPGDiaWork.auchFsc[0] = pData->SEL.INPUT.uchMajor;
            UifPGDiaWork.auchFsc[1] = pData->SEL.INPUT.uchMinor;
            _tcsncpy(UifPGDiaWork.aszMnemonics, pData->SEL.INPUT.aszKey, pData->SEL.INPUT.uchLen);
            return(UIPG_DIA_SUCCESS);

        case FSC_P5:
            if (pData->SEL.INPUT.uchLen != 0) {             /* exist input data ? */
                return(LDT_SEQERR_ADR);
            }
            return(UIPG_ABORT);

        case FSC_RECEIPT_FEED:
        case FSC_JOURNAL_FEED:
        case FSC_RESET_LOG:							/* avoid key sequence error by PCIF function, 11/12/01 */
        case FSC_POWER_DOWN:
            return(UifPGDefProc(pData));

        default:                                        
            return(LDT_SEQERR_ADR);
        }

    case UIM_REDISPLAY:
    case UIM_DRAWER:
        return(UieDefProc(pData));                  /* engine default process */

    case UIM_INIT:
    case UIM_CANCEL:
        break;

    default:                                        /* not use */
        return(LDT_SEQERR_ADR);
    }

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifPGDiaClear(UIFPGDIADATA *pData)
*     Input:    nothing
*     Output:   pData - line number buffer pointer
*     InOut:    nothing
*
** Return:      success = 0
*               cancel  = -2
*
** Description: Request Clear key.                              Saratoga
*===========================================================================
*/
SHORT   UifPGDiaClear(VOID)
{
    USHORT  usModeStatus;       /* mode lock enable/disable bit save area */
    USHORT  usCtrlDevice;       /* control device save area       */
    SHORT   sRetStatus;         /* return status save area */

    UieCreateDialog();
    usModeStatus = UieModeChange(UIE_DISABLE);
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER | UIE_DIS_WAITER);
    UieEchoBack(UIE_ECHO_ROW0_REG, 0);

    for (;;) {
		KEYMSG  Data = {0};               /* key message data */

        UieGetMessage(&Data, 0x0000);
        switch (Data.uchMsg) {
        case UIM_INPUT:
            if (Data.SEL.INPUT.uchMajor == FSC_CLEAR) {
                sRetStatus = SUCCESS;
                break;
            } else if (Data.SEL.INPUT.uchMajor == FSC_P5) {
                sRetStatus = UIPG_ABORT;
                break;
            } else if (Data.SEL.INPUT.uchMajor == FSC_RECEIPT_FEED || Data.SEL.INPUT.uchMajor == FSC_JOURNAL_FEED) {
                UifPGDefProc(&Data);
            } else if (Data.SEL.INPUT.uchMajor == FSC_RESET_LOG || Data.SEL.INPUT.uchMajor == FSC_POWER_DOWN) {
				/* avoid key sequence error by PCIF function, 11/12/01 */
				UifPGDefProc(&Data);
            } else {
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);
            }
            continue;

        case UIM_ERROR:
            UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);

        case UIM_REDISPLAY:
            UieDefProc(&Data);

        default:
            continue;
        }
        break;
    }

    UieEchoBack(UIE_ECHO_ROW0_REG, UIF_DIGIT7);
    UieCtrlDevice(usCtrlDevice);
    UieModeChange(usModeStatus);
    UieDeleteDialog();

    return(sRetStatus);
}


/**** End of File ****/