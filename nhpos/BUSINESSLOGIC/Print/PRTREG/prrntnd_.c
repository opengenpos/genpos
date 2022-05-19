/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
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
* Title       : Print  Normal tender key                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRNTnd_.C                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*               PrtTender() : prints tender operation
*               PrtTender_RJ() : prints tender operation (receipt & journal)
*               PrtTender_VL() : prints tender operation (validation)
*               PrtTender_SP() : prints tender operation (slip)
*               PrtDflTender() : displays tender operation
*
*               PrtDflTenderForm() : displays tender operation
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-16-92 : 00.00.01 : K.Nakajima :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDflTender()                                   
* Oct-27-93 : 02.00.02 : K.You      : add charge posting feature.                                   
* Oct-27-93 : 02.00.02 : K.You      : del. PrtDflTender()                                   
* Apr-07-94 : 00.00.04 : Yoshiko.Jim: add EPT feature in GPUS
* Apr-08-94 : 00.00.04 : K.You      : add validation slip print feature.(mod. PrtTender())
* Apr-25-94 : 00.00.05 : Yoshiko.Jim: E-067 corr. (mod. PrtTender_SP())
* Apr-26-94 : 00.00.05 : K.You      : bug fixed E-056 (mod. PrtTender_VL())
* Jan-17-95 : 03.00.00 : M.Ozawa    : add PrtDflTender() for display on the fly
* Jul-21-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Dec-14-99 : 00.00.01 : M.Ozawa    : add PrtDflTenderForm()
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <rfl.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <dfl.h>
#include <pmg.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"



/**
;============================================================================
;+              P R O G R A M    D E C L A R A T I O N s
;============================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints tender operation
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================*/
VOID PrtTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{
	//SR 131 cwunn 7/16/2003 Tender Digit Masking 
	int i;
	int iMaskedChars = 0;  //number of characters to mask
	int iShownChars = 0;   //number of characerst MDC indicates should not be masked
	PARAMDC MaskMDC, TenderMDC;  //Used to read MDC masking settings and Tender Key settings
	//The following UCHARS are used in reading MDC settings of Tender Keys
	UCHAR	oddOrEven, localMDCBitA, localMDCMask;   

	RflDecryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));

    //if MDC indicates use of masking
	if(CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2) || CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT3)){
		/*SR 131 cwunn 7/15/2003
			Since odd tender keys use even MDC pages and even tender keys use odd MDC
			pages to indicate their charge posting/EPT settings, the following code
			determines which type of MDC page is used.
			localMDCBitA is always the bottom radio button selection on	an MDC page,
			odd mdc bits use ODD_MDC_BIT3/ODD_MDC_MASK and even use EVEN_MDC_BIT3/EVEN_MDC_MASK
			For the MDC pages in question MDC bit A indicates if the
			current tender key causes EPT (on) or Charge Posting (off).
		*/		oddOrEven = (pItem->uchMinorClass+1) % 2;  //uchMinorClass+1 directs odd tenders to even MDC pages
		localMDCBitA = oddOrEven ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
		localMDCMask = oddOrEven ? MDC_ODD_MASK : MDC_EVEN_MASK;

		TenderMDC.uchMajorClass = CLASS_PARAMDC;
		TenderMDC.usAddress = MDC_CPTEND1_ADR + (pItem->uchMinorClass - 1);
		ParaRead(&TenderMDC);

		//SR 131 cwunn 7/17/2003 Determine masking settings
		MaskMDC.uchMajorClass = CLASS_PARAMDC;

		if ((pItem->uchMinorClass >= 12) && (pItem->uchMinorClass <= 20)) { 
			MaskMDC.usAddress = MDC_EPT_MASK3_ADR;
		} else {
			MaskMDC.usAddress = MDC_EPT_MASK2_ADR;
		}

		ParaRead( &MaskMDC );

		if (((pItem->uchMinorClass >= 12) && (pItem->uchMinorClass <= 20)) &&
			ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2)) {

			//mask entire account number unless otherwise indicated
			iShownChars = 0;
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
				//show 2 rightmost digits
				iShownChars = 2;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
				//show 4 rightmost digits
				iShownChars = 4;
			}
			if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
				//show 6 rightmost digits
				iShownChars = 6;
			}
			//iMaskedChars represents the number of characters to mask with X's.
			//This number should be the total number of characters minus the number that should be shown.
			//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
			iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
			iMaskedChars = iMaskedChars ? iMaskedChars : 0;

			for(i=0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
				pItem->aszNumber[i] = _T('X');
			}
		}
		
		//SR 131 cwunn 7/17/2003 Indicate how many account digits to show
		if(((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA) &&
			CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT2)){ //tender status indicates EPT & MDC masks EPTs
			if((pItem->uchMinorClass >= 2) && (pItem->uchMinorClass <=10)){ //Mask only if on Tenders 2-6
				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT2){
					//show 2 rightmost digits
					iShownChars = 2;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == EVEN_MDC_BIT3){
					//show 4 rightmost digits
					iShownChars = 4;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)) == (EVEN_MDC_BIT3 | EVEN_MDC_BIT2)){
					//show 6 rightmost digits
					iShownChars = 6;
				}
				//iMaskedChars represents the number of characters to mask with X's.
				//This number should be the total number of characters minus the number that should be shown.
				//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
				iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for(i=0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = 'X';
				}
			}
		}
		if(((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA) &&
			CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT3)){ //tender status indicates CP (not EPT)
			if((pItem->uchMinorClass >= 11) && (pItem->uchMinorClass <=20)){ //Mask only if on Tenders 7-11
				//mask entire account number unless otherwise indicated
				iShownChars = 0;
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == EVEN_MDC_BIT0){
					//show 2 rightmost digits
					iShownChars = 2;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == EVEN_MDC_BIT1){
					//show 4 rightmost digits
					iShownChars = 4;
				}
				if((MaskMDC.uchMDCData & (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)) == (EVEN_MDC_BIT1 | EVEN_MDC_BIT0)){
					//show 6 rightmost digits
					iShownChars = 6;
				}
				//iMaskedChars represents the number of characters to mask with X's.
				//This number should be the total number of characters minus the number that should be shown.
				//If the number to be shown is equal or larger than the total length, 0 characters should be masked.
				iMaskedChars = _tcslen(pItem->aszNumber)-iShownChars;
				iMaskedChars = iMaskedChars ? iMaskedChars : 0;

				for(i=0; i < iMaskedChars; i++){  //mask the appropriate number of characters.
					pItem->aszNumber[i] = 'X';
				}
			}
		}
	}


    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    if ( pItem->fsPrintStatus & PRT_VALIDATION )
	{ /* validation print */
        if ( CliParaMDCCheck(MDC_VALIDATION_ADR, EVEN_MDC_BIT2) )
		{
            if(PRT_SUCCESS == PrtSPVLInit())
			{
				PrtSPVLHead(pTran);    
				PrtTender_SP(pTran, pItem, 1);          /* SlipValidation E-067 */
				PrtSPVLTrail(pTran);
			}
        } else {
            PrtTender_VL(pTran, pItem);
        }
		RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
    	return ;
    }

    if ( (fsPrtPrintPort & PRT_RECEIPT) || (fsPrtPrintPort & PRT_JOURNAL)) {  /* receipt, journal print */
			PrtTender_RJ(pItem);
		}

	RflEncryptByteString ((UCHAR *)&(pItem->aszNumber[0]), sizeof(pItem->aszNumber));
    return ;
}


/*
*===========================================================================
** Format  : VOID  PrtTender_RJ(ITEMTENDER   *pItem);      
*
*   Input  : ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints tender operation (receipt & journal)
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================*/
VOID PrtTender_RJ(ITEMTENDER  *pItem)
{
    TCHAR   *puchStart, aszPrint[NUM_CPRSPTEXT + 1];
    USHORT  i;
	//SR 131 cwunn 7/16/2003 Tender Digit Masking
	UCHAR	oddOrEven, localMDCBitA, localMDCMask;
	PARAMDC TenderMDC;

    PrtRJOffTend(pItem->fbModifier);                /* Cp off line */
    PrtRJVoid(pItem->fbModifier);                   /* void line */

    PrtRJCPRoomCharge(pItem->aszRoomNo, pItem->aszGuestID);        /* for charge posting */

	/*SR 131 cwunn 7/15/2003
		Since odd tender keys use even MDC pages and even tender keys use odd MDC
		pages to indicate their charge posting/EPT settings, the following code
		determines which type of MDC page is used.
		localMDCBitA is always the bottom radio button selection on	an MDC page,
		odd mdc bits use ODD_MDC_BIT3/ODD_MDC_MASK and even use EVEN_MDC_BIT3/EVEN_MDC_MASK
		For the MDC pages in question MDC bit A indicates if the
		current tender key causes EPT (on) or Charge Posting (off).
	*/
	oddOrEven = (pItem->uchMinorClass+1) % 2;  //uchMinorClass+1 directs odd tenders to even MDC pages
	localMDCBitA = oddOrEven ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = oddOrEven ? MDC_ODD_MASK : MDC_EVEN_MASK;

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = MDC_CPTEND1_ADR + (pItem->uchMinorClass - 1);
	ParaRead(&TenderMDC);

	//always print offline symbol
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
	}
	else {
		//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
		//if tender status indicates EPT
		if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
			//then check if MDC indicates print expiration date on EPT Tender,
			if(!CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
				//then print the expiration date
				PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
			}
		}
		else {	//if tender status indicates CP
			if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit 3, ept is not CP
				//then check if MDC indicates print expiration date on CP Tender
				if(!CliParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
					PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
				}
			}
			else { //tender status indicates non-EPT or CP tender, expiration date
				PrtRJOffline( pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );
			}
		}
	}

	//SR 131 cwunn 7/15/2003 Add MDC check to determine if tender number data should be printed.
	//if tender status indicates EPT
	if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
		PrtRJNumber(pItem->aszNumber);                      /* number line */
	}

    PrtJFolioPost(pItem->aszFolioNumber, pItem->aszPostTransNo);   /* for charge posting */

    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        PrtRJAmtMnem(RflChkTendAdr(pItem), pItem->lTenderAmount); /* tender line */
    }

    if (pItem->lFoodStamp) {                            /* FS */
        PrtRJZeroAmtMnem(TRN_FSCHNG_ADR, pItem->lFoodStamp);
    }

    if (pItem->lFSChange) {                             /* FS credit */
        PrtRJZeroAmtMnem(TRN_FSCRD_ADR, pItem->lFSChange);
    }

    PrtRJZeroAmtMnem(TRN_REGCHG_ADR, pItem->lChange);   /* change */
    
    if (pItem->fsPrintStatus != 0) {
        puchStart = (TCHAR *)pItem->aszCPMsgText;
        for (i = 0; i < pItem->uchCPLineNo; i++) {
            memset(aszPrint, 0, sizeof(aszPrint));
            _tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
            PrtRJCPRspMsgText(aszPrint);         /* for charge posting */
        }
    } else {
        for (i = 0; i < NUM_CPRSPCO; i++) {
            PrtRJCPRspMsgText(pItem->aszCPMsgText[i]);         /* for charge posting */
        }
    }
}                                               

/*
*===========================================================================
** Format  : VOID  PrtTender_VL(TRANINFORMATION  *pTran, ITEMTENDER   *pItem);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints tender operation (validation)
*===========================================================================*/
VOID  PrtTender_VL(TRANINFORMATION  *pTran, ITEMTENDER *pItem)
{
    PmgBeginValidation(PMG_PRT_RCT_JNL);        /* begin validation */

    /* -- in case of 24 char printer -- */
    if ( usPrtVLColumn == PRT_VL_24 ) {
        PrtVLHead(pTran, pItem->fbModifier);            /* send header lines */
        PrtVLTender1(pItem->aszNumber, pItem->lRound);  /* tender 1st line */
                                                        
        if ( pItem->auchExpiraDate[0] != '\0'           /* EPT tender ?     */
            || pItem->auchApproval[0] != '\0'           /* approval code    */
            || pItem->aszRoomNo[0] != '\0'              /* room number      */
            || pItem->fbModifier & OFFEPTTEND_MODIF     /* E-006 corr. 4/20 */
            || pItem->fbModifier & OFFCPTEND_MODIF ) {  /* E-006 corr. 4/20 */

            PrtVLOffline(RflChkTendAdr(pItem), pItem);  /* EPT print        */
        } else {
            PrtVLTender2(pItem);                        /* tender 2nd line  */
        }
        PrtVLTrail(pTran);                              /* send trailer     */
    }

    /* -- in case of 21 char printer -- */

    if ( usPrtVLColumn == PRT_VL_21 ) {
        PrtVSTender(pItem, pTran);
    }

    PrtEndValidation(PMG_PRT_RCT_JNL);          /* end validation */
}

/*
*===========================================================================
** Format  : VOID  PrtTender_SP(TRANINFORMATION *pTran, ITEMTENDER *pItem,
*                               UCHAR uchType);      
*
*   Input  : TRANINFORMATION  *pTran,    -transaction information
*            ITEMTENDER       *pItem     -Item Data address
*            UCHAR            uchType    - 0: normal slip, 1: slip validation
*   Output : none
*   InOut  : none
** Return  : none 
*            
** Synopsis: This function prints tender operation (slip)
*  Change Description: SR 131 cwunn 7/22/2003
*		SR 131 requests that EPT and Charge Posting operations allow for
*		masking of account numbers, depending on MDC settings.  In
*		compliance with SR 131 and pending Visa and Mastecard merchant
*		agreements, account numbers can be masked by configuring
*		MDC Bit 377, CP & EPT Account Number Security Settings
*		and MDC Bit 378, CP & EPT Account Number Masking Settings.
*		Options include printing or not printing account # and date for
*		EPT or CP operations, and masking or not masking account # for
*		EPT or CP operations, when performed using certain tender keys.
*		(EPT  can mask account #s on keys 2-6, CP can mask on keys 7-11).
*		The number of digits masked in CP and EPT are indpendently configured.
*===========================================================================*/
VOID PrtTender_SP(TRANINFORMATION *pTran, ITEMTENDER *pItem, UCHAR uchType) 
{
    TCHAR  aszSPPrintBuff[6 + NUM_CPRSPCO_EPT][PRT_SPCOLUMN + 1]; /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */
    USHORT  i;   
    TCHAR   *puchStart, aszPrint[NUM_CPRSPTEXT + 1];
	//SR 131 cwunn 7/16/2003 Tender Digit Masking
	UCHAR	oddOrEven, localMDCBitA, localMDCMask; 
	PARAMDC TenderMDC;

	/*SR 131 cwunn 7/15/2003
		Since odd tender keys use even MDC pages and even tender keys use odd MDC
		pages to indicate their charge posting/EPT settings, the following code
		determines which type of MDC page is used.
		localMDCBitA is always the bottom radio button selection on	an MDC page,
		odd mdc bits use ODD_MDC_BIT3/ODD_MDC_MASK and even use EVEN_MDC_BIT3/EVEN_MDC_MASK
		For the MDC pages in question MDC bit A indicates if the
		current tender key causes EPT (on) or Charge Posting (off).
	*/
	oddOrEven = (pItem->uchMinorClass+1) % 2;  //uchMinorClass+1 directs odd tenders to even MDC pages
	localMDCBitA = oddOrEven ? ODD_MDC_BIT3 : EVEN_MDC_BIT3;
	localMDCMask = oddOrEven ? MDC_ODD_MASK : MDC_EVEN_MASK;

	TenderMDC.uchMajorClass = CLASS_PARAMDC;
	TenderMDC.usAddress = MDC_CPTEND1_ADR + (pItem->uchMinorClass - 1);
	ParaRead(&TenderMDC);

    /* -- initialize the buffer -- */
    memset(aszSPPrintBuff[0], '\0', sizeof(aszSPPrintBuff));

    /* -- set off line tender -- */
    usSlipLine += PrtSPCPOffTend(aszSPPrintBuff[0], pItem->fbModifier);

	//always print offline symbol
	if ( pItem->fbModifier & OFFEPTTEND_MODIF){
		usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );  /* EPT offline & exp.date   */
	}
	else {
		//SR 131 cwunn 7/15/2003 Add MDC check to determine if expiration date should be printed
		//if tender status indicates EPT
		if((TenderMDC.uchMDCData & localMDCMask) == localMDCBitA){
			//then check if MDC indicates print expiration date on EPT Tender,
			if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT0)){
				//then print the expiration date
				usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );  /* EPT offline & exp.date   */
			}
		}
		else {	//if tender status indicates CP
			if((TenderMDC.uchMDCData & localMDCMask) != localMDCBitA){ //only bit 3, ept is not CP
				//then check if MDC indicates print expiration date on CP Tender
				if(!ParaMDCCheck(MDC_EPT_MASK1_ADR, ODD_MDC_BIT1)){
					usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );   /* EPT offline & exp.date   */
				}
			}
			else { //tender status indicates non-EPT or CP tender, expiration date
				usSlipLine += PrtSPOffline(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );    /* EPT offline & exp.date   */
			}
				
		}
	}

    /* -- set void mnemonic and number -- */
	//always print void lines
	if (pItem->fbModifier & VOID_MODIFIER){
			usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
	}
	else {
		//SR 131 cwunn 7/15/2003 Add MDC check to determine if tender number data should be printed.
		//if tender status indicates EPT
		if(MldCheckEPTAcctPrint(TenderMDC, localMDCBitA, localMDCMask, pItem->aszNumber)){
			usSlipLine += PrtSPVoidNumber(aszSPPrintBuff[usSlipLine], pItem->fbModifier, pItem->usReasonCode, pItem->aszNumber);
		}
	}

    /* -- set room number and guest id -- */
    if ((pItem->aszRoomNo[0] != '\0') && (pItem->aszGuestID[0] != '\0')) {
        usSlipLine += PrtSPCPRoomCharge(aszSPPrintBuff[usSlipLine], pItem->aszRoomNo, pItem->aszGuestID);
    }
/*    usSlipLine                     
        += PrtSPTranNumber(aszSPPrintBuff[usSlipLine], TRN_NUM_ADR,
                           pItem->aszNumber);                               
*/
    /* -- set tender mnemonic and amount -- */
    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], RflChkTendAdr(pItem),  pItem->lTenderAmount);
    }

    /* -- set change mnemonic and amount -- */
    if (pItem->lChange) { 
        usSlipLine += PrtSPMnemAmt(aszSPPrintBuff[usSlipLine], TRN_REGCHG_ADR, pItem->lChange);
    }

    /* -- set response message text -- */
    if (uchType == 0) {                                 /* E-067 corr. 4/25 */
        if (pItem->uchPrintSelect != 0) {
            puchStart = (TCHAR *)pItem->aszCPMsgText;
            for (i = 0; i < pItem->uchCPLineNo; i++) {
                memset(aszPrint, 0, sizeof(aszPrint));
                _tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
                usSlipLine += PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], aszPrint);
            }
        } else {
            for (i = 0; i < NUM_CPRSPCO; i++) {
                usSlipLine += PrtSPCPRspMsgText(aszSPPrintBuff[usSlipLine], pItem->aszCPMsgText[i]);
            }
        }
    }

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* -- print all data in the buffer -- */ 
    for (i = 0; i < usSlipLine; i++) {
        PmgPrint(PMG_PRT_SLIP, aszSPPrintBuff[i], PRT_SPCOLUMN);       
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;        
}

/*
*===========================================================================
** Format  : VOID  PrtDflTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tender operation
*===========================================================================
*/
VOID PrtDflTender(TRANINFORMATION  *pTran, ITEMTENDER  *pItem)
{

	TCHAR  aszDflBuff[30][PRT_DFL_LINE + 1] = {0};   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflOffTend(aszDflBuff[usLineNo], pItem->fbModifier); /* Cp off line */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflCPRoomCharge(aszDflBuff[usLineNo], pItem->aszRoomNo, pItem->aszGuestID);  /* for charge posting */

    /* EPT offline symbol & exp.date */ 
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate,
                  pItem->auchApproval );

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], RflChkTendAdr(pItem), pItem->lTenderAmount);
    }

    /* display balance due data R3.1 */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CURB_ADR, pItem->lBalanceDue);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCHNG_ADR, pItem->lFoodStamp);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCRD_ADR, pItem->lFSChange);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_REGCHG_ADR, pItem->lChange);

    if (pItem->uchPrintSelect != 0) {
		TCHAR   *puchStart = (TCHAR *)pItem->aszCPMsgText;

        for (i = 0; i < pItem->uchCPLineNo && usLineNo < PRT_DFL_LINE_MAX; i++) {
			TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};

			_tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
            usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], aszPrint);         /* for charge posting */
        }
    } else {
        for (i = 0; i < NUM_CPRSPCO && usLineNo < PRT_DFL_LINE_MAX; i++) {
            usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], pItem->aszCPMsgText[i]);         /* for charge posting */
        }
    }

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- check void status -- */
    PrtDflCheckVoid(pItem->fbModifier);

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_TENDER); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }        
    }

    /* -- send display data -- */
    PrtDflSend();

}

/*
*===========================================================================
** Format  : VOID  PrtDflTender(TRANINFORMATION *pTran, ITEMTENDER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMTENDER       *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tender operation
*===========================================================================
*/
USHORT PrtDflTenderForm(TRANINFORMATION  *pTran, ITEMTENDER  *pItem, TCHAR *puchBuffer)
{
	TCHAR  aszDflBuff[PRT_DFL_LINE_MAX + 5][PRT_DFL_LINE + 1] = {0};   /* display data save area */
    USHORT  usLineNo=0;                       /* number of lines to be displayed */
    USHORT  usOffset = 0, i;                       

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflOffTend(aszDflBuff[usLineNo], pItem->fbModifier); /* Cp off line */
    usLineNo += PrtDflVoid(aszDflBuff[usLineNo], pItem->fbModifier, pItem->usReasonCode);   

    usLineNo += PrtDflCPRoomCharge(aszDflBuff[usLineNo], pItem->aszRoomNo, pItem->aszGuestID);  /* for charge posting */

    /* EPT offline symbol & exp.date */ 
    usLineNo += PrtDflOffline(aszDflBuff[usLineNo], pItem->fbModifier, pItem->auchExpiraDate, pItem->auchApproval );

    usLineNo += PrtDflNumber(aszDflBuff[usLineNo], pItem->aszNumber);  

    usLineNo += PrtDflFolioPost(aszDflBuff[usLineNo], pItem->aszFolioNumber, pItem->aszPostTransNo);    /* for charge posting */

    if (pItem->uchMinorClass !=  CLASS_TEND_FSCHANGE) { /* Saratoga */
        usLineNo += PrtDflAmtMnem(aszDflBuff[usLineNo], RflChkTendAdr(pItem), pItem->lTenderAmount);
    }

    /* display balance due data R3.1 */
    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_CURB_ADR, pItem->lBalanceDue);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCHNG_ADR, pItem->lFoodStamp);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_FSCRD_ADR, pItem->lFSChange);

    usLineNo += PrtDflZeroAmtMnem(aszDflBuff[usLineNo], TRN_REGCHG_ADR, pItem->lChange);

    if (pItem->uchPrintSelect != 0) {
		TCHAR   *puchStart = (TCHAR *)pItem->aszCPMsgText;

        for (i = 0; i < pItem->uchCPLineNo && usLineNo < PRT_DFL_LINE_MAX; i++) {
			TCHAR   aszPrint[NUM_CPRSPTEXT + 1] = {0};

			_tcsncpy(aszPrint, puchStart + i * pItem->uchPrintSelect, pItem->uchPrintSelect);
            usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], aszPrint);         /* for charge posting */
        }
    } else {
        for (i = 0; i < NUM_CPRSPCO && usLineNo < PRT_DFL_LINE_MAX; i++) {
            usLineNo += PrtDflCPRspMsgText(aszDflBuff[usLineNo], pItem->aszCPMsgText[i]);         /* for charge posting */
        }
    }

    for (i = 0; i < usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    
	// limit to size KDS_LINE_DATA_LEN characters.
	_tcsncpy(puchBuffer, aszDflBuff[0], ((usLineNo < PRT_DFL_LINE_MAX) ? usLineNo : PRT_DFL_LINE_MAX) * (PRT_DFL_LINE + 1));
    return usLineNo;

}

/***** End Of Source *****/
