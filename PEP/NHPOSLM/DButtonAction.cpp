/*
-----------------------------------------------------------------------------------------------------------------------
 NHPOSLM
 Copyright (C) 2011 GSU
-----------------------------------------------------------------------------------------------------------------------
 Date	|	Changes Made																	| Version	|	By
-----------------------------------------------------------------------------------------------------------------------
 111108		New.  Replace the old DP03ToolBox.																KSo@NCR
-----------------------------------------------------------------------------------------------------------------------
*/
#include "stdafx.h"
#include "P003.h"
#include "NewLayout.h"
#include "DButtonAction.h"

SActionItem sActionItems[] =
{
//	ActionTypeId,						TextId,					Abbreviation,			Group,	Description
	{CWindowButton::BATypeAtFor,		IDD_P03_AT_FOR,			_T("@/For"),			_T(""), _T("@/For Key to enter number of items for price")},

	{CWindowButton::BATypeACKey,		IDD_P03_AC,				_T("AC"),				_T(""), _T("Permits entry of Action Code numbers and Action Code data in Supervisor Mode.")},
	{CWindowButton::BATypeAddCheck,		IDD_P03_ADDCHK,			_T("Add Check"),		_T(""), _T("Pulls up a Guest Check for total/tender/finalize sequence, requires info about which of 3 it is")},
	{CWindowButton::BATypeAdjectiveMod,	IDD_P03_ADJ_MOD,		_T("Adj. Mod"),			_T(""), _T("Adjective modify the adjective (size, etc.) of an Adjective PLU")},
	{CWindowButton::BATypeAdj,			IDD_P03_ADJ,			_T("Adjective"),		_T(""), _T("Allows addition of an adjective code from P46 to an item")},
	{CWindowButton::BATypeAlpha,		IDD_P03_ALPHA,			_T("Alpha"),			_T(""), _T("Permits entry of 12 A/N chars during trans, enters A/N entry mode")},
	{CWindowButton::BATypeSignIn,		IDD_P03_AKEY,			_T("A Key"),			_T(""), _T("Sign-in A - Signs users in")},
	{CWindowButton::BATypeAsk,			IDD_P03_ASK,			_T("Ask"),				_T(""), _T("Begins ask sequence for test inquires on EPT, charge post, frequent shopper")},	//***
	{CWindowButton::BATypeSignIn,		IDD_P03_BKEY,			_T("B Key"),			_T(""), _T("Sign-in B - Signs users in")},
	{CWindowButton::BATypeCancel,		IDD_P03_CANCEL,			_T("Cancel"),			_T(""), _T("Cancels the transaction, action code, control string, etc")},
	{CWindowButton::BATypeChgCmpt,		IDD_P03_CHGCOMP,		_T("Chg. Cmpt."),		_T(""), _T("Calculates change due after transaction finalize (if operator did not enter a tender amount, and can't calculate change)")},
	{CWindowButton::BATypeChrgTips,		IDD_P03_CHARTIP,		_T("Chrg. Tips"),		_T(""), _T("Tips should be charged (preceding numeric entry indicates amount, uses automatic % if no entry), requires tip number.")}, //***
	{CWindowButton::BATypeClear,		IDD_P03_CLEAR,			_T("Clear"),			_T(""), _T("Clears input buffer, or dismisses error messages")},
	{CWindowButton::BATypeCoupon,		IDD_P03_KEYEDCPN,		_T("Coupon"),			_T(""), _T("Predefined coupon entry, requires coupon number.")}, //***
	{CWindowButton::BATypeCouponNum,	IDD_P03_CPNNO,			_T("Coupon #"),			_T(""), _T("Manual entry of coupon number, requires previous entry")},
	{CWindowButton::BATypeCursorVoid,	IDD_P03_CURSORVOID,		_T("Cursor Void/Return"),	_T(""), _T("Voids, 0, or Returns, 1 - 3, item currently highlighted with cursor.")},
	{CWindowButton::BATypeDecTips,		IDD_P03_DECTIP,			_T("Dec. Tips"),		_T(""), _T("Indicates previous numeric entry represents a server's declaration of tips received")},
	{CWindowButton::BATypeDecimal,		IDD_P03_DECPT,			_T("Dec. Point"),		_T(""), _T("Permits entry of fractions such as partial unit weight amounts")},
	{CWindowButton::BATypeDrctShift,	IDD_P03_DRCTSHFT,		_T("Drct. Shift"),		_T(""), _T("Permits direct shift to a preset menu, requires data about which page to shift to")},
	{CWindowButton::BATypeDept,			IDD_P03_KEYEDDPT,		_T("Dept #"),			_T(""), _T("Predefined Department number entry, requires department number.")},
	{CWindowButton::BATypeDeptNum,		IDD_P03_DEPTNO,			_T("Dept"),				_T(""), _T("Indicates previous numeric entry represents a department number.")},
	{CWindowButton::BATypeErCorrect,	IDD_P03_EC,				_T("E/C"),				_T(""), _T("Error Correct, reverses the last entry")},
	{CWindowButton::BATypeFeed,			IDD_P03_FEED,			_T("Feed"),				_T(""), _T("Advances slip printer")},
	{CWindowButton::BATypeForeignTender,IDD_P03_FC,				_T("Foreign"),			_T(""), _T("Allows input of a foreign tender amount")},
	{CWindowButton::BATypeFSMod,		IDD_P03_FOODSTAMPMOD,	_T("FS Mod."),			_T(""), _T("Reverses Food Stamp status of PLU, Discount, etc.")},
	{CWindowButton::BATypeGCTrans,		IDD_P03_GCTRANS,		_T("G.C. Trnsfr"),		_T(""), _T("Start Guest Check transfer sequence.")},
	{CWindowButton::BATypeGCNum,		IDD_P03_GCNO,			_T("G.C. #, Recall Key"),	_T(""), _T("The previous numeric entry is a Guest Check Number. Also Recall Key for Drive Thru.")},
	{CWindowButton::BATypeGiftCard,		IDD_P03_GIFT_CARD,		_T("Gift Card"),		_T(""), _T("Gift card")},			// Duplicate?
	{CWindowButton::BATypeHALOoverride,	IDD_P03_HALO_OVERRIDE,	_T("HALO Ovr"),			_T(""), _T("High Amount Lock Out override key")},
	{CWindowButton::BATypeItemDisc,		IDD_P03_IDISC,			_T("Item Disc."),		_T(""), _T("Allows discounting of one item, by cursor or last item entered, requires item discount number.")},
	{CWindowButton::BATypeItemTrans,	IDD_P03_ITEMTRANS,		_T("Item Trans."),		_T(""), _T("Transfer item from one seat to another in a Guest Check, begins Item Xfer sequence")},
	{CWindowButton::BATypeJFeed,		IDD_P03_JOUFEED,		_T("Jou. Feed"),		_T(""), _T("Advances paper in journal printer")},
	{CWindowButton::BATypeScrollLeft,	IDD_P03_LEFT,			_T("Left Arrow"),		_T("Navigation"), _T("Moves cursor left one line in the scroll area")},
	{CWindowButton::BATypeLeftDisp,		IDD_P03_LEFTDISPLAY,	_T("Left Display"),		_T(""), _T("Returns cursor to current order (left column) in 3-column mode")},
	{CWindowButton::BATypeLevelSet,		IDD_P03_ADJPLS,			_T("Level Set"),		_T(""), _T("Overrides Adj level for the transaction, requires extended FSC to a preset level")},
	{CWindowButton::BATypeLineNum,		IDD_P03_LINENO,			_T("Line #"),			_T(""), _T("Permits entry of the printed line number on a Guest Check")},
	{CWindowButton::BATypeModeKey,		IDD_P03_MENUMODEKEY,	_T("Mode Key"),			_T(""), _T("LOCK, REG, SUPR, PROG modes requires extended FSC designating mode it represents")}, //***
	{CWindowButton::BATypeMoney,		IDD_P03_MONEYDEC,		_T("Money"),			_T(""), _T("Permits operator declaration of media in till, begins Money Declaration sequence")},
	{CWindowButton::BATypeNewCheck,		IDD_P03_NEWCHK,			_T("New Check"),		_T(""), _T("Creates a new Guest Check")},
	{CWindowButton::BATypeNoSale,		IDD_P03_NOSALE,			_T("No Sale"),			_T("Transaction"), _T("No Sale - Opens cash drawer")},
	{CWindowButton::BATypeNumPad,		IDD_P03_NUMBERPAD,		_T("Number Pad"),		_T(""), _T("0-9 on the number pad, requires data @ which number it represents")},
	{CWindowButton::BATypeOffTend,		IDD_P03_OFFTEND,		_T("Off. Tend."),		_T(""), _T("Accepts electronic payment if Electronic Payment (EPT) is unable to communicate")},
	{CWindowButton::BATypeOprInt,		IDD_P03_OPRINT_A,		_T("Opr. IntA"),		_T(""), _T("Operator A Interrupt, indicates previous numeric entry represents interrupting operator's ID")},
	{CWindowButton::BATypeOprInt,		IDD_P03_OPRINT_B,		_T("Opr. IntB"),		_T(""), _T("Operator B Interrupt, indicates previous numeric entry represents interrupting operator's ID")},
	{CWindowButton::BATypeOrderDeclaration,IDD_P03_ORDER_DEC,	_T("Order Dec."),		_T(""), _T("Order declaration. Requires extended FSC for Order Declaration type as number.")}, //***
	{CWindowButton::BATypePaidOrder,	IDD_P03_PAID_REC,		_T("Paid Order"),		_T(""), _T("Permits viewing of a finalized store recall transaction")},
	{CWindowButton::BATypePreBal,		IDD_P03_PB,				_T("P.B."),				_T(""), _T("Recalls a stored Guest Check by number, or by first in queue, for reorder sequence")},
	{CWindowButton::BATypePLU,			IDD_P03_KEYEDPLU,		_T("PLU"),				_T(""), _T("One-key entry of a plu, requires extended FSC as PLU number that this button represents")},
	{CWindowButton::BATypePLUNum,		IDD_P03_PLUNO,			_T("PLU #"),			_T(""), _T("For manual PLU entry, indicates previous numeric entry represents a PLU")},
	{CWindowButton::BATypePO,			IDD_P03_PO,				_T("P.O."),				_T(""), _T("Indicates previous numeric entry represents ammount Paid Out from till. Optional account number entry with #/Type.")},
	{CWindowButton::BATypePreCash,		IDD_P03_PRECASH,		_T("Preset Cash tender"),		_T(""), _T("Single Key entry of predefined tender amounts from P15 table, requires table row number.")},
	{CWindowButton::BATypePriceCheck,	IDD_P03_PRICECHECK,		_T("Price Check"),		_T(""), _T("View price of item through cursor or previous numeric entry")},
	{CWindowButton::BATypePriceEnt,		IDD_P03_PRICEENT,		_T("Price Ent."),		_T(""), _T("Manual price entry, indicates that previous numeric entry was a price")},
	{CWindowButton::BATypePrintMod,		IDD_P03_PRTMOD,			_T("Print Mod."),		_T(""), _T("Allows addition of a print modifier from P47 to an item")},
	{CWindowButton::BATypePrtDemand,	IDD_P03_PRTDEM,			_T("Prt. Dmd."),		_T(""), _T("Prints where you are in an order 2) Post Receipt 3) Park for drive through, prints post receipt with diff mnemonic")},
	{CWindowButton::BATypeQty,			IDD_P03_QTY,			_T("QTY"),				_T("Entry"), _T("Indicates that previous numeric entry represents a quantity of items to sell")},
	{CWindowButton::BATypeReceipt,		IDD_P03_RCTNORCT,		_T("Receipt"),			_T(""), _T("Toggles printing status (i.e. turns off complusory receipt printing and vice versa)")},
	{CWindowButton::BATypeRFeed,		IDD_P03_RECFEED,		_T("Rec. Feed"),		_T(""), _T("Advances paper in receipt printer")},
	{CWindowButton::BATypeRepeat,		IDD_P03_REPEAT,			_T("Repeat"),			_T(""), _T("Repeats last item or the cursor-focused menu item")},
	{CWindowButton::BATypeRevPrint,		IDD_P03_RVSPRI,			_T("Rev. Print"),		_T(""), _T("Prints last 99 lines of Electronic Journal")},
	{CWindowButton::BATypeRegDisc,		IDD_P03_RDISC,			_T("Rglr. Disc."),		_T(""), _T("Allows discounting of an entire check/transaction.")},
	{CWindowButton::BATypeScrollRight,	IDD_P03_RIGHT,			_T("Right Arrow"),		_T("Navigation"),	_T("Moves cursor right one line in the scroll area")},
	{CWindowButton::BATypeROA,			IDD_P03_ROA,			_T("R.O.A."),			_T(""), _T("Indicates previous numeric entry represents an amount paid by customer on account. Optional account number entry with #/Type.")},
	{CWindowButton::BATypeScaleTare,	IDD_P03_SCALE,			_T("Scale/Tare"),		_T(""), _T("Manually enters weight, price, tare weight on items not marked as scaleable")},
	{CWindowButton::BATypeScrollDown,	IDD_P03_DOWN,			_T("Scroll Down"),		_T("Navigation"),	_T("Moves cursor down one line in the scroll area")},
	{CWindowButton::BATypeScrollUp,		IDD_P03_UP,				_T("Scroll Up"),		_T("Navigation"),	_T("Moves cursor up one line in the scroll area")},
	{CWindowButton::BATypeSeatNum,		IDD_P03_SEAT,			_T("Seat #"),			_T(""), _T("Indicates that previous numeric entry represents the seat number data")}, //***
	{CWindowButton::BATypeShift,		IDD_P03_SHIFT,			_T("Shift"),			_T(""), _T("Indicates that previous numeric entry represents a desired menu page shift (indicated by prev num entry)")},
	{CWindowButton::BATypeSplit,		IDD_P03_SPLIT,			_T("Split"),			_T(""), _T("Permits visual and financial splitting of Guest Check for payment, begins Split Check sequence.")},
	{CWindowButton::BATypeString,		IDD_P03_KEYEDSTR,		_T("String"),			_T(""), _T("Run Control String, requires extended FSC as Control String number.")},
	{CWindowButton::BATypeStringNum,	IDD_P03_STRNO,			_T("String #"),			_T(""), _T("Manual entry of a Control String, indicates previous numeric entry is a Control String number.")},
	{CWindowButton::BATypeSurrogateNum,	IDD_P03_BARTENDER,		_T("Surrogate #"),		_T(""), _T("Permits cashier to ring up one trans for a server, indicates prev num entry represents server num.")},
	{CWindowButton::BATypeTableNum,		IDD_P03_TBLNO,			_T("Table #"),			_T(""), _T("Indicates that previous numeric entry represents which table the Guest Check is for")},
	{CWindowButton::BATypeTaxMod,		IDD_P03_TAXMOD,			_T("Tax Mod."),			_T(""), _T("Predefined tax group modifier, requires extended FSC tax itemizer number, range depends on Tax System.")},
	{CWindowButton::BATypeTender,		IDD_P03_TENDER,			_T("Tender"),			_T(""), _T("One of the 20 tender keys, requires extended FSC Tender Key number, tender key must be provisioned.")},
	{CWindowButton::BATypeTimeIn,		IDD_P03_TIMEIN,			_T("Time-In"),			_T(""), _T("Begins time in sequence for the employee indicated by previous numeric entry")},
	{CWindowButton::BATypeTimeOut,		IDD_P03_TIMEOUT,		_T("Time-Out"),			_T(""), _T("Executes time out for the employee indicated by previous numeric entry")},
	{CWindowButton::BATypeTipPO,		IDD_P03_TIPSPO,			_T("Tips P.O."),		_T(""), _T("Permits payment in cash for charge tips")},
	{CWindowButton::BATypeTotal,		IDD_P03_TTL,			_T("Total"),			_T(""), _T("One of the 20 total keys, requires extended FSC Total Key number, total key must be provisioned.")},

	{CWindowButton::BATypeTranVoid,		IDD_P03_VTRAN,			_T("Trans. Void/Return"),	_T("Transaction"),	_T("Voids, 0, or Returns, 1 - 3, ALL items in the current transaction")}, //***
	{CWindowButton::BATypeUPCE,			IDD_P03_UPCEVER,		_T("UPC-E"),			_T(""), _T("Begins UPC-E code entry sequence")},
	{CWindowButton::BATypeVoid,			IDD_P03_VOID,			_T("Void"),				_T(""), _T("Void's a PLU, Dept, R/A, P/O, discount, etc by cursor or line reentry")},	//?
	{CWindowButton::BATypeWait,			IDD_P03_WAIT,			_T("Wait"),				_T(""), _T("Moves a paid store/recal trans to the right column on 3-column.  Can then be bumped off using recall paid key")},
	{CWindowButton::BATypeWicTrans,		IDD_P03_WICTRANS,		_T("WIC Trans."),		_T(""), _T("Indentify trans as WIC, Begins WIC transaction entry")},
	{CWindowButton::BATypeWicMod,		IDD_P03_WICMOD,			_T("WIC Mod."),			_T(""), _T("Reverses WIC status of a PLU or dept. entry")},
	{CWindowButton::BATypeWinDis,		IDD_P03_MENUWINDREMOVE,	_T("Win. Dismiss"),		_T(""), _T("Dismiss a window")},	//?
	{CWindowButton::BATypeWindow,		IDD_P03_MENUWIND,		_T("Window"),			_T(""), _T("Indicates that this button leads to another window of buttons")},	//***
	{CWindowButton::BATypeDblZero,		IDD_P03_00KEY,			_T("00 Key"),			_T(""), _T("Sends two 0 digits as data entry, used for round dollar amounts $X.00")},
	{CWindowButton::BATypeNumPerson,	IDD_P03_NOPERSON,		_T("# Person"),			_T(""), _T("Indicates that previous numeric entry represents the number of people on a Guest Check")},
	{CWindowButton::BATypeNumType,		IDD_P03_NOTYPE,			_T("#/Type"),			_T(""), _T("Indicates that previous numeric entry represents a number or type entry")},
	{CWindowButton::BATypeNumPad,		IDD_P03_DEF_KEYPAD,		_T("Def #Pad"),			_T(""), _T("0-9 on the number pad, requires data @ which number it represents")},
	{CWindowButton::BATypeCharacter,	IDD_P03_CHARACTER_BUTTON,_T("Char. Button"),	_T(""), _T("Enter a character (A, 1, &, or character)")},
	{CWindowButton::BATypeCharacterDelete,IDD_P03_CHARACTER_DELETE,_T("Char. Delete"),	_T(""), _T("Delete a character")},
	{CWindowButton::BATypeProg1,		IDD_P03_P1,				_T("P1"),				_T(""), _T("Program Mode Key - P1. Used as put away or Enter key in A/N keyboard mode.")},
	{CWindowButton::BATypeProg2,		IDD_P03_P2,				_T("P2"),				_T(""), _T("Program Mode Key - P2. Used to toggle A/N mode or N mode for keyboard entry.")},
	{CWindowButton::BATypeProg3,		IDD_P03_P3,				_T("P3"),				_T(""), _T("Program Mode Key - P3")},
	{CWindowButton::BATypeProg4,		IDD_P03_P4,				_T("P4"),				_T(""), _T("Program Mode Key - P4. Used to change to Numeric keyboard mode.")},
	{CWindowButton::BATypeProg5,		IDD_P03_P5,				_T("P5"),				_T(""), _T("Program Mode Key - P5. Used as Cancel key for A/N keyboard mode.")},

	{CWindowButton::BATypeManualValidation,IDD_P03_VALIDATION,	_T("Valid."),			_T(""), _T("Manual validation key")},
	{CWindowButton::BATypeMinimize,		IDD_P03_MINIMIZE,		_T("Minimize"),			_T(""), _T("Minimize the application")},
	{CWindowButton::BATypePLU_Group,	IDD_P03_PLU_GROUP,		_T("PLU Group"),		_T(""), _T("PLU group number for dynamic PLU functionality, requires group number from PLU data.")},
	{CWindowButton::BATypeEditCondiment,IDD_P03_EDIT_CONDIMENT,	_T("Edit Cond."),		_T(""), _T("Edit a Condiment that is highlighted by cursor in Receipt Window.")},
	{CWindowButton::BATypeReceiptId,	IDD_P03_TENT,			_T("Receipt ID"),		_T(""), _T("Receipt or Tent Id to identify an order.")},

	{CWindowButton::BATypeDocumentLaunch,IDD_P03_DOC_LAUNCH,	_T("Launch Appl."),		_T(""), _T("Launch a document through shortcut in Database folder")},	//***
	{CWindowButton::BATypeOperatorPickup,IDD_P03_OPR_PICKUP,	_T("Cash Pickup"),		_T(""), _T("Register Mode cash pickup like AC10")},
	{CWindowButton::BATypeEditCondimentTbl,IDD_P03_EDIT_COND_TBL,_T("Edit Cond. Tbl."),		_T(""), _T("Edit a Condiment Table")},
	{CWindowButton::BATypeDisplayLabeledWindow,IDD_P03_WINDOW_DISPLAY,	_T("Display Win"),		_T(""), _T("Display a window using numeric prefix of name, prefix is embedded")}, //***
	{CWindowButton::BATypeDismissLabeledWindow,IDD_P03_WINDOW_DISMISS,	_T("Dismiss Win"),		_T(""), _T("Dismiss a window using numeric prefix of name, prefix is embedded")},	//***
	{CWindowButton::BATypeDisplayLabeledWindowManual,IDD_P03_WINDOW_DISPLAY_MANUAL,_T("Display Win #"),	_T(""), _T("Display a window using numeric prefix of name, prefix is manually typed in")},
	{CWindowButton::BATypeDismissLabeledWindowManual,IDD_P03_WINDOW_DISMISS_MANUAL,_T("Dismiss Win #"),	_T(""), _T("Dismiss a window using numeric prefix of name, prefix is manually typed in")},
	{CWindowButton::BATypePLU_GroupTable,IDD_P03_PLU_GROUPTABLE,_T("PLU Table"),		_T(""), _T("PLU group for dynamic PLU functionality using multiple OEP tables")},
	{CWindowButton::BATypeNumTypeCaption,IDD_P03_NOTYPE,_T("#/Type Caption"),		_T(""), _T("Insert button caption as a #/Type entry.")},

	{CWindowButton::BATypeVoid, 0, NULL, NULL, NULL}	// This is the end marker of the table
};


int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nRetVal = 0;
	PSActionItem pData1 = (PSActionItem)lParam1;
	PSActionItem pData2 = (PSActionItem)lParam2;

	if (pData1 == NULL || pData2 == NULL)
		return 0;

	switch(lParamSort)
	{
	case 0:	// Abbreviation
		if (pData1->Abbreviation != NULL && pData2->Abbreviation != NULL)
			nRetVal = _tcscmp(pData1->Abbreviation, pData2->Abbreviation);
		break;
	//case 2:	// Group
	//	if (pData1->Group != NULL && pData2->Group != NULL)
	//		nRetVal = _tcscmp(pData1->Group, pData2->Group);
	//	break;
	case 1: // Description
		if (pData1->Description != NULL && pData2->Description != NULL)
			nRetVal = _tcscmp(pData1->Description, pData2->Description);
		break;
	default:
		break;
	}

	return nRetVal;
}

IMPLEMENT_DYNAMIC(DButtonAction, CDialog)

DButtonAction::DButtonAction(CWnd* pParent /*=NULL*/) : CDialog(DButtonAction::IDD, pParent)
{}

DButtonAction::~DButtonAction() {}

void DButtonAction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACTION, m_cActionList);
	DDX_Control(pDX, IDC_STATIC_CURRENTACTIONNAME, m_cCurrentActionName);
}

BEGIN_MESSAGE_MAP(DButtonAction, CDialog)
	ON_WM_CREATE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DButtonAction::OnHdnItemclickListAction)
	ON_BN_CLICKED(IDOK, &DButtonAction::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ACTION, &DButtonAction::OnNMDblclkListAction)
END_MESSAGE_MAP()

BOOL DButtonAction::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Create columns
	this->m_cActionList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);
	this->m_cActionList.InsertColumn(0, _T("Action"), LVCFMT_LEFT, 100);
	//this->m_cActionList.InsertColumn(2, _T("Group"), LVCFMT_LEFT, 80);
	this->m_cActionList.InsertColumn(1, _T("Description"), LVCFMT_LEFT, 530);

	int idx = 0;
	int curActionIdx = -1;

	// Populate action list and remember which one is the current action for the button if any
	PSActionItem pItem = &sActionItems[idx];
	while (pItem->Abbreviation != NULL)
	{
		m_cActionList.InsertItem(idx, pItem->Abbreviation);
		//m_cActionList.SetItemText(idx, 2, pItem->Group);
		m_cActionList.SetItemText(idx, 1, pItem->Description);
		m_cActionList.SetItemData(idx, (LPARAM)pItem);

		if (this->m_currentActionId == pItem->ActionTypeId && this->m_currentTextId == pItem->TextId)
		{
			curActionIdx = idx;
			this->m_cCurrentActionName.SetWindowText(pItem->Abbreviation);
		}

		idx++;
		pItem = &sActionItems[idx];
	}

	// If there is a current action, select it
	if (curActionIdx != -1)
	{
		m_cActionList.SetSelectionMark(curActionIdx);
		m_cActionList.SetItemState(curActionIdx, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}

	// Sort the list; this will change the selected index
	m_cActionList.SortItems(SortFunc, 0);

	// Now ensure the current action is visible.  Note that the index may be changed after sorting
	if (curActionIdx != -1)
	{
		idx = m_cActionList.GetSelectionMark();
		m_cActionList.EnsureVisible(idx, FALSE);
	}

	return TRUE;	// return TRUE unless we override the focus
}

int DButtonAction::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void DButtonAction::OnHdnItemclickListAction(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;
	m_cActionList.SortItems(SortFunc, pLV->iItem);	
	*pResult = 0;
}

void DButtonAction::OnBnClickedOk()
{
	int idx = m_cActionList.GetSelectionMark();
	PSActionItem pItem = (PSActionItem)(m_cActionList.GetItemData(idx));
	if (pItem != NULL)
	{
		this->m_currentActionId = pItem->ActionTypeId;
		this->m_currentTextId = pItem->TextId;
	}
	OnOK();
}


void DButtonAction::OnNMDblclkListAction(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnBnClickedOk();
}
