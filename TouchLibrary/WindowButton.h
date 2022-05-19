#if !defined(AFX_WINDOWBUTTON_H__B4DD3905_DF5E_4D98_BA1E_C5FB5347A952__INCLUDED_)
#define AFX_WINDOWBUTTON_H__B4DD3905_DF5E_4D98_BA1E_C5FB5347A952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowButton.h : header file
//

#include "WindowControl.h"



/////////////////////////////////////////////////////////////////////////////
// CWindowButton window

class CWindowButton : public CWindowControl
{
// Construction
public:
	CWindowButton (CWindowControl *wc = 0);
	CWindowButton (int id, int row, int column, int width = 2, int height = 2);
	virtual ~CWindowButton();

// Attributes
public:
	enum {stdWidth = STD_GRID_WIDTH, stdHeight = 15, stdLeading = 1};

	typedef enum {ButtonShapeRect = 1, ButtonShapeElipse, ButtonShapeRoundedRect } ButtonShape;
	typedef enum {ButtonPatternNone = 1, ButtonPatternHoriz, ButtonPatternVert} ButtonPattern;
	typedef enum {ButtonFontSize6 = 0, ButtonFontSize8, ButtonFontSize10, ButtonFontSize12, ButtonFontSize14, ButtonFontSize16} ButtonFontSize;
	typedef enum {CaptionAlignmentHLeft = 0, CaptionAlignmentHCenter, CaptionAlignmentHRight, CaptionAlignmentVTop, CaptionAlignmentVMiddle, CaptionAlignmentVBottom} CaptionAlignment;
	typedef enum {IconAlignmentHLeft = 0, IconAlignmentHCenter, IconAlignmentHRight} HorizontalIconAlignment;
	typedef enum {IconAlignmentVTop = 0, IconAlignmentVMiddle, IconAlignmentVBottom} VerticalIconAlignment;

	//-------------------------------------------------------------------------------------------------------
	//  ButtonActionType indicates which key press this button represents.
	//	(Some types require extra information, which is stored in ButtonActionUnion
	//
	// WARNING:  The following enum must be in order.  If you are adding a new button type, you MUST add
	//           it to the end of the list.  If you do not, you will cause problems with previous layout files
	//           because this is an enumeration and are in a numerical order sequence.
	//
	typedef enum {BATypePLUNum,	//PLU# - For manual PLU entry, indicates previous numeric entry represents a PLU
				BATypePLU,		//PLU - One-key entry of a plu, requires PLU number that this button represents
				BATypeDept,		//Dept - One key entry of a dept, requires Dept number that this button represents
				BATypeDeptNum,	//Dept # - For Dept entry, requires dept number that thus button represents
				BATypePrintMod,	//Print Modifier - Allows addition of a print modifier from P47 to an item
				BATypeAdj,		//Adjective - Allows addition of an adjective mnemonic address from P46 to an item
				//Following Tender keys must declare which tender they are, whether they use a programmed total key,
				//     and the amount of tender, if button specified.
				BATypeTender,		//Tender - One of the 8 tender keys, requres data about which tender
				BATypeForeignTender,//Foreign - Allows input of a foreign tender amount
				BATypeTotal,		//Total - Provides total key functionalities, must have data as to which total button represents
				BATypeAddCheck,	//Add Check - Pulls up a GC for total/tender/finalize sequence, requires info about which of 3 it is
				BATypeItemDisc,	//Item Disc - Allows discounting of one item, by cursor or line reentry
				BATypeRegDisc,	//Reglr Disc. - Allows discounting of an entire check/transaction.
				BATypeTaxMod,		//Tax Mod. - Predefined tax group modifier, requires data about which tax group will be assessed
				BATypeSignIn,		//Sign-in A / Sign-in B - Signs users in, requires data about whether button is A or B
				BATypeNewCheck,	//New Check - Creates a new GC
				BATypePreBal,		//P.B. - Recalls a stored GC by number, or by first in queue, for reorder sequence
				BATypeGCNum,		//G.C. # - Indicates that the previous numeric entry is a Guest Check Number
				BATypeGCTrans,	//G.C. Trnsfr - Indicates that a guest check transfer is about to occur
				BATypeNumPerson,	//# Person - Indicates that previous numeric entry represents the number of people on a GC
				BATypeTableNum,	//Table # - Indicates that previous numeric entry represents which table the GC is for
				BATypeLineNum,	//Line # - Permits entry of the printed line number on a GC
				BATypeFeed,		//Feed - Advances slip printer
				BATypeReceipt,	//Reciept - Toggles printing status (i.e. turns off complusory receipt printing and vice versa)
				BATypeTipPO,		//Tips P.O - Permits payment in cash for charge tips. 
				BATypeDecTips,	//Dec. Tips - Indicates previous numeric entry represents a server's declaration of tips received
				BATypeNoSale,		//No Sale - Opens cash drawer.
				BATypePO,			//P.O - Paid out subtracts from the media in the till
				BATypeROA,		//Received On Account - Indicates previous numeric entry represents an amount paid by customer on an account
				BATypeChgCmpt,	//Chg Cmpt - Calculates change due after transaction finalize (if operator did not enter a tender amount, and can't calculate change)
				BATypeCancel,		//Cancel - Cancels the transaction, action code, control string, etc
				BATypeVoid,		//Void - Void's a PLU,Dept,R/A,P/O, discount,etc by cursor or line reentry
				BATypeErCorrect,	//E/C - Error Correct, reverses the last entry
				BATypeTranVoid,	//Trans Void - voids ALL items in the current transaction
				BATypeQty,		//QTY - Indicates that previous numeric entry represents a quantity of items to sell
				BATypeShift,		//Shift - Indicates that previous numeric entry represents a desired menu page shift (indicated by prev num entry)
				BATypeNumType,	//#/Type - Indicates that previous numeric entry represents a number or type entry
				BATypePrtDemand,	//Prt Dmd - 1) Prints where you are in an order 2) Post Receipt 3) Park for drive through, prints post receipt with diff mnemonic
				BATypeChrgTips,	//Chrg. Tips - Indicates that tips should be charged (preceding numeric entry indicates amount, uses automatic % if no entry)
				BATypeDblZero,	//Sends to 0's to BL's Ring buffer, used for round dollar amounts "$X.00"
				BATypeACKey,		//AC - Permist entry of Action Code numbers and Action Code Data
				BATypeRFeed,		//Rec. Feed - Advances paper in receipt printer
				BATypeJFeed,		//Jou. Feed - Advances paper in journal printer
				BATypeDecimal,	//Dec. Point - Permits entry of fractions
				BATypeScaleTare,	//Scale/Tare - Manually enters weight, price, tare weight on items not marked as scaleable.
				BATypeRevPrint,	//Rev. Print - Prints last 99 lines of electronic journal
				BATypePreCash,	//Pre. Cash - Single Key entry of predefined tender amounts, requires index into P15 Preset Cash table, FSC_PRESET_AMT
				BATypeDrctShift,	//Drct. Shift - Permits direct shift to a preset menu, requires data about which page to shift to
				BATypeOffTend,	//Off Line Tender - Accepts electronic payment if EPT is unable to communicate.
				BATypeTimeIn,		//Time-In - Begins time in sequence for the employee indicated by previous numeric entry
				BATypeTimeOut,	//Time-Out - Executes time out for the employee indicated by previous numeric entry
				BATypeCouponNum,	//Coupon # - Manual entry of coupon number, requires previous entry
				BATypeCoupon,		//Coupon - Predefined coupon entry, required data about which coupon number it represents
				BATypeScrollUp,	//Scroll Up - Moves cursor up one line in the scroll area
				BATypeScrollDown,	//Scroll Down - Moves cursor down one line in the scroll area
				BATypeScrollLeft,	//Arrow Left- Moves cursor left one line in the scroll area
				BATypeScrollRight,//Arrow Right - Moves cursor right one line in the scroll area	
				BATypeWait,		//Wait - moves a paid store/recal trans to the right column on 3-column.  Can then be bumped off using recall paid key
				BATypePaidOrder,	//Paid Order - Permits viewing of a finalized store recall transaction
				BATypeStringNum,	//Control String # - Manual entry of a control string, indicates previous numeric entry is a CS
				BATypeString,		//Control String - Predefined CS entry, requires data about which CS it represents.
				BATypeAlpha,		//Alpha - Permits entry of 12 A/N chars during trans, enters A/N entry mode
				BATypeLeftDisp,	//Left Display - Returns cursor to current order (left column) in 3-column mode
				BATypeSurrogateNum,//Surrogate # - Permits cashier to ring up one trans for a server, indicates prev num entry represents server num
				BATypeSplit,		//Split - Permits visual and financial splitting of CG for payment, begins Split Check sequence
				BATypeSeatNum,	//Seat # - Indicates that previous numeric entry represents the seat number data,
				BATypeItemTrans,	//Item Trans. - Transfer item from one seat to another in a GC, begins Item Xfer sequence
				BATypeRepeat,		//Repeat - Repeats last item or the cursor-focused menu item
				BATypeCursorVoid,	//Cursor Void - Voids cursor-focused menu item
				BATypePriceCheck,	//Price Check - View price of item through cursor or previous numeric entry
				BATypeOprInt,		//OpIntA/B - requires info about A or B, indicates previous numeric entry represents interrupting operator's ID.
				BATypeMoney,		//Money - Permits operator declaration of media in till, begins Money Declaration sequence	 
				BATypeAsk,		//Ask - Begins ask sequence for test inquires on EPT, charge post, frequent shopper 
				BATypeLevelSet,	//Level Set - Overrides Adj level for the transaction, requires extended FSC to a preset level
				BATypeUPCE,		//UPC-E - Begins UPC-E code entry sequence
				BATypePriceEnt,	//Price Enter - Manual price entry, indicates that previous numeric entry was a price
				BATypeFSMod,		//Food Stamp Modifier - Reverses or toggles Food Stamp status of a PLU or dept. entry
				BATypeWicTrans,	//WIC (Women, Infants, and Children program) Trans - Indentify trans as WIC, Begins WIC transaction entry
				BATypeWicMod,		//WIC (Women, Infants, and Children program) Modifier - Reverses or toggles WIC status of a PLU or dept. entry
				BATypeClear,		//Clear - Clears input buffer, or dismisses error messages
				BATypeNumPad,		//**NEW IN TOUCHSCREEN** - 0-9 on the number pad, requires data @ which number it represents
				BATypeModeKey,	//**NEW IN TOUCHSCREEN** - L,R,S,P modes, requires data @ which mode it represents
				BATypeWindow,		//**NEW IN TOUCHSCREEN** - Indicates that this button leads to another window of buttons
				BATypeOEPEntry,	//**NEW IN TOUCHSCREEN** - Used to Make OEP Selections with Buttons
				BATypeWinDis,		//**NEW IN TOUCHSCREEN** - Used to Dismiss Windows
				BATypeHALOoverride,	/*Halo override key (SR 143 cwunn) */
				BATypeAtFor,			/*/For Key (SR 143 cwunn)     */
				BATypeManualValidation,  /* Manual Validation Key, SR 47 */
				BATypeShutDown,
				BATypeCharacter,	//Used to enter a Character (A,1,&, or character)
				BATypeCharacterDelete, //Used to delete a Character
				BATypeProg1,				//Program Mode Key - P1
				BATypeProg2,				//Program Mode Key - P2
				BATypeProg3,				//Program Mode Key - P3
				BATypeProg4,				//Program Mode Key - P4
				BATypeProg5,				//Program Mode Key - P5
				BATypeMinimize,		//Minimize the application
				BATypePLU_Group,		//PLU group for dynamic PLU functionality
				BATypeEditCondiment,     // Edit a Condiment, FSC_EDIT_CONDIMENT
				BATypeEditCondimentTbl,  // Edit a Condiment Table, FSC_EDIT_COND_TBL
				BATypeReceiptId,		   //Reciept ID, FSC_TENT
				BATypeOrderDeclaration,  //OrderDeclaration, FSC_ORDER_DEC
				BATypeAdjectiveMod,      // Adjective Modifier, specify an explicit Adjective for PLU, FSC_ADJ_MOD
				BATypeGiftCard,          // Gift card,  FSC_GIFT_CARD
				BATypeSuprIntrvn,        // queries for supervisor intervention supervisor code, FSC_SUPR_INTRVN
				BATypeDocumentLaunch,    // launch a document through shortcut in Database folder, FSC_DOC_LAUNCH
				BATypeOperatorPickup,     // Register Mode cash pickup like AC10, FSC_OPR_PICKUP
				BATypeDisplayLabeledWindow,		// Display a window whose label is embedded, FSC_WINDOW_DISPLAY
				BATypeDismissLabeledWindow,		// Dismiss a window whose label is embedded, FSC_WINDOW_DISMISS
				BATypeDisplayLabeledWindowManual,	// Display a window whose label is manually typed in, FSC_WINDOW_DISPLAY
				BATypeDismissLabeledWindowManual,	// Dismiss a window whose label is manually typed in, FSC_WINDOW_DISMISS
				BATypeAutoSignOut,                  // Perform a manual auto signout, FSC_AUTO_SIGN_OUT
				BATypePLU_GroupTable,				//PLU group for dynamic PLU functionality using multiple OEP tables
				BATypeNumTypeCaption                // use button caption with #/Type to allow #/Type strings without needing control string
	} ButtonActionType;

#define BUTTONACTIONTYPEGROUP_PRIORITY    0x0001    // flag in data.group_table.usOptionFlags indicating to use priority

	// WARNING: Be careful about changes to this struct to preserve backward compatibility!!!
	typedef 	struct   tagActionStruct {
		ButtonActionType  type;
		union tagActionUnion {
			UCHAR  PLU[15]; //PLU/Dept/Number data
			UINT   extFSC;	//Extended FSC data for one-touch entry keys, tenders, totals, tax mod, etc.
			UINT   nWinID;	//Control ID of the window to be opened
			bool AorB;		//A or B assignment for OpInt & Sign-in operation
			struct {
				USHORT  usGroupNumber;
				USHORT  usTableNumber;
				USHORT  usOptionFlags;
			} group_table;                // used with BATypePLU_GroupTable action type
		} data;
	} ButtonActionUnion;


	/*Structure containing all the attributes to be serialized for the WindowButton class
	any new attributes need to be added at the end of the struct so data from 
	old layout files does not get corrupted during serialization*/
	// WARNING: Be careful about changes to this struct to preserve backward compatibility!!!
	typedef struct _tagWinBtnAttributes{
		unsigned long signatureStart;
		ButtonShape m_myShape;							//Button shape - Rectangle, rounded rectangle, or elipse
		ButtonPattern m_myPattern;						//Button face pattern - None, horizontal stripes, or vertical stripes
		TCHAR myIcon[30];								//Button icon file nale
		ButtonActionUnion m_myActionUnion;				//type and action of button (FSC and extended FSC)
		CaptionAlignment capAlignment;					//alignment of button caption
		HorizontalIconAlignment horIconAlignment;	    //horizontal alignment of button icon	
		VerticalIconAlignment vertIconAlignment;        //vertical alignment of button icon
		BOOL HorizontalOriented;						//Caption Orientation - TRUE for horizontal, FALSE for Vertical

	}WinBtnAttributes;

	WinBtnAttributes btnAttributes;
	CString m_Icon;

	int nEndOfArchive;

;

// Operations
public:
	virtual void Serialize( CArchive& ar );

	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100);
	virtual BOOL WindowDisplay (CDC* pDC){return FALSE;}
	virtual BOOL WindowDestroy (void) { PostMessage (WM_QUIT, 0, 0); return TRUE;}
	virtual void ControlSerialize (CArchive &ar);

	static CRect getRectangle (int row, int column);
	static CRect getRectangleSized (int column, int row, USHORT usWidthMult, USHORT usHeightMult);

	static CWindowButton * TranslateCWindowControl (CWindowControl *pwc) {
		if (pwc && pwc->controlAttributes.m_nType == ButtonControl)
			return (static_cast <CWindowButton *> (pwc));
		else
			return NULL;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowButton)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindowButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWBUTTON_H__B4DD3905_DF5E_4D98_BA1E_C5FB5347A952__INCLUDED_)
