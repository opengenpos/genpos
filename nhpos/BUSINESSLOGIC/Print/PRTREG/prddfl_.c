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
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1996       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print  Kitchen printer               
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrDDfl_.C                                                      
* --------------------------------------------------------------------------
* Abstract:
*               PrtDflInit(): set initial data
*#              PrtDflCustHeader(): set customer name data
*               PrtDflHeader(): set header data
*               PrtDflTrailer(): set trailer data
*               PrtDflTrType(): check transaction type
*               PrtDflCheckVoid(): item void status check
*               PrtDflSetCRTNo(): set destination CRT No.
*               PrtDflIType(): set item type etc.
*               PrtDflSetData(): set display data
*               PrtDflSend(): send display data
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jul-12-93 : 01.00.12 :  R.itoh    : initial
* Mar-03-95 : 03.00.00 : T.Nakahata : Add Unique Trans No. (mod PrtDflHeader)
* Aug-24-95 : 03.00.04 : T.Nakahata : display customer name at 1st frame
* Sep-26-95 : 03.00.09 : T.Nakahata : init aszDflHeader at DflCustHeader
* Nov-24-95 : 03.01.00 : M.Ozawa    : mod. PrtDflSetTtlNo() for CRT #5-8
* Apr-15-96 : 03.01.04 : T.Nakahata : add DFL message sequential counter.
* Jun-04-96 : 03.01.08 : M.Ozawa    : Del Unique Trans No. for ISV KDS trouble
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbstb.h>
#include <csstbdfl.h>
#include <rfl.h>
#include <uie.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "csstbpar.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
extern CONST TCHAR FARCONST  aszPrtKPGuest_CD[];
extern CONST TCHAR FARCONST  aszPrtKPGuest[];   
extern CONST TCHAR FARCONST  aszPrtKPTrailer[];
CONST TCHAR FARCONST aszPrtDflHeader[] = _T("%-4s %-3s  %-8.8s %-3s  %-4s %s");/*8 characters JHHJ*/


/*
*===========================================================================
** Format  : VOID PrtDflInit(TRANINFORMATION *pTran)
*
*   Input  : TRANINFORMATION *pTran,         -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets initial data. ( for display on the fly )
*===========================================================================
*/
VOID    PrtDflInit(TRANINFORMATION  *pTran)
{
    TCHAR   aszConsNo[5];
    TCHAR   aszTermNo[3];
    USHORT  usLen;

    /* -- initialize dfl data area -- */
    memset(&PrtDflIf, '\0', sizeof(PrtDflIf));

    /* -- set start character -- */
    PrtDflIf.Dfl.DflHead.uchSTX = DFL_STX;

    /* -- set terminal unique address -- */
    memset(aszTermNo, '\0', sizeof(aszTermNo));
    _itot((USHORT)CliReadUAddr(), aszTermNo, 10);        
    usLen = _tcslen(aszTermNo);
    tcharnset(PrtDflIf.Dfl.DflHead.aszTermAdr, _T('0'), PRT_DFL_TMLEN - usLen);
    _tcsncpy(&PrtDflIf.Dfl.DflHead.aszTermAdr[PRT_DFL_TMLEN - usLen], aszTermNo, usLen);

    /* -- set consecutive No. -- */
    memset(aszConsNo, '\0', sizeof(aszConsNo));
    _itot(pTran->TranCurQual.usConsNo, aszConsNo, 10);
    usLen = _tcslen(aszConsNo);
    tcharnset(PrtDflIf.Dfl.DflHead.aszConsNo, '0', PRT_DFL_CONSLEN - usLen);
    _tcsncpy(&PrtDflIf.Dfl.DflHead.aszConsNo[PRT_DFL_CONSLEN - usLen], aszConsNo, usLen);

    /* -- set transaction type (normal, p-void, training) -- */
    PrtDflTrType(pTran);

}

/*
*===========================================================================
** Format  : VOID PrtDflCustHeader( TRANINFORMATION *pTran )
*
*   Input  : TRANINFORMATION *pTran      -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set kitchen CRT's header.
*===========================================================================
*/
VOID PrtDflCustHeader( TRANINFORMATION *pTran )
{
    ITEMMODIFIER    ItemModifier;
/*    UCHAR           aszDflBuff[ 7 ][ PRT_DFL_LINE + 1 ]; */
/*    USHORT          usLineNo; */
/*    USHORT          usOffset = 0; */
/*    USHORT          i; */

    /* --- this frame is not 1st frame or customer name is not specified,
        this function does not execute --- */

    if ( PrtDflIf.Dfl.uchSeqNo != 0 ) {
        return;
    }

    /* --- initialize Dfl Buffer before set data (9-26-95) --- */
	/*    memset( aszDflBuff, 0x00, sizeof( aszDflBuff )); */
    if ( pTran->TranGCFQual.aszName[ 0 ] != '\0' ) {

        memset( &ItemModifier, 0x00, sizeof( ItemModifier ));

        ItemModifier.uchMajorClass = CLASS_ITEMMODIFIER;
        ItemModifier.uchMinorClass = CLASS_ALPHANAME;

        _tcsncpy( ItemModifier.aszName, pTran->TranGCFQual.aszName, NUM_NAME );

        if ( ItemModifier.aszName[ NUM_NAME - 2 ] == PRT_DOUBLE ) {
            ItemModifier.aszName[ NUM_NAME - 2 ] = '\0';
        }
        PrtDflModCustName( pTran, &ItemModifier );
    }

/*******    not send unique transaction no by ISV KDS troble

    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {

        / -- set header -- /

        usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

        / -- set trailer -- /

        usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ],
                                   pTran,
                                   pTran->TranCurQual.ulStoreregNo );

        / -- set unique trans # --- /

        usLineNo += PrtDflTblPerson( aszDflBuff[ usLineNo ],
                                     pTran->TranGCFQual.usTableNo,
                                     0,
                                     PRT_DOUBLE );

        / -- set destination CRT -- /

        PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;   / for #1 - #4 /
        PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;   / for #5 - #8 /

        / -- set display data in the buffer -- / 

        PrtDflIType( usLineNo, DFL_MODIFIER );

        for ( i = 0; i < usLineNo; i++ ) {

            PrtDflSetData( aszDflBuff[ i ], &usOffset );

            if ( aszDflBuff[ i ][ PRT_DFL_LINE ] != '\0' ) {
                i++;
            }
        }

        / -- send display data -- /

        PrtDflSend();
    }
********/

}

/*
*===========================================================================
** Format  : VOID   PrtDflHeader(UCHAR *pszWork, TRANINFORMATION *pTran);      
*
*   Input  : UCHAR *pszWork              -display data buffer poiner
*            TRANINFORMATION *pTran      -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set kitchen CRT's header.
*===========================================================================
*/
USHORT   PrtDflHeader(TCHAR *pszWork, TRANINFORMATION *pTran)
{

	TCHAR  aszTableMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };/* PARA_... defined in "paraequ.h" */
	TCHAR  aszGuestMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszPersonMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };  /* PARA_... defined in "paraequ.h" */
	TCHAR  aszTableNo[PRT_ID_LEN + 1] = { 0 };
	TCHAR  aszNoPerson[PRT_ID_LEN + 1] = { 0 };
	TCHAR  aszGuestNo[PRT_DFL_LINE + 1] = { 0 };
	TCHAR  aszWork[PRT_DFL_LINE * 2 + 1] = { 0 };

    if ( PrtDflIf.Dfl.uchSeqNo != 0 ) {                 /* first frame ? */
        return(0);
    }

    /* --- if current system uses unique transaction number,
        do not send its number and mnemonic --- */
    if ( ! ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO )) {
        /* --- current system uses table no. --- */
        if (pTran->TranGCFQual.usTableNo != 0) {
            _itot(pTran->TranGCFQual.usTableNo, aszTableNo, 10);/* convert int to ascii */
			RflGetSpecMnem(aszTableMnem, SPC_TBL_ADR);        /* get table mnemo */
        }
    }

    if ((pTran->TranGCFQual.usNoPerson != 0) && (CliParaMDCCheck(MDC_TRANNUM_ADR, EVEN_MDC_BIT3))){
        _itot(pTran->TranGCFQual.usNoPerson, aszNoPerson, 10);/* convert int to ascii */
		RflGetTranMnem(aszPersonMnem, TRN_PSN_ADR);        /* get person mnemo */
    }

    if (pTran->TranGCFQual.usGuestNo != 0) {             /* GCF#0, doesnot print */
		RflGetSpecMnem(aszGuestMnem, SPC_GC_ADR);        /* get GC no. menm */

        /* -- GCF no. with check digit -- */
        if (pTran->TranGCFQual.uchCdv != 0) {
            RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest_CD, pTran->TranGCFQual.usGuestNo, pTran->TranGCFQual.uchCdv);
        } else {
            RflSPrintf(aszGuestNo, TCHARSIZEOF(aszGuestNo), aszPrtKPGuest, pTran->TranGCFQual.usGuestNo);
        }
    }

    if (  (pTran->TranGCFQual.usTableNo != 0)
        ||(pTran->TranGCFQual.usNoPerson != 0)
        ||(pTran->TranGCFQual.usGuestNo != 0)) {

        /* -- write table no & no of person -- */
        RflSPrintf(aszWork, PRT_DFL_LINE * 2 + 1, aszPrtDflHeader, aszTableMnem, aszTableNo, aszPersonMnem, aszNoPerson, aszGuestMnem, aszGuestNo);
    }

    /* -- fill space with kp column number -- */ /* if space fills header area, kpmanager doesnot print header */
    PrtFillSpace(aszWork, PRT_DFL_LINE * 2);
    _tcsncpy(pszWork, aszWork, PRT_DFL_LINE);
    *(pszWork + PRT_DFL_LINE) = PRT_SPACE;
    pszWork += PRT_DFL_LINE + 1;
    _tcsncpy(pszWork, aszWork + PRT_DFL_LINE, PRT_DFL_LINE);                   
    return(2);

}

/*
*===========================================================================
** Format  : USHORT   PrtDflTrailer(UCHAR *pszWork, TRANINFORMATION *pTran, ULONG  ulStReg);      
*
*   Input  : UCHAR           *pszWork   -display data buffer pointer
*   Input  : TRANINFORMATION *pTran     -transaction information
*            ULONG           ulStReg    -store  reg number
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function set trailer.
*===========================================================================
*/
USHORT   PrtDflTrailer(TCHAR *pszWork, TRANINFORMATION *pTran, ULONG  ulStReg)
{
	TCHAR  aszMnem[PARA_SPEMNEMO_LEN + 1] = { 0 };
    ULONG  ulID;
	TCHAR  aszDate[PRT_DATETIME_LEN + 1] = { 0 };
	TCHAR  aszWork[PRT_DFL_LINE * 2 + 1] = { 0 };

    if ( PrtDflIf.Dfl.uchSeqNo != 0 ) {                 /* first frame ? */
        return(0);
    }
    /*  --  get cashier or waiter's name and mnemonics -- */
    if ( PRT_CASHIER == PrtChkCasWai(aszMnem, pTran->TranModeQual.ulCashierID, pTran->TranModeQual.ulWaiterID) ) {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulCashierID);
    } else {
        ulID = RflTruncateEmployeeNumber(pTran->TranModeQual.ulWaiterID);
    }

    PrtGetDate(aszDate, TCHARSIZEOF(aszDate), pTran);   /* get date time */

    RflSPrintf(aszWork, PRT_DFL_LINE * 2 + 1, aszPrtKPTrailer,  pTran->TranCurQual.usConsNo, aszMnem, ulID, ulStReg / 1000L, ulStReg % 1000L, aszDate); 
                   
    /* -- fill space with kp column number -- */
    PrtFillSpace(aszWork, PRT_DFL_LINE * 2);
                   
    _tcsncpy(pszWork, aszWork, PRT_DFL_LINE);                   
    *( pszWork + PRT_DFL_LINE ) = PRT_SPACE;
    pszWork += PRT_DFL_LINE + 1;
    _tcsncpy(pszWork, aszWork + PRT_DFL_LINE, PRT_DFL_LINE);                   
    return(2);

}

/*
*===========================================================================
** Format  : VOID   PrtDflSetCRTNo(TRANINFORMATION *pTran, ITEMSALES *pItem);      
*
*   Input  : TRANINFORMATION *pTran     -transaction information address
*            ITEMSALES *pItem           -item data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets destination CRT No. or remote printer number.
*===========================================================================
*/
VOID   PrtDflSetCRTNo(TRANINFORMATION *pTran, ITEMSALES *pItem)
{
	UCHAR       fbCRTNo1 = '0', fbCRTNo2 = '0';    /* destination CRT No., 1-4 and 5-8 */

    if ( PRT_KPITEM == PrtChkKPPort(pTran, pItem, &fbCRTNo1, PRT_NOT_MDC)) {
        
		fbCRTNo2 = (UCHAR)(fbCRTNo1 >> 4);
		fbCRTNo1 &= 0x0F;

        PrtDflIf.Dfl.DflHead.auchCRTNo[0] = PrtDflGetNo(fbCRTNo1);    /* set destination CRT No.1-4 */
        PrtDflIf.Dfl.DflHead.auchCRTNo[1] = PrtDflGetNo(fbCRTNo2);    /* set destination CRT No.1-4 */
    } else {
		//  specify a CRT number of character zero indicating no remote CRT or KDS.
        PrtDflIf.Dfl.DflHead.auchCRTNo[0] = '0';
        PrtDflIf.Dfl.DflHead.auchCRTNo[1] = '0';
    }
}

/*
*===========================================================================
** Format  : VOID   PrtDflSetTtlNo(TRANINFORMATION *pTran);      
*
*   Input  : TRANINFORMATION *pTran     -transaction information address
*            UCHAR     *puchCRTNo       -pointer of destination CRT No.
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets destination CRT No. according to total key status.
*===========================================================================
*/
VOID   PrtDflSetTtlNo(TRANINFORMATION *pTran)
{
    UCHAR uchCRTNo;
    
    uchCRTNo= 0;
    uchCRTNo |= (pTran->TranCurQual.auchTotalStatus[3] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK);

    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = PrtDflGetNo(uchCRTNo);   /* set destination CRT No. */

    /* ---- set CRT #5 - #8     R3.1 --- */
    uchCRTNo= 0;
    uchCRTNo |= (pTran->TranCurQual.auchTotalStatus[5] & CURQUAL_TOTAL_KITPRT_PRINTER_MASK);

    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = PrtDflGetNo(uchCRTNo);    /* set destination CRT No. */
}

/*
*===========================================================================
** Format  : UCHAR   PrtDflGetNo(UCHAR uchCRTNo);      
*
*   Input  : UCHAR  uchCRTNo     -destination CRT No.
*   Output : none
*   InOut  : none
** Return  : UCHAR converted CRT No from binary to text character.
*
** Synopsis: This function gets destination No. and convert it to ascii data.
*
*            The range of the result is a hex digit (0 - 9, A - F).
*===========================================================================
*/
UCHAR   PrtDflGetNo(UCHAR uchCRTNo)
{
    if ( uchCRTNo <= 9 ) {
        uchCRTNo += 0x30;   // add character zero ('0") for range '0' to '9'.
    } else {             
        uchCRTNo += 0x37;   // 10 + 0x37 = 0x40 or 'A' for range 'A' to 'F'.
    }

	return uchCRTNo;
}                
                
/*
*===========================================================================
** Format  : VOID   PrtDflTrType(TRANINFORMATION *pTran);      
*
*   Input  : TRANINFORMATION *pTran     -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function check and set transaction type.
*===========================================================================
*/
VOID   PrtDflTrType(TRANINFORMATION *pTran)
{
    
    if ( pTran->TranCurQual.fsCurStatus & CURQUAL_PRESELECT_MASK ) {
		/* -- preselect void? -- */
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAINING) {
            PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_TRAIN_VOID;
        } else {
            PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_NORMAL_VOID;
        }    
    } else {
		/* -- training mode -- */    
        if ( pTran->TranCurQual.fsCurStatus & CURQUAL_TRAINING) {
            PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_TRAIN;
        } else {    
            PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_NORMAL;
        }             
    }

    /* save transaction type */
    uchPrtDflTypeSav = PrtDflIf.Dfl.DflHead.auchItemType[0];
}

/*
*===========================================================================
** Format  : VOID   PrtDflCheckVoid(VOID *pItem);      
*
*   Input  : VOID *pItem     -item data address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function modifies transaction type if item void is on.
*===========================================================================
*/
VOID   PrtDflCheckVoid(USHORT  fbMod)
{
        
    if ( !( fbMod & VOID_MODIFIER) ) {
        return;
    }        

    switch ( PrtDflIf.Dfl.DflHead.auchItemType[0] ) {
        
    case DFL_TRAIN_VOID:
        PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_TRAIN;        
        break;

    case DFL_NORMAL_VOID:
        PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_NORMAL;        
        break;

    case DFL_TRAIN:
        PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_TRAIN_VOID;        
        break;

    case DFL_NORMAL:
        PrtDflIf.Dfl.DflHead.auchItemType[0] = DFL_NORMAL_VOID;        
        break;
    }
}    
        
/*
*===========================================================================
** Format  : VOID   PrtDflIType(USHORT usLineNo, UCHAR uchItemType);
*
*   Input  : USHORT usLineNo     -No. of lines to be displayed.
*            UCHAR  uchItemType  -Item data type
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets line No. and item type. 
*===========================================================================
*/
VOID   PrtDflIType(USHORT usLineNo, UCHAR uchItemType)
{
	TCHAR aszLineNo[4] = { 0 };
    USHORT usLen;

    if ( PrtDflIf.Dfl.uchSeqNo == 0 ) {
        usLineNo -= 2;                /* header, trailer 1 line = 42 char */
    }

    /* -- set number of lines to be displayed -- */
    _itot(usLineNo, aszLineNo, 10);
    usLen = _tcslen(aszLineNo);
    tcharnset(PrtDflIf.Dfl.DflHead.aszLineNo, _T('0'), PRT_DFL_LINELEN - usLen);
    _tcsncpy(&PrtDflIf.Dfl.DflHead.aszLineNo[PRT_DFL_LINELEN - usLen], aszLineNo, usLen);

    /* -- set item data type -- */
    PrtDflIf.Dfl.DflHead.auchItemType[1] = uchItemType;    

    /* -- set sequence No. -- */
    PrtDflIf.Dfl.uchSeqNo ++;

    if ( PrtDflIf.Dfl.uchSeqNo == 1 ) {                 /* first frame ? */
        PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_FIRST_FRAME;
    } else {
        PrtDflIf.Dfl.DflHead.uchFrameType = PRT_DFL_NOT_FIRST_FRAME;
    }

}    
    
/*
*===========================================================================
** Format  : VOID   PrtDflSetData(UCHAR *pszData, USHORT *pusOff);
*
*   Input  : UCHAR  *pszData     -display 1 line data.
*
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets display data. 
*===========================================================================
*/
VOID   PrtDflSetData(TCHAR *pszData, USHORT *pusOff)
{
    USHORT  usLen;

    /* -- get 1 line data length -- */
    usLen = _tcslen(pszData);

    if ( usLen > PRT_DFL_LINE ) {
        /* -- set display data -- */
        _tcsncpy(&PrtDflIf.auchDispData[*pusOff], pszData, PRT_DFL_LINE);
        *pusOff += PRT_DFL_LINE;
        pszData += PRT_DFL_LINE + 1;
        usLen -= ( PRT_DFL_LINE + 1);
        _tcsncpy(&PrtDflIf.auchDispData[*pusOff], pszData, usLen);
        *pusOff += usLen;
        return;
    }                

    /* -- set display data -- */
    _tcsncpy(&PrtDflIf.auchDispData[*pusOff], pszData, usLen);
    *pusOff += usLen;
    PrtDflIf.auchDispData[*pusOff] = PRT_RETURN;    
    (*pusOff)++;
}

/*
*===========================================================================
** Format  : VOID   PrtDflSend(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets VLI. 
*===========================================================================
*/
VOID   PrtDflSend(VOID)
{
    USHORT  usLen, usDataLen, usTtlLen;
    TCHAR   aszVLI[4];

    /* -- set data end indicater -- */
    usDataLen = _tcslen(PrtDflIf.auchDispData);
    PrtDflIf.auchDispData[usDataLen] = DFL_ETX;        
    usTtlLen = sizeof(PrtDflIf.Dfl) + usDataLen + 2;    /* '+2' is for ETX, XOR area */    
    
    /* -- set VLI  -- */
    memset(aszVLI, '\0', sizeof(aszVLI));    
    _itot(usTtlLen-1, aszVLI, 10);    
    usLen = _tcslen(aszVLI);
    tcharnset(PrtDflIf.Dfl.DflHead.aszVLI, _T('0'), PRT_DFL_VLILEN - usLen);
    _tcsncpy(&PrtDflIf.Dfl.DflHead.aszVLI[PRT_DFL_VLILEN - usLen], aszVLI, usLen);

    /* -- set DFL message sequencial no. (1st=0x31, retry=0x32,0x33...) -- */
    /* --  increments at client module (4/15/96 FVT comment, V3.01.04)  -- */
    PrtDflIf.Dfl.DflHead.uchDflMsgSeqNo = DFL_MSGSEQNO_INIT;

    /* -- set XOR result of display data ( not include Xor area ) -- */
    PrtDflIf.auchDispData[usDataLen + 1] = PrtDflXor((VOID *)&PrtDflIf, (USHORT)(usTtlLen-1));    /* ### Mod (2171 for Win32) V1.0 */

    /* -- set VLI  -- */
    memset(aszVLI, '\0', sizeof(aszVLI));    
    _itot(usTtlLen, aszVLI, 10);    
    usLen = _tcslen(aszVLI);
    tcharnset(PrtDflIf.Dfl.DflHead.aszVLI, _T('0'), PRT_DFL_VLILEN - usLen);
    _tcsncpy(&PrtDflIf.Dfl.DflHead.aszVLI[PRT_DFL_VLILEN - usLen], aszVLI, usLen);

    /* -- send -- */
    if ( DFL_PERFORM == CliDflPrint(&PrtDflIf, usTtlLen) ) {

        fbPrtDflErr &= ~PRT_ALREADY_DISP;       /* reset status */

    } else {

        if (!(fbPrtDflErr & PRT_ALREADY_DISP)) {
            UieErrorMsg(LDT_DFLERR_ADR, UIE_WITHOUT);
                                                /* display error */    
            fbPrtDflErr |= PRT_ALREADY_DISP;    /* set status */
        }
    }

    /* -- clear display data area -- */
    memset(PrtDflIf.auchDispData, '\0', sizeof(PrtDflIf.auchDispData) );

    /* restore transaction type */
    PrtDflIf.Dfl.DflHead.auchItemType[0] = uchPrtDflTypeSav;

}

/*
*===========================================================================
** Format  : UCHAR   PrtDflXor(VOID *puchStart, USHORT usLen);
*
*   Input  : VOID   *puchStart  -data start address
*            USHORT usLen       -display data length
*   Output : none
*   InOut  : none
** Return  : CHAR  fbXor;       -result of the XOR 
*
** Synopsis: This function executes exclusive OR of display data and 
*            returns the result. 
*===========================================================================
*/
UCHAR   PrtDflXor(VOID *puchStart, USHORT usLen)
{
    USHORT  usI;
	USHORT  uchSkip = sizeof(PrtDflIf.Dfl.DflHead);
	UCHAR   uchXOR = 0;
	UCHAR   *puchBlock = puchStart;

    for ( usI = 0, uchXOR = 0; usI < usLen; usI ++, puchBlock ++ ) {   
    
        if ( usI == uchSkip ) { 
            continue;
        }
    
        uchXOR ^= *puchBlock;     
    }

    return(uchXOR);
}

/* ===== End of File ( PrDDfl_.C ) ===== */
