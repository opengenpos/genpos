/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170 GP  *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reentrant Functions Library : LABEL ANALYSIS FUNCTION
* Category    : Rfl Module, NCR 2170 GP 2.0 Application
* Program Name: RFLLABEL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: This function is LABEL ANALYSIS.
*
*           The provided function names are as follows:
*
*       LaMain_ChkModify();
*       LaMain_cvlbl();
*       LaMain_source();
*       LaMain_veloc();
*       LaMain_UpcE();
*       LaMain_plucd();
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Version  :   Name     : Description
* Apr-01-93 : 00.00.01 : M.Suzuki   : Initial
* May-24-93 : 00.00.02 : M.Suzuki   : Support NCR-7852,7870,7890 Scanner
* Jul-20-93 : 00.00.03 : T.Koike    : Add "RflConvScan2Manual" routine
* Oct-04-93 : 01.00.01 : M.Sugiyama : Fixed the UPC-A Label on 7852,7870,7890
*           :          :            : Scanner reading problem. (Changed
*           :          :            : "LaMain_PreCheck()" routine)
* Oct-15-96 : 02.00.00 : S.Kubota   : Change  for UPC Velocity Code
*           :          :            :
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
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>        /* R2.0 */
#include    <csstbpar.h>
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

SHORT  RflSpecialPluCheck (TCHAR *auchPLUNo)
{
	SHORT  sRetStatus = 0;  // assume that this is not a special PLU

    if ((_tcsncmp(auchPLUNo, MLD_NO_DISP_PLU_LOW, STD_PLU_NUMBER_LEN) >= 0) &&
        (_tcsncmp(auchPLUNo, MLD_NO_DISP_PLU_HIGH, STD_PLU_NUMBER_LEN) <= 0)) {
			sRetStatus = 1;  // indicate that this is a special PLU number
	}

	return sRetStatus;
}

/*fhfh
***************************************************************************
*
**    Synopsis : SHORT RflLabelAnalysis(LABELANALYSIS *pData)
*
*     input    : None
*     in/out   : LABELANALYSIS  *pData;
*     output   : None
*
**    Return   : LABEL_OK    : success
*                LABEL_ERR   : error
*
**    Description : PLU label analysis
*
***************************************************************************
fhfh*/
SHORT RflLabelAnalysis(LABELANALYSIS *pData)
{
    SHORT   sRet;                               /* return value             */
    TCHAR   aszScanPlu[STD_PLU_NUMBER_LEN+1];                   /* only PLU # (w/o A.B.C)   */
    SHORT   sScanLen;                           /* aszScanPlu's length      */

    /*--- Initialize pData ---*/
    memset(pData->aszLookPlu, 0, sizeof(LABELANALYSIS) -
			(sizeof(pData->aszScanPlu) + sizeof(pData->fchModifier)));

    sRet = LaMain_PreCheck(pData);              /* scanned PLU pre-check    */
    if (sRet != LABEL_OK) {
        return sRet;
    }

    sRet = LaMain_ChkModify(pData);             /* check Modifier key       */
    if (sRet != LABEL_OK) {
        return sRet;
    }

    memset(&aszScanPlu[0], 0, sizeof(aszScanPlu));  /* 0 clear (aszScanPlu) */
    switch (pData->aszScanPlu[0]) {
        case _T('A'):                                   /* UPC-A or EAN-13      */
            sScanLen = LEN_UPCA;                    /* set UPC-A/EAN-13 len */
            _tcsncpy(&aszScanPlu[0], &pData->aszScanPlu[1], sScanLen);
            break;
        case _T('B'):                                   /* EAN-8                */
            pData->uchType = LABEL_EAN8;            /* set EAN-8 temporary  */
            sScanLen = LEN_EAN8;                    /* set EAN-8 length     */
            _tcsncpy(&aszScanPlu[0], &pData->aszScanPlu[1], sScanLen);
            break;
        case _T('C'):                                   /* UPC-E                */
            pData->uchType = LABEL_UPCE;            /* set UPC-E temporary  */
            sScanLen = LEN_UPCE;                    /* set UPC-E length     */
            _tcsncpy(&aszScanPlu[0], &pData->aszScanPlu[1], sScanLen);
            break;
		case _T(']'):
			if(pData->aszScanPlu[1] == _T('e') && pData->aszScanPlu[2] == _T('0')){
				sScanLen = LEN_RSS14;
				_tcsncpy(&aszScanPlu[0], &pData->aszScanPlu[5], sScanLen);
			}else{
				return LABEL_ERR;
			}
			break;
        default:
            sScanLen = tcharlen(pData->aszScanPlu);   /* set aszScanPlu's len */
            if (sScanLen > STD_PLU_NUMBER_LEN) {                    /* error more than 14   */

				return LABEL_ERR;
            
			}else{

				/* 2172, not support velocity */
				if ( ( sScanLen <= 6 ) && !( pData->fchModifier & LA_EMOD ) ) {
					pData->fchModifier |= LA_UPC_VEL;   /* set velocity     */
					pData->fchModifier |= LA_VEL;
				}
#if 0
														 /* R2.0 Start */
				if ( 0 == CliParaMDCCheck( MDC_PLU5_ADR, EVEN_MDC_BIT3 ) ) {
					if ( ( sScanLen <= 6 ) && !( pData->fchModifier & LA_EMOD ) ) {
						pData->fchModifier |= LA_UPC_VEL;   /* set velocity     */
						pData->fchModifier |= LA_VEL;
					}
				} else {
					if ( ( sScanLen <= 7 ) && !( pData->fchModifier & LA_EMOD ) ) {
						pData->fchModifier |= LA_VEL;
					}
				}
#endif
				_tcsncpy(&aszScanPlu[0], &pData->aszScanPlu[0], sScanLen);
			}
                                                    /* R2.0 End */
    }
    sRet = LaMain_cvlbl(pData, &aszScanPlu[0], sScanLen);
                                                    /* convertion & return value  */
    if (sRet) {
        return sRet;
    } else if (pData->uchType == LABEL_RANDOM) {
        return (_RflInst(pData));
    } else if(pData->uchType == LABEL_RSS14VAR) {
		return (_RflRssVar(pData));
	} else{
        return LABEL_OK;
    }

}

/*fhfh
***************************************************************************
*
**    Synopsis : USHORT RflConvScan2Manual(UCHAR *aszDest, UCHAR *aszSrc)
*
*     input    : aszDest = Destination buffer
*                aszSrc  = Source buffer
*
*     in/out   :
*     output   :
*
**    Return   : 0                  : Non E-Version Label
*                MOD_STAT_EVERSION  : E-Version Label
*
**    Description : Convert Scanner Input String to Manual Entry Label
*
***************************************************************************
fhfh*/

USHORT RflConvScan2Manual(TCHAR *aszDest, TCHAR *aszSrc)
{
    USHORT  i;
    USHORT  usRet;

    switch(*aszSrc) {   /* check E-Version or not */
    case _T('C'):
    case _T('E'):
        usRet = MOD_STAT_EVERSION;
        break;

    default:
        usRet = 0;
        break;
    }
    for(i=0; *(aszSrc+i)!=_T('\0') ;i++) {
        if((_T('0') <= *(aszSrc+i)) && (*(aszSrc+i) <= _T('9')))
            break;
    }
    if(*(aszSrc+i) != _T('\0')) {
        _tcscpy(aszDest,aszSrc+i);
        if(*aszSrc == _T('C')) {  /* E-Version with Check Digit */
            i= tcharlen(aszDest);
            *(aszDest+i-1) = _T('\0');
        }
    }
    else
        *aszDest = _T('\0');

    return usRet;
}


/*fhfh
***************************************************************************
*
**    Synopsis : SHORT  LaMain_PreCheck(LABELANALYSIS *pData)
*
*     input    : LABELANARYSIS  pData
*
**    Return   : LABEL_OK    :success
*                LABEL_ERR   :error
*
**    Description : This function checks scanned PLU label.
*
***************************************************************************
fhfh*/
SHORT LaMain_PreCheck(LABELANALYSIS *pData)
{
    SHORT   sScanLen;                           /* aszScanPlu's length      */

    /* ----- check PLU is entered by Scanner or manual entry ? ----- */
    if (! (pData->fchModifier & LA_SCANNER)) {
        return LABEL_OK;
    }

    switch (pData->aszScanPlu[0]) {
        case _T('A'):                                   /* UPC-A or EAN/JAN-13  */
            sScanLen = tcharlen(pData->aszScanPlu);
            if (sScanLen == LEN_UPCA) {
                                                    /* Chg 10/4/93 M.Sugi */
                memmove(&pData->aszScanPlu[2], &pData->aszScanPlu[1], 12 * sizeof(TCHAR));
                pData->aszScanPlu[1] = _T('0');
            }
        /* no break  */
        case _T('B'):                                   /* EAN/JAN-8            */
        case _T('C'):                                   /* UPC-E                */
            break;

        case _T('E'):                                   /* UPC-E                */
            memmove(&pData->aszScanPlu[0], &pData->aszScanPlu[2], 6 * sizeof(TCHAR));
            pData->aszScanPlu[6] = 0;
            pData->fchModifier |= LA_EMOD;
            break;

        case _T('F'):                                   /* EAN-13 or EAN-8      */
            if (pData->aszScanPlu[1] != _T('F')) {
                pData->aszScanPlu[0] = _T('A');
            } else {
                memmove(&pData->aszScanPlu[1], &pData->aszScanPlu[2], 8 * sizeof(TCHAR));
                pData->aszScanPlu[0] = _T('B');
                pData->aszScanPlu[9] = 0;
            }
            break;

		case _T(']'):
			//check for RSS14 if Scanned and Mark As Such
			if((pData->aszScanPlu[1] == _T('e')) && (pData->aszScanPlu[2] == _T('0'))){
				pData->uchType = LABEL_RSS14;
			}else{
				return LABEL_ERR;
			}
			break;

        default:
            return LABEL_ERR;
    }
    return LABEL_OK;
}


/*fhfh
***************************************************************************
*
**    Synopsis : SHORT  LaMain_ChkModify(LABELANALYSIS *pData)
*
*     input    : LABELANARYSIS  pData
*
**    Return   : LABEL_OK    :success
*                LABEL_ERR   :error
*
**    Description : This function checks Modifier is OK or not.
*
***************************************************************************
fhfh*/
SHORT LaMain_ChkModify(LABELANALYSIS *pData)
{
    if ( pData->fchModifier & LA_EMOD ) {           /* E-MOD key press?     */

        if ( pData->fchModifier & LA_UPC_VEL ) {    /* R2.0 Start */

            return LABEL_ERR;                       /* error                */
        }                                           /* R2.0 End   */

        switch ( pData->aszScanPlu[0] ) {           /* by SCANNER?          */
            case 'A':
            case 'B':
            case 'C':
                return LABEL_ERR;                   /* error                */
            default:
                break;
        }
    }
    return LABEL_OK;                                /* modifier success     */
}

/*fhfh
***************************************************************************
*
**    Synopsis : USHORT LaMain_cvlbl(LABELANALYSIS *pData,
*                                    UCHAR *aszScanPlu, SHORT sScanLen)
*
*     input    : aszScanPlu    only PLU# (exclude 'A','B','C')
*                sScanLen      aszScanPlu's length
*     in/out   : LABELANALYSIS  *pData
*
**    Return   : LABEL_OK    : success
*                LABEL_ERR   : error
*
**    Description : This function checks Modifier and PLU label conversion.
*
***************************************************************************
fhfh*/
USHORT LaMain_cvlbl(LABELANALYSIS *pData, TCHAR *aszScanPlu, SHORT sScanLen)
{
    TCHAR   uchCd;
    SHORT   sRet;
    TCHAR   uchSaveCd;

    tcharnset(pData->aszLookPlu, _T('0'), STD_PLU_NUMBER_LEN);
                                                /* propagate ascii 0        */
    _tcsncpy(&pData->aszLookPlu[STD_PLU_NUMBER_LEN-sScanLen], &aszScanPlu[0], sScanLen);
                                                /* set pMainOut->aszLookPlu */
    pData->aszLookPlu[STD_PLU_NUMBER_LEN] = 0;                  /* set NULL                 */
    if ((pData->fchModifier & LA_EMOD)
            || (pData->uchType == LABEL_UPCE)) {/* check E-Mod key or label */
        if ((sScanLen < 6) || (sScanLen > 8)) { /* OK sScanLen(6,7,8 digits)*/
            return LABEL_ERR;
        }
        if ((sScanLen == 7) && (pData->aszLookPlu[7] != _T('0'))) {
            return LABEL_ERR;                       /* NSC is not 0         */
        }
        if (sScanLen == 8) {                        /* 8 digits Input UPC-E */
            if (pData->aszLookPlu[6] != _T('0')) {      /* NSC xxxxxx cd        */
                return LABEL_ERR;                   /* NSC is not 0         */
            }
            uchSaveCd = pData->aszLookPlu[STD_PLU_NUMBER_LEN-1];      /* save input cd        */
            memmove(&pData->aszLookPlu[1], &pData->aszLookPlu[0], (STD_PLU_NUMBER_LEN-1)*sizeof(TCHAR));
                                                    /* drop CD              */
            LaMain_UpcE(pData->aszLookPlu);         /* UPC-E ==> UPC-A      */
            if (uchSaveCd != pData->aszLookPlu[STD_PLU_NUMBER_LEN-1]) {     /* check CD       */
                return LABEL_ERR;
            }
        } else {
            LaMain_UpcE(pData->aszLookPlu);         /* UPC-E ==> UPC-A      */
        }
/*      sRet = LaMain_source(pData);                ** process as UPC-A     */
        pData->uchType = LABEL_UPCE;                /* set LABEL_UPCE       */
        sRet = LABEL_OK;
    } else if (pData->fchModifier & LA_VEL) {
        sRet = LaMain_veloc(pData, sScanLen);       /* process velocity     */
    } else {
        uchCd = LaMain_plucd(pData->aszLookPlu);    /* caluculation cd      */
        switch(sScanLen) {
			case 14:
				if(uchCd != pData->aszLookPlu[STD_PLU_NUMBER_LEN-1]){
					return LABEL_ERR;
				}
				pData->uchType = LABEL_RSS14;
				sRet = LaMain_rss14(pData);
				break;

				//SR 565, we need to move the PLU number over by one so that we can add a check digit at the end
				// of the PLU number.  This works for EAN-8 PLUs that do not have the check digit already entered
				//into it.
			case 12:
				memmove(&pData->aszLookPlu[0], &pData->aszLookPlu[1], (STD_PLU_NUMBER_LEN - 1) * sizeof(TCHAR));
                                                    /* move for add CD      */
				//We then assign the check digit to the PLU number and assign it to uchCd, because
				//previously it had calculated the wrong check digit, due to the number being in the 
				//wrong location.
				pData->aszLookPlu[STD_PLU_NUMBER_LEN-1] = uchCd = LaMain_plucd(pData->aszLookPlu);

            case 13:                            /* EAN-13 source            */

                if (uchCd != pData->aszLookPlu[STD_PLU_NUMBER_LEN-1]) {
                    return LABEL_ERR;               /* check cd             */
                }
                if (pData->aszLookPlu[1] == _T('0')) {
                    pData->uchType = LABEL_UPCA;    /* set LAVEL_UPCA       */
                } else {
                    pData->uchType = LABEL_EAN13;   /* set LABEL_EAN13      */
                }
                sRet = LaMain_source(pData);        /* process as EAN/UPC   */
                break;
            case 10:                            /* UPC-A SRC w/o NCD & cd   */
                if (pData->aszLookPlu[4] == _T('0')) {      /* 0xxxxxxxxx       */
                    return LABEL_ERR;
                }
            /* no break */
            case 11:                        /* UPC-A SRC / UPC-A rw w/o cd  */
                memmove(&pData->aszLookPlu[2], &pData->aszLookPlu[3], 11 * sizeof(TCHAR));
                pData->aszLookPlu[STD_PLU_NUMBER_LEN-1] = LaMain_plucd(pData->aszLookPlu);
                                                        /* caluculate cd    */
                pData->uchType = LABEL_UPCA;            /* set LAVEL_UPCA   */
                sRet = LaMain_source(pData);        /* process as EAN/UPC   */
                break;
            case 9:
                return LABEL_ERR;               /* not support LAC w/o NSC  */
			
				//SR 565, we need to move the PLU number over by one so that we can add a check digit at the end
				// of the PLU number.  This works for EAN-8 PLUs that do not have the check digit already entered
				//into it.
			case 7:
				memmove(&pData->aszLookPlu[0], &pData->aszLookPlu[1], (STD_PLU_NUMBER_LEN - 1) * sizeof(WCHAR));
                                                    /* move for add CD      */
				//We then assign the check digit to the PLU number and assign it to uchCd, because
				//previously it had calculated the wrong check digit, due to the number being in the 
				//wrong location.
				pData->aszLookPlu[STD_PLU_NUMBER_LEN-1] = uchCd = LaMain_plucd(pData->aszLookPlu);

				//NO BREAK BECAUSE IT NEEDS TO DO THE CODE BELOW.
            case 8:                                 /* EAN-8                */
            default:                                /* UPC-A velocity       */
                if (uchCd != pData->aszLookPlu[STD_PLU_NUMBER_LEN-1]) {
                    return LABEL_ERR;               /* check cd             */
                }

/****** R2.0 START ******

                if ((pData->aszLookPlu[5] == '0')
                        || (pData->aszLookPlu[5] == '2')) {
                    return LABEL_ERR;               * not support          *
                }

 ****** R2.0 END ******/
                pData->uchType = LABEL_EAN8;        /* set LABEL_EAN8       */
                sRet = LABEL_OK;
                break;
        }
    }
    return sRet;
}

/*fhfh
***************************************************************************
*
**    Synopsis : USHORT LaMain_source(LABELANALYSIS *pData)
*
*     in/out   : LABELANALYSIS  *pData
*
**    Return   : LABEL_OK    success
*
**    Description : This function sets pMainOut->uchType(LABEL_RANDOM).
*
*                   The leading digit or prefix of the UPC is decoded as follows:
*                   0, 1, 6, 7, 8, or 9: For most products or PLUs. 
*                   2: Reserved for local use (store/warehouse), for items sold
*                      by variable weight. Variable-weight items, such as meats
*					   and fresh fruits and vegetables, are assigned a UPC by the
*					   store, if they are packaged there. In this case, the LLLLL
*					   is the item number, and the RRRRR is either the weight or
*					   the price, with the first R determining which. 
*                   3: Drugs by National Drug Code number. Pharmaceuticals in the
*                      U.S. have the remainder of the UPC as their National Drug
*					   Code (NDC) number; though usually only over-the-counter drugs
*					   are scanned at point-of-sale, NDC-based UPCs are used on
*					   prescription drug packages as well for inventory purposes. 
*                   4: Reserved for local use (store/warehouse), often for loyalty
*                      cards or store coupons. 
*                   5: Coupons The Manufacturer code is the LLLLL, the first
*                      3 RRR are a family code (set by manufacturer), and the last
*					   2 RR are a coupon code. This 2-digit code determines the amount
*					   of the discount, according to a table set by the GS1 US, with
*					   the final R being the check digit.
*                  99: In-Store coupons are EAN-13 bar codes that are similar to the
*                      standard UPC-A coupons.  Instead of the first two digits being 05
*                      this coupon has first two digits as 99. The rest of the digits
*                      are in the same format as the 05 manufacturer coupon.
***************************************************************************
fhfh*/
USHORT LaMain_source(LABELANALYSIS *pData)
{
    switch (pData->aszLookPlu[1]) {
    case _T('0'):
        switch(pData->aszLookPlu[2]) {
        case _T('2'):
            pData->uchType = LABEL_RANDOM;      /* instore label '02' flag  */
            break;
        case _T('4'):
            pData->uchType = LABEL_RANDOM;      /* instore label '04' flag  */
            break;
        case _T('5'):                           /* standard UPC coupon label '05' flag   */
            pData->uchType = LABEL_COUPON; 
            break;
        default:
            break;
        }
        break;
    case _T('2'):
        pData->uchType = LABEL_RANDOM;          /* instore label '2X' flag  */
        break;
    case _T('9'):
        switch(pData->aszLookPlu[2]) {
        case _T('9'):                           /* In-Store EAN-13 coupon label '99' flag   */
            pData->uchType = LABEL_COUPON; 
            break;
        default:
            break;
        }
    default:
        break;
    }
    return LABEL_OK;
}

/*
***************************************************************************
*
**    Synopsis : USHORT LaMain_rss14(LABELANALYSIS *pData)
*
*     in/out   : LABELANALYSIS  *pData
*
**    Return   : LABEL_OK    success
*
**    Description : This function sets pData->uchType(LABEL_RANDOM).
*
***************************************************************************
*/
USHORT LaMain_rss14(LABELANALYSIS *pData)
{
    switch (pData->aszLookPlu[0]) {
		//indicates that variable data is after the PLU(GTIN)
    case _T('9'):
        pData->uchType = LABEL_RSS14VAR;      /* label '9' flag */
        break;
    default:
        break;
    }
    return LABEL_OK;
}

/*fhfh
*******************************************************************
*
**    Synopsis : USHORT LaMain_veloc(LABELANALYSIS *pData, SHORT sScanLen)
*
*     input    : sScanLen      aszScanPlu's length
*
*     in/out   : LABELANALYSIS  *pData
*
**    Return   :  LABEL_OK     success
*                 LABEL_ERR    error
*
**    Description :  UPC VELOCITY LABEL PROCESS
*                    This Velocity label is w/o "CD check", and adds
*                    "calculate CD".
*                    "calculate CD" have weight of "313131...".
*                       UPC Velocity    ; not check CDV
*                       EAN Velocity    ; check CDV
*
*******************************************************************
fhfh*/

USHORT LaMain_veloc(LABELANALYSIS *pData, SHORT sScanLen)
{
    if ( sScanLen > STD_PLU_NUMBER_LEN-1 ) {                      /* error more than length13 */
        return LABEL_ERR;
    }

                                                              /* R2.0 Start */
    if ( pData->fchModifier & LA_UPC_VEL ) {                /* UPC velocity */
        memmove(&pData->aszLookPlu[0], &pData->aszLookPlu[1], (STD_PLU_NUMBER_LEN-1)*sizeof(TCHAR));
                                                    /* move for add CD      */
        pData->aszLookPlu[STD_PLU_NUMBER_LEN-1] = LaMain_plucd(pData->aszLookPlu);

    } else {                                                /* EAN velocity */
        if ( pData->aszLookPlu[STD_PLU_NUMBER_LEN-1] != LaMain_plucd(pData->aszLookPlu) ) {
            return LABEL_ERR;
        }
    }                                                         /* R2.0 End   */

    pData->uchType = LABEL_VELOC;                   /* set LABEL_VELOC      */
    return LABEL_OK;
}

/*fhfh
*******************************************************************
*
**    Synopsis : VOID LaMain_UpcE(UCHAR *upc_a)
*
*     input    : UCHAR  *upc_a
*
**    Return   :  none
*
**    Description :  Convert from UPC_E to UPC_A
*
*           INPUT : UPC-E  -> X1 X2 X3 X4 X5 X6
*               $ CONVERSION BY THE VALUE OF X6
*
*        X6 <= 2  ;  0  0  X1 X2 X6 0  0  0  0  X3 X4 X5 C2
*        X6  = 3  ;  0  0  X1 X2 X3 0  0  0  0  0  X4 X5 C2
*        X6  = 4  ;  0  0  X1 X2 X3 X4 0  0  0  0  0  X5 C2
*        X6 >= 5  ;  0  0  X1 X2 X3 X4 X5 0  0  0  0  X6 C2
*******************************************************************
fhfh*/
VOID LaMain_UpcE(TCHAR *upc_a)
{
    TCHAR   upc_e[6];                           /* UPC-E LABEL              */

    _tcsncpy(upc_e, &upc_a[8], 6);              /* save no as UPC-E         */
    tcharnset(upc_a, _T('0'), STD_PLU_NUMBER_LEN);              /* UPC-A fill with '0'      */
    //memcpy(upc_e, &upc_a[7], 6);              /* save no as UPC-E         */
    //memset(upc_a, '0', 13);                   /* UPC-A fill with '0'      */
    upc_a[3] = upc_e[0];                        /* move digit 2 <- x1       */
    upc_a[4] = upc_e[1];                        /* move digit 3 <- x2       */
    switch (upc_e[5]) {                         /* X6 test                  */
    case _T('0'):                               /* x6 <= 2                  */
    case _T('1'):
    case _T('2'):
        upc_a[5]  = upc_e[5];                   /* move digit 4  <- x6      */
        upc_a[10]  = upc_e[2];                   /* move digit 9  <- x3      */
        upc_a[11] = upc_e[3];                   /* move digit 10 <- x4      */
        upc_a[12] = upc_e[4];                   /* move digit 11 <- x5      */
        break;
    case _T('3'):
        upc_a[5]  = upc_e[2];                   /* move digit 4  <- x3      */
        upc_a[11] = upc_e[3];                   /* move digit 10 <- x4      */
        upc_a[12] = upc_e[4];                   /* move digit 11 <- x5      */
        break;
    case _T('4'):
        upc_a[5]  = upc_e[2];                   /* move digit 4  <- x3      */
        upc_a[6]  = upc_e[3];                   /* move digit 5  <- x4      */
        upc_a[12] = upc_e[4];                   /* move digit 11 <- x5      */
        break;
    case _T('5'):
    case _T('6'):
    case _T('7'):
    case _T('8'):
    case _T('9'):
        upc_a[5]  = upc_e[2];                   /* move digit 4  <- x3      */
        upc_a[6]  = upc_e[3];                   /* move digit 5  <- x4      */
        upc_a[7]  = upc_e[4];                   /* move digit 6  <- x5      */
        upc_a[12] = upc_e[5];                   /* move digit 11 <- x6      */
        break;
    }
    upc_a[STD_PLU_NUMBER_LEN-1] = LaMain_plucd(upc_a);            /* set upc-a cd             */
}

/*fhfh
*******************************************************************
*
**    Synopsis : UCHAR LaMain_plucd(UCHAR *PluNo)
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

  The Far Right Digit Not including the Check Digit is ODD then
  alternating to the left
*******************************************************************
fhfh*/
TCHAR LaMain_plucd(TCHAR *PluNo)
{
    SHORT    i,sEven,sOdd,cdigit;

    for (sEven = sOdd = i = 0 ; i < STD_PLU_NUMBER_LEN-1 ; ++i) {
        if (i % 2) {
            sEven += (SHORT)PluNo[i]&0x0f;                 /* even addr            */
        } else {
            sOdd  += (SHORT)PluNo[i]&0x0f;                 /* odd addr             */
        }
    }
    cdigit = (SHORT)(sEven + sOdd*3) % 10;
    cdigit = (SHORT)(10 - cdigit) % 10;
    return (TCHAR)(cdigit | 0x30);                  /* return CD about plu# */
}
