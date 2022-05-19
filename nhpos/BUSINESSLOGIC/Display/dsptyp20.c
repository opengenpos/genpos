/*
************************************************************************************
**                                                                                **
**        *=*=*=*=*=*=*=*=*                                                       **
**        *  NCR 2170     *             NCR Corporation, E&M OISO                 **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9                  **
**    <|\/~               ~\/|>                                                   **
**   _/^\_                 _/^\_                                                  **
**                                                                                **
************************************************************************************
*===================================================================================
* Title       : Display Module
* Category    : Display, NCR 2170 US Hospitality Application
* Program Name: DSPTYP20.C
* ----------------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*	DispRegOpen20() : display format in operator open, major class ... "CLASS_UIFREGOPEN"
*	DispRegTransOpen20() : display format in transaction open, major class ... "CLASS_UIFREGTRANSOPEN"
*	DispRegSales20() : display format in sales, major class ... "CLASS_UIFREGSALES"
*	DispRegScale20() : display format in sales, major class ... "CLASS_REGDISP_SALES"
*	DispRegDisc20() : display format in discount, major class ... "CLASS_UIFREGDISC"
*	DispRegCoupon20() : display format in combination coupon, major class ... "CLASS_UIFREGCOUPON"
*	DispRegTotal20() : display format in total key, major class ... "CLASS_UIFREGTOTAL"
*	DispRegTender20() : display format in tender, major class ... "CLASS_UIFREGTENDER"
*	DispRegMisc20() : display format in misc. transaction, major class ... "CLASS_UIFREGMISC"
*	DispRegModifier20() : display format in modifier, major class ... "CLASS_UIFREGMODIFIER"
*	DispRegOther20() : display format in other, major class ... "CLASS_UIFREGOTHER"
*	DispRegOpeClose20() : display format in operator close, major class ... "CLASS_UIFREGOPECLOSE"
*	DispRegModeIn20() : display format in reg mode-in, major class ... "CLASS_UIFREGMODE"
*	DispRegDisp20() : display in modifier key entry, major class ... "CLASS_UIFREGDISP"
*	DispRegGetMnemo() : get the mnemonics to display, ( called in DispRegDisp20() )
*	DispRegMakeAmt() :  make amount line, ( called when minor class is, CLASS_UITAXMODIFIER 1 - 8, or CLASS_UIFDISP_VOID )
*	DispRegCompulsory20() : display lead-thru in compulsory, major class ... "CLASS_REGDISPCOMPLSRY"
*
* ----------------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 : R.Itoh    : initial
* Jun-21-93: 01.00.12 : R.Itoh    : add ETK feature to DispRegCompulsory20().
* Jul-02-93: 01.00.12 : K.You     : add post receipt feature.(mod. DispRegMisc20).
* Jul-15-93: 01.00.12 : K.You     : add pig rule feature.(mod. DispRegModifier20).
* Jan-19-94: 00.00.03 : K.You     : bug fixed DispRewrite().
* Apr-20-94: 00.00.04 : K.You     : bug fixed E-18.(mod. DispRegTender20)
* Mar-22-95: 03.00.00 : M.Ozawa   : add DispRegCoupon20()
* Jul-19-95: 03.00.01 : M.Ozawa   : add alpha mnemonic entry display
* Apr-02-96: 03.01.02 : M.Ozawa   : modify menu shift display

** NCR2171 **
* Aug-26-99: 01.00.00 : M.Teraki  :initial (for 2171)
* Nov-26-99: 01.00.00 : hrkato    :Saratoga
*** GenPOS **
* Oct-02-17 : 02.02.02  : R.Chambers  : new uchMinorClass CLASS_UIFDISP_DESCONLY.
*===================================================================================
*===================================================================================
* PVCS Entry
* ----------------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===================================================================================
*/
#include <tchar.h>
#include <string.h>
#include <stdlib.h>

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "csstbpar.h"
#include "maint.h"
#include "rfl.h"
#include "regstrct.h"
#include "display.h"
#include "disp20.h"

/*
*===========================================================================
** Synopsis:    VOID DispRegOpen20( REGDISPMSG *pData )
*
*     Input:    *pData      - pointer of the display data buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGOPEN"
*===========================================================================
*/
VOID DispRegOpen20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UICASHIERIN:         /* cashier sign-in */
    case CLASS_UIB_IN:              /* cashier B sign-in */
    case CLASS_UIWAITERIN:          /* waiter sign-in */
    case CLASS_UIWAITERLOCKIN:      /* waiter lock sign-In */
    case CLASS_UIBARTENDERIN:       /* bartender sign-in R3.1 */
    case CLASS_UICASINTIN:          /* cashier interrupt sign-in, R3.3 */
    case CLASS_UICASINTB_IN:        /* cashier interrupt B sign-in, R3.3 */
		RflGetLead (pData->aszStringData, LDT_WELCOME_ADR);     /* set welcome message */
        DispRegZeroMnemo(pData, C_DISP); /* display "0.00" and mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID DispRegTransOpen20( REGDISPMSG * pData )
*
*     Input:    *pData      - pointer of the display data buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGTRANSOPEN"
*===========================================================================
*/
VOID DispRegTransOpen20( REGDISPMSG * pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UINEWCHECK:          /* new check */
    case CLASS_UIREORDER:           /* reorder */
    case CLASS_UIADDCHECK1:         /* addcheck 1 */
    case CLASS_UIADDCHECK2:         /* addcheck 2 */
    case CLASS_UIADDCHECK3:         /* addcheck 3 */
    case CLASS_UIRECALL:            /* recall */
    case CLASS_UICASINTRECALL:      /* cashier interrupt recall, R3.3 */
		RflGetLead (pData->aszStringData, LDT_WELCOME_ADR);     /* set welcome message */
        DispRegAmtMnemo(pData, C_DISP); /* display amount & mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID DispRegSales20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGSALES"
*===========================================================================
*/
VOID DispRegSales20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UIDEPT:              /* Dept */
    case CLASS_UIPLU:               /* PLU */
    case CLASS_UIPRINTMODIFIER:     /* print modifier */
    case CLASS_UIMODDISC:           /* modifier discount */
    case CLASS_UIDEPTREPEAT:        /* dept repeat */
    case CLASS_UIPLUREPEAT:         /* PLU repeat */
    case CLASS_UIDISPENSER:         /* Beverage Dispenser Sales R3.1 */
        DispRegSales(pData, C_DISP | C_DISP_MNE_ITEM);  /* display amount and mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;

    case CLASS_UIPRICECHECK:        /* Price Check Key R3.1 */
        DispRegAmtMnemo(pData, 0);  /* display amount & mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID DispRegScale20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_REGDISP_SALES"
*===========================================================================
*/
VOID DispRegScale20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_REGDISP_SCALE2DIGIT: /* scalable item display (2 digits type) */
        DispRegScale(pData, _2_DIGIT, C_DISP);
                                    /* display amount and mnemonics */
        break;

    case CLASS_REGDISP_SCALE3DIGIT: /* scalable item display (3 digits type) */
        DispRegScale(pData, _3_DIGIT, C_DISP);
                                    /* display amount and mnemonics */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
    DispRegOpDscrCntr(pData, 0);    /* control descriptor (ope. display) */
    DispRegCuDscrCntr(pData);       /* control descriptor (cust. display) */
    DispRegOpEchoBack(pData);       /* make display format for echo back */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegDisc20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGDISC"
*===========================================================================
*/
VOID DispRegDisc20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UIITEMDISC1:         /* item discount */
    case CLASS_UIREGDISC1:          /* regular discount #1 */
    case CLASS_UIREGDISC2:          /* regular discount #2 */
    case CLASS_UIREGDISC3:          /* regular discount #3 */
    case CLASS_UIREGDISC4:          /* regular discount #4 */
    case CLASS_UIREGDISC5:          /* regular discount #5 */
    case CLASS_UIREGDISC6:          /* regular discount #6 */
    case CLASS_UICHARGETIP:         /* charge tip */
    case CLASS_UICHARGETIP2:        /* charge tip, V3.3 */
    case CLASS_UICHARGETIP3:        /* charge tip, V3.3 */
        DispRegAmtMnemo(pData, C_DISP | C_DISP_MNE_DISCOUNT); /* display amount & mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID DispRegDisc20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGCOUPON"
*===========================================================================
*/
VOID DispRegCoupon20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UICOMCOUPON:         /* combination coupon */
    case CLASS_UIUPCCOUPON:         /* combination coupon */
        DispRegAmtMnemo(pData, C_DISP | C_DISP_MNE_COUPON); /* display amount & mnemonics */
        break;

    case CLASS_UICOMCOUPONREPEAT:   /* combination coupon repeate */
        DispRegRepeatAmtMnemo(pData, C_DISP | C_DISP_MNE_COUPON); /* display amount & mnemonics */
    break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }

    DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
    DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
    DispRegOpEchoBack(pData);   /* make display format for echo back */

}

/*
*===========================================================================
** Synopsis:    VOID  DispRegTotal20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGTOTAL"
*===========================================================================
*/
VOID DispRegTotal20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UITOTAL1:            /* total #1(subtotal) */
    case CLASS_UITOTAL2:            /* total #2(check total) */
    case CLASS_UITOTAL3:            /* total #3 */
    case CLASS_UITOTAL4:            /* total #4 */
    case CLASS_UITOTAL5:            /* total #5 */
    case CLASS_UITOTAL6:            /* total #6 */
    case CLASS_UITOTAL7:            /* total #7 */
    case CLASS_UITOTAL8:            /* total #8 */
    case CLASS_UITOTAL9:            /* total #9,    Saratoga */
    case CLASS_UITOTAL10:           /* total #10,    Saratoga */
    case CLASS_UITOTAL11:           /* total #11,    Saratoga */
    case CLASS_UITOTAL12:           /* total #12,    Saratoga */
    case CLASS_UITOTAL13:           /* total #13,    Saratoga */
    case CLASS_UITOTAL14:           /* total #14,    Saratoga */
    case CLASS_UITOTAL15:           /* total #15,    Saratoga */
    case CLASS_UITOTAL16:           /* total #16,    Saratoga */
    case CLASS_UITOTAL17:           /* total #17,    Saratoga */
    case CLASS_UITOTAL18:           /* total #18,    Saratoga */
    case CLASS_UITOTAL19:           /* total #19,    Saratoga */
    case CLASS_UITOTAL20:           /* total #20,    Saratoga */
        DispRegAmtMnemo(pData, C_DISP |C_DISP_MNE_TOTAL); /* display amount & mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;

    case CLASS_UISPLIT:             /* split    R3.1 */
        DispRegAmtMnemo(pData, 0); /* display amount & mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegTender20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGTENDER"
*===========================================================================
*/
VOID DispRegTender20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UITENDER1:           /* tender #1 */
    case CLASS_UITENDER3:           /* tender #3 */
    case CLASS_UITENDER4:           /* tender #4 */
    case CLASS_UITENDER5:           /* tender #5 */
    case CLASS_UITENDER6:           /* tender #6 */
    case CLASS_UITENDER7:           /* tender #7 */
    case CLASS_UITENDER8:           /* tender #8 */
    case CLASS_UITENDER9:           /* tender #9 */
    case CLASS_UITENDER10:          /* tender #10 */
    case CLASS_UITENDER11:          /* tender #11 */
	case CLASS_UITENDER12:			/* tender #12 */
	case CLASS_UITENDER13:			/* tender #13 */
	case CLASS_UITENDER14:			/* tender #14 */
	case CLASS_UITENDER15:			/* tender #15 */
	case CLASS_UITENDER16:			/* tender #16 */
	case CLASS_UITENDER17:			/* tender #17 */
	case CLASS_UITENDER18:			/* tender #18 */
	case CLASS_UITENDER19:			/* tender #19 */
	case CLASS_UITENDER20:			/* tender #20 */
        DispRegAmtMnemo(pData, C_DISP | C_DISP_MNE_TENDER);  /* display amount & mnemonics */
        break;

    /*--- Second Food Stamp Tender, V2.01.04 ---*/
    case CLASS_UITENDER2:           /* tender #2 */
        DispRegAmtMnemo(pData, C_DISP | C_DISP_MNE_TENDER);  /* display amount & mnemonics */
        break;

    case CLASS_UIFFSCHANGE:          /* food stamp change */
        DispRegFoodStamp(pData, C_DISP | C_DISP_MNE_CHANGE);
        break;

    case CLASS_UIFOREIGNTOTAL1:     /* foreign currency total #1 */
    case CLASS_UIFOREIGNTOTAL2:     /* foreign currency total #2 */
    case CLASS_UIFOREIGNTOTAL3:     /* foreign currency total #3 */
    case CLASS_UIFOREIGNTOTAL4:     /* foreign currency total #4 */
    case CLASS_UIFOREIGNTOTAL5:     /* foreign currency total #5 */
    case CLASS_UIFOREIGNTOTAL6:     /* foreign currency total #6 */
    case CLASS_UIFOREIGNTOTAL7:     /* foreign currency total #7 */
    case CLASS_UIFOREIGNTOTAL8:     /* foreign currency total #8 */
    case CLASS_UIFOREIGN1:          /* foreign currency tender #1 */
    case CLASS_UIFOREIGN2:          /* foreign currency tender #2 */
    case CLASS_UIFOREIGN3:          /* foreign currency tender #3 */
    case CLASS_UIFOREIGN4:          /* foreign currency tender #4 */
    case CLASS_UIFOREIGN5:          /* foreign currency tender #5 */
    case CLASS_UIFOREIGN6:          /* foreign currency tender #6 */
    case CLASS_UIFOREIGN7:          /* foreign currency tender #7 */
    case CLASS_UIFOREIGN8:          /* foreign currency tender #2 */
        DispRegForeign(pData, C_DISP);  /* display for foreign currency total */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        return;
#endif
*/
    }
    DispRegOpDscrCntr(pData, 0);    /* control descriptor (ope. display) */
    DispRegCuDscrCntr(pData);       /* control descriptor (cust. display) */
    DispRegOpEchoBack(pData);       /* make display format for echo back */
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegMisc20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGMISC"
*===========================================================================
*/
VOID DispRegMisc20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UINOSALE:            /* nosale */
    case CLASS_UICHECKTRANSFERIN:   /* check transfer in */
    case CLASS_UICHECKTRANSFEROUT:  /* check transfer out */
    case CLASS_UIPOSTRECEIPT:       /* post receipt */
    case CLASS_UIGIFTRECEIPT:       /* gift receipt key */
    case CLASS_UIPARKING:           /* parking key */
    case CLASS_UIFMONEY_IN:         /* money declaration-in,    Saratoga */
    case CLASS_UIFMONEY_OUT:        /* money declaration-out,   Saratoga */
		RflGetLead (pData->aszStringData, LDT_WELCOME_ADR);     /* set welcome message */
        DispRegZeroMnemo(pData, C_DISP);  /* display "0.00" and mnemonics */
        break;

    case CLASS_UIPO:                /* paid out */
    case CLASS_UIRA:                /* received on account */
    case CLASS_UITIPSPO:            /* tips paid out */
    case CLASS_UITIPSDECLARED:      /* tips declared */
    case CLASS_UICANCEL:            /* cancel */
    case CLASS_UICHECKTRANSFER:     /* check transfer */
        DispRegAmtMnemo(pData, C_DISP | C_DISP_MNE_BLANK);  /* display amount and mnemonics */
        break;

    case CLASS_UIFMONEY:                  /* money local tender,    Saratoga */
        DispRegMoney(pData);
		break;

    case CLASS_UIFMONEY_FOREIGN:          /* money forign tender */
        DispRegMoney(pData);
        break;

/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        return;
#endif
*/
    }
    DispRegOpDscrCntr(pData, 0);    /* control descriptor (ope. display) */
    DispRegCuDscrCntr(pData);       /* control descriptor (cust. display) */
    DispRegOpEchoBack(pData);       /* make display format for echo back */
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegModifier20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGMODIFIER"
*===========================================================================
*/
VOID DispRegModifier20( REGDISPMSG *pData )
{
    UCHAR   fbCheckData;

    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UIMENUSHIFT:         /* menu shift */
        DispRegRewrite(&DispRegSaveData);   /* redisplay previous data */
        break;

    case CLASS_PRETURN:             /* display transaction return, uchMinorClass == CURQUAL_TRETURN */
    case CLASS_UIPVOID:             /* display preselect void, uchMinorClass == CLASS_PVOID */
    case CLASS_UINORECEIPT:         /* no receipt */
    case CLASS_UITABLENO:           /* table No */
    case CLASS_UINOPERSON:          /* No. of person */
    case CLASS_UILINENO:            /* line No */
    case CLASS_UITARE:              /* scale/tare */
    case CLASS_UIOFFTENDER:         /* off line tender */
    case CLASS_UISEATNO1:           /* seat no */
    case CLASS_UIADJSHIFT:          /* adjective menu shift, saratoga */
    case CLASS_UIFWICTRAN:          /* WIC, Saratoga */
        DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
        break;

    case CLASS_UIALPHANAME:         /* alpha mnemonic entry R3.0 */
        DispRegAmtString(pData, 0);  /* display amount and mnemonics */
        break;

    case CLASS_UITAXMODIFIER1:      /* tax modifier #1 */
    case CLASS_UITAXMODIFIER2:      /* tax modifier #2 */
    case CLASS_UITAXMODIFIER3:      /* tax modifier #3 */
    case CLASS_UITAXMODIFIER4:      /* tax modifier #4 */
    case CLASS_UITAXMODIFIER5:      /* tax modifier #5 */
    case CLASS_UITAXMODIFIER6:      /* tax modifier #6 */
    case CLASS_UITAXMODIFIER7:      /* tax modifier #7 */
    case CLASS_UITAXMODIFIER8:      /* tax modifier #8 */
    case CLASS_UITAXMODIFIER9:      /* tax modifier #9 */
    case CLASS_UITAXEXEMPT1:        /* tax exempt #1 */
    case CLASS_UITAXEXEMPT2:        /* tax exempt #2 */
    case CLASS_UIFDISP_FSMOD:       /* food stamp modifier, Saratoga */
        DispRegMakeAmt(pData);      /* make amount line and display */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }

    if (pData->uchMinorClass == CLASS_UIMENUSHIFT) {    /* lit previous descriptors */
        fbCheckData = (UCHAR)(DispRegSaveData.fbSaveControl & DISP_SAVE_TYPE_MASK);
        if (fbCheckData == DISP_SAVE_TYPE_0) {
            DispRegOpDscrCntr(pData, 0);        /* DISP_SAVE_TYPE_0, use the current descriptor data */
        } else {
            DispRegOpDscrCntr(pData, DSC_SAV);  /* use the descriptor in saved area */
        }
        if ( (DispRegSaveData.uchMajorClass == CLASS_UIFREGOPECLOSE) ||
             (DispRegSaveData.uchMajorClass == CLASS_UIFREGMODE) ) {
                DispRegCuDscrCntr(pData);
                return;
        }
    } else {
        DispRegOpDscrCntr(pData, 0);/* control descriptor (Ope. Display) */
    }
    DispRegOpEchoBack(pData);   /* make display format for echo back */
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegOther20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGOTHER"
*===========================================================================
*/
VOID DispRegOther20( REGDISPMSG *pData )
{
    UCHAR   fbCheckData;

    switch(pData->uchMinorClass) {  /* check Minor Class */
    case CLASS_UICLEAR:                    /* clear key */
        DispRegRewrite(&DispRegSaveData);  /* display in clear key entry */

        fbCheckData = (UCHAR)(DispRegSaveData.fbSaveControl & DISP_SAVE_TYPE_MASK);
        if (fbCheckData == DISP_SAVE_TYPE_0) {
            DispRegOpDscrCntr(pData, 0);         /* DISP_SAVE_TYPE_0, use the current descriptor data */
        } else {
            DispRegOpDscrCntr(pData, DSC_SAV);   /* use the descriptor in saved area */
        }
        if ( (DispRegSaveData.uchMajorClass == CLASS_UIFREGOPECLOSE) ||
             (DispRegSaveData.uchMajorClass == CLASS_UIFREGMODE) ) {
                DispRegCuDscrCntr(pData);
                return;
        }
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;

    case CLASS_UILOWMNEMO:           /* lower mnemonics display */
        DispRegLowMnemo(pData);      /* display mnemonics on second row of lead thru area */
        DispRegOpDscrCntr(pData, 0); /* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);    /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);    /* make display format for echo back */
        break;

    case CLASS_UIPRINTDEMAND:       /* print demand */
    case CLASS_UISCROLLUP:          /* scroll upper end message */
    case CLASS_UISCROLLDOWN:        /* scroll lower end message */
        DispRegZeroMnemo(pData, 0); /* display "0.00" and mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;

    case CLASS_UIEC:                /* error correct */
    case CLASS_UICOMPUTATION:       /* computation */
    case CLASS_UIPAIDORDERRECALL:   /* paid order recall R3.0 */
	case CLASS_UIFPRICE:			/* SR281 ESMITH */
        DispRegAmtMnemo(pData, C_DISP | C_DISP_MNE_BLANK);  /* display amount and mnemonics */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
	//SR 143 @/For & Halo cases added for proper display (cwunn)
	case CLASS_UIFFOR:
	case CLASS_HALO_OVR:
		DispRegQuantity(pData);
		break;

#ifdef  DISP_REG_NOFUNC
    /* deleted function */
    case CLASS_UIORDERDISPLAY:      /* order No display */
        DispRegOrderNo(pData);      /* display order No */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
#endif
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegClose20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGOPECLOSE"
*===========================================================================
*/
VOID DispRegClose20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UICASHIEROUT:        /* cashier sign-out */
    case CLASS_UIB_OUT:             /* cashier B sign-out */
    case CLASS_UICASRELONFIN:       /* release on finalize (cashier) */
    case CLASS_UIWAITEROUT:         /* waiter sign-out */
    case CLASS_UIWAITERLOCKOUT:     /* waiter lock sign-out */
    case CLASS_UIWAIKEYRELONFIN:    /* release on finalize (waiter sign-in by waiter key) */
    case CLASS_UIMODELOCK:          /* mode lock key */
    case CLASS_UICASINTOUT2:        /* cashier interrupt sign-out, R3.3 */
        DispRegModeIn(pData, C_DISP);  /* display "CLOSED, Sign-In Please" */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegModeIn20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGMODE"
*===========================================================================
*/
VOID DispRegModeIn20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_UIREGMODEIN:         /* reg mode-in */
        DispRegModeIn(pData, C_DISP);  /* display "CLOSED, Sign-In Please" */
        DispRegOpDscrCntr(pData, 0);/* control descriptor (ope. display) */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        DispRegOpEchoBack(pData);   /* make display format for echo back */
        break;
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        break;
#endif
*/
    }
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegDisp20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_UIFREGDISP"
*===========================================================================
*/
VOID DispRegDisp20( REGDISPMSG *pData )
{
    pData->fbSaveControl &= DISP_SAVE_FOREIGN_MASK;
    pData->fbSaveControl |= DISP_SAVE_TYPE_4;       /* set type = 4 to display mnemonic and menu number in DispRegOpEchoBack() */

//    DispRegGetMnemo(pData);         /* get the mnemonics to display */

    switch(pData->uchMinorClass) {  /* check minor class */
	case CLASS_UIFDISP_DESCONLY:    /* display descriptors only. no other change to display */
    	memset(pData->aszMnemonic, 0x00, sizeof(pData->aszMnemonic));    // ensure there is no mnemonic to display.
		break;
    case CLASS_UIFDISP_VOID:        /* void key class */
		flDispRegDescrControl |= VOID_CNTRL;   /* set descriptor flag for void */
		flDispRegKeepControl |= VOID_CNTRL;    /* set descriptor keep flag for void */
		RflGetSpecMnem(pData->aszMnemonic, SPC_VOID_ADR);
		DispRegMakeAmt(pData);      /* make amount line and display */
        break;

    case CLASS_UIFDISP_MODDISC:     /* item disc #2 key class */
		flDispRegDescrControl |= ITEMDISC_CNTRL;   /* set descriptor flag for item disc #2 */
		flDispRegKeepControl |= ITEMDISC_CNTRL;    /* set descriptor keep flag for item disc #2 */
		RflGetTranMnem(pData->aszMnemonic, TRN_MODID_ADR);
		DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
        break;

    case CLASS_UIFDISP_QTY:         /* QTY key class */
		RflGetTranMnem(pData->aszMnemonic, TRN_QTY_ADR);
		DispRegQuantity(pData);     /* display quantity and mnemonics */
		break;
	case CLASS_UIFDISP_FOR:			/* SR281 ESMITH */
		RflGetTranMnem(pData->aszMnemonic, TRN_FOR_ADR);
		DispRegQuantity(pData);     /* display quantity and mnemonics */
        break;

    case CLASS_UIFDISP_ADJ:         /* adjective key class */
		RflGetAdj(pData->aszAdjMnem, pData->uchFsc);
        DispRegZeroMnemo(pData, 0); /* display "0.00" and mnemonics */
        break;

    case CLASS_UIFDISP_NUMBER:      /* number key class */
		RflGetTranMnem(pData->aszMnemonic, TRN_NUM_ADR);
		DispRegNumber(pData);       /* display number and mnemonics */
        break;

    case CLASS_UIFDISP_LOCK:        /* lock position display class */
        DispRegLock(pData, C_DISP); /* Display "--------------------" */
        DispRegCuDscrCntr(pData);   /* control descriptor (cust. display) */
        break;

    case CLASS_UIFDISP_PRICECHECK:  /* price check key, 2172 */
		DispRegZeroMnemo(pData, 0); /* display "0.00" and mnemonics */
		break;
	case CLASS_UIFDISP_TAREENTER:
		RflGetLead(pData->aszMnemonic, LDT_TARE_ENTER);
		DispRegZeroMnemo(pData, 0); /* display "0.00" and mnemonics */
        break;

    case CLASS_UIFDISP_DEPTENTER:   /* dept no. entry, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_DEPTNO_ADR);
		DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
        break;

    case CLASS_UIFDISP_PRICEENTER:  /* price entry, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_PP_ADR);
		DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
        break;

    case CLASS_UIFDISP_E_VERSION:   /* e-version key, 2172 */
        DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
        break;

    case CLASS_UIFDISP_SKUNUMBER:      /* number key class, 2172 */
		RflGetTranMnem(pData->aszMnemonic, TRN_SKUNO_ADR);
		DispRegNumber(pData);       /* display number and mnemonics */
        break;

    case CLASS_UIFDISP_CONSNUMBER:      /* number key class, 2172 */
		RflGetTranMnem(pData->aszMnemonic, TRN_CONSNO_ADR);
		DispRegNumber(pData);       /* display number and mnemonics */
        break;

    case CLASS_UIFDISP_POWERDOWN:   /* power down message, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_POWERDOWN_ADR);
		DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
		break;
	case CLASS_UIFDISP_POWERUP:     /* power up message, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_PROGRES_ADR);
		DispRegAmtMnemo(pData, 0);  /* display amount and mnemonics */
        break;

/*
#ifdef  DISP_REG_DEBUG
   default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "Minor Class not found" /
        return;
#endif
*/
    }
    DispRegOpDscrCntr(pData, 0);    /* control descriptor (ope. display) */
    DispRegOpEchoBack(pData);       /* make display format for echo back */
}

#if POSSIBBLE_DEAD_CODE
// moved this functionality into the function above where
// the same type of switch() and case was being used in order
// to combine the the two and make it evident what was being
// done.
//   Richard Chambers, Aug-19-2020

/*
*===========================================================================
** Synopsis:    VOID  DispRegGetMnemo( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function gets the mnemonics to display by using
*               "CliParaRead()".
*===========================================================================
*/
VOID  DispRegGetMnemo( REGDISPMSG *pData )
{
	switch (pData->uchMinorClass) {  /* check minor class */
	case CLASS_UIFDISP_FOR:         /*@FOR key class */ //SR281 ESMITH
		RflGetTranMnem(pData->aszMnemonic, TRN_FOR_ADR);
		break;

	case CLASS_UIFDISP_VOID:        /* void key class */
		flDispRegDescrControl |= VOID_CNTRL;   /* set descriptor flag for void */
		flDispRegKeepControl |= VOID_CNTRL;    /* set descriptor keep flag for void */
		RflGetSpecMnem(pData->aszMnemonic, SPC_VOID_ADR);
		break;

	case CLASS_UIFDISP_ADJ:         /* adjective key class */
		RflGetAdj(pData->aszAdjMnem, pData->uchFsc);
		break;

	case CLASS_UIFDISP_QTY:         /* QTY key class */
		RflGetTranMnem(pData->aszMnemonic, TRN_QTY_ADR);
		break;

	case CLASS_UIFDISP_NUMBER:      /* number key class */
		RflGetTranMnem(pData->aszMnemonic, TRN_NUM_ADR);
		break;

	case CLASS_UIFDISP_MODDISC:     /* item disc #2 key class */
		flDispRegDescrControl |= ITEMDISC_CNTRL;   /* set descriptor flag for item disc #2 */
		flDispRegKeepControl |= ITEMDISC_CNTRL;    /* set descriptor keep flag for item disc #2 */
		RflGetTranMnem(pData->aszMnemonic, TRN_MODID_ADR);
		break;

	case CLASS_UIFDISP_LOCK:        /* lock position display class */
		break;

	case CLASS_UIFDISP_PRICECHECK:  /* price check key, 2172 */
		break;

	case CLASS_UIFDISP_DEPTENTER:   /* dept no. entry, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_DEPTNO_ADR);
		break;

	case CLASS_UIFDISP_TAREENTER:
		RflGetLead(pData->aszMnemonic, LDT_TARE_ENTER);
		break;

	case CLASS_UIFDISP_PRICEENTER:  /* price entry, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_PP_ADR);
		break;

	case CLASS_UIFDISP_E_VERSION:   /* e-version key, 2172 */
		break;

	case CLASS_UIFDISP_SKUNUMBER:      /* number key class, 2172 */
		RflGetTranMnem(pData->aszMnemonic, TRN_SKUNO_ADR);
		break;

	case CLASS_UIFDISP_CONSNUMBER:      /* number key class, 2172 */
		RflGetTranMnem(pData->aszMnemonic, TRN_CONSNO_ADR);
		break;

	case CLASS_UIFDISP_POWERDOWN:   /* power down message, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_POWERDOWN_ADR);
		break;

	case CLASS_UIFDISP_POWERUP:   /* power down message, 2172 */
		RflGetLead(pData->aszMnemonic, LDT_PROGRES_ADR);
		break;
	}

	return;
}
#endif

/*
*===========================================================================
** Synopsis:    VOID  DispRegMakeAmt( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the amount line and devides display type.
*
*===========================================================================
*/
VOID  DispRegMakeAmt( REGDISPMSG *pData )
{
    USHORT  usDataLen = tcharlen(pData->aszStringData);

    if (pData->aszStringData[0] == _T('0')) {
        DispRegNumber(pData);
        return;
    }

    /* if input Data begins with '.', use "0.XX" display type */
    if (pData->aszStringData[0] == _T('.')) {
		TCHAR   aszSaveData[STD_NUMBER_LEN + 1] = { 0 };

        _tcsncpy(aszSaveData, pData->aszStringData, STD_NUMBER_LEN);
        pData->aszStringData[0] = _T('0');
        _tcsncpy(&pData->aszStringData[1], aszSaveData, STD_NUMBER_LEN);
        DispRegNumber(pData);
        return;
    }

    /* Data length is more than 10, no need of decimal point */
    if ( usDataLen >= 10 ) {
        DispRegNumber(pData);       /* display number and mnemonics */
    } else {
		USHORT  i;

        for (i = 0; i < usDataLen; i ++ ) {
            if ( pData->aszStringData[i] != _T('.') ) {
                continue;
            }

            DispRegNumber(pData);   /* display number and mnemonics */
            return;
        }

        pData->lAmount = _ttol(pData->aszStringData);
        DispRegAmtMnemo(pData, 0);
    }
}

/*
*===========================================================================
** Synopsis:    VOID  DispRegCompulsory20( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function devides display type by checking minor class.
*               major class ... "CLASS_REGDISPCOMPLSRY"
*===========================================================================
*/
VOID DispRegCompulsory20( REGDISPMSG *pData )
{
    switch(pData->uchMinorClass) {  /* check minor class */
    case CLASS_REGDISP_SUPERINT:    /* supr. intervention class */
    case CLASS_REGDISP_DEPTNO:      /* dept no. entry, 2172 */
    case CLASS_REGDISP_WICMOD:      /* wic transaction, Saratoga */
        DispRegAmtMnemo(pData, 0);  /* Display amount and mnemonics */
        break;

    case CLASS_REGDISP_PCHECK:      /* price entry, 2172 */
    case CLASS_REGDISP_PCHECK_PM:      /* price entry, 2172 */
        DispRegSales(pData, 0);
        break;

    case CLASS_REGDISP_CPAN:        /* account no. entry request class */
        DispRegAmtMnemo(pData, 0);  /* Display amount and mnemonics */
        break;

    case CLASS_REGDISP_REMOVEKEY:   /* waiter lock key remove request */
    case CLASS_REGDISP_GCNO:        /* GC No. entry request class */
    case CLASS_REGDISP_TABLENO:     /* table No. entry request class */
    case CLASS_REGDISP_PERSON:      /* No. of Person entry request class */
    case CLASS_REGDISP_LINENO:      /* line No. entry request class */
    case CLASS_REGDISP_CASHIERNO:   /* cashier No. entry request class */
    case CLASS_REGDISP_WAITERNO:    /* Waiter No. entry request class */
    case CLASS_REGDISP_ETK:         /* ETK job code entry request class */
    case CLASS_REGDISP_CPRN:        /* room no. entry request class */
    case CLASS_REGDISP_CPGI:        /* guest id entry request class */
    case CLASS_REGDISP_CPLN:        /* guest line no. entry request class */
    case CLASS_REGDISP_OEPNO:       /* order entry prompt no request class R3.0 */
    case CLASS_REGDISP_PAUSE:       /* macro key pause request class R3.0 */
    case CLASS_REGDISP_CUSTOMER:    /* customer name entry request class R3.0 */
    case CLASS_REGDISP_SEATNO:      /* seat no. entry request class R3.1 */
    case CLASS_REGDISP_INSERTKEY:   /* remove server lock, V3.3 */
    case CLASS_REGDISP_SECRET:      /* cashier No. entry request class, V3.3 */
    case CLASS_REGDISP_BUILDING:    /* start plu building, 2172 */
    case CLASS_REGDISP_BIRTHDAY:    /* birthday entry, 2172 */
    case CLASS_REGDISP_CLEAR:       /* clear key request (EPT), Saratoga*/
        DispRegZeroMnemo(pData, 0); /* Display "0.00" and mnemonics */
        break;

    /* ---- R3.1 OEP Scroll of 2x20 Display ---- */
    case CLASS_REGDISP_OEP_LEAD:    /* display oep with leadthrough */
    case CLASS_REGDISP_OEP_PLU:     /* display oep with child plu */
        DispRegOrderEntry(pData);   /* Display parent & child plu */
        break;

    case CLASS_REGDISP_EVENTTRIGGER:     /* display oep with child plu */
        DispRegEventTrigger(pData);   /* Display parent & child plu */
        break;

/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_REG_MINOR_NOT_FOUND_1 );
                                    / "minor class not found" /
        return;
#endif
*/
    }
    DispRegOpDscrCntr(pData, 0);    /* Control Descriptor (Ope. Display) */
    DispRegOpEchoBack(pData);       /* Control for echo back */
}

/*
*===========================================================================
** Synopsis:    VOID DispRegRewrite( REGDISPMSG *pData )
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*    Output:    nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description: This Function makes the display format in clear key entry.
*===========================================================================
*/
VOID DispRegRewrite(REGDISPMSG *pData)
{
    switch(pData->uchMajorClass) {
    case 0:
    case CLASS_UIFREGOPEN:          /* operator open */
        DispRegZeroMnemo(pData, 0); /* display "0.00" */
        break;

    case CLASS_UIFREGMODE:          /* mode-in */
    case CLASS_UIFREGOPECLOSE:      /* operator close */
        DispRegModeIn(pData, C_DISP);  /* display "CLOSED, Sign-In Please" */
        break;

    case CLASS_UIFREGTOTAL:         /* total key */
    case CLASS_REGDISPCOMPLSRY:     /* compulsory */
        DispRegAmtMnemo(pData, 0);  /* display amount & mnemonics */
        break;

    case CLASS_UIFREGTENDER:        /* tender */
        switch(pData->uchMinorClass) {  /* check minor class */
        case CLASS_UITENDER1:       /* tender #1 */
        case CLASS_UITENDER2:       /* tender #2 */
        case CLASS_UITENDER3:       /* tender #3 */
        case CLASS_UITENDER4:       /* tender #4 */
        case CLASS_UITENDER5:       /* tender #5 */
        case CLASS_UITENDER6:       /* tender #6 */
        case CLASS_UITENDER7:       /* tender #7 */
        case CLASS_UITENDER8:       /* tender #8 */
        case CLASS_UITENDER9:       /* tender #9 */
        case CLASS_UITENDER10:      /* tender #10 */
        case CLASS_UITENDER11:      /* tender #11 */
		case CLASS_UITENDER12:			/* tender #12 */
		case CLASS_UITENDER13:			/* tender #13 */
		case CLASS_UITENDER14:			/* tender #14 */
		case CLASS_UITENDER15:			/* tender #15 */
		case CLASS_UITENDER16:			/* tender #16 */
		case CLASS_UITENDER17:			/* tender #17 */
		case CLASS_UITENDER18:			/* tender #18 */
		case CLASS_UITENDER19:			/* tender #19 */
		case CLASS_UITENDER20:			/* tender #20 */
            DispRegAmtMnemo(pData, 0 ); /* display amount & mnemonics */
            break;

        case CLASS_UIFOREIGNTOTAL1: /* foreign currency total #1 */
        case CLASS_UIFOREIGNTOTAL2: /* foreign currency total #2 */
        case CLASS_UIFOREIGNTOTAL3: /* foreign currency total #3 */
        case CLASS_UIFOREIGNTOTAL4: /* foreign currency total #4 */
        case CLASS_UIFOREIGNTOTAL5: /* foreign currency total #5 */
        case CLASS_UIFOREIGNTOTAL6: /* foreign currency total #6 */
        case CLASS_UIFOREIGNTOTAL7: /* foreign currency total #2 */
        case CLASS_UIFOREIGNTOTAL8: /* foreign currency total #2 */
        case CLASS_UIFOREIGN1:      /* foreign currency tender #1 */
        case CLASS_UIFOREIGN2:      /* foreign currency tender #2 */
        case CLASS_UIFOREIGN3:      /* foreign currency tender #3 */
        case CLASS_UIFOREIGN4:      /* foreign currency tender #4 */
        case CLASS_UIFOREIGN5:      /* foreign currency tender #5 */
        case CLASS_UIFOREIGN6:      /* foreign currency tender #6 */
        case CLASS_UIFOREIGN7:      /* foreign currency tender #7 */
        case CLASS_UIFOREIGN8:      /* foreign currency tender #8 */
            DispRegForeign(pData, 0);  /* display for foreign currency total */
        }
        break;

    case CLASS_UIFREGOTHER:
        switch(pData->uchMinorClass) {
		case CLASS_UICLEAR:
			DispRegZeroMnemo(pData, 0); /* display "0.00" */
			break;

#ifdef DISP_REG_NOFUNC
        case CLASS_UIORDERDISPLAY:
            DispRegOrderNo(pData);  /* display order No. */
            break;
#endif
        case CLASS_UISCROLLUP:
        case CLASS_UISCROLLDOWN:
            DispRegZeroMnemo(pData, 0); /* display "0.00" and mnemonics */
            break;
        }
        break;

    /* redisplay for 2x20 scroll R3.1 */
    case CLASS_UIFREGSALES:                 /* Sales */
        if ((UCHAR)(pData->fbSaveControl & DISP_SAVE_TYPE_MASK) == DISP_SAVE_TYPE_1) {
            DispRegSales(pData, 0 ); /* display amount & mnemonics */
        }
        break;

    case CLASS_REGDISP_SALES:               /* Scalable item */
        if ((UCHAR)(pData->fbSaveControl & DISP_SAVE_TYPE_MASK) == DISP_SAVE_TYPE_1) {
            switch(pData->uchMinorClass) {  /* check minor class */
            case CLASS_REGDISP_SCALE2DIGIT: /* scalable item display (2 digits type) */
                DispRegScale(pData, _2_DIGIT, 0);
                                            /* display amount and mnemonics */
                break;

            case CLASS_REGDISP_SCALE3DIGIT: /* scalable item display (3 digits type) */
                DispRegScale(pData, _3_DIGIT, 0);
                                            /* display amount and mnemonics */
                break;
            }
        }
        break;

    case CLASS_UIFREGTRANSOPEN:             /* transaction open */
    case CLASS_UIFREGDISC:                  /* Discount */
    case CLASS_UIFREGCOUPON:                /* Coupon */
        if ((UCHAR)(pData->fbSaveControl & DISP_SAVE_TYPE_MASK) == DISP_SAVE_TYPE_1) {
            DispRegAmtMnemo(pData, 0 ); /* display amount & mnemonics */
            break;
        }
/*
#ifdef  DISP_REG_DEBUG
    default:
        PifLog( MODULE_DISPREG_ID, LOG_ERROR_MAJOR_NOT_FOUND );
                                    / "major class not found" /
        break;
#endif
*/
    }
}


/****** End of Source ******/