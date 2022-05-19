// File Conversion Main File
#include <tchar.h>
#include <string.h>


#include "stdafx.h"
#include "file.h"
#include "paraequ.h"
#include "pif.h"
#include "ecr.h"
#include "plu.h"
#include "pararam.h"
#include "para.h"
#include "csstbfcc.h"
#include "csttl.h"
#include "cpm.h"
#include "eept.h"
#include "uie.h"
#include "BlFWif.h"
                                 /* Uninitialize RAM */


/*
*===========================================================================
** Format  : SHORT FileMigration(VOID *pData, ULONG ulData)
*
*   Input  : VOID pData
*   Output : none
*   InOut  : none
** Return  : SUCCESS
*
** Synopsis: Run through initial Checks to test for NHPOS file migration
*===========================================================================
*/
SHORT PifFileMigration(VOID *pData, ULONG ulDataSize, TCHAR *pVersion)
{
	USHORT convNumber;
	TCHAR szFileVersion[FILE_LEN_ID];

	memcpy(&szFileVersion, pData, sizeof(szFileVersion));

	if(ulDataSize == RELEASE_30_SIZE) //Release 30 Size, we use this because there is no file information in that uniniram
	{
		PifFileMigrationGP41(pData, pVersion);
	}
	else if( convNumber = _tcscmp(pVersion,_T("2.0.3")) < 0 ){
//		PifFileMigrationGP203(pData, pVersion);


	}else{ //add other conversion later, use the information copied into szFileVersion compared
		  //against pVersion
	
		return TRUE;
	}


	return TRUE;

}



/*
*===========================================================================
** Format  : SHORT PifFileMigrateGP41(VOID *pData, TCHAR *pVersion)
*
*   Input  : VOID pData
*   Output : none
*   InOut  : none
** Return  : SUCCESS
*
** Synopsis: Converion of Release 2.0.0.30 to current.

/*PLEASE NOTE: This function only changes the information in the UNINIRAM structure.
If PLU,CASHIER,MLD, ETK, DEPARTMENT changes are necessary, those files will need to be
opened as well( for example PARAPLU for the PLU, PARADEPT for Dept, etc etc).


*===========================================================================
*/
SHORT PifFileMigrationGP41(VOID *pData, TCHAR *pVersion)
{
	UNINIRAM_HGV20 OldPara;
	CONST TCHAR		szNull[] = _T(""),
					szSpaceLDT[] = _T("                    "); //Length of PARA_LEADTHRU_LEN_HGV20, 20 spaces
	int i, j;
	BYTE totalKeyOldType, totalKeyOldIndex;
	SHORT sFile;
	ULONG usActualPosition;
	ULONG ulActualBytesRead;


	// We open the backup file that we have saved the previous data that did not fit the size of the
	// current UNINIRAM.  We open the file, and set the data inside to the older file structure so that we
	// can convert the information on the fly.



	sFile = PifOpenFile(_T("NHPOSParaBKUP.dat"), "rw");
	PifSeekFile(sFile, 0, &usActualPosition);
	PifReadFile(sFile, &OldPara, sizeof(OldPara), &ulActualBytesRead);

	memset(&Para, 0x00, sizeof(UNINIRAM));



/* BEGIN PARAMETER CONVERSION ------------------------------------------------------*/

	memcpy(&ParaFileVersion, pVersion, sizeof(Para.szVer));

	//Program 1
	memcpy(&ParaMDC[0], &OldPara.HGV20_ParaMDC[0], sizeof(OldPara.HGV20_ParaMDC));


	//Program 2
	memcpy(&ParaFlexMem, &OldPara.HGV20_ParaFlexMem, sizeof(OldPara.HGV20_ParaFlexMem));


	//Program 3, conversion neccessary.
	memcpy(&ParaFSC[0], &OldPara.HGV20_ParaFSC[0], sizeof(OldPara.HGV20_ParaFSC));

	memcpy(&ParaSecurityNo, &OldPara.HGV20_ParaSecurityNo, sizeof(OldPara.HGV20_ParaSecurityNo));

	memcpy(&ParaStoRegNo, &OldPara.HGV20_ParaStoRegNo, sizeof(OldPara.HGV20_ParaStoRegNo));

	memcpy(&ParaSupLevel, &OldPara.HGV20_ParaSupLevel, sizeof(OldPara.HGV20_ParaSupLevel));

	memcpy(&ParaActCodeSec, &OldPara.HGV20_ParaActCodeSec, sizeof(OldPara.HGV20_ParaActCodeSec));

	memcpy(&ParaTransHALO, &OldPara.HGV20_ParaTransHALO, sizeof(OldPara.HGV20_ParaTransHALO));

	memcpy(&ParaHourlyTime, &OldPara.HGV20_ParaHourlyTime, sizeof(OldPara.HGV20_ParaHourlyTime));

	memcpy(&ParaSlipFeedCtl, &OldPara.HGV20_ParaSlipFeedCtl, sizeof(OldPara.HGV20_ParaSlipFeedCtl));

	memcpy(&Para.ParaTransMnemo, &OldPara.HGV20_ParaTransMnemo, sizeof(OldPara.HGV20_ParaTransMnemo));

	memcpy(Para.ParaLeadThru,OldPara.HGV20_ParaLeadThru,sizeof(OldPara.HGV20_ParaLeadThru));

	memcpy(&Para.ParaSpeMnemo, &OldPara.HGV20_ParaSpeMnemo, sizeof(OldPara.HGV20_ParaSpeMnemo));

	memcpy(&ParaPCIF, &OldPara.HGV20_ParaPCIF, sizeof(OldPara.HGV20_ParaPCIF));

	memcpy(&Para.ParaAdjMnemo, &OldPara.HGV20_ParaAdjMnemo, sizeof(OldPara.HGV20_ParaAdjMnemo));

	memcpy(&ParaPrtModi, &OldPara.HGV20_ParaPrtModi, sizeof(OldPara.HGV20_ParaPrtModi));

	memcpy(&ParaMajorDEPT, &OldPara.HGV20_ParaMajorDEPT, sizeof(OldPara.HGV20_ParaMajorDEPT));

	memcpy(&ParaAutoAltKitch, &OldPara.HGV20_ParaAutoAltKitch, sizeof(OldPara.HGV20_ParaAutoAltKitch));

	//CPMDATA
	{
		memcpy(Para.ParaHotelId.auchHotelId, &OldPara.HGV20_ParaHotelId.auchHotelId,sizeof(OldPara.HGV20_ParaHotelId.auchHotelId));

		memcpy(Para.ParaHotelId.auchSld, &OldPara.HGV20_ParaHotelId.auchSld,sizeof(OldPara.HGV20_ParaHotelId.auchSld));

		memcpy(Para.ParaHotelId.auchProductId, &OldPara.HGV20_ParaHotelId.auchProductId,sizeof(OldPara.HGV20_ParaHotelId.auchProductId));

		for (j = 0; j < 11; j++) {
			Para.ParaHotelId.auchTend[j] = OldPara.HGV20_ParaHotelId.auchTend[j];
		}
		for (j = 0; j < 2; j++) {
			Para.ParaHotelId.auchApproval[j] = OldPara.HGV20_ParaHotelId.auchApproval[j];
		}
	}

	memcpy(&Para.ParaChar24, &OldPara.HGV20_ParaChar24, sizeof(OldPara.HGV20_ParaChar24));

	//TotalKeyConversion
	memset(&ParaTtlKeyTyp, 0x00, sizeof(ParaTtlKeyTyp));
	for (i = 0; i < MAX_KEYTYPE_SIZE_HGV20; i++) {
		totalKeyOldType = P60_HGV20GETTYPE(OldPara.HGV20_ParaTtlKeyTyp[i]);

		totalKeyOldIndex= P60_HGV20GETINDEX(OldPara.HGV20_ParaTtlKeyTyp[i]);

		ParaTtlKeyTyp[i] = P60SETTYPE(totalKeyOldType);
		ParaTtlKeyTyp[i] += P60SETINDEX(totalKeyOldIndex);
	}

	memcpy(&ParaAutoAltKitch, &OldPara.HGV20_ParaAutoAltKitch, sizeof(OldPara.HGV20_ParaAutoAltKitch));

	memcpy(&ParaTtlKeyCtl, &OldPara.HGV20_ParaTtlKeyCtl, sizeof(OldPara.HGV20_ParaTtlKeyCtl));

	memcpy(&ParaTend, &OldPara.HGV20_ParaTend, sizeof(OldPara.HGV20_ParaTend));

	memcpy(&ParaPLUNoMenu, &OldPara.HGV20_ParaPLUNoMenu, sizeof(OldPara.HGV20_ParaPLUNoMenu));

	memcpy(&ParaCtlTblMenu, &OldPara.HGV20_ParaCtlTblMenu, sizeof(OldPara.HGV20_ParaCtlTblMenu));

	memcpy(&ParaManuAltKitch, &OldPara.HGV20_ParaManuAltKitch, sizeof(OldPara.HGV20_ParaManuAltKitch));

	memcpy(&Para.ParaCashABAssign, &OldPara.HGV20_ParaCashABAssign, sizeof(OldPara.HGV20_ParaCashABAssign));

	memcpy(&ParaRound, &OldPara.HGV20_ParaRound, sizeof(OldPara.HGV20_ParaRound));

	memcpy(&ParaDisc, &OldPara.HGV20_ParaDisc, sizeof(OldPara.HGV20_ParaDisc));

	memcpy(&ParaPromotion, &OldPara.HGV20_ParaPromotion, sizeof(OldPara.HGV20_ParaPromotion));

	memcpy(&ParaRound, &OldPara.HGV20_ParaRound, sizeof(OldPara.HGV20_ParaRound));

	memcpy(&ParaCurrency, &OldPara.HGV20_ParaCurrency, sizeof(OldPara.HGV20_ParaCurrency));

	memcpy(&ParaToneCtl, &OldPara.HGV20_ParaToneCtl, sizeof(OldPara.HGV20_ParaToneCtl));

	memcpy(&ParaMenuPLU, &OldPara.HGV20_ParaMenuPLU, sizeof(OldPara.HGV20_ParaMenuPLU));

	memcpy(&Para.ParaTaxTable, &OldPara.HGV20_ParaTaxTable, sizeof(OldPara.HGV20_ParaTaxTable));

	memcpy(&ParaTaxRate, &OldPara.HGV20_ParaTaxRate, sizeof(OldPara.HGV20_ParaTaxRate));

	memcpy(&ParaMiscPara, &OldPara.HGV20_ParaMiscPara, sizeof(OldPara.HGV20_ParaMiscPara));

	memcpy(&ParaTare, &OldPara.HGV20_ParaTare, sizeof(OldPara.HGV20_ParaTare));

	memcpy(&ParaPresetAmount, &OldPara.HGV20_ParaPresetAmount, sizeof(OldPara.HGV20_ParaPresetAmount));

	memcpy(&Para_SharedPrt, &OldPara.HGV20_ParaSharedPrt, sizeof(OldPara.HGV20_ParaSharedPrt));

	memcpy(&ParaSoftChk, &OldPara.HGV20_ParaSoftChk, sizeof(OldPara.HGV20_ParaSoftChk));

	memcpy(&ParaPigRule, &OldPara.HGV20_ParaPigRule, sizeof(OldPara.HGV20_ParaPigRule));

	memcpy(&ParaOep, &OldPara.HGV20_ParaOep, sizeof(OldPara.HGV20_ParaOep));

	memcpy(&ParaFlexDrive, &OldPara.HGV20_ParaFlexDrive, sizeof(OldPara.HGV20_ParaFlexDrive));

	memcpy(&ParaServiceTime, &OldPara.HGV20_ParaServiceTime, sizeof(OldPara.HGV20_ParaServiceTime));

	memcpy(&ParaLaborCost, &OldPara.HGV20_ParaLaborCost, sizeof(OldPara.HGV20_ParaLaborCost));

	memcpy(&Para_DispPara, &OldPara.HGV20_ParaDispPara, sizeof(OldPara.HGV20_ParaDispPara));

	memcpy(&Para_KdsIp, &OldPara.HGV20_ParaKdsIp, sizeof(OldPara.HGV20_ParaKdsIp));

	memcpy(&ParaRestriction, &OldPara.HGV20_ParaRestriction, sizeof(OldPara.HGV20_ParaRestriction));

	memcpy(&ParaBoundAge, &OldPara.HGV20_auchParaBoundAge, sizeof(OldPara.HGV20_auchParaBoundAge));

	memcpy(&ParaDeptNoMenu, &OldPara.HGV20_ParaDeptNoMenu, sizeof(OldPara.HGV20_ParaDeptNoMenu));

	memcpy(&Para.ParaSpcCo[0], &OldPara.HGV20_ParaSpcCo[0], sizeof(OldPara.HGV20_ParaSpcCo));

	memcpy(&ParaChargePostInformation, &OldPara.HGV20_ChargePostInformation, sizeof(OldPara.HGV20_ChargePostInformation));

	memcpy(&ParaTerminalInformationPara, &OldPara.HGV20_TerminalInformation, sizeof(OldPara.HGV20_TerminalInformation));

	memcpy(&ParaEnhEPTInfo, &OldPara.HGV20_EPT_ENH_Information, sizeof(OldPara.HGV20_EPT_ENH_Information));

	memcpy(&ParaTenderKeyInformation, &OldPara.HGV20_TenderKeyInformation, sizeof(OldPara.HGV20_TenderKeyInformation));

	PifCloseFile(sFile); //Close Back up File

	sFile = PifOpenFile(_T("NHPOSPara.dat"), "rwo"); //Open the current data file for the current uniniram


	PifSeekFile(sFile, 0, &usActualPosition);
	PifWriteFile(sFile, &Para, sizeof(UNINIRAM));	//write the new information to the file.

	PifCloseFile(sFile);

	return TRUE;

}