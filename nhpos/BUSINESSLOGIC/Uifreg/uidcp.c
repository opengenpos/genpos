/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-2000       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Charge Posting Compulsory Module                         
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application         
* Program Name: UIDPC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               UifDiaCP1()     : Room Charge Compulsory Dialog for room #
*               UifDiaCP2()     : Room Charge Compulsory Dialog for guest id
*               UifDiaCP3()     : Room/Account Charge Compulsory Dialog for guest line #
*                               : and account #
*               UifDiaCP4()     : Pre-/Credit Auth. Compulsory Dialog for expiration date
*				UifDiaCP5()		: Compulsory Dialog for Guest Line# for EPT
*				UifDiaCP6()		: Compulsory Dialog for Reference #
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Sep-13-93:00.00.01: K.You     : initial                                   
* Sep-13-93:00.00.01: K.You     : initial                                   
* May-20-94:02.05.00: Yoshiko.J : Q-010 corr. (mod UifDiaCP4())
* May-22-94:02.05.00: K.You     : Q-010 corr. (mod UifDiaCP1,2,3())
* May-25-94:02.05.00: K.You     : chg. Exp. Date Entry (mod UifDiaCP4)
* Apl-15-96:03.01.04: M.Ozawa   : corrected the problem of shift page
* May-27-96:03.01.07: M.Ozawa   : corrected the problem of expiration date
* Feb-16-00:01.00.00: hrkato    : Saratoga
* Apr-28-04:		R.Herrington: Reference # entry
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
#include <tchar.h>
#include <string.h>
#include <stdio.h>

#include <ecr.h>
#include <uie.h>
#include <fsc.h>
#include <pif.h>
#include <cvt.h> 
#include <plu.h> 
#include <regstrct.h>
#include <uireg.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <item.h>
#include <display.h>
#include "uiregloc.h"
#include "uifpgequ.h"
#include "pifmain.h"
#include "EEptTrans.h"
#include	"..\Sa\UIE\uiedev.h"
#include "uifsup.h"
                                                                                
/*
*===========================================================================
*   Code Area Data
*===========================================================================
*/

UIMENU FARCONST aChildRegDiaCP1[] = {{UifDiaCPEntry1, CID_DIACPENTRY1},{NULL, 0}};
UIMENU FARCONST aChildRegDiaCP2[] = {{UifDiaCPEntry2, CID_DIACPENTRY2},{NULL, 0}};
UIMENU FARCONST aChildRegDiaCP3[] = {{UifDiaCPEntry3, CID_DIACPENTRY3},{NULL, 0}};
UIMENU FARCONST aChildRegDiaCP4[] = {{UifDiaCPEntry4, CID_DIACPENTRY4},{NULL, 0}};
UIMENU FARCONST aChildRegDiaCP5[] = {{UifDiaCPEntry5, CID_DIACPENTRY5},{NULL, 0}};
UIMENU FARCONST aChildRegDiaCP6[] = {{UifDiaCPEntry6, CID_DIACPENTRY6},{NULL, 0}};

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

typedef struct {
	UCHAR    uchKeyType;
	CVTTBL  *pCvtTable;
} KeyTypeToCVT;

static KeyTypeToCVT * UifKeyTypeToCvtSet (KeyTypeToCVT *x)
{
	SYSCONFIG CONST  *pSysConfig = PifSysConfig();

	for (; x->pCvtTable; x++) {
		if (x->uchKeyType == pSysConfig->uchKeyType) { 
			UieSetCvt(x->pCvtTable);
			break;
		}
	}

	return x;
}

static KeyTypeToCVT     keyToCvtAlpha[] = {
	{ KEYBOARD_CONVENTION, CvtAlphC1 },        // 7448 conventional type, set standard conventional K/B table alphanumeric mode
	{ KEYBOARD_MICRO, CvtAlphM },              // 7448 Micromotion keyboard, set micromotion K/B table alphanumeric mode
	{ KEYBOARD_TOUCH, CvtAlphM },              // Touchscreen terminal uses Micromotion tables, set micromotion K/B table alphanumeric mode
	{ KEYBOARD_WEDGE_68, CvtAlph5932_68_1 },   // 64 key 5932 Wedge keyboard, set 5932 Wedge 64 K/B table alphanumeric mode
	{ KEYBOARD_WEDGE_78, CvtAlph5932_78_1 },   // 78 key 5932 Wedge keyboard, set 5932 Wedge 78 K/B table alphanumeric mode
//	{ KEYBOARD_TICKET, CvtRegTicket },         // NCR Big Ticket keyboard, set
//	{ KEYBOARD_PCSTD_102, NULL },              // standard PC 102 key keyboard, set
	{ 0, NULL }
};

static KeyTypeToCVT     keyToCvtNumeric[] = {
	{ KEYBOARD_CONVENTION, CvtRegC },       // 7448 conventional type, set standard conventional K/B table numeric mode
	{ KEYBOARD_MICRO, CvtRegM },            // 7448 Micromotion keyboard, set micromotion K/B table numeric mode
	{ KEYBOARD_TOUCH, CvtRegTouch },        // Touchscreen terminal uses Micromotion tables, set micromotion K/B table numeric mode
	{ KEYBOARD_WEDGE_68, CvtReg5932_68 },   // 64 key 5932 Wedge keyboard, set 5932 Wedge 64 K/B table numeric mode
	{ KEYBOARD_WEDGE_78, CvtReg5932_78 },   // 78 key 5932 Wedge keyboard, set 5932 Wedge 78 K/B table numeric mode
//	{ KEYBOARD_TICKET, CvtRegTicket },      // NCR Big Ticket keyboard, set
//	{ KEYBOARD_PCSTD_102, NULL },           // standard PC 102 key keyboard, set
	{ 0, NULL }
};

/*
*===========================================================================
** Synopsis:    SHORT UifDiaCP1(UIFDIADATA *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Charge Posting Compulsory for room #
*===========================================================================
*/

SHORT UifDiaCP1(UIFDIADATA *pData)
{
	SHORT            sRetStatus, sStatusSave;
    USHORT           usCtrlDevice;
    SYSCONFIG CONST  *pSysConfig = PifSysConfig();

    /* set K/B mode to alpha mode */

    UieEchoBack(UIE_ECHO_ROW0_AN, UIFREG_MAX_DIGIT5);       

    /* Reset Single/Double Wide Position */

    UieEchoBackWide(255, 0);

	/* set K/B mode to alphanumeric mode */

#if 1
	if (UifKeyTypeToCvtSet(keyToCvtAlpha)->pCvtTable == NULL)  __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm // set keyboard and break if incorrect
#else
    if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtAlphC1);                                   /* set standard conventional K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {      // 7448 Micromotion keyboard
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {   // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_68_1);                                   /* set 5932 Wedge 64 K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {   // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_78_1);                                   /* set 5932 Wedge 78 K/B table */                        
	} else {
		_asm int 3;
	}
#endif

    UieSetFsc(NULL);                                            /* set FSC table pointer to NULL */

    UieCreateDialog();                                          /* create dialog */
/*  UieCtrlDevice(UIE_ENA_MSR);                                 /  enable MSR */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaCP1);                               /* open Charge Posting entry */

    for (;;) {
		KEYMSG    Data = { 0 };

        UieGetMessage(&Data, 0x0000);                                   /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);                   /* check key */
        if (Data.SEL.INPUT.uchMajor == FSC_P5) {                /* cancel ? */
            break;
        }

        if (sRetStatus == UIFREG_ABORT) {       /* cancel ? */
            break;
		}
        else if (sRetStatus == UIF_DIA_SUCCESS) {
            if ((UifRegDiaWork.aszMnemonics[0] == 0)            /* Q-010 corr 5/22  */
             && (UifRegDiaWork.uchTrack2Len == 0)) {
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);
            } else {
                *pData = UifRegDiaWork;    /* set job code */
                sRetStatus = UIF_SUCCESS;       
                break;
            }
        }
    }

    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
/*  UieCtrlDevice(UIE_DIS_MSR);                                 /  disable MSR */
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieDeleteDialog();                                          /* delete dialog */
    
    /* set K/B mode to neumeric mode */

#if 1
	if (UifKeyTypeToCvtSet(keyToCvtNumeric)->pCvtTable == NULL)  __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm // set keyboard and break if incorrect
#else
	if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtRegC);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {      // 7448 Micromotion keyboard
        UieSetCvt(CvtRegM);                         
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {  // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtRegTouch);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {   // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtReg5932_68);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {   // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtReg5932_78);                         
	} else {
		_asm int 3;
	}
#endif

	/* corrected at 04/15/96 */
    /* UieSetFsc((FSCTBL FAR *)&ParaFSC1[uchDispCurrMenuPage-1]);  */
    UieSetFsc((FSCTBL FAR *)&ParaFSC[uchDispCurrMenuPage-1]);  /* set FSC table pointer */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);           /* set echo back */
    return(sRetStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaCP2(UIFDIADATA *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Charge Posting Compulsory for guest id 
*===========================================================================
*/
SHORT UifDiaCP2(UIFDIADATA *pData)
{
    SHORT            sRetStatus, sStatusSave;
    USHORT           usCtrlDevice;
    SYSCONFIG CONST  *pSysConfig = PifSysConfig();

    /* set K/B mode to alpha mode */
    UieEchoBack(UIE_ECHO_ROW0_AN, UIFREG_MAX_DIGIT2);       

    /* Reset Single/Double Wide Position */
    UieEchoBackWide(255, 0);

#if 1
	if (UifKeyTypeToCvtSet(keyToCvtAlpha)->pCvtTable == NULL)  __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm  // set keyboard and break if incorrect
#else
	if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtAlphC1);                                   /* set standard conventional K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtAlphM);                                    /* set micromotion K/B table */
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_68_1);                                   /* set 5932 Wedge 64 K/B table */                        
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtAlph5932_78_1);                                   /* set 5932 Wedge 78 K/B table */                        
	} else {
		_asm int 3;
	}
#endif

    UieSetFsc(NULL);                                            /* set FSC table pointer to NULL */

    UieCreateDialog();                                          /* create dialog */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaCP2);                               /* open Charge Posting entry */

    for (;;) {
		KEYMSG     Data = { 0 };

        UieGetMessage(&Data, 0x0000);                                   /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);                   /* check key */
        if (Data.SEL.INPUT.uchMajor == FSC_P5) {                /* cancel ? */
            break;
        }

        if (sRetStatus == UIFREG_ABORT) {       /* cancel ? */
            break;
		}
        else if (sRetStatus == UIF_DIA_SUCCESS) {
            if ((UifRegDiaWork.aszMnemonics[0] == 0)            /* Q-010 corr 5/22  */
             && (UifRegDiaWork.uchTrack2Len == 0)) {
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);
            } else {
                *pData = UifRegDiaWork;    /* set job code */
                sRetStatus = UIF_SUCCESS;       
                break;
            }
        }
    }

    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieDeleteDialog();                                          /* delete dialog */
    
    /* set K/B mode to neumeric mode */

#if 1
	if (UifKeyTypeToCvtSet(keyToCvtNumeric)->pCvtTable == NULL)  __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm   // set keyboard and break if incorrect
#else
	if (pSysConfig->uchKeyType == KEYBOARD_CONVENTION) {        /* conventional type */
        UieSetCvt(CvtRegC);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_MICRO) {     // 7448 Micromotion keyboard
        UieSetCvt(CvtRegM);                         
    } else if (pSysConfig->uchKeyType == KEYBOARD_TOUCH) {      // Touchscreen terminal uses Micromotion tables
        UieSetCvt(CvtRegTouch);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_68) {  // 64 key 5932 Wedge keyboard
        UieSetCvt(CvtReg5932_68);                         
	} else if (pSysConfig->uchKeyType == KEYBOARD_WEDGE_78) {  // 78 key 5932 Wedge keyboard
        UieSetCvt(CvtReg5932_78);                         
	} else {
		_asm int 3;
	}
#endif

    /* corrected at 04/15/96 */
    /* UieSetFsc((FSCTBL FAR *)&ParaFSC1[uchDispCurrMenuPage-1]);  */
    UieSetFsc((FSCTBL FAR *)&ParaFSC[uchDispCurrMenuPage-1]);   /* set FSC table pointer */
    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);           /* set echo back */
    return(sRetStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaCP5(UIFDIADATA *pData)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA  
*      Output:  nothing
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
** Description: Compulsory Dialog for Guest Line# for EPT       Saratoga
*===========================================================================
*/
SHORT   UifDiaCP5(UIFDIADATA *pData)
{
    SHORT   sRetStatus, sStatusSave;
    USHORT  usModeStatus;
    USHORT  usCtrlDevice;

    UieCreateDialog();                                  /* create dialog */
    usModeStatus = UieModeChange(UIE_DISABLE);
    usCtrlDevice = UieCtrlDevice(UIE_DIS_SCANNER|UIE_DIS_WAITER|UIE_DIS_MSR);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaCP5);                       /* open Charge Posting entry */

    for (;;) {
		KEYMSG  Data = { 0 };

        UieGetMessage(&Data, 0x0000);                           /* wait key entry */
        sRetStatus = UieCheckSequence(&Data);           /* check key */
        if (sRetStatus == UIFREG_ABORT) {
            break;
        } else if (sRetStatus == UIF_DIA_SUCCESS) {
            *pData = UifRegDiaWork;
            sRetStatus = UIF_SUCCESS;       
            break;
        }
    }
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);
    UieModeChange(usModeStatus);
    UieDeleteDialog();

    return(sRetStatus);
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaCP3(UIFDIADATA *pData, UCHAR uchMaxLen)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA  
*               uchMaxLen       : Max Digit for Input
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Room/Account Charge Compulsory Dialog for guest line # and account #
*===========================================================================
*/
SHORT UifDiaCP3(UIFDIADATA *pDia, UCHAR uchMaxLen)
{
	// Remove MSR event handlers as part of eliminating legacy MSR code for OpenGenPOS.
	// This is to eliminate the possibility of not meeting PCI-DSS account holder information
	// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
	// Datacap Out Of Scope control for electronic payment.
	//
	//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers
	return UIFREG_ABORT;
}

/*
*===========================================================================
** Synopsis:    SHORT UifDiaCP4(UIFDIADATA *pData, UCHAR uchMaxLen)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA
*               uchMaxLen       : Max Digit for Input
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Pre-/Credit Auth. Compulsory Dialog for expiration date
*===========================================================================
*/
SHORT UifDiaCP14(UIFDIADATA *pData, UCHAR uchMaxLen, UCHAR uchEchoType, UCHAR uchMsrEnable)
{
	// Remove MSR event handlers as part of eliminating legacy MSR code for OpenGenPOS.
	// This is to eliminate the possibility of not meeting PCI-DSS account holder information
	// security requirements. GenPOS Rel 2.3.0 retained legacy source but was using the
	// Datacap Out Of Scope control for electronic payment.
	//
	//    OpenGenPOS Rel 2.4.0  3/22/2021  Richard Chambers
	return UIFREG_ABORT;
}


SHORT UifDiaCP4(UIFDIADATA *pData, UCHAR uchMaxLen)
{
	return UifDiaCP14 (pData, uchMaxLen, UIE_ECHO_ROW0_REG, 1);
}
/*
*===========================================================================
** Synopsis:    SHORT UifDiaCP6(UIFDIADATA *pData, UCHAR uchMaxLen)
*
*       Input:  *pKeyMsg        : Pointer to Structure for UIFDIADATA
*               uchMaxLen       : Max Digit for Input
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
** Description: Compulsory Dialog for Reference #
*===========================================================================
*/

SHORT UifDiaCP6(UIFDIADATA *pData, UCHAR uchMaxLen, USHORT usInputType)
{
	USHORT  usUieGetErrorActionFlags = 0;
    USHORT  usCtrlDevice;
    SHORT   sRetStatus, sStatusSave;
	USHORT  usKBMode_Prev = 0x7FFF;

	/* Set KB Mode to ALPHA Mode */
	if ((usInputType & 0x00ff) == UIFREG_ALFANUM_INPUT) {
		usKBMode_Prev = UifChangeKeyboardType(UIF_ALPHA_KB);
	} else if ((usInputType & 0x00ff) == UIFREG_NUMERIC_INPUT) {
		usKBMode_Prev = UifChangeKeyboardType(UIF_NEUME_KB);
	}

	if (usInputType & UIFREG_FLAG_MACROPAUSE) {
		usUieGetErrorActionFlags = UIEGETMESSAGE_FLAG_MACROPAUSE;
	}

	UieCreateDialog();                                  /* create dialog */
    usCtrlDevice = UieCtrlDevice(UIE_DIS_WAITER);   /* disable waiter lock */
    sStatusSave = ItemOtherDifDisableStatus();      /* R3.1 for Beverage Dispenser */
    UieInitMenu(aChildRegDiaCP6);                       /* open Charge Posting entry */
    UieEchoBack(UIE_ECHO_ROW0_REG, uchMaxLen);          /* set echo back */

    for (;;) {
		KEYMSG  Data = { 0 };

        UieGetMessage(&Data, usUieGetErrorActionFlags); /* wait key entry, UifDiaCP6() Reference # */
        sRetStatus = UieCheckSequence(&Data);           /* check key */
        if (sRetStatus == UIFREG_ABORT) {               /* cancel ? */
            break;
        }
        else if (sRetStatus == UIF_DIA_SUCCESS) {    
            if (UifRegDiaWork.aszMnemonics[0] == 0) {   
                UieErrorMsg(LDT_SEQERR_ADR, UIE_WITHOUT);

            } else if ((_tcslen(UifRegDiaWork.aszMnemonics) > uchMaxLen)) {
                UieErrorMsg(LDT_KEYOVER_ADR, UIE_WITHOUT);
                sRetStatus = LDT_KEYOVER_ADR;       /* add, bug fix */
                break;

            } else {
                *pData = UifRegDiaWork;
                sRetStatus = UIF_SUCCESS;       
                break;
            }
        }
    }

    UieEchoBack(UIE_ECHO_ROW0_REG, UIFREG_MAX_INPUT);   /* set echo back */
    if (sStatusSave) {                  /* if previous status is enable */
        ItemOtherDifEnableStatus(1);    /* R3.1 for Beverage Dispenser */
    }
    UieCtrlDevice(usCtrlDevice);            /* enable waiter lock */
    UieDeleteDialog();                                  /* delete dialog */

    /* Set KB Mode to previous Mode if we changed it */
	if (usKBMode_Prev != 0x7FFF)
		UifChangeKeyboardType (usKBMode_Prev);

    UieSetFsc((FSCTBL FAR *)&ParaFSC[uchDispCurrMenuPage-1]);  /* set FSC table pointer */

    return(sRetStatus);
}
/****** End of Source ******/