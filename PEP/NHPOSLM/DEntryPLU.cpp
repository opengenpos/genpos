// DEntryPLU.cpp : implementation file
//

#include "stdafx.h"
#include "NewLayout.h"
#include "DEntryPLU.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDEntryPLU dialog


CDEntryPLU::CDEntryPLU(CWnd* pParent /*=NULL*/)
	: CDialog(CDEntryPLU::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDEntryPLU)
	m_csExtFsc = _T("");
	m_csLabelType = _T("");
	m_iExtFSC = 1;
	//}}AFX_DATA_INIT
}


void CDEntryPLU::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDEntryPLU)
	DDV_MaxChars(pDX, m_csExtFsc, MaxSizeOfPLU);
	DDX_Text(pDX, IDD_P03_EXTEND_MSG, m_csExtMsg);
	DDX_Text(pDX, IDD_P03_EXTMNEMO, m_csExtMnemonics);
	DDX_Text(pDX, IDD_P03_EXTFSC, m_iExtFSC);
	DDX_Text(pDX, IDD_P03_LABEL, m_csLabelType);
	//}}AFX_DATA_MAP
	

	if (pDX->m_bSaveAndValidate) {
		DDX_Text(pDX, IDD_P03_EXTFSC, m_csExtFsc);        // get extended value from control

		// clear the data area used for the PLU number then copy the
		// PLU number from the dialog control into our data area.
		memset(myActionUnion.data.PLU, 0, sizeof(myActionUnion.data.PLU));
		LPTSTR pExtFsc = m_csExtFsc.LockBuffer ();
		ASSERT(pExtFsc);
		for (int i = 0; *pExtFsc && i < sizeof(myActionUnion.data.PLU); i++, pExtFsc++) {
			myActionUnion.data.PLU[i] = (UCHAR) *pExtFsc;
		}
		m_csExtFsc.ReleaseBuffer ();
	}
	else {
		m_csExtFsc.Format (_T("%S"), myActionUnion.data.PLU);
		//set the PLU edit control with previous PLU text if it exists
		DDX_Text(pDX, IDD_P03_EXTFSC, m_csExtFsc);      // set the control with the extended value.
	}
}


BEGIN_MESSAGE_MAP(CDEntryPLU, CDialog)
	//{{AFX_MSG_MAP(CDEntryPLU)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDEntryPLU message handlers

BOOL CDEntryPLU::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	//Set the Label Type to First Index
	((CComboBox *)GetDlgItem (IDD_P03_LABEL))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDEntryPLU::OnOK() 
{
	WCHAR   uchCd;
	CString tempPLUString;
	WCHAR tempPLUArray[MaxSizeOfPLU+1];
	int tempPLULength;
	BOOL bValidPLU = FALSE;
	BOOL bModifiedPLU = FALSE;

	//populate array
	for(int i = 0; i < MaxSizeOfPLU; i++){
		wcscpy(&tempPLUArray[i], WIDE("0"));
	}

	//get the PLU text from the dialog
	GetDlgItem(IDD_P03_EXTFSC)->GetWindowText(tempPLUString);

	//retrieve the length of the PLU entered
	tempPLULength = tempPLUString.GetLength();

	//copy the PLU string into the array
	_tcscpy(&tempPLUArray[MaxSizeOfPLU-tempPLULength], tempPLUString);

	//calculate the check digit based upon the PLU entered
    uchCd = CheckDigitCalc(tempPLUArray);
	
	//do different operations based upon the length of the PLU entered
	switch (tempPLULength){
		case 14:
			//verify that the last digit matches the correct check digit
			if (uchCd != tempPLUArray[MaxSizeOfPLU-1])
			{
				//display error message
				AfxMessageBox(IDS_ERR_PLUNO, MB_OK,0);
				break;
			}
			//else it is a valid PLU, proceed to close dialog
			bValidPLU = TRUE;
			break;
		case 12:
			//move digits over to the left by 1
			memmove(&tempPLUArray[0], &tempPLUArray[1], (MaxSizeOfPLU - 1) * sizeof(WCHAR));
			
			//calculate the check digit
			uchCd = CheckDigitCalc(tempPLUArray);
			
			//store the check digit into the last position on the array
			tempPLUArray[MaxSizeOfPLU-1] = uchCd;
			
		case 13:
			//verify that the last digit matches the correct check digit
			if (uchCd != tempPLUArray[MaxSizeOfPLU-1])
			{
				//display error message
				AfxMessageBox(IDS_ERR_PLUNO, MB_OK,0);
				break;
			}
		
			//copy the PLU array into the tempPLUString
			tempPLUString = tempPLUArray;

			//Set the window text to display the correct PLU
			GetDlgItem(IDD_P03_EXTFSC)->SetWindowText(tempPLUString);

			//proceed to close dialog
			bValidPLU = TRUE;
			break;
		case 10:
			if (tempPLUArray[4] == '0')
			{	
				//display error message
				AfxMessageBox(IDS_ERR_PLUNO, MB_OK,0);
				break;
			}
			//no break because we want to execute the code below
		case 11:
			//move digits over to the left by 1
			memmove(&tempPLUArray[2], &tempPLUArray[3], 11 * sizeof(WCHAR));
		
			//calculate the check digit
			uchCd = CheckDigitCalc(tempPLUArray);
			
			//store the cehck digit into the last position on the array
			tempPLUArray[MaxSizeOfPLU-1] = uchCd;

			//copy the PLU array into the tempPLUString
			tempPLUString = tempPLUArray;

			//Set the window text to display the correct PLU
			GetDlgItem(IDD_P03_EXTFSC)->SetWindowText(tempPLUString);

			//proceed to close dialog
			bValidPLU = TRUE;
			break;
		case 9:
			//nine digit PLU's are not allowed, display error
			AfxMessageBox(IDS_ERR_PLUNO, MB_OK,0);
			break;
		case 7:
			//move digits over to the left by 1
			memmove(&tempPLUArray[0], &tempPLUArray[1], (MaxSizeOfPLU - 1) * sizeof(WCHAR));

			//calculate the check digit
			uchCd = CheckDigitCalc(tempPLUArray);
			
			//store the cehck digit into the last position on the array
			tempPLUArray[MaxSizeOfPLU-1] = uchCd;

			//copy the PLU array into the tempPLUString
			tempPLUString = tempPLUArray;

			//Set the window text to display the correct PLU
			GetDlgItem(IDD_P03_EXTFSC)->SetWindowText(tempPLUString);

			//no break because we want to execute the code below
		case 8: //no break
		default:
			//verify check digit matches digit in last position in the array
			if (uchCd != tempPLUArray[MaxSizeOfPLU-1])
			{
				//display error message
				AfxMessageBox(IDS_ERR_PLUNO, MB_OK,0);
				break;
			}
			
			//proceed to close dialog
			bValidPLU = TRUE;
			break;

		//for lengths under 7, we do not check for validity. These PLU's can be 
		//any combination the user wants them to be
		case 6://no break
		case 5://no break
		case 4://no break
		case 3://no break
		case 2://no break
		case 1://no break
			//if PLU is equal to 0, display error message
			if (tempPLUString == "0")
			{
				//display error message
				AfxMessageBox(IDS_ERR_PLUNO, MB_OK,0);
				break;
			}

			//proceed to close dialog
			bValidPLU = TRUE;
			break;
	}
	
	//if PLU entered is valid, close DEntryPLU dialog
	if (bValidPLU)
	{
		CDialog::OnOK();	
	}
}

/*
*******************************************************************
*
**    Synopsis : WCHAR CDEntryPLU::CheckDigitCalc(WCHAR *PluNo)
*
*     input    : pluno   : PluNo
*
**    Return   : ascii check digit
*
**    Description :  This function Calculates check digit.
*
**    Generate PLU Check Digit
*        0  1  2  3  4  5  6  7  8  9 10 11     (array)
*      +-----------------------------------+
*      |  |  |  |  |  |  |  |  |  |  |  |  |    (id ; ascii data)
*      +-----------------------------------+
*        1  3  1  3  1  3  1  3  1  3  1  3     (weight)
*       Modulus; 10
*******************************************************************
*/
WCHAR CDEntryPLU::CheckDigitCalc(WCHAR *PluNo)
{
    SHORT    i,sEven,sOdd,cdigit;

    for (sEven = sOdd = i = 0 ; i < MaxSizeOfPLU-1 ; ++i) {
        if (i % 2) {
            sEven += (SHORT)(PluNo[i]&0x0f);        /* even addr, Saratoga */
        } else {
            sOdd  += (SHORT)(PluNo[i]&0x0f);        /* odd addr, Saratoga */
        }
    }
    cdigit = (SHORT)((sEven + sOdd*3) % 10);        /* Saratoga */
    cdigit = (SHORT)((10 - cdigit) % 10);           /* Saratoga */
    return (WCHAR)(cdigit | 0x30);
}