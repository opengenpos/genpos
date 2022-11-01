/*------------------------------------------------------------------------*\
*===========================================================================
* Title       : Reentrant Functions Library : LABEL CONVERTION FUNCTION
* Category    : Rfl Module, NCR 2170 GP R2.0 Model Application
* Program Name: RFLRANDM.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
*
*
*
* --------------------------------------------------------------------------
* Update Histories
*   Date    : Version  :   Name			: Description
* May-25-04 : 00.00.01 : R.Herrington   : Initial
*
*===========================================================================
*===========================================================================

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
/*
Application Identifiers are used with RSS-14

If the format is n2 then exactly 2 numbers
If the format is n..15 then up to 15 numbers

AI		Full Title					Format		Data Title
------------------------------------------------------------
01		Global Triade Item Number	n2+n14		GTIN

11		Production date(YYMMDD)		n2+n6		PROD DATE
12		Due date(YYMMDD)			n2+n6		DUE DATE
13		Packaging Date(YYMMDD)		n2+n6		PACK DATE
15		Best Before date(YYMMDD)	n2+n6		BEST BEFORE or SELL BY
17		expiration date(YYMMDD)		n2+n6		USE BY or EXPIRY

390(n)	Amount Payable -
		single monetary area		n4+n..15	AMOUNT
391(n)	Amount Payable - with
		ISO currency code			n4+n3+n..15	AMOUNT
392(n)	Amount Payable for a
		Variable Measure Trade Item
		- single monetary area		n4+n..15	PRICE
393(n)	Amount Payable for a
		Variable Measure Trade Item
		- with ISO currency code	n4+n3+n..15	PRICE

(n) can be 0 through 3, which indicates the decimal point position

Metric Trade Measures
AI		Full Title					Format		Data Title			Units
------------------------------------------------------------------------------
310(n)	Net Weight					n4+n6		NET WEIGHT(kg)		Kilograms
311(n)	Length or 1st Dimension		n4+n6		LENGTH(m)			Metres
312(n)	Width or 2nd Dimension		n4+n6		WIDTH(m)			Metres
313(n)	Depth, thichness, height,
		or third dimension			n4+n6		HEIGHT(m)			Metres
314(n)	Area						n4+n6		AREA(m2)			Metres
315(n)	Net Volume					n4+n6		NET VOLUME(l)		Litres
316(n)	Net Volume					n4+n6		NET VOLUME(m3)		Cubic Metres



Non-Metric Trade Measures
AI		Full Title					Format		Data Title			Units
------------------------------------------------------------------------------
320(n)	Net Weight					n4+n6		NET WEIGHT(lb)		Pounds
321(n)	Length or 1st Dimension		n4+n6		LENGTH(i)			Inches
322(n)	Length or 1st Dimension		n4+n6		LENGTH(f)			Feet
323(n)	Length or 1st Dimension		n4+n6		LENGTH(y)			Yards
324(n)	Width or 2nd Dimension		n4+n6		WIDTH(i)			Inches
325(n)	Width or 2nd Dimension		n4+n6		WIDTH(f)			Feet
326(n)	Width or 2nd Dimension		n4+n6		WIDTH(y)			Yards
327(n)	Depth, thichness, height,
		or third dimension			n4+n6		HEIGHT(i)			Inches
328(n)	Depth, thichness, height,
		or third dimension			n4+n6		HEIGHT(f)			Feet
329(n)	Depth, thichness, height,
		or third dimension			n4+n6		HEIGHT(y)			Yards
350(n)	Area						n4+n6		AREA(i2)			Square Inches
351(n)	Area						n4+n6		AREA(f2)			Square Feet
352(n)	Area						n4+n6		AREA(y2)			Square Yards
356(n)	Net Weight					n4+n6		NET WEIGHT(t)		Troy ounces
357(n)	Net Volume(or weight)		n4+n6		NET VOLUME(oz)		Ounces(US)
360(n)	Net Volume					n4+n6		NET VOLUME(lb)		Quarts
361(n)	Net Volume					n4+n6		NET VOLUME(g)		Gallons(US)
364(n)	Net Volume					n4+n6		NET VOLUME(i3)		Cubic Inches
365(n)	Net Volume					n4+n6		NET VOLUME(f3)		Cubic Feet
366(n)	Net Volume					n4+n6		NET VOLUME(y3)		Cubic Yards

 (n) can be 0 through 9 which indicates the decimal point position


Example RSS 14 Expanded

GTIN = 90614141000022
PRICE = $5.67
WEIGHT = 1.23 lbs

019061414100002239225673202000123
A group seperator (0x1d) is used at then end of
a variable lenght field

n2+n14				n4+n..15		n4+n6
(01)90614141000022 (3922)567(0x1d) (3202)000123
GTIN				PRICE			WEIGHT

*/

/*
***************************************************************************
**    Synopsis : USHORT _RflRssVar(LABELANALYSIS *pData)
*
*     in/out   : pData         Addr. of look up plu# & data.
**    Return   : LABEL_OK   : success
*                LABEL_ERR  : ERROR
**    Description : RSS14 Expanded label conversion                   
***************************************************************************
*/
SHORT _RflRssVar(LABELANALYSIS *pData)
{
	USHORT i = 0, j = 0;

    /* ----- check PLU is entered by Scanner or manual entry ? ----- */
    if (! (pData->fchModifier & LA_SCANNER)) {
		pData->uchType = LABEL_RSS14;
        return LABEL_OK;
    }

	for(i = 0; i < wcslen(pData->aszScanPlu);){
		switch(pData->aszScanPlu[i]){
		case '0':	//GTIN n2 + n14
			if(pData->aszScanPlu[i + 1] == '1'){
				wcsncpy(pData->aszMaskLabel, &pData->aszScanPlu[i + 2], LEN_RSS14);
				i += 16;
			}
			break;
		case '1':
			switch(pData->aszScanPlu[i + 1]){
			case '1':	//n2 + n6 Date
				//Production Date
				pData->uchDateType = LA_DATE_PROD;
				break;
			case '2':	//n2 + n6 Date
				//Due Date
				pData->uchDateType = LA_DATE_DUE;
				break;
			case '3':	//n2 + n6 Date
				//Packaging Date
				pData->uchDateType = LA_DATE_PACK;
				break;
			case '5':	//n2 + n6 Date
				//Best Before date
				pData->uchDateType = LA_DATE_BEST;
				break;
			case '7':	//n2 + n6 Date
				//expiration date
				pData->uchDateType = LA_DATE_EXP;
				break;
			}
			i += 2; //move past the application Identifier
			wcsncpy(pData->aszDate, &pData->aszScanPlu[i], 6);
			i += 6;
			break;
		case '3':
			switch(pData->aszScanPlu[i + 1]){
			case '1':	//Metric Trade Measures
				switch(pData->aszScanPlu[i + 2]){
				case '0':
					pData->ulMeasureType[0] = LA_QTY_WEIGHT | LA_QTY_KG;
					i += 3;
					pData->ulMeasureType[0] |= pData->ulMeasureType[0] | (LA_DEC_0 << (pData->aszScanPlu[i] - '0'));
					i++;
					wcsncpy(pData->aszMeasurement[0], &pData->aszScanPlu[i], 6);
					pData->fsBitMap |= LA_F_WEIGHT;
					pData->uchLookup = LA_EXE_LOOKUP;
					break;
/* //RPH Only use Weight
				case '1':
					pData->ulMeasureType[0] = LA_QTY_LENGTH | LA_QTY_M;
					break;
				case '2':
					pData->ulMeasureType[1] = LA_QTY_WIDTH | LA_QTY_M;
					break;
				case '3':
					pData->ulMeasureType[2] = LA_QTY_HEIGHT | LA_QTY_M;
					break;
				case '4':
					pData->ulMeasureType[0] = LA_QTY_AREA | LA_QTY_M;
					break;
				case '5':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_L;
					break;
				case '6':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_M;
					break;
*/
				}
				break;
			case '2': //Non-Metric Trade Measures
				switch(pData->aszScanPlu[i + 2]){
				case '0':
					pData->ulMeasureType[0] = LA_QTY_WEIGHT | LA_QTY_LB;
					i += 3;
					pData->ulMeasureType[0] |= pData->ulMeasureType[0] | (LA_DEC_0 << (pData->aszScanPlu[i] - '0'));
					i++;
					wcsncpy(pData->aszMeasurement[0], &pData->aszScanPlu[i], 6);
					pData->fsBitMap |= LA_F_WEIGHT;
					pData->uchLookup = LA_EXE_LOOKUP;
					break;
/* //RPH Only use Weight
				case '1':
					pData->ulMeasureType[0] = LA_QTY_LENGTH | LA_QTY_IN;
					break;
				case '2':
					pData->ulMeasureType[0] = LA_QTY_LENGTH | LA_QTY_FT;
					break;
				case '3':
					pData->ulMeasureType[0] = LA_QTY_LENGTH | LA_QTY_YD;
					break;
				case '4':
					pData->ulMeasureType[0] = LA_QTY_WIDTH | LA_QTY_IN;
					break;
				case '5':
					pData->ulMeasureType[0] = LA_QTY_WIDTH | LA_QTY_FT;
					break;
				case '6':
					pData->ulMeasureType[0] = LA_QTY_WIDTH | LA_QTY_YD;
					break;
				case '7':
					pData->ulMeasureType[0] = LA_QTY_HEIGHT | LA_QTY_IN;
					break;
				case '8':
					pData->ulMeasureType[0] = LA_QTY_HEIGHT | LA_QTY_FT;
					break;
				case '9':
					pData->ulMeasureType[0] = LA_QTY_HEIGHT | LA_QTY_YD;
					break;
*/				}
			i += 6;
			break;
/* //RPH Only use Weight
			case '5': //Non-Metric Trade Measures
				switch(pData->aszScanPlu[i + 2]){
				case '0':
					pData->ulMeasureType[0] = LA_QTY_AREA | LA_QTY_IN;
					break;
				case '1':
					pData->ulMeasureType[0] = LA_QTY_AREA | LA_QTY_FT;
					break;
				case '2':
					pData->ulMeasureType[0] = LA_QTY_AREA | LA_QTY_YD;
					break;
				case '6':
					pData->ulMeasureType[0] = LA_QTY_WEIGHT | LA_QTY_TO;
					break;
				case '7':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_OZ;
					break;
				}
				break;
			case '6': //Non-Metric Trade Measures
				switch(pData->aszScanPlu[i + 2]){
				case '0':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_QT;
					break;
				case '1':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_USG;
					break;
				case '4':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_IN;
					break;
				case '5':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_FT;
					break;
				case '6':
					pData->ulMeasureType[0] = LA_QTY_VOLUME | LA_QTY_YD;
					break;
				}
				break;
*/
			i += 6;
			break;
			case '9': //Amount/Price
				switch(pData->aszScanPlu[i + 2]){
				case '0':
					i += 3;
					pData->uchPriceDecimal = pData->aszScanPlu[i] - '0';
					i++;
					break;
				case '1':
					i += 3;
					pData->uchPriceDecimal = pData->aszScanPlu[i] - '0';
					i++;
					wcsncpy(pData->aszISOCurCode, &pData->aszScanPlu[i], 3);
					i += 3;
					break;
				case '2':
					i += 3;
					pData->uchPriceDecimal = pData->aszScanPlu[i] - '0';
					i++;
					break;
				case '3':
					i += 3;
					pData->uchPriceDecimal = pData->aszScanPlu[i] - '0';
					i++;
					wcsncpy(pData->aszISOCurCode, &pData->aszScanPlu[i], 3);
					i += 3;
					break;
				default:
					return(LABEL_ERR);
				}
				j = 0;
				while(pData->aszScanPlu[i] != 0x1d && j < 15){
					pData->aszPrice[j++] = pData->aszScanPlu[i++];
				}
				i++; // move past end of group 0x1d
				pData->fsBitMap |= LA_F_PRICE;
				pData->uchLookup = LA_EXE_LOOKUP;
				break;
			}
			break;
			default:
				return(LABEL_ERR);
		}
	}

    return(LABEL_OK);
}