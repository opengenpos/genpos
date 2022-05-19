// FrameworkWndItem.cpp : implementation file
//

#include "stdafx.h"
#include "Framework.h"
#include "FrameworkWndItem.h"
#include "FrameworkWndCtrl.h"
#include "bl.h"
#include "paraequ.h"
#include "para.h"
#include "ecr.h"
#include "Evs.h"
#include "appllog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CFrameworkWndItemAdHoc, CFrameworkWndItem)
	//{{AFX_MSG_MAP(CFrameworkWndItemAdHoc)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WU_EVS_CREATE, OnEvsCreate)
END_MESSAGE_MAP()



CFrameworkWndItemAdHoc::CFrameworkWndItemAdHoc(int id, int row, int column, int width, int height) :
	CFrameworkWndItem (id, row, column, width, height)
{
	m_pChainNext = 0;
	m_pChainNextParent = 0;

	controlAttributes.m_nType = WindowContainerAdHoc;
}

	
CFrameworkWndItemAdHoc::CFrameworkWndItemAdHoc(CWindowControl *wc) :
	CFrameworkWndItem(wc)
{
	m_pChainNext = 0;
	m_pChainNextParent = 0;

	controlAttributes.m_nType = WindowContainerAdHoc;
}

CFrameworkWndItemAdHoc::~CFrameworkWndItemAdHoc()
{
}

LRESULT CFrameworkWndItemAdHoc::OnEvsCreate(WPARAM wParam, LPARAM lParam)
{
	// Give up our timeslice to allow BusinessLogic to do its thing so
	// that when the PopupWindow() method fires, BusinessLogic will have
	// things set properly.
	Sleep (0);
	if (m_pChainNext) {
		TRACE2("%S(%d): CFrameworkWndItemAdHoc::OnEvsCreate() \n", __FILE__, __LINE__);
		m_pChainNext->PopupWindow (m_pChainNextParent);
	}
	DestroyWindow ();
	return 0;
}

// id -> the id number for this window
// row -> the row of grids on which to place top left corner of window
// column -> the column of grids on which to place top left corner of window
// width -> width of the window in grids
// height -> height of the window in grids
CFrameworkWndItem::CFrameworkWndItem(int id, int row, int column, int width, int height) :
	CWindowItem (id, row, column, width, height)
{
	m_faceBrush = 0;
	m_nPickCount = 0;
	m_bInsideSuperIntrv = 0;
	m_pWebBrowser = 0;
}

	
CFrameworkWndItem::CFrameworkWndItem(CWindowControl *wc) :
	CWindowItem(wc)
{
	m_faceBrush = 0;
	m_nPickCount = 0;
	m_bInsideSuperIntrv = 0;
	m_pWebBrowser = 0;
	m_nRollupState = 0;			// current state of the rollup window. UserDefaultWin_ROLLUP_WIN
	m_nRollupRow[0] = m_nRollupRow[1] = 0;
	m_nRollupColumn[0] = m_nRollupColumn[1] = 0;
	m_nRollupHeight[0] = m_nRollupHeight[1] = 0;
	m_nRollupWidth[0] = m_nRollupWidth[1] = 0;
}

CFrameworkWndItem::~CFrameworkWndItem()
{
	TRACE3("%S(%d): CFrameworkWndItem::~CFrameworkWndItem() destructor called  window named '%s'.\n", __FILE__, __LINE__, myName);
	DeleteObject(m_faceBrush);

	if (m_pWebBrowser) {
		delete m_pWebBrowser;
		m_pWebBrowser = 0;
	}

	POSITION pclListNext = ButtonItemList.GetHeadPosition ();
	POSITION pclList = pclListNext;
	CWindowControl *bp;
	while (pclListNext && (bp = ButtonItemList.GetNext (pclListNext))) {
		delete bp;
		ButtonItemList.RemoveAt (pclList);
		pclList = pclListNext;
	}
}

BOOL CFrameworkWndItem::WindowCreate (CWindowControl *pParentWnd, UINT nID, int scale /* = 100 */)
{
	BOOL  bRet;
	
	TRACE3("%S(%d): BOOL CFrameworkWndItem::WindowCreate() window named '%s'\n", __FILE__, __LINE__, myName);

	bRet = CWindowItem::WindowCreate (pParentWnd, nID, scale);

	// initialize the size information for possible use by rollup and rolldown of a displayed window.
	// see uses of control attribute UserDefaultWin_ROLLUP_WIN for window rollup/rolldown.
	m_nRollupRow[1] = (m_nRollupRow[0] = controlAttributes.m_nRow);
	m_nRollupColumn[1] = (m_nRollupColumn[0] = controlAttributes.m_nColumn);
	m_nRollupHeight[1] = (m_nRollupHeight[0] = controlAttributes.m_nHeight) / 4;
	m_nRollupWidth[1] = (m_nRollupWidth[0] = controlAttributes.m_nWidth);

	m_nPickCount = 0;
	if (bRet)
	{
		if (controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_WEB_BROWSER) {
			if (m_pWebBrowser == 0) {
				m_pWebBrowser = new CWebBrowser2;
				ASSERT (m_pWebBrowser);
				if (m_pWebBrowser) {
					RECT  myRect;
					this->GetClientRect (&myRect);
					if (!m_pWebBrowser->Create(NULL, NULL, WS_VISIBLE, myRect, this, NULL))
					{
						TRACE("failed to create browser\n");
						delete m_pWebBrowser;
						m_pWebBrowser = 0;
					} else {
						// Initialize the first URL.
						COleVariant noArg;
						CString strURL("http://www.google.com");
						m_pWebBrowser->Navigate(this->windowAttributes.webbrowser.url, &noArg, &noArg, &noArg, &noArg);
					}
				}
			}
		} else {
			if (windowAttributes.busRules.NumOfPicks <= 0 || windowAttributes.busRules.NumOfPicks > 100) {
				windowAttributes.busRules.NumOfPicks = 0;
			}

			m_nPickCount = windowAttributes.busRules.NumOfPicks;
			if(!m_faceBrush){
				m_faceBrush = CreateSolidBrush(controlAttributes.m_colorFace);
			}

			WindowRefreshOEPGroup ();
		}
	}
	else {
		PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_WINCREATE_FAIL_1);
	}

	return bRet;
}


/*
 *  CheckCaptionForMnemonicReplacement()
 *
 * Description: Examine a button caption as specified in a Layout file and determine
 *              if the caption has the special characters indicating that the provisioned
 *              mnemonic such as Transaction Mnemonic or Special Mnemonic should be used
 *              for the button caption or not.
 *
 *              This is used for some buttons that it makes sense to be done this way. Buttons
 *              for such use include: Tender Key, Total Key, Adjectives, etc.
 *
 *              We already have something like this for PLU groups. This is meant to allow the use
 *              of a single layout with multiple PEP databases in which the button captions need to
 *              change from PEP database to PEP database.
 *
*/
static BOOL CheckCaptionForMnemonicReplacement (LPCTSTR caption)
{
	CONST TCHAR  tchUsePrintMnemonic[2] = { _T('!'), _T('!') };   // caption special characters !! are to use Print Mnemonic

	return (caption[0] == tchUsePrintMnemonic[0] && caption[1] == tchUsePrintMnemonic[1]);
}

BOOL CFrameworkWndItem::WindowRefreshOEPGroup ()
{
	int          nButton = 0;
	int          nRetStat = 0;
	BL_PLU_INFO  PluInfo;
	
	if (controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_WEB_BROWSER) {
		return TRUE;
	}

	if ( ! (controlAttributes.SpecWin[SpecWinIndex_OEP] & SpecWinMask_OEP_FLAG))
	{
		int          nPluInfoIndex = 0;
		SHORT        sGroupNumber = 0;
		SHORT        sTableNumber = 0;
		USHORT       usFilePart = 0;
		USHORT       usAdjectiveFilter = controlAttributes.SpecWin[SpecWinIndex_FILTER_Conditions];
		BL_PLU_INFO  PluInfoArray[20];

		USHORT  fsComStatus = BlGetNoticeBoardStatus(0);
		if ( (fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE | BLI_STS_M_REACHABLE)) == 0) {
			fsComStatus = BlGetNoticeBoardStatus(1);
		}

		// Review the button list to see if there are any PLU_Group type buttons in the
		// list.  If so, then we need to update their captions, turn off unused
		// buttons, etc.
		memset (&PluInfoArray, 0, sizeof(PluInfoArray));

		POSITION pclListNext = ButtonItemList.GetHeadPosition ();
		CWindowControl *bp;
		if (pclListNext) {
			int nRetry = 3;
			if ( (fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE | BLI_STS_M_REACHABLE)) == 0) {
				PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_FCOMSTATUS_FAIL);
			}
			else {
				nRetry = 3;
				while (! BlFwIfIsPluFileUnLocked() && nRetry > 0) {
					PifSleep(100);
					nRetry--;
				}
				if (nRetry < 1) {
					PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_PLUFILETEST_FAIL);
				}
			}
			// for Dynamic PLU windows we want to be able to describe what adjectives PLUs should be
			// displayed.  what this does is to limit the different versions of an adjective PLU that
			// will display in the window so that a window can be designated as displaying PLUs with
			// a specific size.  for instance if a PLU is Soft Drink with Small, Medium, and Large we
			// can specify that a window is only for displaying the Large size.
			while (pclListNext && (bp = ButtonItemList.GetNext (pclListNext))) {
				ULONG  ulPluAdjectiveFilter = 0;
				USHORT  usPluAdjectiveValue = (controlAttributes.SpecWin[SpecWinIndex_FILTER_Conditions] & SpecWinFilter_ValueMask);

				ulPluAdjectiveFilter |= (usAdjectiveFilter & SpecWinFilter_Adj) ? BL_PLU_INFO_FILTER_ADJ : 0;
				ulPluAdjectiveFilter |= (usAdjectiveFilter & SpecWinFilter_AdjNonZero) ? BL_PLU_INFO_FILTER_ADJNONZERO : 0;
				ulPluAdjectiveFilter |= (usAdjectiveFilter & SpecWinFilter_AdjValueLogicLT) ? BL_PLU_INFO_FILTER_ADJ_LOGIC_LT : 0;
				ulPluAdjectiveFilter |= (usAdjectiveFilter & SpecWinFilter_AdjValueLogicGT) ? BL_PLU_INFO_FILTER_ADJ_LOGIC_GT : 0;

				CFrameworkWndButton *button = CFrameworkWndButton::TranslateCWindowControl (bp);

				if (button)
				{
					button->auchPluNo[0] = 0;
					if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_Group ||
						button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_GroupTable)
					{
						usFilePart = 0;
						if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_Group) {
							sTableNumber = 0;
							sGroupNumber = button->btnAttributes.m_myActionUnion.data.extFSC;
						}
						else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU_GroupTable) {
							sTableNumber = button->btnAttributes.m_myActionUnion.data.group_table.usTableNumber;
							sGroupNumber = button->btnAttributes.m_myActionUnion.data.group_table.usGroupNumber;
							usFilePart = (button->btnAttributes.m_myActionUnion.data.group_table.usOptionFlags & BUTTONACTIONTYPEGROUP_PRIORITY) ? 1 : 0;
						}
						else {
							sTableNumber = -1;
							sGroupNumber = -1;
						}

						nRetStat = 1;
						if (sTableNumber >= 0) {
							// If we have Master and Backup Master communications and we know
							// where to go to get the PLU information then do so.
							// However, if we do not see a Master terminal or the Notice Board
							// indicates there is some kind of Master/Backup communication problem
							// then we will just display blank buttons rather than buttons with
							// PLU information on them.
							if ((fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE | BLI_STS_M_REACHABLE)) != 0)
							{
								do {
									for (nPluInfoIndex = 0;
										PluInfoArray[nPluInfoIndex].usGroupNo && nPluInfoIndex < sizeof(PluInfoArray) / sizeof(PluInfoArray[0]);
										nPluInfoIndex++)
									{
										if (PluInfoArray[nPluInfoIndex].usTableNo == sTableNumber && PluInfoArray[nPluInfoIndex].usGroupNo == sGroupNumber && PluInfoArray[nPluInfoIndex].usFilePart == usFilePart)
										{
											nRetStat = BlFwIfGetGroupPluInformationNext(&PluInfoArray[nPluInfoIndex]);
											break;
										}
									}
									if (nPluInfoIndex < sizeof(PluInfoArray) / sizeof(PluInfoArray[0]) && PluInfoArray[nPluInfoIndex].usGroupNo == 0)
									{
										nRetStat = BlFwIfGetGroupPluInformationFirst(sTableNumber, sGroupNumber, usFilePart, ulPluAdjectiveFilter, usPluAdjectiveValue, &PluInfoArray[nPluInfoIndex]);
									}
								} while (nRetStat >= 100);
							}
						}
						if (nRetStat == 0)
						{
							// We will setup this button for a particular PLU.
							// Set the button caption to the PLU description and
							// make sure that the button is enabled if the button
							// is currently being displayed (m_hWnd is non-zero).
							button->myCaption = PluInfoArray[nPluInfoIndex].auchAdjName;
							button->myCaption += _T(" ");
							button->myCaption += PluInfoArray[nPluInfoIndex].uchPluName;
							memcpy(button->auchPluNo, PluInfoArray[nPluInfoIndex].auchPluNo, sizeof(button->auchPluNo));
							button->uchPluAdjMod = PluInfoArray[nPluInfoIndex].uchPluAdjMod;
							if (button->m_hWnd)
							{
								button->EnableWindow(TRUE);
								button->ShowWindow(SW_SHOW);
							}
						}
						else
						{
							// We will turn this button off as there isn't a PLU in the
							// group for this button.  Erase any caption and make sure
							// the button is disabled so that it won't work if the button
							// is currently being displayed (m_hWnd is non-zero).
							button->myCaption = _T("    ");
							button->auchPluNo[0] = 0;
							button->uchPluAdjMod = 0;
							if (button->m_hWnd)
							{
								button->EnableWindow(FALSE);
								button->ShowWindow(SW_HIDE);
							}
						}
						// update the window if it is currently being displayed
						if (button->m_hWnd)
						{
							button->Invalidate();
							button->UpdateWindow();
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeWindow) {
						// if the button type is to display another window and the button caption starts
						// with special characters then populate the button caption with the name of the target window.
						if (CheckCaptionForMnemonicReplacement(button->myCaption)) {
							TCHAR tcsName[36] = { 0 };
							if (CWindowDocument::pDoc->GetWindowName(button->btnAttributes.m_myActionUnion.data.nWinID, tcsName)) {
								button->myCaption = tcsName;
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU) {  // button caption replacement
						// If the button type is PLU and the button caption starts with special characters
						// then we will auto populate the button caption with the PLU mnemonic.
						// The PLU information is set in AC 68, PLU Maintenance.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							if ( (fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE | BLI_STS_M_REACHABLE)) != 0) {
								BL_PLU_INFO   PluInfo = {0};
								UCHAR         uchAdj = 0;

								if (BlFwIfGetPluInformation(button->btnAttributes.m_myActionUnion.data.PLU, uchAdj, &PluInfo) == 0) {
									// We will only populate if the PLU Group number is non-zero. This filter allows
									// us to change whether a PLU button is displayed or not by changing the group
									// number of the PLU.
									if (PluInfo.usGroupNo) {
										if (PluInfo.auchAdjName[0]) {
											button->myCaption = PluInfo.auchAdjName;
											button->myCaption += _T(" ");
											button->myCaption += PluInfo.uchPluName;
										} else {
											button->myCaption = PluInfo.uchPluName;
										}
									} else {
										if (button->m_hWnd)
										{
											button->EnableWindow (FALSE);
											button->ShowWindow (SW_HIDE);
										}
									}
								}
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeCoupon) {  // button caption replacement
						// If the button type is coupon and the button caption starts with special characters
						// then we will auto populate the button caption with the coupon mnemonic.
						// The coupon information is set in AC 161, Coupon Maintenance, which is also
						// used for the Auto Coupon functionality.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							BL_CPN_INFO           WorkCoupon = {0};

							WorkCoupon.usCpnNo = button->btnAttributes.m_myActionUnion.data.extFSC;
							if (BlFwIfGetCouponInformation(WorkCoupon.usCpnNo, &WorkCoupon) == 0) {
								button->myCaption = WorkCoupon.aszCouponMnem;
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeOrderDeclaration) {  // button caption replacement
						// If the button type is order declare and the button caption starts with special characters
						// then we will auto populate the button caption with the order declare mnemonic.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usExtFsc = (USHORT)(TRN_ORDER_DEC_OFFSET + button->btnAttributes.m_myActionUnion.data.extFSC);
							TCHAR   MemoryMnemonic[32] = {0};

							if (BlFwIfGetMemoryResidentMnemonic (BLI_MNEMOTYPE_P20_TRANS, usExtFsc, MemoryMnemonic) == 0) {
								button->myCaption = MemoryMnemonic;
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeRegDisc) {  // button caption replacement
						// If the button type is Regular Discount (Transaction Discount) and the button caption starts with special characters
						// then we will auto populate the button caption with the Transaction Discount Key mnemonic.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usTranAddressTable[] = {
								TRN_RDISC1_ADR,       // table is indexed by one not zero so just duplicate first entry
								TRN_RDISC1_ADR,
								TRN_RDISC2_ADR,
								TRN_RDISC3_ADR,
								TRN_RDISC4_ADR,
								TRN_RDISC5_ADR,
								TRN_RDISC6_ADR
							};
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (usExtFsc < sizeof(usTranAddressTable)/sizeof(usTranAddressTable[0])) {
								usExtFsc = usTranAddressTable[usExtFsc];
								if (BlFwIfGetMemoryResidentMnemonic (BLI_MNEMOTYPE_P20_TRANS, usExtFsc, MemoryMnemonic) == 0) {
									button->myCaption = MemoryMnemonic;
								}
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeItemDisc) {  // button caption replacement
						// If the button type is Item Discount Key and the button caption starts with special characters
						// then we will auto populate the button caption with the Item Discount Key mnemonic.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usTranAddressTable[] = {
								TRN_ITMDISC_ADR,       // table is indexed by one not zero so just duplicate first entry
								TRN_ITMDISC_ADR,
								TRN_MODID_ADR,
								TRN_ITMDISC_ADR_3,
								TRN_ITMDISC_ADR_4,
								TRN_ITMDISC_ADR_5,
								TRN_ITMDISC_ADR_6
							};
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (usExtFsc < sizeof(usTranAddressTable)/sizeof(usTranAddressTable[0])) {
								usExtFsc = usTranAddressTable[usExtFsc];
								if (BlFwIfGetMemoryResidentMnemonic (BLI_MNEMOTYPE_P20_TRANS, usExtFsc, MemoryMnemonic) == 0) {
									button->myCaption = MemoryMnemonic;
								}
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeTender) {  // button caption replacement
						// If the button type is Tender Key and the button caption starts with special characters
						// then we will auto populate the button caption with the Tender Key mnemonic.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usTranAddressTable[] = {
								TRN_TEND1_ADR,       // table is indexed by one not zero so just duplicate first entry
								TRN_TEND1_ADR,
								TRN_TEND2_ADR,
								TRN_TEND3_ADR,
								TRN_TEND4_ADR,
								TRN_TEND5_ADR,
								TRN_TEND6_ADR,
								TRN_TEND7_ADR,
								TRN_TEND8_ADR,
								TRN_TEND9_ADR,
								TRN_TEND10_ADR,
								TRN_TEND11_ADR,
								TRN_TENDER12_ADR,
								TRN_TENDER13_ADR,
								TRN_TENDER14_ADR,
								TRN_TENDER15_ADR,
								TRN_TENDER16_ADR,
								TRN_TENDER17_ADR,
								TRN_TENDER18_ADR,
								TRN_TENDER19_ADR,
								TRN_TENDER20_ADR
							};
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (usExtFsc < sizeof(usTranAddressTable)/sizeof(usTranAddressTable[0])) {
								usExtFsc = usTranAddressTable[usExtFsc];
								if (BlFwIfGetMemoryResidentMnemonic (BLI_MNEMOTYPE_P20_TRANS, usExtFsc, MemoryMnemonic) == 0) {
									button->myCaption = MemoryMnemonic;
								}
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeForeignTender) {  // button caption replacement
						// If the button type is Tender Key and the button caption starts with special characters
						// then we will auto populate the button caption with the Tender Key mnemonic.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usTranAddressTable[] = {
								TRN_FT1_ADR,       // table is indexed by one not zero so just duplicate first entry
								TRN_FT1_ADR,
								TRN_FT2_ADR,
								TRN_FT3_ADR,
								TRN_FT4_ADR,
								TRN_FT5_ADR,
								TRN_FT6_ADR,
								TRN_FT7_ADR,
								TRN_FT8_ADR
							};
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (usExtFsc < sizeof(usTranAddressTable)/sizeof(usTranAddressTable[0])) {
								usExtFsc = usTranAddressTable[usExtFsc];
								if (BlFwIfGetMemoryResidentMnemonic (BLI_MNEMOTYPE_P20_TRANS, usExtFsc, MemoryMnemonic) == 0) {
									button->myCaption = MemoryMnemonic;
								}
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePreCash) {  // button caption replacement, FSC_PRESET_AMT
						// If the button type is P15 Preset Cash Tender Key and the button caption starts with special characters
						// then we will auto populate the button caption with a Preset Cash Tender Key mnemonic .
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (BlFwIfGetPresetCashMnemonic (usExtFsc, MemoryMnemonic) == 0) {
								button->myCaption = MemoryMnemonic;
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeTotal) {  // button caption replacement
						// If the button type is Total Key and the button caption starts with special characters
						// then we will auto populate the button caption with the Total Key mnemonic.
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usTranAddressTable[] = {
								TRN_TTL1_ADR,       // table is indexed by one not zero so just duplicate first entry
								TRN_TTL1_ADR,
								TRN_TTL2_ADR,
								TRN_TTL3_ADR,
								TRN_TTL4_ADR,
								TRN_TTL5_ADR,
								TRN_TTL6_ADR,
								TRN_TTL7_ADR,
								TRN_TTL8_ADR,
								TRN_TTL9_ADR,
								TRN_TOTAL10_ADR,
								TRN_TOTAL11_ADR,
								TRN_TOTAL12_ADR,
								TRN_TOTAL13_ADR,
								TRN_TOTAL14_ADR,
								TRN_TOTAL15_ADR,
								TRN_TOTAL16_ADR,
								TRN_TOTAL17_ADR,
								TRN_TOTAL18_ADR,
								TRN_TOTAL19_ADR,
								TRN_TOTAL20_ADR
							};
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (usExtFsc < sizeof(usTranAddressTable)/sizeof(usTranAddressTable[0])) {
								usExtFsc = usTranAddressTable[usExtFsc];
								if (BlFwIfGetMemoryResidentMnemonic (BLI_MNEMOTYPE_P20_TRANS, usExtFsc, MemoryMnemonic) == 0) {
									button->myCaption = MemoryMnemonic;
								}
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeDept) {  // button caption replacement
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							BL_DEPT_INFO           WorkDepartment = {0};

							WorkDepartment.usDeptNo = atoi ((CHAR *)&(button->btnAttributes.m_myActionUnion.data.PLU[0]));
							if (BlFwIfGetDepartInformation(WorkDepartment.usDeptNo, &WorkDepartment) == 0) {
								button->myCaption = WorkDepartment.aszDeptMnem;
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeAdj) {  // button caption replacement
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (BlFwIfGetAdjectiveMnemonic(usExtFsc, MemoryMnemonic) == 0) {
								button->myCaption = MemoryMnemonic;
							}
						}
					} else if (button->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePrintMod) {  // button caption replacement
						if (CheckCaptionForMnemonicReplacement (button->myCaption)) {
							USHORT  usExtFsc = button->btnAttributes.m_myActionUnion.data.extFSC;
							TCHAR   MemoryMnemonic[32] = {0};

							if (BlFwIfGetPrintModifierMnemonic(usExtFsc, MemoryMnemonic) == 0) {
								button->myCaption = MemoryMnemonic;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// If this is an auto OEP window then we will need to obtain the group number(s) associated
		// with the window and to then begin to populate the window with the buttons for the
		// OEP groups.  The difference between an auto OEP window and a window with Dynamic PLU buttons
		// is that an auto OEP windown will try to put as many buttons as will fit (depending on the
		// window size and the button size) where as the Dynamic PLU buttons are buttons that will
		// be assigned PLUs from a group and the buttons are placed in the window using the Layout Manager.
		if (controlAttributes.SpecWin[SpecWinIndex_OEP] & SpecWinMask_AUTO_OEP_FLAG)
		{
			CFrameworkWndTextSubWindow  mySubWindow;
			SHORT  sGroupNumber = 0;
			SHORT  sTableNumber = 0;
			CFrameworkWndButton *done;
			CFrameworkWndButton *button;

			mySubWindow.CompileOepString (controlAttributes.mOEPGroupNumbers);

			int nMaxNumberButtons = windowAttributes.oepBtnAcross * windowAttributes.oepBtnDown - 3;
			//make sure the control list is empty
			RemovePopupButton();

			for (sGroupNumber = mySubWindow.GetGroupNoFirst (); sGroupNumber >= 0 && nMaxNumberButtons > 0; sGroupNumber = mySubWindow.GetGroupNoNext ())
			{
				if (sGroupNumber > 0)
				{
					memset (&PluInfo, 0, sizeof(PluInfo));

					sTableNumber = 0;

					nRetStat = BlFwIfGetGroupPluInformationFirst(sTableNumber, sGroupNumber, 0, 0L, 0, &PluInfo);
					while (nRetStat == 100) {
						// A return code of 100 from BlFwIfGetGroupPluInformationNext() indicates that the next PLU
						// from the OEP table file that is being retrieved does not exist in the PLU data base so we
						// need to skip this PLU and just go to the next one.
						nRetStat = BlFwIfGetGroupPluInformationNext(&PluInfo);
					}
					while (nRetStat == 0)
					{
						button = new CFrameworkWndButton((CFrameworkWndButton::SPL_BTN_START + nButton),
														((nButton/windowAttributes.oepBtnAcross) * windowAttributes.oepBtnHeight),
														((nButton%windowAttributes.oepBtnAcross) * windowAttributes.oepBtnWidth),
														windowAttributes.oepBtnWidth, windowAttributes.oepBtnHeight);

						if (button) {
							button->btnAttributes.m_myActionUnion.type = CWindowButton::BATypePLU_Group;
							button->btnAttributes.m_myActionUnion.data.extFSC = sGroupNumber;

							button->myCaption = PluInfo.auchAdjName;
							button->myCaption += _T(" ");
							button->myCaption += PluInfo.uchPluName;
							memcpy (button->auchPluNo, PluInfo.auchPluNo, sizeof(button->auchPluNo));
							button->uchPluAdjMod = PluInfo.uchPluAdjMod;

							button->controlAttributes.m_usWidthMultiplier = windowAttributes.oepBtnWidth;
							button->controlAttributes.m_usHeightMultiplier = windowAttributes.oepBtnHeight;
							button->controlAttributes.m_colorFace = windowAttributes.oepBtnColor;//Set the button's Face Color
							button->btnAttributes.m_myShape = windowAttributes.oepBtnShape;
							button->btnAttributes.m_myPattern = windowAttributes.oepBtnPattern;
							button->controlAttributes.m_colorText = windowAttributes.oepBtnFontColor;
							button->controlAttributes.lfControlFont = windowAttributes.oepBtnFont;
							button->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
							button->m_DialogId = (CFrameworkWndButton::SPL_BTN_START + nButton);

							if (button->WindowCreate(this, button->m_DialogId, 100)) {
								button->UpdateWindow();
								ButtonItemList.AddTail (button);
								nButton++;
								nMaxNumberButtons--;
							}
							else {
								delete button;
								PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_OEP_BUTTON_FAIL_1);
							}
						}
						// A return code of 100 from BlFwIfGetGroupPluInformationNext() indicates that the next PLU
						// from the OEP table file that is being retrieved does not exist in the PLU data base so we
						// need to skip this PLU and just go to the next one.
						do {
							nRetStat = BlFwIfGetGroupPluInformationNext(&PluInfo);
						} while (nRetStat == 100);
					}
				}
			}

			if (nButton)
			{
				PARATRANSMNEMO paraTransMnemo = {0};

				paraTransMnemo.uchMajorClass = CLASS_PARATRANSMNEMO;
				paraTransMnemo.uchAddress = TRN_NONE_DONE_ADR;
				BlFwIfParaRead(&paraTransMnemo);
				done = new CFrameworkWndButton(CFrameworkWndButton::SPL_BTN_DONE, ((windowAttributes.oepBtnDown - 1) * windowAttributes.oepBtnHeight),
													((windowAttributes.oepBtnAcross - 1) * windowAttributes.oepBtnWidth),
													windowAttributes.oepBtnWidth, windowAttributes.oepBtnHeight);

				if (done) {
					done->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeWinDis;

					done->myCaption = paraTransMnemo.aszMnemonics;
					done->controlAttributes.m_usWidthMultiplier = windowAttributes.oepBtnWidth;
					done->controlAttributes.m_usHeightMultiplier = windowAttributes.oepBtnHeight;
					done->controlAttributes.m_colorFace = windowAttributes.oepBtnColor;//Set the button's Face Color
					done->btnAttributes.m_myShape = windowAttributes.oepBtnShape;
					done->btnAttributes.m_myPattern = windowAttributes.oepBtnPattern;
					done->controlAttributes.m_colorText = windowAttributes.oepBtnFontColor;
					done->controlAttributes.lfControlFont = windowAttributes.oepBtnFont;
					done->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
					done->m_DialogId = CFrameworkWndButton::SPL_BTN_DONE;

					if (done->WindowCreate((CWindowControl *)this, done->m_DialogId, 100)) {
						done->UpdateWindow();

						ButtonItemList.AddTail (done);
					}
					else {
						delete done;
						PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_OEP_BUTTON_FAIL_2);
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CFrameworkWndItem::WindowDisplay (CDC* pDC)
{
	int iSaveDC = pDC->SaveDC ();

	CPen penGreen;
	penGreen.CreatePen(PS_SOLID, 3, RGB(20, 150, 20));
	CPen* pOldPen = pDC->SelectObject(&penGreen);

	CBrush* pOldBrush = (CBrush *)pDC->SelectObject(GetStockObject (HOLLOW_BRUSH));

	pDC->Rectangle(CFrameworkWndButton::getRectangleSized (controlAttributes.m_nColumn,
															controlAttributes.m_nRow,
															controlAttributes.m_usWidthMultiplier,
															controlAttributes.m_usHeightMultiplier));

	POSITION  currentPos = ButtonItemList.GetHeadPosition ();
	POSITION  currentPosLast = currentPos;
	while (currentPos) {
		CWindowControl *pwc = ButtonItemList.GetNext (currentPos);
		pwc->WindowDisplay (pDC);
		if (windowAttributes.busRules.NumOfPicks > 0) {
			CFrameworkWndLabel *pwcLabel = CFrameworkWndLabel::TranslateCWindowControl (pwc);
			if (pwcLabel && pwcLabel->labelAttributes.PickCounter) {
				TCHAR  tcLabelText[10];
				_itot (m_nPickCount, tcLabelText, 10);
				pwcLabel->WindowUpdateText(tcLabelText);
			}
		}

		currentPosLast = currentPos;
	}

	pDC->RestoreDC (iSaveDC);
	return TRUE;
}


BOOL CFrameworkWndItem::PopupSupervisorWindow (CWindowControl *pParent)
{
	// Now populate this window with our Supervisor controls
	CFrameworkWndButton *NumpadButton, *ClearButton;
	int          nButton = 0, iButton = 1;
	TCHAR        tcButtonCaption[5] = { _T(' '), _T('0'), _T(' '), 0};

	for (nButton = 0; nButton < 10; nButton++) {
		NumpadButton = new CFrameworkWndButton((CFrameworkWndButton::SPL_BTN_START + nButton),
										((nButton / windowAttributes.oepBtnAcross) * windowAttributes.oepBtnHeight),
										((nButton % windowAttributes.oepBtnAcross) * windowAttributes.oepBtnWidth),
										windowAttributes.oepBtnWidth, windowAttributes.oepBtnHeight);

		if (NumpadButton) {
			NumpadButton->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeNumPad;
			NumpadButton->btnAttributes.m_myActionUnion.data.extFSC = nButton;

			tcButtonCaption[1] = nButton + _T('0');
			NumpadButton->myCaption = tcButtonCaption;
//			NumpadButton->controlAttributes.m_usWidthMultiplier = windowAttributes.oepBtnWidth;
//			NumpadButton->controlAttributes.m_usHeightMultiplier = windowAttributes.oepBtnHeight;
			NumpadButton->controlAttributes.m_colorFace = windowAttributes.oepBtnColor;//Set the button's Face Color
			NumpadButton->btnAttributes.m_myShape = windowAttributes.oepBtnShape;
			NumpadButton->btnAttributes.m_myPattern = windowAttributes.oepBtnPattern;
			NumpadButton->controlAttributes.m_colorText = windowAttributes.oepBtnFontColor;
			NumpadButton->controlAttributes.lfControlFont = windowAttributes.oepBtnFont;
			NumpadButton->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
			NumpadButton->m_DialogId = (CFrameworkWndButton::SPL_BTN_START + nButton);

			if (NumpadButton->WindowCreate(this, NumpadButton->m_DialogId, 100)) {
				NumpadButton->UpdateWindow();
				ButtonItemList.AddTail (NumpadButton);
			}
			else {
				delete NumpadButton;
				PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_OEP_BUTTON_FAIL_1);
			}
		}
	}

	NumpadButton = new CFrameworkWndButton((CFrameworkWndButton::SPL_BTN_START + nButton),
									((windowAttributes.oepBtnDown - 1) * windowAttributes.oepBtnHeight),
									((windowAttributes.oepBtnAcross - 2) * windowAttributes.oepBtnWidth),
									windowAttributes.oepBtnWidth, windowAttributes.oepBtnHeight);

	if (NumpadButton) {
		NumpadButton->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeACKey;
		NumpadButton->btnAttributes.m_myActionUnion.data.extFSC = 0;

		NumpadButton->myCaption = _T(" AC ");
		NumpadButton->controlAttributes.m_usWidthMultiplier = windowAttributes.oepBtnWidth;
		NumpadButton->controlAttributes.m_usHeightMultiplier = windowAttributes.oepBtnHeight;
		NumpadButton->controlAttributes.m_colorFace = windowAttributes.oepBtnColor;//Set the button's Face Color
		NumpadButton->btnAttributes.m_myShape = windowAttributes.oepBtnShape;
		NumpadButton->btnAttributes.m_myPattern = windowAttributes.oepBtnPattern;
		NumpadButton->controlAttributes.m_colorText = windowAttributes.oepBtnFontColor;
		NumpadButton->controlAttributes.lfControlFont = windowAttributes.oepBtnFont;
		NumpadButton->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
		NumpadButton->m_DialogId = (CFrameworkWndButton::SPL_BTN_START + nButton);

		if (NumpadButton->WindowCreate(this, NumpadButton->m_DialogId, 100)) {
			NumpadButton->UpdateWindow();
			ButtonItemList.AddTail (NumpadButton);
		}
		else {
			delete NumpadButton;
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_OEP_BUTTON_FAIL_1);
		}
	}

	PARATRANSMNEMO paraTransMnemo;

	memset(&paraTransMnemo, 0x00, sizeof(paraTransMnemo));

	paraTransMnemo.uchMajorClass = CLASS_PARATRANSMNEMO;
	paraTransMnemo.uchAddress = TRN_NONE_DONE_ADR;
	BlFwIfParaRead(&paraTransMnemo);
	ClearButton = new CFrameworkWndButton(CFrameworkWndButton::SPL_BTN_DONE,
										((windowAttributes.oepBtnDown - 1) * windowAttributes.oepBtnHeight),
										((windowAttributes.oepBtnAcross - 1) * windowAttributes.oepBtnWidth),
										windowAttributes.oepBtnWidth, windowAttributes.oepBtnHeight);

	// Though the label on this button will be the None/Done label
	// we want it to actually send a Clear key sequence into BusinessLogic.
	// This will get us out of the enter Supervisor Number sequence.
	if (ClearButton) {
		ClearButton->btnAttributes.m_myActionUnion.type = CWindowButton::BATypeClear;

		ClearButton->myCaption = paraTransMnemo.aszMnemonics;
		ClearButton->controlAttributes.m_usWidthMultiplier = windowAttributes.oepBtnWidth;
		ClearButton->controlAttributes.m_usHeightMultiplier = windowAttributes.oepBtnHeight;
		ClearButton->controlAttributes.m_colorFace = windowAttributes.oepBtnColor;//Set the button's Face Color
		ClearButton->btnAttributes.m_myShape = windowAttributes.oepBtnShape;
		ClearButton->btnAttributes.m_myPattern = windowAttributes.oepBtnPattern;
		ClearButton->controlAttributes.m_colorText = windowAttributes.oepBtnFontColor;
		ClearButton->controlAttributes.lfControlFont = windowAttributes.oepBtnFont;
		ClearButton->btnAttributes.capAlignment = CWindowButton::CaptionAlignmentVMiddle;
		ClearButton->m_DialogId = CFrameworkWndButton::SPL_BTN_DONE;

		if (ClearButton->WindowCreate(this, ClearButton->m_DialogId, 100)) {
			ClearButton->UpdateWindow();

			ButtonItemList.AddTail (ClearButton);
		}
		else {
			delete ClearButton;
			PifLog (MODULE_FRAMEWORK, LOG_EVENT_FW_OEP_BUTTON_FAIL_2);
		}
	}

	return TRUE;
}


BOOL CFrameworkWndItem::PopdownWindow ()
{
	return CWindowControl::PopdownWindow ();
}

BOOL CFrameworkWndItem::PopupWindow (CWindowControl *pParent)
{
	// Check whether we are restricted to one or more terminal modes (SUPR, PROG, REG).

	// First check is to see if this container has restrictions.
	// If so, then lets check to see if the restriction matches our current terminal mode.
	// If the terminal mode required matches the current terminal mode then we go ahead
	// and do the action.  Otherwise we will just return and do nothing.
	if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type]) {
		BOOL  bLockWindow = TRUE;
		if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_SUPR) {
			if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_SUP) {
				bLockWindow = FALSE;
			}
			else {
				// Lets request Supervisor Intervention to finish popping up this window.
				// If we are not already in Supervisor Intervention then we will request
				// it by sending a FSC_SUPR_INTRVN code to the ring buffer which is the
				// same as CWindowButton::BATypeSuprIntrvn 
				if (BLI_SUPR_INTV_APPROVED == BlUifSupIntervent(BLI_SUPR_INTV_READ)) {
					bLockWindow = FALSE;
					this->m_bInsideSuperIntrv = TRUE;
				}
				else {
					CHARDATA        CharData;
					CharData.uchFlags = 0;
					CharData.uchASCII = 0xFF; 
					CharData.uchScan  = 0x23;
					CFrameworkWndButton::sendButtonMessageToBL(&CharData);

					CFrameworkWndItemAdHoc  *pTempPopup = new CFrameworkWndItemAdHoc (0, 1, 1, 20, 30);
					pTempPopup->m_pChainNext = this;
					pTempPopup->m_pChainNextParent = pParent;

					pTempPopup->WindowCreate (pParent);
					pTempPopup->ShowWindow (SW_SHOW);

					pTempPopup->PopupSupervisorWindow (pParent);
					pTempPopup->BringWindowToTop();
					pTempPopup->Invalidate ();
					pTempPopup->UpdateWindow ();
				}
			}
		}
		if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_PROG) {
			if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_PROG) {
				bLockWindow = FALSE;
			}
		}
		if (controlAttributes.SpecWin[SpecWinIndex_SPR_Type] & SpecWinMask_SPR_REG) {
			if (BlGetKeyboardLockPosition() == PIF_NCRKLOCK_MODE_REG) {
				bLockWindow = FALSE;
			}
		}
		if (bLockWindow)
			return FALSE;
	}

	{
		CASDATA_RTRV casdata;
		BlFwIfGetCasData(&casdata);

		//check the group permissions to see if the user is allowed to use this control

		//does this control have any group permissions set
		if(controlAttributes.ulMaskGroupPermission)
			//does the currently signed in user have permission to use this button
			if(!(controlAttributes.ulMaskGroupPermission & casdata.ulGroupAssociations))
				return FALSE;	//disallow if the user does not have permission
	}

	if (windowAttributes.busRules.NumOfPicks > 0) {
		POSITION  currentPos = ButtonItemList.GetHeadPosition ();
		POSITION  currentPosLast = currentPos;
		while (currentPos) {
			CWindowControl *pwc = ButtonItemList.GetNext (currentPos);
			CFrameworkWndLabel *pwcLabel = CFrameworkWndLabel::TranslateCWindowControl (pwc);
			if (pwcLabel && pwcLabel->labelAttributes.PickCounter) {
				TCHAR  tcLabelText[10];
				_itot (windowAttributes.busRules.NumOfPicks, tcLabelText, 10);
				pwcLabel->myCaption = tcLabelText;
			}

			currentPosLast = currentPos;
		}
	}

	BOOL bRet;
	if ((bRet = CWindowControl::PopupWindow (pParent))) {
		if (controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_WEB_BROWSER) {
			// Initialize the first URL.
			COleVariant noArg;
			CString strURL("http://www.google.com");
			m_pWebBrowser->Navigate(this->windowAttributes.webbrowser.url, &noArg, &noArg, &noArg, &noArg);
		}

#if 0
		// handle translation if BlFwIfWindowTranslate(bTranslate) has been called to turn on translation of window
		// for Left/Right Handedness.
		if (m_wParamWindowTranslation && (controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_WINTRANSLATE) == 0) {
			int xPos = controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth;
			int yPos = controlAttributes.m_nRow * CFrameworkWndButton::stdHeight;
			// translate from right side of Y-axis to left side of Y-axis
			// then swap left and right top corners then shift the window back
			// to the right side of the Y-axis.
			xPos *= -1;
			xPos -= controlAttributes.m_nWidth;
			xPos += GetSystemMetrics (SM_CXSCREEN);
			::SetWindowPos (m_hWnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
		}
#endif
	}
	return bRet;
}

void CFrameworkWndItem::CreatePopupButton(CFrameworkWndButton * pButton)
{
	pButton->WindowCreate((CWindowControl *)this, pButton->m_DialogId, 100);
	pButton->UpdateWindow();
	ButtonItemList.AddTail (pButton);
}

void CFrameworkWndItem::RemovePopupButton(CFrameworkWndButton * pButton)
{
	// if a particular button is specified then delete that button.
	// if no particular button is specified then delete all buttons
	if (pButton) {
		POSITION pclListNext = ButtonItemList.GetHeadPosition ();
		POSITION pclList;
		CFrameworkWndButton *bp;
		CWindowControl *pwc;
		if (pclListNext) {
			pclList = pclListNext;
			while (pclListNext && (pwc = ButtonItemList.GetNext (pclListNext))) {
				bp = CFrameworkWndButton::TranslateCWindowControl (pwc);
				if (bp && bp->m_DialogId == pButton->m_DialogId)
				{
					delete bp;
					ButtonItemList.RemoveAt (pclList);
					break;
				}
				pclList = pclListNext;
			}
		}
	}
	else {
		POSITION pclListNext = ButtonItemList.GetHeadPosition ();
		POSITION pclList;
		CFrameworkWndButton *bp;
		CWindowControl *pwc;
		if (pclListNext) {
			pclList = pclListNext;
			while (pclListNext && (pwc = ButtonItemList.GetNext (pclListNext))) {
				bp = CFrameworkWndButton::TranslateCWindowControl (pwc);
				if (bp)
				{
					delete bp;
					ButtonItemList.RemoveAt (pclList);
				}
				pclList = pclListNext;
			}
		}
	}
}

void CFrameworkWndItem::ControlSerialize (CArchive &ar) 
{
	UCHAR* pBuff = (UCHAR*) &windowAttributes;
	if (ar.IsStoring())
	{
		CWindowItem::ControlSerialize (ar);
	}
	else
	{
		nEndOfArchive = 0;
		try {
			WindowDef.Serialize(ar);

			ULONG mySize;
			memset(&windowAttributes,0,sizeof(_tagWinItemAttributes));
			ar>>mySize;

			// Because the file that is being loaded in may be from a
			// different version of Layout Manager, we must be careful
			// about just reading in the information from the file.
			// If the object stored is larger than the object in the
			// version of Layout Manager being used then we will
			// overwrite memory and cause application failure.
			ULONG  xMySize = mySize;
			UCHAR  ucTemp;

			if (xMySize > sizeof (windowAttributes)) {
				xMySize = sizeof (windowAttributes);
			}
			UINT y = 0;
			for(y = 0; y < xMySize;y++){
				ar>>pBuff[y];
			}
			for(; y < mySize;y++){
				ar>>ucTemp;
			}

			ASSERT(windowAttributes.signatureStart = 0xABCDEF87);

			POSITION pclListNext = ButtonItemList.GetHeadPosition ();
			POSITION pclList;
			CWindowControl *bp;
			if (pclListNext) {
				pclList = pclListNext;
				while (pclListNext && (bp = ButtonItemList.GetNext (pclListNext))) {
					ASSERT(bp);
					delete bp;
					ButtonItemList.RemoveAt (pclList);
					pclList = pclListNext;
				}
			}

			CFrameworkWndControl bpTemp;
			try {
				int nCount = 0;
				ar >> nCount;
				for ( ; nCount > 0; nCount--) {
					bp = bpTemp.SerializeNew (ar);
					ASSERT(bp);
					if (bp) {
						bp->pContainer = this;
						ButtonItemList.AddTail (bp);
					}
				}
			}
			catch (CArchiveException  *e) {
				nEndOfArchive = 1;
				e->Delete();
			}
			catch (CFileException *e) {
				nEndOfArchive = 1;
				e->Delete();
			}
		}
		catch (CArchiveException  *e) {
			nEndOfArchive = 1;
			e->Delete();
		}
		catch (CFileException *e) {
			nEndOfArchive = 1;
			e->Delete();
		}
	}
}

void CFrameworkWndItem::CheckLastPick(void)
{
	if(windowAttributes.busRules.NumOfPicks <= 0 || windowAttributes.busRules.NumOfPicks > 100) {
		windowAttributes.busRules.NumOfPicks = 0;
		m_nPickCount = 0;
		return;
	}else{
		--m_nPickCount;
		if(m_nPickCount == 0){
			m_nPickCount = windowAttributes.busRules.NumOfPicks;
			TRACE2("%S(%d): CFrameworkWndItem::CheckLastPick () DestroyWindow ()\n", __FILE__, __LINE__);
			DestroyWindow();
		}
		else {
			POSITION  currentPos = ButtonItemList.GetHeadPosition ();
			while (currentPos) {
				CWindowControl *pwc = ButtonItemList.GetNext (currentPos);
				CFrameworkWndLabel *pwcLabel = CFrameworkWndLabel::TranslateCWindowControl (pwc);
				if (pwcLabel && pwcLabel->labelAttributes.PickCounter) {
					TCHAR  tcLabelText[10];
					_itot (m_nPickCount, tcLabelText, 10);
					pwcLabel->WindowUpdateText(tcLabelText);
					break;
				}
			}
		}
	}
	return;
}

CWindowControl * CFrameworkWndItem::SearchForItemByName (TCHAR *tcWindowName)
{
	CWindowControl *bi = 0;
	POSITION pos = ButtonItemList.GetHeadPosition ();

	//cycle through the items in the documents control list
	while (pos) {
		bi = ButtonItemList.GetNext (pos);
		ASSERT(bi);
		if (bi) {
			if ( _tcscmp (tcWindowName, bi->myName) == 0) {
				return bi;
			}
		}
	}
	return 0;
}


BEGIN_MESSAGE_MAP(CFrameworkWndItem, CWindowItem)
	//{{AFX_MSG_MAP(CFrameworkWndItem)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameworkWndItem message handlers

void CFrameworkWndItem::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(lpDrawItemStruct->CtlType == ODT_BUTTON)
	{
		CFrameworkWndButton::DrawButton(nIDCtl, lpDrawItemStruct, this);
	}else{
		CWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

void CFrameworkWndItem::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//if the mouse button is released when not on a button
	//set the focus to the top level window so as to deselect
	//the last button selected and redraw the
	//the window item to redraw the buttons as up
	GetTopLevelParent()->SetFocus();
	Invalidate(FALSE);

	if (controlAttributes.SpecWin[UserDefaultWinIndex] & UserDefaultWin_ROLLUP_WIN) {
		if (m_nRollupState < 0 || m_nRollupState > 1) m_nRollupState = 0;

		m_nRollupState = 1 - m_nRollupState;      // Toggle the state of the window to the new state

		int divisor = (m_nRollupState) ? 5 : 1;
		int iLeftSide = (controlAttributes.m_nColumn * CFrameworkWndButton::stdWidth);
		int iTopSide = (controlAttributes.m_nRow * CFrameworkWndButton::stdHeight);
		int iHeight = (controlAttributes.m_nHeight / divisor);
		int iWidth = (controlAttributes.m_nWidth / divisor);

		::SetWindowPos (m_hWnd, HWND_NOTOPMOST, iLeftSide, iTopSide, iWidth, iHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	}

	CWindowItem::OnLButtonUp(nFlags, point);
}

HBRUSH CFrameworkWndItem::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWindowItem::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_BTN){
		pDC->SetBkMode(TRANSPARENT);
		//this is the parent so the color of this FrameworkWndItem
		//is the color of the child windows background
		//return the HBRUSH used to paint the background of
		//button controls
		hbr = m_faceBrush;
	}
		
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CFrameworkWndItem::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ShowCursor(TRUE);
	
	CWindowItem::OnRButtonDown(nFlags, point);
}
