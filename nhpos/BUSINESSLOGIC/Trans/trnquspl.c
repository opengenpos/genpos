/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             AT&T GIS Corporation, E&M OISO   ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 3.1              ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         
:   Category           :                                                 
:   Program Name       : TRNQUSPL.C                                       
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)
*       2012  -> GenPOS Rel 2.2.0
*       2014  -> GenPOS Rel 2.2.1
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:*------------------------------------------------------------------------
:   Abstract         
:
:*------------------------------------------------------------------------
:   Update Histories                                                      
:       Date    :   Version/Revision    :   Name    :   Description      
:     95/11/27      03.01.00                hkato       R3.1
:     99/08/13      03.05.00                K.Iwata     R3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
* Nov-21-15 : 02.02.01 : R.Chambers : using new typedefs for variables and source cleanup.
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include    <stdlib.h>
#include	<stdio.h>
#include	<malloc.h>

#include    <ecr.h>
#include    <regstrct.h>
#include	<rfl.h>
#include    <uie.h>
#include    <fsc.h>
#include    <display.h>
#include    <uireg.h>
#include	<pifmain.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <mld.h>
#include    <bl.h>
#include    <item.h>


/*==========================================================================
*    Synopsis:  VOID    TrnQualTransSpl(ITEMTRANSOPEN *pItem, SHORT sType)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by transaction open
==========================================================================*/

VOID    TrnQualTransSpl(ITEMTRANSOPEN *pItem, TrnStorageType  sStorageType)
{   
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sStorageType);

    switch(pItem->uchMinorClass) {
    case CLASS_NEWCHECK:
        pSpl->TranGCFQual.ulCashierID = pItem->ulCashierID;
/*        pSpl->TranGCFQual.usWaiterID = pItem->usWaiterID; */
        pSpl->TranGCFQual.usGuestNo = pItem->uchSeatNo;
        pSpl->TranGCFQual.uchCdv = pItem->uchCdv;
        pSpl->TranGCFQual.usTableNo = pItem->usTableNo;
        break;

    default:
        break;
    }
}

/*==========================================================================
*    Synopsis:  VOID    TrnQualDiscSpl(ITEMDisc *pItem, SHORT sType)
*
*    Input:     
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   write data by discount
==========================================================================*/

VOID    TrnQualDiscSpl(ITEMDISC *pItem, TrnStorageType  sStorageType)
{   
    TRANINFSPLIT    *pSpl = TrnGetInfSpl(sStorageType);
                                                     
    switch(pItem->uchMinorClass) {
    case CLASS_REGDISC1:
    case CLASS_REGDISC2:
    case CLASS_REGDISC3:
    case CLASS_REGDISC4:
    case CLASS_REGDISC5:
    case CLASS_REGDISC6:
        pSpl->TranGCFQual.fsGCFStatus2 |= GCFQUAL_REGULAR_SPL;
        break;

    default:
        break;
    }
}


SHORT  TrnDisplayFetchGuestCheck (TCHAR *pLeadThruMnemonic, ULONG ulCashierId)    // handle FSC_ASK, ITM_ASK_OPENCHECKS_BY_OPID
{
	UIFDIADATA        UI;
	int               iMaxCount = 48;
	int               lRetStatus = 0;
	MLDPOPORDERUNION_VARY  *pDisp;

	SHORT   sRetStatus = 0;
	GCF_STATUS_STATE  GcfList[200];

	sRetStatus = CliGusAllIdReadFiltered (0, ulCashierId, 0, &GcfList[0], 200);

	if (pLeadThruMnemonic == 0 || sRetStatus < 1) {
		return (UIF_CANCEL);                /* exit ...          */
	}

	pDisp = (MLDPOPORDERUNION_VARY *) alloca (sizeof(MLDPOPORDERUNION_VARY) + sizeof(ORDERPMT_UNION) * (iMaxCount));
	NHPOS_ASSERT(pDisp);

	memset (pDisp, 0, sizeof(MLDPOPORDERUNION_VARY) + sizeof(ORDERPMT_UNION) * (iMaxCount));
	pDisp->uchMajorClass = CLASS_POPUP;
	pDisp->uchMinorClass = CLASS_POPORDER_LABELED;

	_tcsncpy(pDisp->aszLeadThrough, pLeadThruMnemonic, PARA_LEADTHRU_LEN);

	// Now we popup the OEP window with the list of coupons.
	// Then we let the operator pick.  If the operator cancels
	// out, then we pop down the OEP window and return.
	// Other wise we will then process what ever the operator selected.
	// We specify standard window size for an OEP window but the size
	//  in the Layout will override this size so we need to know what
	//  the actual size from the layout is.
	{
		int      i = 0;
		int      iRetStatus;
		int      iCurrentIndex;
		ULONG    ulBlockOffset;
		int      nScrollPage = 0, nDisplayedItems = 0;
		struct {
			int    iCurrentIndex;
			ULONG  ulBlockOffset;
		} iPreauthCurrentIndexStack[100];
		VOSRECT           VosRect;

		// Create our popup window with standard sizes.
		// we must use BlFwIfSetOEPGroups() to set the group before calling
		// the function MldCreatePopUpRectWindowOnly() in order to prevent
		// OEP window close issues the first time the OEP window is used.
		BlFwIfSetOEPGroups(BL_OEPGROUP_STR_OPENCHECK, 0);  // Open Guest Check Display OEP window #201
		VosRect.usRow = 0;    /* start row */
		VosRect.usCol = 0;   /* start colum */
		VosRect.usNumRow = MLD_NOR_POP_ROW_LEN;   /* row length MLD_NOR_POP_ROW_LEN */
		VosRect.usNumCol = MLD_NOR_POP_CLM_LEN;   /* colum length */
		nDisplayedItems = MldCreatePopUpRectWindowOnly(&VosRect);

		if (nDisplayedItems < 1)
		{
			ItemOtherClear();                   /* clear descriptor  */
			MldDeletePopUp();
			BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
			return (UIF_CANCEL);                /* exit ...          */
		}

		// Now we begin to loop through the various pages of coupons to be displayed
		// refreshing the popup window with each screenfull.  If there are more items
		// to be displayed that will fit in this window, then we need to provide a
		// continue button so the operator can go to the next page.  The MLD system
		// will automatically provide a back button for us.
		ulBlockOffset = iCurrentIndex = 0;
		while (1)
		{
			int iPreauthListNextIndex = 0;

			iPreauthCurrentIndexStack[nScrollPage].iCurrentIndex = iCurrentIndex;
			iPreauthCurrentIndexStack[nScrollPage].ulBlockOffset = ulBlockOffset;
			{
				ORDERPMT_UNION *pU = &(pDisp->u[0]);
				TCHAR  tcsTempBuffer[48];

				while (iCurrentIndex < sRetStatus) {
					memset (pU, 0, sizeof(ORDERPMT_UNION));
					iRetStatus = pU->OrderPmtText.usOrder = iCurrentIndex + 1;
					RflSPrintf(tcsTempBuffer, TCHARSIZEOF(tcsTempBuffer), _T("GC# %d - %d"), GcfList[iCurrentIndex].usGCNO, GcfList[iCurrentIndex].usTableNo);
					_tcsncpy (pU->OrderPmtText.aszLabel, tcsTempBuffer, 20);
					pU++;
					iCurrentIndex++;
				}
			}

			if (iCurrentIndex < 1) {
				UI.ulData = UIF_FILE_EMPTY;
				break;
			}

			// Fill in the popup window and then let the operator make a choice
			pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_DONE;
			pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_MORE;
			if (iPreauthListNextIndex == iCurrentIndex) {
				pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_MORE;
			}

			pDisp->ulDisplayFlags &= ~MLD_DISPLAYFLAGS_BACK;
			if (nScrollPage > 0) {
				pDisp->ulDisplayFlags |= MLD_DISPLAYFLAGS_BACK;
			}

			MldUpdatePopUp(pDisp);

			UI.ulOptions |= UIF_PAUSE_OPTION_ALLOW_SCAN;  // allow unique identifier to be scanned.

			if (UifDiaOEP(&UI, pDisp) != UIF_SUCCESS)
			{    /* abort by user     */
				UI.ulData = UIF_CANCEL;
				break;                /* exit ...          */
			}

			// If the operator choses the Continue button to go to the
			// next page, FSC_SCROLL_DOWN, then we pull the next set of coupons
			// from the pDisp array filled in above.
			// If the operator choses the Back button to go to the previous
			// page, FSC_SCROLL_UP, then we will back up to the previous set
			// of coupons from the pDisp array filled in above.
			if (UI.auchFsc[0] == FSC_SCROLL_DOWN)
			{
				if (pDisp->ulDisplayFlags & MLD_DISPLAYFLAGS_MORE) {
					nScrollPage++;
				} else {
					iCurrentIndex = iPreauthCurrentIndexStack[nScrollPage].iCurrentIndex;
					ulBlockOffset = iPreauthCurrentIndexStack[nScrollPage].ulBlockOffset;
				}
			}
			else if (UI.auchFsc[0] == FSC_SCROLL_UP)
			{
				nScrollPage--;
				if (nScrollPage < 1)
					nScrollPage = 0;
				iCurrentIndex = iPreauthCurrentIndexStack[nScrollPage].iCurrentIndex;
				ulBlockOffset = iPreauthCurrentIndexStack[nScrollPage].ulBlockOffset;
			}
			else if (UI.auchFsc[0] == FSC_CLEAR || UI.auchFsc[0] == FSC_ERROR) {
				UI.ulData = UIF_CANCEL;
				break;
			}
			else if (UI.auchFsc[0] == FSC_SCANNER) {
				GCF_STATUS_STATE  RcvBuffer[10];
				UCHAR  uchUniqueIdentifier[32];

				UifConvertStringReturningUniqueIdRaw (UI.auchTrack2, uchUniqueIdentifier);
				if (CliGusSearchForBarCode (0, uchUniqueIdentifier, RcvBuffer, 10) > 0) {
					UI.ulData = RcvBuffer[0].usGCNO;
				} else {
					UI.ulData = UIF_FILE_EMPTY;
				}
				break;
			}
			else
			{
				if (UI.ulData > 0) {
					int i;
					for (i = 0; pDisp->u[i].OrderPmtText.usOrder; i++) {
						if (pDisp->u[i].OrderPmtText.usOrder == UI.ulData) {
							if (pDisp->u[i].OrderPmtText.usOrder > 0 && pDisp->u[i].OrderPmtText.ulOffset == 0) {
								UI.ulData = GcfList[pDisp->u[i].OrderPmtText.usOrder - 1].usGCNO;
							}
							break;
						}
					}
					if (pDisp->u[i].OrderPmtText.usOrder == 0) {
						UI.ulData = 0;
					}
				}
				break;
			}
		}

		ItemOtherClear();                   /* clear descriptor  */
		MldDeletePopUp();
		BlFwIfSetOEPGroups(BL_OEPGROUP_STR_RESET, 0);
	}

	return (SHORT)UI.ulData;
}

SHORT TrnFetchServicedGuestCheck (VOID *pKeyMsg)    // handle FSC_ASK, ITM_ASK_OPENCHECKS_BY_GCN
{
	SHORT  sRetStatus = -1;
	GCF_STATUS_STATE  RcvBuffer[10];
	UCHAR  uchUniqueIdentifier[32];
	KEYMSG *pData = (KEYMSG *)pKeyMsg;

	UifConvertStringReturningUniqueIdRaw (pData->SEL.INPUT.DEV.SCA.auchStr, uchUniqueIdentifier);
	if (CliGusSearchForBarCode (0, uchUniqueIdentifier, RcvBuffer, 10) > 0) {
		sRetStatus = RcvBuffer[0].usGCNO;
	}

	return sRetStatus;
}

/* ===== End of File ( TRNQUSPL.C ) ===== */
