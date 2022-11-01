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

#include    <string.h>
#include    <stdlib.h>
#include    <ecr.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <para.h>        /* R2.0 */
#include    <csstbpar.h>
#include    "rfllabel.h"
#include	"AW_Interfaces.h"

/**************************************************************
 *    EXTERNAL FUNCTION
*/

/**************************************************************
 *    EXTERNAL DATA
 */

/**************************************************************
 *    STATIC DATA
 */


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
    WCHAR   aszScanPlu[STD_PLU_NUMBER_LEN+1];                   /* only PLU # (w/o A.B.C)   */
    int     sScanLen;                           /* aszScanPlu's length      */
	int     nOffset;

    /*--- Initialize pData ---*/
	// Clear the LABELANALYSIS struct from pData->aszLookPlu to the end
	// Figure offset, in bytes, of pData->aszLookPlu from the beginning
	// of the struct then subtract that offset from the total struct size
	// in order to determine the number of bytes to be cleared.
	nOffset = (UCHAR *) &(pData->aszLookPlu) - (UCHAR *)pData;

    memset(pData->aszLookPlu, 0, sizeof(LABELANALYSIS) - nOffset);

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
        case 'A':                                   /* UPC-A or EAN-13      */
            sScanLen = LEN_UPCA;                    /* set UPC-A/EAN-13 len */
            memcpy(&aszScanPlu[0], &pData->aszScanPlu[1], sScanLen * sizeof(WCHAR));
            break;
        case 'B':                                   /* EAN-8                */
            pData->uchType = LABEL_EAN8;            /* set EAN-8 temporary  */
            sScanLen = LEN_EAN8;                    /* set EAN-8 length     */
            memcpy(&aszScanPlu[0], &pData->aszScanPlu[1], sScanLen * sizeof(WCHAR));
            break;
        case 'C':                                   /* UPC-E                */
            pData->uchType = LABEL_UPCE;            /* set UPC-E temporary  */
            sScanLen = LEN_UPCE;                    /* set UPC-E length     */
            memcpy(&aszScanPlu[0], &pData->aszScanPlu[1], sScanLen * sizeof(WCHAR));
            break;
        default:
            sScanLen = wcslen(pData->aszScanPlu);   /* set aszScanPlu's len,    Saratoga */
            if (sScanLen > STD_PLU_NUMBER_LEN) {                    /* error more than 13   */
				if(pData->uchType == LABEL_RSS14){  //Scanned RSS14 is more than 14 digits( 16 to 74 digits)
					sScanLen = LEN_RSS14;
					wcsncpy(&aszScanPlu[0], &pData->aszScanPlu[2], sScanLen);
				}else{
					return LABEL_ERR;
				}
            }else{

			/* 2172, not support velocity */
				if ( ( sScanLen <= 6 ) && !( pData->fchModifier & LA_EMOD ) ) {
					pData->fchModifier |= LA_UPC_VEL;   /* set velocity     */
					pData->fchModifier |= LA_VEL;
				}
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
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
				memcpy(&aszScanPlu[0], &pData->aszScanPlu[0], sScanLen * sizeof(WCHAR));
			}
                                                    /* R2.0 End */
    }
    sRet = LaMain_cvlbl(pData, &aszScanPlu[0], (SHORT)sScanLen);
                                                    /* convertion & return value  */
    if (sRet) {
        return sRet;
    } else if (pData->uchType == LABEL_RANDOM) {
        return (_RflInst(pData));
    } else if(pData->uchType == LABEL_RSS14VAR) {
		return (_RflRssVar(pData));
    } else {
        return LABEL_OK;
    }

}
#if defined(POSSIBLE_DEAD_CODE)

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

USHORT RflConvScan2Manual(WCHAR *aszDest, WCHAR *aszSrc)
{
    int     i;
    USHORT  usRet;

    switch(*aszSrc) {   /* check E-Version or not */
    case 'C':
    case 'E':
        usRet = MOD_STAT_EVERSION;
        break;

    default:
        usRet = 0;
        break;
    }
    for(i=0; *(aszSrc+i)!='\0' ;i++) {
        if(('0' <= *(aszSrc+i)) && (*(aszSrc+i) <= '9'))
            break;
    }
    if(*(aszSrc+i) != '\0') {
        wcscpy(aszDest, (aszSrc+i));    /* Saratoga */
        if(*aszSrc == 'C') {  /* E-Version with Check Digit */
            i=wcslen(aszDest); /* Saratoga */
            *(aszDest+i-1) = '\0';
        }
    }
    else
        *aszDest = '\0';

    return usRet;
}


#endif
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
    int     sScanLen;                           /* aszScanPlu's length      */

    /* ----- check PLU is entered by Scanner or manual entry ? ----- */
    if (! (pData->fchModifier & LA_SCANNER)) {
        return LABEL_OK;
    }

    switch (pData->aszScanPlu[0]) {
        case 'A':                                   /* UPC-A or EAN/JAN-13  */
            sScanLen = wcslen(pData->aszScanPlu);  /* Saratoga */
            if (sScanLen == LEN_UPCA) {
                                                    /* Chg 10/4/93 M.Sugi */
                memmove(&pData->aszScanPlu[2], &pData->aszScanPlu[1], 12 * sizeof(WCHAR));
                pData->aszScanPlu[1] = '0';
            }
        /* no break  */
        case 'B':                                   /* EAN/JAN-8            */
        case 'C':                                   /* UPC-E                */
            break;

        case 'E':                                   /* UPC-E                */
            memmove(&pData->aszScanPlu[0], &pData->aszScanPlu[2], 6 * sizeof(WCHAR));
            pData->aszScanPlu[6] = 0;
            pData->fchModifier |= LA_EMOD;
            break;

        case 'F':                                   /* EAN-13 or EAN-8      */
            if (pData->aszScanPlu[1] != 'F') {
                pData->aszScanPlu[0] = 'A';
            } else {
                memmove(&pData->aszScanPlu[1], &pData->aszScanPlu[2], 8 * sizeof(WCHAR));
                pData->aszScanPlu[0] = 'B';
                pData->aszScanPlu[9] = 0;
            }
            break;

		case '0':
			//check for RSS14 if Scanned and Mark As Such
			if(pData->aszScanPlu[1] == '1'){
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
SHORT LaMain_cvlbl(LABELANALYSIS *pData, WCHAR *aszScanPlu, SHORT sScanLen)
{
    WCHAR   uchCd;
    SHORT   sRet;
    WCHAR   uchSaveCd;
	int i;

    for(i = 0; i < STD_PLU_NUMBER_LEN; i++){
		wcscpy(&pData->aszLookPlu[i], WIDE("0"));
	}
	//memset(&pData->aszLookPlu[0], , sizeof(pData->aszLookPlu));
                                                /* propagate ascii 0        */
    memcpy(&pData->aszLookPlu[STD_PLU_NUMBER_LEN-sScanLen], &aszScanPlu[0], sScanLen * sizeof(WCHAR));
                                                /* set pMainOut->aszLookPlu */
    pData->aszLookPlu[STD_PLU_NUMBER_LEN] = 0;                  /* set NULL                 */
    if ((pData->fchModifier & LA_EMOD)
            || (pData->uchType == LABEL_UPCE)) {/* check E-Mod key or label */
        if ((sScanLen < 6) || (sScanLen > 8)) { /* OK sScanLen(6,7,8 digits)*/
            return LABEL_ERR;
        }
        if ((sScanLen == 7) && (pData->aszLookPlu[7] != '0')) {
            return LABEL_ERR;                       /* NSC is not 0         */
        }
        if (sScanLen == 8) {                        /* 8 digits Input UPC-E */
            if (pData->aszLookPlu[6] != '0') {      /* NSC xxxxxx cd        */
                return LABEL_ERR;                   /* NSC is not 0         */
            }
            uchSaveCd = pData->aszLookPlu[STD_PLU_NUMBER_LEN-1];      /* save input cd        */
            memmove(&pData->aszLookPlu[1], &pData->aszLookPlu[0], (STD_PLU_NUMBER_LEN-1) * sizeof(WCHAR));
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
				memmove(&pData->aszLookPlu[0], &pData->aszLookPlu[1], (STD_PLU_NUMBER_LEN - 1) * sizeof(WCHAR));
                                                    /* move for add CD      */
				//We then assign the check digit to the PLU number and assign it to uchCd, because
				//previously it had calculated the wrong check digit, due to the number being in the 
				//wrong location.
				pData->aszLookPlu[STD_PLU_NUMBER_LEN-1] = uchCd = LaMain_plucd(pData->aszLookPlu);

			case 13:                            /* EAN-13 source            */
            
			                            /* EAN-13 ins/UPC-A w/ CD   */
                if (uchCd != pData->aszLookPlu[STD_PLU_NUMBER_LEN-1]) {
                    return LABEL_ERR;               /* check cd             */
                }
                if (pData->aszLookPlu[1] == '0') {
                    pData->uchType = LABEL_UPCA;    /* set LAVEL_UPCA       */
                } else {
                    pData->uchType = LABEL_EAN13;   /* set LABEL_EAN13      */
                }
                sRet = LaMain_source(pData);        /* process as EAN/UPC   */
                break;
            case 10:                            /* UPC-A SRC w/o NCD & cd   */
                if (pData->aszLookPlu[4] == '0') {      /* 0xxxxxxxxx       */
                    return LABEL_ERR;
                }
            /* no break */
            case 11:                        /* UPC-A SRC / UPC-A rw w/o cd  */
                memmove(&pData->aszLookPlu[2], &pData->aszLookPlu[3], 11 * sizeof(WCHAR));
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
***************************************************************************
fhfh*/
USHORT LaMain_source(LABELANALYSIS *pData)
{
    switch (pData->aszLookPlu[1]) {
    case '0':
        switch(pData->aszLookPlu[2]) {
        case '2':
            pData->uchType = LABEL_RANDOM;      /* instore label '02' flag  */
            break;
        case '4':
            pData->uchType = LABEL_RANDOM;      /* instore label '04' flag  */
            break;
        case '5':
            pData->uchType = LABEL_COUPON;      /* coupon label '05' flag   */
        default:
            break;
        }
        break;
    case '2':
        pData->uchType = LABEL_RANDOM;          /* instore label '2X' flag  */
        break;
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

SHORT LaMain_veloc(LABELANALYSIS *pData, SHORT sScanLen)
{
    if ( sScanLen > STD_PLU_NUMBER_LEN - 1 ) {                      /* error more than length13 */
        return LABEL_ERR;
    }

                                                              /* R2.0 Start */
    if ( pData->fchModifier & LA_UPC_VEL ) {                /* UPC velocity */
        memmove(&pData->aszLookPlu[0], &pData->aszLookPlu[1], (STD_PLU_NUMBER_LEN - 1) * sizeof(WCHAR));
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
VOID LaMain_UpcE(WCHAR *upc_a)
{
    WCHAR   upc_e[6];                           /* UPC-E LABEL              */

    wcsncpy(upc_e, &upc_a[8], 6);                /* save no as UPC-E         */
    _wcsnset(upc_a, '0', STD_PLU_NUMBER_LEN);                     /* UPC-A fill with '0'      */


    upc_a[3] = upc_e[0];                        /* move digit 2 <- x1       */
    upc_a[4] = upc_e[1];                        /* move digit 3 <- x2       */
    switch (upc_e[5]) {                         /* X6 test                  */
    case '0':                                   /* x6 <= 2                  */
    case '1':
    case '2':
        upc_a[5]  = upc_e[5];                   /* move digit 4  <- x6      */
        upc_a[10]  = upc_e[2];                   /* move digit 9  <- x3      */
        upc_a[11] = upc_e[3];                   /* move digit 10 <- x4      */
        upc_a[12] = upc_e[4];                   /* move digit 11 <- x5      */
        break;
    case '3':
        upc_a[5]  = upc_e[2];                   /* move digit 4  <- x3      */
        upc_a[11] = upc_e[3];                   /* move digit 10 <- x4      */
        upc_a[12] = upc_e[4];                   /* move digit 11 <- x5      */
        break;
    case '4':
        upc_a[5]  = upc_e[2];                   /* move digit 4  <- x3      */
        upc_a[6]  = upc_e[3];                   /* move digit 5  <- x4      */
        upc_a[12] = upc_e[4];                   /* move digit 11 <- x5      */
        break;
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        upc_a[5]  = upc_e[2];                   /* move digit 4  <- x3      */
        upc_a[6]  = upc_e[3];                   /* move digit 5  <- x4      */
        upc_a[7]  = upc_e[4];                   /* move digit 6  <- x5      */
        upc_a[12] = upc_e[5];                   /* move digit 11 <- x6      */
        break;
    }
    upc_a[STD_PLU_NUMBER_LEN - 1] = LaMain_plucd(upc_a);            /* set upc-a cd             */
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
*******************************************************************
fhfh*/
WCHAR LaMain_plucd(WCHAR *PluNo)
{
    SHORT    i,sEven,sOdd,cdigit;

    for (sEven = sOdd = i = 0 ; i < STD_PLU_NUMBER_LEN-1 ; ++i) {
        if (i % 2) {
            sEven += (SHORT)(PluNo[i]&0x0f);        /* even addr, Saratoga */
        } else {
            sOdd  += (SHORT)(PluNo[i]&0x0f);        /* odd addr, Saratoga */
        }
    }
    cdigit = (SHORT)((sEven + sOdd*3) % 10);        /* Saratoga */
    cdigit = (SHORT)((10 - cdigit) % 10);           /* Saratoga */
    return (WCHAR)(cdigit | 0x30);                  /* return CD about plu# */
}
