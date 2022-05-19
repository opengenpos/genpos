/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file used in print module                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtDfl.H                                                      
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 01.00.12 :  R.itoh    : initial                                   
* Dec-20-93 :          : K.nakajima : Add structre type "PRTDFLBUFF"
*                                     Add prototype "PrRSetT.c"
* Jan-26-95 : 03.00.00 :  M.Ozawa   : Add prototype for CPM/EPT
* Mar-06-95 : 03.00.00 : T.Nakahata : Add Unique Transaction Number
* Jul-21-95 : 03.00.04 : T.Nakahata : add customer name print
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
* Jan-07-00 : 01.00.00 : H.Endo     : Add Include prtRJ.h for Printer RJ
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
/* endo update(add) 2000/01/01 */
#include <prtRJ.h>
/*
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/
#define     PRT_DFL_HEADER_LEN   16     /* data header length */
#define     PRT_DFL_TMLEN         2     /* terminal No. length */
#define     PRT_DFL_LINELEN       2     /* line No. length */
#define     PRT_DFL_VLILEN        3     /* VLI length */
#define     PRT_DFL_CONSLEN       4     /* consecutive length */
#define     PRT_DFL_DSP_DATA    575     /* display data area */
#define     PRT_DFL_LINE         24     /* display 1 line length */
#define     PRT_DFL_LINE_MAX     16     /* Max display lines, sized to KDS_LINE_DATA_LEN / (PRT_DFL_LINE + 1) or ItemKdsSendData.aszLineData[KDS_LINE_DATA_LEN] */

#define     PRT_DFL_FIRST_FRAME  0x31   /* first frame */
#define     PRT_DFL_NOT_FIRST_FRAME  0x30   /* not first frame */
#define     PRT_DFL_END_FRAME  0x29     /* end frame */
#define     PRT_DFL_SERVICE_END_FRAME  0x28  /* end frame for service total */

/* -- for fbPrtDflErr -- */
#define     PRT_ALREADY_DISP      1     /* already display error */

/*
============================================================================
+                     T Y P E    D E F I N I T I O N S
============================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


typedef struct {
    DFLINTERFACE    Dfl;
    TCHAR           auchDispData[PRT_DFL_DSP_DATA];
} PRTDFLIF;



/* -- type define for DFL work buffer -- */
typedef struct {
    TCHAR  aszDflBuff[30][PRT_DFL_LINE + 1];
} PRTDFLBUFF;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/
extern PRTDFLIF    PrtDflIf;          /* display data buffer */
extern UCHAR    fbPrtDflErr;          /* error displayed or not */
extern UCHAR   uchPrtDflTypeSav;      /* save transaction type */

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/

/* -- PrDDfl_.c -- */
VOID PrtDflInit(TRANINFORMATION  *pTran);
VOID PrtDflCustHeader(TRANINFORMATION  *pTran);
USHORT PrtDflHeader(TCHAR *pszWork, TRANINFORMATION  *pTran);
USHORT PrtDflTrailer(TCHAR *pszWork, TRANINFORMATION *pTran, ULONG  ulStReg);
VOID PrtDflTrType(TRANINFORMATION *pTran);
VOID PrtDflCheckVoid(USHORT fbMod);
VOID PrtDflSetCRTNo(TRANINFORMATION *pTran, ITEMSALES *pItem);
VOID PrtDflSetTtlNo(TRANINFORMATION *pTran);
UCHAR PrtDflGetNo(UCHAR uchCRTNo);
VOID PrtDflIType(USHORT usLineNo, UCHAR uchItemType);
VOID PrtDflSetData(TCHAR *pszData, USHORT *pusOff);
VOID PrtDflSend(VOID);
UCHAR PrtDflXor(VOID *puchStart, USHORT usLen);

/* -- PrDCmn_.c -- */
USHORT  PrtDflVoid(TCHAR *pszWork, USHORT  fbMod, USHORT usReasonCode);
USHORT  PrtDflNumber(TCHAR *pszWork, TCHAR  *pszNumber);
USHORT  PrtDflMnemNumber(TCHAR *pszWork, USHORT usTranAdr, TCHAR  *pszNumber);
USHORT  PrtDflTaxMod(TCHAR *pszWork, USHORT fsTax, USHORT  fbMod);
USHORT  PrtDflWeight(TCHAR *pszWork, TRANINFORMATION  *pTran, ITEMSALES  *pItem);
USHORT  PrtDflSeatNo(TCHAR *pszWork, ITEMSALES *pItem);
USHORT  PrtDflQty(TCHAR *pszWork, ITEMSALES  *pItem);
USHORT  PrtDflLinkQty(TCHAR *pszWork, ITEMSALES  *pItem);
USHORT  PrtDflItems(TCHAR *pszWork, ITEMSALES  *pItem);
USHORT  PrtDflChild(TCHAR *pszWork, UCHAR uchAdj, TCHAR *pszMnem);
USHORT  PrtDflLinkPLU(TCHAR *pszWork, USHORT fsModified, UCHAR uchAdj, TCHAR *pszMnem, DCURRENCY lPrice);
USHORT  PrtDflMnem(TCHAR *pszWork, USHORT usAdr, BOOL fsType);
USHORT  PrtDflPerDisc(TCHAR *pszWork, USHORT usAdr, UCHAR uchRate, DCURRENCY lAmount);
USHORT  PrtDflWaiTaxMod(TCHAR *pszWork, ULONG ulID, USHORT  fsTax, USHORT  fbMod);
USHORT  PrtDflAmtMnem(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount);
USHORT  PrtDflWaiter(TCHAR *pszWork, ULONG ulWaiID);
USHORT  PrtDflGuest(TCHAR *pszWork, USHORT usGuestNo, UCHAR uchCDV);
USHORT  PrtDflTranNum(TCHAR *pszWork, USHORT usTranAdr, ULONG ulNumber);
USHORT  PrtDflEtkCode(TCHAR *pszWork, ITEMMISC  *pItem);
USHORT  PrtDflJobTimeIn(TCHAR *pszWork, TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT  PrtDflJobTimeOut(TCHAR *pszWork, TRANINFORMATION  *pTran, ITEMMISC  *pItem);
USHORT  PrtDflAmtSym(TCHAR *pszWork, USHORT usAdr, DCURRENCY lAmount, BOOL fsType);
USHORT  PrtDflMnemCount(TCHAR *pszWork, USHORT usAdr, SHORT sCount);
USHORT  PrtDflZeroAmtMnem(TCHAR *pszWork, USHORT usAddress, DCURRENCY lAmount);
USHORT  PrtDflForeign1(TCHAR *pszWork, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus);
USHORT  PrtDflForeign2(TCHAR *pszWork, ULONG ulRate, UCHAR fbStatus2);
USHORT  PrtDflForeign3(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lForeign, UCHAR uchAdr, UCHAR fbStatus);
USHORT  PrtDflTblPerson(TCHAR *pszWork, USHORT usTblNo, USHORT usNoPerson, SHORT sWidthType);
USHORT  PrtDflCustName( TCHAR *pszDest, TCHAR *pszCustomerName );
USHORT  PrtDflMulChk(TCHAR *pszWork, USHORT usGuestNo, UCHAR uchCDV);
USHORT  PrtDflOffTend(TCHAR *pszWork, USHORT fbMod);
USHORT  PrtDflCPRoomCharge(TCHAR *pszWork, TCHAR *pRoomNo, TCHAR *pGuestID);
USHORT  PrtDflOffline(TCHAR *pszWork, USHORT fbMod, TCHAR *auchDate, TCHAR *auchApproval);
USHORT  PrtDflFolioPost(TCHAR *pszWork, TCHAR *pszFolioNo, TCHAR *pszPostTranNo);
USHORT  PrtDflCPRspMsgText(TCHAR *pszWork, TCHAR *pRspMsgText);

USHORT  PrtDflCpnItem( TCHAR *pszWork, TCHAR *pszMnemonic, DCURRENCY lAmount );

USHORT  PrtDflZAMnemShift(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtDflAmtMnemShift(TCHAR *pszWork, USHORT usTranAdr, DCURRENCY lAmount, USHORT usColumn);
USHORT  PrtDflPLUNo(TCHAR *pszDest, TCHAR *puchPLUCpn);
USHORT PrtDflRandomNumber( TCHAR *pszDest, TCHAR *pszMnemonic, TCHAR *pszNumber );
USHORT  PrtDflMnemonic(TCHAR *pszWork, TCHAR  *pszMnemonic);
USHORT  PrtDflPrtMod(TCHAR *pszWork, UCHAR uchAdr, DCURRENCY lAmount); /* 2172 */
USHORT  PrtDflTaxMod2(TCHAR *pszWork, USHORT fsTax, UCHAR  uchMinorClass); /* 2172 */
USHORT    PrtDflModSeatNo(TCHAR *pszWork, TCHAR uchSeatNo); /* 2172 */

USHORT  PrtDflItemsEx(TCHAR *pszWork, ITEMSALES  *pItem);	/* for enhanced KDS */

/* -- PrRSET_.c -- */
UCHAR PrtDflSETChkChildKP(TRANINFORMATION  *pTran, ITEMSALES *pItem);
SHORT PrtDflSETParent(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem);
VOID  PrtDflSETSendAsNotKP(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem);
VOID  PrtDflSETSendNoKPChild(PRTDFLBUFF *pBuff, TRANINFORMATION  *pTran, ITEMSALES *pItem);
VOID  PrtDflSETSend(UCHAR uchDestKP1, UCHAR uchDestKP2, USHORT  usLineNo, PRTDFLBUFF *pBuff, ITEMSALES *pItem);

/* ===== End of File ( PrtDfl.H ) ===== */
