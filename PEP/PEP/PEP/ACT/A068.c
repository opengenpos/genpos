/*
* ---------------------------------------------------------------------------
* Title         :   PLU Maintenance (AC 63, 64, 68, 82)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A068.C
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Dec-21-93 : 00.00.02 : T.Yamamoto : Prohibit PLU No. 0 entry
* Apr-03-95 : 03.00.00 : H.Ishida   : Add print Table No., Group No.and Print
*           :          :            : Priority by A/C 68
* Jan-31-96 : 03.00.05 : M.Suzuki   : Add R3.1
* Sep-11-98 : 03.03.00 : A.Mitsui   : V3.3
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Include File Declarations
* ===========================================================================
*/
#include    <Windows.h>
#include    <string.h>
#include    <stdio.h>
#include    <stdlib.h>

#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csop.h>
#include    <rfl.h>           /* Saratoga */
#include    <plu.h>           /* Saratoga */

#include    "action.h"
#include    "pepcomm.h"
#include    "pepflag.h"
#include    "file.h"
#include    "a068.h"
#include    "a111.h"
#include    "a114.h"
#include    "a116.h"
#include    "a160.h"
#include    "a161.h"
#include    "a170.h"
#include    "a071.h"

/*
* ===========================================================================
*       Compiler Message Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Public Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Static Work Area Declarations
* ===========================================================================
*/
static  LPA68DESC   lpPluDesc;  /* Address of Control Code String Buffer    */
static  UINT        /*unPLUMax,*/   /* Max. Record No. of PLU        (Prog. #2) */
                    unDeptMax;  /* Max. Record No. of Department (Prog. #2) */
                    /*unCurPlu;*/   /* Current Record No. of PLU                */
static  WORD        wDeptMax;   /* Current Max. No. of Department Records   */
static  PLUIF       tempPlu;    /* Temporary PLU Data Structure             */
static  UCHAR       uchTaxMode; /* Current Tax Mode (0:U.S. 1:Canada 2:VAT) */
static  BOOL        fDelConf = A68_DELCONF;
                                /* Delete Confirmation Flag                 */
static  BOOL        fVer;       /* Nomal Version or E-Version flag          */

static  ULONG       ulPLUMax;   /* Max. Record No. of PLU        (Prog. #2) */
static  ULONG       ulCurPlu;   /* Current Record No. of PLU                */
static  WCHAR       workPlu[OP_PLU_LEN+1];

static USHORT	usPPIMax; // added for SR 1169. CSMALL 11/27/2006
/*
* ===========================================================================
*       Code Area Declarations
* ===========================================================================
*/

/*
 *  The following function, A068GeneratePluFile() is used to generate a large number of
 *  PLU records when doing testing of various kinds. Normally this function is not used
 *  and it should not be part of the standard release build since it will destroy what
 *  ever PLU data is in the PEP file by overwriting it with created PLU records.
 *
 *  See the call site in the function A068MaintDlgProc() when handling the WM_INITDIALOG
 *  message as the AC68 dialog is created and initialized for PLU maintenance.
*/
SHORT  A068GeneratePluFile (ULONG  ulMaxNumberOfRecords)
{
    LABELANALYSIS  Label;
    PLUIF          PluWork;
	ULONG          ulCount;
	SHORT          sStat;
	DWORD          dwPrice = 150;

    memset(&Label, 0x00, sizeof(LABELANALYSIS));
	memset (&PluWork, 0, sizeof(PluWork));

    Label.fchModifier = LA_NORMAL;   /* Normal        */

	PluWork.ParaPlu.ContPlu.uchItemType = PLU_NON_ADJ_TYP;
	PluWork.ParaPlu.ContPlu.usDept = 1;
	PluWork.uchPluAdj = 1;

	for (ulCount = 1; ulCount <= ulMaxNumberOfRecords; ulCount++) {
		// Create the following data which changes from PLU to PLU.
		//    PLU number - generated using RflLabelAnalysis() for it to be properly formatted
		//    PLU Mnemonic - generated based on the PLU number
		//    PLU Price - use PLU Number least significant 3 digits + 100 to generate a price between 100 and 1099.
		// The other data such as department number and adjective type are all constant.
		PluWork.ParaPlu.auchPluName[0] = _T('P');
		PluWork.ParaPlu.auchPluName[1] = _T('L');
		PluWork.ParaPlu.auchPluName[2] = _T('U');
		PluWork.ParaPlu.auchPluName[3] = _T(' ');

		swprintf_s (&PluWork.ParaPlu.auchPluName[4], 10, L"%7.7d", ulCount);
		swprintf_s (Label.aszScanPlu, 12, L"%d", ulCount);
		sStat = RflLabelAnalysis(&Label);
        memcpy(PluWork.auchPluNo, Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));
		RflConv4bto3b(PluWork.ParaPlu.auchPrice, ((ulCount % 1000) + 100));

		/* ----- Assign PLU Record ----- */
		OpPluAssign(&PluWork, 0);
	}

	return 0;
}

// WARNING: This function is used in function WinMain() in file Pep.c to
// handle a command line argument to export the PLU data as a .csv file.
// Any changes to this function's interface should be coordinated with
// the source code in that area.
LONG  A068ExportPluDataFile (FILE *pFile)
{
	PLUIF   tempPlu = {0};    /* Temporary PLU Data Structure             */
    ULONG   ulCur = 0L;
	SHORT   nRet = 0;
	struct  {
		USHORT  usOffset;
		USHORT  usBitMask;
		char    *mnemonic;
	} StatusTableExport[] = {
		{2, 0x08, "Dept. CC"},         // IDD_A68_STS16
		{2, 0x01, "Scalable"},         // IDD_A68_STS14
		{2, 0x02, "Condiment"},        // IDD_A68_STS15
		{1, 0x08, "Food Stamp"},       // IDD_A68_STS34
		{5, 0x40, "WIC"},              // IDD_A68_STS35
		{1, 0x01, "Tax #1"},           // IDD_A68_STS09
		{1, 0x02, "Tax #2"},           // IDD_A68_STS10
		{1, 0x04, "Tax #3"},           // IDD_A68_STS11
		{1, 0x40, "Disc #1"},          // IDD_A68_STS12
		{1, 0x80, "Disc #2"},          // IDD_A68_STS13
		{2, 0x10, "RP #1"},            // IDD_A68_STS17
		{2, 0x20, "RP #2"},            // IDD_A68_STS18
		{2, 0x40, "RP #3"},            // IDD_A68_STS19
		{2, 0x80, "RP #4"},            // IDD_A68_STS20
		{5, 0x01, "RP #5"},            // IDD_A68_STS30
		{5, 0x02, "RP #6"},            // IDD_A68_STS31
		{5, 0x04, "RP #7"},            // IDD_A68_STS32
		{5, 0x08, "RP #8"}             // IDD_A68_STS33
	};

	if (!pFile) return 0;

	{
		int i;
		fprintf (pFile, "PLU, Type, Adj, Mnemonic, Price, Dept, Table, Group");
		for (i = 0; i < sizeof(StatusTableExport)/sizeof(StatusTableExport[0]); i++) {
			fprintf (pFile, ", %s", StatusTableExport[i].mnemonic);
		}
		fprintf (pFile, ", Tare, PPI, Sales Code, Bonus Index, Print Priority, Report Code, Linked PLU, Family Code, Color Palette");
		fprintf (pFile, "\n");
	}

	tempPlu.ulCounter = 0L;
    do {
        /* ----- Load PLU Records from PLU File until End of File ----- */
        nRet = OpPluAllRead(&tempPlu, 0);

        /* ----- Check End of PLU File ----- */
        if (nRet == OP_FILE_NOT_FOUND || nRet == OP_EOF) {
            break;
        } else {
			int i;
			int   pluType = ' ';
			LONG  lPrice = 0;
			PepConv3Cto4L((LPBYTE)tempPlu.ParaPlu.auchPrice, (LPDWORD)&lPrice);
			switch (tempPlu.ParaPlu.ContPlu.uchItemType) {
				case PLU_OPEN_TYP:
					pluType = 'O';
					break;
				case PLU_NON_ADJ_TYP:
					pluType = ' ';
					break;
				case PLU_ADJ_TYP:
					pluType = 'A';
					break;
			}

            tempPlu.ParaPlu.auchPluName[OP_PLU_NAME_SIZE] = 0;  // ensure string is terminated.

			fprintf (pFile, " %-13.13S, %c, %d, %S, %d, %d, %d, %d", tempPlu.auchPluNo, pluType, tempPlu.uchPluAdj, tempPlu.ParaPlu.auchPluName, lPrice,
				tempPlu.ParaPlu.ContPlu.usDept, tempPlu.ParaPlu.uchTableNumber, tempPlu.ParaPlu.uchGroupNumber);

			for (i = 0; i < sizeof(StatusTableExport)/sizeof(StatusTableExport[0]); i++) {
				fprintf (pFile, ",%d", ((tempPlu.ParaPlu.ContPlu.auchContOther[StatusTableExport[i].usOffset] & StatusTableExport[i].usBitMask) ? 1 : 0));
			}

			fprintf (pFile, ", %d, %d, %d, %d, %d, %d, %d, %d, %d", (tempPlu.ParaPlu.usTareInformation & PLU_REQ_TARE_MASK), tempPlu.ParaPlu.uchLinkPPI, tempPlu.ParaPlu.uchRestrict, tempPlu.ParaPlu.ContPlu.usBonusIndex,
				tempPlu.ParaPlu.uchPrintPriority, tempPlu.ParaPlu.ContPlu.uchRept, tempPlu.ParaPlu.usLinkNo, tempPlu.ParaPlu.usFamilyCode, tempPlu.ParaPlu.uchColorPaletteCode);
			fprintf (pFile, "\n");

        }
    } while (nRet == OP_PERFORM);

	return  1;
}

BOOL A068ExportPluDataFileStart (HWND hDlg)
{
    static  WCHAR    szFileName[512];
	OPENFILENAMEW    ofn = {0};

	szFileName[0] = '\0';
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFilter = 0;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName);
    ofn.lpstrFileTitle = 0;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    if (!GetPepSaveFileName(&ofn)) {
        return FALSE;
    }

	{
		int   i;
		FILE *pFile;
		char  aszFileName[512];

		for (i = 0; aszFileName[i] = szFileName[i]; i++);

		if ((pFile = fopen (aszFileName, "w")) != NULL) {
			A068ExportPluDataFile (pFile);
			fclose (pFile);
		} else {
               MessageBeep(MB_ICONEXCLAMATION);
               MessageBoxPopUp(hDlg, L"Unable to open file.", L"File Save Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return TRUE;
}

typedef  struct {
    WCHAR	auchPluNo[OP_PLU_LEN];
	WCHAR   auchPluName[OP_PLU_NAME_SIZE + 1];
} FilteredPluData;

#define  FILTEREDPLUDATA_BY_GROUPNO          0x00000001    // IDD_A68_FILTERBY_GROUPNO
#define  FILTEREDPLUDATA_BY_TABLENO          0x00000002    // IDD_A68_FILTERBY_TABLENO
#define  FILTEREDPLUDATA_BY_DEPTNO           0x00000004    // IDD_A68_FILTERBY_DEPTNO
#define  FILTEREDPLUDATA_BY_PRINTPRIORITY    0x00000008    // IDD_A68_FILTERBY_PRINTPRIORITY
#define  FILTEREDPLUDATA_BY_SALESRESTRICT    0x00000010    // IDD_A68_FILTERBY_SALESRESTRICT
#define  FILTEREDPLUDATA_BY_PPICODE          0x00000020    // IDD_A68_FILTERBY_PPICODE
#define  FILTEREDPLUDATA_BY_ADJ_GROUP        0x00000040    // IDD_A68_FILTERBY_ADJ_GROUP
#define  FILTEREDPLUDATA_BY_REM_DEV_MTCH     0x00000080    // IDD_A68_FILTERBY_REM_DEV_MTCH
#define  FILTEREDPLUDATA_BY_REM_DEV_ANY      0x00000100    // IDD_A68_FILTERBY_REM_DEV_ANY
#define  FILTEREDPLUDATA_BY_TARENO           0x00000200    // IDD_A68_FILTERBY_TARENO
#define  FILTEREDPLUDATA_BY_BONUSTOTAL       0x00000400    // IDD_A68_FILTERBY_BONUSTOTAL
#define  FILTEREDPLUDATA_BY_DISCITMIZR_MTCH  0x00000800    // IDD_A68_FILTERBY_DISCITMIZR_MTCH
#define  FILTEREDPLUDATA_BY_TAXITMIZR_MTCH   0x00001000    // IDD_A68_FILTERBY_TAXITMIZR_MTCH

typedef struct {
	ULONG   ulFilterMask;
	USHORT	usFamilyCode;
	USHORT  usDept;               // FILTEREDPLUDATA_BY_DEPTNO,  IDD_A68_FILTERBY_DEPTNO
    UCHAR   uchTableNumber;       // FILTEREDPLUDATA_BY_TABLENO, IDD_A68_FILTERBY_TABLENO
    UCHAR   uchGroupNumber;       // FILTEREDPLUDATA_BY_GROUPNO, IDD_A68_FILTERBY_GROUPNO
	UCHAR   uchPrintPriority;     // FILTEREDPLUDATA_BY_PRINTPRIORITY, IDD_A68_FILTERBY_PRINTPRIORITY
    UCHAR   uchRestrict;          // Sales Restriction Code, FILTEREDPLUDATA_BY_SALESRESTRICT, IDD_A68_FILTERBY_SALESRESTRICT
    UCHAR   uchTareNumber;        // Tare Number, FILTEREDPLUDATA_BY_TARENO, IDD_A68_FILTERBY_TARENO
	UCHAR   uchAdjective;         // FILTEREDPLUDATA_BY_ADJ_GROUP, IDD_A68_FILTERBY_ADJ_GROUP
	UCHAR   uchRemoteDev_01;      // Device 1 - 4, FILTEREDPLUDATA_BY_REM_DEV_MTCH, IDD_A68_FILTERBY_REM_DEV_MTCH
	UCHAR   uchRemoteDev_02;      // Device 5 - 8, FILTEREDPLUDATA_BY_REM_DEV_MTCH, IDD_A68_FILTERBY_REM_DEV_MTCH
	USHORT  uchLinkPPI;           // PPI code, FILTEREDPLUDATA_BY_PPICODE, IDD_A68_FILTERBY_PPICODE
	USHORT	usBonusIndex;         // Bonus Total Index, FILTEREDPLUDATA_BY_BONUSTOTAL, IDD_A68_FILTERBY_BONUSTOTAL
	UCHAR   uchDiscItemizer;      // Discount Itemizer #1 or #2, FILTEREDPLUDATA_BY_DISCITMIZR_MTCH, IDD_A68_FILTERBY_DISCITMIZR_MTCH
	UCHAR   uchTaxItemizer;       // Tax Itemizer #1 or #2, FILTEREDPLUDATA_BY_TAXITMIZR_MTCH, IDD_A68_FILTERBY_TAXITMIZR_MTCH
} FilteredPluDataFilter;

static LONG  A068FilterPluDataFile (HWND hDlg, int iListId, FilteredPluDataFilter Filter, FilteredPluData *pPluList, int nSize)
{
	PLUIF   tempPlu = {0};    /* Temporary PLU Data Structure             */
    ULONG   ulCur = 0L;
	SHORT   nRet = 0;
	LONG    lCount = 0;

	tempPlu.ulCounter = 0L;
    do {
        /* ----- Load PLU Records from PLU File until End of File ----- */
        nRet = OpPluAllRead(&tempPlu, 0);

        /* ----- Check End of PLU File ----- */
        if (nRet == OP_FILE_NOT_FOUND || nRet == OP_EOF) {
            break;
        } else {
			int  iCopied = 0;    // indicaates if any of the filter criteria was met or not

			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_GROUPNO) {
				if ((iCopied |= (tempPlu.ParaPlu.uchGroupNumber == Filter.uchGroupNumber)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_TABLENO) {
				if ((iCopied |= (tempPlu.ParaPlu.uchTableNumber == Filter.uchTableNumber)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_DEPTNO) {
				if ((iCopied |= (tempPlu.ParaPlu.ContPlu.usDept == Filter.usDept)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_PRINTPRIORITY) {
				if ((iCopied |= (tempPlu.ParaPlu.uchPrintPriority == Filter.uchPrintPriority)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_TABLENO) {
				if ((iCopied |= (tempPlu.ParaPlu.uchTableNumber == Filter.uchTableNumber)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_SALESRESTRICT) {
				if ((iCopied |= (tempPlu.ParaPlu.uchRestrict == Filter.uchRestrict)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_TARENO) {
				if ((iCopied |= ((tempPlu.ParaPlu.usTareInformation & PLU_REQ_TARE_MASK) == Filter.uchTareNumber)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_BONUSTOTAL) {
				if ((iCopied |= (tempPlu.ParaPlu.ContPlu.usBonusIndex == Filter.usBonusIndex)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_PPICODE) {
				if ((iCopied |= (tempPlu.ParaPlu.uchLinkPPI == Filter.uchLinkPPI)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if ( (Filter.ulFilterMask & FILTEREDPLUDATA_BY_ADJ_GROUP) && (tempPlu.ParaPlu.ContPlu.uchItemType & PLU_ADJ_TYP) == PLU_ADJ_TYP) {
				if ((iCopied |= ((tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_3] & PLU_USE_ADJG_MASK) == Filter.uchAdjective)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
 			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_REM_DEV_MTCH) {
				UCHAR  uchMask_01 = 0;
				UCHAR  uchMask_02 = 0;

				uchMask_01 |= (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] & (PLU_SND_KITCH1 | PLU_SND_KITCH2 | PLU_SND_KITCH3 | PLU_SND_KITCH4));
				uchMask_02 |= (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] & (PLU_SND_KITCH5 | PLU_SND_KITCH6 | PLU_SND_KITCH7 | PLU_SND_KITCH8));
				if ((iCopied |= (uchMask_01 == Filter.uchRemoteDev_01 && uchMask_02 == Filter.uchRemoteDev_02)) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			} else if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_REM_DEV_ANY) {
				UCHAR  uchMask_01 = 0;
				UCHAR  uchMask_02 = 0;

				uchMask_01 |= (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] & (PLU_SND_KITCH1 | PLU_SND_KITCH2 | PLU_SND_KITCH3 | PLU_SND_KITCH4));
				uchMask_02 |= (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] & (PLU_SND_KITCH5 | PLU_SND_KITCH6 | PLU_SND_KITCH7 | PLU_SND_KITCH8));
				if ((iCopied |= ((uchMask_01 & Filter.uchRemoteDev_01) || (uchMask_02 & Filter.uchRemoteDev_02))) && pPluList) {
					memcpy (pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy (pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_DISCITMIZR_MTCH) {
				UCHAR  uchMask_01 = 0;

				uchMask_01 |= (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & (PLU_AFFECT_DISC1 | PLU_AFFECT_DISC2));
				if ((iCopied |= (uchMask_01 == Filter.uchDiscItemizer) ) && pPluList) {
					memcpy(pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy(pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
			if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_TAXITMIZR_MTCH) {
				UCHAR  uchMask_01 = 0;

				uchMask_01 |= (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2 | PLU_AFFECT_TAX3));
				if ((iCopied |= (uchMask_01 == Filter.uchTaxItemizer)) && pPluList) {
					memcpy(pPluList->auchPluNo, tempPlu.auchPluNo, sizeof(pPluList->auchPluNo));
					memcpy(pPluList->auchPluName, tempPlu.ParaPlu.auchPluName, sizeof(pPluList->auchPluName));
				}
			}
			// if we have a match and if the match is either not an adjective or it is
			// the first adjective then lets add it to the list. no need to have multiple
			// copies of the same PLU in this list.
			if (iCopied && tempPlu.uchPluAdj < 2) {
				if (iListId) A068SetListDesc(hDlg, iListId, &tempPlu);
				if (pPluList) pPluList++;
				lCount++;         // count the number of PLUs that we have found
			}
			if (pPluList && lCount >= nSize) break;  // if count matches size and we are doing an array then break.
       }
    } while (nRet == OP_PERFORM);

	return  lCount;  // retun the number of PLUs that we found, either added to list box or in array or both.
}

static void A068DisplayFiteredByPluData (HWND hDlg, int iListId)    // handler for IDD_A68_FILTERBY_UPDATE
{
	FilteredPluDataFilter  Filter = {0};
	FilteredPluData        PluList[50] = {0};
	LONG  lCount;

#if 0
		case IDD_A68_DEPTNO:  unMax = A68_DEPT_MAX;  break;
		case IDD_A68_RPTCD:  unMax = A68_RPT_MAX;  break;
		case IDD_A68_TBLNO:  unMax = A68_TBLNO_MAX;  break;
		case IDD_A68_GRPNO:  unMax = A68_GRPNO_MAX;  break;
		case IDD_A68_PRTPRY:  unMax = A68_PRTPRY_MAX;  break;
		case IDD_A68_PPI:  unMax = usPPIMax;    /*A68_PPI_MAX*/  break;
		case IDD_A68_SALES:  unMax = A68_SALES_MAX;  break;
		case IDD_A68_TARE:  unMax = A68_TARE_MAX;  break;
		case IDD_A68_COLOR:  unMax = A68_COLOR_MAX;  break;
		default:  unMax = A68_BTTL_MAX;  break;
	}
#endif

    /* ----- Get Inputed Value from Specified EditText ----- */
    Filter.usDept = GetDlgItemInt(hDlg, IDD_A68_DEPTNO, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_DEPTNO) ? FILTEREDPLUDATA_BY_DEPTNO : 0;
    Filter.uchGroupNumber = GetDlgItemInt(hDlg, IDD_A68_GRPNO, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_GROUPNO) ? FILTEREDPLUDATA_BY_GROUPNO : 0;
    Filter.uchPrintPriority = GetDlgItemInt(hDlg, IDD_A68_PRTPRY, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_PRINTPRIORITY) ? FILTEREDPLUDATA_BY_PRINTPRIORITY : 0;
	Filter.uchTableNumber = GetDlgItemInt(hDlg, IDD_A68_TBLNO, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_TABLENO) ? FILTEREDPLUDATA_BY_TABLENO : 0;
	Filter.uchRestrict = GetDlgItemInt(hDlg, IDD_A68_SALES, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_SALESRESTRICT) ? FILTEREDPLUDATA_BY_SALESRESTRICT : 0;
	Filter.uchTareNumber = GetDlgItemInt(hDlg, IDD_A68_TARE, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_TARENO) ? FILTEREDPLUDATA_BY_TARENO : 0;
	Filter.usBonusIndex = GetDlgItemInt(hDlg, IDD_A68_BONUS, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_BONUSTOTAL) ? FILTEREDPLUDATA_BY_BONUSTOTAL : 0;
	Filter.uchLinkPPI = GetDlgItemInt(hDlg, IDD_A68_PPI, NULL, FALSE);
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_PPICODE) ? FILTEREDPLUDATA_BY_PPICODE : 0;
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_ADJ_GROUP) ? FILTEREDPLUDATA_BY_ADJ_GROUP : 0;
	if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_ADJ_GROUP) {
		// We allow filtering by Adjective Group only if the current PLU is an adjective PLU.
		// Don't worry about right justifying the adjective group number. We just do a comparison
		// above with exact same masking.
		if (tempPlu.ParaPlu.ContPlu.uchItemType & PLU_ADJ_TYP) {
			Filter.uchAdjective = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_3] & PLU_USE_ADJG_MASK);
		} else {
			Filter.ulFilterMask &= ~ FILTEREDPLUDATA_BY_ADJ_GROUP;
		}
	}
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_REM_DEV_MTCH) ? FILTEREDPLUDATA_BY_REM_DEV_MTCH : 0;
	if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_REM_DEV_MTCH) {
		// Don't worry about right justifying the remote device bit mask. We just do a comparison
		// above with exact same masking.
		Filter.uchRemoteDev_01 = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] & (PLU_SND_KITCH1 | PLU_SND_KITCH2 | PLU_SND_KITCH3 | PLU_SND_KITCH4));
		Filter.uchRemoteDev_02 = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] & (PLU_SND_KITCH5 | PLU_SND_KITCH6 | PLU_SND_KITCH7 | PLU_SND_KITCH8));
	}
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_REM_DEV_ANY) ? FILTEREDPLUDATA_BY_REM_DEV_ANY : 0;
	if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_REM_DEV_ANY) {
		// Don't worry about right justifying the remote device bit mask. We just do a comparison
		// above with exact same masking.
		Filter.uchRemoteDev_01 = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] & (PLU_SND_KITCH1 | PLU_SND_KITCH2 | PLU_SND_KITCH3 | PLU_SND_KITCH4));
		Filter.uchRemoteDev_02 = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] & (PLU_SND_KITCH5 | PLU_SND_KITCH6 | PLU_SND_KITCH7 | PLU_SND_KITCH8));
	}
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_DISCITMIZR_MTCH) ? FILTEREDPLUDATA_BY_DISCITMIZR_MTCH : 0;
	if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_DISCITMIZR_MTCH) {
		Filter.uchDiscItemizer = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & (PLU_AFFECT_DISC1 | PLU_AFFECT_DISC2));
	}
	Filter.ulFilterMask |= IsDlgButtonChecked(hDlg, IDD_A68_FILTERBY_TAXITMIZR_MTCH) ? FILTEREDPLUDATA_BY_TAXITMIZR_MTCH : 0;
	if (Filter.ulFilterMask & FILTEREDPLUDATA_BY_TAXITMIZR_MTCH) {
		Filter.uchTaxItemizer = (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & (PLU_AFFECT_TAX1 | PLU_AFFECT_TAX2 | PLU_AFFECT_TAX3));
	}

	{
		SendDlgItemMessage(hDlg, iListId, LB_RESETCONTENT, 0, 0L);

		// just update the Filter By list and do not bother to generate a list of matching PLUs.
		lCount = A068FilterPluDataFile (hDlg, iListId, Filter, 0, 0);
	}
}


/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A068MaintDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*                       PLU Maintenance (AC 63, 63, 68, 82)
* ===========================================================================
*/
BOOL    WINAPI  A068MaintDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static  HGLOBAL hGlbDesc;               /* Handle of String Global Heap */
    static  UINT    unPluType;              /* Current Maintenance PLU Type */
    static  DWORD   adwPrePrc[A68_ADJ_NUM]; /* Current Maintenance Price    */
    static  PEPSPIN PepSpin;                /* Configulation of SpinButton  */
    int     idEdit;
    WORD    wDlgID;
    WCHAR    szMsg[256];
    WCHAR    szDesc[128];
    WCHAR    szWork[48];
    WCHAR    szPluNo[OP_PLU_LEN + 1];
    DWORD   dwIndex;
    int     nCount;
    WCHAR    szTemp[OP_PLU_LEN + 1];
    BOOL    bRet;                          /* Saratoga */
    SHORT   sRet = OP_PERFORM;
	int		wI;			//ESMITH
	WCHAR	szType[A68_PRC_LEN + 4];  //ESMITH
	WCHAR	 szCharString[OP_PLU_LEN + 1];		//SR1016	***PDINU
	WCHAR   *pszChar = 0;						//SR1016	***PDINU
	DWORD i;   //Index for Mnemonic search. SR1016 ***PDINU
	DWORD j;	 //Index for array that stores user input for Mnemonic search.  SR1016  ***PDINU
	DWORD checkCount;							//SR1016	***PDINU
	static int arrayIndex = 0;					//SR1016	***PDINU
	static int displayArray[1000];				//SR1016	***PDINU
	static int arrayMarker = 0;					//SR1016	***PDINU
	WCHAR   szTempBuff[256];					//SR1016	***PDINU
	WCHAR   stringCompare[OP_PLU_LEN + 1];		//SR1016	***PDINU
	static  WCHAR  oldString[OP_PLU_LEN + 1];	//SR1016	***PDINU

	PARAFLEXMEM FlexData;

	/* SR 1169 - adding restriction to PPI field, to align with
		P2 PPI setting. CSMALL 11/27/2006
	/* ----- Set address to PPI File ----- */
	FlexData.uchAddress = FLEX_PPI_ADR;
	/* ----- Read the filexible memory data of PLU File ----- */
	ParaFlexMemRead(&FlexData);
	usPPIMax = (USHORT)FlexData.ulRecordNumber;
    
    switch (wMsg) {

    case    WM_INITDIALOG:
#if 0
		// uncomment and modify to auto generate a very large PLU file
		// we currently just generate whatever number is specified for number of PLUs.
		//
		// WARNING: This function should not be enabled for a release build of PEP!!!
		//
		FlexData.uchAddress = FLEX_PLU_ADR;
		ParaFlexMemRead(&FlexData);
		A068GeneratePluFile (FlexData.ulRecordNumber);
#endif

		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));
        
                /* ----- set initial description, V3.3 ----- */
        LoadString(hResourceDll, IDS_A68_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
        WindowRedrawText(hDlg, szDesc);
        LoadString(hResourceDll, IDS_SET, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_SET, szDesc);

        LoadString(hResourceDll, IDS_A68_STRDESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRDESC, szDesc);
        LoadString(hResourceDll, IDS_A68_STRDESC2, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRDESC2, szDesc);
        LoadString(hResourceDll, IDS_A68_ADDCHG, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_ADDCHG, szDesc);
        LoadString(hResourceDll, IDS_A68_DEL, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_DEL, szDesc);
        LoadString(hResourceDll, IDS_A68_STRPLUNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRPLUNO, szDesc);
        LoadString(hResourceDll, IDS_A68_STRPLULEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRPLULEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRDEPTNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRDEPTNO, szDesc);
        LoadString(hResourceDll, IDS_A68_STRRPT, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRRPT, szDesc);
        LoadString(hResourceDll, IDS_A68_STRRPTNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRRPTNO, szDesc);
        LoadString(hResourceDll, IDS_A68_STRINDEX, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRINDEX, szDesc);
        LoadString(hResourceDll, IDS_A68_STRINDEXLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRINDEXLEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRTABLE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRTABLE, szDesc);
        LoadString(hResourceDll, IDS_A68_STRTABLELEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRTABLELEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRGRP, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRGRP, szDesc);
        LoadString(hResourceDll, IDS_A68_STRGRPLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRGRPLEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRPRT, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRPRT, szDesc);
        LoadString(hResourceDll, IDS_A68_STRPRTLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRPRTLEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRPPI, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRPPI, szDesc);
        // max PPI value should reflect PPI setting from P2. SR 1169 CSMALL 11/27/2006
        LoadString(hResourceDll, IDS_A68_STRPPILEN, szWork, PEP_STRING_LEN_MAC(szWork));
		wsPepf(szDesc, szWork, usPPIMax);
        DlgItemRedrawText(hDlg, IDD_A68_STRPPILEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRMNEMO, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRMNEMO, szDesc);
        LoadString(hResourceDll, IDS_A68_STRMNEMOMAX, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRMNEMOMAX, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJ, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJ, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJ1, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJ1, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJ2, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJ2, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJ3, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJ3, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJ4, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJ4, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJ5, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJ5, szDesc);
        LoadString(hResourceDll, IDS_A68_STRADJLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRADJLEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRCODE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRCODE, szDesc);
        LoadString(hResourceDll, IDS_A68_STRPRICE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRPRICE, szDesc);
        LoadString(hResourceDll, IDS_A68_CONF, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_CONF, szDesc);
/* Saratoga */
        LoadString(hResourceDll, IDS_A68_STRMULTI, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRMULTI, szDesc);
        LoadString(hResourceDll, IDS_A68_STRMULTILEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRMULTILEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRLINK, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRLINK, szDesc);
        LoadString(hResourceDll, IDS_A68_STRLINKLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRLINKLEN, szDesc);
        LoadString(hResourceDll, IDS_A68_STRSALES, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRSALES, szDesc);
        LoadString(hResourceDll, IDS_A68_STRSALESLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRSALESLEN, szDesc);
        LoadString(hResourceDll, IDS_A68_NORMAL, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_NORMAL, szDesc);
        LoadString(hResourceDll, IDS_A68_EVER, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_EVER, szDesc);
/* ADD Saratoga */
        LoadString(hResourceDll, IDS_A68_NEXT, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_NEXT, szDesc);
        LoadString(hResourceDll, IDS_A68_TOP, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_TOP, szDesc);
/* ### ADD Saratoga FamilyCode (V1_0.02)(051800) */
        LoadString(hResourceDll, IDS_A68_STRFAMILY, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRFAMILY, szDesc);
        LoadString(hResourceDll, IDS_A68_STRFAMILYLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRFAMILYLEN, szDesc);

		/* ESMITH */
		LoadString(hResourceDll, IDD_A68_STROPEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
	    DlgItemRedrawText(hDlg, IDD_A68_STROPEN, szDesc);
		LoadString(hResourceDll, IDD_A68_STRPRESET, szDesc, PEP_STRING_LEN_MAC(szDesc));
	    DlgItemRedrawText(hDlg, IDD_A68_STRPRESET, szDesc);
		LoadString(hResourceDll, IDD_A68_STRADJECTIVE, szDesc, PEP_STRING_LEN_MAC(szDesc));
	    DlgItemRedrawText(hDlg, IDD_A68_STRADJECTIVE, szDesc);
		LoadString(hResourceDll, IDD_A68_STRPLUTYPE, szDesc, PEP_STRING_LEN_MAC(szDesc));
	    DlgItemRedrawText(hDlg, IDD_A68_STRPLUTYPE, szDesc);

		/* CSMALL - PLU Color Enhancements */
        LoadString(hResourceDll, IDS_A68_STRCOLOR, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRCOLOR, szDesc);
        LoadString(hResourceDll, IDS_A68_STRCOLORLEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRCOLORLEN, szDesc);


		LoadString(hResourceDll, IDS_A68_STRTARE, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRTARE, szDesc);
        LoadString(hResourceDll, IDS_A68_STRTARELEN, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STRTARELEN, szDesc);

        LoadString(hResourceDll, IDS_A68_STRTARECHECK, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_STSTARECCHECK, szDesc);

		/* ---- Set Initial state of the preset type ESMITH ---- */
		A068ChngPresetState(hDlg,IDD_A68_PRESETTYPE);

		fVer = LA_NORMAL;

        /* ----- Read No. of PLU Memory from Prog. #2 ----- */
        A068ReadRec();

        /* ----- Check Current No. of PLU Memory in Prog. #2 ----- */
        if (! ulPLUMax) {
            /* ----- Load Error Message from Resource ----- */
            /* ----- Display Error Message with MessageBox ----- */
            LoadString(hResourceDll, IDS_A68_FLEX_EMPTY, szMsg, PEP_STRING_LEN_MAC(szMsg));
            A068DispErr(hDlg, szMsg, MB_OK | MB_ICONEXCLAMATION);

			//Return Cancel to indicate failure
            EndDialog(hDlg, LOWORD(IDCANCEL));
        } else {
            if (A068AllocMem((LPHGLOBAL)&hGlbDesc)) {
                /* ----- Load Error Message from Resource ----- */
                /* ----- Display Error Message with MessageBox ----- */
                LoadString(hResourceDll, IDS_PEP_ALLOC_ERR, szMsg, PEP_STRING_LEN_MAC(szMsg));
                A068DispErr(hDlg, szMsg, MB_OK | MB_ICONEXCLAMATION);

				//Return Cancel to indicate failure
                EndDialog(hDlg, LOWORD(IDCANCEL));
            } else {
                /* ----- Load PLU Data and Initialize Configulation ----- */
                A068InitData(hDlg, IDD_A68);

                /* ----- save plu No.(last index of list box) ----- */
                nCount = (int)SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);
                DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_GETTEXT, (WORD)(nCount - 1), (LONG)szWork);
                memset(workPlu, 0x00, sizeof(workPlu));
                memcpy(workPlu, szWork, OP_PLU_LEN * sizeof(WCHAR));

                /* ----- Initialize Configulation of SpinButton ----- */
                PepSpin.lMin       = (long)A68_DATA_MIN;
                PepSpin.nStep      = A68_SPIN_STEP;
                PepSpin.nTurbo     = 0;
                PepSpin.fSpinStyle = PEP_SPIN_NOROLL;
            }
        }
		A111ModeLessDialog (hDlg, SW_HIDE);
		A114ModeLessDialog (hDlg, SW_HIDE);
		A116ModeLessDialog (hDlg, SW_HIDE);
		A160ModeLessDialog(hDlg, SW_HIDE);
		A161ModeLessDialog(hDlg, SW_HIDE);
		A170ModeLessDialog(hDlg, SW_HIDE);
		// AC71 PPI modeless dialog causes file corruption due to use of PepFileBackup() in A071DlgProc() WM_INITDIALOG.
		// A071ModeLessDialog(hDlg, SW_HIDE);
        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j = IDD_A68_CTRL; j <= IDD_A68_STRPLUTYPE; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j = IDD_A68_STRDESC; j <= IDD_A68_STRFAMILYLEN; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDD_SET, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case    WM_VSCROLL:
        /* ----- Set Target EditText ID ----- */
        idEdit = (int)(GetDlgCtrlID((HWND)lParam) - A68_SPIN_OFFSET);

        /* ----- Set Maximum Value of Data Range by Selected Button ----- */
        if (idEdit == IDD_A68_NO) {
/*** Saratoga
            PepSpin.nTurbo   = A68_SPIN_TURBO;
            PepSpin.nTurStep = A68_SPIN_TURSTEP;
            PepSpin.lMax     = (long)A68_NO_MAX;
            PepSpin.lMin     = (long)A68_NO_MIN;
***/
        } else if (idEdit == IDD_A68_DEPTNO) {
            PepSpin.lMax = (long)wDeptMax;
            PepSpin.lMin = (long)A68_DEPT_MIN;
        } else if (idEdit == IDD_A68_RPTCD) {
            PepSpin.lMax = (long)A68_RPT_MAX;
            PepSpin.lMin = (long)A68_DATA_MIN;
        } else if (idEdit == IDD_A68_BONUS) {
            PepSpin.lMax = (long)A68_BTTL_MAX;
            PepSpin.lMin = (long)A68_DATA_MIN;
        } else if (idEdit == IDD_A68_TBLNO) {
            PepSpin.lMax = (long)A68_TBLNO_MAX;
            PepSpin.lMin = (long)A68_DATA_MIN;
        } else if (idEdit == IDD_A68_GRPNO) {
            PepSpin.lMax = (long)A68_GRPNO_MAX;
            PepSpin.lMin = (long)A68_DATA_MIN;
        } else if (idEdit == IDD_A68_PRTPRY) {
            PepSpin.lMax = (long)A68_PRTPRY_MAX;
            PepSpin.lMin = (long)A68_DATA_MIN;
        } else if (idEdit == IDD_A68_PPI) {
            PepSpin.lMax = (long)usPPIMax;/*A68_PPI_MAX*/
            PepSpin.lMin = (long)A68_DATA_MIN;
        } else if (idEdit == IDD_A68_LINK) {      /* Saratoga */
            PepSpin.lMax = (long)A68_LINK_MAX;    /* Saratoga */
            PepSpin.lMin = (long)A68_DATA_MIN;    /* Saratoga */
        } else if (idEdit == IDD_A68_SALES) {     /* Saratoga */
            PepSpin.lMax = (long)A68_SALES_MAX;   /* Saratoga */
            PepSpin.lMin = (long)A68_DATA_MIN;    /* Saratoga */
        } else if(idEdit == IDD_A68_MULTI) {      /* Saratoga */
            PepSpin.lMax = (long)A68_MULTI_MAX;   /* Saratoga */
            PepSpin.lMin = (long)A68_DATA_MIN;    /* Saratoga */
        } else if (idEdit == IDD_A68_FAMILY) {     /* Saratoga */
            PepSpin.lMax = (long)A68_FAMILY_MAX;   /* Saratoga */
            PepSpin.lMin = (long)A68_DATA_MIN;    /* Saratoga */
        } else if (idEdit == IDD_A68_COLOR) {
			PepSpin.lMax = (long)A68_COLOR_MAX;
			PepSpin.lMin = (long)A68_DATA_COLORPALETTE_MIN;
        } else if (idEdit == IDD_A68_TARE) {
			PepSpin.lMax = (long)A68_TARE_MAX;
			PepSpin.lMin = (long)0;
		} else {
            PepSpin.lMax     = A68_PRC_MAX;
            PepSpin.lMin     = (long)A68_DATA_MIN;
            PepSpin.nTurbo   = A68_SPIN_TURBO;
            PepSpin.nTurStep = A68_SPIN_TURSTEP;
        }

        PepSpinProc(hDlg, wParam, idEdit, (LPPEPSPIN)&PepSpin);

        PostMessage(hDlg, WM_COMMAND, MAKELONG(LOWORD(idEdit), EN_CHANGE), 0L);

        return FALSE;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {
		case IDD_A68_EXPORT_PLU:
			A068ExportPluDataFileStart (hDlg);
			break;

		case IDD_A68_MODELESS_A111:                     // display/undisplay AC111 modeless dialog
			ShowWindow (A111ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;
		case IDD_A68_MODELESS_A114:                     // display/undisplay AC114 modeless dialog
			ShowWindow (A114ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;
		case IDD_A68_MODELESS_A116:                     // display/undisplay AC114 modeless dialog
			ShowWindow (A116ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;
		case IDD_A68_MODELESS_A160:                     // display/undisplay AC160 modeless dialog
			ShowWindow (A160ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;
		case IDD_A68_MODELESS_A161:                     // display/undisplay AC161 modeless dialog
			ShowWindow (A161ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;
		case IDD_A68_MODELESS_A170:                     // display/undisplay AC170 modeless dialog
			ShowWindow (A170ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;
		case IDD_A68_MODELESS_A071:                     // display/undisplay AC071 modeless dialog
			// AC71 PPI modeless dialog causes file corruption due to use of PepFileBackup() in A071DlgProc() WM_INITDIALOG.
			//ShowWindow (A071ModeLessDialogWnd(), (IsDlgButtonChecked(hDlg, wParam) == BST_CHECKED ? SW_SHOW : SW_HIDE));
			break;

        /* ---- Click events for PLU Type ---- */
		case    IDD_A68_PRESETTYPE:		
			 if (HIWORD(wParam) == BN_CLICKED) 
			 {
				/* ---- Change state of the preset type ESMITH ---- */
				A068ChngPresetState(hDlg,A68_TYPE_NONADJ);
				return TRUE;
			 }
			 return FALSE;

		case    IDD_A68_OPENTYPE:
			 if (HIWORD(wParam) == BN_CLICKED) 
			 {
				/* ---- Change state of the preset type ESMITH ---- */
				A068ChngPresetState(hDlg,A68_TYPE_OPEN);
				return TRUE;
			 }

			 return FALSE;

		case    IDD_A68_ADJECTIVETYPE:
			 if (HIWORD(wParam) == BN_CLICKED) 
			 {
				/* ---- Change state of the preset type ESMITH ---- */
				A068ChngPresetState(hDlg,A68_TYPE_ONEADJ);
				return TRUE;
			 }

			 return FALSE;

        case    IDD_A68_NEXT:
        case    IDD_A68_TOP:
            if (LOWORD(wParam) == IDD_A68_NEXT){
               EnableWindow(GetDlgItem(hDlg, IDD_A68_TOP), TRUE);
               A068ResetListDesc(hDlg, IDD_A68_NEXT);

               nCount = (int)SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code             
               if (nCount <= A68_ADDPOS + 1) {
#endif
               if (nCount < A68_ADDPOS) {
                  EnableWindow(GetDlgItem(hDlg, IDD_A68_NEXT), FALSE);
                  EnableWindow(GetDlgItem(hDlg, IDD_A68_TOP), TRUE);//RPH 12/09/02 SR#66
               }
            } else {
               EnableWindow(GetDlgItem(hDlg, IDD_A68_NEXT), TRUE);    /* Next button Enable */
               EnableWindow(GetDlgItem(hDlg, IDD_A68_TOP), FALSE);    /* Top button Disable */

               memset(workPlu, 0x00, OP_PLU_LEN * sizeof(WCHAR));
               A068ResetListDesc(hDlg, IDD_A68_TOP);
            }
            return FALSE;

        case    IDD_A68_CONF:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (IsDlgButtonChecked(hDlg, wParam)) {
                    fDelConf = A68_DELCONF;
                } else {
                    fDelConf &= ~A68_DELCONF;
                }
                return TRUE;
            }
            return FALSE;

        case    IDD_SET:
            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Determine Current Tax Mode ----- */
                wDlgID = (WORD)((uchTaxMode == A68_TAX_US)  ? IDD_A68_CTRL :
                                (uchTaxMode == A68_TAX_CAN) ? IDD_A68_CAN  :
                                                              IDD_A68_VAT);

                /* ----- Create Control Code DialogBox ----- */
				tempPlu.ParaPlu.ContPlu.uchItemType &= ~PLU_TYPE_MASK_REG;
				if (IsDlgButtonChecked(hDlg, IDD_A68_OPENTYPE) == BST_CHECKED) {
					tempPlu.ParaPlu.ContPlu.uchItemType |= PLU_OPEN_TYP;
				} else if (IsDlgButtonChecked(hDlg, IDD_A68_ADJECTIVETYPE) == BST_CHECKED) {
					tempPlu.ParaPlu.ContPlu.uchItemType |= PLU_ADJ_TYP;
				} else if (IsDlgButtonChecked(hDlg, IDD_A68_PRESETTYPE) == BST_CHECKED) {
					tempPlu.ParaPlu.ContPlu.uchItemType |= PLU_NON_ADJ_TYP;
				}

                DialogBoxPopup(hResourceDll, MAKEINTRESOURCEW(wDlgID), hDlg, A068CtrlDlgProc);

                /* ----- Set Inputed Control Code Data to ListBox ----- */
                A068SetCtrlList(hDlg, (LPA68DATA)&tempPlu.ParaPlu);
            }
            return TRUE;

        case    IDD_A68_ADDCHG:
            if (HIWORD(wParam) == BN_CLICKED) {
				/* ---- ESMITH Clear values from edit boxes if not enabled ---- */
				for (wI = 0; wI <= A68_ADJ_NUM; wI++)
				{
					if((DlgItemGetText(hDlg, IDD_A68_PRESET1 + wI ,szType, A68_PRC_LEN + 1) > 0) &&
						(!IsWindowEnabled(GetDlgItem(hDlg,IDD_A68_PRESET1+wI))))
					{
						SendDlgItemMessage(hDlg, IDD_A68_PRESET1+wI, EM_SETSEL, 0, MAKELONG(0, -1));
						SendDlgItemMessage(hDlg, IDD_A68_PRESET1+wI, WM_CLEAR,  0, 0L);		
					}
				}
				
                /* ----- Save Preset Price / Check PLU Type ----- */
                A068SaveChkPrc(hDlg, (LPDWORD)adwPrePrc, (LPUINT)&unPluType);

                /* ----- Save Current PLU Data to Temporary Area ----- */
                A068SaveRec(hDlg, IDD_A68);
				// perform a check on the department number to make sure that it exists.
				// a department number that does not exist in AC114 will result in GenPOS
				// issuing an error message of Not On File when the PLU is rung up so
				// the department number specified must exist for proper functioning.
				{
					DEPTIF    tempDept = {0};
					SHORT     sDeptRet = 0;

					tempDept.usDeptNo = tempPlu.ParaPlu.ContPlu.usDept;
					tempDept.ParaDept.usTareInformation = 0;
					sDeptRet = OpDeptIndRead(&tempDept, 0);
					if (sDeptRet < 0) {
					   MessageBeep(MB_ICONEXCLAMATION);
					   wcscpy (szMsg, L"Department number specified not found.  Use AC114 to add it.");
					   wcscpy (szDesc, L"PLU Dept. No. Error");
					   MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);
					}
				}

                /* ----- Add/Change Inputed PLU Data to PLU File ----- */
                if (! A068RegistData(hDlg, unPluType, (LPDWORD)adwPrePrc)) {
                    /* ----- Refresh PLU ListBox ----- */
                    A068RefreshList(hDlg, wParam, unPluType);
                }

                /* ----- PLU No. clear, Saratoga ----- */
                memset(szWork, 0x00, sizeof(szWork));
                DlgItemRedrawText(hDlg, IDD_A68_NO, szWork);

                /* ----- Mnemonic clear, Saratoga -----*/
                memset(szWork, 0x00, sizeof(szWork));
                DlgItemRedrawText(hDlg, IDD_A68_MNEMO, szWork);
                DlgItemRedrawText(hDlg, IDD_A68_ALTMNEMO, szWork);

                /* disable control button ----- */
                EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), FALSE);  /* Saratoga */
                EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL), FALSE);     /* Saratoga */

                A068CtrlButton(hDlg);

                /* ----- Reset Edit Box ----- */
                /*A068ResetEditBox(hDlg);*/
/*                SetFocus(GetDlgItem(hDlg, IDD_A68_LIST)); ,Saratoga */
                SetFocus(GetDlgItem(hDlg, IDD_A68_NO));
                fVer = LA_NORMAL;    /* falg reset */
				SendDlgItemMessage(hDlg, IDD_A68_FILTERBY_LIST, LB_SETCURSEL, -1, 0L);
                return TRUE;
            }
            return FALSE;

        case    IDD_A68_DEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                fVer = LA_NORMAL;

                /* ----- Save Current PLU Data to Temporary Area ----- */
                A068SaveRec(hDlg, IDD_A68);

                if (A068ChkDelConf(hDlg) == IDOK) {
                    /* ----- Save Preset Price / Check PLU Type ----- */
                    A068SaveChkPrc(hDlg, (LPDWORD)adwPrePrc, (LPUINT)&unPluType);
                    /* ----- Delete Selected Data from PLU File ----- */
                    if (! A068DeleteData((LPUINT)&unPluType)) {
                        /* ----- Refresh PLU ListBox ----- */
                        A068RefreshList(hDlg, wParam, unPluType);
                    }

                    A068CtrlButton(hDlg);
                    SetFocus(GetDlgItem(hDlg, IDD_A68_LIST));
                }
				SendDlgItemMessage(hDlg, IDD_A68_FILTERBY_LIST, LB_SETCURSEL, -1, 0L);
                return TRUE;
            }
            return FALSE;

        case    IDD_A68_FILTERBY_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
				A068SetRec(hDlg, IDD_A68_FILTERBY_LIST, (LPUINT)&unPluType, (LPDWORD)adwPrePrc);
				// clear the cursor selection in the main list box as it may no longer be valid.
				SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_SETCURSEL, -1, 0L);
                return TRUE;
            }
            return FALSE;

        case    IDD_A68_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                /* ----- Check No. of Current PLU Records ----- */
                if (ulCurPlu) {
                    /* ----- Set Selected PLU Data to Each Control ----- */
                    A068SetRec(hDlg, IDD_A68_LIST, (LPUINT)&unPluType, (LPDWORD)adwPrePrc);
                } else {
                    /* ----- Clear All Control ----- */
                    A068ClrRec(hDlg);
                }

                /* ----- Enable/Disable PushButtons ----- */
                A068CtrlButton(hDlg);

				// clear the cursor selection in the Filter By list box as it may no longer be valid.
				SendDlgItemMessage(hDlg, IDD_A68_FILTERBY_LIST, LB_SETCURSEL, -1, 0L);
                return TRUE;
            }
            return FALSE;

        case    IDD_A68_NO:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Enable/Disable PushButtons ----- */
                A068CtrlButton(hDlg);
                EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), FALSE);  /* Saratoga */
                EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL), FALSE);     /* Saratoga */
                return TRUE;
            }
            return FALSE;

        case    IDD_A68_DEPTNO:
        case    IDD_A68_RPTCD:
        case    IDD_A68_BONUS:
        case    IDD_A68_TBLNO:
        case    IDD_A68_GRPNO:
        case    IDD_A68_PRTPRY:
        case    IDD_A68_PPI:
		case	IDD_A68_SALES:
		case	IDD_A68_TARE:
            if (HIWORD(wParam) == EN_CHANGE) {
                /* ----- Determine Inputed Data is Out of Range ----- */
                A068ChkRng(hDlg, LOWORD(wParam));
                return TRUE;
            }
            return FALSE;

/* Saratoga Start */
			/*Search and Search UPC
			This Section handles the Search and Search UPC functionality
			in AC68.  When The Searh or Search UPC buttons are pressed
			the current list in the list box is searched to find a match.
			If the current displayed list does not contain the search
			string(number) the current list in the list box is saved to memory
			and the PLU file is searched to find the search string.
			
			If the search string is found the current list is reset to show
			the found PLU at the top and fetch the following PLUs to display 
			up to A68_ADDPOS number of PLUs.
			
			If it is not found the original list is redisplayed.*/

        case    IDD_A68_NORMAL:  //Search Button
        case    IDD_A68_EVER:	//Search UPC Button
			/* ----- Set flag ----- */
            if(LOWORD(wParam) == IDD_A68_NORMAL) {
               fVer = LA_NORMAL;   /* Normal        */
            } else {
               fVer = LA_EMOD;     /* UPC-E Version */
            }

            /* ----- Get PLU No. ----- */
			memset(szPluNo, 0x00, sizeof(szPluNo));
			bRet = A068GetPluNo(hDlg, szPluNo, TRUE);

            if(bRet != FALSE) {               /* ----- Error!!! -----*/
               /* ----- Display caution message ----- */
               LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szDesc, PEP_STRING_LEN_MAC(szDesc));
               LoadString(hResourceDll, IDS_A68_ERR_PLUNO, szMsg, PEP_STRING_LEN_MAC(szMsg));

               MessageBeep(MB_ICONEXCLAMATION);
               MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);

               /* ----- Set focus, and cursor selected ----- */
               SetFocus(GetDlgItem(hDlg, IDD_A68_NO));

               SendDlgItemMessage(hDlg, IDD_A68_NO, EM_SETSEL, 1, MAKELONG(-1, 0));
            } else {
               /* ----- find list-box  ----- */
               dwIndex = SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);

               if (dwIndex != 0) {
                  RflConvertPLU (szWork, tempPlu.auchPluNo);
                  wsPepf(szPluNo, WIDE("%14s"), szWork);

                  dwIndex = DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szPluNo));

                  if (dwIndex != LB_ERR) {
                     SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_SETCURSEL, (WPARAM)dwIndex, 0L);

                     A068SetRec(hDlg, IDD_A68_LIST, (LPUINT)&unPluType, (LPDWORD)adwPrePrc);
                     DlgItemRedrawText(hDlg, IDD_A68_NO, szWork);
                     EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL), TRUE);
                  } else {
                     memcpy(szTemp, workPlu, OP_PLU_LEN * sizeof(WCHAR));
                     memcpy(workPlu, szWork, OP_PLU_LEN * sizeof(WCHAR));

                     sRet = A068IntoPluFile(hDlg);
                     if (sRet != OP_NOT_IN_FILE) {
                        A068ResetListDesc(hDlg, IDD_A68_NEXT);
                        SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_SETCURSEL, 0, 0L);
                        A068SetRec(hDlg, IDD_A68_LIST, (LPUINT)&unPluType, (LPDWORD)adwPrePrc);
                        SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_SETCURSEL, 0, 0L);
                        EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL), TRUE);   /* Saratoga */
                        DlgItemRedrawText(hDlg, IDD_A68_NO, szWork);
                     } else {
                        memcpy(workPlu, szTemp, OP_PLU_LEN * sizeof(WCHAR));
                        DlgItemRedrawText(hDlg, IDD_A68_NO, szWork);
                     }

					 // CSMALL - fix for SR 948
					 //LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szDesc, PEP_STRING_LEN_MAC(szDesc));
					 LoadString(hResourceDll, IDS_A68_PLU_NOTEXIST, szMsg, PEP_STRING_LEN_MAC(szMsg));

					 MessageBoxPopUp(hDlg, szMsg, NULL, MB_OK | MB_ICONEXCLAMATION);
                  }
               } else {
                 RflConvertPLU (szWork, tempPlu.auchPluNo);
                 wsPepf(szPluNo, WIDE("%14s"), szWork);
                 DlgItemRedrawText(hDlg, IDD_A68_NO, szWork);
               }

               nCount = (int)SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);
               
               /*RPH 12/09/02 SR#66 Fix when Top and Next Buttons are Displayed*/
			   if (nCount < A68_PLU_LIST_MAX) {
                   /*Top Button Enable
				   If we get here we will no longer be at the first set of PLUs
				   in the PLU data set and will need the top button active to move back*/
				   EnableWindow(GetDlgItem(hDlg, IDD_A68_TOP), TRUE);

                  if (nCount < A68_ADDPOS) {
                    /* ----- Next Button Disable because we are Displaying the last
					  PLU in the data set in the current list----- */
                    EnableWindow(GetDlgItem(hDlg, IDD_A68_NEXT), FALSE);
                  } else {
                    /* ----- Next Button Enable because we are not Displaying the last
					  PLU in the data set----- */
                    EnableWindow(GetDlgItem(hDlg, IDD_A68_NEXT), TRUE);
                  }
               }

               /* ----- Enable control ----- */
               EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), TRUE);   /* Saratoga */
               /*RPH 12/09/02 SR#65 commented out to keep button active
			   while text is in the Search Field*/
			   //EnableWindow(GetDlgItem(hDlg, IDD_A68_NORMAL), FALSE);  /* Saratoga */
               //EnableWindow(GetDlgItem(hDlg, IDD_A68_EVER), FALSE);    /* Saratoga */
               SetFocus(GetDlgItem(hDlg,IDD_A68_MNEMO));               /* Saratoga */

               return FALSE;
            }
        return TRUE;

		case IDD_A68_FILTERBY_UPDATE:
			A068DisplayFiteredByPluData (hDlg, IDD_A68_FILTERBY_LIST);
			return TRUE;

		/* SR 1016  ***PDINU */
		case IDD_A68_STRINGSEARCH:
			memset(szCharString, 0x00, sizeof(szCharString));
			memset(stringCompare, 0x00, sizeof(stringCompare));
			dwIndex = SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);
			DlgItemGetText(hDlg, IDD_A68_STRING, szCharString, PLU_MAX_DIGIT + 1);

			if (_wcsicmp(szCharString, oldString) != 0){	//If a different search parameter was entered, clear displayArray.
				arrayIndex = 0;
				arrayMarker = 0;
				memset(displayArray, 0, sizeof(displayArray));
			}

			if (arrayIndex != 0 && displayArray[1] != 0){   //If there is more than one match, loop through the results
				A068DisplayArrays(displayArray[arrayMarker], hDlg, (LPUINT)&unPluType, (LPDWORD)adwPrePrc);
				arrayMarker++;
				if (arrayMarker == arrayIndex){			//used for looping through displayArray
					arrayMarker = 0;
					arrayIndex = 0;
				}
				return TRUE;
			}

			if (wcslen(szCharString) == 0){    //No user input
				LoadString(hResourceDll, IDS_A68_ERR_PLUNO, szDesc, PEP_STRING_LEN_MAC(szDesc));
                LoadString(hResourceDll, IDS_A68_ERR_NOVAL, szMsg, PEP_STRING_LEN_MAC(szMsg));

				MessageBeep(MB_ICONEXCLAMATION);
				MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);
				return TRUE;
			}
			
			memset(displayArray, 0, sizeof(displayArray));
			memset(oldString, 0x00, sizeof(oldString));
			memcpy(oldString, szCharString, OP_PLU_LEN * sizeof(WCHAR));
			
			/*  This part of the code does the searching.  The outer FOR 
				statement loops through all of the PLU's in the list box
				while the inner FOR loop finds a match.  The matches are
				stored in displayArray where the code will use that to
				loop through the found matches everytime the search button
				is pressed with the same parameter.    ***PDINU
			*/
			for (i = 0; i <= dwIndex; i++){
				memset(&szTempBuff, 0x00, sizeof(szTempBuff));
				DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_GETTEXT, i, (LPARAM)(szTempBuff));
				pszChar = &szTempBuff[17];
				memcpy(stringCompare, pszChar, OP_PLU_LEN * sizeof(WCHAR));
				
				checkCount = 0;
				for (j = 0; j < wcslen(stringCompare); j++){
					if (tolower(stringCompare[j]) == tolower(szCharString[checkCount]) && arrayIndex < sizeof(displayArray)/sizeof(displayArray[0]) - 1){   //If there is a perfect match
						checkCount++;
						if (checkCount == wcslen(szCharString)){   //If there is a perfect match
							displayArray[arrayIndex++] = (i + 1);
							break;
						}
					} else {
						j -= checkCount;
						checkCount = 0;
					}
				}
				if ((i >= dwIndex) && (displayArray[0] != 0)){   //saftey switch incase only one item was found
					A068DisplayArrays(displayArray[arrayMarker], hDlg, (LPUINT)&unPluType, (LPDWORD)adwPrePrc);
					arrayMarker++;
					break;
				}
			}

			if( displayArray[0] == 0){   //ERROR
				/* ----- Display caution message ----- */
               LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szDesc, PEP_STRING_LEN_MAC(szDesc));
               LoadString(hResourceDll, IDS_A68_ERR_PLUMNEM, szMsg, PEP_STRING_LEN_MAC(szMsg));

               MessageBeep(MB_ICONEXCLAMATION);
               MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);

               /* ----- Set focus, and cursor selected ----- */
               SetFocus(GetDlgItem(hDlg, IDD_A68_STRING));
               SendDlgItemMessage(hDlg, IDD_A68_STRING, EM_SETSEL, 1, MAKELONG(-1, 0));
			}

			SendDlgItemMessage(hDlg, IDD_A68_FILTERBY_LIST, LB_SETCURSEL, -1, 0L);
			return TRUE;
/* Saratoga End */

        case    IDOK:
        case    IDCANCEL:
			/* Clears the Mnemonic search field for a new search  ***PDINU */
			memset(displayArray, 0, sizeof(displayArray));
			memset(oldString, 0x00, sizeof(oldString));
			arrayIndex = 0;

            if (HIWORD(wParam) == BN_CLICKED) {
                /* ----- Finalize DialogBox ----- */
                A068FinDlg(hDlg, wParam, (LPHGLOBAL)&hGlbDesc);
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
        }
        return FALSE;

    default:
        return FALSE;
    }
}

/* ***PDINU
* ===========================================================================
**  Synopsis    :   VOID    A068DisplayArrays()
*
**  Input       :  	int		index		-	index to display
*					HWND    hDlg        -   Window Handle of a DialogBox
*                   LPUINT  lpunPluType -   Address of PLU Type Data Area
*                   LPDWORD lpadwPrePrc -   Address of Preset Price Array 
*
**  Return      :   No return value.
*
**  Description :
*       This procedure will recieve an index containing 
*		a matching PLU item found by the Mnemonic Search feature.
* ===========================================================================
*/
VOID	A068DisplayArrays(DWORD index, HWND hDlg, LPUINT lpunPluType, LPDWORD lpadwPrePrc)
{
	
	SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_SETCURSEL, (WPARAM)(index - 1), 0L);
	A068SetRec(hDlg, IDD_A68_LIST, (LPUINT)&lpunPluType, (LPDWORD)lpadwPrePrc);
	EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL), TRUE);

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    WINAPI  A068CtrlDlgProc()
*
**  Input       :   HWND    hDlg    -   Window Handle of DialogBox
*                   WORD    wMsg    -   Dispacthed Message
*                   WORD    wParam  -   16 Bits of Additional Message
*                   LONG    lParam  -   32 Bits of Additional Message
*
**  Return      :   BOOL    TRUE    -   The Procedure Processes the Message.
*                           FALSE   -   It Does Not Process the Message.
*
**  Description :
*       This is a dialogbox procedure for 
*                       PLU Maintenance Control Code.
* ===========================================================================
*/
BOOL    WINAPI  A068CtrlDlgProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wMsg) {

    case    WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, (WPARAM)hResourceFont, MAKELPARAM(TRUE, 0));

        /* ----- Set Current Selected PLU Data to Each Controls ----- */
        A068InitData(hDlg, IDD_A68_CTRL);
		
        SetFocus(GetDlgItem(hDlg, IDD_A68_NO));

        return TRUE;

	case WM_SETFONT:
		if (hResourceFont) {
			int j;
			for(j=IDD_A68_CTLDESC; j<=IDD_A68_TYPEGP; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			for(j=IDD_A68_STS01; j<=IDD_A68_STS36; j++)
			{
				SendDlgItemMessage(hDlg, j, WM_SETFONT, (WPARAM)hResourceFont, 0);
			}
			SendDlgItemMessage(hDlg, IDOK, WM_SETFONT, (WPARAM)hResourceFont, 0);
			SendDlgItemMessage(hDlg, IDCANCEL, WM_SETFONT, (WPARAM)hResourceFont, 0);
		}
		return FALSE;

    case    WM_COMMAND:
        switch (LOWORD(wParam)) {

        case    IDOK:
        case    IDCANCEL:
            if (HIWORD(wParam) == BN_CLICKED) {
                if (LOWORD(wParam) == IDOK) {
                    /* ----- Save Current Inputed Data to Temporary ----- */
                    A068SaveRec(hDlg, IDD_A68_CTRL);
                }

                EndDialog(hDlg, LOWORD(wParam));

                return TRUE;
            }
			break;

		case IDD_A68_STS16:    // Checkbox for Use Departmental Control Code, set, or use PLU specific Control Code, not set.
			{
				int i;
				UINT  bIsChecked = IsDlgButtonChecked (hDlg, IDD_A68_STS16);
				BOOL   bIsAdjectivePlu = (tempPlu.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP;

				// up to but not including the Adjective Group
				for (i = IDD_A68_STS01; i <= IDD_A68_STS20; i++) {
					if (i == IDD_A68_STS16) continue;    // do not modify the Use Departmental Control Code checkbox itself!
					// if Departmental checked then disable and vice versa so invert Departmental Control Code setting.
					EnableWindow(GetDlgItem (hDlg, i), ! bIsChecked);
				}
				if (bIsAdjectivePlu) {
					for (i = IDD_A68_STS21; i <= IDD_A68_STS24; i++) {
						// if Departmental checked then disable and vice versa so invert Departmental Control Code setting.
						EnableWindow(GetDlgItem (hDlg, i), ! bIsChecked);
					}
				}
				// handle the remote printers #5 - #8 along with foodstamp and WIC
				for (i = IDD_A68_STS30; i <= IDD_A68_STS35; i++) {
					EnableWindow(GetDlgItem (hDlg, i), ! IsDlgButtonChecked (hDlg, IDD_A68_STS16));
				}
			}
			return TRUE;
			break;

		case    IDD_A005_PREC01:
			CheckDlgButton(hDlg, IDD_A005_PREC02, FALSE);
			CheckDlgButton(hDlg, IDD_A005_PREC01, TRUE);
			return TRUE;

        case    IDD_A005_PREC02:
			CheckDlgButton(hDlg, IDD_A005_PREC02, TRUE);
			CheckDlgButton(hDlg, IDD_A005_PREC01, FALSE);
			return TRUE;

		case	IDD_A68_GIFTCARD:
			if (IsDlgButtonChecked(hDlg, IDD_A68_GIFTCARD)) {
				CheckDlgButton(hDlg, IDD_A68_GIFTCARD, FALSE);
				CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, FALSE);
				CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, FALSE);
			} else {
				CheckDlgButton(hDlg, IDD_A68_GIFTCARD, TRUE);
				CheckDlgButton(hDlg, IDD_A68_EPAYMENT, FALSE);
				// If Reload is not set then we will force Issue as it must be one or the other
				if (! IsDlgButtonChecked(hDlg, IDD_A68_GIFT_RELOAD)) {
					CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, TRUE);
				}
			}
			return TRUE;

		case	IDD_A68_EPAYMENT:
			if (IsDlgButtonChecked(hDlg, IDD_A68_EPAYMENT)) {
				CheckDlgButton(hDlg, IDD_A68_EPAYMENT, FALSE);
				CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, FALSE);
				CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, FALSE);
			} else {
				CheckDlgButton(hDlg, IDD_A68_GIFTCARD, FALSE);
				CheckDlgButton(hDlg, IDD_A68_EPAYMENT, TRUE);
				// If Reload is not set then we will force Issue as it must be one or the other
				if (! IsDlgButtonChecked(hDlg, IDD_A68_GIFT_RELOAD)) {
					CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, TRUE);
				}
			}
			return TRUE;

		case	IDD_A68_GIFT_ISSUE:
			CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, TRUE);
			CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, FALSE);
			return TRUE;

		case	IDD_A68_GIFT_RELOAD:
			CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, TRUE);
			CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, FALSE);
			return TRUE;
        }
    }
    return FALSE;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ResetListDesc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   ULONG   ulStart -   
*                   ULONG   ulEnd   -   
*
**  Return      :   No return value.
*
**  Description :
*    PLU Data from PLU file. And then it sets them to listbox.
* ===========================================================================
*/
BOOL A068ResetListDesc(HWND hDlg, WPARAM wBtnID)
{

#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
    ECMCOND EcmCond;      /* condition   */
#endif

    RECPLU  RecPlu;       /* record data */
    ECMRANGE EcmRange;

    USHORT  usHandle;     /* plu handle  */

    WCHAR    szDesc[128];
    ULONG   ulCounter = 0L;
    BOOL    bStat = FALSE;
    WCHAR    szStartNo[OP_PLU_LEN+1];
    WCHAR    szWorkNo[OP_PLU_LEN+1];
    USHORT  usStat;
    USHORT  usRead;
    USHORT  usI, usII;
    int     nCount;

    /* ----- Clear Control Code ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_RESETCONTENT, 0, 0L);

    /* initialize */
    memset(szStartNo, 0x00, sizeof(szStartNo)); /* Find Start Plu No. */
    memset(szWorkNo, 0x00, sizeof(szWorkNo));   /* Plu No. Work area  */
    memset(&EcmRange, 0, sizeof(ECMRANGE));           /* ECMCOND   */

    if (wBtnID == IDD_A68_NEXT) {
       usII = 0;
       memset(szWorkNo, 0x00, sizeof(szWorkNo));

       for (usI = 0; usI < OP_PLU_LEN; usI++) {
           if ((workPlu[usI] != 0x00) && (workPlu[usI] != 0x20)) {
              szWorkNo[usII] = workPlu[usI];
              usII++;
           }
       }

#if 0
       /* ----- Get PLU Length ----- */
       nLen = wcslen(szWorkNo);
       if (nLen == OP_PLU_LEN) {
          memcpy(szStartNo, szWorkNo, OP_PLU_LEN * sizeof(WCHAR));
       } else {
          wsPepf(szStartNo, WIDE("%014s"), szWorkNo);
          szStartNo[OP_PLU_LEN - 1] = '0';
       }
#endif
	   {
			SHORT          sStat;
			LABELANALYSIS  Label;

			memset(&Label, 0x00, sizeof(LABELANALYSIS));
			wcscpy (Label.aszScanPlu, szWorkNo);

			Label.fchModifier = LA_NORMAL;   /* Normal        */
			sStat = RflLabelAnalysis(&Label);
			memcpy(EcmRange.itemFrom.aucPluNumber, Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));
	   }
    } else {
		{
			int i;
			for(i = 0; i < STD_PLU_NUMBER_LEN; i++){
				EcmRange.itemFrom.aucPluNumber[i] = WIDE('0');
			}
		}
    }

    {
		int j;
		for(j = 0; j < STD_PLU_NUMBER_LEN; j++){
			EcmRange.itemTo.aucPluNumber[j] = WIDE('9');
		}
	}
    EcmRange.usReserve = 0;
    EcmRange.fsOption = (REPORT_ACTIVE | REPORT_INACTIVE);

    if ((usStat = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &usHandle, &EcmRange)) != SPLU_COMPLETED)
    {
       memset(szDesc, 0x00, sizeof(szDesc));
       LoadString(hResourceDll, IDS_A68_ERR_UNKOWN, szDesc, PEP_STRING_LEN_MAC(szDesc));

       A068DispErr(hDlg, szDesc, MB_OK | MB_ICONEXCLAMATION);
       bStat = FALSE;
       return bStat;
    }

    do{
      if ((usStat = PluReportRecord(usHandle, &RecPlu, &usRead)) == SPLU_COMPLETED) {
        switch(usStat){
          case  SPLU_REC_NOT_FOUND:
          case  SPLU_END_OF_FILE:
            bStat = FALSE;
            break;

          case  SPLU_COMPLETED:
            if(ulCounter < A68_ADDPOS) {
               A068ResetDesc(hDlg, &RecPlu);
               bStat = TRUE;
            } else {
               bStat = FALSE;
            }
            break;

          default:
            bStat = FALSE;
            break;
        }

        ulCounter++;
      }
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
    }while((usStat != SPLU_END_OF_FILE) || (bStat == TRUE));
#endif
    }while(usStat == SPLU_COMPLETED && bStat == TRUE);

    PluExitCritMode(usHandle, 0);

    /* ----- save plu No.(last Plu No. listbox of Plu) ----- */
    nCount = (int)SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);

    DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_GETTEXT, (WORD)(nCount - 1), (LONG)szDesc);
    memset(workPlu, 0x00, sizeof(workPlu));
	memcpy(workPlu, szDesc, OP_PLU_LEN * sizeof(WCHAR));

    return bStat;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ResetDesc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   RECPLU  pRecPlu -   
*
**  Return      :   No return value
*
**  Description :
*       This procedure sets the data in the dialogbox(When Initial).
* ===========================================================================
*/
VOID    A068ResetDesc(HWND hDlg, RECPLU* pRecPlu)
{
    WCHAR    szWork[PLU_MAX_DIGIT + 1] = {0};
    USHORT  usLen = 0;

    RflConvertPLU (szWork, pRecPlu->aucPluNumber);
    usLen = (USHORT)wcslen(szWork);
    if (usLen) {
		WCHAR    szName[64] = {0}, szDesc[64] = {0};
		WCHAR    szcom[] = WIDE(" : ");

        /* ----- Replace Double Key Code (0x12 -> '~') ----- */
         PepReplaceMnemonic (pRecPlu->aucMnemonic, szName, (A68_MNEMO_SIZE ), PEP_MNEMO_READ);
		 szName[A68_MNEMO_SIZE] = 0;

        /* ----- Create Description for Insert ListBox ----- */
        wsPepf(szDesc, WIDE("%14s"), szWork);
        szDesc[OP_PLU_LEN] = 0x00;
        wcscat(szDesc, szcom);
        wcscat(szDesc, szName);

        /* ----- Insert Created Description to PLU ListBox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
    }
}

/*
* ===========================================================================
**  Synopsis:   static  BOOL    A068GetPluNo();
*
**  Input   :   HWND        hDlg      - handle of a dialogbox procedure
*               LPSTR       lpszPluNo - address of PLU No.
*               BOOL        bType     - 
*
**  Return  :   TRUE                  - user process is executed
*               FALSE                 - invalid PLU No.
*
**  Description:
*       This function get PLU No. as PLU formated data.
* ===========================================================================
*/
BOOL   A068GetPluNo(HWND hDlg, WCHAR* lpszPluNo, BOOL bType)
{
    LABELANALYSIS Label;
    BOOL          sStat;
    WCHAR    szMsg[256];
    WCHAR    szDesc[128];

    /* ----- Initialize buffer ----- */
/*    memset(lpszPluNo, 0x00, OP_PLU_LEN + 1); */
    memset(&Label, 0x00, sizeof(LABELANALYSIS));

    /* ----- Set flag ----- */
    if(fVer == LA_NORMAL) {
       Label.fchModifier = LA_NORMAL;   /* Normal        */
    } else {
       Label.fchModifier = LA_EMOD;     /* UPC-E Version */
    }

    if(bType != FALSE) {
       /* ----- Get PLU No. ----- */
       DlgItemGetText(hDlg, IDD_A68_NO, Label.aszScanPlu, PLU_MAX_DIGIT + 1);
    } else {
      memcpy(Label.aszScanPlu, lpszPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));
    }

    /* ----- Analyze PLU No. ----- */
    sStat = RflLabelAnalysis(&Label);

    if(sStat == LABEL_ERR) {
       return TRUE;
    } else {                   /* sStat == LABEL_OK */
       if(Label.uchType == LABEL_RANDOM) {
          if(Label.uchLookup == LA_EXE_LOOKUP) {
             memcpy(tempPlu.auchPluNo, Label.aszMaskLabel, OP_PLU_LEN * sizeof(WCHAR));

             if(fVer == LA_EMOD) {
                memcpy(lpszPluNo, Label.aszMaskLabel, OP_PLU_LEN * sizeof(WCHAR));
             } else {
                /* ----- Analyses PLU types and set PLU number ----- */
                if(bType != FALSE) {
                   RflConvertPLU (lpszPluNo, Label.aszMaskLabel);
                } else {
                   memcpy(lpszPluNo, Label.aszMaskLabel, OP_PLU_LEN * sizeof(WCHAR));
                }
             }
             return FALSE;

          } else {
             /* ----- Display caution message ----- */
             memset(szDesc, 0x00, sizeof(szDesc));
             memset(szMsg, 0x00, sizeof(szMsg));

             LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szDesc, PEP_STRING_LEN_MAC(szDesc));
             LoadString(hResourceDll, IDS_A68_LABEL, szMsg, PEP_STRING_LEN_MAC(szMsg));

             MessageBeep(MB_ICONEXCLAMATION);
             MessageBoxPopUp(hDlg, szMsg, szDesc, MB_OK | MB_ICONEXCLAMATION);

             /* ----- Set focus, and cursor selected ----- */
             SetFocus(GetDlgItem(hDlg, IDD_A68_NO));

             SendDlgItemMessage(hDlg, IDD_A68_NO, EM_SETSEL, 1, MAKELONG(-1, 0));

             return FALSE;
          }
       } else if (Label.uchType == LABEL_COUPON) {
           return TRUE;
       } else {

             memcpy(tempPlu.auchPluNo, Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));

             if(fVer == LA_EMOD) {
				memcpy(lpszPluNo, Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));
             } else {
                /* ----- Analyses PLU types and set PLU number ----- */
                memset(lpszPluNo, 0x00, OP_PLU_LEN * sizeof(WCHAR));

                if(bType != FALSE) {
                   RflConvertPLU (lpszPluNo, Label.aszLookPlu);
                } else {
                   memcpy(lpszPluNo, Label.aszLookPlu, OP_PLU_LEN * sizeof(WCHAR));
                }
             }

             return FALSE;
       }
    }

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ResetEditBox()
*
*  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
**
**  Return      :   No return value.
*
**  Description :
*       This procedure Editbox reset
*
* ===========================================================================
*/
VOID    A068ResetEditBox(HWND hDlg)
{
    WORD   wID;
    WCHAR   szBuffer[OP_PLU_LEN + 1];

    SetDlgItemInt(hDlg, IDD_A68_DEPTNO,  (WORD)A68_NO_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_RPTCD,   (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_BONUS,   (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_TBLNO,   (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_GRPNO,   (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_PRTPRY,  (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_PPI,     (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_MULTI,   (WORD)A68_NO_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_LINK,    (WORD)A68_DATA_MIN, FALSE);
    SetDlgItemInt(hDlg, IDD_A68_SALES,   (WORD)A68_DATA_MIN, FALSE);
/* ### ADD Saratoga FamilyCode (V1_0.02)(051800) */
    SetDlgItemInt(hDlg, IDD_A68_FAMILY,  (WORD)A68_DATA_MIN, FALSE);
	SetDlgItemInt(hDlg, IDD_A68_COLOR,   (WORD)A68_DATA_MIN, FALSE);

    memset(szBuffer, 0x00, sizeof(szBuffer));

    for (wID = IDD_A68_PRESET1; wID <= IDD_A68_PRESET5; wID++) {
        DlgItemRedrawText(hDlg, wID, szBuffer);
    }

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ReadRec()
*
**  Input       :   No input value.
*
**  Return      :   No return value.
*
**  Description :
*       This procedure retrieves number of Department & PLU records from
*   Parameter File (Program Mode No. 2). And it sets them to static area.
* ===========================================================================
*/
VOID    A068ReadRec()
{
    FLEXMEM     ParaFlex[MAX_FLXMEM_SIZE];
    USHORT      usRetLen;
    
    /* ----- read the filexible memory data of PLU & Dept. Max ----- */
    ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)&ParaFlex, sizeof(ParaFlex), 0, (USHORT *)&usRetLen);

    /* ----- Set No. of Department Memory to Static Area ----- */
    unDeptMax = (UINT)ParaFlex[FLEX_DEPT_ADR - 1].ulRecordNumber;

    /* ----- Set No. of PLU Memory to Static Area ----- */
    ulPLUMax = ParaFlex[FLEX_PLU_ADR  - 1].ulRecordNumber;

}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A068AllocMem()
*
**  Input       :   LPHGLOBAL   lphDesc -   Address of Global Heap Handle
*
**  Return      :   BOOL    TRUE    -   Memory Allocation is Failed.
*                           FALSE   -   Momory Allocation is Success.
*
**  Description :
*       This function allocates the data buffer from the global heap.
*   It returns TRUE, if it failed in momory allocation. Otherwise it returns 
*   FALSE.
* ===========================================================================
*/
BOOL    A068AllocMem(LPHGLOBAL lphDesc)
{
    BOOL        fRet = FALSE;
    WORD        wI;
    LPA68DESC   lpWork;

    /* ----- Allocate Memory from Global Heap ----- */
    *lphDesc = GlobalAlloc(GHND, (sizeof(A68DESC) * A68_CTRL_NUM) * 2);

    if (! *lphDesc) {
        fRet = TRUE;
    } else {
        /* ----- Lock the Allocated Area ----- */
        lpPluDesc = (LPA68DESC)GlobalLock(*lphDesc);

        for (wI = 0, lpWork = lpPluDesc; wI < A68_CTRL_NUM; wI++, lpWork++) {
            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_A68_STS_001 + wI, lpWork->szCtrlOFF, A68_BUFF_LEN);
            LoadString(hResourceDll, IDS_A68_STS_101 + wI, lpWork->szCtrlON,  A68_BUFF_LEN);
        }
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068InitData()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wDlgID  -   Initialize DialogBox ID
*
**  Return      :   No return value.
*
**  Description :
*       This procedure initilaizes configulation of dialogbox. If dialogbox is
*   main dialogbox, it creates the backup file, and loads initial PLU Data from
*   PLU file. And then it sets them to listbox.
* ===========================================================================
*/
VOID    A068InitData(HWND hDlg, WPARAM wDlgID)
{
    FLEXMEM aFlexMemData[MAX_FLXMEM_SIZE];
    USHORT  usRetLen;
    short   nRet = OP_PERFORM;

    WCHAR   szFile[PEP_FILE_BACKUP_LEN + 1], szBuff[128], szDesc[128];
    UINT    wI, wJ, wID;
    ULONG   ulCur = 0L;
    BOOL    fCheck = TRUE;

    UCHAR   uchI;
    WCHAR    szPlu[OP_PLU_LEN + 1];

    /* ----- Determine Current Initialize DialogBox ID ----- */
    if (LOWORD(wDlgID) == IDD_A68) {       /* Main PLU DialogBox */
		BYTE    bTaxCheck1, bTaxCheck2;

		/* ----- Get Current Tax Mode Status from Prog. #1 (MDC) ----- */
		bTaxCheck1 = ParaMDCCheck(MDC_TAX_ADR, ODD_MDC_BIT0);
		bTaxCheck2 = ParaMDCCheck(MDC_TAXINTL_ADR, ODD_MDC_BIT0);

        uchTaxMode = (UCHAR)(!(bTaxCheck1) ? A68_TAX_US  : !(bTaxCheck2) ? A68_TAX_CAN : A68_TAX_VAT);

        /* ----- Set Fixed Font to ListBox / Mnemonic EditText ----- */
        PepSetFont(hDlg, IDD_A68_LIST);
        PepSetFont(hDlg, IDD_A68_MNEMO);
        PepSetFont(hDlg, IDD_A68_ALTMNEMO);

        /* ----- Check Delete Confirmation Button ----- */
        if (fDelConf != A68_DELCONF) {
            fCheck = FALSE;
        }
        CheckDlgButton(hDlg, IDD_A68_CONF, fCheck);

        /* ----- Load PLU File Name from Resorece ----- */
        LoadString(hResourceDll, IDS_FILE_NAME_PLU, szFile, PEP_STRING_LEN_MAC(szFile));

        /* ----- Create Backup File for Cancel Key ----- */
        PepFileBackup(hDlg, szFile, PEP_FILE_BACKUP);

        /* ----- Load PLU Index File Name from Resorce, Saratoga ----- */
		LoadString(hResourceDll, IDS_FILE_NAME_PLUINDEX, szFile, PEP_STRING_LEN_MAC(szFile));

        /* ----- Get PLU Index file information, Saratoga ----- */
        PepIndexFileBackup(hDlg, szFile, PEP_FILE_BACKUP);

        /* ----- Limit Max. Length of Input Data ----- */
        SendDlgItemMessage(hDlg, IDD_A68_NO,      EM_LIMITTEXT, A68_NO_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_DEPTNO,  EM_LIMITTEXT, A68_DEPT_LEN,  0L);
        SendDlgItemMessage(hDlg, IDD_A68_RPTCD,   EM_LIMITTEXT, A68_RPT_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_BONUS,   EM_LIMITTEXT, A68_BTTL_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PRESET1, EM_LIMITTEXT, A68_PRC_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PRESET2, EM_LIMITTEXT, A68_PRC_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PRESET3, EM_LIMITTEXT, A68_PRC_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PRESET4, EM_LIMITTEXT, A68_PRC_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PRESET5, EM_LIMITTEXT, A68_PRC_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_MNEMO,   EM_LIMITTEXT, A68_MNEMO_SIZE, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_ALTMNEMO,EM_LIMITTEXT, A68_MNEMO_SIZE, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_TBLNO,   EM_LIMITTEXT, A68_TBLNO_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_GRPNO,   EM_LIMITTEXT, A68_GRPNO_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PRTPRY,  EM_LIMITTEXT, A68_PRTPRY_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_PPI,     EM_LIMITTEXT, A68_PPI_LEN, 0L);
/* Saratoga */
        SendDlgItemMessage(hDlg, IDD_A68_MULTI,   EM_LIMITTEXT, A68_MULTI_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_LINK,    EM_LIMITTEXT, A68_LINK_LEN, 0L);
        SendDlgItemMessage(hDlg, IDD_A68_SALES,   EM_LIMITTEXT, A68_SALES_LEN, 0L);
/* ### ADD Saratoga FamilyCode (V1_0.02)(051800) */
        SendDlgItemMessage(hDlg, IDD_A68_FAMILY,  EM_LIMITTEXT, A68_FAMILY_LEN, 0L);
		SendDlgItemMessage(hDlg, IDD_A68_COLOR,  EM_LIMITTEXT, A68_COLOR_LEN, 0L);
		SendDlgItemMessage(hDlg, IDD_A68_TARE,  EM_LIMITTEXT, A68_COLOR_LEN, 0L);

        /* ----- Reset PLU Work Area Counter ----- */
        tempPlu.ulCounter = 0L;
		{
			int i;
			for(i = 0; i < STD_PLU_NUMBER_LEN; i++){
				wcscpy(&szPlu[i], WIDE("0"));
			}
		}
		//memset(szPlu, '0', sizeof(szPlu));    /* Saratoga */

        do {
            /* ----- Load PLU Records from PLU File until End of File ----- */
            nRet = OpPluAllRead(&tempPlu, 0);

            /* ----- Check End of PLU File ----- */
            if (nRet == OP_FILE_NOT_FOUND || nRet == OP_EOF) {
                break;
            } else {
                /* ----- Check Loaded PLU Adjective No. is 1 ----- */
                if (tempPlu.uchPluAdj == A68_ADJ_NO1) {
                    if( ulCur < A68_PLU_LIST_MAX)
						A068SetListDesc(hDlg, IDD_A68_LIST, &tempPlu);
                }

                /* ----- Increment Current No. of PLU Records ----- */
               /* ulCur++; *//* Saratoga */

                if (memcmp(tempPlu.auchPluNo, szPlu, OP_PLU_LEN * sizeof(WCHAR)) != 0) { /* Saratoga */
                   ulCur++;                                                /* Saratoga */
                   memcpy(szPlu, tempPlu.auchPluNo, OP_PLU_LEN * sizeof(WCHAR));         /* Saratoga */
                }                                                          /* Saratoga */
            }
        } while (nRet == OP_PERFORM);


        /* 09/14/00 */
        if ((nRet != OP_PERFORM) && (nRet != OP_EOF) && (nRet != OP_FILE_NOT_FOUND)) {
           /* ----- Display caution message ----- */
           LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szDesc, PEP_STRING_LEN_MAC(szDesc));
           LoadString(hResourceDll, IDS_A68_ERR_PLUREC, szBuff, PEP_STRING_LEN_MAC(szBuff));

           MessageBeep(MB_ICONEXCLAMATION);
           MessageBoxPopUp(hDlg, szBuff, szDesc, MB_OK | MB_ICONEXCLAMATION);
        }

/* ADD Saratoga */

        /* ------ Next/Top Button Initialize ----- */
        if (ulCur > A68_PLU_LIST_MAX) {
           EnableWindow(GetDlgItem(hDlg, IDD_A68_NEXT), TRUE);
           EnableWindow(GetDlgItem(hDlg, IDD_A68_TOP), FALSE);
        } else {
           EnableWindow(GetDlgItem(hDlg, IDD_A68_NEXT), FALSE);
           EnableWindow(GetDlgItem(hDlg, IDD_A68_TOP), FALSE);
        }

        /* ----- Check Current No. of Department (Prog. #2) ----- */
        ParaAllRead(CLASS_PARAFLEXMEM, (UCHAR *)&aFlexMemData, sizeof(aFlexMemData), 0, (USHORT *)&usRetLen);

        /* ----- Store Number of Department Record ----- */
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
		/* ###DEL Saratoga */
        wDeptMax = (WORD)aFlexMemData[FLEX_DEPT_ADR - 1].ulRecordNumber;

        wDeptMax = (WORD)((wDeptMax != 0) ? wDeptMax : A68_DEPT_MAX);
#endif
        wDeptMax = 9999;

        /* ----- Set Department Data Range to Static Text ----- */
        LoadString(hResourceDll, IDS_A68_DEPT_MAX, szBuff, PEP_STRING_LEN_MAC(szBuff));

        wsPepf(szDesc, szBuff, wDeptMax);

        DlgItemRedrawText(hDlg, IDD_A68_DEPT_MAX, szDesc);

        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_A68_MAX, szBuff, PEP_STRING_LEN_MAC(szBuff));

        /* ----- Create Max. Record No. String ----- */
        wsPepf(szDesc, szBuff, ulPLUMax);

        /* ----- Set Max. PLU Record No. (Prog. #2) ----- */
        DlgItemRedrawText(hDlg, IDD_A68_MAX, szDesc);

        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_A68_CUR, szBuff, PEP_STRING_LEN_MAC(szBuff));

        /* ----- Create Current Record No. String ----- */
        wsPepf(szDesc, szBuff, ulCur);

        /* ----- Set Current PLU Record No. (Prog. #2) ----- */
        DlgItemRedrawText(hDlg, IDD_A68_CUR, szDesc);

        /* ----- Store Current Exist PLU Data to Static Area ----- */
        ulCurPlu = ulCur;

        /* ----- Set Default Data to Each Edit Controls ----- */
        SetDlgItemInt(hDlg, IDD_A68_DEPTNO, A68_DEPT_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_RPTCD,  A68_DATA_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_BONUS,  A68_DATA_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_TBLNO,  A68_DATA_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_GRPNO,  A68_DATA_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_PRTPRY, A68_DATA_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_PPI,    A68_DATA_MIN, FALSE);
/* Saratoga */
        SetDlgItemInt(hDlg, IDD_A68_MULTI,  A68_NO_MIN,   FALSE);
        SetDlgItemInt(hDlg, IDD_A68_LINK,   A68_DATA_MIN, FALSE);
        SetDlgItemInt(hDlg, IDD_A68_SALES,  A68_DATA_MIN, FALSE);
/* ### ADD Saratoga FamilyCode (V1_0.02)(051800) */
        SetDlgItemInt(hDlg, IDD_A68_FAMILY,  A68_DATA_MIN, FALSE);
		SetDlgItemInt(hDlg, IDD_A68_COLOR,	A68_DATA_COLORPALETTE_MIN, FALSE);
		SetDlgItemInt(hDlg, IDD_A68_TARE, A68_DATA_MIN, FALSE);

        A068CtrlButton(hDlg);

        EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL),    FALSE);   /* Saratoga */
        EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), FALSE);   /* Saratoga */

        memset(&tempPlu, 0x00, sizeof( PLUIF ));

    } else {                    /* PLU Control Code DialogBox */

        /* ----- Initialize Description ----- */
        /* ----- set common description  ----- */
        LoadString(hResourceDll, IDS_A68_TYPEGP, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemRedrawText(hDlg, IDD_A68_TYPEGP, szDesc);

            /* ----- tax mode check ----- */
        if (uchTaxMode == A68_TAX_US) { /* US Mode */
            /* ----- set string ----- */
            LoadString(hResourceDll, IDS_A68_CTL_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
            WindowRedrawText(hDlg, szDesc);

            for (wI = 0, wJ = 0; (IDS_A68_CTL_01 + wI) <= IDS_A68_CTL_33; wI++, wJ++) {
                LoadString(hResourceDll, IDS_A68_CTL_01 + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));

                /* ----- except not used no. ----- */
                if ((IDD_A68_STS01 + wJ) == IDD_A68_ADJKEY) {
                    wJ ++;
                }
                DlgItemRedrawText(hDlg, IDD_A68_STS01 + wJ, szDesc);
            }

            LoadString(hResourceDll, IDS_A68_CTLDESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_CTLDESC, szDesc);

            /* ### ADD Saratoga Food Stamp (052300) */
            /* ----- Food Stamp (US only) ---- */
            LoadString(hResourceDll, IDS_A68_CTL_34, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_STS34, szDesc);
        } else if (uchTaxMode == A68_TAX_CAN) { /* Canadian Mode */
            /* ----- set string ----- */
            LoadString(hResourceDll, IDS_A68_CAN_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
            WindowRedrawText(hDlg, szDesc);

            for (wI = 0, wJ = 0; (IDS_A68_CTL_01 + wI) <= IDS_A68_CTL_33; wI++, wJ++) {
                
                LoadString(hResourceDll, IDS_A68_CAN_01 + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
                
                /* ----- except not used no. ----- */
                if ((IDD_A68_STS01 + wJ) == IDD_A68_STS09) {
                    wJ += (IDD_A68_STS12) - (IDD_A68_STS09);
                }
                
                if ((IDD_A68_STS01 + wJ) == IDD_A68_ADJKEY) {
                    wJ ++;
                }
                    
                DlgItemRedrawText(hDlg, IDD_A68_STS01 + wJ, szDesc);
            }
            
            for (wI = 0; (IDS_A68_RADIO1 + wI) <= IDS_A68_RADIOA; wI++) {
                LoadString(hResourceDll, IDS_A68_RADIO1 + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
                DlgItemRedrawText(hDlg, IDD_A68_RADIO1 + wI, szDesc);
            }

            LoadString(hResourceDll, IDS_A68_CANDESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_CANDESC, szDesc);
            LoadString(hResourceDll, IDS_A68_CANTAX, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_CANTAX, szDesc);

        } else {    /* VAT Mode */
            /* ----- set string ----- */
            LoadString(hResourceDll, IDS_A68_CAN_CAPTION, szDesc, PEP_STRING_LEN_MAC(szDesc));
            WindowRedrawText(hDlg, szDesc);

            for (wI = 0, wJ = 0; (IDS_A68_CTL_01 + wI) <= IDS_A68_CTL_33; wI++, wJ++) {
                LoadString(hResourceDll, IDS_A68_CAN_01 + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
                /* ----- except not used no. ----- */
                if ((IDD_A68_STS01 + wJ) == IDD_A68_STS09) {
                    wJ += (IDD_A68_STS12) - (IDD_A68_STS09);
                }
                
                if ((IDD_A68_STS01 + wJ) == IDD_A68_ADJKEY) {
                    wJ ++;
                }
                DlgItemRedrawText(hDlg, IDD_A68_STS01 + wJ, szDesc);
            }
            
            for (wI = 0; (IDS_A68_VATRADIO1 + wI) <= IDS_A68_VATRADIO4; wI++) {
                LoadString(hResourceDll, IDS_A68_VATRADIO1 + wI, szDesc, PEP_STRING_LEN_MAC(szDesc));
                DlgItemRedrawText(hDlg, IDD_A68_VATRADIO1 + wI, szDesc);
            }

            LoadString(hResourceDll, IDS_A68_VATSERVICE, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_VATSERVICE, szDesc);

            LoadString(hResourceDll, IDS_A68_VATDESC, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_VATDESC, szDesc);
            LoadString(hResourceDll, IDS_A68_VATTAX, szDesc, PEP_STRING_LEN_MAC(szDesc));
            DlgItemRedrawText(hDlg, IDD_A68_VATTAX, szDesc);
        }

        /* ----- Check Address 3 & 4 ----- */
        for (wI = 0, wID = IDD_A68_STS01; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Whether Target Bit is Reserved Area or Not ----- */
            if (A68_CTRL_BIT1 & (A68_BIT_CHK << wI)) {
                /* ----- Check Whether Target Bit is ON or OFF ----- */
                if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_0] & (A68_BIT_CHK << wI)) {
                    CheckDlgButton(hDlg, wID, TRUE);
                }
                wID++;
            }
        }

        /* ----- Check Address 5 & 6 ----- */
        if (uchTaxMode == A68_TAX_CAN) { /* Canadian Tax Mode */

            /* ----- Get Current Tax Data (Addr.5 Bit 0 - 3) ----- */
            wI = (A68_TAX_BIT & tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1]);

            /* ----- Set Target RadioButton ID ----- */
            wID = (IDD_A68_RADIO1 + ((wI > A68_TAX_MAX) ? A68_TAX_MAX : wI));

            /* ----- Check RadioButton with Current Tax Data ----- */
            CheckRadioButton(hDlg, IDD_A68_RADIO1, IDD_A68_RADIOA, wID);

            /* ----- Set Start Counter & Start Button ID for Addr. 6,7 ----- */
            wI  = A68_DISC_OFS;
            wID = IDD_A68_STS12;

        } else if (uchTaxMode == A68_TAX_US) { /* U.S. Tax Mode */

            /* ----- Set Start Counter & Start Button ID ----- */
            wI  = 0;
            wID = IDD_A68_STS09;

            /* ### ADD Saratoga Food Stamp (052300) */
            if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & A68_FOODSTAMP_BIT_US)
                CheckDlgButton(hDlg, IDD_A68_STS34, TRUE);
            else
                CheckDlgButton(hDlg, IDD_A68_STS34, FALSE);
        } else {    /* VAT Mode */

            /* ----- Get Current Tax Data (Addr.5 Bit 0 - 1) ----- */
            wI = (A68_TAX_BIT_VAT & tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1]);

            /* ----- Set Target RadioButton ID ----- */
            wID = (IDD_A68_VATRADIO1 + wI);

            /* ----- Check RadioButton with Current Tax Data ----- */
            CheckRadioButton(hDlg, IDD_A68_VATRADIO1, IDD_A68_VATRADIO4, wID);
            
            /* ----- Check Whether Target Bit is ON or OFF (Addr.5 Bit 3) ----- */
            wID = IDD_A68_VATSERVICE;

            if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & A68_TAX_BIT_SERVICE) {
                CheckDlgButton(hDlg, wID, TRUE);
            }

            /* ----- Set Start Counter & Start Button ID for Bit 6,7 ----- */
            wI  = A68_DISC_OFS;
            wID = IDD_A68_STS12;
        }

        for (; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT2 & (A68_BIT_CHK << wI)) {
                /* ----- Check Whether Target Bit is ON or OFF ----- */
                if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] & (A68_BIT_CHK << wI)) {
                    CheckDlgButton(hDlg, wID, TRUE);
                }
                wID++;
            }
        }

        /* ----- Check Address 7 & 8 ----- */
        for (wI = 0, wID = IDD_A68_STS14; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT3 & (A68_BIT_CHK << wI)) {
                /* ----- Check Whether Target Bit is ON or OFF ----- */
                if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] & (A68_BIT_CHK << wI)) {
                    CheckDlgButton(hDlg, wID, TRUE);
                }
                wID++;
            }
        }

		{
			BOOL   bIsAdjectivePlu = (tempPlu.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP;
			UCHAR  bitMaskTestGroup = 0;
			UCHAR  bitMasksGroup[] = {
				PLU_USE_ADJG1,    /* Use Adjective Group #1, A068InitData() */
				PLU_USE_ADJG2,    /* Use Adjective Group #2, A068InitData() */
				PLU_USE_ADJG3,    /* Use Adjective Group #3, A068InitData() */
				PLU_USE_ADJG4     /* Use Adjective Group #4, A068InitData() */
			};
			UCHAR  bitMasksProhibit[] = {
				PLU_PROHIBIT_VAL1,    /* Prohibit Validation #1, A068InitData() */
				PLU_PROHIBIT_VAL2,    /* Prohibit Validation #2, A068InitData() */
				PLU_PROHIBIT_VAL3,    /* Prohibit Validation #3, A068InitData() */
				PLU_PROHIBIT_VAL4,    /* Prohibit Validation #4, A068InitData() */
				PLU_PROHIBIT_VAL5     /* Prohibit Validation #5, A068InitData() */
			};

			/* ----- Check Address 9 & 10 Adjective Group Number ----- */
			for (wI = 0, wID = IDD_A68_STS21; wID <= IDD_A68_STS24; wI++, wID++) {
				/* ----- Check Whether Target Bit is ON or OFF ----- */
				if (bIsAdjectivePlu && tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_3] & bitMasksGroup[wI]) {
					CheckDlgButton(hDlg, wID, TRUE);
					bitMaskTestGroup |= bitMasksGroup[wI];
				}
				EnableWindow(GetDlgItem (hDlg, wID), bIsAdjectivePlu);
			}

			// if this is an Adjective PLU and there is not an Adjective Group set then arbitrarily pick Group #1.
			if (bIsAdjectivePlu && bitMaskTestGroup == 0) {
				CheckDlgButton(hDlg, IDD_A68_STS21, TRUE);
			}

			/* ----- Check Address 11 & 12 Adjective Prohibit settings ----- */
			/* ----- Set Initial Address 11 Data to ComboBox ----- */
			for (wID = IDS_A68_STS_ADJ0; wID <= IDS_A68_STS_ADJ5; wID++) {
				/* ----- Load String for Insert ComboBox ----- */
				LoadString(hResourceDll, wID, szDesc, PEP_STRING_LEN_MAC(szDesc));
				/* ----- Insert Loaded Description to ComboBox ----- */
				DlgItemSendTextMessage(hDlg, IDD_A68_ADJKEY, CB_INSERTSTRING, -1, (LPARAM)(szDesc));  //RPH 12/06/02 SR#83
			}

			wI = A68GETADJKEY(tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_4]);
			wI = ((wI > A68_ADJVAR_BIT) ? A68_ADJVAR_BIT : wI);
			SendDlgItemMessage(hDlg, IDD_A68_ADJKEY, CB_SETCURSEL, (WPARAM)wI, 0L);
			EnableWindow(GetDlgItem (hDlg, IDD_A68_ADJKEY), bIsAdjectivePlu);

			for (wI = 0, wID = IDD_A68_STS25; wID <= IDD_A68_STS29; wI++, wID++) {
                if (bIsAdjectivePlu && tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_4] & bitMasksProhibit[wI]) {
                    CheckDlgButton(hDlg, wID, TRUE);
                }
				EnableWindow(GetDlgItem (hDlg, wID), bIsAdjectivePlu);
			}
		}

		/* ----- Check Address 18 ----- */
		for (wI = 0, wID = IDD_A68_STS30; wI < A68_CTRL_BIT; wI++) {
			/* ----- Check Target Bit is Reserved Area ----- */
			if (A68_CTRL_BIT6 & (A68_BIT_CHK << wI)) {
				/* ----- Check Whether Target Bit is ON or OFF ----- */
				if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] & (A68_BIT_CHK << wI)) {
					CheckDlgButton(hDlg, wID, TRUE);
				}
				wID++;
			}
		}

		/* ----- Check Address 20 ----- */

		// this code redone to accurately display the
		// settings for gift card.
		//  This code makes sure that the automation process
		//  in storing the status bits doesn't handle the 
		//  PPI and combination Coupon precedence status
		//  bits.  They need to be handled a special way.
		//  ***PDINU
        for (wI = 0, wID = IDD_A68_STS36; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT7 & (A68_BIT_CHK << wI)) {
                /* ----- Check Whether Target Bit is ON or OFF ----- */
                if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & (A68_BIT_CHK << wI)) {
                    CheckDlgButton(hDlg, wID, TRUE);
                }
                wID++;
            }
        }

		//  The code below sets the bits for the PPI and 
		//  Combination Coupon precedence functionality
	
		if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & A68_CTRL_COUPON) {
			CheckDlgButton(hDlg, IDD_A005_PREC02, TRUE);
			CheckDlgButton(hDlg, IDD_A005_PREC01, FALSE);
		} else
		{
			CheckDlgButton(hDlg, IDD_A005_PREC01, TRUE);
			CheckDlgButton(hDlg, IDD_A005_PREC02, FALSE);
		}

		// Sets the Gift Card or Electronic Payment option for the correct button
		// Used for Gift Card and for Employee Payroll Deduction (VCS)
		CheckDlgButton(hDlg, IDD_A68_GIFTCARD, FALSE);
		CheckDlgButton(hDlg, IDD_A68_EPAYMENT, FALSE);
		CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, FALSE);
		CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, FALSE);
		if ((tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & (A68_CTRL_GIFTCARD | A68_CTRL_EPAYMENT)) != 0) {
			if ((tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & A68_CTRL_GIFTCARD) != 0) 
				CheckDlgButton(hDlg, IDD_A68_GIFTCARD, TRUE);
			else if ((tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & A68_CTRL_EPAYMENT) != 0) 
				CheckDlgButton(hDlg, IDD_A68_EPAYMENT, TRUE);

			if ((tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & A68_CTRL_RELOAD) == 0) {
				CheckDlgButton(hDlg, IDD_A68_GIFT_ISSUE, TRUE);
			}
			else if (tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] & A68_CTRL_RELOAD) {
				CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, TRUE);
			} else {
				CheckDlgButton(hDlg, IDD_A68_GIFT_RELOAD, TRUE);    // default is for the PLU to be a reload.
			}
		}

/*** Saratoga Start ***/

        /* ----- Set Initial Address 19 ----- */
        for(wID = IDS_A68_STS_TYPEKEY0; wID <= IDS_A68_STS_TYPEKEY3; wID++) {
            /* ----- Load String for Insert ComboBox ----- */
            LoadString(hResourceDll, wID, szDesc, PEP_STRING_LEN_MAC(szDesc));
            /* ----- Insert Loaded Description to ComboBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_TYPEKEY, CB_INSERTSTRING, -1, (LPARAM)(szDesc));  //RPH 12/06/02 SR#83
        }

        /* ----- Set Initial Address 19 Data to ComboBox ----- */

/*
        wI = (WORD)A68GETTYPEKEY(tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_8]);

        wI = (WORD)((wI > A68_TYPE_BIT) ? A68_TYPE_BIT : wI);
        SendDlgItemMessage(hDlg, IDD_A68_TYPEKEY, CB_SETCURSEL, wI, 0L);
*/
        uchI = (UCHAR)(tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] >> 4);
        uchI &= 0x03; /* ### ADD Saratog WIC (TYPE BIT = 0011)(052700) */
        SendDlgItemMessage(hDlg, IDD_A68_TYPEKEY, CB_SETCURSEL, (WPARAM)uchI, 0L);

        /* ### ADD Saratoga WIC (052700) */
        /* ----- Set Initial Address 19 Data to WIC ----- */
        if(tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] & A68_WIC_BIT_US)
            CheckDlgButton(hDlg, IDD_A68_STS35, TRUE);
        else
            CheckDlgButton(hDlg, IDD_A68_STS35, FALSE);

		if (IsDlgButtonChecked (hDlg, IDD_A68_STS16)) {
			int i;
			for (i = IDD_A68_STS01; i <= IDD_A68_STS24; i++) {
				if (i == IDD_A68_STS16) continue;    // do not modify the Use Departmental Control Code checkbox itself!
				EnableWindow(GetDlgItem (hDlg, i), ! IsDlgButtonChecked (hDlg, IDD_A68_STS16));
			}
			// handle the remote printers #5 - #8 along with foodstamp and WIC
			for (i = IDD_A68_STS30; i <= IDD_A68_STS35; i++) {
				EnableWindow(GetDlgItem (hDlg, i), ! IsDlgButtonChecked (hDlg, IDD_A68_STS16));
			}
		}
/*** Saratoga End ***/
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068SaveChkPrc()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPDWORD lpadwPrePrc -   Address of Preset Price Array
*                   LPUINT  lpunPluType -   Address of Current PLU Type Buffer
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed preset price data from edittext, and save
*   them to static work area. And then it determines the PLU type of current
*   editted PLU record.
* ===========================================================================
*/
VOID
A068SaveChkPrc(HWND hDlg, LPDWORD lpadwPrePrc, LPUINT lpunPluType)
{
    long    lWork;
    WCHAR   szWork[16];
    WORD    wI, wID;
    DWORD   dwTextLen;

    for (wI = 0, wID = IDD_A68_PRESET1; wI < A68_ADJ_NUM; wI++, wID++) {

        /* ----- Get Length of Inputed Preset Price ----- */
        dwTextLen = DlgItemSendTextMessage(hDlg, wID, WM_GETTEXTLENGTH, 0, 0L);

        /* ----- Get Inputed Preset Price Value ----- */
        DlgItemGetText(hDlg, wID, szWork, sizeof(szWork)/sizeof(szWork[0]));

        /* ----- Save Preset Price Data to Static Work Area ----- */
        lWork = _wtol(szWork);

        if (lWork < (LONG)A68_DATA_MIN) {
            lWork = (LONG)A68_DATA_MIN;
        }

        *(lpadwPrePrc + wI) = (DWORD)lWork;

        /* ----- Determine PLU Type of Current Editted PLU Record ----- */
        if (wID == IDD_A68_PRESET1) {
            /* ----- Check Length of Preset Price Data ----- */
            if (! dwTextLen) {
                *lpunPluType = (UINT)A68_TYPE_OPEN; /* Open Price PLU       */
            } else {
                *lpunPluType = (UINT)A68_TYPE_NONADJ;   /* Non-Adjective PLU    */
            }
        } else {
            /* ----- Check Length of Preset Price Data ----- */
            if (dwTextLen) {
                *lpunPluType = (UINT)A68_TYPE_ONEADJ;   /* One-Adjective PLU    */
            }
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068SaveRec()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wDlgID  -   Current Editting DialogBox ID
*
**  Return      :   No return value
*
**  Description :
*       This procedure gets inputed PLU Data from each controls, and saves them
*   to a static work area.
* ===========================================================================
*/
VOID    A068SaveRec(HWND hDlg, WPARAM wDlgID)
{
    UINT    unValue;
    WORD    wI, wID;
    WCHAR   szWork[A68_MNEMO_SIZE + 1];        /* Saratoga */
    WCHAR   szPlu[PLU_MAX_DIGIT + 1];          /* NCR2172 */
    WCHAR   szTemp[PLU_MAX_DIGIT + 1];
    BOOL    bRet;
    WCHAR   uchI;
    int     nLen;

    /* ----- Determine Current Editting DialogBox ----- */
    if (LOWORD(wDlgID) == IDD_A68) {    /* PLU Main DialogBox */
        /* ----- Get Inputed PLU No. from PLU No. EditText ----- */
        memset(szPlu, 0x00, sizeof(szPlu));
        unValue = DlgItemGetText(hDlg, IDD_A68_NO, szPlu, sizeof(szPlu)/sizeof(szPlu[0]));

        nLen = wcslen(szPlu);
        if (nLen < PLU_MAX_DIGIT) {
			{
				int i;
				for(i = 0; i < STD_PLU_NUMBER_LEN; i++){
					wcscpy(szTemp, WIDE("0"));
				}
			}
		   //memset(szTemp, '0', sizeof(szTemp));
           szTemp[PLU_MAX_DIGIT] = 0x00;
           memcpy(&szTemp[PLU_MAX_DIGIT - nLen], szPlu, nLen * sizeof(WCHAR));
           memcpy(szPlu, szTemp, PLU_MAX_DIGIT * sizeof(WCHAR));
        }

        bRet = A068GetPluNo(hDlg, szPlu, TRUE);

        /* ----- Set PLU Adjective No. to Static Work Area ----- */
        tempPlu.uchPluAdj = A68_ADJ_NO1;

        /* ----- Get Inputed Depertment No. from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_DEPTNO, NULL, FALSE);
        tempPlu.ParaPlu.ContPlu.usDept = (USHORT)unValue;

        /* ----- Reset Report Code Area without Reserved Area ----- */
        unValue = (UINT)PEP_LOMASK(tempPlu.ParaPlu.ContPlu.uchRept);

        /* ----- Get Inputed Report Code from EditText ----- */
        unValue |= GetDlgItemInt(hDlg, IDD_A68_RPTCD, NULL, FALSE);
        tempPlu.ParaPlu.ContPlu.uchRept = (UCHAR)unValue;

        /* ----- Reset Bonus Total Index Area without Other Data ----- */
        unValue =tempPlu.ParaPlu.ContPlu.usBonusIndex;

        /* ----- Get Inputed Bonus Total Index from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_BONUS, NULL, FALSE);
        tempPlu.ParaPlu.ContPlu.usBonusIndex = (UCHAR)unValue;

        /* ----- Get Inputed Mnemonic Data from EditText ----- */
        /* ----- Set Inputed Mnemonic Data to Static Work Area ----- */
        memset(szWork, 0, sizeof(szWork));
        DlgItemGetText(hDlg, IDD_A68_MNEMO, szWork, sizeof(szWork)/sizeof(szWork[0]));
        PepReplaceMnemonic (szWork, tempPlu.ParaPlu.auchPluName, (A68_MNEMO_SIZE ), PEP_MNEMO_WRITE);

        /* ----- Get Inputed Mnemonic Data from EditText ----- */
        /* ----- Set Inputed Mnemonic Data to Static Work Area ----- */
        memset(szWork, 0, sizeof(szWork));
        DlgItemGetText(hDlg, IDD_A68_ALTMNEMO, szWork, sizeof(szWork)/sizeof(szWork[0]));
		PepReplaceMnemonic (szWork, tempPlu.ParaPlu.auchAltPluName, (A68_MNEMO_SIZE ), PEP_MNEMO_WRITE);

        /* ----- Get Inputed Table Number from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_TBLNO, NULL, FALSE);
        tempPlu.ParaPlu.uchTableNumber = (UCHAR)unValue;

        /* ----- Get Inputed Group Number from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_GRPNO, NULL, FALSE);
        tempPlu.ParaPlu.uchGroupNumber = (UCHAR)unValue;

        /* ----- Get Inputed Print Priority from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_PRTPRY, NULL, FALSE);
        tempPlu.ParaPlu.uchPrintPriority = (UCHAR)unValue;

        /* ----- Get Inputed PPI from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_PPI, NULL, FALSE);
        tempPlu.ParaPlu.uchLinkPPI = (USHORT)unValue; //ESMITH

        /* ----- Get Inputed Color Palette Number from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_COLOR, NULL, FALSE);
        tempPlu.ParaPlu.uchColorPaletteCode = (UCHAR)unValue;

/*** Saratoga ***/

        /* ----- Get Inputed Link PLU from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_LINK, NULL, FALSE);
        tempPlu.ParaPlu.usLinkNo = (USHORT)unValue;

        /* ----- Get Inputed Sales Restriction Code from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_SALES, NULL, FALSE);
        tempPlu.ParaPlu.uchRestrict = (UCHAR)unValue;

        /* ----- Get Inputed Price Multiple from EditText ----- */
        unValue = GetDlgItemInt(hDlg, IDD_A68_MULTI, NULL, FALSE);
        tempPlu.ParaPlu.uchPM = (UCHAR)unValue;

        /* ----- Get Inputed Family Code from EditText ----- */
        /* ### ADD Saratoga Family Code (V1_0.02)(051800) */
        unValue = GetDlgItemInt(hDlg, IDD_A68_FAMILY, NULL, FALSE);
        tempPlu.ParaPlu.usFamilyCode = (USHORT)unValue;

		//tare information for weighted goods.  Now an item can
		//have a specified tare, and will automatically add the 
		//tare to the transaction.  In order to save space
		//we save the flag and the number in the same location
		//the flag is set on the high end of the ushort while the
		//other information, the tare number will be set in the
		//low end, this number will never go past 255, at least
		//while im alive. JHHJ
		unValue = GetDlgItemInt(hDlg, IDD_A68_TARE, NULL, FALSE);

        tempPlu.ParaPlu.usTareInformation = (USHORT)(unValue & PLU_REQ_TARE_MASK);

		if(IsDlgButtonChecked(hDlg, IDD_A68_STSTARECCHECK))
		{
			tempPlu.ParaPlu.usTareInformation |= PLU_REQ_TARE;
		}

		// new extended group and priority for new Dynamic PLU Button functionality for Amtrak
		tempPlu.ParaPlu.uchExtendedGroupPriority[0] = GetDlgItemInt(hDlg, IDD_A68_EXT_GRP_1, NULL, FALSE);
		tempPlu.ParaPlu.uchExtendedGroupPriority[1] = GetDlgItemInt(hDlg, IDD_A68_EXT_PRI_1, NULL, FALSE);
		tempPlu.ParaPlu.uchExtendedGroupPriority[2] = GetDlgItemInt(hDlg, IDD_A68_EXT_GRP_2, NULL, FALSE);
		tempPlu.ParaPlu.uchExtendedGroupPriority[3] = GetDlgItemInt(hDlg, IDD_A68_EXT_PRI_2, NULL, FALSE);
		tempPlu.ParaPlu.uchExtendedGroupPriority[4] = GetDlgItemInt(hDlg, IDD_A68_EXT_GRP_3, NULL, FALSE);
		tempPlu.ParaPlu.uchExtendedGroupPriority[5] = GetDlgItemInt(hDlg, IDD_A68_EXT_PRI_3, NULL, FALSE);

	/*** Saratoga End ***/
    } else {                    /* PLU Control Code DialogBox */
        /* ----- Get Address 3 & 4 ----- */
        for (wI = 0, wID = IDD_A68_STS01; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT1 & (A68_BIT_CHK << wI)) {
                /* ----- Determine CheckButton is Checked or Not ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_0] |= (A68_BIT_CHK << wI);
                } else {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_0] &= ~(A68_BIT_CHK << wI);
                }
                wID++;
            }
        }

        /* ----- Get Address 5 & 6 ----- */
        if (uchTaxMode == A68_TAX_CAN) { /* Canadian Tax Mode */
            /* ----- Reset Canaian Tax Data Area ----- */
            tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] &= ~A68_TAX_BIT;
            for (wI = 0, wID = IDD_A68_RADIO1; wI < A68_TAX_MAX; wI++, wID++) {
                /* ----- Get Current Selected RadioButton ID ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    break;
                }
            }

            /* ----- Set Canadian Tax Data to Static Work Area ----- */
            tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] |= (UCHAR)wI;

            /* ----- Set Start Counter & Start Button ID ----- */
            wI = A68_DISC_OFS;
            wID = IDD_A68_STS12;
        } else if (uchTaxMode == A68_TAX_US) {              /* U.S. Tax Mode    */
            /* ----- Set Start Counter & Start Button ID ----- */
            wI = 0;
            wID = IDD_A68_STS09;

            /* ### ADD Saratoga Food Stamp (052300) */
            if (IsDlgButtonChecked(hDlg, IDD_A68_STS34))
                tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] |= (A68_FOODSTAMP_BIT_US);
            else
                tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] &= ~(A68_FOODSTAMP_BIT_US);
        } else {    /* VAT Mode */
            /* ----- Reset VAT Data Area ----- */
            tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] &= ~A68_TAX_BIT;
            for (wI = 0, wID = IDD_A68_VATRADIO1; wI < A68_TAX_MAX_VAT; wI++, wID++) {
                /* ----- Get Current Selected RadioButton ID ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    break;
                }
            }

            /* ----- Set VAT Data to Static Work Area ----- */
            tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] |= (UCHAR)wI;

            /* ----- Determine CheckButton is Checked or Not for Bit 3----- */
            wID = IDD_A68_VATSERVICE;
            if (IsDlgButtonChecked(hDlg, wID)) {
                tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] |= A68_TAX_BIT_SERVICE;
            } else {
                tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] &= ~(A68_TAX_BIT_SERVICE);
            }

            /* ----- Set Start Counter & Start Button ID ----- */
            wI = A68_DISC_OFS;
            wID = IDD_A68_STS12;
        }

        for (; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT2 & (A68_BIT_CHK << wI)) {
                /* ----- Determine CheckButton is Checked or Not ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] |= (A68_BIT_CHK << wI);
                } else {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_1] &= ~(A68_BIT_CHK << wI);
                }
                wID++;
            }
        }

        /* ----- Get Address 7 & 8 ----- */
        for (wI = 0, wID = IDD_A68_STS14; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT3 & (A68_BIT_CHK << wI)) {
                /* ----- Determine CheckButton is Checked or Not ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] |= (A68_BIT_CHK << wI);
                } else {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_2] &= ~(A68_BIT_CHK << wI);
                }
                wID++;
            }
        }

		{
			BOOL   bIsAdjectivePlu = (tempPlu.ParaPlu.ContPlu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP;
			UCHAR bitMasksGroup[] = {
				PLU_USE_ADJG1,    /* Use Adjective Group #1, A068InitData() */
				PLU_USE_ADJG2,    /* Use Adjective Group #2, A068InitData() */
				PLU_USE_ADJG3,    /* Use Adjective Group #3, A068InitData() */
				PLU_USE_ADJG4     /* Use Adjective Group #4, A068InitData() */
			};
			UCHAR bitMasksProhibit[] = {
				PLU_PROHIBIT_VAL1,    /* Prohibit Validation #1, A068InitData() */
				PLU_PROHIBIT_VAL2,    /* Prohibit Validation #2, A068InitData() */
				PLU_PROHIBIT_VAL3,    /* Prohibit Validation #3, A068InitData() */
				PLU_PROHIBIT_VAL4,    /* Prohibit Validation #4, A068InitData() */
				PLU_PROHIBIT_VAL5     /* Prohibit Validation #5, A068InitData() */
			};

			/* ----- Get Address 9 & 10 ----- */
			for (wI = 0, wID = IDD_A68_STS21; wID <= IDD_A68_STS24; wI++, wID++) {
				/* ----- Determine CheckButton is Checked or Not ----- */
				if (bIsAdjectivePlu && IsDlgButtonChecked(hDlg, wID)) {
					tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_3] |= bitMasksGroup[wI];
				} else {
					tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_3] &= ~bitMasksGroup[wI];
				}
			}

			/* ----- Reset Address 11 Data Area ----- */
			tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_4] &= A68_CTRL_BIT5;

			/* ----- Get Address 11 Data from EditText ----- */
			wI = (WORD)SendDlgItemMessage(hDlg, IDD_A68_ADJKEY, CB_GETCURSEL, 0, 0L);
			tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_4] |= wI;

			/* ----- Get Address 12 ----- */
			for (wI = 0, wID = IDD_A68_STS25; wID <= IDD_A68_STS29; wI++, wID++) {
				/* ----- Determine CheckButton is Checked or Not ----- */
				if (bIsAdjectivePlu && IsDlgButtonChecked(hDlg, wID)) {
					tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_4] |= bitMasksProhibit[wI];
				} else {
					tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_4] &= ~bitMasksProhibit[wI];
				}
			}
		}

        /* ----- Get Address 18 ----- */
        for (wI = 0, wID = IDD_A68_STS30; wI < A68_CTRL_BIT; wI++) {
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT6 & (A68_BIT_CHK << wI)) {
                /* ----- Determine CheckButton is Checked or Not ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] |= (A68_BIT_CHK << wI);
                } else {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] &= ~(A68_BIT_CHK << wI);
                }
                wID++;
            }
        }

		/* ----- ADDRESS 20 -----   ***PDINU*/
		for (wI = 0, wID = IDD_A68_STS36; wI < A68_CTRL_BIT; wI++) {
			//  This code makes sure that the automation process
			//  in storing the status bits doesn't handle the 
			//  PPI and combination Coupon precedence status
			//  bits.  They need to be handled a special way.
			//  ***PDINU
		
            /* ----- Check Target Bit is Reserved Area ----- */
            if (A68_CTRL_BIT7 & (A68_BIT_CHK << wI)) {
                /* ----- Determine CheckButton is Checked or Not ----- */
                if (IsDlgButtonChecked(hDlg, wID)) {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] |= (A68_BIT_CHK << wI);
                } else {
                    tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] &= ~(A68_BIT_CHK << wI);
                }
                wID++;
            }
       }

		//  The code below sets the bits for the Gift Card and Electronic Payment 
		//  functionality use with Mercury and DataCap servers.
		tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] &= ~A68_CTRL_RELOAD;
		tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] &= ~(A68_CTRL_GIFTCARD | A68_CTRL_EPAYMENT);
		if(IsDlgButtonChecked(hDlg, IDD_A68_GIFTCARD))
		{
			tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] |= A68_CTRL_GIFTCARD;
			if(IsDlgButtonChecked(hDlg, IDD_A68_GIFT_RELOAD))
			{
				tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] |= A68_CTRL_RELOAD;
			}
		}
		else if(IsDlgButtonChecked(hDlg, IDD_A68_EPAYMENT))
		{
			tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] |= A68_CTRL_EPAYMENT;
			if(IsDlgButtonChecked(hDlg, IDD_A68_GIFT_RELOAD))
			{
				tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] |= A68_CTRL_RELOAD;
			}
		}

 		//  The code below sets the bits for the PPI and 
		//  Combination Coupon precedence functionality
		if(SendDlgItemMessage(hDlg, IDD_A005_PREC02, BM_GETCHECK, 0, 0L))
		{
			tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] |= A68_CTRL_COUPON;
		}else
		{
			tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_6] &= ~A68_CTRL_COUPON;
		}

/*** Saratoga Start ***/

        /* ----- Get Address 19 Data from EditText ----- */

        wI = (WORD)SendDlgItemMessage(hDlg, IDD_A68_TYPEKEY, CB_GETCURSEL, 0, 0L);

        uchI = tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5];
        uchI &= A68_TYPE_BIT;
        uchI |= ((UCHAR)wI << 4);

        tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] = (UCHAR)uchI;

        /* ### ADD Saratoga WIC (052700) */
        if (IsDlgButtonChecked(hDlg, IDD_A68_STS35))
            tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] |= (A68_WIC_BIT_US);
        else
            tempPlu.ParaPlu.ContPlu.auchContOther[A68_CTRL_5] &= ~(A68_WIC_BIT_US);

/*** Saratoga End ***/
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068SetRec()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPUINT  lpunPluType -   Address of PLU Type Data Area
*                   LPDWORD lpadwPrePrc -   Address of Preset Price Array 
*
**  Return      :   No return value.
*
**  Description :
*       This procedure 
* ===========================================================================
*/
VOID    A068SetRec(HWND hDlg, int idcListBox, LPUINT lpunPluType, LPDWORD lpadwPrePrc)
{
    PLUIF   PluWork;
    WCHAR   szWork[OP_PLU_NAME_SIZE + 1], szBuff[48];
    DWORD   dwPrice;
    WORD    wI, wIndex;

    UINT    unValue;

    WCHAR   szPlu[OP_PLU_LEN + 1];
    WCHAR   szPluWork[OP_PLU_LEN + 1];
    USHORT  usI, usII;
    BOOL    bRet;

    /* ----- Get Selected Index of ListBox ----- */
    wIndex = (WORD)SendDlgItemMessage(hDlg, idcListBox, LB_GETCURSEL, 0, 0L);
    DlgItemSendTextMessage(hDlg, idcListBox, LB_GETTEXT, wIndex, (LPARAM)(szBuff));

    /* ----- Compute Selected PLU No. and Set to Buffer ----- */
	/* NCR2172 Start */
	  memset(szWork, 0x00, sizeof(szWork));
      memset(szPlu, 0x00, sizeof(szPlu));
      memcpy(szPlu, szBuff, PLU_MAX_DIGIT * sizeof(WCHAR));

      memset(szPluWork, 0x00, sizeof(szPluWork));

      for(usI = 0, usII = 0; usI < PLU_MAX_DIGIT; usI++) {
         if((szPlu[usI] != 0x20) && (szPlu[usI] != 0x00)) {
             szPluWork[usII] = szPlu[usI];
             usII++;
         }
      }

      fVer = LA_NORMAL;
      bRet = A068GetPluNo(hDlg, szPluWork, FALSE);

      memcpy(tempPlu.auchPluNo, szPluWork, PLU_MAX_DIGIT * sizeof(WCHAR));
	  tempPlu.ParaPlu.usTareInformation = 0;
	  SendMessage(GetDlgItem(hDlg, IDD_A68_STSTARECCHECK), BM_SETCHECK, BST_UNCHECKED, 0); 

	  memset (tempPlu.ParaPlu.uchExtendedGroupPriority, 0, sizeof(tempPlu.ParaPlu.uchExtendedGroupPriority));

	/* NCR2172 End */

    tempPlu.uchPluAdj = A68_ADJ_NO1;

    /* ----- Retrieve PLU Record of Specified PLU No. ----- */
    OpPluIndRead(&tempPlu, 0);

    /* ----- Get PLU Type ----- */
    if (tempPlu.ParaPlu.ContPlu.uchItemType == PLU_OPEN_TYP) {
       *lpunPluType = A68_TYPE_OPEN;
    } else if (tempPlu.ParaPlu.ContPlu.uchItemType == PLU_NON_ADJ_TYP) {
       *lpunPluType = A68_TYPE_NONADJ;
    } else {
       *lpunPluType = A68_TYPE_ONEADJ;
    }

    /* ----- Set PLU No. to EditText ----- */
    memset(szPlu, 0x00, sizeof(szPlu));
    RflConvertPLU (szPlu, szPluWork);

    DlgItemRedrawText(hDlg, IDD_A68_NO, szPlu);

    /* ----- Set Department Number to EditText ----- */
    unValue = (UINT)tempPlu.ParaPlu.ContPlu.usDept;
    SetDlgItemInt(hDlg, IDD_A68_DEPTNO, unValue, FALSE);

    /* ----- Set Report Code to EditText ----- */
    unValue = A68GETRPTCODE(tempPlu.ParaPlu.ContPlu.uchRept);
    SetDlgItemInt(hDlg, IDD_A68_RPTCD, unValue, FALSE);
    
    /* ----- Set Bonus Total Index to EditText ----- */
    unValue = tempPlu.ParaPlu.ContPlu.usBonusIndex;
    unValue = (unValue > A68_BTTL_MAX) ? A68_BTTL_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_BONUS, unValue, FALSE);

    /* ----- Set PLU Mnemonic to EditText ----- */
    PepReplaceMnemonic (tempPlu.ParaPlu.auchPluName, szWork, (A68_MNEMO_SIZE ), PEP_MNEMO_READ);
    DlgItemRedrawText(hDlg, IDD_A68_MNEMO, szWork);
	PepReplaceMnemonic (tempPlu.ParaPlu.auchAltPluName, szWork, (A68_MNEMO_SIZE ), PEP_MNEMO_READ);
    DlgItemRedrawText(hDlg, IDD_A68_ALTMNEMO, szWork);

    /* ----- Set Table Number to EditText ----- */
    unValue = (UINT)(tempPlu.ParaPlu.uchTableNumber);
    unValue = (unValue > A68_TBLNO_MAX) ? A68_TBLNO_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_TBLNO, unValue, FALSE);

    /* ----- Set Group Number to EditText ----- */
    unValue = (UINT)(tempPlu.ParaPlu.uchGroupNumber);
    unValue = (unValue > A68_GRPNO_MAX) ? A68_GRPNO_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_GRPNO, unValue, FALSE);

    /* ----- Set Print Prtiorty to EditText ----- */
    unValue = (UINT)(tempPlu.ParaPlu.uchPrintPriority);
    unValue = (unValue > A68_PRTPRY_MAX) ? A68_PRTPRY_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_PRTPRY, unValue, FALSE);

    /* ----- Set PPI to EditText ----- */
    unValue = (UINT)(tempPlu.ParaPlu.uchLinkPPI);
    unValue = (unValue > A68_PPI_MAX) ? A68_PPI_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_PPI, unValue, FALSE);

/*** Saratoga Start ***/

    /* ----- Set Link PLU to EditText ----- */
    unValue = (UINT)tempPlu.ParaPlu.usLinkNo;
    unValue = (unValue > A68_LINK_MAX) ? A68_LINK_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_LINK, unValue, FALSE);

    /* ----- Set Sales Restriction Code to EditText ----- */
    unValue = (UINT)tempPlu.ParaPlu.uchRestrict;
    unValue = (unValue > A68_SALES_MAX) ? A68_SALES_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_SALES, unValue, FALSE);

    /* ----- Set Price Multiple to EditText ----- */
    unValue = (UINT)tempPlu.ParaPlu.uchPM;
    unValue = (unValue > A68_MULTI_MAX) ? A68_MULTI_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_MULTI, unValue, FALSE);

    /* ----- Set Family Code to EditText ----- */
    /* ### ADD Saratoga Family Code (V1_0.02)(051800) */
    unValue = (UINT)tempPlu.ParaPlu.usFamilyCode;
    unValue = (unValue > A68_FAMILY_MAX) ? A68_FAMILY_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_FAMILY, unValue, FALSE);
	/*** Saratoga End ***/

    /* ----- Set Color Palette to EditText ----- */
    unValue = (UINT)(tempPlu.ParaPlu.uchColorPaletteCode);
    unValue = (unValue > A68_COLOR_MAX) ? A68_COLOR_MAX : unValue;
    SetDlgItemInt(hDlg, IDD_A68_COLOR, unValue, FALSE);

    /* ----- Check PLU Type is Open Price or Not ----- */
    if (*lpunPluType != A68_TYPE_OPEN) {
        /* ----- Set Preset Price for Adjective #1 ----- */
        PepConv3Cto4L((LPBYTE)tempPlu.ParaPlu.auchPrice, (LPDWORD)&dwPrice);
        _ultow(dwPrice, szWork, A68_RADIX_10);
        DlgItemRedrawText(hDlg, IDD_A68_PRESET1, szWork);
    } else {
        /* ----- Erase Contents of EditText (Preset Price Adjective #1) ----- */
        SendDlgItemMessage(hDlg, IDD_A68_PRESET1, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, IDD_A68_PRESET1, WM_CLEAR,  0, 0L);
    }


	//tare information for weighted goods.  Now an item can
	//have a specified tare, and will automatically add the 
	//tare to the transaction.  In order to save space
	//we save the flag and the number in the same location
	//the flag is set on the high end of the ushort while the
	//other information, the tare number will be set in the
	//low end, this number will never go past 255, at least
	//while im alive. JHHJ
	unValue = (UINT)tempPlu.ParaPlu.usTareInformation;

	SetDlgItemInt(hDlg, IDD_A68_TARE, (unValue & PLU_REQ_TARE_MASK), FALSE);

	if(tempPlu.ParaPlu.usTareInformation & PLU_REQ_TARE)
	{
		SendMessage(GetDlgItem(hDlg,IDD_A68_STSTARECCHECK), BM_SETCHECK, BST_CHECKED, 0); 
	}

	// new extended group and priority for new Dynamic PLU Button functionality for Amtrak
	SetDlgItemInt(hDlg, IDD_A68_EXT_GRP_1, tempPlu.ParaPlu.uchExtendedGroupPriority[0], FALSE);
    SetDlgItemInt(hDlg, IDD_A68_EXT_PRI_1, tempPlu.ParaPlu.uchExtendedGroupPriority[1], FALSE);
    SetDlgItemInt(hDlg, IDD_A68_EXT_GRP_2, tempPlu.ParaPlu.uchExtendedGroupPriority[2], FALSE);
    SetDlgItemInt(hDlg, IDD_A68_EXT_PRI_2, tempPlu.ParaPlu.uchExtendedGroupPriority[3], FALSE);
    SetDlgItemInt(hDlg, IDD_A68_EXT_GRP_3, tempPlu.ParaPlu.uchExtendedGroupPriority[4], FALSE);
    SetDlgItemInt(hDlg, IDD_A68_EXT_PRI_3, tempPlu.ParaPlu.uchExtendedGroupPriority[5], FALSE);

    /* ----- Check PLU Type is One Adjective or Not ----- */
    if (*lpunPluType == A68_TYPE_ONEADJ) {  /* One Adjective PLU */

        for (wI = 0; wI < A68_TYPE3_NUM; wI++) {

            /* ----- Set PLU No. to PLU Work Buffer ----- */
            memcpy(PluWork.auchPluNo, tempPlu.auchPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

            /* ----- Set Adjective No. to PLU work Buffer ----- */
            PluWork.uchPluAdj = (UCHAR)(A68_ADJ_NO2 + wI);

            /* ----- Retrieve PLU Record of Specified PLU/Adjective No.----- */
            OpPluIndRead(&PluWork, 0);

            /* ----- Convert 3 BYTE Data to DWORD Data ----- */
            PepConv3Cto4L((LPBYTE)(PluWork.ParaPlu.auchPrice), (LPDWORD)&dwPrice);

            /* ----- Set Loaded Preset Price Data to Static Buffer ----- */
            *(lpadwPrePrc + wI) = dwPrice;

            /* ----- Set Preset Price for Adjective # 2 - 5 ----- */
            _ultow(dwPrice, szWork, A68_RADIX_10);

            DlgItemRedrawText(hDlg, IDD_A68_PRESET2 + wI, szWork);
        }
    } else {                            /* Non-Adjective / Open PLU */
        for (wI = IDD_A68_PRESET2; wI <= IDD_A68_PRESET5; wI++) {
            /* ----- Erase Contents of EditText (Preset Price #2 - 5) ----- */
            SendDlgItemMessage(hDlg, wI, EM_SETSEL, 0, MAKELONG(0, -1));
            SendDlgItemMessage(hDlg, wI, WM_CLEAR,  0, 0L);
        }
    }

	/* ---- Change the state of the preset type ESMITH ---- */
	A068ChngPresetState(hDlg,*lpunPluType);

    /* ----- Set Control Code Data to Control Code ListBox ----- */
    A068SetCtrlList(hDlg, (LPA68DATA)(&(tempPlu.ParaPlu)));
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ChngPresetState()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT	unType		-   state of Preset Value
*                   
*
**  Return      :   No return value.
*
**  Description :
*       This procedure sets the state of the preset text boxes, scroll bars,
*		and the preset radio button state. For open type everything is 
*		disabled, because the value is set by the operator at the time the 
*       item is purchased. The preset state, only the first adjective is 
*       available, because it only has one price, if any of the other 
*       adjectives are set (other than the first one) then the type is an 
*       "adjective", therefore everything is enabled allowing a user to set 
*       the value for all of the adjectives.
* ===========================================================================
*/
VOID	A068ChngPresetState(HWND hDlg, UINT unType)
{
	UINT    wI;
	WCHAR	szType[A68_PRC_LEN + 4];

	switch(unType)
	{
	case A68_TYPE_ONEADJ:
		SendMessage(GetDlgItem(hDlg,IDD_A68_OPENTYPE), BM_SETCHECK, BST_UNCHECKED, 0); 
		SendMessage(GetDlgItem(hDlg,IDD_A68_ADJECTIVETYPE), BM_SETCHECK, BST_CHECKED, 0); 
		SendMessage(GetDlgItem(hDlg,IDD_A68_PRESETTYPE), BM_SETCHECK, BST_UNCHECKED, 0); 

		for (wI = 0; wI < A68_ADJ_NUM; wI++) 
		{
			EnableWindow(GetDlgItem(hDlg,IDD_A68_PRESET1+wI),    TRUE); 
			EnableWindow(GetDlgItem(hDlg,IDD_A68_PRE1_SPIN+wI),    TRUE);
			
			if((DlgItemGetText(hDlg,IDD_A68_PRESET1+wI,szType, A68_PRC_LEN + 1) == 0))
			{
				SetDlgItemInt(hDlg, IDD_A68_PRESET1+wI,  A68_DATA_MIN,   TRUE);	
			}
        }

		break;

	case A68_TYPE_OPEN:
		SendMessage(GetDlgItem(hDlg,IDD_A68_OPENTYPE), BM_SETCHECK, BST_CHECKED, 0); 
		SendMessage(GetDlgItem(hDlg,IDD_A68_ADJECTIVETYPE), BM_SETCHECK, BST_UNCHECKED, 0); 
		SendMessage(GetDlgItem(hDlg,IDD_A68_PRESETTYPE), BM_SETCHECK, BST_UNCHECKED, 0); 

		for (wI = 0; wI < A68_ADJ_NUM; wI++)  // SR 1042 - '<=' changed to '<' - CSMALL
		{
			EnableWindow(GetDlgItem(hDlg,IDD_A68_PRESET1+wI),    FALSE); 
			EnableWindow(GetDlgItem(hDlg,IDD_A68_PRE1_SPIN+wI),    FALSE);
        }
		break;

	default:
		SendMessage(GetDlgItem(hDlg,IDD_A68_OPENTYPE), BM_SETCHECK, BST_UNCHECKED, 0); 
		SendMessage(GetDlgItem(hDlg,IDD_A68_ADJECTIVETYPE), BM_SETCHECK, BST_UNCHECKED, 0); 
		SendMessage(GetDlgItem(hDlg,IDD_A68_PRESETTYPE), BM_SETCHECK, BST_CHECKED, 0);
		
		EnableWindow(GetDlgItem(hDlg,IDD_A68_PRESET1),    TRUE); 
		EnableWindow(GetDlgItem(hDlg,IDD_A68_PRE1_SPIN),    TRUE);

		if((DlgItemGetText(hDlg,IDD_A68_PRESET1,szType, sizeof(szType)/sizeof(szType[0])) == 0))
		{
			SetDlgItemInt(hDlg, IDD_A68_PRESET1,  A68_DATA_MIN,   TRUE);	
		}

		for (wI = 0; wI < A68_TYPE3_NUM; wI++) 
		{
			EnableWindow(GetDlgItem(hDlg,IDD_A68_PRESET2+wI),    FALSE); 
			EnableWindow(GetDlgItem(hDlg,IDD_A68_PRE2_SPIN+wI),    FALSE);
        }
		break;
	}
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068IntoPluFile()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   LPUINT  lpunPluType -   Address of PLU Type Data Area
*                   LPDWORD lpadwPrePrc -   Address of Preset Price Array 
*
**  Return      :   No return value.
*
**  Description :
*       This procedure 
* ===========================================================================
*/
SHORT    A068IntoPluFile(HWND hDlg)
{
    SHORT   sRet = OP_PERFORM;

    /* ----- Retrieve PLU Record of Specified PLU No. ----- */
    sRet = OpPluIndRead(&tempPlu, 0);

    if (sRet == OP_NOT_IN_FILE) 
       sRet = OP_NOT_IN_FILE;

    return sRet;
    
    hDlg = hDlg;
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ClrRec()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value
*
**  Description :
*       This procedure erases the controls of main dialogbox.
* ===========================================================================
*/
VOID    A068ClrRec(HWND hDlg)
{
    WORD    wI;

    /* ----- Clear PLU Number ----- */
    SendDlgItemMessage(hDlg, IDD_A68_NO, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A68_NO, WM_CLEAR,  0, 0L);

    /* ----- Clear PLU Mnemonic ----- */
    SendDlgItemMessage(hDlg, IDD_A68_MNEMO, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A68_MNEMO, WM_CLEAR,  0, 0L);
    SendDlgItemMessage(hDlg, IDD_A68_ALTMNEMO, EM_SETSEL, 0, MAKELONG(0, -1));
    SendDlgItemMessage(hDlg, IDD_A68_ALTMNEMO, WM_CLEAR,  0, 0L);

    /* ----- Clear Department Number ----- */
    SendDlgItemMessage(hDlg, IDD_A68_DEPTNO, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_DEPTNO, WM_CLEAR,  0, 0L);*/
    SetDlgItemInt(hDlg, IDD_A68_DEPTNO, A68_DEPT_MIN, FALSE);

    /* ----- Clear Report Code ----- */
    SendDlgItemMessage(hDlg, IDD_A68_RPTCD, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_RPTCD, WM_CLEAR,  0, 0L);*/
    SetDlgItemInt(hDlg, IDD_A68_RPTCD, A68_DATA_MIN, FALSE);

    /* ----- Clear Bonus Total Index ----- */
    SendDlgItemMessage(hDlg, IDD_A68_BONUS, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_BONUS, WM_CLEAR,  0, 0L); */
    SetDlgItemInt(hDlg, IDD_A68_BONUS,  A68_DATA_MIN, FALSE);

    /* ----- Clear Table Number ----- */
    SendDlgItemMessage(hDlg, IDD_A68_TBLNO, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_TBLNO, WM_CLEAR,  0, 0L);*/
    SetDlgItemInt(hDlg, IDD_A68_TBLNO,  A68_DATA_MIN, FALSE);

    /* ----- Clear Group Number ----- */
    SendDlgItemMessage(hDlg, IDD_A68_GRPNO, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_GRPNO, WM_CLEAR,  0, 0L);*/
    SetDlgItemInt(hDlg, IDD_A68_GRPNO,  A68_DATA_MIN, FALSE);

    /* ----- Clear Print Priorty ----- */
    SendDlgItemMessage(hDlg, IDD_A68_PRTPRY, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_PRTPRY, WM_CLEAR,  0, 0L);*/
    SetDlgItemInt(hDlg, IDD_A68_PRTPRY, A68_DATA_MIN, FALSE);

    /* ----- Clear PPI ----- */
    SendDlgItemMessage(hDlg, IDD_A68_PPI, EM_SETSEL, 0, MAKELONG(0, -1));
    /*SendDlgItemMessage(hDlg, IDD_A68_PPI, WM_CLEAR,  0, 0L);*/
    SetDlgItemInt(hDlg, IDD_A68_PPI, A68_DATA_MIN, FALSE);

    /* ----- Clear Price Mult. ,Saratoga ----- */
    SendDlgItemMessage(hDlg, IDD_A68_MULTI, EM_SETSEL, 0, MAKELONG(0, -1));
    SetDlgItemInt(hDlg, IDD_A68_MULTI, A68_NO_MIN, FALSE);

    /* ----- Clear Link PLU ,Saratoga ----- */
    SendDlgItemMessage(hDlg, IDD_A68_LINK, EM_SETSEL, 0, MAKELONG(0, -1));
    SetDlgItemInt(hDlg, IDD_A68_LINK, A68_DATA_MIN, FALSE);

    /* ----- Clear Sales Code ,Saratoga ----- */
    SendDlgItemMessage(hDlg, IDD_A68_SALES, EM_SETSEL, 0, MAKELONG(0, -1));
    SetDlgItemInt(hDlg, IDD_A68_SALES, A68_DATA_MIN, FALSE);

    /* ----- Clear Family Code ,Saratoga ----- */
    /* ### ADD Saratoga Family Code (V1_0.02)(051800) */
    SendDlgItemMessage(hDlg, IDD_A68_FAMILY, EM_SETSEL, 0, MAKELONG(0, -1));
    SetDlgItemInt(hDlg, IDD_A68_FAMILY, A68_DATA_MIN, FALSE);

    /* ----- Clear Color Palette ----- */
    SendDlgItemMessage(hDlg, IDD_A68_COLOR, EM_SETSEL, 0, MAKELONG(0, -1));
    SetDlgItemInt(hDlg, IDD_A68_COLOR, A68_DATA_MIN, FALSE);

    /* ----- Clear Preset Price ----- */
    for (wI = IDD_A68_PRESET1; wI <= IDD_A68_PRESET5; wI++) {
        SendDlgItemMessage(hDlg, wI, EM_SETSEL, 0, MAKELONG(0, -1));
        SendDlgItemMessage(hDlg, wI, WM_CLEAR,  0, 0L);
    }

    /* ----- Clear Control Code ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A68_CTLLIST, LB_RESETCONTENT, 0, 0L);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068SetCtrlList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPA68DATA lpPlu   -   Address of PLU Data Structure
*
**  Return      :   No return value
*
**  Description :
*       This procedure inserts the control code description to control code
*   listbox with current PLU data.
* ===========================================================================
*/
VOID    A068SetCtrlList(HWND hDlg, LPA68DATA lpPlu)
{
    LPCWSTR  lpszDesc;
    UINT     wI, wTbl;
    WCHAR    szWork[128], szDesc[128];

    /* ----- Reset Control Code ListBox ----- */
    SendDlgItemMessage(hDlg, IDD_A68_CTLLIST, LB_RESETCONTENT, 0, 0L);

    /* ----- Check Address 3 & 4 ----- */
    for (wI = 0, wTbl = 0; wI < A68_CTRL_BIT; wI++) {
        /* ----- Check Target Bit is Reserved Area or Not ----- */
        if (A68_CTRL_BIT1 & (A68_BIT_CHK << wI)) {
            /* ----- Set Address of Control Code Description ----- */
            lpszDesc = (lpPlu->bControl[A68_CTRL_2] & (A68_BIT_CHK << wI)) ? (lpPluDesc + wTbl)->szCtrlON : (lpPluDesc + wTbl)->szCtrlOFF;

            /* ----- Set Control Code Descriptionto ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

            wTbl++;
        }
    }

    /* ----- Check Address 5 & 6 ----- */
    if (uchTaxMode == A68_TAX_CAN) { /* Canadian Tax Mode */

        wI = IDS_A68_STS_TAX0 + (lpPlu->bControl[A68_CTRL_3] & A68_TAX_BIT);

        /* ----- Load Canadian Tax String from Resource ----- */
        LoadString(hResourceDll, wI, szWork, PEP_STRING_LEN_MAC(szWork));

        DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

        /* ----- Set Start Counter for Address 6 ----- */
        wI    = A68_DISC_OFS;
        wTbl += A68_DISC_NUM;

    } else if (uchTaxMode == A68_TAX_US) {       /* U.S. Tax Mode */

        wI = 0;

    } else { /* VAT Mode */
        wI = IDS_A68_VATRADIO1 + (lpPlu->bControl[A68_CTRL_3] & A68_TAX_BIT_VAT);

        /* ----- Load International VAT String from Resource ----- */
        LoadString(hResourceDll, wI, szWork, PEP_STRING_LEN_MAC(szWork));

        DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

        /* ----- Set Bit 3 Control Code Description ----- */
        LoadString(hResourceDll, IDS_A68_VATSERON, szDesc, PEP_STRING_LEN_MAC(szDesc));
        LoadString(hResourceDll, IDS_A68_VATSEROFF, szWork, PEP_STRING_LEN_MAC(szWork));

        lpszDesc = (lpPlu->bControl[A68_CTRL_3] & A68_TAX_BIT_SERVICE) ? szDesc : szWork;

        /* ----- Set Control Code Description to ListBox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

        /* ----- Set Start Counter for Address 6 ----- */
        wI    = A68_DISC_OFS;
        wTbl += A68_DISC_NUM;
    }

    for (; wI < A68_CTRL_BIT; wI++) {

        /* ----- Check Target Bit is Reserved Area or Not ----- */
        if (A68_CTRL_BIT2 & (A68_BIT_CHK << wI)){

            /* ----- Set Address of Control Code Description ----- */
            lpszDesc = (lpPlu->bControl[A68_CTRL_3] & (A68_BIT_CHK << wI)) ? (lpPluDesc + wTbl)->szCtrlON : (lpPluDesc + wTbl)->szCtrlOFF;

            /* ----- Set Control Code Descriptionto ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

            wTbl++;
        }
    }

    /* ### ADD Saratoga Food Stamp (052300) */
    if((uchTaxMode == A68_TAX_US)) {
        if(lpPlu->bControl[A68_CTRL_3] & A68_FOODSTAMP_BIT_US)
            LoadString(hResourceDll, IDS_A68_STS_134, szDesc, PEP_STRING_LEN_MAC(szDesc));
        else
            LoadString(hResourceDll, IDS_A68_STS_034, szDesc, PEP_STRING_LEN_MAC(szDesc));
        DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)szDesc);
    }

    /* ----- Check Address 7 & 8 ----- */

    for (wI = 0; wI < A68_CTRL_BIT; wI++) {

        /* ----- Check Target Bit is Reserved Area or Not ----- */
        if (A68_CTRL_BIT3 & (A68_BIT_CHK << wI)) {

            /* ----- Set Address of Control Code Description ----- */
            lpszDesc = (lpPlu->bControl[A68_CTRL_4] & (A68_BIT_CHK << wI)) ? (lpPluDesc + wTbl)->szCtrlON : (lpPluDesc + wTbl)->szCtrlOFF;

            /* ----- Set Control Code Descriptionto ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

            wTbl++;
        }
    }

    /* ----- Check Address 18 ----- */
    for (wI = 0; wI < A68_CTRL_BIT; wI++) {
        /* ----- Check Target Bit is Reserved Area or Not ----- */
        if (A68_CTRL_BIT6 & (A68_BIT_CHK << wI)) {
            /* ----- Set Address of Control Code Description ----- */
            lpszDesc = (lpPlu->bControl[A68_CTRL_7] & (A68_BIT_CHK << wI)) ? (lpPluDesc + wTbl)->szCtrlON : (lpPluDesc + wTbl)->szCtrlOFF;

            /* ----- Set Control Code Descriptionto ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

            wTbl++;
        }
    }

    /* ----- Check Address 9 & 10 ----- */
    for (wI = 0; wI < A68_CTRL_BIT; wI++) {
        /* ----- Check Target Bit is Reserved Area or Not ----- */
        if (A68_CTRL_BIT4 & (A68_BIT_CHK << wI)) {

            /* ----- Set Address of Control Code Description ----- */
            lpszDesc = (lpPlu->bControl[A68_CTRL_5] & (A68_BIT_CHK << wI)) ? (lpPluDesc + wTbl)->szCtrlON : (lpPluDesc + wTbl)->szCtrlOFF;

            /* ----- Set Control Code Descriptionto ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

            wTbl++;
        }
    }

    /* ----- Check Address 11 & 12 ----- */
    for (wI = 0; wI < A68_CTRL_BIT; wI++) {

        /* ----- Check Target Bit is Reserved Area or Not ----- */
        if (A68_CTRL_BIT5 & (A68_BIT_CHK << wI)) {

            /* ----- Set Address of Control Code Description ----- */
            lpszDesc = (lpPlu->bControl[A68_CTRL_6] & (A68_BIT_CHK << wI)) ? (lpPluDesc + wTbl)->szCtrlON : (lpPluDesc + wTbl)->szCtrlOFF;

            /* ----- Set Control Code Descriptionto ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpszDesc);

            wTbl++;
        }
    }

    /* ----- Check Adjective Key Type ----- */
    wI = IDS_A68_STS_ADJ0 + A68GETADJKEY(lpPlu->bControl[A68_CTRL_6]);

    /* ----- Load Adjective Control String from Resource ----- */
    LoadString(hResourceDll, wI, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Insert Loaded String to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

    /* ----- Check Address 19, Age Type ( Saratoga ) ----- */
    wI = IDS_A68_STS_TYPEKEY0 + A68GETTYPEKEY(lpPlu->bControl[A68_CTRL_7]);

    LoadString(hResourceDll, wI, szWork, PEP_STRING_LEN_MAC(szWork));

    /* ----- Insert Loaded String to Control Code ListBox ----- */
    DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szWork));

    /* ----- Check Address 19, WIC ( Saratoga ) ----- */
    /* ### ADD Saratoga WIC (052700) */
        if(lpPlu->bControl[A68_CTRL_7] & A68_WIC_BIT_US)
            wsPepf(szDesc,WIDE("WIC item   "));
        else
            wsPepf(szDesc,WIDE("Non WIC item   "));
        DlgItemSendTextMessage(hDlg, IDD_A68_CTLLIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)szDesc);

}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068SetListDesc()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value
*
**  Description :
*       This procedure sets the data in the dialogbox(When Initial).
* ===========================================================================
*/
VOID    A068SetListDesc(HWND hDlg, int iListId, PLUIF *pPluData)
{
    WCHAR    szcom[] = WIDE(" : ");
	WCHAR    szWork[PLU_MAX_DIGIT + 1] = {0};
    USHORT  usLen = 0;

    RflConvertPLU (szWork, pPluData->auchPluNo);

    usLen = (USHORT)wcslen(szWork);

    if (usLen) {
		WCHAR    szName[OP_PLU_NAME_SIZE + 1] = {0};
		WCHAR    szDesc[50] = {0};

        /* ----- Replace Double Key Code (0x12 -> '~') ----- */
        PepReplaceMnemonic (pPluData->ParaPlu.auchPluName, szName, (A68_MNEMO_SIZE ), PEP_MNEMO_READ);

        /* ----- Create Description for Insert ListBox ----- */
        wsPepf(szDesc, WIDE("%14s"), szWork);
        szDesc[OP_PLU_LEN + 1] = 0x00;
        wcscat(szDesc, szcom);
        wcsncat(szDesc, szName, 50);

        /* ----- Insert Created Description to PLU ListBox ----- */
        DlgItemSendTextMessage(hDlg, iListId, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szDesc));
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068RefreshList()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wParam  -   User Selected Button ID
*                   UINT    unType  -   PLU Type of Current Selected PLU
*
**  Return      :   No return value.
*
**  Description :
*       This procedure redraws a PLU listbox.
* ===========================================================================
*/
VOID    A068RefreshList(HWND hDlg, WPARAM wParam, UINT unType)
{
    BOOL    fCheck = FALSE;
    DWORD   dwIndex;
	WCHAR   szPlu[PLU_MAX_DIGIT + 1] = {0}, szBuff[48] = {0}, szWork[48] = {0};
    WCHAR   szcom[] = WIDE(" : ");

    /* ----- Reform Description for Insert ListBox ----- */
    PepReplaceMnemonic (tempPlu.ParaPlu.auchPluName, szWork, (A68_MNEMO_SIZE ), PEP_MNEMO_READ);
    memcpy(szPlu, tempPlu.auchPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));
    RflConvertPLU (szPlu, tempPlu.auchPluNo);

    wsPepf(szBuff, WIDE("%14s"), szPlu);
    szBuff[PLU_MAX_DIGIT] = 0x00;
    wcscat(szBuff, szcom);
    wcscat(szBuff, szWork);

    /* ----- Create String for Search PLU No. in ListBox ----- */
    wsPepf(szWork, WIDE("%14s "), szPlu);

    /* ----- Search for Corresponding PLU in PLU ListBox ----- */
    dwIndex = DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szWork));

    /* ----- Determine Whether Target PLU Data Exist or Not ----- */
    if (dwIndex == LB_ERR) {    /* Not Found Target PLU No. */
        /* ----- Insert New Data into PLU ListBox ----- */
        DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)(szBuff));

        /* ----- Get No. of Current Records ----- */
        dwIndex = SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_GETCOUNT, 0, 0L);

        /* ----- Adjust Cursor Position ----- */
        -- dwIndex;

        /* ----- No. of PLU Records is Changed (One Record Increment) ----- */
        fCheck = TRUE;

    } else {                    /* Found in PLU Records */
        if (wParam == IDD_A68_DEL) {     /* Delete Button is Pressed */
            /* ----- Delete a Target String from PLU ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

            if (dwIndex) {                  /* Remain PLU Records */
                /* ----- Adjust Cursor Position ----- */
                -- dwIndex;
            } else {                        /* Delete All PLU Record */
                /* ----- Clear Temporary PLU Data Area ----- */
                memset(&tempPlu, 0, sizeof(PLUIF));
            }

            /* ----- No. of PLU Record is Changed (One Record Decrement) ----- */
            fCheck = TRUE;
        } else {                        /* Change PLU Record */
            /* ----- Delete Old String from PLU ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_DELETESTRING, (WPARAM)dwIndex, 0L);

            /* ----- Insert New Description to PLU ListBox ----- */
            DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_INSERTSTRING, (WPARAM)dwIndex, (LPARAM)(szBuff));

            /* ----- No. of PLU Record is Not Changed ----- */
            fCheck = FALSE;
        }
    }

    /* ----- Determine Whether No. of PLU Record is Changed or Not ----- */
    if (fCheck) {
        /* ----- Compute No. of PLU Records ----- */
        if (wParam == IDD_A68_DEL) {     /* Delete Button is Pressed     */
            ulCurPlu -= (unType == A68_TYPE_ONEADJ) ? A68_ADJ_NUM : 1;

            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_A68_CUR, szWork, PEP_STRING_LEN_MAC(szWork));

            /* ----- Set No. of Current PLU Records ----- */
            wsPepf(szBuff, szWork, ulCurPlu);

            DlgItemRedrawText(hDlg, IDD_A68_CUR, szBuff);
        }
    }

    /* ----- Set Cursor to Inserted String ----- */
    SendDlgItemMessage(hDlg, IDD_A68_LIST, LB_SETCURSEL, (WPARAM)dwIndex, 0L);

    if (wParam == IDD_A68_DEL) {     /* Delete Button is Pressed */
        /* ----- Show Next PLU Records to Each Control ----- */
        PostMessage(hDlg, WM_COMMAND, MAKELONG(IDD_A68_LIST, LBN_SELCHANGE), 0L);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A068RegistData()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT    unPluType   -   Current Maintenance PLU Type
*                   LPDWORD lpadwPrePrc -   Address of Preset Price Array
*
**  Return      :   BOOL    TRUE        -   Fail to Register PLU Data
*                           FALSE       -   Succeed in Register PLU Data
*
**  Description :
*       This procedure register inputed PLU data to PLU file. If No. of PLU
*   records is over than maximum No. of PLU records, it return TRUE. And If
*   inputed PLU type is different from previous PLU type, and user selects
*   CANCEL in MessageBox, it returns TRUE.
*       Otherwise it returns FALSE.
* ===========================================================================
*/
BOOL A068RegistData(HWND hDlg, UINT unPluType, LPDWORD lpadwPrePrc)
{
	PLUIF   PluWork = {0};
    BOOL    fRet = FALSE;
    UINT    unType;
    ULONG   ulCur;
    short   nStatus;
    WORD    wI;
    WCHAR   szDesc[128], szWork[128];
    USHORT  usI;

    /* ----- ###Sara### Plu check digit ----- */
    for ( usI = 0; usI < OP_PLU_LEN; usI++) {
        if ((tempPlu.auchPluNo[usI] == 0x00) || (tempPlu.auchPluNo[usI] == 0x20)) {
            tempPlu.auchPluNo[usI] = 0x30;
        }
    }

    /* ----- Determine Whether Target Data Already Exists in PLU File ----- */
    memcpy(PluWork.auchPluNo, tempPlu.auchPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

    PluWork.uchPluAdj = tempPlu.uchPluAdj;
    nStatus = OpPluIndRead(&PluWork, 0);

    if (nStatus == OP_NOT_IN_FILE) {    /* Not Found in PLU File    */
        /* ----- Check Max. Record No. of PLU Records ----- */
        if (! A068ChkMaxRec(unPluType)) {       /* Not Over Max. Record */
            ulCur = ulCurPlu;
            /* ----- Set PLU Type ----- */            
            if (unPluType == A68_TYPE_OPEN) {
               tempPlu.ParaPlu.ContPlu.uchItemType = PLU_OPEN_TYP;
            } else if (unPluType == A68_TYPE_NONADJ) {
              tempPlu.ParaPlu.ContPlu.uchItemType = PLU_NON_ADJ_TYP;
            } else {
              tempPlu.ParaPlu.ContPlu.uchItemType = PLU_ADJ_TYP;
            }

            /* ----- Set Preset Price ----- */
            PepConv4Lto3C((LPBYTE)tempPlu.ParaPlu.auchPrice, *lpadwPrePrc);

            /* ----- Assign PLU Record ----- */
            OpPluAssign(&tempPlu, 0);
            ulCur++;
            
            /* ----- Check PLU Type is One Adjective or Not ----- */
            if (unPluType == A68_TYPE_ONEADJ) {
                for (wI = A68_ADJ_NO2; wI <= A68_ADJ_NUM; wI++) {
                    /* ----- Assign Adjective # 2 - 5 ----- */
                    tempPlu.uchPluAdj = (UCHAR)wI;
                    PepConv4Lto3C((LPBYTE)tempPlu.ParaPlu.auchPrice, *(++lpadwPrePrc));
                    OpPluAssign(&tempPlu, 0);
                    /* ulCur++; */ /* Saratoga */
                }
            }

            /* ----- Store Current Exist PLU Data to Static Area ----- */
            ulCurPlu = ulCur;
        } else {                            /* Over Max. PLU Record */
            /* ----- Load String from Resource ----- */
            LoadString(hResourceDll, IDS_A68_OVERREC, szDesc, PEP_STRING_LEN_MAC(szDesc));
            A068DispErr(hDlg, szDesc, MB_OK | MB_ICONEXCLAMATION);
            fRet = TRUE;
        }
    } else {                            /* Found in PLU Records */
        /* ----- Get Previous PLU Type ----- */
        unType = (UINT)PluWork.ParaPlu.ContPlu.uchItemType;

        if (A068ChkPluType(hDlg, unPluType, (unType >> A68_DEPT_SFT))) {
            ulCur = ulCurPlu;

            /* ----- Set PLU Type ----- */
            if (unPluType == A68_TYPE_OPEN) {
               tempPlu.ParaPlu.ContPlu.uchItemType = PLU_OPEN_TYP;
            } else if (unPluType == A68_TYPE_NONADJ) {
               tempPlu.ParaPlu.ContPlu.uchItemType = PLU_NON_ADJ_TYP;
            } else {
               tempPlu.ParaPlu.ContPlu.uchItemType = PLU_ADJ_TYP;
            }

            /* ----- Set Preset Price ----- */
            PepConv4Lto3C((LPBYTE)tempPlu.ParaPlu.auchPrice, *lpadwPrePrc);

            /* ----- Assign PLU Record ----- */
            OpPluAssign(&tempPlu, 0);
            ulCur = (unPluType != (unType >> A68_DEPT_SFT)) ? (ulCur + 1L) : ulCur;
            /* ----- Check PLU Type is One Adjective or Not ----- */
            if (unPluType == A68_TYPE_ONEADJ) {
                for (wI = A68_ADJ_NO2; wI <= A68_ADJ_NUM; wI++) {
                    /* ----- Assign Adjective # 2 - 5 ----- */
                    tempPlu.uchPluAdj = (UCHAR)wI;
                    PepConv4Lto3C((LPBYTE)tempPlu.ParaPlu.auchPrice, *(++lpadwPrePrc));
                    OpPluAssign(&tempPlu, 0);
                }
            }

            /* ----- Store Current Exist PLU Data to Static Area ----- */
            ulCurPlu = ulCur;
        } else {        /* Plu Type is Different */
            fRet = TRUE;
        }
    }

    /* ----- Load String from Resource ----- */
    LoadString(hResourceDll, IDS_A68_CUR, szWork, PEP_STRING_LEN_MAC(szWork));
    /* ----- Create Current Record No. String ----- */
    wsPepf(szDesc, szWork, ulCurPlu);
    /* ----- Set Current PLU Record No. (Prog. #2) ----- */
    DlgItemRedrawText(hDlg, IDD_A68_CUR, szDesc);

    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A068DeleteData()
*
**  Input       :   LPUINT  lpunPluType -   Address of PLU Type Area
*
**  Return      :   BOOL    TRUE        -   Fail to Delete PLU Data
*                           FALSE       -   Succeed in Delete PLU Data
*
**  Description :
*       This procedure deletes a specified PLU record from PLU file.
* ===========================================================================
*/
BOOL    A068DeleteData(LPUINT lpunPluType)
{
    BOOL    fRet = TRUE;
    WORD    wI;
    short   nStatus;
    PLUIF   PluWork;

    /* ----- Determine Whether Target Data Exists in PLU File ----- */
    memcpy(PluWork.auchPluNo, tempPlu.auchPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));
    PluWork.uchPluAdj = A68_ADJ_NO1;
    nStatus = OpPluIndRead(&PluWork, 0);
    if (nStatus != OP_NOT_IN_FILE) {    /* Target PLU is Found in PLU File  */

        *lpunPluType = (UINT)PluWork.ParaPlu.ContPlu.uchItemType;

        /* ----- Delete a Specified PLU Record ----- */
        OpPluDelete(&tempPlu, 0);

        /* ----- Check PLU Type is One Adjective or Not ----- */
        if (*lpunPluType == A68_TYPE_ONEADJ) {
            for (wI = A68_ADJ_NO2; wI <= A68_ADJ_NUM; wI++) {
                /* ----- Delete Adjective # 2 - 5 ----- */
                tempPlu.uchPluAdj = (UCHAR)wI;
                OpPluDelete(&tempPlu, 0);
            }
        }
        fRet = FALSE;
    }
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A068ChkMaxRec()
*
**  Input       :   UINT    unType  -   Current Maintenance PLU Type
*
**  Return      :   BOOL    TRUE    -   No. of PLU Record is Over Maximum.
*                           FALSE   -   No. of PLU Record is within Maximum.
*
**  Description :
*       This procedure compares No. of current records with maximum no. of
*   PLU records. It returns TRUE, if No. of Current records is over than
*   maximum No. of PLU records. Otherwise it returens FALSE.
* ===========================================================================
*/
BOOL    A068ChkMaxRec(UINT  unType)
{
    BOOL    fRet;
    UINT    unValue;

    /* ----- Check Current Maintenance PLU Type is One Adjective ----- */
    unValue = (unType == A68_TYPE_ONEADJ) ? A68_ADJ_NUM : A68_MAXCHK;
    /* ----- Check No. of Records with Max. Record No. ----- */
    fRet = (ulPLUMax < (ULONG)(ulCurPlu + (ULONG)unValue)) ? TRUE : FALSE;
    return (fRet);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068DispErr()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   LPCSTR  lpszErr -   Address of Error Message String
*                   UINT    fuStyle -   MessageBox Style
**
*   Return  :   None
*
**  Description:
*       This procedure sounds beep tone, and displays error message.
* ===========================================================================
*/
VOID    A068DispErr(HWND hDlg, LPCWSTR lpszErr, UINT fuStyle)
{
    WCHAR    szCaption[PEP_CAPTION_LEN];

    /*  ----- Load Caption from Resource ----- */
    LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szCaption, PEP_STRING_LEN_MAC(szCaption)); 
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBoxPopUp(hDlg, lpszErr, szCaption, fuStyle);
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068FinDlg()
*
**  Input       :   HWND        hDlg    -   Window Handle of a DialogBox
*                   WORD        wParam  -   User Selected Button ID
*                   LPHGLOBAL   lphDesc -   Handle of a String Heap Area
*
**  Return      :   No return value.
*
**  Description :
*       This procedure saves inputed PLU data, and sets file status flag, if
*   user selects OK button. Otherwise it restores PLU file with backup file.
*   And then deletes backup file.
* ===========================================================================
*/
VOID    A068FinDlg(HWND hDlg, WPARAM wParam, LPHGLOBAL lphDesc)
{
    WCHAR    szFile[PEP_FILE_BACKUP_LEN];
    HCURSOR  hCursor;

    /* ----- Change cursor (arrow -> hour-glass) ----- */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    if (LOWORD(wParam) == IDOK) {
        /* ----- Save Inputed PLU Data to PLU File ----- */
        OpPluFileUpdate(0);
        /* ----- Set PLU File Status Flag to ON ----- */
        PepSetModFlag(hwndActMain, PEP_MF_PLU, 0);
    } else {
        /* ----- Load PLU File Name from Resource ----- */
        LoadString(hResourceDll, IDS_FILE_NAME_PLU, szFile, PEP_STRING_LEN_MAC(szFile));
        /* ----- Restore PLU File with Backup File ----- */
        PepFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
        /* ----- Load PLU Index File Name from Resource ----- */
		LoadString(hResourceDll, IDS_FILE_NAME_PLUINDEX, szFile, PEP_STRING_LEN_MAC(szFile));
        /* ----- Restore PLU Index File with Backup File ----- */
        PepIndexFileBackup(hDlg, szFile, PEP_FILE_RESTORE);
    }

    GlobalUnlock(*lphDesc);
    GlobalFree(*lphDesc);

    /* ----- Restore cursor (hour-glass -> arrow) ----- */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    /* ----- Delete Backup File ----- */
    PepFileDelete();

    PepIndexFileDelete();
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068CtrlButton()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets PLU No. from EditText, and then it controls
*   pushbuttons status. It enables pushbuttons, if PLU No. exists in edittext.
*   Otherwise it disables pushbuttons.
* ===========================================================================
*/
VOID    A068CtrlButton(HWND hDlg)
{
    DWORD   dwIndex;
    UINT    unPLUNo, unDept;
    BOOL    fCheck, fDept;
    WCHAR   szDesc[16];
	WCHAR   szPluNo[PLU_MAX_DIGIT + 1] = {0};

    /* ----- Check Data Exist in PLU No. EditText ----- */
    unPLUNo = DlgItemGetText(hDlg, IDD_A68_NO, szPluNo, PLU_MAX_DIGIT + 1);
    unDept  = GetDlgItemInt(hDlg, IDD_A68_DEPTNO, (LPBOOL)&fDept, FALSE);

    fCheck = ((fDept == 0) || (unPLUNo == 0)) ? FALSE : TRUE;

    /* ----- Enabel / Disable PushButtons ----- */
/* Saratoga 
    EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL),    fCheck); 
    EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), fCheck);
*/
    EnableWindow(GetDlgItem(hDlg, IDD_SET),        fCheck);
    EnableWindow(GetDlgItem(hDlg, IDD_A68_NORMAL), fCheck);
    EnableWindow(GetDlgItem(hDlg, IDD_A68_EVER),   fCheck);

    if (fCheck == TRUE) {           /* Inputed Value is Valid   */
        /* ----- Create String for Search Dept No. into ListBox ----- */
        wsPepf(szDesc, WIDE("%14s "),  szPluNo);
        dwIndex = DlgItemSendTextMessage(hDlg, IDD_A68_LIST, LB_FINDSTRING, (WPARAM)-1, (LPARAM)(szDesc));
        fCheck = (dwIndex == LB_ERR) ? FALSE : TRUE;

        EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL), fCheck);
        EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), fCheck);

        if (! unPLUNo) {
            /* ----- Set Focus to Dept No. EditText ----- */
            SetFocus(GetDlgItem(hDlg, IDD_A68_NO));
        }
    }
}

/*
* ===========================================================================
**  Synopsis    :   VOID    A068ChkRng()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*                   WORD    wEditID -   Inputed EditText ID
*
**  Return      :   No return value.
*
**  Description :
*       This procedure gets inputed value from specified edittext, and then
*   it determines inputed data is out of range or not.
*       It displays error message with MessageBox, if inputed data is out of 
*   range.
* ===========================================================================
*/
VOID    A068ChkRng(HWND hDlg, WORD wEditID)
{
    UINT    unValue, unMax;
    WCHAR   szDesc[PEP_LOADSTRING_LEN], szWork[PEP_LOADSTRING_LEN];

    /* ----- Set Max. Range of Input Data ----- */
	unMax = A68_BTTL_MAX;
	switch (wEditID) {
		case IDD_A68_DEPTNO:  unMax = A68_DEPT_MAX;  break;
		case IDD_A68_RPTCD:  unMax = A68_RPT_MAX;  break;
		case IDD_A68_TBLNO:  unMax = A68_TBLNO_MAX;  break;
		case IDD_A68_GRPNO:  unMax = A68_GRPNO_MAX;  break;
		case IDD_A68_PRTPRY:  unMax = A68_PRTPRY_MAX;  break;
		case IDD_A68_PPI:  unMax = usPPIMax;    /*A68_PPI_MAX*/  break;
		case IDD_A68_SALES:  unMax = A68_SALES_MAX;  break;
		case IDD_A68_TARE:  unMax = A68_TARE_MAX;  break;
		case IDD_A68_COLOR:  unMax = A68_COLOR_MAX;  break;
		default:  unMax = A68_BTTL_MAX;  break;
	}

    /* ----- Get Inputed Value from Specified EditText ----- */
    unValue = GetDlgItemInt(hDlg, wEditID, NULL, FALSE);
    if (unValue > unMax) {
		UINT    unMin = (wEditID == IDD_A68_DEPTNO) ? A68_DEPT_MIN : 0;

        /* ----- Load Error Message from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_OVERRANGE, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Create Error Message with Data Range ----- */
        wsPepf(szDesc, szWork, unMin, unMax);

        /* ----- Display Error Message ----- */
        A068DispErr(hDlg, szDesc, MB_OK | MB_ICONEXCLAMATION);

        /* ----- Undo Data Input ----- */
        SendDlgItemMessage(hDlg, wEditID, EM_UNDO, 0, 0L);
        SendDlgItemMessage(hDlg, wEditID, EM_SETSEL, 0, MAKELONG(0, -1));
    } else if (wEditID == IDD_A68_DEPTNO) {
		BOOL    fCheck;
		UINT    unPlu;
		WCHAR   szPluNo[OP_PLU_LEN + 1] = {0};

        /* ----- Get PLU No. from EditText ----- */
        unPlu = DlgItemGetText(hDlg, IDD_A68_NO, szPluNo, sizeof(szPluNo)/sizeof(szPluNo[0]));  /* Saratoga */

        /* ----- Check Inputed Dept. Value with Minimum Value of Range ----- */
        fCheck = ((unValue <= wDeptMax) && (unValue >= A68_DEPT_MIN) && (unPlu != 0)) ? TRUE : FALSE;

        /* ----- Enable / Disable Buttons ----- */
        EnableWindow(GetDlgItem(hDlg, IDD_SET),       fCheck);
        EnableWindow(GetDlgItem(hDlg, IDD_A68_DEL),    fCheck);
        EnableWindow(GetDlgItem(hDlg, IDD_A68_ADDCHG), fCheck);
    }
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A068ChkDelConf()
*
**  Input       :   HWND    hDlg    -   Window Handle of a DialogBox
*
**  Return      :   BOOL    IDOK    -   Delete Current PLU Record.
*                           IDCANCEL-   Not Delete Current PLU Record.
*
**  Description :
*       This procedure checks current delete confirmation flag. And then it
*   displays messagebox with warning message, if confirmation flag is on.
*       It returns IDOK, user selects delete current selected PLU record.
*   Otherwise it returns IDCANCEL.
* ===========================================================================
*/
BOOL    A068ChkDelConf(HWND hDlg)
{
    BOOL    fRet = IDOK;

    if (fDelConf == A68_DELCONF) {
		WCHAR    szMsg[PEP_OVER_LEN] = {0}, szDesc[PEP_OVER_LEN] = {0}, szCap[PEP_CAPTION_LEN] = {0};
		WCHAR    szPlu[PLU_MAX_DIGIT + 1] = {0};

        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szCap, PEP_STRING_LEN_MAC(szCap));
        LoadString(hResourceDll, IDS_A68_DELCONF, szMsg, PEP_STRING_LEN_MAC(szMsg));

        RflConvertPLU(szPlu, tempPlu.auchPluNo);
        wsPepf(szDesc, szMsg, szPlu);

        /* ----- Display MessageBox ----- */
        MessageBeep(MB_ICONEXCLAMATION);

        fRet = MessageBoxPopUp(hDlg, szDesc, szCap, MB_ICONEXCLAMATION | MB_OKCANCEL);
    }
    return fRet;
}

/*
* ===========================================================================
**  Synopsis    :   BOOL    A068ChkPluType()
*
**  Input       :   HWND    hDlg        -   Window Handle of a DialogBox
*                   UINT    unNewType   -   Inputed PLU Type 
*                   UINT    unOldType   -   Previous PLU Type
*
**  Return      :   BOOL    TRUE        -   Change PLU Type with New PLU Type
*                           FALSE       -   Not Change PLU Type
*
**  Description :
*       This procedure determines the current maintenance PLU type. And it
*   shows messagebox, if current type is different from previous plu type.
* ===========================================================================
*/
BOOL    A068ChkPluType(HWND hDlg, UINT unNewType, UINT unOldType)
{
    PLUIF   PluWork;
    WORD    wI;
    ULONG   ulCur = ulCurPlu;
    BOOL    fRet = TRUE, fCheck;
    WCHAR   szCaption[PEP_CAPTION_LEN],
            szOldType[48],
            szNewType[48],
            szWork[256],
            szDesc[256];

    if (unNewType != unOldType) {

        /* ----- Load String from Resource ----- */
        LoadString(hResourceDll, IDS_PEP_CAPTION_A68, szCaption, PEP_STRING_LEN_MAC(szCaption));
        LoadString(hResourceDll, IDS_A68_OPENPLU + unOldType - 1, szOldType, PEP_STRING_LEN_MAC(szOldType));
        LoadString(hResourceDll, IDS_A68_OPENPLU + unNewType - 1, szNewType, PEP_STRING_LEN_MAC(szNewType));
        LoadString(hResourceDll, IDS_A68_CHGTYPE, szWork, PEP_STRING_LEN_MAC(szWork));

        /* ----- Create Description for MessageBox ----- */
        wsPepf(szDesc, szWork, szOldType, szNewType);

        /* ----- Display MessageBox ----- */
        MessageBeep(MB_ICONEXCLAMATION);

        fCheck = MessageBoxPopUp(hDlg, szDesc, szCaption, MB_ICONQUESTION | MB_OKCANCEL);

        if (fCheck == IDOK) {
             if (ulPLUMax >= ulCur) {
                ulCur = ulCurPlu;
                memcpy(PluWork.auchPluNo, tempPlu.auchPluNo, PLU_MAX_DIGIT * sizeof(WCHAR));

                PluWork.uchPluAdj = A68_ADJ_NO1;

                /* ----- Delete a Specified PLU Record ----- */
                OpPluDelete(&PluWork, 0);

                ulCur--;

                /* ----- Check PLU Type is One Adjective or Not ----- */
                if (unOldType == A68_TYPE_ONEADJ) {
                    for (wI = A68_ADJ_NO2; wI <= A68_ADJ_NUM; wI++) {
                        /* ----- Delete Adjective # 2 - 5 ----- */
                        PluWork.uchPluAdj = (UCHAR)wI;

                        OpPluDelete(&PluWork, 0);

                        /*ulCur--;*//* Saratoga */
                    }
                }

                /* ----- Load String from Resource ----- */
                LoadString(hResourceDll, IDS_A68_CUR, szWork, PEP_STRING_LEN_MAC(szWork));

                /* ----- Create Current Record No. String ----- */
                wsPepf(szDesc, szWork, ulCur);

                /* ----- Set Current PLU Record No. (Prog. #2) ----- */
                DlgItemRedrawText(hDlg, IDD_A68_CUR, szDesc);

                /* ----- Store Current Exist PLU Data to Static Area ----- */
                ulCurPlu = ulCur;
            } else {    /* Over maximum Plu records */
                /* ----- Display Error Message ----- */
                LoadString(hResourceDll, IDS_A68_OVERREC, szDesc, PEP_STRING_LEN_MAC(szDesc));

                A068DispErr(hDlg, szDesc, MB_OK | MB_ICONEXCLAMATION);

                fRet = FALSE;
            }
        } else {    /* User selects Cancel Button */
            fRet = FALSE;
        }
    }
    return fRet;
}

/* ===== End of File (A068.C) ===== */
