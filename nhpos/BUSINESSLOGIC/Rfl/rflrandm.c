/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170 GP  *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1996            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reentrant Functions Library : LABEL CONVERTION FUNCTION
* Category    : Rfl Module, NCR 2170 GP R2.0 Model Application
* Program Name: RFLRANDM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: This function is LABEL CONVERTION.
*
*           The provided function names are as follows:
*
*       LaInst_GetMDCAdr();         R2.0
*       LaInst_CheckType();
*       LaInst_CheckEANRandom();    R2.0
*       LaInst_CheckLabel();
*       LaInst_CheckLabelA():
*       LaInst_CheckLabelB();
*       LaInst_CheckLabelC();       R2.0
*       LaInst_pr4cd();
*       LaInst_pr5cd();
*
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Version  :   Name     : Description
* Apr-01-93 : 00.00.01 : M.Suzuki   : Initial
* Oct-15-96 : 02.00.00 : S.Kubota   : Change for Random Weight Label
*           :          :            : Move from Int'l R1.0 for Random Weight Label(20-25)
*           :          :                LaInst_CheckEANRandom()(), LaInst_CheckLabelC()
* Nov-25-96 : 02.00.00 : hrkato     : R2.0
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

/*------------------------------------------------------------------------*\

            I N C L U D E  F I L E S

\*------------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include    <stdlib.h>
#include    <ecr.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <rfl.h>
#include    <pif.h>
#include    "rfllabel.h"

/**************************************************************
 *    EXTERNAL FUNCTION
*/

/**************************************************************
 *    EXTERNAL DATA
 */

/**************************************************************
 *    STATIC DATA
 */

/*****************************************************************************
*   EAN-13 RANDOM WEIGHT LABEL
*           ( F1 F2 X1 X2 X3 X4 X5 X6 X7 X8 X9 XA C2 )  F1F2 = 02,04,20-29
*
*                                                                M D C
*   Group A : Type 1-5 (On Mass Memory)                     7 6 5 4 3 2 1 0
*
*       Type 1: F1 F2 X1 X2 X3 X4 X5 X6 X7 X8 X9 XA C2      0 0 0 0 0 0 0 0
*                     <----------- A/N ----------->
*
*       Type 2: F1 F2 X1 X2 X3 X4 X5 C1 Y1 Y2 Y3 Y4 C2      0 0 0 $ $ $ 0 0
*                     <--- A/N ---->    <-- V/F -->
*
*       Type 3: F1 F2 X1 X2 X3 X4 C1 Y1 Y2 Y3 Y4 Y5 C2      0 0 0 $ $ $ 0 1
*                     <-- A/N -->    <--- V/F ---->
*
*       Type 4: F1 F2 X1 X2 X3 X4 X5 Y1 Y2 Y3 Y4 Y5 C2      0 0 0 $ $ $ 1 1
*                     <--- A/N ----> <--- V/F ---->
*
*       Type 5: F1 F2 X1 X2 X3 X4 X5 X6 Y1 Y2 Y3 Y4 C2      0 0 0 $ $ $ 1 0
*                     <----- A/N -----> <-- V/F -->
*
*   Group B : Type 6-12 (Read Only Label)
*
*       Type 6: F1 F2 X1 X2 X3 X4 00 C1 Y1 Y2 Y3 Y4 C2      1 0 0 X X X X X
*                     <- DEPT# ->       <- PRICE ->
*
*       Type 7: F1 F2 X1 X2 X3 X4 C1 Y1 Y2 Y3 Y4 Y5 C2      0 1 0 X X X X X
*                     <- DEPT# ->    <-- PRICE --->
*
*       Type 8: F1 F2 X1 X2 X3 X4 X5 Y1 Y2 Y3 Y4 Y5 C2      1 1 0 X X X X X
*                     <-- DEPT# ---> <-- PRICE --->
*
*       Type 9: F1 F2 X1 X2 X3 X4 X5 X6 X7 X8 X9 XA C2      0 0 1 X X X X X
*                     <--------- SKU NO ---------->
*
*       Type10: F1 F2 X1 X2 X3 X4 X5 Y1 Y2 Y3 Y4 Y5 C2      1 0 1 X X X X X
*                     <--- SKU# ---> <-- PRICE --->
*
*       Type11: F1 F2 X1 X2 X3 X4 X5 X6 X7 X8 X9 XA C2      0 1 1 X X X X X
*                     <------- CONSUMER NO ------->
*
*       Type12: F1 F2 X1 X2 X3 X4 X5 X6 X7 X8 X9 XA C2      1 1 1 X X X X X
*                     <------- ACCOUNT NO -------->
*
*               A/N = Article Number        X = 0 or 1
*               V/F = Variable Number       $$$ =  001,010,100
*
*****************************************************************************/

/*fhfh
***************************************************************************
**    Synopsis : USHORT _RflInst(LABELANALYSIS *pData)
*
*     in/out   : pData         Addr. of look up plu# & data.
**    Return   : LABEL_OK   : success
*                LABEL_ERR  : ERROR
**    Description : Instore label conversion                    R2.0
***************************************************************************
fhfh*/
SHORT _RflInst(LABELANALYSIS *pData)
{
    UCHAR   uchType;            /* Random Weight Label Type */
    UCHAR   uchMDC;
    USHORT  usAddr;                                     /* R2.0 96.11.05 */
    PARAMDC MDCRcvBuff;                                 /* R2.0 96.11.05 */

    usAddr = LaInst_GetMDCAdr (&pData->aszLookPlu[0]);   /* R2.0 96.11.05 */

    MDCRcvBuff.uchMajorClass = CLASS_PARAMDC;
    MDCRcvBuff.usAddress    = usAddr;
    CliParaRead(&MDCRcvBuff);
    uchMDC = MDCRcvBuff.uchMDCData;
    
    uchType = 0;
#if 0
    /* --- Check Frequent Shopper Label --- */
    LaInst_CheckFreq(pData->aszLookPlu, &uchType);
    if (uchType == 0) {
        if (LaInst_CheckType(uchMDC, &uchType)) {
            return LABEL_ERR;
        }
        LaInst_CheckEANRandom(usAddr, &uchType);        /* R2.0 96.11.05 */
    }
#endif

    if (LaInst_CheckType(uchMDC, &uchType)) {
        return LABEL_ERR;
    }
    if (LaInst_CheckLabel(pData, uchType, uchMDC)) {
        return LABEL_ERR;
    }

    if ( ( uchType <= TYPE_05 ) ||
         ( uchType >= TYPE_20 && uchType <= TYPE_25 ) ) {     /* R2.0 96.11.05 */
		/* Group-A : type 1-5   */

        pData->aszMaskLabel[STD_PLU_NUMBER_LEN - 1] = LaMain_plucd(pData->aszMaskLabel);                         /* set Mask Label       */
        pData->uchLookup = LA_EXE_LOOKUP;

    } else {
		/* Group-B : type 6-12  */
        pData->uchLookup = LA_NOT_LOOKUP;
    }

    return(LABEL_OK);
}

/*fhfh
***************************************************************************
*
**    Synopsis : VOID   LaInst_GetMDCAdr(TCHAR* aszLookPlu)
*
*     input    : TCHAR* aszLookPlu      PLU Label
*
*     output   : None
*
**    Return   : USHORT                 MDC Address
*
**    Description : READ MDC OF TARGET RANDOM WEIGHT PLU
*
*                   This is reading the MDC controlling the PLU Label
*                   Analysis. The MDC is actually a Low/High or Odd/Even
*                   consecutive pair of MDC addresses.
*
*                   The second and third digits of the PLU label are used to
*                   determine the MDC address to use.
***************************************************************************
fhfh*/
USHORT LaInst_GetMDCAdr(TCHAR* aszLookPlu)
{
    USHORT      usAddr = 0;

    switch (aszLookPlu[1]) {
    case _T('0'):
        switch(aszLookPlu[2]) {
        case _T('2'):
            usAddr = MDC_PLU02;  // MDC_PLULB02L_ADR
            break;
        case _T('4'):
            usAddr = MDC_PLU04;  // MDC_PLULB04L_ADR
            break;
        default:
            usAddr = 0;
            break;
        }
        break;

    case _T('2'):
        switch(aszLookPlu[2]) {
        case _T('0'):
            usAddr = MDC_PLU20;  // MDC_PLULB20L_ADR
            break;
        case _T('1'):
            usAddr = MDC_PLU21;  // MDC_PLULB21L_ADR
            break;
        case _T('2'):
            usAddr = MDC_PLU22;  // MDC_PLULB22L_ADR
            break;
        case _T('3'):
            usAddr = MDC_PLU23;  // MDC_PLULB23L_ADR
            break;
        case _T('4'):
            usAddr = MDC_PLU24;  // MDC_PLULB24L_ADR
            break;
        case _T('5'):
            usAddr = MDC_PLU25;  // MDC_PLULB25L_ADR
            break;
        case _T('6'):
            usAddr = MDC_PLU26;  // MDC_PLULB26L_ADR
            break;
        case _T('7'):
            usAddr = MDC_PLU27;  // MDC_PLULB27L_ADR
            break;
        case _T('8'):
            usAddr = MDC_PLU28;  // MDC_PLULB28L_ADR
            break;
        case _T('9'):
            usAddr = MDC_PLU29;  // MDC_PLULB29L_ADR
            break;
        default:
            usAddr = 0;
            break;
        }
        break;

    default:
        usAddr = 0;
        break;
    }

    return(usAddr);
}

/*fhfh
***************************************************************************
**    Synopsis : SHORT  LaInst_CheckType(UCHAR uchMDC, UCHAR *uchType)
*
*     input    : UCHAR  uchMDC
*     output   : UCHAR  *uchType
**    Return   : LABEL_OK   : success
*                LABEL_ERR  : ERROR
**    Description:
***************************************************************************
fhfh*/
SHORT   LaInst_CheckType(UCHAR uchMDC, UCHAR *uchType)
{
    if ((uchMDC & MASK_GROUPB) == 0) {      /* Group A : type 1-5   */
        switch (uchMDC & MASK_GROUPA) {
        case MDC_TYPE_02:
            *uchType = TYPE_02;
            break;

        case MDC_TYPE_03:
            *uchType = TYPE_03;
            break;

        case MDC_TYPE_04:
            *uchType = TYPE_04;
            break;

        case MDC_TYPE_05:
            *uchType = TYPE_05;
            break;
        }
        switch (uchMDC & MASK_SELL) {
        case SEL_NON:
            *uchType = TYPE_01;
            break;

        case SEL_PRC:
            break;

        case SEL_WGT:
            break;

        case SEL_QTY:
            break;

        default:
            return LABEL_ERR;
        }

    } else {                                    /* Group B : type 6-12  */
        switch (uchMDC & MASK_GROUPB) {
        case MDC_TYPE_06:
            *uchType = TYPE_06;
            break;

        case MDC_TYPE_07:
            *uchType = TYPE_07;
            break;

        case MDC_TYPE_08:
            *uchType = TYPE_08;
            break;

        case MDC_TYPE_09:
            *uchType = TYPE_09;
            break;

        case MDC_TYPE_10:
            *uchType = TYPE_10;
            break;

        case MDC_TYPE_11:
            *uchType = TYPE_11;
            break;

        case MDC_TYPE_12:
            *uchType = TYPE_12;
            break;
        }
    }

    return(LABEL_OK);
}

#if 0
/*fhfh
***************************************************************************
*
**    Synopsis : VOID   LaInst_CheckEANRandom(UCHAR uchAddr, UCHAR *uchType)
*
*     input    : UCHAR  uchAddr
*
*     output   : UCHAR  *uchType
*
**    Return   : none
*
**    Description :
*
***************************************************************************
fhfh*/
VOID    LaInst_CheckEANRandom(USHORT usAddr, UCHAR *uchType)
{
    if ( usAddr == MDC_PLU20
        && CliParaMDCCheck( MDC_PLULABEL1_ADR, ODD_MDC_BIT0 ) ) {

        *uchType = TYPE_20;

    } else if ( usAddr == MDC_PLU21
        && CliParaMDCCheck( MDC_PLULABEL1_ADR, ODD_MDC_BIT1 ) ) {

        *uchType = TYPE_21;

    } else if ( usAddr == MDC_PLU22
        && CliParaMDCCheck( MDC_PLULABEL1_ADR, ODD_MDC_BIT2 ) ) {

        *uchType = TYPE_22;

    } else if ( usAddr == MDC_PLU23
        && CliParaMDCCheck( MDC_PLULABEL1_ADR, ODD_MDC_BIT3 ) ) {

        *uchType = TYPE_23;

    } else if ( usAddr == MDC_PLU24
        && CliParaMDCCheck( MDC_PLULABEL2_ADR, EVEN_MDC_BIT0 ) ) {

        *uchType = TYPE_24;

    } else if ( usAddr == MDC_PLU25
        && CliParaMDCCheck( MDC_PLULABEL1_ADR, EVEN_MDC_BIT1 ) ) {

        *uchType = TYPE_25;

    }
}
#endif

/*fhfh
***************************************************************************
**    Synopsis : SHORT  LaInst_CheckLabel(LABELANALYSIS *pData,
*                                         UCHAR uchType, UCHAR uchMDC);
*     input    : UCHAR  uchType
*                UCHAR  uchMDC
*     in/out   : LABELANALYSIS  *pData
**    Return   : LABEL_OK       sucess
*                LABEL_ERR      error
**    Description : Instore label conversion                    R2.0
***************************************************************************
fhfh*/
SHORT   LaInst_CheckLabel(LABELANALYSIS *pData, UCHAR uchType, UCHAR uchMDC)
{
    switch ( uchType ) {
    case TYPE_01:                                   /* Article Number       */
    case TYPE_02:                                   /* A/N + Variable Field */
    case TYPE_03:                                   /* A/N + Variable Field */
    case TYPE_04:                                   /* A/N + Variable Field */
    case TYPE_05:                                   /* A/N + Variable Field */
        if (LaInst_CheckLabelA(pData, uchType, uchMDC))
            return LABEL_ERR;
        break;

    case TYPE_06:                                   /* Dept # + Price       */
    case TYPE_07:                                   /* Dept # + Price       */
    case TYPE_08:                                   /* Dept # + Price       */
        if (LaInst_CheckLabelB(pData, uchType)) {
            return LABEL_ERR;
        }
        break;

    case TYPE_09:                                   /* SKU Number           */
        memmove (pData->aszLabelNumber, &pData->aszLookPlu[3], LA_LEN_NUMBER * sizeof(TCHAR));
        pData->fsBitMap |= LA_F_SKU_NO;
        break;

    case TYPE_10:                                   /* SKU # + Price        */
        memmove (pData->aszLabelNumber, &pData->aszLookPlu[3], LA_LEN_SKU5 * sizeof(TCHAR));
        memmove (pData->aszPrice, &pData->aszLookPlu[8], LA_LEN_PRICE5 * sizeof(TCHAR));
        pData->fsBitMap |= (LA_F_SKU_NO | LA_F_PRICE);
        break;

    case TYPE_11:                                   /* Consumer Number      */
        memmove (pData->aszLabelNumber, &pData->aszLookPlu[3], LA_LEN_NUMBER * sizeof(TCHAR));
        pData->fsBitMap |= LA_F_CONS_NO;
        break;

    case TYPE_12:                                   /* Account Number       */
        memmove (pData->aszLabelNumber, &pData->aszLookPlu[3], LA_LEN_NUMBER * sizeof(TCHAR));
        pData->fsBitMap |= LA_F_NUMBER;
        break;                                      /* R2.0 96.11.05 */
                                                    /* R2.0 Start */
    case TYPE_20:                                   /* price (xx.xx)        */
    case TYPE_21:                                   /* price (xxx.x)        */
    case TYPE_22:                                   /* price (xxxx)         */
    case TYPE_23:                                   /* weight (x.xxx)       */
    case TYPE_24:                                   /* weight (xx.xx)       */
    case TYPE_25:                                   /* weight (xxx.x)       */
        LaInst_CheckLabelC(pData, uchType);
        break;
                                                    /* R2.0 End */
    case TYPE_FREQ:                                 /* Freq. Shopper#, R2.0 */
        memmove (pData->aszLabelNumber, &pData->aszLookPlu[3], LA_LEN_NUMBER * sizeof(TCHAR));
        pData->fsBitMap |= LA_F_FREQ;
        break;
    }
    return(LABEL_OK);
}

/*fhfh
***************************************************************************
*
**    Synopsis : SHORT  LaInst_CheckLabelA(LABELANALYSIS *pData,
*                                          UCHAR uchType, UCHAR uchMDC);
*
*     input    : UCHAR  uchType
*                UCHAR  uchMDC
*
*     in/out   : LABELANALYSIS  *pData
*
**    Return   : LABEL_OK       sucess
*                LABEL_ERR      error
*
**    Description : Instore label conversion
*
***************************************************************************
fhfh*/
SHORT  LaInst_CheckLabelA(LABELANALYSIS *pData, UCHAR uchType, UCHAR uchMDC)
{
    USHORT  usNumberLen;

    /*--- Convert Variable Number to Long Data ---*/
    switch (uchType) {
		case TYPE_02:
		case TYPE_05:
			memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
			break;

		case TYPE_03:
		case TYPE_04:
			memmove (pData->aszPrice, &pData->aszLookPlu[8], LA_LEN_PRICE5 * sizeof(TCHAR));
			break;

		default:
			break;
    }

    /*--- Check CD of Variable Number ---*/
    if (uchType == TYPE_02) {
        if (pData->aszLookPlu[8] != LaInst_pr4cd(&pData->aszLookPlu[9])) {
            return LABEL_ERR;
        }
    } else if (uchType == TYPE_03) {
        if (pData->aszLookPlu[7] != LaInst_pr5cd(&pData->aszLookPlu[8])) {
            return LABEL_ERR;
        }
    }

    /*--- Check Variable Number : Price/Weight/Qty ---*/
    switch (uchMDC & MASK_SELL) {
		case SEL_NON:
			pData->fsBitMap |= LA_F_ARTINO;
			break;

		case SEL_PRC:
			pData->fsBitMap |= (LA_F_ARTINO | LA_F_PRICE);
			break;

		case SEL_WGT:
			pData->fsBitMap |= (LA_F_ARTINO | LA_F_WEIGHT);
			break;

		case SEL_QTY:
			pData->fsBitMap |= (LA_F_ARTINO | LA_F_QUANTITY);
			break;

		default:
			break;
    }

    /*--- Mask Variable Number ---*/
    switch (uchType) {
		case TYPE_01:
			usNumberLen = STD_PLU_NUMBER_LEN;
			break;

		case TYPE_02:
		case TYPE_04:
			usNumberLen = 7;
			break;

		case TYPE_03:
			usNumberLen = 6;
			break;

		case TYPE_05:
			usNumberLen = 8;
			break;

		default:
			usNumberLen = 0;
			break;
    }
    tcharnset (pData->aszMaskLabel, _T('0'), STD_PLU_NUMBER_LEN);
    memmove (pData->aszMaskLabel, pData->aszLookPlu, usNumberLen * sizeof(TCHAR));

    return LABEL_OK;
}

/*fhfh
***************************************************************************
*
**    Synopsis : SHORT  LaInst_CheckLabelB(LABELANALYSIS *pData,
*                                          UCHAR uchType);
*
*     input    : UCHAR  uchType
*
*     in/out   : LABELANALYSIS  *pData
*
**    Return   : LABEL_OK       sucess
*                LABEL_ERR      error
*
**    Description : Instore label conversion
*
***************************************************************************
fhfh*/
SHORT  LaInst_CheckLabelB(LABELANALYSIS *pData, UCHAR uchType)
{
    switch(uchType) {
    case TYPE_06:
        if ( pData->aszLookPlu[8] != LaInst_pr4cd(&pData->aszLookPlu[9]) ) {
            return LABEL_ERR;
        }
        memmove (pData->aszDept,  &pData->aszLookPlu[3], LA_LEN_DEPT4 * sizeof(TCHAR));
        memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
        break;

    case TYPE_07:
        if ( pData->aszLookPlu[6] != LaInst_pr5cd(&pData->aszLookPlu[7]) ) {
            return LABEL_ERR;
        }
        memmove (pData->aszDept,  &pData->aszLookPlu[3], LA_LEN_DEPT4 * sizeof(TCHAR));
        memmove (pData->aszPrice, &pData->aszLookPlu[8], LA_LEN_PRICE5 * sizeof(TCHAR));
        break;
/*  case TYPE_08:   */
    default:
        memmove (pData->aszDept,  &pData->aszLookPlu[3], LA_LEN_DEPT5 * sizeof(TCHAR));
        memmove (pData->aszPrice, &pData->aszLookPlu[8], LA_LEN_PRICE5 * sizeof(TCHAR));
    }
    pData->fsBitMap |= (LA_F_PRICE | LA_F_DEPT);
    return LABEL_OK;
}

/*fhfh
***************************************************************************
*
**    Synopsis : VOID   LaInst_CheckLabelC(LABELANALYSIS *pData,
*                                          UCHAR uchType, UCHAR uchMDC);
*
*     input    : UCHAR  uchType
*                UCHAR  uchMDC
*
*     in/out   : LABELANALYSIS  *pData
*
**    Return   : LABEL_OK       sucess
*                LABEL_ERR      error
*
**    Description : Instore label conversion
*
***************************************************************************
fhfh*/
VOID    LaInst_CheckLabelC(LABELANALYSIS *pData, UCHAR uchType)
{
    USHORT  usNumberLen;
    SYSCONFIG CONST *pSysConfig = PifSysConfig();

    /*--- Convert Variable Number to Long Data ---*/
    /*--- Mask Variable Number ---*/
    switch (uchType) {
    case TYPE_20:
        pData->fsBitMap |= (LA_F_ARTINO | LA_F_PRICE);
        if (pSysConfig->ausOption[1] == 1) {            /* 3 decimal position ? */
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
            pData->aszPrice[4] = _T('0');
        } else if (pSysConfig->ausOption[1] == 2) {     /* no decimal position ? */
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE2 * sizeof(TCHAR));
        } else {
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
        }
        break;

    case TYPE_21:
        pData->fsBitMap |= (LA_F_ARTINO | LA_F_PRICE);
        if (pSysConfig->ausOption[1] == 1) {            /* 3 decimal position ? */
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
            pData->aszPrice[4] = _T('0');
            pData->aszPrice[5] = _T('0');
        } else if (pSysConfig->ausOption[1] == 2) {     /* no decimal position ? */
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE3 * sizeof(TCHAR));
        } else {
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
            pData->aszPrice[4] = _T('0');
        }
        break;

    case TYPE_22:
        pData->fsBitMap |= (LA_F_ARTINO | LA_F_PRICE);
        if (pSysConfig->ausOption[1] == 1) {            /* 3 decimal position ? */
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
            pData->aszPrice[4] = _T('0');
            pData->aszPrice[5] = _T('0');
            pData->aszPrice[6] = _T('0');
        } else if (pSysConfig->ausOption[1] == 2) {     /* no decimal position ? */
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
        } else {
            memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
            pData->aszPrice[4] = _T('0');
            pData->aszPrice[5] = _T('0');
        }
        break;

    case TYPE_23:
        pData->fsBitMap |= (LA_F_ARTINO | LA_F_WEIGHT);
        memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
        break;

    case TYPE_24:
        pData->fsBitMap |= (LA_F_ARTINO | LA_F_WEIGHT);
        memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
        pData->aszPrice[4] = _T('0');
        break;

    case TYPE_25:
        pData->fsBitMap |= (LA_F_ARTINO | LA_F_WEIGHT);
        memmove (pData->aszPrice, &pData->aszLookPlu[9], LA_LEN_PRICE4 * sizeof(TCHAR));
        pData->aszPrice[4] = _T('0');
        pData->aszPrice[5] = _T('0');
        break;
    }

    usNumberLen = 8;

    tcharnset (pData->aszMaskLabel, _T('0'), STD_PLU_NUMBER_LEN);
    //memset(pData->aszMaskLabel,'0',13);
    memmove (pData->aszMaskLabel, pData->aszLookPlu, usNumberLen * sizeof(TCHAR));
}

/*fhfh
***************************************************************************
*
**    Synopsis : TCHAR LaInst_pr4cd(TCHAR *price)
*
*     input    : TCHAR  *price
*
**    Return   : TCHAR  check digit
*
**    Description : GENERATE 4 DIGIT PRICE CHECK DIGIT
*
*       0   1   2   3   (array)
*     +---------------+
*     |   |   |   |   | (price; ascii data)
*     +---------------+
*       2-  2-  3   5-  (weight)
*       Return data is ascii check digit
***************************************************************************
fhfh*/
TCHAR LaInst_pr4cd(TCHAR *price)
{
    SHORT temp,cal;
                                                /* STEP 1/2                 */
    temp = (price[0] & 0x0f) * 2;               /* for p1                   */
    cal  = temp - (temp / 10);

    temp = (price[1] & 0x0f) * 2;               /* for p2                   */
    cal += temp - (temp / 10);

    cal += ((price[2] & 0x0f) * 3) % 10;        /* for p3                   */

    temp = (price[3] & 0x0f) * 5;               /* for p4                   */
    cal += temp - (temp / 10);
                                                /* STEP 3/4                 */
    return((TCHAR)(((cal * 3) % 10) | 0x30));   /* low digit is cd          */
}

/*fhfh
***************************************************************************
*
**    Synopsis : TCHAR LaInst_pr5cd(TCHAR *price)
*
*     input    : TCHAR  *price
*
**    Return   : TCHAR  price check digit (ascii code)
*
**    Description : GENERATE 5 DIGIT PRICE CHECK DIGIT
*
*       0    1    2      3    4    (array)
*     +------------------------+
*     | P1 | P2 | P3 | P4 | P5 |  (price; ascii data)
*     +------------------------+
*       5+  2-  5-  5+  2-        (weight)
*     weight of cd = 5-
*     return data is price check digit (ascii code)
***************************************************************************
fhfh*/
TCHAR LaInst_pr5cd(TCHAR *price)
{
    SHORT temp,cal;
                                                /* STEP 1/2                 */
    temp = (price[0] & 0x0f) * 5;               /* for p1                   */
    cal  = (temp / 10) + (temp % 10);

    temp = (price[1] & 0x0f) * 2;               /* for p2                   */
    cal += (temp - (temp / 10)) % 10;

    temp = (price[2] & 0x0f) * 5;               /* for p3                   */
    cal += (temp - (temp / 10)) % 10;

    temp = (price[3] & 0x0f) * 5;               /* for p4                   */
    cal += (temp / 10) + (temp % 10);

    temp = (price[4] & 0x0f) * 2;               /* for p5                   */
    cal += (temp - (temp / 10)) % 10;
                                                /* STEP 3                   */
    cal = (10 - cal % 10) % 10;                 /* 10's complement          */

    switch (cal) {                              /* STEP 4                   */
    case 0:
        return (_T('0'));
    case 5:
        return (_T('1'));
    case 9:
        return (_T('2'));
    case 4:
        return (_T('3'));
    case 8:
        return (_T('4'));
    case 3:
        return (_T('5'));
    case 7:
        return (_T('6'));
    case 2:
        return (_T('7'));
    case 6:
        return (_T('8'));
    case 1:
    default:
        return (_T('9'));
    }
}

#if 0
/*
***************************************************************************
**    Synopsis : VOID   LaInst_CheckFreq(TCHAR *puchPLU, UCHAR *uchType)
*     input    :
*     output   :
**    Return   :
**    Description:                                      R2.0
***************************************************************************
*/
VOID    LaInst_CheckFreq(TCHAR *puchPLU, UCHAR *uchType)
{
    *uchType = 0;

    switch (puchPLU[0]) {
    case _T('0'):
        switch(puchPLU[1]) {
        case _T('2'):
            if (CliParaMDCCheck(MDC_PLULABEL3_ADR, ODD_MDC_BIT0)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('4'):
            if (CliParaMDCCheck(MDC_PLULABEL5_ADR, ODD_MDC_BIT3)) {
                *uchType = TYPE_FREQ;
            }
            break;

        default:
            break;
        }
        break;                      /* Add for Bug Fix 97/5/30 by Y.Sakuma */

    case _T('2'):
        switch(puchPLU[1]) {
        case _T('0'):
            if (CliParaMDCCheck(MDC_PLULABEL3_ADR, ODD_MDC_BIT1)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('1'):
            if (CliParaMDCCheck(MDC_PLULABEL3_ADR, ODD_MDC_BIT2)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('2'):
            if (CliParaMDCCheck(MDC_PLULABEL3_ADR, ODD_MDC_BIT3)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('3'):
            if (CliParaMDCCheck(MDC_PLULABEL4_ADR, EVEN_MDC_BIT0)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('4'):
            if (CliParaMDCCheck(MDC_PLULABEL4_ADR, EVEN_MDC_BIT1)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('5'):
            if (CliParaMDCCheck(MDC_PLULABEL4_ADR, EVEN_MDC_BIT2)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('6'):
            if (CliParaMDCCheck(MDC_PLULABEL4_ADR, EVEN_MDC_BIT3)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('7'):
            if (CliParaMDCCheck(MDC_PLULABEL5_ADR, ODD_MDC_BIT0)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('8'):
            if (CliParaMDCCheck(MDC_PLULABEL5_ADR, ODD_MDC_BIT1)) {
                *uchType = TYPE_FREQ;
            }
            break;

        case _T('9'):
            if (CliParaMDCCheck(MDC_PLULABEL5_ADR, ODD_MDC_BIT2)) {
                *uchType = TYPE_FREQ;
            }
            break;

        default:
            break;
        }

    default:
        break;
    }
}
#endif

/* --- End of Source --- */
