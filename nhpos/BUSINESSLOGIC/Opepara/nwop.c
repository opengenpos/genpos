/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO   ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER module, Program List
*   Category           : Client/Server OPERATIONAL PARAMATER, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : NWOP.C
*  ------------------------------------------------------------------------
*   Abstract           : This module provides the following parameter files.
*                           Deparartment, PLU, Coupon, Control String, PPI
*   The provided function names are as follows.
*
*   OpInit                      * Initialize function *
*   OpOepCreatFile              * Creat Oep file *  ADD 2172 Rel 1.0
*   OpPluCreatFile           HC * Creat Plu file *
*   OpDeptCreatFile             * Creat Dept file *
*   OpPluCheckAndCreatFile      * Check and Creat Plu file *
*   OpDeptCheckAndCreatFile     * Check and Creat Dept file *
*   OpPluCheckAndDeleteFile     * Check and Delete Plu file *
*   OpDeptCheckAndDeleteFile    * Check and Delete Dept file
*   OpPluAssign                 * Assign/addition plu *
*   OpDeptAssign                * Assign/addition dept *
*   OpPluDelete                 * Delete plu *
*   OpPluFileUpDate          HC * Update Plu file *
*   OpPluIndRead                * Read Plu individual *
*   OpDeptIndRead               * Read Dept individual *
*   OpMajorDeptRead             * Read plu by Major Dept *
*   OpPluAllRead                * Read plu all *
*   OpRprtPluRead               * Read Plu by report No. *
*   OpDeptPluread               * Read Plu by Dept No. *
*   OpLock                      * Get Lock Handle *
*   OpUnLock                    * Release Lock Handle *
*   OpReqBackup                 * Request Back up *
*   OpReqBackupPara             * Request Back up Parameter *
*   OpResBackup                 * Response Back up *
*   OpReqAll                    * Requset All *
*   OpReqAllPara                * Requset All paramater *
*   OpResAll                    * Response All paramater *
*   OpReqSup                    * Requset super *
*   OpReqSupPara                * Requset paramater of super *
*   OpResSup                    * Response paramater for super *
*   OpConvertError              * Error convert *
*   OpOepPluRead                * Read a Plu by Order Entry Prompt Group Number
*   OpCpnCreatFile              * Creat Combination Coupon file *
*   OpCpnCheckAndCreatFile      * Check and Creat Combination Coupon file *
*   OpCpnCheckAndDeleteFile     * Check and Delete Combination Coupon file *
*   OpCpnAssign                 * Assign/addition Combination Coupon *
*   OpCpnIndRead                * Read Combination Coupon individual *
*   OpCstrCreatFile             * Creat Control String file *
*   OpCstrCheckAndCreatFile     * Check and Creat Control String file *
*   OpCstrCheckAndDeleteFile    * Check and Delete Control String file *
*   OpCstrAssign                * Assign/addition Control String *
*   OpCstrDelete                * Delete Control String *
*   OpCstrIndRead               * Read Control String individual *
*   OpPpiCreatFile              * Creat PPI file,               R3.1
*   OpPpiCheckAndCreatFile      * Check and Creat PPI file,     R3.1
*   OpPpiCheckAndDeleteFile     * Check and Delete PPI file,    R3.1
*   OpPpiAssign                 * Assign/addition PPI record,   R3.1
*   OpPpiDelete                 * Delete PPI record,            R3.1
*   OpPpiIndRead                * Individual Read PPI record,   R3.1
*
*  ------------------------------------------------------------------------
*   Update Histories  :
*   Date      :Ver.Rev :NAME       :Description
*   Nov-03-93 :00.00.01:H.Yamaguchi:Increase PLU 1000 --> 3000
*   Mar-22-94 :00.00.04:K.You      :Add auchNEW_FILE_READ_WRITE[]
*   Mar-31-94 :00.00.04:K.You      :Add flex GC feature(mod. auchFlexMem table)
*   Feb-27-95 :03.00.00:H.Ishida   :Add process
*             :        :           :  OpOepPluRead, OpCpnCreatFile, OpCpnCheckAndCreatFile,
*             :        :           :  OpCpnCheckAndDeleteFile, OpCpnAssign, OpCpnIndRead,
*             :        :           :  OpReqCpn, OpResCpn
*   Mar-31-95 :03.00.00:H.Ishida   :Add process
*             :        :           :  OpCstrCreatFile, OpCstrCheckAndCreatFile,
*             :        :           :  OpCstrCheckAndDeleteFile, OpCstrAssign,
*             :        :           :  OpCstrDelete, OpCstrIndRead
*   Jul-24-95 :03.00.00:hkato      :R3.0
*   Mar-18-96 :03.01.00:T.Nakahata :R3.1 Initial
*       Add a PPI(promotional pricing item) file (AC71)
*       Increase contents of parameter backup table (AC75)
*   Aug-04-98 :03.03.00:hrkato     : V3.3
*   Aug-17-98 :03.03.00:M.Ozawa    : Add MLD Mnemonics File
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
*
** GenPOS
* Jun-20-18 : 02.02.02 : R.Chambers : fix defect in OpRprtPluRead() with new function OpAssignRecPluToPifRepPlu().
* Jun-20-18 : 02.02.02 : R.Chambers : fix defect in OpDeptPluRead() with new function OpAssignRecPluToPifDepPlu().
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include	<stdio.h>
#include	<math.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <paraequ.h>
#include    <para.h>
#include    <plu.h>
#include    <csop.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csstbpar.h>
#include    <appllog.h>
#include    <applini.h>
#include    "csopin.h"
#include    "rfl.h"
#include	<BlFWif.h>

PifSemHandle  husOpSem = PIF_SEM_INVALID_HANDLE;
USHORT  husOpLockHandle;
SHORT   hsOpPluFileHandle;
SHORT   hsOpDeptFileHandle;
/* Add 2172 Rel 1.0 */
SHORT   hsOpOepFileHandle;
SHORT   hsOpCpnFileHandle;
SHORT   hsOpCstrFileHandle;
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) BEGIN === */
SHORT   hsOpPpiFileHandle;
/* === Add PPI (Promotional Pricing Item) File (Release 3.1) END === */
SHORT   hsOpMldFileHandle;  /* V3.3 */


UCHAR CONST auchNEW_FILE_WRITE[] = "nw";
UCHAR CONST auchNEW_FILE_READ_WRITE[]  = "nwr";
UCHAR CONST auchOLD_FILE_WRITE[] = "ow";
UCHAR CONST auchOLD_FILE_READ_WRITE[]  = "owr";
UCHAR CONST auchOLD_FILE_READ[] = "or";

/* temporary file on DRAM, saratoga */
UCHAR CONST auchTEMP_NEW_FILE_WRITE[] = "tnw";
UCHAR CONST auchTEMP_NEW_FILE_READ_WRITE[]  = "tnwr";
UCHAR CONST auchTEMP_OLD_FILE_WRITE[] = "tow";
UCHAR CONST auchTEMP_OLD_FILE_READ_WRITE[]  = "towr";
UCHAR CONST auchTEMP_OLD_FILE_READ[] = "tor";

/* direct write to flash disk, saratoga */
UCHAR CONST auchNEW_FILE_WRITE_FLASH[] = "nwf";
UCHAR CONST auchNEW_FILE_READ_WRITE_FLASH[]  = "nwrf";
UCHAR CONST auchOLD_FILE_WRITE_FLASH[] = "owf";
UCHAR CONST auchOLD_FILE_READ_WRITE_FLASH[]  = "owrf";
UCHAR CONST auchOLD_FILE_READ_WRITE_FLASH_EX[]  = "owrf-";  // deny sharing

/* direct write to flash disk, saratoga */
UCHAR CONST auchNEW_FILE_WRITE_SAVTTL[] = "nws";
UCHAR CONST auchNEW_FILE_READ_WRITE_SAVTTL[]  = "nwrs";
UCHAR CONST auchOLD_FILE_WRITE_SAVTTL[] = "ows";
UCHAR CONST auchOLD_FILE_READ_WRITE_SAVTTL[]  = "owrs";


/* UCHAR FARCONST auchOP_PLU_OLD[] = "PARAM!LU";  FOR PEP Only used   */
TCHAR CONST auchOP_PLU[]  = _T("PARAMPLU");
TCHAR CONST auchOP_DEPT[] = _T("PARAMDEP");
TCHAR CONST auchOP_CPN[]  = _T("PARAMCPN");
TCHAR CONST auchOP_CSTR[] = _T("PARAMCST");
TCHAR CONST auchOP_OEP[]  = _T("PARAMOEP");   /* Add 2172 Rel 1.0 */
TCHAR CONST auchOP_PPI[] = _T("PARAMPPI");    /* Add PPI (Promotional Pricing Item) File (Release 3.1) */
TCHAR CONST auchOP_MLD[] = _T("PARAMMLD");    /* V3.3 */
TCHAR CONST auchOP_RSN[] = _T("PARAMRSN");    /* Reason Code file for Rel 2.2.1 and Amtrak/VCS */

TCHAR CONST *auchOP_OEP_TABLE[] = {
	_T("PARAMOEP"),
	_T("PARAMOEA"),
	_T("PARAMOEB"),
	_T("PARAMOEC")
};


/*
	This array appears to contain the various UNINIRAM Para data items which
	are supervisor set data which needs to be broadcast to the other terminals
	in the cluster if the supervisor has modified this data and then wants the
	other terminals in the cluster to know of the change.
 */
UCHAR FARCONST auchClassNoSup[] = {CLASS_PARAPLUNOMENU,    /* AC 4      */
                                   CLASS_PARAROUNDTBL,     /* AC 84     */
                                   CLASS_PARADISCTBL,      /* AC 86     */
                                   CLASS_PARAPROMOTION,    /* AC 88     */
                                   CLASS_PARACURRENCYTBL,  /* AC 89     */
                                   CLASS_PARATARE,         /* AC 111    */
                                   CLASS_PARADEPTNOMENU,   /* AC 115 Saratoga */
                                   CLASS_PARAMENUPLUTBL,   /* AC 116    */
                                   CLASS_PARATAXRATETBL,   /* AC 127    */
                                   CLASS_PARAMISCPARA,     /* AC 128 Saratoga */
                                   CLASS_PARATAXTBL1,      /* AC 124    */
                                   CLASS_PARARESTRICTION,  /* AC 170 Saratoga */
                                   CLASS_PARABOUNDAGE,     /* AC 208 Saratoga */
                                   CLASS_PARACTLTBLMENU,   /* AC 5      */
                                   CLASS_PARAMANUALTKITCH, /* AC 6      */
                                   //CLASS_PARACASHABASSIGN, /* AC 7      */
                                   CLASS_PARASUPLEVEL,     /* P  8      */
                                   CLASS_PARATONECTL,      /* AC 169    */
                                   CLASS_PARASOFTCHK,      /* AC 87  EH */
                                   CLASS_PARAOEPTBL,       /* AC 160    */
                                   CLASS_PARAFXDRIVE,      /* AC 162    */
                                   CLASS_PARASERVICETIME,  /* AC 133, R3.1 */
                                   CLASS_PARAPIGRULE,      /* AC 130 EH */
                                   CLASS_PARALABORCOST,    /* AC 154, R3.1 */
                                   CLASS_PARADISPPARA,     /* AC 137, R3.1 */
								   CLASS_PARASTOREFORWARD, // store and forward limits
                                   OP_EOT};

/*
	The following array contains a list of all of the different kinds of
	data that is broadcast by the Master terminal or the Backup Master terminal
	when doing a broadcast all.

	New Major classes which are added to paraequ.h which are then included in the
	UNINIRAM Para along with the functions ParaAllRead() and ParaAllWrite ()
	and are included in the provisioning utility interface of Isp in file
	ispopr.c also need to be added to this array here.  This array is used
	in several places in this file nwop.c to indicate which data items in
	the UNINIRAM Para need to be broadcast from the Master terminal to the
	Backup Master terminal.

 */
UCHAR FARCONST auchClassNoAll[] = {CLASS_PARAMDC,          /* P  1      */
                                   CLASS_PARAFSC,          /* P  3      */
                                   CLASS_PARASECURITYNO,   /* P  6      */
                                   CLASS_PARASUPLEVEL,     /* P  8      */
                                   CLASS_PARAACTCODESEC,   /* P  9      */
                                   CLASS_PARATRANSHALO,    /* P 10      */
                                   CLASS_PARAHOURLYTIME,   /* P 17      */
                                   CLASS_PARASLIPFEEDCTL,  /* P 18      */
                                   CLASS_PARATRANSMNEMO,   /* P 20      */
                                   CLASS_PARALEADTHRU,     /* P 21      */
                                   CLASS_PARAREPORTNAME,   /* P 22      */
                                   CLASS_PARASPECIALMNEMO, /* P 23      */
                                   CLASS_PARAADJMNEMO,     /* P 46      */
                                   CLASS_PARAPRTMODI,      /* P 47      */
                                   CLASS_PARAMAJORDEPT,    /* P 48      */
                                   CLASS_PARACHAR24,       /* P 57      */
                                   CLASS_PARAAUTOALTKITCH, /* P 49      */
                                   CLASS_PARAKDSIP,        /* P 51 Saratoga */
                                   CLASS_PARAFLEXMEM,      /* P  2      */
                                   CLASS_PARATTLKEYTYP,    /* P 60      */
                                   CLASS_PARATTLKEYCTL,    /* P 61      */
                                   CLASS_PARATEND,         /* P 62, V3.3*/
								   CLASS_PARAAUTOCPN,	   /* P 67		*/
                                   CLASS_PARAPCIF,         /* P 39      */
                                   CLASS_PARAPLUNOMENU,    /* AC 4      */
                                   CLASS_PARAROUNDTBL,     /* AC 84     */
                                   CLASS_PARADISCTBL,      /* AC 86     */
                                   CLASS_PARAPROMOTION,    /* AC 88     */
                                   CLASS_PARACURRENCYTBL,  /* AC 89     */
                                   CLASS_PARATARE,         /* AC 111    */
                                   CLASS_PARAMENUPLUTBL,   /* AC 116    */
                                   CLASS_PARATAXRATETBL,   /* AC 127    */
                                   CLASS_PARAMISCPARA,     /* AC 128 Saratoga */
                                   CLASS_PARATAXTBL1,      /* AC 124    */
                                   CLASS_PARACTLTBLMENU,   /* AC 5      */
                                   CLASS_PARAMANUALTKITCH, /* AC 6      */
                                   //CLASS_PARACASHABASSIGN, /* AC 7      */
                                   CLASS_PARATONECTL,      /* AC 169    */
                                   CLASS_PARASOFTCHK,      /* AC 87  EH */
                                   CLASS_PARASHRPRT,       /* P  50  EH */
                                   CLASS_PARAPRESETAMOUNT, /* P  15  EH */
                                   CLASS_PARAHOTELID,      /* P  54  HO */
                                   CLASS_PARAOEPTBL,       /* AC 160    */
                                   CLASS_PARAFXDRIVE,      /* AC 162    */
                                   CLASS_PARASERVICETIME,  /* AC 133, R3.1 */
                                   CLASS_PARAPIGRULE,      /* AC 130 EH */
                                   CLASS_PARALABORCOST,    /* AC 154, R3.1 */
                                   CLASS_PARADISPPARA,     /* AC 137, R3.1 */
                                   CLASS_PARADEPTNOMENU,   /* AC 115 Saratoga */
                                   CLASS_PARARESTRICTION,  /* AC 170 Saratoga */
                                   CLASS_PARABOUNDAGE,     /* AC 208 Saratoga */
								   CLASS_PARACOLORPALETTE, /* AC 209 Color Palette */
								   CLASS_PARATTLKEYORDERDEC,  // Order Declare types for total keys
                                   CLASS_PARATERMINALINFO,  // P97 terminal information
                                   CLASS_TENDERKEY_INFO,    // P98 new interface for Tender key information
								   CLASS_PARASTOREFORWARD, // store and forward limits
								   CLASS_PARAREASONCODE,   // Reason code mnemonics limits
                                   OP_EOT};


static VOID (PIFENTRY *pPifSaveFarData)(VOID) = PifSaveFarData; /* saratoga */

static VOID OpAssignPifPluToRecPlu (RECPLU *pRecPlu, PLUIF *pPif)
{
	// following two PLU record items may vary depending on the adjective
	// provisioning of the PLU record.
//    pRecPlu->uchAdjective             = 1;  /* always adjective code is 1 on PLU record */
//    RflConv3bto4b(pRecPlu->ulUnitPrice[0], pPif->ParaPlu.auchPrice);

	_tcsncpy(pRecPlu->aucPluNumber, pPif->auchPluNo, PLU_NUMBER_LEN);
    pRecPlu->usDeptNumber             = pPif->ParaPlu.ContPlu.usDept;
    pRecPlu->uchPriceMulWeight        = pPif->ParaPlu.uchPriceMulWeight;
    _tcsncpy(pRecPlu->aucMnemonic, pPif->ParaPlu.auchPluName, PLU_MNEMONIC_LEN);
    _tcsncpy(pRecPlu->auchAltPluName, pPif->ParaPlu.auchAltPluName, PLU_MNEMONIC_LEN);
    memcpy(pRecPlu->aucStatus, pPif->ParaPlu.ContPlu.auchContOther, sizeof(pRecPlu->aucStatus));
	memcpy(pRecPlu->auchStatus2, pPif->ParaPlu.auchStatus2, sizeof(pRecPlu->auchStatus2));
    pRecPlu->uchReportCode            = pPif->ParaPlu.ContPlu.uchRept;
    pRecPlu->usLinkNumber             = pPif->ParaPlu.usLinkNo;
    pRecPlu->uchRestrict              = pPif->ParaPlu.uchRestrict;
    pRecPlu->uchTableNumber           = pPif->ParaPlu.uchTableNumber;
    pRecPlu->uchGroupNumber           = pPif->ParaPlu.uchGroupNumber;
    pRecPlu->uchPrintPriority         = pPif->ParaPlu.uchPrintPriority;
    pRecPlu->uchItemType              = pPif->ParaPlu.ContPlu.uchItemType;
    pRecPlu->uchLinkPPI               = pPif->ParaPlu.uchLinkPPI;
    pRecPlu->uchMixMatchTare          = 0;  /* reserved, see RECMIX */ /* ### NOT PORTED FROM NCR 2172 GP software version */
    pRecPlu->usFamilyCode             = pPif->ParaPlu.usFamilyCode;
	pRecPlu->ulVoidTotalCounter       = pPif->ParaPlu.ulVoidTotalCounter;
	pRecPlu->ulDiscountTotalCounter   = pPif->ParaPlu.ulDiscountTotalCounter;
	pRecPlu->ulFreqShopperPoints      = pPif->ParaPlu.ulFreqShopperPoints;
	memcpy(pRecPlu->auchAlphaNumRouting, pPif->ParaPlu.auchAlphaNumRouting, sizeof(pRecPlu->auchAlphaNumRouting));
    pRecPlu->uchColorPaletteCode      = pPif->ParaPlu.uchColorPaletteCode;
    pRecPlu->usBonusIndex             = pPif->ParaPlu.ContPlu.usBonusIndex;
    pRecPlu->usTareInformation        = pPif->ParaPlu.usTareInformation;
	memcpy (pRecPlu->uchExtendedGroupPriority, pPif->ParaPlu.uchExtendedGroupPriority, sizeof(pRecPlu->uchExtendedGroupPriority));
	memcpy (pRecPlu->uchPluOptionFlags, pPif->ParaPlu.uchPluOptionFlags, sizeof(pRecPlu->uchPluOptionFlags));
	memcpy(pRecPlu->futureTotals, pPif->ParaPlu.futureTotals, sizeof(pRecPlu->futureTotals));
}

static VOID OpAssignRecPluToPifPlu (PLUIF *pPif, RECPLU *pRecPlu)
{
	// following two PLU record items may vary depending on the adjective
	// provisioning of the PLU record.
//    pPif->uchPluAdj = 1;  /* always adjective code is 1 on PLU record */
//    RflConv4bto3b(pPif->ParaPlu.auchPrice, pRecPlu->ulUnitPrice[0]);

	_tcsncpy(pPif->auchPluNo, pRecPlu->aucPluNumber, PLU_NUMBER_LEN);
    pPif->ParaPlu.ContPlu.usDept      = pRecPlu->usDeptNumber;
    pPif->ParaPlu.uchPriceMulWeight   = pRecPlu->uchPriceMulWeight;
    _tcsncpy(pPif->ParaPlu.auchPluName, pRecPlu->aucMnemonic, STD_PLU_MNEMONIC_LEN);
    _tcsncpy(pPif->ParaPlu.auchAltPluName, pRecPlu->auchAltPluName, STD_PLU_MNEMONIC_LEN);
    memcpy(pPif->ParaPlu.ContPlu.auchContOther, pRecPlu->aucStatus, sizeof(pPif->ParaPlu.ContPlu.auchContOther));
	memcpy(pPif->ParaPlu.auchStatus2, pRecPlu->auchStatus2, sizeof(pPif->ParaPlu.auchStatus2));
    pPif->ParaPlu.ContPlu.uchRept     = pRecPlu->uchReportCode;
    pPif->ParaPlu.usLinkNo            = pRecPlu->usLinkNumber;
    pPif->ParaPlu.uchRestrict         = pRecPlu->uchRestrict;
    pPif->ParaPlu.uchTableNumber      = pRecPlu->uchTableNumber;
    pPif->ParaPlu.uchGroupNumber      = pRecPlu->uchGroupNumber;
    pPif->ParaPlu.uchPrintPriority    = pRecPlu->uchPrintPriority;
    pPif->ParaPlu.ContPlu.uchItemType = pRecPlu->uchItemType;
    pPif->ParaPlu.uchLinkPPI          = pRecPlu->uchLinkPPI;
//  pPif->ParaPlu.  0 = pRecPlu->uchMixMatchTare;  /* reserved */
    pPif->ParaPlu.usFamilyCode           = pRecPlu->usFamilyCode;
	pPif->ParaPlu.ulVoidTotalCounter     = pRecPlu->ulVoidTotalCounter;
	pPif->ParaPlu.ulDiscountTotalCounter = pRecPlu->ulDiscountTotalCounter;
	pPif->ParaPlu.ulFreqShopperPoints    = pRecPlu->ulFreqShopperPoints;
	memcpy(pPif->ParaPlu.auchAlphaNumRouting, pRecPlu->auchAlphaNumRouting, sizeof(pPif->ParaPlu.auchAlphaNumRouting));
	pPif->ParaPlu.uchColorPaletteCode	 = pRecPlu->uchColorPaletteCode;
    pPif->ParaPlu.ContPlu.usBonusIndex   = pRecPlu->usBonusIndex;
	pPif->ParaPlu.usTareInformation      = pRecPlu->usTareInformation;
	memcpy (pPif->ParaPlu.uchExtendedGroupPriority, pRecPlu->uchExtendedGroupPriority, sizeof(pPif->ParaPlu.uchExtendedGroupPriority));
	memcpy (pPif->ParaPlu.uchPluOptionFlags, pRecPlu->uchPluOptionFlags, sizeof(pPif->ParaPlu.uchPluOptionFlags));
	memcpy(pPif->ParaPlu.futureTotals, pRecPlu->futureTotals, sizeof(pPif->ParaPlu.futureTotals));
}

static VOID OpAssignRecPluToPifRepPlu (PLUIF_REP *pPif, RECPLU *pRecPlu)
{
	// following two PLU record items may vary depending on the adjective
	// provisioning of the PLU record.
//    pPif->uchPluAdj = 1;  /* always adjective code is 1 on PLU record */
//    RflConv4bto3b(pPif->ParaPlu.auchPrice, pRecPlu->ulUnitPrice[0]);

	_tcsncpy(pPif->auchPluNo, pRecPlu->aucPluNumber, PLU_NUMBER_LEN);
    pPif->ParaPlu.ContPlu.usDept      = pRecPlu->usDeptNumber;
    pPif->ParaPlu.uchPriceMulWeight   = pRecPlu->uchPriceMulWeight;
    _tcsncpy(pPif->ParaPlu.auchPluName, pRecPlu->aucMnemonic, STD_PLU_MNEMONIC_LEN);
    _tcsncpy(pPif->ParaPlu.auchAltPluName, pRecPlu->auchAltPluName, STD_PLU_MNEMONIC_LEN);
    memcpy(pPif->ParaPlu.ContPlu.auchContOther, pRecPlu->aucStatus, sizeof(pPif->ParaPlu.ContPlu.auchContOther));
	memcpy(pPif->ParaPlu.auchStatus2, pRecPlu->auchStatus2, sizeof(pPif->ParaPlu.auchStatus2));
    pPif->ParaPlu.ContPlu.uchRept     = pRecPlu->uchReportCode;
    pPif->ParaPlu.usLinkNo            = pRecPlu->usLinkNumber;
    pPif->ParaPlu.uchRestrict         = pRecPlu->uchRestrict;
    pPif->ParaPlu.uchTableNumber      = pRecPlu->uchTableNumber;
    pPif->ParaPlu.uchGroupNumber      = pRecPlu->uchGroupNumber;
    pPif->ParaPlu.uchPrintPriority    = pRecPlu->uchPrintPriority;
    pPif->ParaPlu.ContPlu.uchItemType = pRecPlu->uchItemType;
    pPif->ParaPlu.uchLinkPPI          = pRecPlu->uchLinkPPI;
//  pPif->ParaPlu.  0 = pRecPlu->uchMixMatchTare;  /* reserved */
    pPif->ParaPlu.usFamilyCode           = pRecPlu->usFamilyCode;
	pPif->ParaPlu.ulVoidTotalCounter     = pRecPlu->ulVoidTotalCounter;
	pPif->ParaPlu.ulDiscountTotalCounter = pRecPlu->ulDiscountTotalCounter;
	pPif->ParaPlu.ulFreqShopperPoints    = pRecPlu->ulFreqShopperPoints;
	memcpy(pPif->ParaPlu.auchAlphaNumRouting, pRecPlu->auchAlphaNumRouting, sizeof(pPif->ParaPlu.auchAlphaNumRouting));
	pPif->ParaPlu.uchColorPaletteCode	 = pRecPlu->uchColorPaletteCode;
    pPif->ParaPlu.ContPlu.usBonusIndex   = pRecPlu->usBonusIndex;
	pPif->ParaPlu.usTareInformation      = pRecPlu->usTareInformation;
	memcpy (pPif->ParaPlu.uchExtendedGroupPriority, pRecPlu->uchExtendedGroupPriority, sizeof(pPif->ParaPlu.uchExtendedGroupPriority));
	memcpy (pPif->ParaPlu.uchPluOptionFlags, pRecPlu->uchPluOptionFlags, sizeof(pPif->ParaPlu.uchPluOptionFlags));
	memcpy(pPif->ParaPlu.futureTotals, pRecPlu->futureTotals, sizeof(pPif->ParaPlu.futureTotals));
}

static VOID OpAssignRecPluToPifDepPlu (PLUIF_DEP *pPif, RECPLU *pRecPlu)
{
	// following two PLU record items may vary depending on the adjective
	// provisioning of the PLU record.
//    pPif->uchPluAdj = 1;  /* always adjective code is 1 on PLU record */
//    RflConv4bto3b(pPif->ParaPlu.auchPrice, pRecPlu->ulUnitPrice[0]);

	_tcsncpy(pPif->auchPluNo, pRecPlu->aucPluNumber, PLU_NUMBER_LEN);
    pPif->ParaPlu.ContPlu.usDept      = pRecPlu->usDeptNumber;
    pPif->ParaPlu.uchPriceMulWeight   = pRecPlu->uchPriceMulWeight;
    _tcsncpy(pPif->ParaPlu.auchPluName, pRecPlu->aucMnemonic, STD_PLU_MNEMONIC_LEN);
    _tcsncpy(pPif->ParaPlu.auchAltPluName, pRecPlu->auchAltPluName, STD_PLU_MNEMONIC_LEN);
    memcpy(pPif->ParaPlu.ContPlu.auchContOther, pRecPlu->aucStatus, sizeof(pPif->ParaPlu.ContPlu.auchContOther));
	memcpy(pPif->ParaPlu.auchStatus2, pRecPlu->auchStatus2, sizeof(pPif->ParaPlu.auchStatus2));
    pPif->ParaPlu.ContPlu.uchRept     = pRecPlu->uchReportCode;
    pPif->ParaPlu.usLinkNo            = pRecPlu->usLinkNumber;
    pPif->ParaPlu.uchRestrict         = pRecPlu->uchRestrict;
    pPif->ParaPlu.uchTableNumber      = pRecPlu->uchTableNumber;
    pPif->ParaPlu.uchGroupNumber      = pRecPlu->uchGroupNumber;
    pPif->ParaPlu.uchPrintPriority    = pRecPlu->uchPrintPriority;
    pPif->ParaPlu.ContPlu.uchItemType = pRecPlu->uchItemType;
    pPif->ParaPlu.uchLinkPPI          = pRecPlu->uchLinkPPI;
//  pPif->ParaPlu.  0 = pRecPlu->uchMixMatchTare;  /* reserved */
    pPif->ParaPlu.usFamilyCode           = pRecPlu->usFamilyCode;
	pPif->ParaPlu.ulVoidTotalCounter     = pRecPlu->ulVoidTotalCounter;
	pPif->ParaPlu.ulDiscountTotalCounter = pRecPlu->ulDiscountTotalCounter;
	pPif->ParaPlu.ulFreqShopperPoints    = pRecPlu->ulFreqShopperPoints;
	memcpy(pPif->ParaPlu.auchAlphaNumRouting, pRecPlu->auchAlphaNumRouting, sizeof(pPif->ParaPlu.auchAlphaNumRouting));
	pPif->ParaPlu.uchColorPaletteCode	 = pRecPlu->uchColorPaletteCode;
    pPif->ParaPlu.ContPlu.usBonusIndex   = pRecPlu->usBonusIndex;
	pPif->ParaPlu.usTareInformation      = pRecPlu->usTareInformation;
	memcpy (pPif->ParaPlu.uchExtendedGroupPriority, pRecPlu->uchExtendedGroupPriority, sizeof(pPif->ParaPlu.uchExtendedGroupPriority));
	memcpy (pPif->ParaPlu.uchPluOptionFlags, pRecPlu->uchPluOptionFlags, sizeof(pPif->ParaPlu.uchPluOptionFlags));
	memcpy(pPif->ParaPlu.futureTotals, pRecPlu->futureTotals, sizeof(pPif->ParaPlu.futureTotals));
}

/*
*==========================================================================
**    Synopsis:   VOID    OpInit(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :    Nothing
*
**  Description: Initialize for Operational Paramater Module.
*==========================================================================
*/
VOID    OpInit(VOID)
{
    /* Create Semaphore */
    husOpSem = PifCreateSem( 1 );
    PluInitialize();
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

struct {
	TCHAR  *auchFileName;
	USHORT  usMaxRecordCount;
	USHORT  usRecordSize;
} OpMnemonicsFileTable[] = {
	{0, 0},
	{_T("PARAMMLD"), MAX_MLDMNEMO_SIZE, STD_MLD_MNEMONICS_LEN},
	{_T("PARAMRSN"), MAX_RSNMNEMO_SIZE, STD_RSN_MNEMONICS_LEN},
	{0, 0}
};

SHORT   Op_MnemonicsFileOpenFile(USHORT usMnemonicFileId)
{
	TCHAR  *auchFileName;

	if (0 == usMnemonicFileId || usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return -1;
	}

	auchFileName = OpMnemonicsFileTable[usMnemonicFileId].auchFileName;

    hsOpMldFileHandle = PifOpenFile(auchFileName, auchOLD_FILE_READ_WRITE);

	return hsOpMldFileHandle;
}

SHORT   OpMnemonicsFileCreate (USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED   MldFileHed;
    ULONG           ulActualPosition;
    ULONG           ulNewSize;
    ULONG           ulWriteOffset;
    TCHAR           *auchMldFile[128];
	TCHAR           *auchFileName;
	USHORT          usRecordSize;

	if (0 == usMnemonicFileId || usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return OP_NO_MAKE_FILE;
	}

	auchFileName = OpMnemonicsFileTable[usMnemonicFileId].auchFileName;
	usRecordSize = OpMnemonicsFileTable[usMnemonicFileId].usRecordSize;

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Delete MLD File */
    PifDeleteFile(auchFileName);

    /* Check Number of MLD */
    if (0 == usNumberOfAddress) {
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    if ((hsOpMldFileHandle = PifOpenFile(auchFileName, auchNEW_FILE_WRITE)) < 0) {
        PifAbort(MODULE_OP_MLD, FAULT_ERROR_FILE_OPEN);
    }

    /* Check file size and creat file */
    ulNewSize = ( ULONG )( sizeof(OPMLD_FILEHED) + ( usRecordSize * usNumberOfAddress ));
    if (PifSeekFile(hsOpMldFileHandle, ulNewSize, &ulActualPosition) < 0) {   /* PIF_ERROR_DISK_FULL, OpMldCreatFile() */
        PifCloseFile(hsOpMldFileHandle);
        PifDeleteFile(auchFileName);              /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    ulWriteOffset = OP_FILE_HEAD_POSITION;
	memset (&MldFileHed, 0, sizeof(MldFileHed));
	MldFileHed.usNumberOfAddress = OpMnemonicsFileTable[usMnemonicFileId].usMaxRecordCount;
    Op_WriteMldFile( ulWriteOffset, &MldFileHed, sizeof(OPMLD_FILEHED));
    ulWriteOffset += ( ULONG )sizeof(OPMLD_FILEHED);

    /* Clear Contents of File */
    memset( auchMldFile, 0x00, sizeof( auchMldFile ));

    while( ulWriteOffset < ulNewSize ) {
        Op_WriteMldFile( ulWriteOffset, auchMldFile, usRecordSize);  // OpMldCreatFile() - create the MLD mnemonics file.
        ulWriteOffset += ( ULONG )usRecordSize;
    }

    Op_CloseMldFileReleaseSem();
    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileCheckAndCreatFile(USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((sStatus = Op_MnemonicsFileOpenFile(usMnemonicFileId)) < 0) {
		if (sStatus != OP_NO_MAKE_FILE) {
			sStatus = OpMnemonicsFileCreate (usMnemonicFileId, usNumberOfAddress, usLockHnd);
			return (sStatus);
		} else {
			return (sStatus);
		}
    }

    PifCloseFile(hsOpMldFileHandle);
	hsOpMldFileHandle = -1;
    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileCheckAndDeleteFile(USHORT usMnemonicFileId, USHORT usNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed;

    Op_LockCheck(usLockHnd);

    if ((Op_MnemonicsFileOpenFile(usMnemonicFileId)) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));

    if ( MldFileHed.usNumberOfAddress < usNumberOfAddress ) {
		MldFileHed.usNumberOfAddress = usNumberOfAddress;
		Op_WriteMldFile (OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    PifCloseFile(hsOpMldFileHandle);

    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileAssign(OPMNEMONICFILE *pMnemonicFile, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed;
	TCHAR           *auchFileName;
	USHORT          usRecordSize, usMaxRecordCount;

    /*  CHECK LOCK */
	NHPOS_ASSERT(sizeof(TCHAR) == sizeof(pMnemonicFile->aszMnemonicValue[0]));
	NHPOS_ASSERT(sizeof(pMnemonicFile->aszMnemonicValue) >= OP_MLD_MNEMONICS_LEN * sizeof(TCHAR));

	if (0 == pMnemonicFile->usMnemonicFileId || pMnemonicFile->usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return 0;
	}

	auchFileName = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].auchFileName;
	usRecordSize = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usRecordSize;
	usMaxRecordCount = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usMaxRecordCount;

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */
    if (Op_MnemonicsFileOpenFile(pMnemonicFile->usMnemonicFileId) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));

	// If the file header indicates a smaller max record count than we show then
	// this is probably due to a new release using an older file so update the file.
	if (MldFileHed.usNumberOfAddress < usMaxRecordCount) {
		MldFileHed.usNumberOfAddress = usMaxRecordCount;
		Op_WriteMldFile (OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    /* CHECK RANGE OF MLD NO  */
    if ( (pMnemonicFile->usMnemonicAddress == 0) || (pMnemonicFile->usMnemonicAddress > MldFileHed.usNumberOfAddress ) ) {
        Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_MLD);
    }

    Op_WriteMldFile( sizeof(MldFileHed) + (ULONG)((pMnemonicFile->usMnemonicAddress - 1) * (usRecordSize + 1) * sizeof(TCHAR)), pMnemonicFile->aszMnemonicValue, usRecordSize * sizeof(TCHAR));
    Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

SHORT   OpMnemonicsFileIndRead(OPMNEMONICFILE *pMnemonicFile, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed;
	TCHAR           *auchFileName;
	USHORT          usRecordSize, usMaxRecordCount;

    memset(&pMnemonicFile->aszMnemonicValue, 0, sizeof(pMnemonicFile->aszMnemonicValue));

	if (0 == pMnemonicFile->usMnemonicFileId || pMnemonicFile->usMnemonicFileId > sizeof(OpMnemonicsFileTable)/sizeof(OpMnemonicsFileTable[0])) {
		return 0;
	}

	auchFileName = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].auchFileName;
	usRecordSize = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usRecordSize;
	usMaxRecordCount = OpMnemonicsFileTable[pMnemonicFile->usMnemonicFileId].usMaxRecordCount;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (usLockHnd) {
        if (Op_LockCheck(usLockHnd) == OP_LOCK) {
            PifReleaseSem(husOpSem);
            return (OP_LOCK);
        }
    }

    if (Op_MnemonicsFileOpenFile(pMnemonicFile->usMnemonicFileId) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(MldFileHed));

    if ( (pMnemonicFile->usMnemonicAddress == 0) || (pMnemonicFile->usMnemonicAddress > MldFileHed.usNumberOfAddress ) ) {
        Op_CloseMldFileReleaseSem();
		NHPOS_ASSERT_TEXT(0, "OpMldIndRead(): ( pMld->uchAddress > (UCHAR)MldFileHed.usNumberOfAddress)");
        return (OP_PERFORM);
    }

    Op_ReadMldFile((ULONG)(sizeof(MldFileHed) + (pMnemonicFile->usMnemonicAddress - 1) * (usRecordSize + 1) * sizeof(TCHAR)),
                    &pMnemonicFile->aszMnemonicValue, usRecordSize * sizeof(TCHAR));

    Op_CloseMldFileReleaseSem();
    return (OP_PERFORM);
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/*
/*
*==========================================================================
**    Synopsis:    SHORT   OpOepCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd)
*
*
*       Input:    usNumberOfPlu - Number of Plu
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Oep File. 2172
*==========================================================================
*/
SHORT   OpOepCreatFile(USHORT  usOepTableNo, ULONG ulNumberOfPlu, USHORT usLockHnd)
{
	TCHAR         CONST *auchOP_OEP_CurrentFile = 0;
    OPOEP_FILEHED OepFileHed;
    ULONG         ulActualPosition;
//  UCHAR         auchParaEmpTable[OP_OEPPLU_PARA_EMPTY_TABLE_SIZE];

    Op_LockCheck(usLockHnd);  /* Dummy */

	if (usOepTableNo < sizeof(auchOP_OEP_TABLE)/sizeof(auchOP_OEP_TABLE[0])) {
		auchOP_OEP_CurrentFile = auchOP_OEP_TABLE[usOepTableNo];
	} else {
		return(OP_NO_MAKE_FILE);
	}

    /* Creat Operational file */
    PifDeleteFile(auchOP_OEP_CurrentFile);

    /* Check Number of Dept */
    if (!ulNumberOfPlu) {           /* usNumberOfPlu == 0 */
        return(SHORT)OP_PERFORM;
    }

    if ((hsOpOepFileHandle = PifOpenFile(auchOP_OEP_CurrentFile, auchNEW_FILE_WRITE)) < 0x0000) {  /* Don't occured error */
        PifAbort(MODULE_OP_OEP, FAULT_ERROR_FILE_OPEN);
    }

    /* Make File Header */
    if(ulNumberOfPlu > OP_NUMBER_OF_MAX_OEP)
        OepFileHed.usNumberOfMaxOep = OP_NUMBER_OF_MAX_OEP;
    else
        OepFileHed.usNumberOfMaxOep = (SHORT)ulNumberOfPlu;

    OepFileHed.usNumberOfResidentOep = 0x00;
    OepFileHed.offulOfIndexTbl = sizeof(OepFileHed);
    OepFileHed.usNumberOfResidentOep2 = 0x00;
    OepFileHed.offulOfIndexTbl2 = OepFileHed.offulOfIndexTbl + (OepFileHed.usNumberOfMaxOep * sizeof(OPOEP_INDEXENTRY));

    OepFileHed.ulOepFileSize = OepFileHed.offulOfIndexTbl2 + (OepFileHed.usNumberOfMaxOep * sizeof(OPOEP_INDEXENTRY));

    /* Check file size and creat file */

    if (PifSeekFile(hsOpOepFileHandle, OepFileHed.ulOepFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpOepFileHandle);
		hsOpOepFileHandle = -1;
        PifDeleteFile(auchOP_OEP);          /* Can't make file then delete file */
        return(OP_NO_MAKE_FILE);
    }

    /* Write File Header */
    Op_WriteOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));

    PifCloseFile(hsOpOepFileHandle);
	hsOpOepFileHandle = -1;
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpOepCheckAndCreatFile(ULONG ulNumberOfDept,
*                                                  USHORT usLockHnd)
*
*       Input:    ulNumberOfDept - Number of OEP
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*    Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create OEP File. saratoga
*==========================================================================
*/
SHORT   OpOepCheckAndCreatFile(USHORT  usOepTableNo, ULONG ulNumberOfOep, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((Op_OepOpenFile(usOepTableNo)) < 0) {
        sStatus = OpOepCreatFile(usOepTableNo, ulNumberOfOep, usLockHnd);
        return(sStatus);
    }

    PifCloseFile(hsOpOepFileHandle);
	hsOpOepFileHandle = -1;
    return(OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpOepCheckAndDeleteFile(ULONG ulNumberOfOep,
*                                                  USHORT usLockHnd)
*
*       Input:    ulNumberOfOep - Number of Oep
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*                   OP_DELETE_DEPT_FILE
**  Description:  Check OEP size, if not equal the delete OEP File.
*==========================================================================
*/
SHORT   OpOepCheckAndDeleteFile(USHORT  usOepTableNo, ULONG ulNumberOfOep, USHORT usLockHnd)
{
	TCHAR         CONST * auchOP_OEP_CurrentFile = 0;
    OPOEP_FILEHED OepFileHed;

	if (usOepTableNo < sizeof(auchOP_OEP_TABLE)/sizeof(auchOP_OEP_TABLE[0])) {
		auchOP_OEP_CurrentFile = auchOP_OEP_TABLE[usOepTableNo];
	} else {
		return(OP_PERFORM);
	}

	Op_LockCheck(usLockHnd);

    if ((Op_OepOpenFile(usOepTableNo)) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return(OP_PERFORM);
    }

    Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OPOEP_FILEHED));

    PifCloseFile(hsOpOepFileHandle);
	hsOpOepFileHandle = -1;

    if (ulNumberOfOep > OP_NUMBER_OF_MAX_OEP) {
        ulNumberOfOep = OP_NUMBER_OF_MAX_OEP;
    }

    if ( OepFileHed.usNumberOfMaxOep == (USHORT)ulNumberOfOep ) {
        return(OP_PERFORM);
    }

    PifLog(MODULE_OP_OEP, LOG_EVENT_OP_OEP_DELETE_FILE);
    PifDeleteFile(auchOP_OEP_CurrentFile);
    return(OP_DELETE_OEP_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluCreatFile(ULONG ulNumberOfPlu,
*                                         USHORT usLockHnd)
*
*       Input:    ulNumberOfPlu - Number of Plu
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
                   OP_FILE_NOT_FOUND
                   OP_LOCKE
                   OP_NO_MAKE_FILE
*
*
**  Description:  Create Plu File.
*==========================================================================
*/

SHORT   OpPluCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd)
{
    USHORT usResult;
    SHORT  sStatus;

    PifRequestSem(husOpSem);
    if(Op_LockCheck(usLockHnd) == OP_LOCK)  /* Dummy */
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluDeleteFile(FILE_PLU);
    if(usResult) {
        /* ozawa */
        if (usResult != SPLU_FILE_NOT_FOUND) {
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }
    }

    /* ozawa */
    usResult = PluDeleteFile(FILE_PLU_INDEX);
    if(usResult) {
        /* ozawa */
        if (usResult != SPLU_FILE_NOT_FOUND) {
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }
    }

    /* Check Number of Plu */
    if (!ulNumberOfPlu)           /* ulNumberOfPlu == 0 */
    {
        PifReleaseSem(husOpSem);
        return OP_PERFORM;
    }

    sStatus = OpOepCreatFile(0, ulNumberOfPlu, usLockHnd);
    if(sStatus)
    {
        PifReleaseSem(husOpSem);
        return sStatus;
    }

    usResult = PluCreateFile(FILE_PLU,ulNumberOfPlu);
    switch(usResult)
    {
    case SPLU_DEVICE_FULL:
        OpOepCreatFile(0, 0, usLockHnd);    /* remove appendix files */
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);

    case SPLU_INVALID_DATA:
    case SPLU_REC_LOCKED:
    case SPLU_FILE_LOCKED:
        Op_PluAbort(FAULT_ERROR_FILE_OPEN);
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
        break;
    case SPLU_COMPLETED:
    default:
        break;
    }

    /* ozawa */
    usResult = PluCreateFile(FILE_PLU_INDEX,ulNumberOfPlu);
    switch(usResult)
    {
    case SPLU_DEVICE_FULL:
        OpOepCreatFile(0, 0, usLockHnd);    /* remove appendix files */
        PluDeleteFile(FILE_PLU);
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);

    case SPLU_INVALID_DATA:
    case SPLU_REC_LOCKED:
    case SPLU_FILE_LOCKED:
        Op_PluAbort(FAULT_ERROR_FILE_OPEN);
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
        break;
    case SPLU_COMPLETED:
    default:
        break;
    }

    PifReleaseSem(husOpSem);
    return OP_PERFORM;
}


/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptCreatFile(USHORT usNumberOfDept,
*                                         USHORT usLockHnd)
*
*       Input:    usNumberOfDept - Number of Dept
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Dept File. 2172
*==========================================================================
*/

SHORT   OpDeptCreatFile(USHORT usNumberOfDept, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    ULONG         ulActualPosition;
    UCHAR         auchParaEmpTable[OPDEPT_PARA_EMPTY_TABLE_SIZE];

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Creat Operational file */
    PifDeleteFile(auchOP_DEPT);

    /* Check Number of Dept */
    if (!usNumberOfDept) {           /* usNumberOfDept == 0 */
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    if ((hsOpDeptFileHandle = PifOpenFile(auchOP_DEPT, auchNEW_FILE_WRITE)) < 0x0000) {  /* Don't occured error */
        PifAbort(MODULE_OP_DEPT, FAULT_ERROR_FILE_OPEN);
    }

    /* Make File Header */
    DeptFileHed.usNumberOfMaxDept = usNumberOfDept;
    DeptFileHed.usNumberOfResidentDept = 0x00;
    DeptFileHed.offulOfIndexTbl = sizeof(DeptFileHed);
    DeptFileHed.offulOfIndexTblBlk = DeptFileHed.offulOfIndexTbl + (usNumberOfDept * sizeof(OPDEPT_INDEXENTRY));
    DeptFileHed.offulOfParaEmpTbl =  DeptFileHed.offulOfIndexTblBlk + (usNumberOfDept * OP_DEPT_INDEXBLK_SIZE);
    DeptFileHed.offulOfParaTbl = DeptFileHed.offulOfParaEmpTbl + OPDEPT_PARA_EMPTY_TABLE_SIZE;
    DeptFileHed.offulOfWork   = DeptFileHed.offulOfParaTbl + (ULONG)(((ULONG)usNumberOfDept) * sizeof(OPDEPT_PARAENTRY) );
    DeptFileHed.ulDeptFileSize = DeptFileHed.offulOfWork + usNumberOfDept;

    /* Check file size and creat file */
    if (PifSeekFile(hsOpDeptFileHandle, DeptFileHed.ulDeptFileSize, &ulActualPosition) < 0) { /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpDeptFileHandle);
        PifDeleteFile(auchOP_DEPT);          /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    /* Write File Header */
    Op_WriteDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    /* Clear and Write Empty table */
    memset(auchParaEmpTable, 0x00, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_WriteDeptFile((ULONG)DeptFileHed.offulOfParaEmpTbl, auchParaEmpTable, OPDEPT_PARA_EMPTY_TABLE_SIZE);
    Op_CloseDeptFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnCreatFile(USHORT usNumberOfCpn,
*                                          USHORT usLockHnd)
*
*       Input:    usNumberOfCpn - Number of Combination Coupon
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create Combination Coupon File.
*==========================================================================
*/
SHORT   OpCpnCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd)
{
    OPCPN_FILEHED  *pCpnFileHead;
    ULONG          ulActualPosition;
    ULONG          ulFilePos;
    UCHAR          auchCpnFileHead[sizeof(OPCPN_FILEHED)];
    UCHAR          auchCpnFile[sizeof(COMCOUPONPARA) * OP_CPN_NUMBER_OF_MAX / 10];
    USHORT         usLoop,usLoopMod;

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Delete Cpn File */
    PifDeleteFile(auchOP_CPN);

    /* Check Number of Cpn */
    if (!usNumberOfCpn) {         /* usNumberOfCpn == 0 */
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    /* Creat Cpn File */
    if ((hsOpCpnFileHandle = PifOpenFile(auchOP_CPN, auchNEW_FILE_WRITE)) < 0) {
        PifAbort(MODULE_OP_CPN, FAULT_ERROR_FILE_OPEN);
    }

    /* Check file size and creat file */
    if (PifSeekFile(hsOpCpnFileHandle, (ULONG)(sizeof(OPCPN_FILEHED) + usNumberOfCpn * sizeof(COMCOUPONPARA)), &ulActualPosition) < 0) {  /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpCpnFileHandle);
        PifDeleteFile(auchOP_CPN);
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    /* Make Header File */
    pCpnFileHead = (OPCPN_FILEHED *)auchCpnFileHead;
    pCpnFileHead->usNumberOfCpn = usNumberOfCpn;

    /* Write Header File */
    Op_WriteCpnFile(OP_FILE_HEAD_POSITION, auchCpnFileHead, (USHORT)sizeof(OPCPN_FILEHED)); /* if error, let system error */
    /* Clear Contents of File */

    memset(&auchCpnFile[0], 0x00, sizeof(auchCpnFile));
    ulFilePos = (ULONG)(sizeof(OPCPN_FILEHED));
    usLoop = usNumberOfCpn / 10;
    usLoopMod = usNumberOfCpn % 10;
    for(;usLoop != 0;usLoop--){
        /* Write File */
        Op_WriteCpnFile(ulFilePos, auchCpnFile, (USHORT)(sizeof(auchCpnFile))); /* if error, let system error */
        ulFilePos += sizeof(auchCpnFile);
    }
    if(usLoopMod){
        Op_WriteCpnFile(ulFilePos, auchCpnFile, (USHORT)(usLoopMod * sizeof(COMCOUPONPARA))); /* if error, let system error */
    }

    Op_CloseCpnFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrCreatFile(USHORT usNumberOfCstr,
*                                          USHORT usLockHnd)
*
*       Input:    usNumberOfCstr - Number of Control String
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Control String File.
*==========================================================================
*/
SHORT   OpCstrCreatFile(USHORT usNumberOfCstr, USHORT usLockHnd)
{
    OPCSTR_FILEHED  CstrFileHed;
    ULONG         ulActualPosition;
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Creat Operational file */
    PifDeleteFile(auchOP_CSTR);

    /* Check Number of Cstr */
    if (!usNumberOfCstr) {           /* usNumberOfCstr == 0 */
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    if ((hsOpCstrFileHandle = PifOpenFile(auchOP_CSTR, auchNEW_FILE_WRITE)) < 0x0000) {  /* Don't occured error */
        Op_PluAbort(FAULT_ERROR_FILE_OPEN);
    }

    /* Make File Header */
    CstrFileHed.usNumberOfCstr = usNumberOfCstr;
    CstrFileHed.usOffsetOfTransCstr = 0x00;

    /* Check file size and creat file */
    if (PifSeekFile(hsOpCstrFileHandle, (ULONG)(sizeof(OPCSTR_FILEHED) + sizeof(ausIndexTable) + (usNumberOfCstr * sizeof(OPCSTR_PARAENTRY))), &ulActualPosition) < 0) {  /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpCstrFileHandle);
        PifDeleteFile(auchOP_CSTR);
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    /* Write File Header */
    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Clear and Write Index table */
    memset(ausIndexTable, 0x00, sizeof(ausIndexTable));
    Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed)), ausIndexTable, sizeof(ausIndexTable));
    Op_CloseCstrFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluCheckAndCreatFile(ULONG ulNumberOfPlu,
*                                                 USHORT usLockHnd)
*
*       Input:    ulNumberOfPlu - Number of Plu
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_LOCKE
*                  OP_FILE_NOT_FOUND
*                  OP_NO_MAKE_FILE
*
**  Description: Check if not exist, then Create Plu File.
*==========================================================================
*/
SHORT   OpPluCheckAndCreatFile(ULONG ulNumberOfPlu, USHORT usLockHnd)
{
    SHORT   sStatus;
    USHORT usResult;
    MFINFO mfinfo;

    usResult = PluSenseFile(FILE_PLU,&mfinfo);

    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        sStatus = OpPluCreatFile(ulNumberOfPlu, usLockHnd);
    }
    else
    {
        sStatus = OpConvertPluError(usResult);
    }

    return sStatus;
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluCheckAndDeleteFile(ULONG ulNumberOfPlu,
*                                                 USHORT usLockHnd)
*
*       Input:    ulNumberOfPlu - Number of Plu
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_LOCKE
*                  OP_FILE_NOT_FOUND

**  Description:  Check if not equal, then delete Plu File.
*==========================================================================
*/
SHORT   OpPluCheckAndDeleteFile(ULONG ulNumberOfPlu, USHORT usLockHnd)
{
    USHORT usResult;
    MFINFO mfinfo;

    PifRequestSem(husOpSem);
    if(Op_LockCheck(usLockHnd) == OP_LOCK)  /* Dummy */
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OP_PERFORM;
    }
    else
    {
        if(mfinfo.ulMaxRec == ulNumberOfPlu)
        {
            PifReleaseSem(husOpSem); /* ozawa */
            return OP_PERFORM;
        }
        else
        {
            PifLog(MODULE_OP_PLU, LOG_EVENT_OP_PLU_DELETE_FILE);
            usResult = PluDeleteFile(FILE_PLU);
        }
    }
    PifReleaseSem(husOpSem);
    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptCheckAndCreatFile(USHORT usNumberOfDept,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfDept - Number of Dept
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*    Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Dept File.
*==========================================================================
*/
SHORT   OpDeptCheckAndCreatFile(USHORT usNumberOfDept, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((Op_DeptOpenFile()) < 0) {
        sStatus = OpDeptCreatFile(usNumberOfDept, usLockHnd);
        return(sStatus);
    }

    PifCloseFile(hsOpDeptFileHandle);
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptCheckAndDeleteFile(USHORT usNumberOfDept,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfDept - Number of Dept
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*                   OP_DELETE_DEPT_FILE
**  Description:  Check dept size, if not equal the delete Dept File.
*==========================================================================
*/
SHORT   OpDeptCheckAndDeleteFile(USHORT usNumberOfDept, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;

    Op_LockCheck(usLockHnd);

    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(OPDEPT_FILEHED));

    PifCloseFile(hsOpDeptFileHandle);

    if ( DeptFileHed.usNumberOfMaxDept == usNumberOfDept ) {
        return (OP_PERFORM);
    }

    PifLog(MODULE_OP_DEPT, LOG_EVENT_OP_DEPT_DELETE_FILE);
    PifDeleteFile(auchOP_DEPT);
    return (OP_DELETE_DEPT_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnCheckAndCreatFile(USHORT usNumberOfCpn,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfCpn - Number of Combination Coupon
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*    Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Combination Coupon File.
*==========================================================================
*/
SHORT   OpCpnCheckAndCreatFile(USHORT usNumberOfCpn, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((Op_CpnOpenFile()) < 0) {
        sStatus = OpCpnCreatFile(usNumberOfCpn, usLockHnd);
        return(sStatus);
    }

    PifCloseFile(hsOpCpnFileHandle);
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnCheckAndDeleteFile(USHORT usNumberOfCpn,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfCpn - Number of Combination Coupon
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*                   OP_DELETE_CPN_FILE
**  Description:  Check Combination Coupon size, if not equal the delete
*                 Combination Coupon File.
*==========================================================================
*/
SHORT   OpCpnCheckAndDeleteFile(USHORT usNumberOfCpn, USHORT usLockHnd)
{
    OPCPN_FILEHED CpnFileHed;

    Op_LockCheck(usLockHnd);

    if ((Op_CpnOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(OPCPN_FILEHED));

    PifCloseFile(hsOpCpnFileHandle);

    if ( CpnFileHed.usNumberOfCpn == usNumberOfCpn ) {
        return (OP_PERFORM);
    }

    PifLog(MODULE_OP_CPN, LOG_EVENT_OP_CPN_DELETE_FILE);
    PifDeleteFile(auchOP_CPN);
    return (OP_DELETE_CPN_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrCheckAndCreatFile(USHORT usNumberOfCstr,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfCstr - Number of Control String
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*    Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Control String File.
*==========================================================================
*/
SHORT   OpCstrCheckAndCreatFile(USHORT usNumberOfCstr, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((Op_CstrOpenFile()) < 0) {
        sStatus = OpCstrCreatFile(usNumberOfCstr, usLockHnd);
        return(sStatus);
    }

    PifCloseFile(hsOpCstrFileHandle);
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrCheckAndDeleteFile(USHORT usNumberOfCstr,
*                                                   USHORT usLockHnd)
*
*       Input:    usNumberOfCstr - Number of Control String
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*                   OP_DELETE_CSTR_FILE
**  Description:  Check Control String size, if not equal the delete
*                 Control String File.
*==========================================================================
*/
SHORT   OpCstrCheckAndDeleteFile(USHORT usNumberOfCstr, USHORT usLockHnd)
{
    OPCSTR_FILEHED CstrFileHed;

    Op_LockCheck(usLockHnd);

    if ((Op_CstrOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    PifCloseFile(hsOpCstrFileHandle);

    if ( CstrFileHed.usNumberOfCstr == usNumberOfCstr ) {
        return (OP_PERFORM);
    }

    PifLog(MODULE_OP_CSTR, LOG_EVENT_OP_CSTR_DELETE_FILE);
    PifDeleteFile(auchOP_CSTR);
    return (OP_DELETE_CSTR_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd)
*
*       Input:  pDpi->usDeptNo
*               pDpi->ParaDept
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_NEW_DEPT
*   Abnormal End:  OP_CHANGE_DEPT
*                  OP_FILE_NOT_FOUND
*                  OP_DEPT_FULL
*                  OP_LOCK
*
**  Description:  Add and change dept record.    2172
*==========================================================================
*/
SHORT   OpDeptAssign(DEPTIF *pDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    SHORT         sStatus;
    USHORT        usParaBlockNo;

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    /* CHECK ASSIGN */
    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));  /* get dept file header */
    if ((sStatus = Op_DeptIndex(&DeptFileHed, OP_REGIST, pDpi->usDeptNo, &usParaBlockNo)) == OP_DEPT_FULL) {
        Op_CloseDeptFileReleaseSem();
        return (OP_DEPT_FULL);
    }

    /*  MAKE PARAMATER */
    Op_DeptParaWrite(&DeptFileHed, usParaBlockNo, &pDpi->ParaDept);
    Op_CloseDeptFileReleaseSem();
    return (sStatus);               /* usStatus = OP_NEW_DEPT or OP_CHANGE_DEPT */
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAssign(PLUIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif->auchPluNo
*               pPif->uchPluAdj
*               pPif->ParaPlu
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_LOCKE
*                  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_NOT_IN_FILE
*                  OP_PLU_FULL
*
*
**  Description:  Add and change plu record.
*==========================================================================
*/
SHORT   OpPluAssign(PLUIF *pPif, USHORT usLockHnd)
{
	SHORT  sRetStatus;
    MFINFO mfinfo;
    USHORT usResult;
    USHORT usRecPluLen;
    RECPLU recplu;
    RECPLU recpluold;
    PRECPLU pRecPlu;
    PRECPLU pRecPluOld;
    USHORT  usAdj, i;

    pRecPlu = &recplu;
    pRecPluOld = &recpluold;

    usAdj = (USHORT)pPif->uchPluAdj;
    usAdj--; /* decrement adjective code */

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

    usRecPluLen = 0;
    memset(pRecPlu,0,sizeof(RECPLU));
    memset(pRecPluOld,0,sizeof(RECPLU));

	OpAssignPifPluToRecPlu (pRecPlu, pPif);
	pRecPlu->uchAdjective  = 1;  /* always adjective code is 1 on PLU record */
    pRecPlu->ulUnitPrice[usAdj]       = RflFunc3bto4b(pPif->ParaPlu.auchPrice);

    usResult = PluReplaceRecord(FILE_PLU, pRecPlu, pRecPluOld, &usRecPluLen);
	sRetStatus = OpConvertPluError(usResult);

    if(usResult == 0)
    {
        sRetStatus = OP_NEW_PLU;
        if(usRecPluLen > 0)
        {
            /* preserve other adjective price */
            for (i=0; i < MAX_ADJECTIVE_NO; i++) {
                if (i != usAdj) {
                    pRecPlu->ulUnitPrice[i] = pRecPluOld->ulUnitPrice[i];
                }
            }
            usResult = PluReplaceRecord(FILE_PLU, (VOID*)pRecPlu, (VOID*)pRecPluOld, &usRecPluLen);
            sRetStatus = OP_CHANGE_PLU;
        }
    }

    PifReleaseSem(husOpSem);

	return sRetStatus;
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnAssign(CPNTIF *pCpni, USHORT usLockHnd)
*
*       Input:   pCpni->usCpnNo
*                pCpni->ParaCpn
*                USHORT usLockHnd
*
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_FILE_NOT_FOUND
*                  OP_ABNORMAL_CPN - HALT
*                  OP_LOCK
*
**  Description:  Add and Chnge Combination Coupon.
*==========================================================================
*/
SHORT   OpCpnAssign(CPNIF *pCpni, USHORT usLockHnd)
{
    OPCPN_FILEHED CpnFileHed;

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */
    if ((Op_CpnOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(OPCPN_FILEHED));

    /* CHECK RANGE OF COMBINATION COUPON NO  */
    if (pCpni->uchCpnNo == 0) {
        Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_CPN);
    }

    if ( pCpni->uchCpnNo > CpnFileHed.usNumberOfCpn ) {
        Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_CPN);
    }

    Op_WriteCpnFile( sizeof(CpnFileHed) + (ULONG)((pCpni->uchCpnNo - 1) * sizeof(COMCOUPONPARA)), &pCpni->ParaCpn, sizeof(pCpni->ParaCpn));
    Op_CloseCpnFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd)
*
*       Input:  pCstr->usCstrNo
*               pCstr->IndexCstr
*               pCstr->ParaCstr
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_FILE_NOT_FOUND
*                  OP_ABNORMAL_CSTR - HALT
*                  OP_LOCK
*
**  Description:  Add and Chnge Control String.
*==========================================================================
*/
SHORT   OpCstrAssign(CSTRIF *pCstr, USHORT usLockHnd)
{
    OPCSTR_FILEHED CstrFileHed;
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];
    OPCSTR_PARAENTRY  ausParaCstr[OP_CSTR_PARA_SIZE];
    USHORT  ausSortPtr[OP_CSTR_INDEX_SIZE + 1],
            ausIDBase,
            ausID1,
            ausID2,
            ausID3;
    USHORT  ausNumber,
            ausOffset;

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */
    if ((Op_CstrOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* CHECK RANGE OF Control Sting Number  */
    if (pCstr->usCstrNo == 0) {
         Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_CSTR);
    }

    if ( pCstr->usCstrNo > OP_CSTR_INDEX_SIZE ) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_CSTR);
    }

    Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed)), &ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    ausNumber = (USHORT)(( CstrFileHed.usOffsetOfTransCstr + pCstr->IndexCstr.usCstrLength - ausIndexTable[pCstr->usCstrNo - 1].usCstrLength ) / sizeof(OPCSTR_PARAENTRY));

    if ( CstrFileHed.usNumberOfCstr < ausNumber ) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return (OP_FILE_FULL);
    }

    if ((ausIndexTable[pCstr->usCstrNo - 1].usCstrLength != 0) || (ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset != 0)) {

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;

        ausIDBase = 0;

        ausSortPtr[ausIDBase] = pCstr->usCstrNo;
        ausIDBase++;

        for (ausID1 = 0; ausID1 < OP_CSTR_INDEX_SIZE; ausID1++) {
            if ( ausOffset < ausIndexTable[ausID1].usCstrOffset) {
                ausSortPtr[ausIDBase] = (USHORT)(ausID1 + 1);
                ausIDBase++;
            }
        }

        for (ausID1 = 0; ausID1 < ausIDBase; ausID1++) {

            ausOffset = (USHORT)(ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
            for (ausID2 = (USHORT)(ausID1 + 1); ausID2 < ausIDBase; ausID2++) {
                if (ausOffset == ausIndexTable[ausSortPtr[ausID2] - 1].usCstrOffset){
                    ausID3 = ausSortPtr[ausID1 + 1];
                    ausSortPtr[ausID1 + 1] = ausSortPtr[ausID2];
                    ausSortPtr[ausID2] = ausID3;
                    break;
                }
            }
        }

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;
        for (ausID1 = 1; ausID1 < ausIDBase; ausID1++) {

            Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset),
                                    &ausParaCstr, ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausOffset),
                                     &ausParaCstr, ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            /* Set Index table */
            ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset = ausOffset;
            ausOffset = (USHORT)(ausOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
        }

    } else {
        ausOffset = CstrFileHed.usOffsetOfTransCstr;
    }

    /* Set Update Parameter data */
    if (pCstr->IndexCstr.usCstrLength == 0){
        ausOffset = 0;
    } else {
        Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausOffset),
                                 &pCstr->ParaCstr, pCstr->IndexCstr.usCstrLength);
    }

    /* Set Transaction Offset of Header */
    CstrFileHed.usOffsetOfTransCstr = (USHORT)(ausOffset + pCstr->IndexCstr.usCstrLength);

    /* Write File Header */
    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Set Index Table */
    ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset = ausOffset;
    ausIndexTable[pCstr->usCstrNo - 1].usCstrLength = pCstr->IndexCstr.usCstrLength;

    /* Write Index table */
    Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed)), ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd)
*
*       Input:  pCstr->usCstrNo
*               pCstr->IndexCstr
*               pCstr->ParaCstr
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_FILE_NOT_FOUND
*                  OP_ABNORMAL_CSTR - HALT
*                  OP_LOCK
*
**  Description:  Delete a Control String in file.
*==========================================================================
*/
SHORT   OpCstrDelete(CSTRIF *pCstr, USHORT usLockHnd)
{
    OPCSTR_FILEHED  CstrFileHed;
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];
    OPCSTR_PARAENTRY  ausParaCstr[OP_CSTR_PARA_SIZE];
    USHORT  ausSortPtr[OP_CSTR_INDEX_SIZE + 1],
            ausIDBase,
            ausID1,
            ausID2,
            ausID3;
    USHORT  ausOffset;

    /*  CHECK LOCK */
    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */
    if ((Op_CstrOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* CHECK RANGE OF Ccontrol String Number  */
    if (pCstr->usCstrNo == 0) {
         Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_CSTR);
    }

    if ( pCstr->usCstrNo > OP_CSTR_INDEX_SIZE ) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_CSTR);
    }

    Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed)), &ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    /* Check Parameter Data Length */
    if (ausIndexTable[pCstr->usCstrNo - 1].usCstrLength == 0) {
        Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
        return (OP_PERFORM);
    }

    if ( CstrFileHed.usOffsetOfTransCstr > ausIndexTable[pCstr->usCstrNo - 1].usCstrLength ) {

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;

        ausIDBase = 0;
        ausSortPtr[ausIDBase] = pCstr->usCstrNo;
        ausIDBase++;

        for (ausID1 = 0; ausID1 < OP_CSTR_INDEX_SIZE; ausID1++) {
            if ( ausOffset < ausIndexTable[ausID1].usCstrOffset) {
                ausSortPtr[ausIDBase] = (USHORT)(ausID1 + 1);
                ausIDBase++;
            }
        }

        for (ausID1 = 0; ausID1 < ausIDBase; ausID1++) {
            ausOffset = (USHORT)(ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
            for (ausID2 = (USHORT)(ausID1 + 1); ausID2 < ausIDBase; ausID2++) {
                if (ausOffset == ausIndexTable[ausSortPtr[ausID2] - 1].usCstrOffset){
                    ausID3 = ausSortPtr[ausID1 + 1];
                    ausSortPtr[ausID1 + 1] = ausSortPtr[ausID2];
                    ausSortPtr[ausID2] = ausID3;
                    break;
                }
            }
        }

        ausOffset =  ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;
        for (ausID1 = 1; ausID1 < ausIDBase; ausID1++) {

            Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset),
                                    &ausParaCstr,
                                    ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed) + (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) + ausOffset),
                                    &ausParaCstr,
                                    (USHORT)ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);

            /* Set Index table */
            ausIndexTable[ausSortPtr[ausID1] - 1].usCstrOffset = ausOffset;
            ausOffset = (USHORT)(ausOffset + ausIndexTable[ausSortPtr[ausID1] - 1].usCstrLength);
        }

        /* Set Transaction Offset of Header */
        CstrFileHed.usOffsetOfTransCstr = ausOffset;

        /* Set Index Table */
        ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset = 0x00;
        ausIndexTable[pCstr->usCstrNo - 1].usCstrLength = 0x00;
    }
    else  {
        /* Set Transaction Offset of Header */
        CstrFileHed.usOffsetOfTransCstr = 0x00;

        /* Clear Index table */
        memset(ausIndexTable, 0x00, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));
    }

    /* Write File Header */
    Op_WriteCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(CstrFileHed));

    /* Write Index table */
    Op_WriteCstrFile((ULONG)(sizeof(CstrFileHed)), ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    Op_CloseCstrFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluDelete(PLUIF *pPif, USHORT usLockHnd)
*
*       Input:   pPif->auchPluNo
*                pPif->uchPluAdj
*                USHORT usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCKE
*                  OP_NOT_IN_FILE
*
**  Description: Delete a Plu in file.
*==========================================================================
*/
SHORT   OpPluDelete(PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo;
    USHORT usResult;
    UCHAR  aucOldRec[sizeof(RECPLU)];		//Fix for problem with AC68 in supervisor mode causing crash, this was
											//previously hardcoded to 100, which was enough for the size of a RECPLU structure
											//took the hardcoded value out and used the actual size of the RECPLU structure. JHHJ
    USHORT usOldRecLen;

    PifRequestSem(husOpSem);

    if (pPif->uchPluAdj > 1) {

        /* delete record only adjective code is 1 */
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }
    else
    {
        usResult = PluDeleteRecord(FILE_PLU,            //File ID
                                   pPif->auchPluNo,     //ptr. to item number
                                   pPif->uchPluAdj,     //adjective number
                                   0,                   //programability
                                   aucOldRec,           //buffer ptr. to respond old rec.
                                   &usOldRecLen);       //buffer ptr. to respond rec. length
    }

    PifReleaseSem(husOpSem);
    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluFileUpdate(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
                   OP_EOF
*   Abnormal End:  OP_PLU_ERROR
*                  OP_LOCK
*                  OP_FILE_NOT_FOUND
*
*
**  Description:  Update Plu File (Make sub Index table)
*==========================================================================
*/
static SHORT OpPluFileUpdateOep (SHORT hsFileHandle, UCHAR uchItemType, UCHAR uchGroupNumber, UCHAR uchPriority, TCHAR *aucPluNumber)
{
    ULONG          offulSearchPoint = 0;
    USHORT         usResult = 1;  // not SPLU_COMPLETED as that is actually an error condition meaning file is full.
    OPOEP_FILEHED  OepFileHed;
	USHORT         usLoop;
	USHORT         *pusNumberOfResidentOep;
	TCHAR  auchOepPlu [OP_PLU_OEP_LEN];

	auchOepPlu[2] = _T('0');
	auchOepPlu[1] = _T('0');
	auchOepPlu[0] = _T('0');
	_tcsncpy (auchOepPlu+3, aucPluNumber, PLU_NUMBER_LEN);
	pusNumberOfResidentOep = &OepFileHed.usNumberOfResidentOep;

	hsOpOepFileHandle = hsFileHandle;  // override the current setting for the global file handle to insert this PLU into this OEP table file.

	for (usLoop = 0; usLoop < 2; usLoop++) {
		Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));
		/* OEP File limited to 8000 */
		if (*pusNumberOfResidentOep < OepFileHed.usNumberOfMaxOep) {
			if(Op_OepIndex(&OepFileHed, usLoop, OP_REGIST, uchGroupNumber, auchOepPlu, 1, &offulSearchPoint) == OP_NOT_IN_FILE)
			{
				usResult = SPLU_REC_NOT_FOUND;
				break;
			}
		} else {
			usResult = SPLU_COMPLETED;
			break;
		}

		if ((uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
			UCHAR     uchAdj;

			for (uchAdj = 2; uchAdj <= 5; uchAdj++) {
				Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));
				/* OEP File limited to 8000 */
				if (*pusNumberOfResidentOep < OepFileHed.usNumberOfMaxOep) {
					if(Op_OepIndex(&OepFileHed, usLoop, OP_REGIST, uchGroupNumber, auchOepPlu, uchAdj, &offulSearchPoint) == OP_NOT_IN_FILE)
					{
						usResult = SPLU_REC_NOT_FOUND;
						break;
					}
				} else {
					usResult = SPLU_COMPLETED;
					break;
				}
			}
		}

		auchOepPlu[2] = (uchPriority % 10) + _T('0');
		uchPriority /= 10;
		auchOepPlu[1] = (uchPriority % 10) + _T('0');
		uchPriority /= 10;
		auchOepPlu[0] = (uchPriority % 10) + _T('0');
		pusNumberOfResidentOep = &OepFileHed.usNumberOfResidentOep2;
	}

	return usResult;
}

SHORT   OpPluFileUpdate(USHORT usLockHnd)
{
    USHORT usResult;
    USHORT husHandle;
    MFINFO mfinfo;

    ECMRANGE ecmrange;
    PECMRANGE pEcmRange;
    RECPLU   recplu;
    PRECPLU  pRecPlu;
    USHORT   usRecLen;
	USHORT   usOepTableNumber;
	SHORT    husCurrentOepTableFile[4];
    PARAFLEXMEM        WorkFlex;

    pEcmRange = &ecmrange;
    pRecPlu = &recplu;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

    tcharnset(pEcmRange->itemFrom.aucPluNumber, _T('0'), PLU_NUMBER_LEN);
    tcharnset(pEcmRange->itemTo.aucPluNumber, _T('9'), PLU_NUMBER_LEN);
    pEcmRange->fsOption = REPORT_ACTIVE | REPORT_INACTIVE;
    pEcmRange->usReserve = 0;

    usResult = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &husHandle, pEcmRange);

    if(usResult)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

    /* recreate oep file at the plu file update */
    WorkFlex.uchMajorClass = CLASS_PARAFLEXMEM;
    WorkFlex.uchAddress = FLEX_PLU_ADR;
    CliParaRead(&WorkFlex);

	for (usOepTableNumber = 0; usOepTableNumber < 4; usOepTableNumber++) {
		if (usResult = OpOepCreatFile(usOepTableNumber, WorkFlex.ulRecordNumber, 0) != OP_PERFORM) {
			PluExitCritMode(husHandle,0);
			PifReleaseSem(husOpSem);
			PifLog(MODULE_OP_OEP, LOG_EVENT_OP_OEP_CREATE_FILE);
			PifLog(MODULE_DATA_VALUE(MODULE_OP_OEP), usOepTableNumber);
			PifLog(MODULE_ERROR_NO(MODULE_OP_OEP), (USHORT)abs(usResult));
			return (usResult);
		}

		if ((husCurrentOepTableFile[usOepTableNumber] = Op_OepOpenFile(usOepTableNumber)) < 0) /* pif_error_file_exist */
		{
			PluExitCritMode(husHandle,0);
			PifReleaseSem(husOpSem);
			PifLog(MODULE_OP_OEP, LOG_EVENT_OP_OEP_OPEN_FILE);
			PifLog(MODULE_DATA_VALUE(MODULE_OP_OEP), usOepTableNumber);
			PifLog(MODULE_ERROR_NO(MODULE_OP_OEP), (USHORT)abs(husCurrentOepTableFile[usOepTableNumber]));
			return OP_FILE_NOT_FOUND;
		}
	}

    while(1)
    {
        usResult = PluReportRecord(husHandle, pRecPlu, &usRecLen);

        if((usResult == SPLU_REC_LOCKED) || (usResult == SPLU_FUNC_CONTINUED) || (usResult == SPLU_COMPLETED))
        {
			usResult = 1;
            if (pRecPlu->uchGroupNumber) { /* */
				usResult = OpPluFileUpdateOep (husCurrentOepTableFile[0], pRecPlu->uchItemType, pRecPlu->uchGroupNumber, pRecPlu->uchPrintPriority, pRecPlu->aucPluNumber);
			}
			if (pRecPlu->uchExtendedGroupPriority[0]) {
				usResult = OpPluFileUpdateOep (husCurrentOepTableFile[1], pRecPlu->uchItemType, pRecPlu->uchExtendedGroupPriority[0], pRecPlu->uchExtendedGroupPriority[1], pRecPlu->aucPluNumber);
			}
			if (pRecPlu->uchExtendedGroupPriority[2]) {
				usResult = OpPluFileUpdateOep (husCurrentOepTableFile[2], pRecPlu->uchItemType, pRecPlu->uchExtendedGroupPriority[2], pRecPlu->uchExtendedGroupPriority[3], pRecPlu->aucPluNumber);
			}
			if (pRecPlu->uchExtendedGroupPriority[4]) {
				usResult = OpPluFileUpdateOep (husCurrentOepTableFile[3], pRecPlu->uchItemType, pRecPlu->uchExtendedGroupPriority[4], pRecPlu->uchExtendedGroupPriority[5], pRecPlu->aucPluNumber);
			}
			if (usResult != 1)
				break;
        }
        else if(usResult == SPLU_END_OF_FILE)
        {
            usResult = SPLU_COMPLETED;
            break;
        }
        else {
            break;
		}
    }

    PluExitCritMode(husHandle,0);

	// Close the multiple OEP table files we have open, clone of Op_CloseOepFileReleaseSem().
	PifCloseFile(husCurrentOepTableFile[0]);
    PifCloseFile(husCurrentOepTableFile[1]);
	PifCloseFile(husCurrentOepTableFile[2]);
    PifCloseFile(husCurrentOepTableFile[3]);
	hsOpOepFileHandle = -1;

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluIndRead(PLUIF *Pif, USHORT usLockHnd)
*
*       Input:    *Pif->auchPluNo
*                 *Pif->uchPluAdj
*                 usLockHnd
*      Output:    PLUIF  *Pif->ParaPlu
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*                  OP_NOT_IN_FILE
*
*
**  Description: Read a Plu paramater Individually.
*==========================================================================
*/
SHORT   OpPluIndRead(PLUIF *pPif, USHORT usLockHnd)
{
    MFINFO mfinfo;
    USHORT usResult;
    USHORT usRecPluLen;
    RECPLU recplu;
    PRECPLU pRecPlu;
    ITEMNO itemno;
    PITEMNO pItemNumber;

    pRecPlu = &recplu;
    pItemNumber = &itemno;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    /** removed, V1.0.13
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    } **/

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

    _tcsncpy(pItemNumber->aucPluNumber,pPif->auchPluNo, PLU_NUMBER_LEN);
    usResult = PluReadRecord(FILE_PLU, pItemNumber, 1, pRecPlu, &usRecPluLen);
    if(!usResult)
    {
		OpAssignRecPluToPifPlu (pPif, pRecPlu);
		RflConv4bto3b(pPif->ParaPlu.auchPrice, pRecPlu->ulUnitPrice[pPif->uchPluAdj-1]);
    }

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd)
*
*       Input:   pDpi->usDeptNo
*                USHORT usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Delete a dept in file. 2172
*==========================================================================
*/
SHORT   OpDeptDelete(DEPTIF *pDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    SHORT         sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN DEPT FILE */

    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }
    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    /* CHECK ASSIGN DEPT AND DELETED */

    sStatus = Op_DeptIndexDel(&DeptFileHed, pDpi->usDeptNo);

    Op_CloseDeptFileReleaseSem();
    return (sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptIndRead(DEPTIF *Dpi, USHORT usLockHnd)
*
*       Input:    *Dpi->usDeptNo
*                 usLockHnd
*      Output:    DEPTIF  *Dpi->ParaDept
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a Dept paramater Individually. 2172
*==========================================================================
*/
SHORT   OpDeptIndRead(DEPTIF *pDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    USHORT        usParaBlockNo;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    if (Op_DeptIndex(&DeptFileHed, OP_SEARCH_ONLY,
                 pDpi->usDeptNo,
                 &usParaBlockNo) == OP_NOT_IN_FILE) {
        Op_CloseDeptFileReleaseSem();
        return (OP_NOT_IN_FILE);
    }

    Op_DeptParaRead(&DeptFileHed, usParaBlockNo, &pDpi->ParaDept);
    Op_CloseDeptFileReleaseSem();
    return (OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpCpnIndRead(CPNIF *pCpni, USHORT usLockHnd)
*
*       Input:   *pCpni->usCpnNo
*                usLockHnd
*      Output:   CPNIF *pCpni->ParaCpn
*       InOut:   Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a Combination Coupon Individually
*==========================================================================
*/
SHORT   OpCpnIndRead(CPNIF *pCpni, USHORT usLockHnd)
{
    OPCPN_FILEHED CpnFileHed;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    if ((Op_CpnOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadCpnFile(OP_FILE_HEAD_POSITION, &CpnFileHed, sizeof(OPCPN_FILEHED));

    if ((!pCpni->uchCpnNo) ||
		( pCpni->uchCpnNo > CpnFileHed.usNumberOfCpn)) {  //Removed UCHAR cast for coupon max 100->300 JHHJ
        Op_CloseCpnFileReleaseSem();
        return (OP_ABNORMAL_CPN);
    }

    Op_ReadCpnFile((ULONG)(sizeof(CpnFileHed) + ((pCpni->uchCpnNo - 1)
                    * sizeof(COMCOUPONPARA))),
                    &pCpni->ParaCpn, sizeof(COMCOUPONPARA));

    Op_CloseCpnFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd)
*
*       Input:   *pCstr->usCstrNo
*                usLockHnd
*      Output:   CSTRIF *pCstr->ParaCstr
*                CSTRIF *pCstr->IndexCstr
*       InOut:   Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_ABNORMAL_CSTR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a Control String Individually
*==========================================================================
*/
SHORT   OpCstrIndRead(CSTRIF *pCstr, USHORT usLockHnd)
{
    OPCSTR_FILEHED  CstrFileHed;
    OPCSTR_INDEXENTRY ausIndexTable[OP_CSTR_INDEX_SIZE];

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    /* Removing Lock Check to allow Control Strings to start/run
		within AC codes (like AC 68, 63, 82, 114) - CSMALL 08/30/2006
	if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }
	*/

    if ((Op_CstrOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    /* Read Header in Control String File */

    Op_ReadCstrFile(OP_FILE_HEAD_POSITION, &CstrFileHed, sizeof(OPCSTR_FILEHED));

    /* Check input Data of Number */

    if ((!pCstr->usCstrNo) ||
        ( pCstr->usCstrNo > OP_CSTR_INDEX_SIZE)) {
        Op_CloseCstrFileReleaseSem();
        return (OP_ABNORMAL_CPN);
    }

    /* Read Index Table in Control String File */
    Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed)),
                    &ausIndexTable, (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE));

    /* Set Index Data */
    pCstr->IndexCstr.usCstrOffset = ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset;
    pCstr->IndexCstr.usCstrLength = ausIndexTable[pCstr->usCstrNo - 1].usCstrLength;

    /* Read area clear */
    memset(pCstr->ParaCstr, 0x00, (sizeof(OPCSTR_PARAENTRY) * OP_CSTR_PARA_SIZE));

    if (ausIndexTable[pCstr->usCstrNo - 1].usCstrLength != 0) {

        /* Read parameter Table in Control String File */
        Op_ReadCstrFile((ULONG)(sizeof(CstrFileHed) +
                                (sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE) +
                                ausIndexTable[pCstr->usCstrNo - 1].usCstrOffset),
                                (UCHAR *)pCstr->ParaCstr,
                                ausIndexTable[pCstr->usCstrNo - 1].usCstrLength);
    }

    Op_CloseCstrFileReleaseSem();
    return (OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpMajorDeptRead(MDEPTIF *pDpi, USHORT usLockHnd)
*
*       Input:    pDpi -> usMajorDeptNo
*                 usLockHnd
*      Output:    pDpi -> DeptPara
*       InOut:    pDpi -> ulOffset
*
**  Return    :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_NOT_IN_FILE
*                 OP_FILE_NOT_FOUND
*                 OP_LOCK
*
**  Description:  Read a Department paramater by Major Dept. 2172
*==========================================================================
*/
SHORT   OpMajorDeptRead(MDEPTIF *pMDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    DEPTIF  Dpi;
    SHORT         sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN DEPT FILE */
    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    /*  GET FILE HEADDER */
    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    do {
        sStatus = Op_GetDept(&DeptFileHed, pMDpi->ulCounter++, &Dpi);
        if (sStatus == OP_EOF) {
            Op_CloseDeptFileReleaseSem();
            return (OP_EOF);
        }
		// if the major department number is 0xff then we are iterating through all of the
		// departments rather than iterating though only the departments of a single major department.
		if (pMDpi->uchMajorDeptNo == 0xff) break;  // special major department number
    } while (pMDpi -> uchMajorDeptNo != Dpi.ParaDept.uchMdept);

    pMDpi->usDeptNo = Dpi.usDeptNo;
	pMDpi->ParaDept = Dpi.ParaDept;

    Op_CloseDeptFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd)
*
*       Input:  pDpi -> culCounter
*               usLockHnd
*      Output:  pDpi -> usDeptNo
*               pDpi -> ParaDept
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_FILE_NOT_FOUND
*                 OP_LOCK
*
**  Description: Read All Dept .  2172
*==========================================================================
*/
SHORT   OpDeptAllRead(DEPTIF *pDpi, USHORT usLockHnd)
{
    OPDEPT_FILEHED DeptFileHed;
    SHORT         sStatus;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    if ((Op_DeptOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadDeptFile(OP_FILE_HEAD_POSITION, &DeptFileHed, sizeof(DeptFileHed));

    sStatus = Op_GetDept(&DeptFileHed, pDpi->ulCounter++, pDpi);
    Op_CloseDeptFileReleaseSem();
    return (sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAllRead(PLUIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif -> culCounter
*               usLockHnd
*      Output:  pPif -> auchPluNo
*               pPif -> uchPluAdj
*               pPif -> ParaPlu
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_PLU_ERROR
*                 OP_LOCKE
*                 OP_FILE_NOT_FOUND
*
*
**  Description: Read All Plu .
*==========================================================================
*/
SHORT   OpPluAllRead(PLUIF *pPif, USHORT usLockHnd)
{
    USHORT usResult;
    MFINFO mfinfo;
    USHORT husHandle;
    USHORT usRecLen;
    RECPLU recplu;
    PRECPLU pRecPlu;
    ECMRANGE ecmrange;
    PECMRANGE pEcmRange;
    ITEMNO itemno;
    PITEMNO pItemNumber;
    ULONG ulCount = 0;

    pEcmRange = &ecmrange;
    pRecPlu = &recplu;
    pItemNumber = &itemno;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

	// On first access pPif->ulCounter will be 0, and this if statement will be entered
	if(!pPif->ulCounter)
	{	
		tcharnset(pEcmRange->itemFrom.aucPluNumber, _T('0'), PLU_NUMBER_LEN);
		tcharnset(pEcmRange->itemTo.aucPluNumber, _T('9'), PLU_NUMBER_LEN);
		pEcmRange->fsOption = REPORT_ACTIVE | REPORT_INACTIVE;
		pEcmRange->usReserve = 0;
		
		usResult = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &husHandle, pEcmRange);
		if(usResult)
		{
			PifReleaseSem(husOpSem);
			return OpConvertPluError(usResult);
		}
		
		pPif->husHandle = husHandle;

	}

    /* read adjective price, before PluReportRecord() */

    if (pPif->usAdjCo)
    {
        _tcsncpy(pItemNumber->aucPluNumber,pPif->auchPluNo,PLU_NUMBER_LEN);
        pPif->usAdjCo++;
        usResult = PluReadRecord(FILE_PLU, pItemNumber, 1, (VOID*)pRecPlu, &usRecLen);
        if(usResult)
        {
            PluExitCritMode(pPif->husHandle,0);
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }
        else
        {
			OpAssignRecPluToPifPlu (pPif, pRecPlu);
            pPif->uchPluAdj = (UCHAR)pPif->usAdjCo;
			RflConv4bto3b(pPif->ParaPlu.auchPrice, pRecPlu->ulUnitPrice[pPif->uchPluAdj-1]);
        }

        if (pPif->usAdjCo >= MAX_ADJECTIVE_NO)
        {
            pPif->usAdjCo = 0;
        }
    }
    else
    {

        usResult = PluReportRecord(pPif->husHandle, pRecPlu, &usRecLen);

        if((usResult == SPLU_REC_LOCKED) || (usResult == SPLU_FUNC_CONTINUED)
                    || (usResult == SPLU_COMPLETED))
        {
            pPif->ulCounter++;
			OpAssignRecPluToPifPlu (pPif, pRecPlu);
            pPif->uchPluAdj = 1; /* adjectve is always 1 on PLU record */
			RflConv4bto3b(pPif->ParaPlu.auchPrice, pRecPlu->ulUnitPrice[pPif->uchPluAdj-1]);
            
			if ((pRecPlu->uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP)
            {
                pPif->usAdjCo++;
            }
            else
            {
                pPif->usAdjCo = 0;
            }
            usResult = SPLU_COMPLETED;
        }
    }

	if((usResult == SPLU_END_OF_FILE) || (usResult))
    {
		PluExitCritMode(pPif->husHandle,0);
	}

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd)
*
*       Input:  Prp -> uchReport     - report code to match against when searching PLUs
*               Prp -> culCounter    - progress counter, should be set to zero to start.
*               usLockHnd
*      Output:  Prp -> auchPluNo
*               Prp -> uchPluAdj
*               Prp -> ParaPlu
*               Prp -> culCounter
*
*       InOut:  Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*                  OP_EOF
*   Abnormal End:  OP_PLU_ERROR
*                  OP_LOCKE
*                  OP_FILE_NOT_FOUND
*
**  Description: Read a Plu by Report No.
*
*                This function is used to iterate over the PLU database returning
*                a series of PLUs that match the report number specified in the
*                PLUIF_REP member uchReport.
*
*                The member variable culCounter is a counter that is used as
*                part of the iteration to control the progress of the iteration.
*
*                A value of zero for culCounter indicates the start of the iteration.
*==========================================================================
*/
SHORT   OpRprtPluRead(PLUIF_REP *pPrp, USHORT usLockHnd)
{
    USHORT usResult;
    static BOOL bCnt = FALSE;
    static BOOL bFirstAccess = FALSE;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    /* first access */
    if(pPrp->culCounter && bCnt)
        bFirstAccess = TRUE;

    if(pPrp->culCounter == 0 || bFirstAccess)
    {
		ECMRANGE  ecmrange = {0};
		USHORT    husHandle;

        if(pPrp->culCounter == 0)
            bCnt = TRUE;

        tcharnset(ecmrange.itemFrom.aucPluNumber, _T('0'), PLU_NUMBER_LEN);
        tcharnset(ecmrange.itemTo.aucPluNumber, _T('9'), PLU_NUMBER_LEN);
        ecmrange.fsOption = REPORT_ACTIVE | REPORT_INACTIVE;

        usResult = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &husHandle, &ecmrange);
        if(usResult)
		{
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }

        pPrp->husHandle = husHandle;

        if(pPrp->culCounter == 0)
            pPrp->usAdjCo = 0;
    }

    /* read adjective price,before PluReportRecord() */
    if (pPrp->usAdjCo)
    {
		RECPLU  recplu = {0};
		USHORT  usRecLen;
		ITEMNO  itemno = {0};

        _tcsncpy(itemno.aucPluNumber, pPrp->auchPluNo, PLU_NUMBER_LEN);
        pPrp->usAdjCo++;
		/* adjective is always 1 on PLU record */
        usResult = PluReadRecord(FILE_PLU, &itemno, 1, &recplu, &usRecLen);
        if(usResult)
        {
            PluExitCritMode(pPrp->husHandle,0);
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }

        OpAssignRecPluToPifRepPlu (pPrp, &recplu);
        pPrp->uchPluAdj = (UCHAR)pPrp->usAdjCo;
		RflConv4bto3b(pPrp->ParaPlu.auchPrice, recplu.ulUnitPrice[pPrp->uchPluAdj-1]);

        if (pPrp->usAdjCo >= MAX_ADJECTIVE_NO)
        {
            pPrp->usAdjCo = 0;
        }
    }
    else
    {
		ULONG ulCount = 0;

        /* read each record sequentially */
        while(1)
        {
			RECPLU recplu = {0};
			USHORT usRecLen;

            usResult = PluReportRecord(pPrp->husHandle, &recplu, &usRecLen);
            if(bFirstAccess)
            {
                if(pPrp->culCounter != ulCount)
                {
                    ulCount++;
                    continue;
                }
                bFirstAccess = FALSE;
                bCnt = FALSE;
                ulCount = 0;
            }

			if((usResult == SPLU_REC_LOCKED) || (usResult == SPLU_FUNC_CONTINUED)
                    || (usResult == SPLU_COMPLETED))
            {
                pPrp->culCounter++;

                if(recplu.uchReportCode == pPrp->uchReport)
                {
					OpAssignRecPluToPifRepPlu (pPrp, &recplu);
                    pPrp->uchPluAdj                   = 1;/* adjective is always 1 on PLU record */
					RflConv4bto3b(pPrp->ParaPlu.auchPrice, recplu.ulUnitPrice[pPrp->uchPluAdj-1]);

                    if ((recplu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP)
                    {
                        pPrp->usAdjCo++;
                    }
                    else
                    {
                        pPrp->usAdjCo = 0;
                    }

                    usResult = SPLU_COMPLETED;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    if(usResult == SPLU_END_OF_FILE || usResult || bCnt)
    {
        PluExitCritMode(pPrp->husHandle, 0);
    }

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpDeptPluRead(PLUIF_DEP *pDp, USHORT usLockHnd)
*
*       Input:  pDp -> usDept         - department code to match against when searching PLUs
*               pDp -> culCounter     - progress counter, should be set to zero to start.
*               usLockHnd
*      Output:  pDp -> auchPluNo
*               pDp -> uchPluAdj
*               pDp -> ParaPlu
*               pDp -> culCounter
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_PLU_ERROR
*                 OP_FILE_NOT_FOUND
*                 OP_LOCK
*
*
**  Description: Read a Plu by Department No.                   Saratoga
*
*                This function is used to iterate over the PLU database returning
*                a series of PLUs that match the department number specified in the
*                PLUIF_DEP member usDept.
*
*                The member variable culCounter is a counter that is used as
*                part of the iteration to control the progress of the iteration.
*
*                A value of zero for culCounter indicates the start of the iteration.
*==========================================================================
*/
SHORT   OpDeptPluRead(PLUIF_DEP *pDp, USHORT usLockHnd)
{
    USHORT usResult;
	USHORT usLockSingleUse = (usLockHnd & 0xf000);
    ULONG ulCount = 0;

	usLockHnd &= 0x0fff;   // we are using the high nibble of lock indicator as exit crit mode flag.

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    if (pDp->culCounter == 0)
    {
		ECMRANGE  ecmrange = {0};
		USHORT    husHandle = 0;

        tcharnset(ecmrange.itemFrom.aucPluNumber, _T('0'), PLU_NUMBER_LEN);
        tcharnset(ecmrange.itemTo.aucPluNumber, _T('9'), PLU_NUMBER_LEN);
        ecmrange.fsOption = REPORT_ACTIVE | REPORT_INACTIVE;

        usResult = PluEnterCritMode(FILE_PLU, FUNC_REPORT_RANGE, &husHandle, &ecmrange);
        if(usResult)
        {
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }

        pDp->husHandle = husHandle;

        if(pDp->culCounter == 0)
            pDp->usAdjCo = 0;
	} else {
		usLockSingleUse = 0;  // single use only allowed if culCount is zero indicating first time in.
	}

    /* read adjective price,before PluReportRecord() */
    if (pDp->usAdjCo)
    {
		RECPLU  recplu = {0};
		ITEMNO  itemno = {0};
		USHORT  usRecLen;

        _tcsncpy(itemno.aucPluNumber, pDp->auchPluNo, PLU_NUMBER_LEN);
        pDp->usAdjCo++;
		/* adjective is always 1 on PLU record */
        usResult = PluReadRecord(FILE_PLU, &itemno, 1, &recplu, &usRecLen);
        if(usResult)
        {
            PluExitCritMode(pDp->husHandle, 0);
            PifReleaseSem(husOpSem);
            return OpConvertPluError(usResult);
        }

        OpAssignRecPluToPifDepPlu (pDp, &recplu);
        pDp->uchPluAdj = (UCHAR)pDp->usAdjCo;
		RflConv4bto3b(pDp->ParaPlu.auchPrice, recplu.ulUnitPrice[pDp->uchPluAdj-1]);

        if (pDp->usAdjCo >= MAX_ADJECTIVE_NO)
        {
            pDp->usAdjCo = 0;
        }
    }
    else
    {
        /* read each record sequentially */
        while(1)
        {
			RECPLU  recplu = {0};
			USHORT  usRecLen;

            usResult = PluReportRecord(pDp->husHandle, &recplu, &usRecLen);
            if((usResult == SPLU_REC_LOCKED) || (usResult == SPLU_FUNC_CONTINUED)
                    || (usResult == SPLU_COMPLETED))
            {
                pDp->culCounter++;

                if(recplu.usDeptNumber == pDp->usDept)
                {
                    OpAssignRecPluToPifDepPlu (pDp, &recplu);
                    pDp->uchPluAdj                   = 1;/* adjective is always 1 on PLU record */
					RflConv4bto3b(pDp->ParaPlu.auchPrice, recplu.ulUnitPrice[pDp->uchPluAdj - 1]);

                    if ((recplu.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP)
                    {
                        pDp->usAdjCo++;
                    }
                    else
                    {
                        pDp->usAdjCo = 0;
                    }

                    usResult = SPLU_COMPLETED;
                    break;
                }
            }
            else
            {
               break;
            }
        }
    }

    if(usResult == SPLU_END_OF_FILE || usResult || usLockSingleUse)
    {
        PluExitCritMode(pDp->husHandle,0);
    }

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpOepPluRead(PLUIF_OEP *pDp, USHORT usLockHnd, UCHAR ucNumOfPLUrequested)
*
*       Input:  pDp -> uchGroupNo
*               pDp -> culCounter ... At First Time Zero
*               usLockHnd
*				ucNumOfPLUrequested  ... holds number of items requested - CSMALL
*      Output:  pDp -> Plu20[20]
*               pDp -> culCounter
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*                 OP_EOF
*   Abnormal End: OP_PLU_ERROR
*                 OP_LOCK
*                 OP_FILE_NOT_FOUND
*                 OP_NOT_IN_FILE
*
**  Description: Read 20 Plus by Order Entry Prompt Group Number,    R3.0
*==========================================================================
*/
SHORT   OpOepPluReadMultiTable(USHORT usTableNo, USHORT usFilePart, PLUIF_OEP *pDp, USHORT usLockHnd, UCHAR ucNumOfPLUrequested)
{
    UCHAR   i;

    USHORT usResult;
    OPOEP_FILEHED OepFileHed;
    OPOEP_INDEXENTRY OepIndex;
    ITEMNO itemno;
    PITEMNO pItemNumber;
    RECPLU recplu;
    PRECPLU pRecPlu;
    /* USHORT usRecPluLen; */
    ULONG  offulSearchPoint;
    TCHAR  auchDummy[OP_PLU_OEP_LEN];
    SHORT  sReturn;

    offulSearchPoint = 0;
    pItemNumber = &itemno;
    pRecPlu = &recplu;
    tcharnset(auchDummy, _T('0'), OP_PLU_OEP_LEN);

    PifRequestSem(husOpSem);

#if 0
    /*
		LOCK CHECK
		This lock check has been disabled to allow the use of
		PLU lookups from an AC63 in Supervisor Mode.  By disabling
		this lock check we can use the Dynamic PLU button logic
		in Supervisor Mode to look up a PLU that is in an OEP
		Group.  - Richard Chambers, 11/03/2006 for Rel 2.0.4
	*/
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }
#endif

    if ((Op_OepOpenFile(usTableNo)) == PIF_ERROR_FILE_NOT_FOUND)  /* pif_error_file_exist */
    {
        PifReleaseSem(husOpSem);
        return OP_FILE_NOT_FOUND;
    }

    Op_ReadOepFile(OP_FILE_HEAD_POSITION, &OepFileHed, sizeof(OepFileHed));
    sReturn = Op_OepIndex(&OepFileHed, usFilePart, OP_SEARCH_ONLY, pDp->usGroupNo, auchDummy, 1, &offulSearchPoint);
    if ((sReturn != OP_PERFORM) && (sReturn != OP_NOT_IN_FILE)) {
        Op_CloseOepFileReleaseSem();
        return sReturn;
    }

    if (pDp->culCounter) {
        offulSearchPoint += (sizeof(OepIndex) * pDp->culCounter);
    }

    /*	set to ucNumOfPLUrequested to loop for less/more
		times depending on how many buttons are to be
		displayed. - CSMALL
	 */
    for(  i=0 ; i < ucNumOfPLUrequested ; i++ ) {
        if (offulSearchPoint < OepFileHed.ulOepFileSize) {
            Op_ReadOepFile(offulSearchPoint, &OepIndex, sizeof(OepIndex));

            if(pDp->usGroupNo != OepIndex.usGroupNo)
            {
                usResult = SPLU_COMPLETED;
                break;
            }

            pDp->culCounter++;

            _tcsncpy(pDp->PluPara20[i].auchPluNo, OepIndex.auchOepPluNo + 3, PLU_NUMBER_LEN);  // copy the PLU number ignoring the three digit priority
            pDp -> PluPara20[i].uchPluAdj = OepIndex.uchAdjective;
            usResult = SPLU_COMPLETED;
            offulSearchPoint += sizeof(OepIndex);
        } else {
            /* reach to EOF of PARAMOEP */
            usResult = SPLU_COMPLETED;
            break;
        }
    }

    Op_CloseOepFileReleaseSem();

    return OpConvertPluError(usResult);
}

SHORT   OpOepPluRead(PLUIF_OEP *pDp, USHORT usLockHnd, UCHAR ucNumOfPLUrequested)
{
	return OpOepPluReadMultiTable(0, 0, pDp, usLockHnd, ucNumOfPLUrequested);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluAbort(USHORT husHandle, USHORT usLockHnd)
*
*       Input:  husHandle
*               usLockHnd
*      Output:  pPif -> auchPluNo
*               pPif -> uchPluAdj
*               pPif -> ParaPlu
*       InOut:  Nothing
*
**  Return    :
*     Normal End: OP_PERFORM
*
*   Abnormal End: OP_LOCKED
*
*
**  Description: Exit Critical Mode after Abort Process, saratoga
*==========================================================================
*/
SHORT   OpPluAbort(USHORT husHandle, USHORT usLockHnd)
{
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    }

    if (husHandle) {
        PluExitCritMode(husHandle,0);
    }

    PifReleaseSem(husOpSem);

    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  value of lock handle
*   Abnormal End:  OP_LOCK
*
**  Description: Get Lonk handle and Lock
*                   (All function in Thie module is locked)
*==========================================================================
*/
SHORT   OpLock(VOID)
{
    SHORT hmyLockHandle = OP_LOCK;

    PifRequestSem(husOpSem);
    if (husOpLockHandle == 0x00){
		hmyLockHandle = husOpLockHandle = 0x1F;
    }

    PifReleaseSem(husOpSem);
    return (hmyLockHandle);
}
/*
*==========================================================================
**    Synopsis:    VOID   OpUnLock(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:  UnLock (All function in Thie module is locked)
*==========================================================================
*/
VOID   OpUnLock(VOID)
{
    PifRequestSem(husOpSem);
    husOpLockHandle = 0x00;
    PifReleaseSem(husOpSem);
}

/** ---------------------------------------------------------------------
	The following functions are used as part of blocking and unblocking
	operator sign-in.  These functions are used as part of the Operator
	Message functionality to provide a mechanism so that a remote PC
	can send an Operator Message with operational flags set so as to
	allow or dis-allow an operator from doing a sign-in.
*/

static SHORT  sOpSignInBlockVariable = 0;

SHORT OpSignInBlockOn (VOID)
{
	SHORT  sOpSignInStatus = 0;

    PifRequestSem(husOpSem);
	sOpSignInStatus = sOpSignInBlockVariable;
    sOpSignInBlockVariable = 1;
    PifReleaseSem(husOpSem);

	return sOpSignInStatus;
}

SHORT OpSignInBlockOff (VOID)
{
	SHORT  sOpSignInStatus = 0;

    PifRequestSem(husOpSem);
	sOpSignInStatus = sOpSignInBlockVariable;
    sOpSignInBlockVariable = 0;
    PifReleaseSem(husOpSem);

	return sOpSignInStatus;
}

SHORT OpSignInStatus (VOID)
{
	SHORT  sOpSignInStatus = 0;

    PifRequestSem(husOpSem);
	sOpSignInStatus = sOpSignInBlockVariable;
    PifReleaseSem(husOpSem);

	return sOpSignInStatus;
}
//-----------------------------------------------------------

/*
*==========================================================================
**    Synopsis:    BOOL   OpIsUnLockedPluFile(VOID)
*
*       Input:    Nothing
*      Output:    TRUE if not locked or FALSE otherwise
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:  Check to see if the PLU file is locked or not.
*==========================================================================
*/
BOOL OpIsUnLockedPluFile (VOID)
{
	BOOL  bRetVal = 0;

    PifRequestSem(husOpSem);
    bRetVal = (husOpLockHandle == 0x00);
    PifReleaseSem(husOpSem);

	return bRetVal;
}

/*
*==========================================================================
**    Synopsis:    VOID   OpReqPluOep(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request Paramater for OEP tables. (for broadcast)
*==========================================================================
*/
SHORT   OpReqPluOep(USHORT usFcode, USHORT usLockHnd)
{
    SHORT   sStatus;

    /* Add 2172 Rel1.0, Saratoga */
    sStatus = OpReqOep(usFcode, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqOep() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

    BlFwIfReloadOEPGroup ();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    VOID   OpReqSup(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request Paramater for supervizer mode. (for broadcast)
*==========================================================================
*/
SHORT   OpReqSup(USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((sStatus = OpReqSupPara(usLockHnd)) < 0) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqSupPara() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

    if (pPifSaveFarData != NULL) {               /* saratoga */
        (*pPifSaveFarData)();
    }

    ApplSetupDevice(APPL_INIT_ALL);

    sStatus = OpReqDept(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqDept() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqCpn(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqCpn() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqCstr(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqCstr() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    /* === Add PPI File (Release 3.1) BEGIN === */
    sStatus = OpReqPpi(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqPpi() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }
    /* === Add PPI File (Release 3.1) BEGIN === */

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    /* Add 2172 Rel1.0, Saratoga */
    sStatus = OpReqOep(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqOep() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

    BlFwIfReloadOEPGroup ();
    return (OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    VOID   OpReqSupPara(USHORT usLockHnd)
*
*       Input:    USHORT    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request Paramater for supervizer mode. (for broadcast)
*==========================================================================
*/
SHORT   OpReqSupPara(USHORT usLockHnd)
{
    OP_BACKUP   Op_Backup, *pOp_BackupRcv;
    USHORT      usRcvLen, usReturnLen;
    UCHAR       auchRcvBuf[OP_BROADCAST_SIZE];
    SHORT       sStatus;
    USHORT      usRcvBufPnt;

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return (OP_LOCK);
    }
    Op_Backup.uchClass = OP_PARA_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if  ((sStatus = CstReqOpePara(CLI_FCOPREQSUP, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
			char  xBuff[128];
			sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
			sprintf (xBuff, "OpReqAllPara(): sStatus %d, usSentSeqNo 0x%x", sStatus, Op_Backup.usSeqNo);
			NHPOS_ASSERT_TEXT((sStatus >= 0), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_FAILED);
			PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
            return (sStatus);
            break;
        }
        pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;
        usRcvBufPnt = sizeof(OP_BACKUP);
        usRcvLen   -= sizeof(OP_BACKUP);

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqSupPara(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqSupPara(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_PARA_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            sStatus = OP_COMM_ERROR;
            break;
        }

        for ( ; usRcvLen > 0; ) {
            if (auchClassNoSup[Op_Backup.uchClass] == OP_EOT) {
                break;
            }
            CliParaAllWrite(auchClassNoSup[Op_Backup.uchClass++], &auchRcvBuf[usRcvBufPnt], usRcvLen,
                            (USHORT)Op_Backup.offulFilePosition, &usReturnLen);
            Op_Backup.offulFilePosition = 0L;
            usRcvLen -= usReturnLen;
            usRcvBufPnt += usReturnLen;
        }

		//we now check to see if there is an op_end status, which means it is at the end of a class, and
		//must read the next. we also make sure that we are at the end of the table.
        if ((pOp_BackupRcv->uchStatus == OP_END) && (auchClassNoSup[pOp_BackupRcv->uchClass] == OP_EOT)) {
            break;
        }
        Op_Backup.uchClass = pOp_BackupRcv->uchClass;
        Op_Backup.offulFilePosition = pOp_BackupRcv->offulFilePosition;
        Op_Backup.usSeqNo++;
    }
    return (sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpResSup(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description:  Response Superviser paramater. (for broadcast)
*==========================================================================
*/
SHORT   OpResSup(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
    OP_BACKUP   *pOp_BackUpRcv, *pOp_BackUpSnd;
    USHORT      usReturnLen, usSndLenBack, usSndLenWork;
	SHORT       sRetStatus;

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;

    /* Add 2172 Rel1.0 */
    if (pOp_BackUpRcv->uchClass == OP_OEP_FILE) {    // OpResSup():  Send response to request for OEP file
        sRetStatus = OpResOep (puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResOep(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    if (pOp_BackUpRcv->uchClass == OP_DEPT_FILE) {    // OpResSup():  Send response to request for Department file
        sRetStatus = OpResDept(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResDept(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    if (pOp_BackUpRcv->uchClass == OP_CPN_FILE) {    // OpResSup():  Send response to request for Coupon file
        sRetStatus = OpResCpn(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResCpn(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    if (pOp_BackUpRcv->uchClass == OP_CSTR_FILE) {    // OpResSup():  Send response to request for Control String file
        sRetStatus = OpResCstr(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResCstr(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* === Add PPI File (Release 3.1) BEGIN === */
    if (pOp_BackUpRcv->uchClass == OP_PPI_FILE) {    // OpResSup():  Send response to request for PPI file
        sRetStatus = OpResPpi(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResPpi(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }
    /* === Add PPI File (Release 3.1) END === */

    if (pOp_BackUpRcv->uchClass == OP_MLD_FILE) {    // OpResSup():  Send response to request for MLD file
        sRetStatus = OpResMld(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResMld(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* Hosts address boradcast, V1.0.04 */
    if (pOp_BackUpRcv->uchClass == OP_HOSTS_FILE) {    // OpResSup():  Send response to request for Hosts file
        sRetStatus = OpResHostsIP(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResHostsIP(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* Biometrics file address boradcast, V1.0.04 */
    if (pOp_BackUpRcv->uchClass == OP_BIOMETRICS_FILE) {    // OpResSup():  Send response to request for Biometrics file
        sRetStatus = OpResBiometrics(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResBiometrics(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* Reason Code file address broadcast, V1.0.04 */
    if (pOp_BackUpRcv->uchClass == OP_REASONCODES_FILE) {    // OpResSup():  Send response to request for Biometrics file
        sRetStatus = OpResReasonCodes(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResReasonCodes(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }
	// The file transfer request is not for one of the standard files so it is
	// for the Para memory resident data.  First record indicated by OP_PARA_FILE.
    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    if (usRcvLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
        return (OP_COMM_ERROR);
    }
    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;
    if (OP_BROADCAST_SIZE > *pusSndLen) {
        usSndLenBack = *pusSndLen;
    } else {
        usSndLenBack = OP_BROADCAST_SIZE;
    }
    usSndLenWork = usSndLenBack - sizeof(OP_BACKUP);
    puchSndData += sizeof(OP_BACKUP);
    pOp_BackUpSnd->uchClass = pOp_BackUpRcv->uchClass;
    pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    pOp_BackUpSnd->usSeqNo = pOp_BackUpRcv->usSeqNo;
    for ( ; usSndLenWork > 0; ) {
        if (auchClassNoSup[pOp_BackUpSnd->uchClass] == OP_EOT) {
            pOp_BackUpSnd->uchStatus = OP_END;
            break;
        }
        if (CliParaAllRead( auchClassNoSup[pOp_BackUpSnd->uchClass], puchSndData, usSndLenWork, (USHORT)pOp_BackUpSnd->offulFilePosition, &usReturnLen) != 00) {
            pOp_BackUpSnd->offulFilePosition += usReturnLen;
            pOp_BackUpSnd->uchStatus = OP_CONTINUE;
        } else {
            pOp_BackUpSnd->offulFilePosition = 0L;
            pOp_BackUpSnd->uchStatus = OP_CONTINUE;
            pOp_BackUpSnd->uchClass++;
        }
        puchSndData += usReturnLen;
        usSndLenWork -= usReturnLen;
    }
    *pusSndLen = usSndLenBack - usSndLenWork;
    PifReleaseSem(husOpSem);
    return (OP_PERFORM);
}

/*
*===========================================================================
**    Synopsis:    VOID   OpReqAll(USHORT usLockHnd)
*
*       Input:    USHORT    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request All Paramater . (for broadcast)
*==========================================================================
*/
SHORT   OpReqAll(USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((sStatus = OpReqAllPara(usLockHnd)) < 0) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqAllPara() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

    if (pPifSaveFarData != NULL) {               /* saratoga */
        (*pPifSaveFarData)();
    }

    Op_AllFileCreate();

    /* Hosts address boradcast, V1.0.04 */
    sStatus = OpReqHostsIP(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqHostsIP() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
//        return (sStatus);   Removed to allow transfer to continue even if no Hosts file
    }

    ApplSetupDevice(APPL_INIT_ALL);

    /* Add 2172 Rel1.0, Saratoga */
    sStatus = OpReqOep(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqOep() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
//  Commented return out so that if there is an error with loading the OEP files
//  we will continue on anyway rather than aborting.  At least this way we will
//  have a working system even if not all OEP files transferred.  This data is
//  used for dynamic PLU buttons and initial check on whether a PLU exists.
//  With Amtrak we have increased the number of these files and not all may
//  may transfer over.  AC75 after doing a Master Terminal restart should
//  make sure that all of the files are created and are transferred.
//        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqDept(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqDept() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqCpn(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqCpn() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqCstr(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqCstr() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    /* === Add PPI File (Release 3.1) BEGIN === */
    sStatus = OpReqPpi(CLI_FCOPREQSUP, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqPpi() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }
    /* === Add PPI File (Release 3.1) END === */

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqMld(CLI_FCOPREQSUP, usLockHnd);  /* V3.3 */
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqMld() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqBiometrics(CLI_FCOPREQSUP, usLockHnd);  /* V3.3 */
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqBiometrics() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	PifSleep(50);    // Give another terminal the opportunity to do a file download

    sStatus = OpReqReasonCodes(CLI_FCOPREQSUP, usLockHnd);  /* V3.3 */
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		{
			char    xBuff[128];
			sprintf (xBuff, "OpReqReasonCodes() failed, sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == 0), xBuff)
		}
        return (sStatus);
    }

	BlFwIfReloadOEPGroup ();

    return (OP_PERFORM);
}

/*
*===========================================================================
**    Synopsis:    VOID   OpReqAllPara(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request All Paramater . (for broadcast)
*==========================================================================
*/
SHORT   OpReqAllPara(USHORT usLockHnd)
{
    OP_BACKUP   Op_Backup, *pOp_BackupRcv;
    USHORT      usRcvLen, usReturnLen;
    UCHAR       auchRcvBuf[OP_BROADCAST_SIZE];
    SHORT       sStatus;
    USHORT      usRcvBufPnt;

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return(OP_LOCK);
    }

    Op_Backup.uchClass = OP_PARA_FILE;
    Op_Backup.offulFilePosition = 0L;
    Op_Backup.usSeqNo = 1;

    for (;;) {
        usRcvLen = OP_BROADCAST_SIZE;
        if  ((sStatus = CstReqOpePara(CLI_FCOPREQALL, (UCHAR *)&Op_Backup, sizeof(OP_BACKUP), auchRcvBuf, &usRcvLen)) < 0 ) {
			char  xBuff[128];
			sStatus += STUB_RETCODE;     // move the STUB_ type of error code into the OP_ error code range for use by UicDisplayErrorMsg()
			sprintf (xBuff, "OpReqAllPara(): sStatus %d, usSentSeqNo 0x%x", sStatus, Op_Backup.usSeqNo);
			NHPOS_ASSERT_TEXT((sStatus >= 0), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_FAILED);
			PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
            return (sStatus);
        }
        pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;
        usRcvBufPnt = sizeof(OP_BACKUP);
        usRcvLen   -= sizeof(OP_BACKUP);

        if (pOp_BackupRcv->usSeqNo != Op_Backup.usSeqNo) {
			char  xBuff[128];
			if (Op_Backup.usSeqNo == pOp_BackupRcv->usSeqNo + 1) {
				sprintf (xBuff, "OpReqAllPara(): OP_COMM_IGNORE usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
				NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
				continue;
			}
			sprintf (xBuff, "OpReqAllPara(): OP_COMM_ERROR usSentSeqNo 0x%x, usRecvSeqNo 0x%x", Op_Backup.usSeqNo, pOp_BackupRcv->usSeqNo);
			NHPOS_ASSERT_TEXT((pOp_BackupRcv->usSeqNo == Op_Backup.usSeqNo), xBuff);
			PifLog(MODULE_OP_BACKUP, LOG_EVENT_OP_REQPARA_RQST_SEQNO);
			PifLog (MODULE_DATA_VALUE(MODULE_OP_BACKUP), OP_PARA_FILE);
			PifLog (MODULE_LINE_NO(MODULE_OP_BACKUP), (USHORT)__LINE__);
            return(OP_COMM_ERROR);
        }

        for ( ; usRcvLen > 0; ) {
            if (auchClassNoAll[Op_Backup.uchClass] == OP_EOT) {
                break;
            }

            CliParaAllWrite(auchClassNoAll[Op_Backup.uchClass++], &auchRcvBuf[usRcvBufPnt],
                            usRcvLen, (USHORT)Op_Backup.offulFilePosition, &usReturnLen);
            Op_Backup.offulFilePosition = 0L;
            usRcvLen -= usReturnLen;
            usRcvBufPnt += usReturnLen;
        }

		//we now check to see if there is an op_end status, which means it is at the end of a class, and
		//must read the next. we also make sure that we are at the end of the table.
        if ((pOp_BackupRcv->uchStatus == OP_END) && (auchClassNoAll[pOp_BackupRcv->uchClass] == OP_EOT)) {
            break;
        }
        Op_Backup.uchClass = pOp_BackupRcv->uchClass;
        Op_Backup.offulFilePosition = pOp_BackupRcv->offulFilePosition;
        Op_Backup.usSeqNo++;
    }
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpResAll(UCHAR   *puchRcvData,
*                                   USHORT  usRcvLen,
*                                   UCHAR   *puchSndData,
*                                   UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR;
*
**  Description: Response All paramater. (for broadcast)
*==========================================================================
*/
SHORT   OpResAll(UCHAR  *puchRcvData,
                 USHORT  usRcvLen,
                 UCHAR  *puchSndData,
                 USHORT *pusSndLen,
                 USHORT  usLockHnd)
{
    OP_BACKUP   *pOp_BackUpRcv, *pOp_BackUpSnd;
    USHORT      usReturnLen, usSndLenWork, usSndLenBack;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }
    if (usRcvLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
        return (OP_COMM_ERROR);
    }
    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;
    if (OP_BROADCAST_SIZE > *pusSndLen) {
        usSndLenBack = *pusSndLen;
    } else {
        usSndLenBack = OP_BROADCAST_SIZE;
    }
    usSndLenWork = usSndLenBack - sizeof(OP_BACKUP);
    puchSndData += sizeof(OP_BACKUP);
    pOp_BackUpSnd->uchClass = pOp_BackUpRcv->uchClass;
    pOp_BackUpSnd->offulFilePosition = pOp_BackUpRcv->offulFilePosition;
    pOp_BackUpSnd->usSeqNo = pOp_BackUpRcv->usSeqNo;
    for ( ; usSndLenWork > 0; ) {
        if (auchClassNoAll[pOp_BackUpSnd->uchClass] == OP_EOT) {
            pOp_BackUpSnd->uchStatus = OP_END;
            break;
        }
        if (CliParaAllRead( auchClassNoAll[pOp_BackUpSnd->uchClass], puchSndData, usSndLenWork,
                            (USHORT)pOp_BackUpSnd->offulFilePosition, &usReturnLen) != 00) {
            pOp_BackUpSnd->offulFilePosition += usReturnLen;
            pOp_BackUpSnd->uchStatus = OP_CONTINUE;
        } else {
            pOp_BackUpSnd->offulFilePosition = 0L;
            pOp_BackUpSnd->uchStatus = OP_END;
            pOp_BackUpSnd->uchClass++;
        }
        puchSndData += usReturnLen;
        usSndLenWork -= usReturnLen;
    }
    *pusSndLen = usSndLenBack - usSndLenWork;
    PifReleaseSem(husOpSem);
    return (OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    VOID   OpReqBackUp(USHORT usLockHnd)
*
*       Input:    USHORT    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description: Request All Paramater . (for broadcast)
*==========================================================================
*/
SHORT   OpReqBackUp(USHORT usLockHnd)
{
    SHORT   sStatus;

    OpUnLock();       /* Clear lock handle */

    if ((sStatus = OpReqBackUpPara(usLockHnd)) < 0) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_BACKUP);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqBackUpPara(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }

    if (pPifSaveFarData != NULL) {               /* saratoga */
        (*pPifSaveFarData)();
    }

    Op_AllFileCreate();

    /* Hosts address boradcast, V1.0.04 */
/***
    sStatus = OpReqHostsIP(CLI_FCBAKOPPARA, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return(sStatus);
    }
***/
    /* Add 2172 Rel1.0 */
    sStatus = OpReqOep(CLI_FCBAKOPPARA, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_OEP);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqOep(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }
    sStatus = OpReqDept(CLI_FCBAKOPPARA, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_DEPT);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqDept(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }
    sStatus = OpReqCpn(CLI_FCBAKOPPARA, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_CPN);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqCpn(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }
    sStatus = OpReqCstr(CLI_FCBAKOPPARA, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_CSTR);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqCstr(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }
    /* === Add PPI File (Release 3.1) BEGIN === */
    sStatus = OpReqPpi(CLI_FCBAKOPPARA, usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_PPI);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqPpi(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }
    /* === Add PPI File (Release 3.1) END === */

    sStatus = OpReqMld(CLI_FCBAKOPPARA, usLockHnd); /* V3.3 */
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_MLD);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqMld(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }

    sStatus = OpReqBiometrics(CLI_FCBAKOPPARA, usLockHnd); /* V3.3 */
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_MLD);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqBiometrics(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }

    sStatus = OpReqReasonCodes(CLI_FCBAKOPPARA, usLockHnd); /* V3.3 */
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
		PifLog(MODULE_OP_BACKUP, MODULE_OP_MLD);
		PifLog(MODULE_ERROR_NO(MODULE_OP_BACKUP), (USHORT)abs(sStatus));
		{
			char xBuff[128];
			sprintf(xBuff,"OpReqReasonCodes(): sStatus = %d", sStatus);
			NHPOS_ASSERT_TEXT((sStatus == OP_PERFORM), xBuff);
		}
        return (sStatus);
    }

	//	OpReqBiometricsFile(usLockHnd);

    return (OP_PERFORM);
}
/*
*==========================================================================
**    Synopsis:    VOID   OpReqBackUpPara(USHORT usLockHnd)
*
*       Input:    usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:  Request All Paramater . (for broadcast)
*==========================================================================
*/
SHORT   OpReqBackUpPara(USHORT usLockHnd)
{
    OP_BACKUP   Op_Backup, *pOp_BackupRcv;
    USHORT      usRcvLen, usReturnLen;
    UCHAR       auchRcvBuf[OP_BACKUP_WORK_SIZE];
    SHORT       sStatus;

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        return (OP_LOCK);
    }
    Op_Backup.uchClass = CLASS_PARAFLEXMEM;
    Op_Backup.offulFilePosition = 0L;
    pOp_BackupRcv = (OP_BACKUP *)auchRcvBuf;
    usRcvLen = OP_BACKUP_WORK_SIZE;
    if  ((sStatus = SstReqBackUp(CLI_FCBAKOPPARA,
                                (UCHAR *)&Op_Backup,
                                sizeof(OP_BACKUP),
                             auchRcvBuf, &usRcvLen)) < 0 ) {
        return (sStatus);
    }
    if (pOp_BackupRcv->uchClass == CLASS_PARAFLEXMEM) {
        CliParaAllWrite(CLASS_PARAFLEXMEM,
                        &auchRcvBuf[sizeof(OP_BACKUP)],
                        (USHORT)(usRcvLen - sizeof(OP_BACKUP)),
                        (USHORT)Op_Backup.offulFilePosition,
                        &usReturnLen
                        );
        if (pOp_BackupRcv->uchStatus != OP_END) {
            return OP_COMM_ERROR;
        }
    } else {
        return OP_COMM_ERROR;
    }
    return (sStatus);
}
/*
*==========================================================================
**    Synopsis:    SHORT   OpResBackUp( UCHAR   *puchRcvData,
*                                       USHORT  usRcvLen,
*                                       UCHAR   *puchSndData,
*                                       UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   OP_PERFORM
*                 OP_COMM_ERROR
*
**  Description: Response All paramater. (for broadcast)
*==========================================================================
*/
SHORT   OpResBackUp(UCHAR  *puchRcvData,
                    USHORT  usRcvLen,
                    UCHAR  *puchSndData,
                    USHORT *pusSndLen,
                    USHORT  usLockHnd)
{
    OP_BACKUP   *pOp_BackUpRcv, *pOp_BackUpSnd;
    USHORT      usReturnLen, usSndLenBack,usSndLenWork;
	SHORT       sRetStatus;

    pOp_BackUpRcv = (OP_BACKUP *)puchRcvData;

    /* Hosts address boradcast, V1.0.04 */
    if (pOp_BackUpRcv->uchClass == OP_HOSTS_FILE) {
        sRetStatus = OpResHostsIP(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResHostsIP(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* Add 2172 Rel1.0 */
    if (pOp_BackUpRcv->uchClass == OP_OEP_FILE) {
        sRetStatus = OpResOep(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResOep(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    if (pOp_BackUpRcv->uchClass == OP_DEPT_FILE) {
        sRetStatus = OpResDept(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResDept(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }
/*   Del 2172 Rel 1.0
    if (pOp_BackUpRcv->uchClass == OP_PLU_FILE) {
        return (OpResPlu(puchRcvData,
                          usRcvLen,
                          puchSndData,
                          pusSndLen,
                          usLockHnd));
    }
*/
    if (pOp_BackUpRcv->uchClass == OP_CPN_FILE) {
        sRetStatus = OpResCpn(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResCpn(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    if (pOp_BackUpRcv->uchClass == OP_CSTR_FILE) {
        sRetStatus = OpResCstr(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResCstr(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* === Add PPI File (Release 3.1) BEGIN === */
    if (pOp_BackUpRcv->uchClass == OP_PPI_FILE) {
        sRetStatus = OpResPpi(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResPpi(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }
    /* === Add PPI File (Release 3.1) END === */

    if (pOp_BackUpRcv->uchClass == OP_MLD_FILE) {   /* V3.3 */
        sRetStatus = OpResMld(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResMld(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    if (pOp_BackUpRcv->uchClass == OP_BIOMETRICS_FILE) {   /* V3.3 */
        sRetStatus = OpResBiometrics(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResBiometrics(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    /* Reason Code file address broadcast, V1.0.04 */
    if (pOp_BackUpRcv->uchClass == OP_REASONCODES_FILE) {    // OpResSup():  Send response to request for Biometrics file
        sRetStatus = OpResReasonCodes(puchRcvData, usRcvLen, puchSndData, pusSndLen, usLockHnd);
		if ( sRetStatus != OP_PERFORM) {
			char    xBuff[128];
			sprintf (xBuff, "OpResReasonCodes(): sStatus = %d", sRetStatus);
			NHPOS_ASSERT_TEXT((sRetStatus == 0), xBuff)
		}
		return sRetStatus;
    }

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return(OP_LOCK);
    }
    if (usRcvLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
        return (OP_COMM_ERROR);
    }
    if (*pusSndLen < sizeof(OP_BACKUP)) {
        PifReleaseSem(husOpSem);
        return (OP_COMM_ERROR);
    }
    pOp_BackUpSnd = (OP_BACKUP *)puchSndData;
    if (OP_BACKUP_WORK_SIZE > *pusSndLen) {
        usSndLenWork = usSndLenBack = (*pusSndLen - sizeof(OP_BACKUP));
    } else {
        usSndLenWork = usSndLenBack = (OP_BACKUP_WORK_SIZE - sizeof(OP_BACKUP));
    }
    puchSndData += sizeof(OP_BACKUP);
    pOp_BackUpSnd->uchClass = CLASS_PARAFLEXMEM;
    if (CliParaAllRead( CLASS_PARAFLEXMEM, puchSndData, usSndLenWork,
                        (USHORT)pOp_BackUpRcv->offulFilePosition, &usReturnLen) != 00) {
        pOp_BackUpSnd->uchStatus = OP_CONTINUE;
        pOp_BackUpRcv->offulFilePosition = usSndLenWork;
    } else {
        pOp_BackUpSnd->uchStatus = OP_END;
        pOp_BackUpRcv->offulFilePosition = 0L;
    }
    *pusSndLen = usReturnLen + sizeof(OP_BACKUP);
    PifReleaseSem(husOpSem);
    return (OP_PERFORM);
}

/*
*===========================================================================
** Synopsis:    USHORT     OpConvertError(SHORT sError);
*     Input:    sError
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Leadthru No
*
** Description: This function converts error code to leadthru number to
*               be used to select the appropriate error message to present
*               to the operator.
*===========================================================================
*/
#if defined(OpConvertError)
USHORT   OpConvertError_Special(SHORT sError);
USHORT   OpConvertError_Debug(SHORT sError, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "OpConvertError_Debug(): sError = %d, File %s, lineno = %d", sError, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return OpConvertError_Special(sError);
}

USHORT   OpConvertError_Special(SHORT sError)
#else
USHORT   OpConvertError(SHORT sError)
#endif
{
    USHORT  usLeadthruNo;

    switch ( sError ) {
    case  OP_PERFORM :         /* Success    */
        usLeadthruNo = 0;
        break;

    case  OP_FILE_NOT_FOUND :  /* Error   -1  */
        usLeadthruNo = LDT_PROHBT_ADR  ;   /*  10 * Prohibit Operation */
        break;

    case  OP_PLU_FULL :        /* Error   -2  */
        usLeadthruNo = LDT_FLFUL_ADR ;     /*   2 * File Full */
        break;

    case  OP_NOT_IN_FILE :     /* Error   -3  */
    case  OP_ABNORMAL_CSTR  :  /* Error  -9  */
    case  OP_ABNORMAL_CPN:     /* Error -8, Saratoga */
        usLeadthruNo = LDT_NTINFL_ADR ;    /*   1 * Not In File */
        break;

    case  OP_LOCK  :           /* Error   -5  */
        usLeadthruNo = LDT_LOCK_ADR  ;     /*  22 * During Lock */
        break;

    case  OP_NO_MAKE_FILE  :   /* Error   -6  */
        usLeadthruNo = LDT_FLFUL_ADR ;     /*   2 * File Full */
        break;

    case  OP_ABNORMAL_DEPT  :  /* Error  -7  */
        usLeadthruNo = LDT_KEYOVER_ADR   ;     /*  8 * Key Entered Over Limitation */
        break;

    case  OP_NOT_MASTER :      /* Error  -20  */
        usLeadthruNo = LDT_I_AM_NOT_MASTER ;    /*  10 * Prohibit Operation */
        break;

    case  OP_M_DOWN_ERROR :
		usLeadthruNo = LDT_LNKDWN_ADR;
		break;

    case  OP_BM_DOWN_ERROR :
		usLeadthruNo = LDT_SYSBSY_ADR;
		break;

    case  OP_BUSY_ERROR :
		usLeadthruNo = LDT_SYSBSY_ADR;
 		break;

    case  OP_TIME_OUT_ERROR :
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

    case  OP_UNMATCH_TRNO :
		usLeadthruNo = LDT_COMUNI_ADR;   // Transaction number did not match expected number
		break;

    case  OP_DUR_INQUIRY :
		usLeadthruNo = LDT_LNKDWN_ADR;
		break;

	case  OP_COMM_ERROR:
		usLeadthruNo = LDT_REQST_TIMED_OUT;
		break;

    default:
        usLeadthruNo = LDT_ERR_ADR   ;     /*  21 * Error(Global) */
        break;
    }

	if (sError != OP_PERFORM) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sError));
		PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), usLeadthruNo);
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
	}

    return (usLeadthruNo);
}
/*
*===========================================================================
** Synopsis:    SHORT     OpConvertPluError(USHORT usError);
*     Input:    usError
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ErrorNo
*
** Description: This function converts PLU error code to a standard error code
*               that is then used by other functions to decide what to do.
*===========================================================================
*/
#if defined(OpConvertPluError)
USHORT   OpConvertPluError_Special(USHORT usError);
USHORT   OpConvertPluError_Debug(USHORT usError, char *aszFilePath, int nLineNo)
{
	int iLen = 0;
	char  xBuffer[256];

	iLen = strlen (aszFilePath);
	if (iLen > 30) {
		iLen = iLen - 30;
	}
	else {
		iLen = 0;
	}

	sprintf (xBuffer, "OpConvertPluError_Debug(): usError = %d, File %s, lineno = %d", usError, aszFilePath + iLen, nLineNo);
	NHPOS_ASSERT_TEXT(0, xBuffer);
	return OpConvertPluError_Special(usError);
}

USHORT   OpConvertPluError_Special(USHORT usError)
#else
SHORT   OpConvertPluError(USHORT usError)
#endif
{
    SHORT  sErrorNo;

    switch ( usError ) {
		case SPLU_COMPLETED:                 /* Success        */
			sErrorNo = OP_PERFORM;          /* 0              */
			break;

		case SPLU_INVALID_DATA:              /* Error   1002  */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_INVALID_DATA);
			sErrorNo = OP_FILE_NOT_FOUND;   /* Error   -2    */
			break;

		case SPLU_FILE_NOT_FOUND:            /* Error   1005  */
			sErrorNo = OP_FILE_NOT_FOUND;   /* Error   -2    */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_FILE_NOT_FOUND);
			break;

		case SPLU_DEVICE_FULL:               /* Error   1006  */
			sErrorNo = OP_NO_MAKE_FILE;     /* Error   -6    */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_DEVICE_FULL);
			break;

		case SPLU_FILE_OVERFLOW:             /* Error   1007  */
			sErrorNo = OP_PLU_FULL;         /* 2 * File Full */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_FILE_OVERFLOW);
			break;

		case SPLU_REC_LOCKED:                /* Error   1008  */
			sErrorNo = OP_LOCK;             /* Error   -5    */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_REC_LOCKED);
			break;
		
		case SPLU_REC_PROTECTED:             /* Error   1011  */
			sErrorNo = OP_LOCK;             /* Error   -5    */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_REC_PROTECTED);
			break;
		
		case SPLU_FILE_LOCKED:               /* Error   1021  */
			sErrorNo = OP_LOCK;             /* Error   -5    */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_FILE_LOCKED);
			break;

		case SPLU_REC_NOT_FOUND:             /* Error   1010  */
			sErrorNo = OP_NOT_IN_FILE;      /*   -3 * Not In File */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_REC_NOT_FOUND);
			break;

		case SPLU_FUNC_CONTINUED:            /* Error   1015  */
			sErrorNo = OP_CONT;             /* Error   -38   */
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_FUNC_CONTINUED);
			break;

		case SPLU_END_OF_FILE:               /* Error   1016  */
			sErrorNo = OP_EOF;              /* Error   -38   */
//			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_END_OF_FILE);
			break;

		case SPLU_INVALID_HANDLE:            /* Error   1003  */
			sErrorNo = OP_PLU_ERROR;
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_INVALID_HANDLE);
			break;

		case SPLU_TOO_MANY_USERS:            /* Error   1004  */
			sErrorNo = OP_PLU_ERROR;
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_TOO_MANY_USERS);
			break;

		case SPLU_ADJ_VIOLATION:             /* Error   1013  */
			sErrorNo = OP_PLU_ERROR;
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_ADJ_VIOLATION);
			break;

		case SPLU_FUNC_KILLED:               /* Error   1024  */
			sErrorNo = OP_PLU_ERROR;
			PifLog(MODULE_OP_PLU, LOG_EVENT_OP_SPLU_FUNC_KILLED);
			break;

		default:
			sErrorNo = OP_PLU_ERROR;
			break;
    }

	if (usError != SPLU_COMPLETED && usError != SPLU_END_OF_FILE) {
		PifLog (MODULE_OP_PLU, LOG_EVENT_OP_CONVERTERROR);
		PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), usError);
		PifLog (MODULE_ERROR_NO(MODULE_OP_PLU), (USHORT)abs(sErrorNo));
		PifLog (MODULE_LINE_NO(MODULE_OP_PLU), (USHORT)__LINE__);
	}

    return (sErrorNo);
}

/******************** New Functions (Release 3.1) BEGIN *******************/
/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiCreatFile(USHORT usNumberOfPpi,
*                                         USHORT usLockHnd)
*
*       Input:    usNumberOfCpn - Number of Promotional Pricing Item
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create PPI (Promotional Pricing Item) File.
*==========================================================================
*/
SHORT   OpPpiCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd)
{
    OPPPI_FILEHED   PpiFileHed;
    ULONG           ulActualPosition;
    ULONG           ulNewSize;
    ULONG           ulWriteOffset;
    UCHAR           auchPpiFile[ sizeof( OPPPI_PARAENTRY ) ];

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Delete PPI File */
    PifDeleteFile(auchOP_PPI);

    /* Check Number of PPI */
    if (!usNumberOfPpi) {
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    if ((hsOpPpiFileHandle = PifOpenFile(auchOP_PPI, auchNEW_FILE_WRITE)) < 0) {
        PifAbort(MODULE_OP_PPI, FAULT_ERROR_FILE_OPEN);
    }

    /* Check file size and creat file */
    ulNewSize = ( ULONG )( sizeof( OPPPI_FILEHED ) + ( sizeof( OPPPI_PARAENTRY ) * usNumberOfPpi ));
    if (PifSeekFile(hsOpPpiFileHandle, ulNewSize, &ulActualPosition) < 0) {   /* PIF_ERROR_DISK_FULL */
        PifCloseFile(hsOpPpiFileHandle);
        PifDeleteFile(auchOP_PPI);              /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    /* Make Header File */
    memset( &PpiFileHed, 0x00, sizeof( OPPPI_FILEHED ));
    PpiFileHed.usNumberOfPpi = usNumberOfPpi;

    ulWriteOffset = OP_FILE_HEAD_POSITION;
    Op_WritePpiFile( ulWriteOffset, &PpiFileHed, sizeof( OPPPI_FILEHED ));
    ulWriteOffset += ( ULONG )sizeof( OPPPI_FILEHED );

    /* Clear Contents of File */
    memset( auchPpiFile, 0x00, sizeof( auchPpiFile ));

    while( ulWriteOffset < ulNewSize ) {
        Op_WritePpiFile( ulWriteOffset, auchPpiFile, sizeof( auchPpiFile ));
        ulWriteOffset += ( ULONG )sizeof( auchPpiFile );
    }

    Op_ClosePpiFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiCheckAndCreatFile(USHORT usNumberOfPpi,
*                                                 USHORT usLockHnd )
*
*       Input:    usNumberOfPpi - Number of Promotinal Pricing Item
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*    Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Promotional Pricing Item File.
*==========================================================================
*/
SHORT   OpPpiCheckAndCreatFile(USHORT usNumberOfPpi, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((Op_PpiOpenFile()) < 0) {

        sStatus = OpPpiCreatFile(usNumberOfPpi, usLockHnd);
        return (sStatus);
    }
    PifCloseFile(hsOpPpiFileHandle);
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiCheckAndDeleteFile(USHORT usNumberOfPpi,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfCpn - Number of Promotional Pricing Item
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*                   OP_DELETE_PPI_FILE
**  Description:  Check PPI File size, if not equal the delete PPI File.
*==========================================================================
*/
SHORT   OpPpiCheckAndDeleteFile(USHORT usNumberOfPpi, USHORT usLockHnd)
{
    OPPPI_FILEHED PpiFileHed;

    Op_LockCheck(usLockHnd);

    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(OPPPI_FILEHED));

    PifCloseFile(hsOpPpiFileHandle);

    if ( PpiFileHed.usNumberOfPpi == usNumberOfPpi ) {
        return (OP_PERFORM);
    }

    PifLog(MODULE_OP_PPI, LOG_EVENT_OP_PPI_DELETE_FILE);
    PifDeleteFile(auchOP_PPI);
    return (OP_DELETE_PPI_FILE);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif->uchPpiCode
*               pPif->ParaPpi
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_NEW_PPI
*   Abnormal End:  OP_CHANGE_PPI
*                  OP_FILE_NOT_FOUND
*                  OP_FILE_FULL
*                  OP_LOCK
*
**  Description:  Add and change plu record.
*==========================================================================
*/
SHORT   OpPpiAssign(PPIIF *pPif, USHORT usLockHnd)
{
    OPPPI_FILEHED PpiFileHed;

    /*  CHECK LOCK */

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */

    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(OPPPI_FILEHED));

    /* CHECK RANGE OF PPI NO  */

    if (pPif->uchPpiCode == 0) {
        Op_ClosePpiFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_PPI);
    }

    if ( pPif->uchPpiCode > (USHORT)PpiFileHed.usNumberOfPpi ) { //ESMITH
	//if ( pPif->uchPpiCode > (UCHAR)PpiFileHed.usNumberOfPpi ) {
        Op_ClosePpiFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_PPI);
    }

    Op_WritePpiFile( sizeof(PpiFileHed) + (ULONG)((pPif->uchPpiCode - 1) * sizeof(OPPPI_PARAENTRY)),
                      &pPif->ParaPpi, sizeof(pPif->ParaPpi));
    Op_ClosePpiFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiDelete(PPIIF *pPif, USHORT usLockHnd)
*
*       Input:  pPif->uchPpiCode
*               pPif->ParaPpi
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_PERFORM
*   Abnormal End:  OP_CHANGE_PPI
*                  OP_FILE_NOT_FOUND
*                  OP_FILE_FULL
*                  OP_LOCK
*
**  Description:  Add and change plu record.
*==========================================================================
*/
SHORT   OpPpiDelete(PPIIF *pPif, USHORT usLockHnd)
{
    PPIIF   PpiIF;
    SHORT   sRetCode;

    memset( &PpiIF, 0x00, sizeof( PPIIF ));
    PpiIF.uchPpiCode = pPif->uchPpiCode;

    sRetCode = OpPpiAssign( &PpiIF, usLockHnd );
    return ( sRetCode );
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPpiIndRead(PPIIF *Pif, USHORT usLockHnd)
*
*       Input:    *Pif->uchPpiCode
*                 usLockHnd
*      Output:    PPIIF  *Pif->ParaPpi
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a PPI paramater Individually.
*==========================================================================
*/
SHORT   OpPpiIndRead(PPIIF *pPif, USHORT usLockHnd)
{
    OPPPI_FILEHED PpiFileHed;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    if ((Op_PpiOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadPpiFile(OP_FILE_HEAD_POSITION, &PpiFileHed, sizeof(PpiFileHed));

    if ((!pPif->uchPpiCode) ||
         ( pPif->uchPpiCode > (USHORT)PpiFileHed.usNumberOfPpi)) { //ESMITH
 		//( pPif->uchPpiCode > (UCHAR)PpiFileHed.usNumberOfPpi)) {

        Op_ClosePpiFileReleaseSem();
        return (OP_ABNORMAL_PPI);
    }

    Op_ReadPpiFile((ULONG)(sizeof(PpiFileHed) + ((pPif->uchPpiCode - 1) * sizeof(OPPPI_PARAENTRY))), &pPif->ParaPpi, sizeof(OPPPI_PARAENTRY));

    Op_ClosePpiFileReleaseSem();
    return (OP_PERFORM);
}

/******************** New Functions (Release 3.1) END *******************/

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldCreatFile(USHORT usNumberOfAddress,
*                                         USHORT usLockHnd)
*
*       Input:    usNumberOfAddress
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NO_MAKE_FILE
*
**  Description: Create Multi-line Display Mnemonics File. V3.3
*==========================================================================
*/
SHORT   OpMldCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED   MldFileHed;
    ULONG           ulActualPosition;
    ULONG           ulNewSize;
    ULONG           ulWriteOffset;
    UCHAR           auchMldFile[ sizeof( MLD_RECORD ) ];

    PifRequestSem(husOpSem);

    Op_LockCheck(usLockHnd);  /* Dummy */

    /* Delete MLD File */
    PifDeleteFile(auchOP_MLD);

    /* Check Number of MLD */
    if (!usNumberOfAddress) {
        PifReleaseSem(husOpSem);
        return (OP_PERFORM);
    }

    if ((hsOpMldFileHandle = PifOpenFile(auchOP_MLD, auchNEW_FILE_WRITE)) < 0) {
        PifAbort(MODULE_OP_MLD, FAULT_ERROR_FILE_OPEN);
    }

    /* Check file size and creat file */
    ulNewSize = ( ULONG )( sizeof( OPMLD_FILEHED ) + ( sizeof( MLD_RECORD ) * usNumberOfAddress ));
    if (PifSeekFile(hsOpMldFileHandle, ulNewSize, &ulActualPosition) < 0) {   /* PIF_ERROR_DISK_FULL, OpMldCreatFile() */
        PifCloseFile(hsOpMldFileHandle);
        PifDeleteFile(auchOP_MLD);              /* Can't make file then delete file */
        PifReleaseSem(husOpSem);
        return (OP_NO_MAKE_FILE);
    }

    /* Make Header File */
    memset( &MldFileHed, 0x00, sizeof( OPMLD_FILEHED ));
    MldFileHed.usNumberOfAddress = usNumberOfAddress;

    ulWriteOffset = OP_FILE_HEAD_POSITION;
    Op_WriteMldFile( ulWriteOffset, &MldFileHed, sizeof( OPMLD_FILEHED ));
    ulWriteOffset += ( ULONG )sizeof( OPMLD_FILEHED );

    /* Clear Contents of File */
    memset( auchMldFile, 0x00, sizeof( auchMldFile ));

    while( ulWriteOffset < ulNewSize ) {
        Op_WriteMldFile( ulWriteOffset, auchMldFile, sizeof( auchMldFile ));  // OpMldCreatFile() - create the MLD mnemonics file.
        ulWriteOffset += ( ULONG )sizeof( auchMldFile );
    }

    Op_CloseMldFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldCheckAndCreatFile(USHORT usNumberOfAddress,
*                                                 USHORT usLockHnd )
*
*       Input:    usNumberOfAddress
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*    Abnormal End:  OP_NO_MAKE_FILE
*
**  Description:  Create Multi-Line Mnemonics File. V3.3
*==========================================================================
*/
SHORT   OpMldCheckAndCreatFile(USHORT usNumberOfAddress, USHORT usLockHnd)
{
    SHORT   sStatus;

    if ((Op_MldOpenFile()) < 0) {
        sStatus = OpMldCreatFile(usNumberOfAddress, usLockHnd);
        return (sStatus);
    }
    PifCloseFile(hsOpMldFileHandle);
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldCheckAndDeleteFile(USHORT usNumberOfAddress,
*                                                  USHORT usLockHnd)
*
*       Input:    usNumberOfAddress
*                 usLockHnd
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*      Normal End:  OP_PERFORM
*                   OP_DELETE_MLD_FILE
**  Description:  Check MLD File size, if not equal the delete MLD File. V3.3
*==========================================================================
*/
SHORT   OpMldCheckAndDeleteFile(USHORT usNumberOfAddress, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed;

    Op_LockCheck(usLockHnd);

    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        return (OP_PERFORM);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));

    if ( MldFileHed.usNumberOfAddress < usNumberOfAddress ) {
		MldFileHed.usNumberOfAddress = usNumberOfAddress;
		Op_WriteMldFile (OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    PifCloseFile(hsOpMldFileHandle);

    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd)
*
*       Input:  pMld->uchAddress
*               pMld->aszMnemonics
*               usLockHnd
*
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*    Normal End:   OP_NEW_MLD
*   Abnormal End:  OP_CHANGE_MLD
*                  OP_FILE_NOT_FOUND
*                  OP_FILE_FULL
*                  OP_LOCK
*
**  Description:  Add and change mld mnemonics. V3.3
*                 If the file header indicates the MLD file contains less than the
*                 maximum number of MLD records we support then set the file header
*                 to the number we do support.
*==========================================================================
*/
SHORT   OpMldAssign(MLDIF *pMld, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed;

    /*  CHECK LOCK */
	NHPOS_ASSERT(sizeof(TCHAR) == sizeof(pMld->aszMnemonics[0]));
	NHPOS_ASSERT(sizeof(pMld->aszMnemonics) >= OP_MLD_MNEMONICS_LEN * sizeof(TCHAR));

    PifRequestSem(husOpSem);

    if (Op_LockCheck(usLockHnd) == OP_LOCK) {
        PifReleaseSem(husOpSem);
        return (OP_LOCK);
    }

    /*  OPEN FILE  */
    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));

	if (MldFileHed.usNumberOfAddress < MAX_MLDMNEMO_SIZE) {
		MldFileHed.usNumberOfAddress = MAX_MLDMNEMO_SIZE;
		Op_WriteMldFile (OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(OPMLD_FILEHED));
	}

    /* CHECK RANGE OF MLD NO  */
    if ( (pMld->usAddress == 0) || (pMld->usAddress > MldFileHed.usNumberOfAddress ) ) {
        Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
        return (OP_ABNORMAL_MLD);
    }

    Op_WriteMldFile( sizeof(MldFileHed) + (ULONG)((pMld->usAddress - 1) * (OP_MLD_MNEMONICS_LEN + 1) * sizeof(TCHAR)),
                      pMld->aszMnemonics, OP_MLD_MNEMONICS_LEN * sizeof(TCHAR));
    Op_CloseMldFileReleaseSem();           /* close file and release semaphore */
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd)
*
*       Input:    *pMld->uchAddress
*                 usLockHnd
*      Output:    MLDIF  *pMld->aszMnemonics
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_NOT_IN_FILE
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*
**  Description: Read a MLD mnemonics Individually. V3.3
*                if requested MLD record number is greater than the max
*                in the MLD file then we will just return an empty mnemonic.
*==========================================================================
*/
SHORT   OpMldIndRead(MLDIF *pMld, USHORT usLockHnd)
{
    OPMLD_FILEHED MldFileHed;

    memset(&pMld->aszMnemonics, 0, sizeof(pMld->aszMnemonics));

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    if (usLockHnd) {
        if (Op_LockCheck(usLockHnd) == OP_LOCK) {
            PifReleaseSem(husOpSem);
            return (OP_LOCK);
        }
    }

    if ((Op_MldOpenFile()) == PIF_ERROR_FILE_NOT_FOUND) {  /* pif_error_file_exist */
        PifReleaseSem(husOpSem);
        return (OP_FILE_NOT_FOUND);
    }

    Op_ReadMldFile(OP_FILE_HEAD_POSITION, &MldFileHed, sizeof(MldFileHed));

    if ((!pMld->usAddress) || ( pMld->usAddress > MldFileHed.usNumberOfAddress)) {
        Op_CloseMldFileReleaseSem();
		NHPOS_ASSERT_TEXT(0, "OpMldIndRead(): ( pMld->uchAddress > (UCHAR)MldFileHed.usNumberOfAddress)");
        return (OP_PERFORM);
    }

    Op_ReadMldFile((ULONG)(sizeof(MldFileHed) + (pMld->usAddress - 1) * (OP_MLD_MNEMONICS_LEN + 1) * sizeof(TCHAR)), //Specify the offset  in BYTES from the beginning of the file to location
                    &pMld->aszMnemonics, OP_MLD_MNEMONICS_LEN * sizeof(TCHAR)); //Specify Number of BYTES to READ

    Op_CloseMldFileReleaseSem();
    return (OP_PERFORM);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpPluOepRead(OPPLUOEPPLUNO *pPluNo, OPPLUOEPPLUNAME *pPluName, USHORT usLockHnd)
*
*       Input:    *Pif->auchPluNo
*                 *Pif->uchPluAdj
*                 usLockHnd
*      Output:    PLUIF  *Pif->ParaPlu
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*                  OP_NOT_IN_FILE
*
*
**  Description: Read a Plu paramater Individually.
*==========================================================================
*/
SHORT   OpPluOepRead(OPPLUOEPPLUNO *pPluNo, OPPLUOEPPLUNAME *pPluName, USHORT usLockHnd)
{
    MFINFO mfinfo;
    USHORT usResult, i;
    USHORT usRecPluLen;
    RECPLU recplu;
    PRECPLU pRecPlu;
    ITEMNO itemno;
    PITEMNO pItemNumber;

    pRecPlu = &recplu;
    pItemNumber = &itemno;

    PifRequestSem(husOpSem);

    /*  LOCK CHECK */
    /** removed, V1.0.13
    if (Op_LockCheck(usLockHnd) == OP_LOCK)
    {
        PifReleaseSem(husOpSem);
        return OP_LOCK;
    } **/

    usResult = PluSenseFile(FILE_PLU,&mfinfo);
    if(usResult == SPLU_FILE_NOT_FOUND)
    {
        PifReleaseSem(husOpSem);
        return OpConvertPluError(usResult);
    }

	for (i=0; i < OP_OEP_PLU_SIZE; i++) {

		memset(&pPluName->auchPluName[i][0], 0, STD_PLU_MNEMONIC_LEN * sizeof(TCHAR));
		if (pPluNo->auchPluNo[i][0]) {
    		_tcsncpy(pItemNumber->aucPluNumber,&pPluNo->auchPluNo[i][0],PLU_NUMBER_LEN);

		    usResult = PluReadRecord(FILE_PLU, pItemNumber, 1, pRecPlu, &usRecPluLen);
		    if(usResult == 0)
		    {
				_tcsncpy(&pPluName->auchPluName[i][0], pRecPlu->aucMnemonic, STD_PLU_MNEMONIC_LEN);
				pPluName->uchItemType[i] = pRecPlu->uchItemType;
				pPluName->auchContOther[0][i] = pRecPlu->aucStatus[2];
				pPluName->auchContOther[1][i] = pRecPlu->aucStatus[3];
				pPluName->auchContOther[2][i] = pRecPlu->aucStatus[4]; //SR 336 Transfer the control information for
																	   //prohibited adjectives being displayed JHHJ
			}
		}
	}

    PifReleaseSem(husOpSem);

    return OpConvertPluError(usResult);
}

/*
*==========================================================================
**    Synopsis:    SHORT   OpReqLay(USHORT usLockHnd)
*
*       Input:    *Pif->auchPluNo
*                 *Pif->uchPluAdj
*                 usLockHnd
*      Output:    PLUIF  *Pif->ParaPlu
*       InOut:    Nothing
*
**  Return    :
*     Normal End:  OP_PERFORM
*   Abnormal End:  OP_PLU_ERROR
*                  OP_FILE_NOT_FOUND
*                  OP_LOCK
*                  OP_NOT_IN_FILE
*
*
**  Description: Request the standard set of layout files.
*                The standard set of layout files comprise
*                the first two layout files, PARAMLA and PARAMLB
*                along with the next four of the possible layout
*                files, PARAMLC, PARAMLD, PARAMLE, PARAMLF..
*==========================================================================
*///ESMITH LAYOUT
SHORT   OpReqLay(USHORT usLockHnd)
{
    SHORT   sStatus;

	sStatus = OpReqFile(_T("PARAMLB"), usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return (sStatus);
    }

	sStatus = OpReqFile(_T("PARAMLA"), usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return (sStatus);
    }

	sStatus = OpReqFile(_T("PARAMLC"), usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return (sStatus);
    }

	sStatus = OpReqFile(_T("PARAMLD"), usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return (sStatus);
    }

	sStatus = OpReqFile(_T("PARAMLE"), usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return (sStatus);
    }

	sStatus = OpReqFile(_T("PARAMLF"), usLockHnd);
    if ( sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND ) {
        return (sStatus);
    }

    return (OP_PERFORM);
}

// Printer logo files are bit map files that are downloaded to the receipt printer
// and are printed on the receipt as either a top of form logo, typically a logo
// of the establishment, or other bit maps such as coupons.
SHORT   OpReqPrinterLogos(USHORT usLockHnd)
{
	CHAR    xBuff[128];
    SHORT   sStatus;
	int     i = 0;
	TCHAR   *tcsLogoFileNames[] = {
				_T("PARAMLID"),      // printer logo index file
				_T("PARAMLO1"),      // printer logo file for logo # 1
				_T("PARAMLO2"),
				_T("PARAMLO3"),
				_T("PARAMLO4"),
				_T("PARAMLO5"),
				_T("PARAMLO6"),
				_T("PARAMLO7"),
				_T("PARAMLO8"),
				_T("PARAMLO9"),
				_T("PARAMLOA"),
				0
			};

	for (i = 0; tcsLogoFileNames[i]; i++) {
		sStatus = OpReqFile(tcsLogoFileNames[i], usLockHnd);
		sprintf (xBuff, "OpReqPrinterLogos(): OpReqFile() %S status = %d", tcsLogoFileNames[i], sStatus);
		NHPOS_ASSERT_TEXT((sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND), xBuff);
	}

    return (OP_PERFORM);
}

// Additional PARAM files are files that have been added containing various kinds
// of provisioning data.  Implements CLI_LOAD_ADDED_PARAM and CLI_FCOPREQPARAM.
//
// There is a need to create a set of empty files for these at GenPOS startup
// in order for file transfer to work properly.
// See function UifCreateMain() for an example of file creation at startup.
//
// Additional PARAM files are:
//  - PARAMINI was added in Rel 2.2 to provide initialization data read at startup
//             this file contains things like customer specific flags, etc.
//  - PARAMTDR was added in Rel 2.2 for Amtrak containing tender key additional data
//             such as electronic payment account identification rules, etc.
SHORT   OpReqAdditionalParams(USHORT usLockHnd)
{
	CHAR    xBuff[128];
    SHORT   sStatus;
	int     i = 0;
	TCHAR   *tcsParamFileNames[] = {
				_T("PARAMINI"),      // parameter INI file with initialization data
				_T("PARAMTDR"),      // tender parameter data file with tender rules
				0
			};

	for (i = 0; tcsParamFileNames[i]; i++) {
		sStatus = OpReqFile(tcsParamFileNames[i], usLockHnd);
		sprintf (xBuff, "OpReqAdditionalParams(): OpReqFile() %S status = %d", tcsParamFileNames[i], sStatus);
		NHPOS_ASSERT_TEXT((sStatus != OP_PERFORM && sStatus != OP_FILE_NOT_FOUND), xBuff);
	}

    return (OP_PERFORM);
}


//----------   Database File open functions  ------

/*                 
*==========================================================================
**    Synopsis:   SHORT  Op_OepOpenFile(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return   :    Nothing
*
**  Description:  Open "PARAMOEP" file.
*==========================================================================
*/
SHORT  Op_OepOpenFile(USHORT  usOepTableNo)
{
	hsOpOepFileHandle = -1;    // Op_OepOpenFile() initialize the variable in case there is a problem

	if (usOepTableNo < sizeof(auchOP_OEP_TABLE)/sizeof(auchOP_OEP_TABLE[0])) {
		hsOpOepFileHandle = PifOpenFile(auchOP_OEP_TABLE[usOepTableNo], auchOLD_FILE_READ_WRITE);
	}

    return (hsOpOepFileHandle);
}
/*======= End of Source =======*/