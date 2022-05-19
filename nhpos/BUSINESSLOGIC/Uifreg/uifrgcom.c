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
* Title       : User Interface for Reg Common Module
* Category    : User Interface for Reg, NCR 2170 US Hospitality Application
* Program Name: UIFRGCOM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:
*
*               UifRegInputCheck() : Reg Input Data Check
*               UifRegInputCheckWOAmount() : Reg Input Data Check and W/O Amount
*               UifRegVoidNoCheck() : Reg Item Void,Number Check
*               UifRegWorkClear() : Reg UI Work Clear
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* May-07-92:00.00.01:M.Suzuki   : initial
*          :        :           :
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
#include	<windows.h>
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <regstrct.h>
#include <paraequ.h>
#include <uireg.h>
#include "rfl.h"
#include "uiregloc.h"
#include <para.h>
#include "csstbpar.h"
#include "csttl.h"
#include "mld.h"
#include "display.h"
#include "item.h"
#include "transact.h"
#include "..\Sa\UIE\uieio.h"
#include "BlFWif.h"
#include "..\TOTAL\TOTALUM\ttltum.h"
#include "csstbttl.h"
#include "fsc.h"

extern TRANINFORMATION     TrnInformation;
extern TTLCSREGFIN  TtlIndFinWork;
extern TTLCSCASTOTAL  TtlCasDayWork;

/*
*===========================================================================
** Synopsis:    _int64 UieGetCurrentTimeMilli (void)
*     Input:    nothing
*     Output:   number of milliseconds since Epoch
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Obtain the current date/time and return it as a huge integer
*               of the number of milliseconds.
*
*               The purpose of this function is to be able to determine differences
*               in time for instance when polling a device to check for a time out.
*
*               This function returns an _int64 value that is the number of milliseconds
*               since Epoch, the system time start date which is something like 1601.
*===========================================================================
**/
_int64 UieGetCurrentTimeMilli (void)
{
	SYSTEMTIME   mySystemTime;
	union {
		_int64       u64TimeInSeconds;
		FILETIME     fileTime;
	} myFileTime;
	_int64       u64ConvertFactor = 10000;  // SystemTimeToFileTime() returns to nearest 100 nanosecond

	GetSystemTime (&mySystemTime);
	myFileTime.u64TimeInSeconds = 0;
	SystemTimeToFileTime (&mySystemTime, &myFileTime.fileTime);

	myFileTime.u64TimeInSeconds /= u64ConvertFactor;

	return myFileTime.u64TimeInSeconds;
}

/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    SHORT UifRegInputCheck(KEYMSG *pData, UCHAR uchLen)
*     Input:    pData - key message pointer
*               uchLen - Max Input Length
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Input Check
*===========================================================================
*/
SHORT UifRegInputCheck(KEYMSG *pData, UCHAR uchLen)
{
	SHORT  sRet = SUCCESS;

    if (pData->SEL.INPUT.uchDec != 0xFF) {         /* input decimal point ? */
        sRet = (LDT_KEYOVER_ADR);
    }
    else if (pData->SEL.INPUT.uchLen > uchLen) {        /* input digit over ? */
        sRet = (LDT_KEYOVER_ADR);
    }
    else if ((pData->SEL.INPUT.uchLen != 0) && 
		(pData->SEL.INPUT.lData == 0L) &&
		(pData->SEL.INPUT.aszKey[0] == 0)) {
                                                /* input '0' ? */
        sRet = (LDT_KEYOVER_ADR);
    }

#if defined(_DEBUG) || defined(DEBUG)
	NHPOS_ASSERT(sRet == SUCCESS);
#endif

    return sRet;
}

/*
*===========================================================================
** Synopsis:    SHORT UifRegInputCheckWOAmount(KEYMSG *pData, UCHAR uchLen)
*     Input:    pData - key message pointer
*               uchLen - Max Input Length
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Input Check w/o Amount
*===========================================================================
*/
SHORT UifRegInputCheckWOAmount(KEYMSG *pData, UCHAR uchLen)
{
    if (pData->SEL.INPUT.uchLen == 0) {        /* w/o amount ? */
        return(LDT_DATA_ADR);
    }
    return (UifRegInputCheck(pData, uchLen));
}

/*
*===========================================================================
** Synopsis:    SHORT UifRegVoidNoCheck(VOID)
*     Input:    nothing
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Item Void,Number Check
*===========================================================================
*/
SHORT UifRegVoidNoCheck(VOID)
{
    if ((uchUifRegVoid != 0) || (aszUifRegNumber[0] != 0)) {
                                /* input item void or input number ? */
        return(LDT_SEQERR_ADR);
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID UifRegWorkClear(VOID)
*     Input:    nothing
*     Output:   nothing
*     INOut:    nothing
*
** Return:      nothing
*
** Description: Reg UI work area clear
*===========================================================================
*/
VOID UifRegWorkClear(VOID)
{
    memset(&UifRegData, 0, sizeof(UifRegData));     /* Clear UI Work */
    memset(&UifRegDiaWork, 0, sizeof(UifRegDiaWork));
    memset(aszUifRegNumber, 0, sizeof(aszUifRegNumber));
    uchUifRegVoid = 0;
    flUifRegStatus &= ~UIFREG_MODIFIER;             /* reset modifier status */
    fchUifSalesMod = 0;	/* 2172 */
}

/*
*===========================================================================
** Synopsis:    SHORT UifRegInputCheck(KEYMSG *pData, UCHAR uchLen)
*     Input:    pData - key message pointer
*               uchLen - Max Input Length
*     Output:   nothing
*     INOut:    nothing
*
** Return:      error status
*                 success = 0
*                 error   not= 0
*
** Description: Reg Input Check ("0" is OK)         Saratoga: Nov/27/2000
*===========================================================================
*/
SHORT   UifRegInputZeroCheck(KEYMSG *pData, UCHAR uchLen)
{
    if (pData->SEL.INPUT.uchDec != 0xFF) {         /* input decimal point ? */
        return(LDT_KEYOVER_ADR);
    }
    if (pData->SEL.INPUT.uchLen > uchLen) {        /* input digit over ? */
        return(LDT_KEYOVER_ADR);
    }
    if (pData->SEL.INPUT.uchLen == 0) {
        return(LDT_KEYOVER_ADR);
    }
    return (SUCCESS);
}
/*
*===========================================================================
** Synopsis:    VOID UifRegCheckDrawerLimit(VOID)
*     Input:    nothing
*     Output:   nothing
*     INOut:    nothing
*
** Return:      nothing
*
** Description: Checks Drawer Limit, if over, it alerts the cashier. SR 155
*===========================================================================
*/
VOID UifRegCheckDrawerLimit(USHORT status)
{
	PARAMISCPARA	ParaMiscPara = { 0 };
	DCURRENCY		lAmountOnHand = 0;
	STANZA          Stanza[10];
	SHORT			i;
	static ULONG    ulCasID;           /* Handle Area */
	TTLCASTENDER    CastTtl = { 0 };

	//Read the Parameter from AC128 Cash Drawer Limit
	ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
	ParaMiscPara.uchAddress = MISC_CASHLIMIT_ADR;
	ParaMiscParaRead(&ParaMiscPara);

	if(!status)//Cashier is signing in
	{
		ulCasID = UifRegData.regopeopen.ulID;  //find out my ID, set to static variable, until another cashier signs in
	}

	CastTtl.uchMajorClass = CLASS_TTLCASHIER;	//set up Cashier Total Data for read
	CastTtl.uchMinorClass = CLASS_TTLCURDAY;
	CliCasTtlTenderAmountRead( ulCasID, &CastTtl);	//Read cashier totals from Master Terminal
	lAmountOnHand = (CastTtl.lCashTender);	//Set tender to amount on hand variable for checking

	if(ParaMiscPara.ulMiscPara !=0 ) //if the Parameter is not zero
		{
			if( lAmountOnHand > (LONG)ParaMiscPara.ulMiscPara)
			//If it is larger than allowed, then continue
			{
				REGDISPMSG      ReDisplay = { 0 };
				REGDISPMSG      tempDisplay = { 0 };

				if(status)  //checks status
				{
					/*if the status is 1, the function is being called from ItemTender
					so it needs to use DispRegPrevData for Redisplay*/
					tempDisplay = DispRegPrevData;
				}
				else {
					//use DispRegSaveData for Redisplay
					tempDisplay = DispRegSaveData;
				}

				//Build the Sound for when pickup is required
				for(i = 0; i < 9; i++){
					Stanza[i].usDuration = 50;
					if ((! i) || (! (i % 2))){
						Stanza[i].usFrequency = STANZA_FREQUENCY_SILENCE;
					} else {
						Stanza[i].usFrequency =  500;
					}
				}
				Stanza[i].usDuration  = STANZA_DURATION_END_STANZA;
				Stanza[i].usFrequency = STANZA_DURATION_END_STANZA;

				//Sets the display to alert cashier that they are over the limit
				ReDisplay.uchMajorClass = CLASS_UIFREGOTHER;
				ReDisplay.uchMinorClass = CLASS_UILOWMNEMO;
				ReDisplay.fbSaveControl = DISP_SAVE_TYPE_0;
				RflGetTranMnem (ReDisplay.aszMnemonic, TRN_PCKUP_REQ);
				//Display the Transaction Mnemonics
				DispRegPrevData = DispRegSaveData;
				DispWrite(&ReDisplay);

				PifPlayStanza(Stanza); /* Send the pulse sound */
				PifSleep(2000); //wait 2 seconds
				DispWrite(&tempDisplay); //send previous message
		}
	}
}
//SR 143 @/For Key cwunn ported the following from 2170GP
// UifRegQtyCheck()
// UifRegWQtyCheck()

/*
*===========================================================================
** Synopsis:    SHORT UifRegQtyCheck(KEYMSG *pData, UCHAR uchLen, UCHAR uchDec)
*     Input:    pData - key message pointer
*               uchLen - Max Input Length
*               uchDec - Max Decimal Length
*
*     Output:   nothing
*     INOut:    nothing
*
** Return:      LDT_KEYOVER_ADR :
*                   if Decimal Value is grater then specified decimal digits,
*                     Integer Value digit is greater than specified digit,
*                     or Indexed value is zero.
*
*               SUCCESS :
*                   if Indexed value is not entered or is valid,
*
** Description: This routine checks whether the entered index value is valid.
*               therefor, if indexed value is not entered, return SUCCESS status.
*               it is used for checking of decimal quantity value.
*
*===========================================================================
*/
SHORT UifRegQtyCheck(KEYMSG *pData, UCHAR uchLen, UCHAR uchDec)
{
    /* if decimal value is entered --- */
    if (pData->SEL.INPUT.uchDec != NON_DECIMAL) {
        /* --- if decimal digit is illegal --- */
        if (pData->SEL.INPUT.uchDec > uchDec) {
            /* --- return illegal data message number --- */
            return(LDT_KEYOVER_ADR);
        }
        /* --- if integer digit is illegal --- */
        if ((UCHAR)(pData->SEL.INPUT.uchLen -
                pData->SEL.INPUT.uchDec - 1) > uchLen) {
            /* --- return illegal data message number --- */
            return(LDT_KEYOVER_ADR);
        }
    }
    /* --- if indexed value is w/o decimal --- */
    else {
        if (pData->SEL.INPUT.uchLen > uchLen) {
            /* --- return illegal data message number --- */
            return(LDT_KEYOVER_ADR);
        }
    }
    /* --- indexed value is zero entered --- */
    if ((pData->SEL.INPUT.uchLen != 0) && (pData->SEL.INPUT.lData == 0L)) {
        return(LDT_KEYOVER_ADR);
    }
    return (SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT UifRegWQtytCheck(KEYMSG *pData, UCHAR uchLen, UCHAR uchDec)
*     Input:    pData - key message pointer
*               uchLen - Max Input Length
*               uchDec - Max Decimal Length
*
*     Output:   nothing
*     INOut:    nothing
*
** Return:      LDT_KEYOVER_ADR :
*                   if Decimal Value is grater then specified decimal digits,
*                     Integer Value digit is greater than specified digit,
*                     Indexed value is zero,
*                     or Indexed value is none.
*
*               SUCCESS :
*                   if Indexed value is valid,
*
** Description: This routine checks whether the entered index value is valid.
*               it is used for checking of decimal quantity value.
*
*===========================================================================
*/

SHORT UifRegWQtyCheck(KEYMSG *pData, UCHAR uchLen, UCHAR uchDec)

{
    /* --- if indexed data is none, --- */
    if (pData->SEL.INPUT.uchLen == 0) {
        /* return illegal data message number --- */
        return(LDT_SEQERR_ADR);
    }
    return(UifRegQtyCheck(pData, uchLen, uchDec));
}


STANZA  *UifCashDrawerMakeStanza(STANZA *pStanza, int maxNotes)
{
    SHORT   i;

	memset(pStanza, 0, sizeof(STANZA) * maxNotes);

	maxNotes--;  // subtract one as we have to have the end of stanza terminator
    for (i = 0; i < maxNotes; i++) {
		(pStanza + i)->usDuration = 300;        /* set sound time */
		if ( (i % 4) == 0) {
			(pStanza + i)->usDuration = 200;        /* set sound time */
            (pStanza + i)->usFrequency = STANZA_FREQUENCY_SILENCE;    /* set frequency(not sound)*/
		} else if ( (i % 4) == 1) {
            (pStanza + i)->usFrequency = 1500;  /* set frequency (sound) */
		} else if ( (i % 4) == 2) {
            (pStanza + i)->usFrequency = 2500;  /* set frequency (sound) */
        } else {
            (pStanza + i)->usFrequency = 3500;  /* set frequency (sound) */
        }
    }
	(pStanza + i)->usFrequency = STANZA_FREQUENCY_LOOP;
	(pStanza + i)->usDuration = 75;
    return (pStanza);
}

/*
*===========================================================================
** Synopsis:    VOID UifRegClearDisplay(VOID)
*     Input:    nothing
*     Output:   nothing
*     INOut:    nothing
*
** Return:      nothing
*
** Description: Clears Display at tender if Desired. SR 155
*===========================================================================
*/
SHORT UifRegCloseDrawerWarning (int iDelayOverride)
{
	SHORT           sRetStatus;     // indicate if there are drawers that we need to report
	_int64          u64StartTime, u64CurrentTime, u64TimeDelay = 12000;
	ULONG			ulLong[2];
	PARAMISCPARA	ParaMiscPara;
	BOOL            bPlayOpenDrawerStanza = 1;
	STANZA          myStanza[21];

	/* Read the parameter information for CashDrawerLimit option, this effects printing
	out the correct mnemonic on the reset display, if it is on, we will read from one
	display structure, if it is off, there is another one to be read out of*/
	ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
	ParaMiscPara.uchAddress = MISC_CASHLIMIT_ADR;
	ParaMiscParaRead(&ParaMiscPara);

	UifCashDrawerMakeStanza (myStanza, sizeof(myStanza)/sizeof(myStanza[0]));

	{
		UCHAR               MDCTime;

		MDCTime = CliParaMDCCheckField(MDC_DRAWEROPEN_TIME, MDC_PARAM_BYTE);
		if (MDCTime > 0) {
			u64TimeDelay = MDCTime * 1000;
		} else {
			bPlayOpenDrawerStanza = 0;
			u64TimeDelay = 12000;
		}
	}

	if (iDelayOverride > 0) {
		u64TimeDelay = iDelayOverride * 1000;
	} else if (iDelayOverride < 0) {
		u64TimeDelay = 0;
	}

	sRetStatus = 0;   // assume there are no drawers so inform the call of this fact
	/*If there are no drawers, sleep, then continue with the function*/
	if ( (PifSysConfig()->uchDrawer1 == DRAWER_NONE) && (PifSysConfig()->uchDrawer2 == DRAWER_NONE) )
	{
		PifSleep(2000);
	} else {
		u64StartTime = UieGetCurrentTimeMilli ();
		for(;;)				//infinite loop, wait for drawer inforamtion
		{
			/*Check to see if both drawers are closed*/
			BlFwIfDrawerStatus(ulLong);
			if((ulLong[0] == BLFWIF_DRAWER_CLOSED) && (ulLong[1] == BLFWIF_DRAWER_CLOSED))
			{
				break;
			}

			sRetStatus = 1;  // there are drawers that we need to report and they are open
			u64CurrentTime = UieGetCurrentTimeMilli ();

			if (u64CurrentTime - u64StartTime < u64TimeDelay) {
				PifSleep (100);
				continue;
			}

			if (bPlayOpenDrawerStanza)
				UieQueueStanzaToPlay (myStanza); /* make a pulse tone */

			//SR 671, without having this message, the terminal will be sitting
			//in an infinate loop without notifying the user that the terminal is 
			//waiting for a cash drawer to close. JHHJ
			UieErrorMsg( LDT_DRAWCLSE_ADR, UIE_WITHOUT );
			if (bPlayOpenDrawerStanza)
				UieQueueStanzaStopCurrent();
		}
	}
	return sRetStatus;
}

VOID UifRegClearDisplay(USHORT usKeyTypeFsc)
{
	USHORT          usClearTheDisplay = 0;
	SHORT           sDrawersProvisioned = 0;

	if (usKeyTypeFsc == FSC_CANCEL) {
		if (CliParaMDCCheckField(MDC_CLEAR_ON_TENDER, MDC_PARAM_BIT_B))
			usClearTheDisplay = 1;
	}
	else if(CliParaMDCCheckField(MDC_CLEAR_ON_TENDER, MDC_PARAM_BIT_C)) {
		usClearTheDisplay = 1;
	}

	/*If the drawer is set as a Compulsory drawer, go into this function and continue with the clear display,
	if it is a non-compulsory drawer, we will not be able to clear the screen, due to the fact that the
	operator can enter items into the terminal while the drawer is open*/
	if(ParaMDCCheck( MDC_DRAWER_ADR, EVEN_MDC_BIT1 ) == 0 )
	{
		sDrawersProvisioned = UifRegCloseDrawerWarning(0);
	}

	/*This is the MDC bit that checks if the functionality is on*/
	if(usClearTheDisplay)
	{
		PARAMISCPARA	ParaMiscPara = {0};
		REGDISPMSG		clearDisplay = {0};
		ITEMOPEOPEN		scrollDisplay = {0};

		if (sDrawersProvisioned == 0 && usKeyTypeFsc == FSC_GIFT_CARD) {
			PifSleep (15000);
		}

		ItemCommonMldInit();   //Clear The Display of all Data

		/* Read the parameter information for CashDrawerLimit option, this effects printing
		out the correct mnemonic on the reset display, if it is on, we will read from one
		display structure, if it is off, there is another one to be read out of*/
		ParaMiscPara.uchMajorClass = CLASS_PARAMISCPARA;
		ParaMiscPara.uchAddress = MISC_CASHLIMIT_ADR;
		ParaMiscParaRead(&ParaMiscPara);

		if(ParaMiscPara.ulMiscPara != 0)
		{
			clearDisplay = DispRegPrevData;
		}else
		{
			clearDisplay = DispRegSaveData;
		}

		/*Display the saved data for the operator information display*/
		DispWrite(&clearDisplay);

		/*The following sets up the scroll display to set up the window information
		in the scroll windows*/
		scrollDisplay.uchMajorClass = CLASS_ITEMOPEOPEN;
		scrollDisplay.uchMinorClass = CLASS_CASHIERIN;
		memcpy(&scrollDisplay.aszMnemonic, &TrnInformation.TranModeQual.aszCashierMnem, sizeof(scrollDisplay.aszMnemonic));
		scrollDisplay.ulID = TrnInformation.TranModeQual.ulCashierID;
		//Display the information to the scroll bar
		MldScrollWrite(&scrollDisplay, MLD_NEW_ITEMIZE);
	}
}
/* --- End of Source File --- */