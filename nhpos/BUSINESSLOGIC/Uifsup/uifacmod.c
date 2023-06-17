/*
*===========================================================================
* Title       : Entry Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: UIFACMOD.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifACMode()         : Supervisor Entry Mode 
*               UifACModeIn()       : Supervisor Mode In  
*               UifACEnter()        : Supervisor Enter Mode
*               UifACDefProc()      : Supervisor Default Mode
*               UifACChange1KB()    : Change Alpha KB to Numeric KB 1 Mode 
*               UifACChange2KB()    : Change Alpha KB to Numeric KB 2 Mode 
*               UifACShift()        : Execute Shift Key Operation Mode
*               UifACExitFunction() : A/C Exit Mode
*               UifACChgKBType()    : Change KB Type
*               UifACInit();        : Clear Own Work Area
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. : Name       : Description
* Aug-09-93 : 01.00.12 : K.You      : Initial.
* Aug-23-93 : 01.00.13 : J.IKEDA    : Chg SuperNo from AC_SLIP_PROMOTION
*           :          :            : to AC_SOFTCHK_MSG
* Aug-31-93 : 01.00.14 : M.Suzuki   : Change location of "UicCheckAndSet"
* Sep-27-93 : 02.00.01 : K.You      : Bug Fixed E-90 (Mod. UifACEnter)
* oct-05-93 : 02.00.01 : J.Ikeda    : Adds A/C153 Function
* Nov-15-93 : 02.00.02 : M.Yammaoto : Add A/C78, A/C97
*    -  -   : 03.00.00 :            : Add R3.0 Action Codes
* Oct-30-95 : 03.00.10 : M.Ozawa    : Add UieModeChange() at AC70/122
* Dec-07-95 : 03.01.00 : M.Ozawa    : Add MSR entry at UifACModeIn()
* Aug-11-99 : 03.05.00 : K.Iwata    : R3.5 (remove WAITER_MODEL)
* Aug-17-99 : 03.05.00 : K.Iwata    : Merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* 2171 for Win32
* Sep-06-99 : 01.00.00 : K.Iwata    : V1.0 Initial
* Dec-27-99 : 01.00.00 : hrkato     : Saratoga
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2  R.Chambers   Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
*===========================================================================
*/
/*
		Directions, instructions, and bits of good advice.

	When making changes to the Supervisor and Program Mode menus, you will
	need to do a couple of things.
		- allocate an unused MLD mnemonic for the new menu item (P65)
		- assign a unique number to the action code (AC) or program code (P)
		- create the #define for the MLD in paraequ.h for instance LCD_PG95_ADR
		- add the menu item to the tables below
		- add any new sub-menu items needed to the sub-menu trees or create
		  new sub-menu trees

	How the code works.
	In the function UifACMenu() in file uifacmod.c the code checks to see
	the role of the terminal - Master, Backup Master, or Satellite - and then
	displays the appropriate menu array - ACEntryMenu, ACEntryMenuBMAST, or
	ACEntryMenuSatt respectively.
	
	Each menu array contains one or more entries for a list of menu items, the
	list being a single display page.  The rule of thumb is each of the display
	pages should contain no more than 20 menu items.

	Some of the Supervisor and Program Mode menu items have sub-menus.  Most of
	the report types of menu items, for instance, have a sub-menu allowing the
	user to pick the type of report to generate such as a detailed report or a
	summary report.

	The sub-menus are accessed from the specific menu entry function which in
	turn displays the sub-menu.  For an example take a look at the source code
	supporting the AC26 menu item on the Supervisor Menu.  This code can be
	found in file uifac26.c with the menu function starting with UifAC26Function ().

	As you look at the code you will see how different sub-menus are displayed based
	on where the user is in the menu tree.

	Notice that the function UieNextMenu() is used to go to the next menu state and
	that the function UifACPGMLDDispCom () is used to display the appropriate sub-menu.

 */

/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include "ecr.h"
#include "log.h"
#include "uie.h"
#include "pif.h"
#include "plu.h"
#include "pifmain.h"
#include "fsc.h"
#include "paraequ.h"
#include "para.h"
#include "pararam.h"
#include "csop.h"
#include "csstbpar.h"
#include "csstbstb.h"
#include "csstbfcc.h"
#include "csetk.h"
#include "uic.h"
#include "maint.h"
#include "display.h"

#include "uifsup.h"
#include "mld.h"
#include "uifpgequ.h"
#include "cvt.h"
#include "csttl.h"
#include "report.h"
#include "uireg.h"
#include "appllog.h"
#include "mldsup.h"
#include "mldmenu.h"
#include "uifsupex.h"
#include  "BlFWif.h"
#include "..\Sa\UIE\uieio.h"
#include "pmg.h"
#include "rfl.h"
#include "..\Uifreg\uiregloc.h"

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/
USHORT      usUifACControl;                 /* Control Data Save Area */
UCHAR       uchUifACDataType;               /* Data Type Save Area */
UCHAR       uchUifACRstType;                /* Reset Type Save Area */
UCHAR       uchUifACRptType;                /* Report Type Save Area */
UCHAR       uchUifACDescCtl;                /* Descriptor Control Save Area */
USHORT      usUifACRptGcfNo;                /* GC No. Save Area */
USHORT      usUifACRptTermNo;               /* terminal No. Save Area */
ULONG       ulUifACRptCashierNo;            /* Cashier/Waiter No. Save Area */
SHORT       husUifACHandle;                 /* UIC Handle Save Area */

static UCHAR       uchUifACMenuNumber = 0;         /* AC Mode Menu Number (0~3) */
static UCHAR       uchUifACMenuMore = 0;           /* Indicates that more pages follow this page. */
static CONST MLDMENU * * ACCurrentMenu = 0;

UCHAR       uchUifACRptOnOffMld;    /* MLD Display Report on/off Flug 95/03/22 */
USHORT      uchUifTrailerOnOff;     /* Trailer display mld controll */
UCHAR       uchUifReDispMode;       /* After C-String EXEC. Redisplay Mode Flug */
UCHAR       uchCStringFlug;         /* Control String Execute Flug In ACModeIn */
UCHAR	    uchUifACRptOnOffFile;   /* Print to file on/off Flag */  //ESMITH PRTFILE

static VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* Add R3.0 */

UCHAR UifACMenuBack(UCHAR uchPageNumber) ;

SHORT  ItemCasSecretCode(ULONG *puchSecret);
SHORT  ItemSetCasSecretCode (ULONG ulUifEmployeeId, ULONG ulUifEmployeePin, ULONG *ulUifEmployeePinOld);

/*
==============================================================================
;                      C O M M O N   R O M   T A B L E s                         
;=============================================================================
*/

/* Define UifACEnter() Function */
UIMENU CONST aACEnter[] = {{UifACEnter,CID_ACENTER},         
                           {NULL,      0          }};

/* Define UifACModeIn() Function */
UIMENU CONST aACModeIn[] = {{UifACModeIn,CID_ACMODEIN},         
                            {NULL,        0          }};

/* Define A/C Key Sequence */
UISEQ CONST aszSeqACFSC_AC[] = {FSC_AC, FSC_KEYED_STRING, FSC_STRING, FSC_ASK, FSC_SUPR_INTRVN, FSC_DOC_LAUNCH,  0};  /* Saratoga */
UISEQ CONST aszSeqACFSC_AC_MSR[] = {FSC_AC, FSC_KEYED_STRING,  FSC_ASK,
                                    FSC_STRING, FSC_MSR, 0};
UISEQ CONST aszSeqACFSC_AC_CUR[] = {FSC_AC,          FSC_SCROLL_UP, 
                                    FSC_SCROLL_DOWN, FSC_NUM_TYPE,
                                    FSC_KEYED_STRING,FSC_STRING,
									FSC_FOR, FSC_SUPR_INTRVN, FSC_DOC_LAUNCH,
                                    0};
UISEQ CONST aszSeqACFSC_EC[] = {FSC_EC, 0};
UISEQ CONST aszSeqACFSC_CANCEL[] = {FSC_CANCEL, 0};
UISEQ CONST aszSeqACFSC_P1[] = {FSC_P1, 0};
UISEQ CONST aszSeqACFSC_P2[] = {FSC_P2, 0};
UISEQ CONST aszSeqACFSC_P4[] = {FSC_P4, 0};

UISEQ CONST aszSeqAC23EnterDataType[] = {FSC_NUM_TYPE, FSC_AC, 0};
//GroupReport - CSMALL
UISEQ CONST aszSeqAC31EnterDataType[] = {FSC_NUM_TYPE, FSC_AC, 0};

/*
*=============================================================================
**  Synopsis:    SHORT UifACMode(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Supervisor Entry Mode   
*===============================================================================
*/

/* Define Next Function */
UIMENU CONST aChildACMode[] = {{UifACModeIn,CID_ACMODEIN},
                               {NULL,       0           }};

SHORT UifACMode(KEYMSG *pKeyMsg) 
{
    SHORT       sError = SUCCESS;
	MAINTREDISP ReDisplay = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        MldRefresh();                           /* destroy previous windows */
        MaintPreModeInSup();                    /* Execute Supervisor Mode In Procedure */
        uchUifReDispMode = STS_AC_MODE;         /* Redisplay Mode Set */
        UieSetAbort(UIE_POS3 | UIE_POS4 | UIE_POS5);      /* set abort key enable position */
        UieEchoBack(UIE_ECHO_NO, UIFREG_MAX_ID_DIGITS);   /* Stop Echo Back and Set Input Digit */
        UieCtrlDevice(UIE_DIS_WAITER);          /* set disable input device */
        UieCtrlDevice(UIE_ENA_MSR);             /* enable MSR R3.1 */
        UieCtrlDevice(UIE_ENA_VIRTUALKEY);      // enable Virtual Keyboard such as FreedomPay
        UieNextMenu(aChildACMode);              /* Open Next Function */
        break;

    case UIM_QUIT:
        if (husUifACHandle) {
			SHORT  sResetStatus;

            if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  /* reset transaction lock */
                PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
                PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
                PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
            }
            husUifACHandle = 0;
        }
        MldRefresh();

        /* Reset All Descriptor */
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){ /* MLD Display Out */
            uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);
        }

		/* Reset Print Flag */ //ESMITH PRTFILE
		if(uchUifACRptOnOffFile == RPT_PRINTFILE_ON){ /* Print to file on */
			uchUifACRptOnOffFile = RPT_PRINTFILE_OFF;
			MaintSetPrintMode(PRT_SENDTO_PRINTER);
		}

        MldSetDescriptor(MLD_PRECHECK_SYSTEM);      /* change descriptor to register mode */
        UieSetAbort(UIE_POS3 |UIE_POS4 | UIE_POS5);           /* set abort key enable position */
        UieCtrlDevice(UIE_DIS_MSR|UIE_DIS_SCANNER);                 /* disable MSR R3.1 */
        break;

    case UIM_INPUT:
		if (pKeyMsg->SEL.INPUT.uchMajor == FSC_CANCEL) {
			return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 
        }
		// fall through to do default processing.
    default:
        sError = UifACDefProc(pKeyMsg);
        break;
    }
    return(sError);
}

/*
*==============================================================================
**  Synopsis: SHORT UifACModeIn(KEYMSG *pKeyMsg) 
* 
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**      Return: SUCCESS         : Successful     
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Supervisor Mode In   
*===============================================================================
*/

/* Define Next Function */
UIMENU CONST aChildACModeIn[] = {{UifACEnter,CID_ACENTER},
                                 {NULL,      0          }};

SHORT UifACModeIn(KEYMSG *pKeyMsg) 
{
    SHORT       sError = SUCCESS;
	MAINTMODEIN ModeIn = {0};

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        if (pKeyMsg->SEL.usFunc == 0) {
            uchCStringFlug = STS_CSTRING_EXEC;      /* Control String Execute OK */
            UieOpenSequence(aszSeqACFSC_AC_MSR);        /* Register Key Sequence */
        } else {
            MaintShrTermUnLock();
            UieNextMenu(aChildACModeIn);            /* Open Next Function */
        }
        break;
        
    case UIM_ACCEPTED:
        UieNextMenu(aChildACModeIn);
        return(SUCCESS);
    
    case UIM_REJECT:
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_KEYED_STRING:          /* Execute Control String Function */
        case FSC_STRING:
#if 0
			/* Need to allow Control String execution at this point - CSMALL 08/28/2006
            if (uchCStringFlug == STS_CSTRING_ERROR){
                return(LDT_SEQERR_ADR);  /* Control String Execute Error
            }
			*/
#endif
            sError = UifRegString(pKeyMsg);
            if (sError != SUCCESS){
                sError = LDT_SEQERR_ADR;
            }
            UifACRedisp();
            return(sError);

        case FSC_ASK:       /* Saratoga */
            if((pKeyMsg->SEL.INPUT.uchMinor >= (UCHAR)EXTFSC_ASK_REPORT16) &&
                (pKeyMsg->SEL.INPUT.uchMinor <= (UCHAR)EXTFSC_ASK_REPORT20) &&
                uchCStringFlug != 0) {
                sError = MaintSupASK(pKeyMsg->SEL.INPUT.uchMinor);
                UifACRedisp();
                return(sError);

            } else {
                return(LDT_SEQERR_ADR);
            }

        case FSC_AC:
            /* break; fall through to MSR source */
        case FSC_MSR:
#ifdef NDEBUG
			BlFwIfNagScreen();
#endif

            ModeIn.uchStatus = 0;               /* Set W/ Amount Status */
			/* --- In case of MLD parameter downloading,    Saratoga --- */
			if ((husUifACHandle = UicCheckAndSet()) == UIC_NOT_IDLE) {
				husUifACHandle = 0;
				return(LDT_LOCK_ADR);
			}

			{
				MAINTREDISP     ReDisplay;
				UIFDIAEMPLOYEE  UifDiaEmployee;
				extern ULONG    ulMaintSupNumber;            /* supervisor number */

				sError = CheckSuperNumber(pKeyMsg, &UifDiaEmployee);
				if (sError == SUCCESS) {
					ulMaintSupNumber = UifDiaEmployee.ulSwipeEmployeeId;
					MldSetDescriptor(MLD_SUPER_MODE);      /* change descriptor to super mode */
					MldSetMode(MLD_SUPER_MODE);             /* MLD Set Supervisor Mode */
					uchCStringFlug = STS_CSTRING_ERROR;      /* Control String Execute NG */
					UieNextMenu(aChildACModeIn);                    /* Open Next Function */
					UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT13); /* Saratoga */
				} else {
					SHORT sResetStatus;
					ulMaintSupNumber = 0;
					if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  /* reset transaction lock */
						PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
						PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
						PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
						PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
						PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
					}
					husUifACHandle = 0;
					ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
					ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
					MaintReDisp(&ReDisplay);
				}
            }
            return(sError);

        case FSC_CANCEL:
            return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 
        }
		// fall through to do default processing
    default:
        sError = UifACDefProc(pKeyMsg);                         /* Execute Default Procedure */
        break;
    }
    return(sError);
}


/*
*=============================================================================
**  Synopsis: SHORT UifACEnter(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Supervisor Enter Mode 
*===============================================================================
*/

/* Define Next Function and Key Sequence */
UIMENU CONST aChildAC1Func[] = {{UifAC1Function,CID_AC1FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC2Func[] = {{UifAC2Function,CID_AC2FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC3Func[] = {{UifAC3Function,CID_AC3FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC4Func[] = {{UifAC4Function,CID_AC4FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC5Func[] = {{UifAC5Function,CID_AC5FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC6Func[] = {{UifAC6Function,CID_AC6FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC7Func[] = {{UifAC7Function,CID_AC7FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC15Func[] = {{UifAC15Function,CID_AC15FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC17Func[] = {{UifAC17Function,CID_AC17FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC10Func[] = {{UifAC10Function,CID_AC10FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC11Func[] = {{UifAC11Function,CID_AC11FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC18Func[] = {{UifAC18Function,CID_AC18FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC19Func[] = {{UifAC19Function,CID_AC19FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC20Func[] = {{UifAC20Function,CID_AC20FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC21Func[] = {{UifAC21Function,CID_AC21FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC23Func[] = {{UifAC23Function,CID_AC23FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC24Func[] = {{UifAC24Function,CID_AC24FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC26Func[] = {{UifAC26Function,CID_AC26FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC29Func[] = {{UifAC29Function,CID_AC29FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC30Func[] = {{UifAC30Function,CID_AC30FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};

//GroupReport - CSMALL
UIMENU CONST aChildAC31Func[] = {{UifAC31Function,CID_AC31FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC33Func[] = {{UifAC33Function,CID_AC33FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC39Func[] = {{UifAC39Function,CID_AC39FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC40Func[] = {{UifAC40Function,CID_AC40FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC42Func[] = {{UifAC42Function,CID_AC42FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC63Func[] = {{UifAC63Function,CID_AC63FUNCTION},{NULL,0}};
UIMENU CONST aChildAC64Func[] = {{UifAC64Function,CID_AC64FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC66Func[] = {{UifAC66Function,CID_AC66FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};/* All File Close,  Saratoga*/
UIMENU CONST aChildAC68Func[] = {{UifAC68Function,CID_AC68FUNCTION},{NULL,0}};
UIMENU CONST aChildAC71Func[] = {{UifAC71Function,CID_AC71FUNCTION},{NULL,0}};
UIMENU CONST aChildAC75Func[] = {{UifAC75Function,CID_AC75FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC78Func[] = {{UifAC78Function,CID_AC78FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC82Func[] = {{UifAC82Function,CID_AC82FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC84Func[] = {{UifAC84Function,CID_AC84FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC85Func[] = {{UifAC85Function,CID_AC85FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC86Func[] = {{UifAC86Function,CID_AC86FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC87Func[] = {{UifAC87Function,CID_AC87FUNCTION},{NULL,0}};
UIMENU CONST aChildAC88Func[] = {{UifAC88Function,CID_AC88FUNCTION},{NULL,0}};
UIMENU CONST aChildAC89Func[] = {{UifAC89Function,CID_AC89FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC97Func[] = {{UifAC97Function,CID_AC97FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC99Func[] = {{UifAC99Function,CID_AC99FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC100Func[] = {{UifAC100Function,CID_AC100FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC102Func[] = {{UifAC102Function,CID_AC102FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC103Func[] = {{UifAC103Function,CID_AC103FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC111Func[] = {{UifAC111Function,CID_AC111FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC114Func[] = {{UifAC114Function,CID_AC114FUNCTION},{NULL,0}};
UIMENU CONST aChildAC115Func[] = {{UifAC115Function,CID_AC115FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC116Func[] = {{UifAC116Function,CID_AC116FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC119Func[] = {{UifAC119Function,CID_AC119FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC122Func[] = {{UifAC122Function,CID_AC122FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}}; 
UIMENU CONST aChildAC123Func[] = {{UifAC123Function,CID_AC123FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC124Func[] = {{UifAC124Function,CID_AC124FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC125Func[] = {{UifAC125Function,CID_AC125FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC126Func[] = {{UifAC126Function,CID_AC126FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC127Func[] = {{UifAC127Function,CID_AC127FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC128Func[] = {{UifAC128Function,CID_AC128FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC129Func[] = {{UifAC129Function,CID_AC129FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC130Func[] = {{UifAC130Function,CID_AC130FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC131Func[] = {{UifAC131Function,CID_AC131FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC132Func[] = {{UifAC132Function,CID_AC132FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC133Func[] = {{UifAC133Function,CID_AC133FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC135Func[] = {{UifAC135Function,CID_AC135FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC136Func[] = {{UifAC136Function,CID_AC136FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC137Func[] = {{UifAC137Function,CID_AC137FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC150Func[] = {{UifAC150Function,CID_AC150FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC151Func[] = {{UifAC151Function,CID_AC151FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC152Func[] = {{UifAC152Function,CID_AC152FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC153Func[] = {{UifAC153Function,CID_AC153FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC154Func[] = {{UifAC154Function,CID_AC154FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC160Func[] = {{UifAC160Function,CID_AC160FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC161Func[] = {{UifAC161Function,CID_AC161FUNCTION},{NULL,0}};
UIMENU CONST aChildAC162Func[] = {{UifAC162Function,CID_AC162FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC169Func[] = {{UifAC169Function,CID_AC169FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC170Func[] = {{UifAC170Function,CID_AC170FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC175Func[] = {{UifAC175Function,CID_AC175FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}}; 
UIMENU CONST aChildAC176Func[] = {{UifAC176Function,CID_AC176FUNCTION},{NULL,0}};
// adding for new AC for Dynamic Window Reload
UIMENU CONST aChildAC194Func[] = {{UifAC194Function,CID_AC194FUNCTION},{NULL,0}};
UIMENU CONST aChildAC208Func[] = {{UifAC208Function,CID_AC208FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}}; 
UIMENU CONST aChildAC223Func[] = {{UifAC223Function,CID_AC223FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}}; 
UIMENU CONST aChildAC233Func[] = {{UifAC233Function,CID_AC233FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}}; 
UIMENU CONST aChildAC985Func[] = {{UifAC985Function,CID_AC985FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC999Func[] = {{UifAC999Function,CID_AC999FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC888Func[] = {{UifAC888Function,CID_AC888FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC777Func[] = {{UifAC777Function,CID_AC777FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC275Func[] = {{UifAC275Function,CID_AC275FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC276Func[] = {{UifAC276Function,CID_AC276FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};
UIMENU CONST aChildAC444Func[] = {{UifAC444Function,CID_AC444FUNCTION},{UifACCancel,CID_ACCANCEL},{NULL,0}};

SHORT UifACEnter(KEYMSG *pKeyMsg) 
{
	SYSCONFIG CONST  *pSysConfig = PifSysConfig(); 
    SHORT             sError = SUCCESS;
    MAINTENT          Ent = {0};
	MAINTREDISP       ReDisplay = {0};

    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        if (pKeyMsg->SEL.usFunc) {                      /* In case of 1st initialize, "usFunc" is 0 */
            MaintShrTermUnLock();                       /* Terminal Unlock */
        }
        if (husUifACHandle) {   /* after set flag ? (terminate some A/C execution) */
			SHORT  sResetStatus;
            if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  /* reset transaction lock */
                PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
                PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
                PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
            }
            husUifACHandle = 0;
        }
        if (pKeyMsg->SEL.usFunc) {                      /* In case of 1st initialize, "usFunc" is 0 */
            UieAccept();    /* Return To Modein With Accept Message */
            return(SUCCESS);
        }
        UifACInit();                                    /* Clear Own/Maint/Report Work Area */
        UieOpenSequence(aszSeqACFSC_AC_CUR);                /* Register Key Sequence */
        UieModeChange(UIE_ENABLE);                      /* Enable Mode Change */
        UieCtrlDevice(UIE_DIS_MSR);
        
        /* Reset All Descriptor */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);

        /* Display Entry */
        if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
            UifACMenu(uchUifACMenuNumber);          /* AC Number Menu For LCD */
        }

        MaintDisp(0, 0, 0, 0, 0, 0, 0L, LDT_AC_ADR);
        uchUifReDispMode = STS_AC_ENTER;        /* Redisplay Mode Set */

        /* print out pc i/f log, V3.3 */
        if (uchResetLog != 0 || uchPowerDownLog != 0) {
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
        }
        return(SUCCESS);                /* 2171,    Saratoga */

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_KEYED_STRING:          /* Execute Control String Function */
        case FSC_STRING:
            sError = UifRegString(pKeyMsg);
            if (sError != SUCCESS){
                sError = LDT_SEQERR_ADR;
            }
            UifACRedisp();
            return(sError);

        case FSC_ASK:       /* Saratoga */
            if((pKeyMsg->SEL.INPUT.uchMinor >= (UCHAR)EXTFSC_ASK_REPORT16) &&
                (pKeyMsg->SEL.INPUT.uchMinor <= (UCHAR)EXTFSC_ASK_REPORT20)) {
                sError = MaintSupASK(pKeyMsg->SEL.INPUT.uchMinor);
                return(sError);
            } else {
                return(LDT_SEQERR_ADR);
            }

        case FSC_NUM_TYPE:              /* When Report Out MLD Display On/Off */
            if (pKeyMsg->SEL.INPUT.uchLen) {   /* With Amount Case */
                return(LDT_SEQERR_ADR);
            }

            if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
                if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){ /* MLD Display Out */
                    uchUifACRptOnOffMld = RPT_DISPLAY_OFF;
                    /* Reset All Descriptor */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_RECEIPT;
                    MaintReDisp(&ReDisplay);
                }else{
                    uchUifACRptOnOffMld = RPT_DISPLAY_ON;
                    /* Set No Receipt Descriptor */
                    ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                    ReDisplay.uchMinorClass = CLASS_MAINTREDISP_NORECEIPT;
                    MaintReDisp(&ReDisplay);
                }
            }
            UieReject();
            return(SUCCESS);

		case FSC_FOR: //ESMITH PRTFILE
            if (pKeyMsg->SEL.INPUT.uchLen) {   /* With Amount Case */
                return(LDT_SEQERR_ADR);
            }

			if(uchUifACRptOnOffFile == RPT_PRINTFILE_ON){ /* Print to file on */
				uchUifACRptOnOffFile = RPT_PRINTFILE_OFF;
				MaintSetPrintMode(PRT_SENDTO_PRINTER);
			} else {
				uchUifACRptOnOffFile = RPT_PRINTFILE_ON;
				MaintSetPrintMode(PRT_SENDTO_FILE_SUP);
			}
			return(SUCCESS);

        case FSC_SCROLL_UP:
            if (pKeyMsg->SEL.INPUT.uchLen) {   /* With Amount Case */
                return(LDT_SEQERR_ADR);
            }

            if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
                /* Scroll Up */
                if(uchUifACMenuNumber > 0) {   /* Previous Menu Page */
                    uchUifACMenuNumber = UifACMenuBack(uchUifACMenuNumber);
                }
            }
            UieReject();
            return(SUCCESS);

        case FSC_SCROLL_DOWN:
            if (pKeyMsg->SEL.INPUT.uchLen) {   /* With Amount Case */
                return(LDT_SEQERR_ADR);
            }

            if (pSysConfig->uchOperType == DISP_LCD) {  /* check LCD is provided */
				if(uchUifACMenuMore) 
				{   /* Preview Menu Page */
					uchUifACMenuNumber = uchUifACMenuMore;
					UifACMenu(uchUifACMenuNumber);
				}
            }
            UieReject();
            return(SUCCESS);

        case FSC_AC:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3) {   /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            /* Execute Enter A/C Code Procedure */
            Ent.uchMajorClass = CLASS_MAINTENT;         /* Set Major Class Data */
            Ent.uchStatus = 0;                          /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {           /* W/o Amount Input Case */
                Ent.uchStatus |= MAINT_WITHOUT_DATA;    /* Set W/o Amount Status */
            } else {                                    /* W/ Amount Input Case */
                Ent.usSuperNo = ( USHORT)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintEntSup(&Ent)) == SUCCESS) {  /* Open Next Function By A/C Code */
                /* check if terminal lock is successful */
                if ((husUifACHandle = UicCheckAndSet()) == UIC_NOT_IDLE) { /* ### MOD (2172 Rel1.0) */
                    husUifACHandle = 0;
                    return(LDT_LOCK_ADR);
                }
                /* do not lock target shared printer at this point, in case of target is down at AC85, 12/08/01 */
                switch (Ent.usSuperNo) {                    
                case AC_COPY_TTL:                   /* COPY TOTAL/COUNTER FILE */
				case AC_DISCON_MAST:                /* DISCONNECT MASTER/BACKUP MASTER */
                case AC_FORCE_MAST:                 /* Force up to date on both Master and Backup */
				case AC_SHUTDOWN:					/* SHUTDOWN PROCEDURE JHHJ 7-20-04*/
                	break;

                case AC_JOIN_CLUSTER:              // UifAC444Function() to join a cluster 
                    uchUifTrailerOnOff = AC_JOIN_CLUSTER;
                	break;

				/*SR 474 PLEASE NOTE: THIS CASE STATEMENT NEEDS TO BE DIRECTLY ABOVE THE DEFAULT
				  SO THAT IF IT DOES NOT RETURN A PROHIBITED RESPONSE, IT DOES THE CORRECT 
				  PROCEDURE JHHJ 3-22-05*/
				case AC_SET_CTLPG:
				case AC_PLU_MENU:
					{
						PARATERMINALINFO  TermInfo = {0};

						/*SR 474 we need to check and see what kind of terminal we are
						so that we dont allow a user to change our Menu page on touch screen*/
						TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
						ParaTerminalInfoParaRead (&TermInfo);
						if(TermInfo.TerminalInfo.uchMenuPageDefault >= STD_TOUCH_MENU_PAGE)
						{
							if(husUifACHandle) //above we have turned the lock flag on, so we must reset it.
							{
								UicResetFlag(husUifACHandle);
							}
							return(LDT_PROHBT_ADR);
						}
					}
					// if ok then drop through to the default case.
                default:
	                if ((sError = MaintShrTermLockSup(Ent.usSuperNo)) != SUCCESS) {        /* In case of SYSTEM BUSY */
						SHORT  sResetStatus;
    	                if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  /* reset transaction lock */
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
        	                PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
            	        }
                	    husUifACHandle = 0;
                    	UieExit(aACModeIn);                     /* Return to UifACModeIn() */
	                    return(SUCCESS);
    	            }
    	            break;
    	        }
                
                UieCtrlDevice(UIE_ENA_SCANNER); /* set scanner as enable, 2172 */
                UifACPGMLDClear();        /* Menu Clear */

                switch (Ent.usSuperNo) {                    
                case AC_GCNO_ASSIN:                 /* GUEST CHECK NO. ASSIGNMENT */
                    UieNextMenu(aChildAC1Func);    
                    break;
                
                case AC_GCFL_READ_RPT:              /* GUEST CHECK FILE READ REPORT */
                    uchUifTrailerOnOff = AC_GCFL_READ_RPT;
                    UieNextMenu(aChildAC2Func); 
                    break;
                
                case AC_GCFL_RESET_RPT:             /* GUEST CHECK FILE RESET REPORT */
                    UieNextMenu(aChildAC3Func); 
                    break;
                
                case AC_PLU_MENU:                   /* SET PLU NO.ON MENU PAGE  */
                    UieNextMenu(aChildAC4Func); 
                    break;

                case AC_SET_CTLPG:                  /* SET CONTROL TABLE OF MENU PAGE  */
                    UieNextMenu(aChildAC5Func); 
                    break;

				case AC_MANU_KITCH:                 /* MANUAL ALTERNATIVE KITCHEN PRINTER ASSIGNMENT */
                    UieNextMenu(aChildAC6Func); 
                    break;

				case AC_CASH_ABKEY:                 /* CASHIER A/B KEYS ASSIGNMENT  */
                    UieNextMenu(aChildAC7Func); 
                    break;
                case AC_SUPSEC_NO:                  /* SUPERVISOR SECRET NUMBER CHANGE  */
                    UieNextMenu(aChildAC15Func);
                    break;

                case AC_TOD:                        /* SET DATE AND TIME  */
                    UieNextMenu(aChildAC17Func);
                    break;

				case AC_LOAN:
                    UieNextMenu(aChildAC10Func);    /* Loan,    Saratoga */
                    break;

                case AC_PICKUP:
                    UieNextMenu(aChildAC11Func);    /* Pick-up,  Saratoga */
                    break;

                case AC_PTD_RPT:                    /* PTD RESET REPORT */
                    UieNextMenu(aChildAC18Func);
                    break;
                
                case AC_CASH_RESET_RPT:             /* CASHIER RESET REPORT */
                    UieNextMenu(aChildAC19Func);
                    break;
                
                case AC_CASH_ASSIN:                 /* CASHIER NO. ASSIGNMENT */
                    UieNextMenu(aChildAC20Func);
                    break;
                
                case AC_CASH_READ_RPT:              /* CASHIER READ REPORT */
                    uchUifTrailerOnOff = AC_CASH_READ_RPT;
                    UieNextMenu(aChildAC21Func);
                    break;
                
                case AC_REGFIN_READ_RPT:            /* REGISTER FINANCIAL REPORT  */
                    uchUifTrailerOnOff = AC_REGFIN_READ_RPT;
                    UieNextMenu(aChildAC23Func);
                    break;
                
                case AC_HOURLY_READ_RPT:            /* HOURLY ACTIVITY REPORT */
                    uchUifTrailerOnOff = AC_HOURLY_READ_RPT;
                    UieNextMenu(aChildAC24Func);
                    break;
                
                case AC_DEPTSALE_READ_RPT:          /* DEPARTMENT SALES READ REPORT */
                    uchUifTrailerOnOff = AC_DEPTSALE_READ_RPT;
                    UieNextMenu(aChildAC26Func);
                    break;
                
                case AC_PLUSALE_READ_RPT:           /* PLU SALES READ REPORT */
                    uchUifTrailerOnOff = AC_PLUSALE_READ_RPT;
                    UieNextMenu(aChildAC29Func);
                    break;
                
                case AC_CPN_READ_RPT:              /* COUPON READ REPORT */
                    uchUifTrailerOnOff = AC_CPN_READ_RPT;
                    UieNextMenu(aChildAC30Func);
                    break;

				//GroupReport - CSMALL
				case AC_PLUGROUP_READ_RPT:				/* PLU GROUP SUMMARY REPORT */
					uchUifTrailerOnOff = AC_PLUGROUP_READ_RPT;
					UieNextMenu(aChildAC31Func);
					break;

                case AC_CSTR_MAINT:                 /* CONTROL STRING MAINTENANCE */
                    if((sError = MaintCstrLock()) != SUCCESS){
						SHORT  sResetStatus;
                        if((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS){
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI,FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     /* Return to UifACModeIn() */
                        return(sError);
                    }
                    usUifACControl = 0;              /* Reset Control Flugs */
                    UieNextMenu(aChildAC33Func);
                    break;
                
                case AC_PLU_RESET_RPT:              /* PLU ALL RESET REPORT */
                    UieNextMenu(aChildAC39Func);
                    break;
                
                case AC_CPN_RESET_RPT:              /* COUPON RESET REPORT */
                    UieNextMenu(aChildAC40Func);
                    break;

                case AC_COPY_TTL:                   /* COPY TOTAL/COUNTER FILE */
                    UieNextMenu(aChildAC42Func);
                    break;

                case AC_JOIN_CLUSTER:               // CID_AC444FUNCTION Join a cluster
                    UieNextMenu(aChildAC444Func);
                    break;

                case AC_PLU_MAINT:                  /* PLU  MAINTENANCE */
                    if ((sError = MaintPLUDEPTLock()) != SUCCESS) {     // Lock PLU,DEPT File 
						SHORT   sResetStatus;
                        if ((sResetStatus = UicResetFlag(husUifACHandle) != UIC_SUCCESS)) {  // 9/27/93 reset transaction lock
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     // Return to UifACModeIn()
                        return(sError);
                    }
                    UieNextMenu(aChildAC63Func);
                    break;
                
                case AC_PLU_PRICE_CHG:              /* PLU PRICE CHANGE */
                    UieNextMenu(aChildAC64Func);
                    break;

				/* --- All File Close,  Saratoga --- */
                case AC_PLU_DOWN:
                    UieNextMenu(aChildAC66Func);
                    break;

                case AC_PLU_ADDDEL:                 /* PLU ADD/DELETION */
					if ((sError = MaintPLUDEPTLock()) != SUCCESS) {   // Lock PLU,DEPT File 
						SHORT   sResetStatus;
						if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  // 9/27/93 reset transaction lock 
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
							PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
						}
						husUifACHandle = 0;
						UieExit(aACModeIn);                     // Return to UifACModeIn() 
						return(sError);
                    }
                    UieNextMenu(aChildAC68Func);
                    break;

                case AC_PPI_MAINT:                   /* PPI TABLE MAINTENANCE */
                    if((sError = MaintPPILock()) != SUCCESS){
						SHORT   sResetStatus;
                        if((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS){
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI,FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     /* Return to UifACModeIn() */
                        return(sError);
                    }
                    usUifACControl = 0;              /* Reset Control Flugs */
                    UieNextMenu(aChildAC71Func);
                    break;
                
                case AC_EJ_READ_RPT:                /* E/J READ REPORT  */
                    uchUifTrailerOnOff = AC_EJ_READ_RPT;
                    UieModeChange(UIE_DISABLE);                 /* for LCD read report */
                    sError = UifAC70Function();
                    UieModeChange(UIE_ENABLE);                  /* for LCD read report */
                    UieExit(aACModeIn);             /* Return to UifACModeIn() */
					return(sError);
                    break;

				case AC_PLUPARA_READ_RPT:           /* PLU PARAMETER FILE READ REPORT */
                    sError = UifAC72Function();
                    UieExit(aACModeIn);                 
                    break;

				case AC_ALL_DOWN:                   /* BROADCAST/REQUEST TO DOWNLOAD ALL PARAMETER  */
                    UieNextMenu(aChildAC75Func);
                    break;

				case AC_START_STOP_CPM:             /* Start and Stop CPM */
                    UieNextMenu(aChildAC78Func);
                    break;

                case AC_PLU_MNEMO_CHG:              /* PLU MNEMONIC CHANGE */
                    UieNextMenu(aChildAC82Func);
                    break;
                
                case AC_ROUND:                      /* SET ROUNDING TABLE */
                    UieNextMenu(aChildAC84Func);
                    break;

				case AC_DISCON_MAST:                /* DISCONNECT MASTER/BACKUP MASTER */
                    UieNextMenu(aChildAC85Func);
                    break;

                case AC_FORCE_MAST:                 /* Force up to date on Master and Backup Master */
                    UieNextMenu(aChildAC985Func);
                    break;

				case AC_DISC_BONUS:                 /* SET DISCOUNT/BONUS % RATES */
                    UieNextMenu(aChildAC86Func);
                    break;
                
                case AC_SOFTCHK_MSG:                /* SET SALES PROMOTION MESSAGE ON SLIP */
                    UieNextMenu(aChildAC87Func);
                    break;
                
                case AC_SALES_PROMOTION:            /* SET SALES PROMOTION MESSAGE */
                    UieNextMenu(aChildAC88Func);
                    break;
                
                case AC_CURRENCY:                   /* SET CURRENCY CONVERSION RATE */
                    UieNextMenu(aChildAC89Func);
                    break;

                case AC_TALLY_CPM_EPT:              /* SET READ AND RESET EPT AND CPM TALLY */
                    UieNextMenu(aChildAC97Func);
                    break;
                
                case AC_EOD_RPT:                    /* END OF DAY RESET REPORT */
                    UieNextMenu(aChildAC99Func);
                    break;
                
                case AC_EJ_RESET_RPT:               /* E/J RESET REPORT */
                    UieNextMenu(aChildAC100Func);
                    break;

				case AC_STORE_FORWARD:
					UieNextMenu(aChildAC102Func);	/* STORE AND FORWARD CONTROL */
					break;

				case AC_EMG_GCFL_CLS:               /* EMERGENT GUEST CHECK FILE CLOSE */
                    UieNextMenu(aChildAC103Func); 
                    break;

				case AC_TARE_TABLE:                 /* SET TARE TABLE */
                    UieNextMenu(aChildAC111Func); 
                    break;
                
                case AC_DEPT_MAINT:                 /* DEPARTMENT  MAINTENANCE/REPORT */
                    if ((sError = MaintPLUDEPTLock()) != SUCCESS) {   // Lock PLU,DEPT File 
						SHORT   sResetStatus;
                        if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  // 9/27/93 reset transaction lock 
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     // Return to UifACModeIn() 
                        return(sError);
                    }
                    UieNextMenu(aChildAC114Func); 
                    break;
                
                case AC_DEPT_MENU:                   /* SET DEPT NO.ON MENU PAGE, 2172  */
                    UieNextMenu(aChildAC115Func); 
                    break;

                case AC_SET_PLU:                    /* ASSIGN SET MENU */
                    UieNextMenu(aChildAC116Func); 
                    break;
                
                case AC_EMG_CASHFL_CLS:             /* EMERGENT CASHIER FILE CLOSE */
                    UieNextMenu(aChildAC119Func); 
                    break;
                                                    
                case AC_MAJDEPT_RPT:                /* MAJOR DEPARTMENT SALES READ REPORT */
                    uchUifTrailerOnOff = AC_MAJDEPT_RPT;

                    /* Check PTD Exist or Not */
                    if (!RptPTDChkFileExist(FLEX_DEPT_ADR)) {       /* Not PTD Case */
                        uchUifACDataType = ( UCHAR)RPT_TYPE_DALY;

                        UieModeChange(UIE_DISABLE);                 /* for LCD read report */
                        MaintDisp(AC_MAJDEPT_RPT, 0, 0, ( UCHAR)RPT_TYPE_DALY, 0, 0, 0L, 0);
                        sError = RptDEPTRead(CLASS_TTLCURDAY, RPT_ALL_MDEPT_READ, 0);
                        UieModeChange(UIE_ENABLE);                  /* for LCD read report */
                        UieExit(aACModeIn);                 
                    } else {
                        UieNextMenu(aChildAC122Func);
                    }
                    break;

				case AC_MEDIA_RPT:               /* MEDIA FLASH REPORT */
                    uchUifTrailerOnOff = AC_MEDIA_RPT;
                    UieNextMenu(aChildAC123Func); 
                    break;
                
                case AC_TAXTBL1:                 /* TAX TABLE #1 PROGRAMMING */
                    UieNextMenu(aChildAC124Func); 
                    break;
                
                case AC_TAXTBL2:                 /* TAX TABLE #2 PROGRAMMING */
                    UieNextMenu(aChildAC125Func); 
                    break;
                
                case AC_TAXTBL3:                 /* TAX TABLE #3 PROGRAMMING */
                    UieNextMenu(aChildAC126Func); 
                    break;
                
                case AC_TAX_RATE:                /* SET TAX/SERVICE/RECIPROCAL RATES */
                    UieNextMenu(aChildAC127Func); 
                    break;                       

                case AC_MISC_PARA:               /* SET MISC. PARAMETER */
                    UieNextMenu(aChildAC128Func);
                    break;

                case AC_TAXTBL4:                 /* TAX TABLE #4 PROGRAMMING */
                    UieNextMenu(aChildAC129Func); 
                    break;
                
                case AC_PIGRULE:                 /* SET TAX/SERVICE/RECIPROCAL RATES */
                    UieNextMenu(aChildAC130Func); 
                    break;

                case AC_SERVICE_READ_RPT:        /* SERVICE TIME REPORT */
                    uchUifTrailerOnOff = AC_SERVICE_READ_RPT;
                    UieNextMenu(aChildAC131Func); 
                    break;                       
                
                case AC_SERVICE_RESET_RPT:       /* SERVICE TIME RESET REPORT */
                    UieNextMenu(aChildAC132Func); 
                    break;                       
                
                case AC_SERVICE_MAINT:           /* SET SERVICE TIME BORDER */
                    UieNextMenu(aChildAC133Func); 
                    break;                       

                case AC_PROG_READ_RPT:           /* PROGRAMABLE READ REPORT */   
                    uchUifTrailerOnOff = AC_PROG_READ_RPT;
                    UieNextMenu(aChildAC135Func); 
                    break;                       
                
                case AC_PROG_RESET_RPT:          /* PROGRAMABLE RESET REPORT */   
                    UieNextMenu(aChildAC136Func); 
                    break;                       
                
                case AC_DISPENSER_PARA:           /* SET BEVERAGE DISPENSER PARA */
                    UieNextMenu(aChildAC137Func); 
                    break;                       
                
                case AC_ETK_READ_RPT:
                    uchUifTrailerOnOff = AC_ETK_READ_RPT;
                    UieNextMenu(aChildAC150Func);   /* ETK READ REPROT */
                    break;
                
                case AC_ETK_RESET_RPT:
                    UieNextMenu(aChildAC151Func);   /* ETK RESET REPORT */
                    break;
                
                case AC_ETK_ASSIN:
                    if ((sError = RptETKLock()) != SUCCESS) {
						SHORT   sResetStatus;
                        if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  /* 9/27/93 reset transaction lock */
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     /* Return to UifACModeIn() */
                        return(sError);
                    }
                    UieNextMenu(aChildAC152Func);   /* ETK ASSIGN REPORT */
                    break;

				case AC_ETK_MAINT:
                    if ((sError = RptETKLock()) != SUCCESS) {
						SHORT   sResetStatus;
                        if ((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS) {  /* 9/27/93 reset transaction lock */
							PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
							PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
							PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     /* Return to UifACModeIn() */
                        return(sError);
                    }
                    UieNextMenu(aChildAC153Func);
                    break;

                case AC_ETK_LABOR_COST:              /* SET LABOR COST VALUE */
                    UieNextMenu(aChildAC154Func); 
                    break;                       
                
                case AC_OEP_MAINT:                   /* SET ORDER ENTRY PROMPT TABLE */
                    UieNextMenu(aChildAC160Func); 
                    break;

                case AC_CPN_MAINT:
                    if((sError = MaintCPNLock()) != SUCCESS){
						SHORT  sResetStatus;
                        if((sResetStatus = UicResetFlag(husUifACHandle)) != UIC_SUCCESS){
                            PifLog(MODULE_UI, FAULT_INVALID_HANDLE);
                            PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
                            PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sResetStatus));
                            PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
                            PifAbort(MODULE_UI, FAULT_INVALID_HANDLE);
                        }
                        husUifACHandle = 0;
                        UieExit(aACModeIn);                     /* Return to UifACModeIn() */
                        return(sError);
                    }
                    UieNextMenu(aChildAC161Func);
                    break;

                case AC_FLEX_DRIVE:                   /* A/C 162 */
                    UieNextMenu(aChildAC162Func); 
                    break;

                case AC_TONE_CTL:                   /* ADJUST TONE CONTROL */
                    UieNextMenu(aChildAC169Func); 
                    break;

				case AC_PLU_SLREST:
                    UieNextMenu(aChildAC170Func);   /* SALES RESTRICTION 2172 */
                    break;

				case AC_SUP_DOWN:                   /* BROADCAST/REQUEST TO DOWNLOAD SUPER PARAMETER */
                    UieNextMenu(aChildAC175Func); 
                    break;

                case AC_CSOPN_RPT:                  /* OPEN/GUEST CHECK STATUS READ REPORT */      
					uchUifTrailerOnOff = AC_CSOPN_RPT;
                    UieNextMenu(aChildAC176Func);
                    break;

				case AC_RELOADOEP:
					UieNextMenu(aChildAC194Func);
					break;

                case AC_BOUNDARY_AGE:                /* BOUNDARY AGE 2172 */
                    UieNextMenu(aChildAC208Func);
                    break;

                case AC_IND_READ_RPT:                /* TERMINAL READ REPORT */
                    uchUifTrailerOnOff = AC_IND_READ_RPT;
                    UieNextMenu(aChildAC223Func);
                    break;
                
                case AC_IND_RESET_RPT:               /* TERMINAL RESET REPORT */
                    UieNextMenu(aChildAC233Func);
                    break;
                
				case AC_SET_LAYOUT:
					uchUifTrailerOnOff = AC_SET_LAYOUT;
					UieNextMenu(aChildAC777Func);
					break;

				case AC_CONFIG_INFO:
					uchUifTrailerOnOff = AC_CONFIG_INFO;
					UieNextMenu(aChildAC888Func);
					UieExit(aACModeIn);
					return SUCCESS;

				case AC_SHUTDOWN:					 //AC999 SHUTDOWN APP
					UieNextMenu(aChildAC999Func);
					break;

				case AC_SET_DELAYED_BAL: //Delayed Balance EOD JHHJ
					UieNextMenu(aChildAC275Func);
					break;

				case AC_SET_DELAYED_BAL_OFF:
					UieNextMenu(aChildAC276Func);
					break;

                default:
                    break;
                }
                UieModeChange(UIE_DISABLE);                     /* Disable Mode Change */
                RptInitialize();                                /* Clear Report Work Area */
                if ((Ent.usSuperNo == AC_PLUPARA_READ_RPT)      /* A/C 72 Case */
                    // || (Ent.usSuperNo == AC_EJ_READ_RPT)       /* A/C 70 Case */
                     || (Ent.usSuperNo == AC_CSOPN_RPT)		/* A/C 176 Case */
					 || (Ent.usSuperNo == AC_RELOADOEP))	/* AC 194 Case - added for Dynamic Window Reload - CSMALL*/
				{      
                    UieModeChange(UIE_ENABLE);                  /* Enable Mode Change */
                    return(SUCCESS);
                } else if ((Ent.usSuperNo == AC_MAJDEPT_RPT) && !RptPTDChkFileExist(FLEX_DEPT_ADR)) {/* Not PTD in A/C 122 */
                    UieModeChange(UIE_ENABLE);                  /* Enable Mode Change */
                    return(SUCCESS);
                }
            }
            if (sError == SUCCESS){
                UieReject();
            }
            return(sError);

        case FSC_CANCEL:
            return(LDT_SEQERR_ADR);             /* Return Sequence Error */ 
        }
		// fall through to do default processing.
    }
    return(UifACDefProc(pKeyMsg)); /* ### Mov (2171 for Win32) V1.0 */
}
/*
*=============================================================================
**  Synopsis: SHORT UifACDefProc(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**  Return: SUCCESS : Successful 
*               
**  Description: Supervisor Default Mode 
*===============================================================================
*/
SHORT UifACDefProc(KEYMSG *pKeyMsg) 
{
    USHORT          usACNumber = MaintGetACNumber();
    SHORT           sError = SUCCESS;
	MAINTREDISP     ReDisplay = {0};
    MAINTSETPAGE    SetPage = {0};

    switch (pKeyMsg->uchMsg) {
    case UIM_CANCEL:
        switch (usACNumber) {
        case AC_LOAN:
        case AC_PICKUP:
            UifAC1011KeyStatusClear();                  /* Reset Modifier Key (AC10/11) */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);                    /* complusory redisplay */
        }
        return(SUCCESS);

    case UIM_QUIT:
        if (pPifSaveFarData != NULL) {               /* Simulator */
            (*pPifSaveFarData)();                           
        } 
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_KEYED_STRING:          /* Execute Control String Function */
        case FSC_STRING:
            sError = UifRegString(pKeyMsg);
            if (sError != SUCCESS){
                sError = LDT_SEQERR_ADR;
            }
            UifACRedisp();
            return(sError);

        case FSC_CANCEL:
            /* --- Saratoga --- */
            if ((usACNumber == AC_LOAN) || (usACNumber == AC_PICKUP)) {
                sError = MaintLoanPickupAbort();    /* Loan/Pickup Cashier Close */
                if (sError) {
                    return(sError);
                }
                UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT13); /* Start Echo Back and Set Max Input Digit */
            }
            /* --- Saratoga --- */
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);    /* Execute Finalize Procedure */
            usUifACControl &= ~UIF_SHIFT_PAGE;      /* Reset Shift Page Bit */
            UieExit(aACModeIn);                     /* Return to UifACModeIn() */
            return(SUCCESS);

        case FSC_RECEIPT_FEED:
            MaintFeedRec(MAINT_RECKEYDEF_FEED);     /* Execute Receipt Feed */
            UieKeepMessage();
            return(SUCCESS);

        case FSC_JOURNAL_FEED:
            MaintFeedJou();             /* Execute Journal Feed */
            UieKeepMessage();
            return(SUCCESS);

        case FSC_REVERSE_PRINT:         /* Execute Reverse Function */
            return(RptEJReverse());

        case FSC_MENU_SHIFT:
            /* Check Digit */
            if (pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1) {              /* Over Digit */
                return(LDT_KEYOVER_ADR);
            }

            /* Clear SetPage Work */
            memset(&SetPage, '\0', sizeof(MAINTSETPAGE));
            SetPage.uchMajorClass = CLASS_MAINTSETPAGE;                 /* Set Major Class */
            SetPage.uchStatus = 0;                                      /* Set W/ Amount Status */
            if (!pKeyMsg->SEL.INPUT.uchLen) {                           /* W/o Amount Input Case */
                SetPage.uchStatus |= MAINT_WITHOUT_DATA;                /* Set W/o Amount Status */
            } else {                                                    /* W/ Amount Input Case */
                SetPage.uchPageNumber = ( UCHAR)pKeyMsg->SEL.INPUT.lData;
            }
            if ((sError = MaintSetPageOnly(&SetPage)) == SUCCESS) { 
				FSCTBL   *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];

                UieSetFsc(pData);
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_PAGEDISP;
                MaintReDisp(&ReDisplay);
            }
            return(sError);
          
        case FSC_D_MENU_SHIFT:
            /* Check W/o Amount */
            if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
                return(LDT_SEQERR_ADR);
            }

            /* Clear SetPage Work */
            memset(&SetPage, '\0', sizeof(MAINTSETPAGE));
            SetPage.uchMajorClass = CLASS_MAINTSETPAGE;                 /* Set Major Class */
            SetPage.uchPageNumber = pKeyMsg->SEL.INPUT.uchMinor;        /* set page number */
            SetPage.uchStatus = 0;                                      /* Set W/ Amount Status */

            if ((sError = MaintSetPageOnly(&SetPage)) == SUCCESS) { 
				FSCTBL   *pData = (FSCTBL *)&ParaFSC[uchMaintMenuPage - 1];
                UieSetFsc(pData);
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_PAGEDISP;
                MaintReDisp(&ReDisplay);
            }
            return(sError);

        /* print out pc i/f log, V3.3 */
        case FSC_RESET_LOG:              // in UifACDefProc(): call MaintResetLog() to issue PCIF reset log.  see IspWriteResetLog()
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
                if (MaintPowerDownLog(uchPowerDownLog) == 0) {
                    uchPowerDownLog = 0;
                }
            }
            return(SUCCESS);
            break;

		case FSC_SUPR_INTRVN:
			// Handle the FSC to perform a Supervisor Intervention action.
			// We issue the tone and display the leadthru for Supervisor
			// Intervention then call UifSupIntervent () which will do 
			// do a loop processing keyboard input until either a correct
			// Supervisor number is entered or a Clear key is pressed.
			{
				/*--- BEEP TONE ---*/
				PifBeep(UIE_SPVINT_TONE);
    
				// Display the leadthru to inform the operator what is expected
				UifRedisplaySupInt();

				// Process the Supervisor Intervention action
				sError = UifSupIntervent ();

				// Clear the leadthru area as we are done.
                ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
                ReDisplay.uchMinorClass = CLASS_MAINTREDISP_PAGEDISP;
                MaintReDisp(&ReDisplay);
			}
			return sError;

		case FSC_DOC_LAUNCH:
			// Handle the FSC to perform a document launch action.
			// We issue the tone with Supervisor Intervention and then call
			// use one of the spawn primitives to spawn a new processs.
			// We require the system setup to have put a link or shortcut
			// that is linked to the document or application to launch.
			// The shortcut is put into the Database folder allowing people to
			// download and update links.
			sError = UieShelExecuteLinkFile (pKeyMsg->SEL.INPUT.aszKey, pKeyMsg->SEL.INPUT.uchLen);
			return UIF_SUCCESS;

		case FSC_WINDOW_DISPLAY:
			sError = SUCCESS;
			if (pKeyMsg->SEL.INPUT.uchLen < 1) {
				sError = LDT_SEQERR_ADR;
			}
			else {
				if (pKeyMsg->SEL.INPUT.uchLen > 41 && pKeyMsg->SEL.INPUT.aszKey[0] == _T('9')) {
					BlFwIfPopupWindowGroup(pKeyMsg->SEL.INPUT.aszKey+4);	//the sequence of digits previous to this button press should be the window we want to popup
				} else {
					BlFwIfPopupWindowPrefix(pKeyMsg->SEL.INPUT.aszKey);	//the sequence of digits previous to this button press should be the window we want to popup
				}
			}

			// Clear the leadthru area as we are done.
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_PAGEDISP;
            MaintReDisp(&ReDisplay);

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window display is basically transparent to the UIE functionality.
			return sError;

		case FSC_WINDOW_DISMISS:
			sError = SUCCESS;
			if (pKeyMsg->SEL.INPUT.uchLen < 1) {
				sError = LDT_SEQERR_ADR;
			}
			else {
				if (pKeyMsg->SEL.INPUT.uchLen > 4 && pKeyMsg->SEL.INPUT.aszKey[0] == _T('9')) {
					BlFwIfPopdownWindowGroup(pKeyMsg->SEL.INPUT.aszKey+4);	//the sequence of digits previous to this button press should be the window we want to popup
				} else {
					BlFwIfPopdownWindowPrefix(pKeyMsg->SEL.INPUT.aszKey);
				}
			}

			// Clear the leadthru area as we are done.
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_PAGEDISP;
            MaintReDisp(&ReDisplay);

			// just return rather than doing any kind of UIE menu tree action or cleanup.
			// the window dismiss is basically transparent to the UIE functionality.
			return sError;

        case FSC_CNTRL_STRING_EVENT:           /* Journal Feed Key */
			{
				USHORT  usEventNumber = pKeyMsg->SEL.INPUT.uchMinor;
				UifRegStringWaitEvent(usEventNumber);
			}

			// Clear the leadthru area as we are done.
//			ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
//			ReDisplay.uchMinorClass = CLASS_MAINTREDISP_PAGEDISP;
//			MaintReDisp(&ReDisplay);
            return(SUCCESS);
        }
		// drop through and perform default processing.
    default:
        return(UieDefProc(pKeyMsg));    /* Execute Uie Default Procedure */
    }

	return(sError);
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifACChangeKB1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Change Alpha KB to Numeric KB 1 Mode  
*===============================================================================
*/
SHORT UifACChangeKB1(KEYMSG *pKeyMsg) 
{
	MAINTREDISP     ReDisplay = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P4);                                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */
        if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }

        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P4:
            /* Set KB Mode to Neumeric Mode */
            UifACChgKBType(UIF_NEUME_KB);
            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT10);             /* Set Display Format and Set Max. Input Digit */
 
            /* Reset Alpha Descriptor */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);

            UieAccept();                                                /* Return to UifACXXXFunction() */
            return(SUCCESS);
		}
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifACChangeKB2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Change Alpha KB to Numeric KB 2 Mode  
*===============================================================================
*/
SHORT UifACChangeKB2(KEYMSG *pKeyMsg) 
{
	MAINTREDISP     ReDisplay = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P4);                                /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        /* Check Digit */
        if (pKeyMsg->SEL.INPUT.uchLen) {                                /* W/ Amount Case */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P4:
            /* Set KB Mode to Neumeric Mode */
            UifACChgKBType(UIF_NEUME_KB);
            UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT10);             /* Set Display Format and Set Max. Input Digit */

            /* Reset Alpha Descriptor */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);

            UieAccept();                                                /* Return to UifACXXXFunction() */
            return(SUCCESS);
		}
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: SHORT UifACShift(KEYMSG *pKeyMsg) 
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
SHORT UifACShift(KEYMSG *pKeyMsg) 
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();    /* get system config */

	switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_P2);                            /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_P2:
            /* Set KB Mode to ALPHA Mode */
            if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION){		/* Conventional Type */
                if (usUifACControl & UIF_SHIFT_PAGE) {                  /* Now is Shift Page Case */
                    UieSetCvt(CvtAlphC1);                               /* Set Standard Alpha Conventional KB Table */
                    usUifACControl &= ~UIF_SHIFT_PAGE;                  /* Reset Shift Page Bit */
                } else {
                    UieSetCvt(CvtAlphC2);                               /* Set Shift Alpha Conventional KB Table */      
                    usUifACControl |= UIF_SHIFT_PAGE;                   /* Set Shift Page Bit */
                }
                UieSetFsc(NULL);                                        /* Set FSC Table Pointer to NULL */
                UieKeepMessage();                                       /* Keep Input Data for UI Engine */
                return(SUCCESS);
			} else if(pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {/* Wedge 64 key Type */
                if (usUifACControl & UIF_SHIFT_PAGE) {                  /* Now is Shift Page Case */
                    UieSetCvt(CvtAlph5932_68_1);                               /* Set Standard Alpha Conventional KB Table */
                    usUifACControl &= ~UIF_SHIFT_PAGE;                  /* Reset Shift Page Bit */
                } else {
                    UieSetCvt(CvtAlph5932_68_2);                               /* Set Shift Alpha Conventional KB Table */      
                    usUifACControl |= UIF_SHIFT_PAGE;                   /* Set Shift Page Bit */
                }
                UieSetFsc(NULL);                                        /* Set FSC Table Pointer to NULL */
                UieKeepMessage();                                       /* Keep Input Data for UI Engine */
                return(SUCCESS);
			} else if(pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {/* Wedge 78 key Type */
                if (usUifACControl & UIF_SHIFT_PAGE) {                  /* Now is Shift Page Case */
                    UieSetCvt(CvtAlph5932_78_1);                               /* Set Standard Alpha Conventional KB Table */
                    usUifACControl &= ~UIF_SHIFT_PAGE;                  /* Reset Shift Page Bit */
                } else {
                    UieSetCvt(CvtAlph5932_78_2);                               /* Set Shift Alpha Conventional KB Table */      
                    usUifACControl |= UIF_SHIFT_PAGE;                   /* Set Shift Page Bit */
                }
                UieSetFsc(NULL);                                        /* Set FSC Table Pointer to NULL */
                UieKeepMessage();                                       /* Keep Input Data for UI Engine */
                return(SUCCESS);
			} else {                                                    /* Micromotion Type */
                return(LDT_SEQERR_ADR);                         
			}
		}
		// drop through and do default processing
    default:
        return(UifACDefProc(pKeyMsg));                                  /* Execute Default Procedure */
    }
}
    
/*
*=============================================================================
**  Synopsis: SHORT UifACExitFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C Exit Function Mode 
*===============================================================================
*/
SHORT UifACExitFunction(KEYMSG *pKeyMsg) 
{
	MAINTREDISP     ReDisplay = {0};

    switch(pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_AC);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        if (pKeyMsg->SEL.INPUT.uchLen) {                    /* W/ Amount */
            return(LDT_SEQERR_ADR);
        }
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_AC:
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);

            /* Reset Descriptor */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
            MaintReDisp(&ReDisplay);

            UieExit(aACEnter);                              /* Return to UifACEnter() */
            return(SUCCESS);
        }
		// drop through and do default processing.
    default:
        return(UifACDefProc(pKeyMsg));                                      /* Execute Default Procedure */
    }
}
    

/*
*=============================================================================
**  Synopsis: VOID UifACChgKBType(UCHAR uchKBType) 
*
*       Input:  uchKBType       : UIF_ALPHA_KB ( Alpha KB Type )
*                               : UIF_NEUME_KB ( Neumeric KB Type) 
*      Output:  nothing                         
**  Return:     nothing         
*               
**  Description: Change KB Type 
*===============================================================================
*/
USHORT UifChangeKeyboardType (UCHAR  uchKBType)
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();    /* get system config */
	MAINTREDISP     ReDisplay = {0};
	USHORT          usKBMode = 0;

	usKBMode = pSysConfig->uchKBMode;  // UifACChgKBType() 

    if ((uchKBType == UIF_ALPHA_KB) && (usKBMode != UIF_ALPHA_KB)) {
        /* Set KB Mode to ALPHA Mode */
        if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* Conventional Type */
            UieSetCvt(CvtAlphC1);                                   /* Set Standard Conventional KB Table */                        
            usUifACControl &= ~UIF_SHIFT_PAGE;                      /* Reset Shift Page Bit */
		} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
            UieSetCvt(CvtAlphM);                                    /* set Micromotion KB Table */
		} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
            UieSetCvt(CvtAlphM);                                    /* set Micromotion KB Table */
		} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
            UieSetCvt(CvtAlph5932_68_1);                                   /* Set 5932 Wedge 64 KB Table */                        
            usUifACControl &= ~UIF_SHIFT_PAGE;                      /* Reset Shift Page Bit */
		} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
            UieSetCvt(CvtAlph5932_78_1);                                   /* Set 5932 Wedge 78 KB Table */                        
            usUifACControl &= ~UIF_SHIFT_PAGE;                      /* Reset Shift Page Bit */
		} else {
            __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
		}

        UieSetFsc(NULL);                                            /* Set FSC Table Pointer to NULL */

        /* Set Single/Double Wide Position */
        //UieEchoBackWide(0, 19);                                     /* RFC 01/30/2001 */
        
        if (pSysConfig->uchOperType == DISP_2X20) {                 
            UieEchoBackWide(0, 19);
        } else if (pSysConfig->uchOperType == DISP_10N10D) {
            UieEchoBackWide(0, 9);
        } else {
            UieEchoBackWide(0,39);   // LCD
        }
        
		UieSetKBMode (UIF_ALPHA_KB);
    } else if((uchKBType == UIF_NEUME_KB) && (usKBMode != UIF_NEUME_KB))
	{
        /* Set KB Mode to Neumeric Mode */
		if(UieMode.uchReally == UIE_POSITION5)
		{
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
                __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
			}
			
			//SR 661, by moving this into the area where we change it only when we are in program mode
			//we alleviate the problem where the scroll up and down buttons wont work. JHHJ
			UieSetStatusKeyboard();
		}else
		{
			FSCTBL   *pData; 
			USHORT   usTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO((*pSysConfig).usTerminalPositionFromName);

			if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* Conventional Type */
				UieSetCvt(CvtRegC);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
				UieSetCvt(CvtRegM);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
				UieSetCvt(CvtRegTouch);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 68 key 5932 Wedge keyboard
				UieSetCvt(CvtReg5932_68);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
				UieSetCvt(CvtReg5932_78);                         
			} else {
                __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
			}

			// guard the terminal position to ensure it is in correct range, default is we be Master
			if (! (PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) >= 1 && PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) <= PIF_NET_MAX_IP)) {
				usTerminalPosition = 1;
			}
			//SR 661, by moving this into the area where we change it only when we are not in program mode
			//we alleviate the problem where the scroll up and down buttons wont work. JHHJ
			pData = (FSCTBL *)&ParaFSC[Para.TerminalInformation[usTerminalPosition - 1].uchMenuPageDefault - 1];
			UieSetFsc(pData);
		}

		UieSetKBMode (UIF_NEUME_KB);
    }

	return usKBMode;
}

USHORT UifACChgKBType(UCHAR uchKBType) 
{
    SYSCONFIG CONST *pSysConfig = PifSysConfig();    /* get system config */
	MAINTREDISP     ReDisplay = {0};
	USHORT          usKBMode = 0;

	usKBMode = pSysConfig->uchKBMode;  // UifACChgKBType() 

    if ((uchKBType == UIF_ALPHA_KB) && (usKBMode != UIF_ALPHA_KB)) {
        /* Set KB Mode to ALPHA Mode */
        if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* Conventional Type */
            UieSetCvt(CvtAlphC1);                                   /* Set Standard Conventional KB Table */                        
            usUifACControl &= ~UIF_SHIFT_PAGE;                      /* Reset Shift Page Bit */
		} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
            UieSetCvt(CvtAlphM);                                    /* set Micromotion KB Table */
		} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
            UieSetCvt(CvtAlphM);                                    /* set Micromotion KB Table */
		} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
            UieSetCvt(CvtAlph5932_68_1);                                   /* Set 5932 Wedge 64 KB Table */                        
            usUifACControl &= ~UIF_SHIFT_PAGE;                      /* Reset Shift Page Bit */
		} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
            UieSetCvt(CvtAlph5932_78_1);                                   /* Set 5932 Wedge 78 KB Table */                        
            usUifACControl &= ~UIF_SHIFT_PAGE;                      /* Reset Shift Page Bit */
		} else {
            __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
		}

        UieSetFsc(NULL);                                            /* Set FSC Table Pointer to NULL */

        /* Set Alpha Descriptor */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_ALPHA;
        MaintReDisp(&ReDisplay);

        /* Set Single/Double Wide Position */
        //UieEchoBackWide(0, 19);                                     /* RFC 01/30/2001 */
        
        if (pSysConfig->uchOperType == DISP_2X20) {                 
            UieEchoBackWide(0, 19);
        } else if (pSysConfig->uchOperType == DISP_10N10D) {
            UieEchoBackWide(0, 9);
        } else {
            UieEchoBackWide(0,39);   // LCD
        }
        
		UieSetKBMode (UIF_ALPHA_KB);
    } else if((uchKBType == UIF_NEUME_KB) && (usKBMode != UIF_NEUME_KB))
	{
        /* Set KB Mode to Neumeric Mode */
		if(UieMode.uchReally == UIE_POSITION5)
		{
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
                __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
			}
			
			//SR 661, by moving this into the area where we change it only when we are in program mode
			//we alleviate the problem where the scroll up and down buttons wont work. JHHJ
			UieSetStatusKeyboard();
		}else
		{
			FSCTBL   *pData; 
			USHORT   usTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO((*pSysConfig).usTerminalPositionFromName);

			if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* Conventional Type */
				UieSetCvt(CvtRegC);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
				UieSetCvt(CvtRegM);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
				UieSetCvt(CvtRegTouch);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 68 key 5932 Wedge keyboard
				UieSetCvt(CvtReg5932_68);                         
			} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
				UieSetCvt(CvtReg5932_78);                         
			} else {
                __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
			}

			// guard the terminal position to ensure it is in correct range, default is we be Master
			if (! (PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) >= 1 && PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName) <= PIF_NET_MAX_IP)) {
				usTerminalPosition = 1;
			}
			//SR 661, by moving this into the area where we change it only when we are not in program mode
			//we alleviate the problem where the scroll up and down buttons wont work. JHHJ
			pData = (FSCTBL *)&ParaFSC[Para.TerminalInformation[usTerminalPosition - 1].uchMenuPageDefault - 1];
			UieSetFsc(pData);
		}

        /* Reset Alpha Descriptor */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);

		UieSetKBMode (UIF_NEUME_KB);
    }

	return usKBMode;
}

/*
*=============================================================================
**  Synopsis: VOID UifACInit( VOID ) 
*
*       Input:  nothing        
*                                
*      Output:  nothing                         
**  Return:     nothing         
*               
**  Description: Clear Own Work Area 
*===============================================================================
*/
VOID UifACInit( VOID ) 
{
    /* Clear UI Supervisor Mode Work Area */
    usUifACControl = 0;                 /* Control Data Save Area */
    uchUifACDataType = 0;               /* Data Type Save Area */
    uchUifACRstType = 0;                /* Reset Type Save Area */
    uchUifACRptType = 0;                /* Report Type Save Area */

    uchUifACRptOnOffMld = RPT_DISPLAY_OFF; /* MLD Display Not Out */
    uchUifTrailerOnOff = 0;             /* MLD Display Not Out Trailer */
    uchUifACMenuNumber = 0;             /* AC Mode Menu Number (0~3) */
	uchUifACRptOnOffFile = RPT_PRINTFILE_OFF; /* Send to printer not file */ //ESMITH PRTFILE

    /* Clear Report/Maintenance Module Work */
    RptInitialize();
    MaintInit();
	MaintSetPrintMode(PRT_SENDTO_PRINTER);			/* Set Print send to printer */
}

/*
*=============================================================================
**  Synopsis: SHORT UifACCancel(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*       Output: nothing
*
**  Return: SUCCESS : Successful 
*               
**  Description: Supervisor Cancel Mode 
*===============================================================================
*/
SHORT UifACCancel(KEYMSG *pKeyMsg) 
{
    switch (pKeyMsg->uchMsg) {
    case UIM_INIT:
        UieOpenSequence(aszSeqACFSC_CANCEL);                    /* Register Key Sequence */
        return(SUCCESS);

    case UIM_INPUT:
        switch (pKeyMsg->SEL.INPUT.uchMajor) {
        case FSC_CANCEL:
            MaintFin(CLASS_MAINTTRAILER_PRTSUP);    /* Execute Finalize Procedure */
            usUifACControl &= ~UIF_SHIFT_PAGE;      /* Reset Shift Page Bit */
            UieExit(aACModeIn);                     /* Return to UifACModeIn() */
            return(SUCCESS);
        }
		// drop through and do default processing
    default:
        return(UieDefProc(pKeyMsg));    /* Execute Uie Default Procedure */
    }
}
/*
*=============================================================================
**  Synopsis: VOID UifACMenu(UCHAR MenuPageNumber) 
*
*       Input:  MenuPageNumber  :    MenuPageNumber
*       Output: nothing
*
**  Return: nothing
*               
**  Description: Supervisor AC Code Menu Display
*===============================================================================
*/
VOID UifACMenu(UCHAR uchPageNumber) 
{
	SYSCONFIG CONST * pSysConfig = PifSysConfig(); 
	USHORT            usTerminalPosition;
	PARATERMINALINFO  TermInfo = {0};

	/* SR 474 JHHJ, added this so that the user cannot edit Menu Pages
	when we are on a touch screen terminal.  The user does not need
	to change the Menu Page for a touch screen because they are set to specific location
	so that FrameworkWndButton uses them properly.
	This code removes the MLD for AC 4 and 5*/
	usTerminalPosition = TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);

	if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
		// indicate this is a Disconnected Statellite type of terminal.
		usTerminalPosition = 50;
	}

	if(TermInfo.TerminalInfo.uchMenuPageDefault < STD_TOUCH_MENU_PAGE)
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			if (usTerminalPosition <= PIF_NET_MAX_IP)
				ACCurrentMenu = ACEntryMenu + uchPageNumber;
			else
				ACCurrentMenu = ACEntryDisconnectedMenu + uchPageNumber;
		} else if (usTerminalPosition > 2) {
			if (usTerminalPosition <= PIF_NET_MAX_IP) {   /* Standard Sattelite */
				ACCurrentMenu = ACEntryMenuSatt + uchPageNumber;
			}
			else {
				// terminal is a Disconnected Satellite
				ACCurrentMenu = ACEntryMenuDisconnectedSatt + uchPageNumber;
			}
		} else if ( usBMOption == 1 ) {    /* saratoga */
			ACCurrentMenu = ACEntryMenuBMast + uchPageNumber;
		} else {
			ACCurrentMenu = ACEntryMenuSatt + uchPageNumber;
		}
	} else{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			if (usTerminalPosition <= PIF_NET_MAX_IP)
				ACCurrentMenu = ACEntryMenuTouch + uchPageNumber;
			else
				ACCurrentMenu = ACEntryMenuDisconnectedTouch + uchPageNumber;
		} else if (usTerminalPosition > 2) {   /* Sattelite */
			if (usTerminalPosition <= PIF_NET_MAX_IP) {   /* Standard Sattelite */
				ACCurrentMenu = ACEntryMenuSattTouch + uchPageNumber;
			} else {
				// terminal is a Disconnected Satellite
				ACCurrentMenu = ACEntryMenuDisconnectedSattTouch + uchPageNumber;
			}
		} else if ( usBMOption == 1 ) {    /* saratoga */
			ACCurrentMenu = ACEntryMenuBMastTouch + uchPageNumber;
		}else {
			ACCurrentMenu = ACEntryMenuSattTouch + uchPageNumber;
		}
	}

	uchUifACMenuMore = UifACPGMLDDispCom(ACCurrentMenu);

	if (uchUifACMenuMore)
		uchUifACMenuMore += uchPageNumber;
}

UCHAR UifACMenuBack(UCHAR uchPageNumber) 
{
	SYSCONFIG CONST  *pSysConfig = PifSysConfig(); 
	USHORT            usTerminalPosition;
	PARATERMINALINFO  TermInfo = {0};


	/* SR 474 JHHJ, added this so that the user cannot edit Menu Pages
	when we are on a touch screen terminal.  The user does not need
	to change the Menu Page for a touch screen because they are set to specific location
	so that FrameworkWndButton uses them properly.
	This code removes the MLD for AC 4 and 5*/
	usTerminalPosition = TermInfo.uchAddress = PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName);
	ParaTerminalInfoParaRead (&TermInfo);

	if (CstIamDisconnectedSatellite() == STUB_SUCCESS) {
		// indicate this is a Disconnected Statellite type of terminal.
		usTerminalPosition = 50;
	}

	if (uchPageNumber > 0)
		uchPageNumber--;

	if(TermInfo.TerminalInfo.uchMenuPageDefault < STD_TOUCH_MENU_PAGE)
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			while (uchPageNumber > 0 && ACEntryMenu[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			ACCurrentMenu = ACEntryMenu + uchPageNumber;
		}else if (usTerminalPosition > 2) {
			if (usTerminalPosition <= PIF_NET_MAX_IP) {   /* Standard Sattelite */
				while (uchPageNumber > 0 && ACEntryMenuSatt[uchPageNumber - 1]->usAddress) {
					uchPageNumber--;
				}
				ACCurrentMenu = ACEntryMenuSatt + uchPageNumber;
			}
			else {
				// terminal is a Disconnected Satellite
				while (uchPageNumber > 0 && ACEntryMenuDisconnectedSatt[uchPageNumber - 1]->usAddress) {
					uchPageNumber--;
				}
				ACCurrentMenu = ACEntryMenuDisconnectedSatt + uchPageNumber;
			}
		} else if ( usBMOption == 1 ) {    /* saratoga */
			while (uchPageNumber > 0 && ACEntryMenuBMast[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			ACCurrentMenu = ACEntryMenuBMast + uchPageNumber;
		}else {
			while (uchPageNumber > 0 && ACEntryMenuSatt[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			ACCurrentMenu = ACEntryMenuSatt + uchPageNumber;
		}
	} else
	{
		if (CliCheckAsMaster() == STUB_SUCCESS) {   /* Master */
			while (uchPageNumber > 0 && ACEntryMenuTouch[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			ACCurrentMenu = ACEntryMenuTouch + uchPageNumber;
		}else if (usTerminalPosition > 2) {   /* Sattelite */
			if (usTerminalPosition <= PIF_NET_MAX_IP) {   /* Standard Sattelite */
				while (uchPageNumber > 0 && ACEntryMenuSattTouch[uchPageNumber - 1]->usAddress) {
					uchPageNumber--;
				}
				ACCurrentMenu = ACEntryMenuSattTouch + uchPageNumber;
			}
			else {
				// terminal is a Disconnected Satellite
				while (uchPageNumber > 0 && ACEntryMenuDisconnectedSattTouch[uchPageNumber - 1]->usAddress) {
					uchPageNumber--;
				}
				ACCurrentMenu = ACEntryMenuDisconnectedSattTouch + uchPageNumber;
			}
		} else if ( usBMOption == 1 ) {    /* saratoga */
			while (uchPageNumber > 0 && ACEntryMenuBMastTouch[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			ACCurrentMenu = ACEntryMenuBMastTouch + uchPageNumber;
		}else {
			while (uchPageNumber > 0 && ACEntryMenuSattTouch[uchPageNumber - 1]->usAddress) {
				uchPageNumber--;
			}
			ACCurrentMenu = ACEntryMenuSattTouch + uchPageNumber;
		}
	}

	uchUifACMenuMore = UifACPGMLDDispCom(ACCurrentMenu);

	if (uchUifACMenuMore)
		uchUifACMenuMore += uchPageNumber;

	return uchPageNumber;
}

/*
*=============================================================================
**  Synopsis: VOID UifACRedisp(VOID) 
*
*       Input:  nothing
*       Output: nothing
*
**  Return: nothing
*               
**  Description: Supervisor LCD Redisplay
*===============================================================================
*/
VOID UifACRedisp(VOID) 
{
    MAINTDSPFLINE   MaintDspFLine = {0};
    MAINTREDISP     ReDisplay = {0};

    switch(uchUifReDispMode){
    case STS_AC_MODE:
        /* Display "SUPER" Character */
        MaintDspFLine.uchMajorClass = CLASS_MAINTDSPFLINE; 
        MaintDspFLine.uchMinorClass = CLASS_MAINTPREMD_OP;
		RflGetLead (MaintDspFLine.aszMnemonics, LDT_SUPMSG_ADR);
        DispWrite(&MaintDspFLine);                   /* call DispSupFLine() */ 

        UieEchoBack(UIE_ECHO_NO, UIF_DIGIT10); /* No Echo Back and Set Max Input Digit */
        /* Reset All Descriptor */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);
        break;

    case STS_AC_ENTER:
        MaintDisp(0, 0, 0, 0, 0, 0, 0L, LDT_AC_ADR);

        UieEchoBack(UIE_ECHO_ROW0_NUMBER, UIF_DIGIT13); /* Saratoga  Start Echo Back and Set Max Input Digit */

        /* Reset All Descriptor */
        ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
        ReDisplay.uchMinorClass = CLASS_MAINTREDISP_CLR;
        MaintReDisp(&ReDisplay);
        if(uchUifACRptOnOffMld == RPT_DISPLAY_ON){ /* MLD Display Out */
            ReDisplay.uchMajorClass = CLASS_MAINTREDISP;
            ReDisplay.uchMinorClass = CLASS_MAINTREDISP_NORECEIPT;
            MaintReDisp(&ReDisplay);
        }
        break;

    default:
        break;
    }
    return;
}


/*
*===========================================================================
** Synopsis:    SHORT CheckSuperNumber( KEYMSG *pData, UIFDIAEMPLOYEE  *pUifDiaEmployee )
*               
*     Input:    
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS: supervisor number is exist
*               LDT_SEQERR_ADR: sequence error
*               LDT_KEYOVER_ADR: wrong data
*
** Description: This function sets up and calls mode-in function at supervisor mode.
*===========================================================================
*/
SHORT CheckSuperNumber(KEYMSG *pData, UIFDIAEMPLOYEE  *pUifDiaEmployee)
{
	SHORT        sError = LDT_SEQERR_ADR;

	switch (pData->SEL.INPUT.uchMajor) {
		case FSC_P1:                    // P1 key is same as Num Type key (FSC_NUM_TYPE) as used in several places so be consistent
        case FSC_NUM_TYPE:
        case FSC_AC:
            /* break; fall through to MSR source */
        case FSC_MSR:
			{
				MAINTMODEIN   ModeIn = {0};
				SHORT         sRetStatus;
				extern ULONG  ulMaintSupNumber;            /* supervisor number */

				ModeIn.uchStatus = 0;               /* Set W/ Amount Status */
				if (pData->SEL.INPUT.uchMajor == FSC_MSR) {
					sRetStatus = UifTrackToUifDiaEmployee (pData->SEL.INPUT.DEV.MSR.auchTrack2, pData->SEL.INPUT.DEV.MSR.uchLength2, pUifDiaEmployee);
					if (pData->SEL.INPUT.DEV.MSR.uchPaymentType == BIOMETRICS_ENTRY) {
						pUifDiaEmployee->ulStatusFlags |= UIFREGMISC_STATUS_METHOD_BIOMET;
					} else {
						pUifDiaEmployee->ulStatusFlags |= UIFREGMISC_STATUS_METHOD_SWIPE;
					}
				} else {
					// assume that this is an Amtrak type of card
					pUifDiaEmployee->usSwipeOperation = MSR_ID_GENERIC;
					pUifDiaEmployee->ulSwipeEmployeeId = _ttol(pData->SEL.INPUT.aszKey);
					pUifDiaEmployee->ulSwipeEmployeePin = 0;
					sRetStatus = UifFillInEmployeeRecord (pUifDiaEmployee);
					pUifDiaEmployee->ulStatusFlags |= UIFREGMISC_STATUS_METHOD_MANUAL;
				}

				if (sRetStatus != UIF_SUCCESS) {
					// if a Sign-in into Supervisor Mode does not work it may be due to a communication
					// problem with the Master Terminal.
					// we therefor provide a means to Sign-in into Supervisor Mode on a brand new default Satellite
					// terminal so that an AC75 can be done to transfer data from the Master Terminal.
					// see the ParaSupLevel table in UNINIRAM UNINT ParaDefault where we define a special
					// Supervisor Mode id of 65535 that can be used to Sign-in into Supervisor Mode.
					// this default Para will be overwritten the first time AC75 is done.
					if (pUifDiaEmployee->ulSwipeEmployeeId > 65000 && pUifDiaEmployee->ulSwipeEmployeeId < 66000) {
						PARASUPLEVEL    ParaSupLevelStruct = {0};

				        ParaSupLevelStruct.uchMajorClass = CLASS_PARASUPLEVEL;
						ParaSupLevelStruct.uchAddress = 0;
						ParaSupLevelStruct.usSuperNumber = (USHORT)pUifDiaEmployee->ulSwipeEmployeeId;
						if (CliParaSupLevelRead(&ParaSupLevelStruct) == SUCCESS) {  /* read data */
							pUifDiaEmployee->usSwipeOperationSecondary = MSR_ID_SUPERVISOR;
							pUifDiaEmployee->ulSwipeSupervisorNumber = 899;
						} else {
							return LDT_NTINFL_ADR;
						}
					} else if (sRetStatus == ETK_NOT_IN_FILE)
						return LDT_NTINFL_ADR;
					else
						return LDT_SEQERR_ADR;
				}

				memset (pData->SEL.INPUT.aszKey, 0, sizeof(pData->SEL.INPUT.aszKey));
				if (pUifDiaEmployee->usSwipeOperation == MSR_ID_GENERIC) {
					if (pUifDiaEmployee->usSwipeOperationSecondary != MSR_ID_SUPERVISOR)
						return(LDT_INVALID_SUP_NUM);

					pData->SEL.INPUT.aszKey[0] = _T('5');  // first character is MSR_ID_SUPERVISOR as we are replicating Supervisor card swipe
					_ltot (pUifDiaEmployee->ulSwipeSupervisorNumber, pData->SEL.INPUT.aszKey + 1, 10);
					pData->SEL.INPUT.uchLen = 6;
					ModeIn.ulModeInNumber = pUifDiaEmployee->ulSwipeSupervisorNumber;
				}
				else {
					int      i;

					if (!pData->SEL.INPUT.DEV.MSR.uchLength2) return(LDT_KEYOVER_ADR);

					for (i = 0; i < UIF_DIGIT7; i++) {
						if ((pData->SEL.INPUT.DEV.MSR.auchTrack2[i] < '0') || (pData->SEL.INPUT.DEV.MSR.auchTrack2[i] > '9')) {
							break;
						}
					}
					if (i >= UIF_DIGIT7) {
						i = UIF_DIGIT6;
					}
					_tcsncpy(&(pData->SEL.INPUT.aszKey[0]), &(pData->SEL.INPUT.DEV.MSR.auchTrack2[0]), i);
					pData->SEL.INPUT.uchLen = (UCHAR)i;
				}

				{
					ULONG  ulSecretCodeOrPin = 0;

					if (pUifDiaEmployee->ulStatusFlags & UIFDIAEMPLOYEE_STATUS_SUP_PIN_REQ){
						sRetStatus = ItemCasSecretCode(&ulSecretCodeOrPin);
						if(sRetStatus == UIF_SUCCESS){
							if (pUifDiaEmployee->ulSwipeEmployeePin == 0) {
								ItemSetCasSecretCode (pUifDiaEmployee->ulSwipeEmployeeId, ulSecretCodeOrPin, 0);
							} else if(pUifDiaEmployee->ulSwipeEmployeePin != ulSecretCodeOrPin) {
								return LDT_INVALID_SUP_NUM;
							}
						} else {
							return LDT_INVALID_SUP_NUM;
						}
					}
				}

				/* Execute Supervisor Mode In Procedure */
				ModeIn.uchMajorClass = CLASS_MAINTMODEIN;           /* Set Major Class Data */
				ModeIn.ulModeInNumber = pUifDiaEmployee->ulSwipeSupervisorNumber;
                ModeIn.uchStatus |= MAINT_MSR_DATA;
				sError = MaintModeInSup(&ModeIn);
				if (sError == SUCCESS) {
					char  xBuff[128];

					sprintf ( xBuff, "==ACTION: Supervisor Sign-in %8.8u", pUifDiaEmployee->ulSwipeEmployeeId);
					NHPOS_NONASSERT_NOTE("==ACTION", xBuff);
				}
            }
			break;
	}

    return(sError);
}

/**** End of File ****/
