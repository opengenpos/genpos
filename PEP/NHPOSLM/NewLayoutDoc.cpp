// NewLayoutDoc.cpp : implementation of the CNewLayoutDoc class
//
#include "stdafx.h"
#include "NewLayout.h"
#include <afxdlgs.h>
#include <afx.h>
#include <time.h>
#include "NewLayoutDoc.h"
#include "P003.h"
#include "ChildFrm.h"
#include "DFileSummaryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutDoc

IMPLEMENT_DYNCREATE(CNewLayoutDoc, CDocument)

BEGIN_MESSAGE_MAP(CNewLayoutDoc, CDocument)
	//{{AFX_MSG_MAP(CNewLayoutDoc)
	ON_COMMAND(ID_FILE_FILESUMMARY, OnFileSummary)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_TOOLBAR_PROPERTIES, OnFileSummary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutDoc construction/destruction

CNewLayoutDoc::CNewLayoutDoc()
{
	// TODO: add one-time construction code here
	myWindowDocument.ClearDocument ();
	
}

CNewLayoutDoc::~CNewLayoutDoc()
{

}

BOOL CNewLayoutDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	myWindowDocument.InitializeDefaults();
	return TRUE;
}


	
/////////////////////////////////////////////////////////////////////////////
// CNewLayoutDoc serialization

void CNewLayoutDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		/*the following code searhces the documents view list for the CNewLayoutView
		and gets the active window ID so we can assign it to the documentAttributes 
		for serialization. We do this so when this layout is reopened, we have the same 
		window initially active that we had when the file was saved - 7/15/04*/
		CNewLayoutView * myView = NULL;
		POSITION pos = GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = GetNextView(pos);
			if( pView->IsKindOf(RUNTIME_CLASS(CNewLayoutView))){
				myView = (CNewLayoutView*)pView;
			}
		} 
		myWindowDocument.documentAttributes.uiInitialID = myView->uiActiveID;
	}
	else
	{
		// TODO: add loading code here
	}

	myWindowDocument.Serialize (ar);
	myWindowDocument.CheckPointWithNoChange();
}

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutDoc diagnostics

#ifdef _DEBUG
void CNewLayoutDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNewLayoutDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewLayoutDoc commands


void CNewLayoutDoc::OnFileExport()
{
	//Strings to hold data for file output
	CString csFilePath;
	CString header1;
	CString header2;
	CString header3;
	CString header4;
	CString output;
	CString delemeter = _T(",");

	UINT nFlags =  CFile::modeWrite | CFile::modeCreate;
	CString szFilter = _T("CSV (Comma Delemited (*.csv)|*.csv||");

	//initialize File Dialog so user can set the file path
	CFileDialog fd(FALSE,_T("csv"),_T("LOMExport"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);
	int status = fd.DoModal();
	if(status == 2){
		return;
	}
	else{
		csFilePath = fd.GetPathName();
	

		CStdioFile myFile(csFilePath,nFlags);
		myExportFile = &myFile;

		header1 = _T("\"BUTTON CAPTION\"");
		header2 = _T("\"FSC\"");
		header3 = _T("\"ExtFSC\"");
		header4 = _T("\"BUTTON ID\"");
		output.Format(_T("%s%s%s%s%s%s%s%s\n"),
						header4,
						delemeter,
						header1,
						delemeter,
						header2,
						delemeter,
						header3,
						delemeter);
		myExportFile->WriteString(output);

		//cycle through all the documents windows
		POSITION currentPos = myWindowDocument.listControls.GetHeadPosition ();
		POSITION  currentPosLast = currentPos;
		while (currentPos) {
			//get the current window in the list
			CWindowControl *pwc = myWindowDocument.listControls.GetNext (currentPos);
			CWindowItemExt *pwi = CWindowItemExt::TranslateCWindowControl(pwc);

			CWindowControl *bi =0;
			int cnt = 0;
			POSITION pos = pwi->ButtonItemList.GetHeadPosition ();
			//while there are still controls left in the windows control list
			while (pos) {
				cnt++;
				//get the next control in the list and asign to bi
				bi = pwi->ButtonItemList.GetNext (pos);
				
				if(bi->controlAttributes.m_nType == CWindowControl::ButtonControl ){
					WriteExportData(bi);
				}
					
				//control is a window
				if(bi->controlAttributes.m_nType == CWindowControl::WindowContainer){
					ExpSubControl(CWindowItemExt::TranslateCWindowControl(bi));
				}
				//bi = pwi->GetDataNext();

			}
			
		}
		myExportFile->Close();
	}

}





BOOL CNewLayoutDoc::WriteExportData(CWindowControl *wc)
{

	//Strings to hold data for file output
	CString delemeter(_T(","));
	CString message1(wc->myCaption);
	CString message2;
	CString message3;
	CString message4;
	CString output;
	
	//load data into strings
	message1.Format(_T("\"%s\""),wc->myCaption);
	message2.Format(_T("%d"),CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.type);
	if(wc->controlAttributes.m_myAction == 20401){
		CString temp;
		temp = CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.data.PLU;
		message3.Format(_T("%s"),temp);
	}
	else{
		message3.Format(_T("%d"),CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.data.extFSC);
	}
	message4.Format(_T("%d"),CWindowButton::TranslateCWindowControl(wc)->controlAttributes.m_myId);
	output.Format(_T("%s%s%s%s%s%s%s\n"),
					message4,
					delemeter,
					message1,
					delemeter,
					message2,
					delemeter,
					message3
					/*delemeter*/);
	
	//write the data
	myExportFile->WriteString(output);
	myExportFile->Flush();

	return TRUE;
}

BOOL CNewLayoutDoc::ExpSubControl(CWindowItem *wi)
{

	CWindowControl *pwc = 0;

	pwc = wi->GetDataFirst();

	while (pwc) {
		CRect kk;
		if(pwc->controlAttributes.m_nType == CWindowControl::ButtonControl){
			WriteExportData(pwc);
		}

		if(pwc->controlAttributes.m_nType == CWindowControl::WindowContainer){
			ExpSubControl(CWindowItemExt::TranslateCWindowControl(pwc));
		}

		
		pwc = wi->GetDataNext ();
		
	}
	return TRUE;
}

void CNewLayoutDoc::OnFileImport()
{
	//AfxMessageBox(_T("Close your Excel file before continuing"),MB_OK,0);
	//Strings to hold data for file input
	CString csFilePath;
	CString csCaption;
	CString csFSC;
	CString csExtFSC;
	CString csID;
	CString input;
	CString filedata;
	CString csComma = _T(",");
	CString	csQuote = _T('"');
	CString token;


	UINT nFlags =  CFile::modeRead;
	CString szFilter = _T("CSV (Comma Delemited (*.csv)|*.csv||");

	//initialize File Dialog so user can get the import file
	CFileDialog fd(TRUE,_T("csv"),_T("LOMExport"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);
	int status = fd.DoModal();
	if(status == 2){
		return;
	}
	else{
		csFilePath = fd.GetPathName();
	

		CStdioFile myImportFile;

		if (_taccess(csFilePath, 0) != 0) {

			AfxMessageBox(_T("File not found."));
			return;
		}

		myImportFile.Open(csFilePath,
			 CFile::modeRead | CFile::shareExclusive |
			 CFile::typeText);

		while( myImportFile.ReadString(filedata))
		{
			
			filedata += _T(",");
			input += filedata;
			
		}
	
		myImportFile.Close();

		CWindowControl * wc = 0;
		TCHAR tempStringConverter[sizeof(CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.data.PLU)];
		/*
			An error will occur while doing an lstrcpy with a fixed buffer size when the length
			of the string has not been determined. The function lstrcpy will copy over characters
			until it reaches a null character. Since the size has not been determined prior to
			doing the series:

				TCHAR buff[256];
				lstrcpy(buff,input);

				token = wcstok( buff, delemeter );	

			lstrcpy will write over the stack when inputs' length is greater than 255 (one space for
			null terminator).
		*/
		int cnt = 1;
		int nPos;

		nPos = input.Find(csComma);
		token = input.Left(nPos);
		input.TrimRight(csComma);
		input.Delete(0,nPos);
		input.TrimLeft(csComma);
		input.TrimLeft();

		while( token.GetLength())
		{
			// While there are tokens in "string" 
			if(token.Compare(_T("\"BUTTON CAPTION\""))!= 0 
				&& token.Compare(_T("\"FSC\"")) !=0 
				&& token.Compare(_T("\"ExtFSC\""))!= 0
				&& token.Compare(_T("\"BUTTON ID\""))!= 0){

				//Remove Quotes from String prior to setting
				token.TrimLeft(csQuote);
				token.TrimRight(csQuote);
			
				if(cnt == 1){
					wc = FindImportControl(_wtoi(token));
				}
					if(wc){
						
						if(cnt == 2){
							wc->myCaption = token;
						}
						if(cnt == 3){
							CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.type = (CWindowButton::ButtonActionType)_wtoi(token);
							
						}
						if(cnt == 4){
							if(CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.type == CWindowButton::BATypePLU ||
								CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.type == CWindowButton::BATypeDept){
								
								//Need to convert from possible Unicode to ANSI character set as the PLU is just digits
								//copy the CString to TCHAR array then assign the elemnts of the THCAR array to the elements of the PLU UCHAR array
								_tcsncpy_s(tempStringConverter, token, sizeof(CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.data.PLU));
								tempStringConverter[sizeof(tempStringConverter) - 1] = 0;
								for(int i = 0; i < sizeof(CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.data.PLU);i++){
									CWindowButton * mb = CWindowButton::TranslateCWindowControl(wc);
									mb->btnAttributes.m_myActionUnion.data.PLU[i] = (UCHAR)tempStringConverter[i];

									if(tempStringConverter[i] == 0)
										break;
								}
								
							}
							else{
								CWindowButton::TranslateCWindowControl(wc)->btnAttributes.m_myActionUnion.data.extFSC = _wtoi(token);
							}
							UpdateButtonAction(CWindowButtonExt::TranslateCWindowControl(wc));
						}
					}
				
				
			}
			//else
			// Get next token: 			
			//token = wcstok( NULL, delemeter );
			// If the comma can not be found you are at the end of your
			// read string (return of find will result in a -1 return) 
			// and the very last value should be applied to your 
			// token variable and the input string should be cleared.
			nPos = input.Find(csComma);

			if( nPos == -1 ) {
				token = input;
				input = _T("");
			}else{
				token = input.Left(nPos);
			}

			input.TrimRight(csComma);
			input.Delete(0,nPos);
			input.TrimLeft(csComma);
			input.TrimLeft();

			if(cnt != 4)
				cnt++;
			else{
				cnt = 1;
				wc = 0;
			}
	   }
		UpdateAllViews(NULL);
	}


}
//UPDATE TO SEARCH FOR SUBWINDOWS
CWindowControl* CNewLayoutDoc::FindImportControl(UINT id)
{
	int nfReturn = 0;
	//create a NULL WindowControl
	CWindowControl *bi = 0;

	POSITION pos = myWindowDocument.listControls.GetHeadPosition ();
	//while there are still controls left in the documetns window list
	while (pos) {
		
		//get the next window item in the list and asign to bi
		bi = myWindowDocument.listControls.GetNext (pos);
		
		CWindowItemExt *pw = CWindowItemExt::TranslateCWindowControl (bi);

		//get the first control in the windows control list
		CWindowControl *pwc = pw->GetDataFirst ();
		//while there are still controls in teh windows list
		while (pwc) {
			if(pwc->controlAttributes.m_myId == id){
				return pwc;
			}
			//user didnt click in this control, get the next contol in the windows list
			pwc = pw->GetDataNext ();
		}
	}
	//no item found
	return NULL;
}


BOOL CNewLayoutDoc::UpdateButtonAction(CWindowButtonExt *bi)
{
	CWindowButton::ButtonActionType type;
	BOOL bReturn = TRUE; 

	type = bi->btnAttributes.m_myActionUnion.type;
	switch(type){
	case CWindowButton::BATypePLU:
		bi->controlAttributes.m_myAction = IDD_P03_KEYEDPLU;
		break;

	case CWindowButton::BATypeDept:
		bi->controlAttributes.m_myAction = IDD_P03_KEYEDDPT;
		break;

	case CWindowButton::BATypePrintMod:
		bi->controlAttributes.m_myAction = IDD_P03_PRTMOD;
		break;

	case CWindowButton::BATypeAdj:
		bi->controlAttributes.m_myAction = IDD_P03_ADJ;
		break;

	case CWindowButton::BATypeTender:
		bi->controlAttributes.m_myAction = IDD_P03_TENDER;
		break;

	case CWindowButton::BATypeForeignTender:
		bi->controlAttributes.m_myAction = IDD_P03_FC;
		break;

	case CWindowButton::BATypeTotal:
		bi->controlAttributes.m_myAction = IDD_P03_TTL;
		break;

	case CWindowButton::BATypeAddCheck:
		bi->controlAttributes.m_myAction = IDD_P03_ADDCHK;
		break;

	case CWindowButton::BATypeItemDisc:
		bi->controlAttributes.m_myAction = IDD_P03_IDISC;
		break;

	case CWindowButton::BATypeRegDisc:
		bi->controlAttributes.m_myAction = IDD_P03_RDISC;
		break;

	case CWindowButton::BATypeTaxMod:
		bi->controlAttributes.m_myAction = IDD_P03_TAXMOD;
		break;

	case CWindowButton::BATypeSignIn:
		if(bi->btnAttributes.m_myActionUnion.data.AorB == 0){
			bi->controlAttributes.m_myAction = IDD_P03_AKEY;
		}
		else{
			bi->controlAttributes.m_myAction = IDD_P03_BKEY;
		}
		break;

	case CWindowButton::BATypePrtDemand:
		bi->controlAttributes.m_myAction = IDD_P03_PRTDEM;
		break;

	case CWindowButton::BATypeChrgTips:
		bi->controlAttributes.m_myAction = IDD_P03_CHARTIP;
		break;

	case CWindowButton::BATypePreCash:
		bi->controlAttributes.m_myAction = IDD_P03_PRECASH;
		break;

	case CWindowButton::BATypeDrctShift:
		bi->controlAttributes.m_myAction = IDD_P03_DRCTSHFT;
		break;

	case CWindowButton::BATypeCoupon:
		bi->controlAttributes.m_myAction = IDD_P03_KEYEDCPN;
		break;

	case CWindowButton::BATypeString:
		bi->controlAttributes.m_myAction = IDD_P03_KEYEDSTR;
		break;

	case CWindowButton::BATypeSeatNum:
		bi->controlAttributes.m_myAction = IDD_P03_SEAT;
		break;

	case CWindowButton::BATypeAsk:
		bi->controlAttributes.m_myAction = IDD_P03_ASK;
		break;

	case CWindowButton::BATypeLevelSet:
		bi->controlAttributes.m_myAction = IDD_P03_ADJPLS;
		break;

	case CWindowButton::BATypeWindow:
		bi->controlAttributes.m_myAction = IDD_P03_MENUWIND;
		break;

	case CWindowButton::BATypeModeKey:
		bi->controlAttributes.m_myAction = IDD_P03_MENUMODEKEY;
		break;

	case CWindowButton::BATypeNumPad:
		bi->controlAttributes.m_myAction = IDD_P03_NUMBERPAD;
		break;
		
	case CWindowButton::BATypeClear:
		bi->controlAttributes.m_myAction = IDD_P03_CLEAR;
		break;

	case CWindowButton::BATypeGCNum:
		bi->controlAttributes.m_myAction = IDD_P03_GCNO;
		break;

	case CWindowButton::BATypeACKey:
		bi->controlAttributes.m_myAction = IDD_P03_AC;
		break;

	case CWindowButton::BATypeAlpha:
		bi->controlAttributes.m_myAction = IDD_P03_ALPHA;
		break;

	case CWindowButton::BATypeCancel:
		bi->controlAttributes.m_myAction = IDD_P03_CANCEL;
		break;

	case CWindowButton::BATypeChgCmpt:
		bi->controlAttributes.m_myAction = IDD_P03_CHGCOMP;
		break;

	case CWindowButton::BATypeCouponNum:
		bi->controlAttributes.m_myAction = IDD_P03_CPNNO;
		break;

	case CWindowButton::BATypeCursorVoid:
		bi->controlAttributes.m_myAction = IDD_P03_CURSORVOID;
		break;

	case CWindowButton::BATypeDecTips:
		bi->controlAttributes.m_myAction = IDD_P03_DECTIP;
		break;

	case CWindowButton::BATypeDecimal:
		bi->controlAttributes.m_myAction = IDD_P03_DECPT;
		break;

	case CWindowButton::BATypeDeptNum:
		bi->controlAttributes.m_myAction = IDD_P03_DEPTNO;
		break;

	case CWindowButton::BATypeErCorrect:
		bi->controlAttributes.m_myAction = IDD_P03_EC;
		break;

	case CWindowButton::BATypeFeed:
		bi->controlAttributes.m_myAction = IDD_P03_FEED;
		break;

	case CWindowButton::BATypeFSMod:
		bi->controlAttributes.m_myAction = IDD_P03_FOODSTAMPMOD;
		break;

	case CWindowButton::BATypeGCTrans:
		bi->controlAttributes.m_myAction = IDD_P03_GCTRANS;
		break;

	case CWindowButton::BATypeItemTrans:
		bi->controlAttributes.m_myAction = IDD_P03_ITEMTRANS;
		break;

	case CWindowButton::BATypeJFeed:
		bi->controlAttributes.m_myAction = IDD_P03_JOUFEED;
		break;

	case CWindowButton::BATypeScrollLeft:
		bi->controlAttributes.m_myAction = IDD_P03_LEFT;
		break;

	case CWindowButton::BATypeLeftDisp:
		bi->controlAttributes.m_myAction = IDD_P03_LEFTDISPLAY;
		break;

	case CWindowButton::BATypeLineNum:
		bi->controlAttributes.m_myAction = IDD_P03_LINENO;
		break;

	case CWindowButton::BATypeMoney:
		bi->controlAttributes.m_myAction = IDD_P03_MONEYDEC;
		break;

	case CWindowButton::BATypeNewCheck:
		bi->controlAttributes.m_myAction = IDD_P03_NEWCHK;
		break;

	case CWindowButton::BATypeNoSale:
		bi->controlAttributes.m_myAction = IDD_P03_NOSALE;
		break;

	case CWindowButton::BATypeOffTend:
		bi->controlAttributes.m_myAction = IDD_P03_OFFTEND;
		break;

	case CWindowButton::BATypeOprInt:
		if (bi->btnAttributes.m_myActionUnion.data.AorB == 0)
			bi->controlAttributes.m_myAction = IDD_P03_OPRINT_A;
		else
			bi->controlAttributes.m_myAction = IDD_P03_OPRINT_B;
		break;

	case CWindowButton::BATypePaidOrder:
		bi->controlAttributes.m_myAction = IDD_P03_PAID_REC;
		break;

	case CWindowButton::BATypePreBal:
		bi->controlAttributes.m_myAction = IDD_P03_PB;
		break;

	case CWindowButton::BATypePLUNum:
		bi->controlAttributes.m_myAction = IDD_P03_PLUNO;
		break;

	case CWindowButton::BATypePO:
		bi->controlAttributes.m_myAction = IDD_P03_PO;
		break;

	case CWindowButton::BATypePriceCheck:
		bi->controlAttributes.m_myAction = IDD_P03_PRICECHECK;
		break;

	case CWindowButton::BATypePriceEnt:
		bi->controlAttributes.m_myAction = IDD_P03_PRICEENT;
		break;

	case CWindowButton::BATypeQty:
		bi->controlAttributes.m_myAction = IDD_P03_QTY;
		break;

	case CWindowButton::BATypeReceipt:
		bi->controlAttributes.m_myAction = IDD_P03_RCTNORCT ;
		break;

	case CWindowButton::BATypeRFeed:
		bi->controlAttributes.m_myAction = IDD_P03_RECFEED;
		break;

	case CWindowButton::BATypeRepeat:
		bi->controlAttributes.m_myAction = IDD_P03_REPEAT;
		break;

	case CWindowButton::BATypeRevPrint:
		bi->controlAttributes.m_myAction = IDD_P03_RVSPRI;
		break;

	case CWindowButton::BATypeScrollRight:
		bi->controlAttributes.m_myAction = IDD_P03_RIGHT;
		break;

	case CWindowButton::BATypeROA:
		bi->controlAttributes.m_myAction = IDD_P03_ROA;
		break;

	case CWindowButton::BATypeScaleTare:
		bi->controlAttributes.m_myAction = IDD_P03_SCALE;
		break;

	case CWindowButton::BATypeScrollDown:
		bi->controlAttributes.m_myAction = IDD_P03_DOWN;
		break;

	case CWindowButton::BATypeScrollUp:
		bi->controlAttributes.m_myAction = IDD_P03_UP;
		break;

	case CWindowButton::BATypeShift:
		bi->controlAttributes.m_myAction = IDD_P03_SHIFT;
		break;

	case CWindowButton::BATypeSplit:
		bi->controlAttributes.m_myAction = IDD_P03_SPLIT;
		break;

	case CWindowButton::BATypeStringNum:
		bi->controlAttributes.m_myAction = IDD_P03_STRNO;
		break;

	case CWindowButton::BATypeSurrogateNum:
		bi->controlAttributes.m_myAction = IDD_P03_BARTENDER;
		break;

	case CWindowButton::BATypeTableNum:
		bi->controlAttributes.m_myAction = IDD_P03_TBLNO;
		break;

	case CWindowButton::BATypeTimeIn:
		bi->controlAttributes.m_myAction = IDD_P03_TIMEIN;
		break;

	case CWindowButton::BATypeTimeOut:
		bi->controlAttributes.m_myAction = IDD_P03_TIMEOUT;
		break;

	case CWindowButton::BATypeTipPO:
		bi->controlAttributes.m_myAction = IDD_P03_TIPSPO;
		break;

	case CWindowButton::BATypeTranVoid:
		bi->controlAttributes.m_myAction = IDD_P03_VTRAN;
		break;

	case CWindowButton::BATypeUPCE:
		bi->controlAttributes.m_myAction = IDD_P03_UPCEVER;
		break;

	case CWindowButton::BATypeVoid:
		bi->controlAttributes.m_myAction = IDD_P03_VOID;
		break;

	case CWindowButton::BATypeWait:
		bi->controlAttributes.m_myAction = IDD_P03_WAIT;
		break;

	case CWindowButton::BATypeWicTrans:
		bi->controlAttributes.m_myAction = IDD_P03_WICTRANS;
		break;

	case CWindowButton::BATypeWicMod:
		bi->controlAttributes.m_myAction = IDD_P03_WICMOD;
		break;

	case CWindowButton::BATypeDblZero:
		bi->controlAttributes.m_myAction = IDD_P03_00KEY;
		break;

	case CWindowButton::BATypeNumPerson:
		bi->controlAttributes.m_myAction = IDD_P03_NOPERSON;
		break;

	case CWindowButton::BATypeNumType:
		bi->controlAttributes.m_myAction = IDD_P03_NOTYPE;
		break;

	case CWindowButton::BATypeNumTypeCaption:
		bi->controlAttributes.m_myAction = IDD_P03_NOTYPE;
		break;

	case CWindowButton::BATypeWinDis:
		bi->controlAttributes.m_myAction = IDD_P03_MENUWINDREMOVE;
		break;

	case CWindowButton::BATypeCharacter:
		bi->controlAttributes.m_myAction = IDD_P03_CHARACTER_BUTTON;
		break;

	case CWindowButton::BATypeCharacterDelete:
		bi->controlAttributes.m_myAction = IDD_P03_CHARACTER_DELETE;
		break;

	case CWindowButton::BATypeProg1:
		bi->controlAttributes.m_myAction = IDD_P03_P1;
		break;

	case CWindowButton::BATypeProg2:
		bi->controlAttributes.m_myAction = IDD_P03_P2;
		break;

	case CWindowButton::BATypeProg3:
		bi->controlAttributes.m_myAction = IDD_P03_P3;
		break;

	case CWindowButton::BATypeProg4:
		bi->controlAttributes.m_myAction = IDD_P03_P4;
		break;

	case CWindowButton::BATypeProg5:
		bi->controlAttributes.m_myAction = IDD_P03_P5;
		break;

	default:
		bReturn = FALSE;
	}

	return bReturn;

}

void CNewLayoutDoc::OnFileSummary() 
{
	CDFileSummaryDlg *fsDlg = new CDFileSummaryDlg();

	//set the dialogs CWindowDocument member for access to file summary info
	fsDlg->pDoc = &myWindowDocument;

	//initialize dialog data with the Documents file summary info
	fsDlg->m_nMjVersion = myWindowDocument.documentAttributes.nVersion[0];
	fsDlg->m_nMnVersion = myWindowDocument.documentAttributes.nVersion[1];
	fsDlg->m_nRelease = myWindowDocument.documentAttributes.nVersion[2];
	fsDlg->m_editSummary = myWindowDocument.csSummary;
	fsDlg->m_csPEPFile = myWindowDocument.csPEP;
	fsDlg->m_csIconDir = myWindowDocument.csIconDir;

	//set variable for displaying files title in dialogs title bar
	fsDlg->csDialogTitle = m_strTitle;

	if(fsDlg->DoModal()){
		//set the documents file summary info with the edited info
		myWindowDocument.documentAttributes.nVersion[0] = fsDlg->m_nMjVersion;
		myWindowDocument.documentAttributes.nVersion[1] = fsDlg->m_nMnVersion;
		myWindowDocument.documentAttributes.nVersion[2] = fsDlg->m_nRelease;
		myWindowDocument.csSummary = fsDlg->m_editSummary;
		myWindowDocument.csPEP = fsDlg->m_csPEPFile;
		myWindowDocument.csIconDir = fsDlg->m_csIconDir;
	}
}

BOOL CNewLayoutDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	SYSTEMTIME lastMod;
	int modIndex = 0;
	
	//get system time
	time_t t_time;
	time(&t_time);

	/*create an CTime object with the recieved system time 
	and convert it to SYSTEMTIME*/
	CTime t_now(t_time);
	t_now.GetAsSystemTime(lastMod);

	/*loop through our array of times modified and set modIndex 
	variable to the 1st index with no entry - using the year part
	of the structure to determine if that particular index has saved data*/
	for( int x = 0; x < 5; x++) {
		if(myWindowDocument.documentAttributes.lastModified[x].wYear){
			modIndex = x + 1;
		}
	}

	//if all of the indexes of the SYSTEMTIME array have entrys, delete the oldes entry
	if(modIndex == 5){
		modIndex--;
		memset(&myWindowDocument.documentAttributes.lastModified[modIndex],
			   0,
			   sizeof(myWindowDocument.documentAttributes.lastModified[modIndex]));
	}
	/*loop through the entries and shift all of them down one index
	so we can enter the time of this save in the first index*/
	for( ;modIndex > 0; modIndex-- ) {
		myWindowDocument.documentAttributes.lastModified[modIndex] = 
			myWindowDocument.documentAttributes.lastModified[modIndex - 1];
	}



	//Version Number Information - CSMALL
	//	Saving Layout Manager version to to File Properties - CSMALL 5/30/06
		DWORD dwHandle;
		DWORD dwLen = 0;
		unsigned int puLen;
		TCHAR	pInfoBuf[1024];
		TCHAR majorVersionNum[10];
		TCHAR minorVersionNum[10];
		TCHAR releaseVersionNum[10];

		VS_FIXEDFILEINFO *lpBuf;

		TCHAR *myFileName = _T("C:\\FlashDisk\\NCR\\Saratoga\\Program\\NHPOSLM.exe");
		memset(majorVersionNum, 0x00, sizeof(majorVersionNum));
		memset(minorVersionNum, 0x00, sizeof(minorVersionNum));
		memset(releaseVersionNum, 0x00, sizeof(releaseVersionNum));

		dwLen = GetFileVersionInfoSize(myFileName, &dwHandle);
		GetFileVersionInfo(myFileName,dwHandle,sizeof(pInfoBuf), pInfoBuf);

		if( dwLen != 0){
			VerQueryValue(pInfoBuf,_T("\\"),(void **)&lpBuf, &puLen);

			_stprintf_s(majorVersionNum, _T("%2.2d"), HIWORD(lpBuf->dwFileVersionMS));
			_stprintf_s(minorVersionNum, _T("%2.2d"), LOWORD(lpBuf->dwFileVersionMS));
			_stprintf_s(releaseVersionNum, _T("%2.2d"), HIWORD(lpBuf->dwFileVersionLS));

			myWindowDocument.documentAttributes.nVersion[0] = _ttoi(majorVersionNum);
			myWindowDocument.documentAttributes.nVersion[1] = _ttoi(minorVersionNum);
			myWindowDocument.documentAttributes.nVersion[2] = _ttoi(releaseVersionNum);
		}
		else{
			myWindowDocument.documentAttributes.nVersion[0] = 0;
			myWindowDocument.documentAttributes.nVersion[1] = 0;
			myWindowDocument.documentAttributes.nVersion[2] = 0;
		}


	//enter the time of this save in the first index
	myWindowDocument.documentAttributes.lastModified[modIndex] = lastMod;
	myWindowDocument.bModified = 0;
	return CDocument::OnSaveDocument(lpszPathName);
}



BOOL CNewLayoutDoc::SaveModified() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bModified = myWindowDocument.bModified;
	return CDocument::SaveModified();

}

