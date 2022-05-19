/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display  Check Display Type & call function 
* Category    : Multiline, NCR 2170 US Hospitarity Application
* Program Name: MldMain.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   MldItemCasIn()    : display modifier key operation
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jan-14-00 : 01.00.00 : H.Endo     : Initial
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
#include <tchar.h>
#include <memory.h>

#include <ecr.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 

#include <regstrct.h>
#include <transact.h>
#include <pif.h>

#include <csttl.h>
#include <csop.h>
#include <report.h>

#include <fdt.h>

#include <mld.h>
#include <mldmenu.h>
#include <mldsup.h>
#include "mldlocal.h"

USHORT  husMldSemaphore = PIF_SEM_INVALID_HANDLE;         /* multiline display semaphore handle */

/*
*===========================================================================
** Format  : USHORT PrtTypeCheckRJ(VOID);
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : USHORT    sPrinterType      if Printer is RJ return 1 
*                                        else return 0
*
** Synopsis: This function get printer information and check printer is RJ or not.
*===========================================================================
*/
USHORT MldDispType2x20(VOID)
{
    
    if (PifSysConfig()->uchOperType == DISP_2X20) {
        return 1;
    }else{
        return 0;
    }
}

MLDTRANSDATA *MldMainSetMldData (MLDTRANSDATA *pWorkMLD)
{
    memset(pWorkMLD, 0, sizeof(MLDTRANSDATA));
    if ((TrnInformationPtr->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_STORE_RECALL
        || ((TrnInformationPtr->TranCurQual.flPrintStatus & CURQUAL_GC_SYS_MASK) == CURQUAL_POSTCHECK
            && (TrnInformationPtr->TranCurQual.fsCurStatus & CURQUAL_OPEMASK) != CURQUAL_CASHIER))
	{
        pWorkMLD->usVliSize = TrnInformationPtr->usTranConsStoVli;
        pWorkMLD->sFileHandle = TrnInformationPtr->hsTranConsStorage;
	    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {      /* Display by Prnt Priority */
            pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranConsIndex;
        } else {
            pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranNoConsIndex;
        }
    } else {
        pWorkMLD->usVliSize = TrnInformationPtr->usTranItemStoVli;
        pWorkMLD->sFileHandle = TrnInformationPtr->hsTranItemStorage;
	    if (CliParaMDCCheck(MDC_DISPLAY_ADR, ODD_MDC_BIT0)) {      /* Display by Prnt Priority */
            pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranItemIndex;
    	} else {
            pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranNoItemIndex;
        }
    }

	return pWorkMLD;
}

SHORT MldMainCallWithMldData (SHORT (*pFunc)(MLDTRANSDATA *pWorkMLD))
{
	MLDTRANSDATA    WorkMLD = {0};

	return pFunc(MldMainSetMldData(&WorkMLD));
}

MLDTRANSDATA *MldMainSetMldDataSplit (MLDTRANSDATA *pWorkMLD, TrnStorageType  sStorageType)
{
    memset(pWorkMLD, 0, sizeof(MLDTRANSDATA));
	switch (sStorageType) {
		case TRN_TYPE_SPLITA:
			// we do not worry about print priority display with split window
			pWorkMLD->usVliSize = TrnInfSplPtrSplitA->usTranConsStoVli;
			pWorkMLD->sFileHandle = TrnInfSplPtrSplitA->hsTranConsStorage;
			pWorkMLD->sIndexHandle = TrnInfSplPtrSplitA->hsTranConsIndex;
			break;

		case TRN_TYPE_SPLITB:
			// we do not worry about print priority display with split window
			pWorkMLD->usVliSize = TrnInfSplPtrSplitB->usTranConsStoVli;
			pWorkMLD->sFileHandle = TrnInfSplPtrSplitB->hsTranConsStorage;
			pWorkMLD->sIndexHandle = TrnInfSplPtrSplitB->hsTranConsIndex;
			break;

		case TRN_TYPE_CONSSTORAGE:
			// we do not worry about print priority display with split window
			pWorkMLD->usVliSize = TrnInformationPtr->usTranConsStoVli;
			pWorkMLD->sFileHandle = TrnInformationPtr->hsTranConsStorage;
			pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranConsIndex;
			break;

		case TRN_TYPE_ITEMSTORAGE:
			// we do not worry about print priority display with split window
			pWorkMLD->usVliSize = TrnInformationPtr->usTranItemStoVli;
			pWorkMLD->sFileHandle = TrnInformationPtr->hsTranItemStorage;
			pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranItemIndex;
			break;

		case TRN_TYPE_NOITEMSTORAGE:
			pWorkMLD->usVliSize = TrnInformationPtr->usTranItemStoVli;
			pWorkMLD->sFileHandle = TrnInformationPtr->hsTranItemStorage;
			pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranNoItemIndex;
			break;

		case TRN_TYPE_NOCONSSTORAGE:
			pWorkMLD->usVliSize = TrnInformationPtr->usTranConsStoVli;
			pWorkMLD->sFileHandle = TrnInformationPtr->hsTranConsStorage;
			pWorkMLD->sIndexHandle = TrnInformationPtr->hsTranNoConsIndex;
			break;

		default:
			MldMainSetMldData (pWorkMLD);
			break;
	}

	return pWorkMLD;
}

SHORT MldMainCallWithMldDataSplit (SHORT (*pFunc)(MLDTRANSDATA *pWorkMLD), TrnStorageType  sStorageType)
{
	MLDTRANSDATA    WorkMLD = {0};

	return pFunc(MldMainSetMldDataSplit(&WorkMLD, sStorageType));
}



/********
*
* MLD.H
*
*/
/* MLD Initialization */
SHORT   MldInit(VOID)
{
        return MLDLCDMldInit();  /* MldInit for lcd */
}
/* MLD Reinitialization */
SHORT   MldRefresh(VOID)
{
        return MLDLCDMldRefresh();  /* MldRefresh for lcd */
}
SHORT   MldSetMode(USHORT usSystem)            /* set type of display */
{
        return MLDLCDMldSetMode(usSystem);  /* MldSetMode  for lcd */
}
SHORT   MldSetDescriptor(USHORT usMode)        /* set descriptor mnemonic */
{
        return MLDLCDMldSetDescriptor(usMode);  /* MldSetDescriptor  for lcd */
}
SHORT   MldDisplayInit(USHORT usDisplay, USHORT usAttrib)  /* Initialize each display */
{
        return MLDLCDMldDisplayInit(usDisplay,usAttrib);  /* MldDisplayInit  for lcd */
}
SHORT   MldGetMldStatus(VOID)                  /* return whether mld is provied or not */
{
        return MLDLCDMldGetMldStatus();  /* MldGetMldStatus  for lcd */
}
SHORT   MldDispCursor(VOID)                    /* display cursor on scroll display */
{
        return MLDLCDMldDispCursor();  /* MldDispCursor  for lcd */
}

/* -- Storage File Interface -- */
SHORT   MldCreateFile(USHORT usSize, UCHAR uchType)        /* create mld storage file */
{
        return MLDLCDMldCreateFile(usSize,uchType);  /* MldCreateFile  for lcd */
}
SHORT   MldChkAndCreFile(USHORT usSize, UCHAR uchType)     /* check and create mld storage file */
{
        return MLDLCDMldChkAndCreFile(usSize,uchType);  /* MldChkAndCreFile  for lcd */
}
VOID    MldChkAndDelFile(USHORT usSize, UCHAR uchType)     /* check and delete mld storage file */
{
        MLDLCDMldChkAndDelFile(usSize,uchType);  /* MldChkAndDelFile  for lcd */
        return ;
}
SHORT   MldCheckSize(VOID *pData, USHORT usSize2)      /* check storage buffer full */
{
        return MLDLCDMldCheckSize(pData, usSize2);  /* MldCheckSize  for lcd */
}

/* -- Register Mode Interface -- */
SHORT   MldScrollWrite(VOID *pData, USHORT usType) /* display on scroll#1 */
{
        return MLDLCDMldScrollWrite(pData, usType);  /* MldScrollWrite  for lcd */
}
SHORT   MldScrollWrite2(USHORT usScroll, VOID *pData, USHORT usType) /* display on scroll#1/2/3 */
{
        return MLDLCDMldScrollWrite2(usScroll, pData, usType);  /* MldScrollWrite2  for lcd */
}
SHORT   MldScrollFileWrite(VOID *pData)        /* put scroll data to storage file */
{
        return MLDLCDMldScrollFileWrite(pData);  /* MldScrollFileWrite  for lcd */
}
SHORT   MldScrollFileWriteSpl(VOID *pData)        /* put scroll data to storage file */
{
        return MLDLCDMldScrollFileWriteSpl(pData);  /* MldScrollFileWrite  for lcd */
}
SHORT   MldScrollFileDelete(VOID *pData)        /* delete an item from the scroll data in storage file */
{
        return MLDLCDMldScrollFileDelete(pData);  /* MldScrollFileDelete  for lcd */
}
SHORT   MldECScrollWrite(VOID)                 /* clear displayed data */
{
        return MLDLCDMldECScrollWrite();  /* MldECScrollWrite  for lcd */
}
SHORT   MldECScrollWrite2(USHORT usScroll)     /* clear displayed data */
{
        return MLDLCDMldECScrollWrite2(usScroll);  /* MldECScrollWrite2  for lcd */
}
SHORT   MldPutTransToScroll(MLDTRANSDATA *pData1)
{
        return MLDLCDMldPutTransToScroll(pData1);  /* MldPutTransToScroll  for lcd */
}
SHORT   MldRedisplayToScroll(USHORT usScroll)
{
        return MLDLCDMldRedisplayToScroll(usScroll);  /* MldPutTransToScroll2  for lcd */
}

#if POSSIBLE_DEAD_CODE
SHORT   MldPutTransToScroll2(MLDTRANSDATA *pData1)
{
        return MLDLCDMldPutTransToScroll2(pData1);  /* MldPutTransToScroll2  for lcd */
}
#endif
SHORT   MldPutTrnToScrollReverse(MLDTRANSDATA *pData1)
{
        return MLDLCDMldPutTrnToScrollReverse(pData1);  /* MldPutTrnToScrollReverse  for lcd */
}
#if POSSIBLE_DEAD_CODE
SHORT   MldPutTrnToScrollReverse2(MLDTRANSDATA *pData1)
{
        return MLDLCDMldPutTrnToScrollReverse2(pData1);  /* MldPutTrnToScrollReverse2  for lcd */
}
#endif
SHORT   MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle)
{
        return MLDLCDMldPutGcfToScroll(usScroll, sFileHandle);  /* MldPutGcfToScroll  for lcd */
}
#if POSSIBLE_DEAD_CODE
SHORT   MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle)
{
        return MLDLCDMldPutGcfToScroll2(usScroll, sFileHandle);  /* MldPutGcfToScroll2  for lcd */
}
#endif
SHORT   MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1)
{
        return MLDLCDMldPutSplitTransToScroll(usScroll, pData1);  /* MldPutSplitTransToScroll  for lcd */
}
#if POSSIBLE_DEAD_CODE
SHORT   MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1)
{
        return MLDLCDMldPutSplitTransToScroll2(usScroll, pData1);  /* MldPutSplitTransToScroll2  for lcd */
}
#endif

SHORT   MldPutAllGcfToScroll(SHORT sFileHandle1,
                                SHORT sFileHandle2,
                                SHORT sFileHandle3)
{
        return MLDLCDMldPutAllGcfToScroll(sFileHandle1,sFileHandle2,sFileHandle3);  /* MldPutAllGcfToScroll  for lcd */
}
#if POSSIBLE_DEAD_CODE
SHORT   MldPutAllGcfToScroll2(SHORT sFileHandle1,
                                SHORT sFileHandle2,
                                SHORT sFileHandle3)
{
        return MLDLCDMldPutAllGcfToScroll2(sFileHandle1,sFileHandle2,sFileHandle3);  /* MldPutAllGcfToScroll2  for lcd */
}
#endif
SHORT   MldGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2, USHORT usType)
{
        return MLDLCDMldGetCursorDisplay(usScroll, pData1, pData2, usType);  /* MldGetCursorDisplay  for lcd */
}

SHORT   MldUpCursor(VOID)                      /* move cursor to upper line */
{
        return MLDLCDMldUpCursor();  /* MldUpCursor  for lcd */
}
SHORT   MldDownCursor(VOID)                    /* move cursor to lower line */
{
        return MLDLCDMldDownCursor();  /* MldDownCursor  for lcd */
}
SHORT   MldMoveCursor(USHORT usType)           /* move cursor to next display */
{
        return MLDLCDMldMoveCursor(usType);  /* MldMoveCursor  for lcd */
}
SHORT   MldSetCursor(USHORT usScroll)          /* move cursor to directed display */
{
        return MLDLCDMldSetCursor(usScroll);  /* MldSetCursor  for lcd */
}
SHORT   MldQueryCurScroll(VOID)                /* returns current active display */
{
        return MLDLCDMldQueryCurScroll();  /* MldQueryCurScroll  for lcd */
}
SHORT   MldQueryMoveCursor(USHORT usSCroll)    /* check if cursor is moved in scroll */
{
        return MLDLCDMldQueryMoveCursor(usSCroll);  /* MldQueryMoveCursor  for lcd */
}
SHORT   MldDispSubTotal(USHORT usScroll, DCURRENCY lTotal) /* display total amount */
{
        return MLDLCDMldDispSubTotal(usScroll, lTotal);  /* MldDispSubTotal  for lcd */
}
SHORT   MldDispSeatNum(USHORT usScroll, MLDSEAT *pSeatNum) /* display seat# */
{
        return MLDLCDMldDispSeatNum(usScroll, pSeatNum);  /* MldDispSeatNum  for lcd */
}
SHORT   MldCreatePopUp(VOID *pData)            /* show order prompt display */
{
        return MLDLCDMldCreatePopUp(pData);  /* MldCreatePopUp  for lcd */
}
SHORT   MldUpdatePopUp(VOID *pData)            /* change order prompt display */
{
        return MLDLCDMldUpdatePopUp(pData);  /* MldUpdatePopUp  for lcd */
}
SHORT   MldDeletePopUp(VOID)                   /* delete order prompt display */
{
        return MLDLCDMldDeletePopUp();  /* MldDeletePopUp  for lcd */
}

SHORT   MldForceDeletePopUp(VOID)                   /* delete order prompt display */
{
        return MLDLCDMldForceDeletePopUp();  /* MldDeletePopUp  for lcd */
}

SHORT   MldPopUpUpCursor(VOID)                 /* up cursor of order prompt display */
{
        return MLDLCDMldPopUpUpCursor();  /* MldPopUpUpCursor  for lcd */
}
SHORT   MldPopUpDownCursor(VOID)               /* down cursor of order prompt display */
{
        return MLDLCDMldPopUpDownCursor();  /* MldPopUpDownCursor  for lcd */
}
SHORT   MldPopUpRedisplay(USHORT usType)       /* redisplay of order prompt display */
{
        return MLDLCDMldPopUpRedisplay(usType);  /* MldPopUpRedisplay  for lcd */
}
SHORT   MldPopUpGetCursorDisplay(VOID *pData)  /* get data class of current display */
{
        return MLDLCDMldPopUpGetCursorDisplay(pData);  /* MldPopUpGetCursorDisplay  for lcd */
}
SHORT   MldDispWaitKey(FDTWAITORDER *pData)    /* show order prompt display */
{
        return MLDLCDMldDispWaitKey(pData);  /* MldDispWaitKey  for lcd */
}

SHORT   MldDispGuestNum(MLDGUESTNUM *pGuestNum) /* display guest check no. V3.3 */
{
        return MLDLCDMldDispGuestNum(pGuestNum);  /* MldDispGuestNum  for lcd */
}
SHORT   MldDispTableNum(MLDGUESTNUM *pGuestNum) /* display table number of a guest check */
{
        return MLDLCDMldDispTableNum(pGuestNum);  /* MldDispTableNum  for lcd */
}

/* -- Supervisor Mode Interface -- */
SHORT MldDispItem(VOID  *pItem, USHORT usStatus)
{
        return MLDLCDMldDispItem(pItem, usStatus);  /* MldDispItem  for lcd */
}
SHORT MldIRptItemOther(USHORT usScroll)
{
        return MLDLCDMldIRptItemOther(usScroll);  /* MldIRptItemOther  for lcd */
}

SHORT   MldStringAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                TCHAR *pszString, USHORT usLength)  /* free format display */
{
        return MLDLCDMldStringAttr(usScroll, sRow, uchAttr,pszString, usLength);  /* MldStringAttr  for lcd */
}
SHORT   MldString(USHORT usDisplay, SHORT sRow,
                TCHAR *pszString, USHORT usLength)  /* free format display */
{
        return MLDLCDMldString(usDisplay, sRow,pszString, usLength);  /* MldString  for lcd */
}
SHORT   MldScrollUp(USHORT usScroll, USHORT usNumRow) /* make lower line to blank */
{
        return MLDLCDMldScrollUp(usScroll, usNumRow);  /* MldScrollUp  for lcd */
}
SHORT   MldScrollDown(USHORT usScroll, USHORT usNumRow)/* make upper line to blank */
{
        return MLDLCDMldScrollDown(usScroll, usNumRow);  /* MldScrollDown  for lcd */
}
SHORT   MldScrollClear(USHORT usScroll)        /* make all line to blank */
{
        return MLDLCDMldScrollClear(usScroll);  /* MldScrollClear  for lcd */
}
/*
*
* mldmenu.h
*
*/

UCHAR  UifACPGMLDDispCom(CONST MLDMENU * *ptr) 
{
	return MLDLCDUifACPGMLDDispCom(ptr);  /* UifACPGMLDDispCom  for lcd */
}
VOID UifACPGMLDClear(VOID) 
{
        MLDLCDUifACPGMLDClear();  /* UifACPGMLDClear  for lcd */
}

/************ MLDSUP */
/*
*
****
*
*
*/


/* SHORT MldDispItem(VOID  *pItem, USHORT usStatus); */
VOID  MldRptSupHeader( MAINTHEADER *pData )
{
        MLDLCDMldRptSupHeader(pData);  /* MldRptSupHeader  for lcd */
}
VOID  MldRptSupTrailer( MAINTTRAILER *pData )
{
        MLDLCDMldRptSupTrailer(pData);  /* MldRptSupTrailer  for lcd */
}
VOID  MldRptSupFormTrailer(MAINTTRAILER *pData)
{
        MLDLCDMldRptSupFormTrailer(pData);  /* MldRptSupFormTrailer  for lcd */
}
SHORT MldRptSupGCF( RPTGUEST *pData )
{
        return MLDLCDMldRptSupGCF(pData);  /* MldRptSupGCF  for lcd */
}
SHORT MldRptSupCashWaitFile( VOID *pData )
{
        return MLDLCDMldRptSupCashWaitFile(pData);  /* MldRptSupCashWaitFile  for lcd */
}
SHORT MldRptSupRegFin( RPTREGFIN *pItem)
{
        return MLDLCDMldRptSupRegFin(pItem);  /* MldRptSupRegFin  for lcd */
}
SHORT MldRptSupHourlyAct( RPTHOURLY *pItem)
{
        return MLDLCDMldRptSupHourlyAct(pItem);  /* MldRptSupHourlyAct  for lcd */
}
SHORT MldRptSupDEPTFile( RPTDEPT *pItem, USHORT usStatus)
{
        return MLDLCDMldRptSupDEPTFile(pItem, usStatus);  /* MldRptSupDEPTFile  for lcd */
}
SHORT  MldRptSupPLUFile( RPTPLU *pData, USHORT usStatus)
{
        return MLDLCDMldRptSupPLUFile(pData, usStatus);  /* MldRptSupPLUFile  for lcd */
}
SHORT  MldRptSupCPNFile( RPTCPN *pData, USHORT usStatus)
{
        return MLDLCDMldRptSupCPNFile(pData, usStatus);  /* MldRptSupCPNFile  for lcd */
}
SHORT  MldRptSupETKFile( RPTEMPLOYEE *pData )
{
        return MLDLCDMldRptSupETKFile(pData);  /* MldRptSupETKFile  for lcd */
}
SHORT  MldRptSupEJ( RPTEJ *pItem)
{
        return MLDLCDMldRptSupEJ(pItem);  /* MldRptSupEJ  for lcd */
}
SHORT MldRptSupServiceTime( RPTSERVICE *pItem)
{
        return MLDLCDMldRptSupServiceTime(pItem);  /* MldRptSupServiceTime  for lcd */
}
SHORT MldRptSupProgRpt( RPTPROGRAM *pItem)
{
        return MLDLCDMldRptSupProgRpt(pItem);  /* MldRptSupProgRpt  for lcd */
}
VOID MldRptSupMakeString(RPTEMPLOYEE *pData )
{
        MLDLCDMldRptSupMakeString(pData);  /* MldRptSupMakeString  for lcd */
}
VOID  MldRptSupErrorCode( MAINTERRORCODE *pData )
{
        MLDLCDMldRptSupErrorCode(pData);  /* MldRptSupErrorCode  for lcd */
}
VOID  MldPause(VOID)
{
        MLDLCDMldPause();  /* MldPause  for lcd */
}

SHORT MldContinueCheck(VOID) 
{
        return MLDLCDMldContinueCheck();  /* MldContinueCheck  for lcd */
}

VOID  MldRptSupTrans( MAINTTRANS *pData, USHORT usStatus )
{
        MLDLCDMldRptSupTrans(pData, usStatus);  /* MldRptSupTrans  for lcd */
}
VOID MldParaCASDisp(MAINTCASHIERIF *pDept)
{
        MLDLCDMldParaCASDisp(pDept);  /* MldParaCASDisp  for lcd */
}
/* VOID MldParaCashNoDisp(PARACASHIERNO *pCash, USHORT usClear); */
VOID MldParaPLUDisp(PARAPLU *pPlu, USHORT usStatus)
{
        MLDLCDMldParaPLUDisp(pPlu, usStatus);  /* MldParaPLUDisp  for lcd */
}
VOID MldParaDEPTDisp(PARADEPT *pDept)
{
        MLDLCDMldParaDEPTDisp(pDept);  /* MldParaDEPTDisp  for lcd */
}
VOID MldParaMenuPluDisp(PARAMENUPLUTBL *pMep)
{
        MLDLCDMldParaMenuPluDisp(pMep);  /* MldParaMenuPluDisp  for lcd */
}
VOID MldParaTaxDisp( MAINTTAXTBL *pTax )
{
        MLDLCDMldParaTaxDisp(pTax);  /* MldParaTaxDisp  for lcd */
}
VOID MldParaEmpNoDisp(PARAEMPLOYEENO *pEmp, USHORT usClear)
{
        MLDLCDMldParaEmpNoDisp(pEmp, usClear);  /* MldParaEmpNoDisp  for lcd */
}
VOID MldParaOEPDisp(PARAOEPTBL *pOep)
{
        MLDLCDMldParaOEPDisp(pOep);  /* MldParaOEPDisp  for lcd */
}
VOID MldParaCPNDisp(PARACPN *pCpn, USHORT usStatus)
{
        MLDLCDMldParaCPNDisp(pCpn, usStatus);  /* MldParaCPNDisp  for lcd */
}
VOID MldParaPPI(PARAPPI *pPPI, USHORT usClear)
{
        MLDLCDMldParaPPI(pPPI, usClear);  /* MldParaPPI  for lcd */
}

SHORT	MldSetMoveCursor(USHORT usScroll)		/* sets the Cursor status to moved*/
{
        return MLDLCDMldSetMoveCursor(usScroll);
}

USHORT	MldGetVosWinHandle(USHORT usScroll)		/* get the vos window handle*/
{
        return MLDLCDMldGetVosWinHandle(usScroll);
}


/* end of file */