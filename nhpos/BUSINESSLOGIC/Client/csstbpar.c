/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, System Parameter Source File
* Category    : Client/Server STUB, US Hospitality Model
* Program Name: CSSTBPAR.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*				CliParaAllRead()		Read all parameter on system RAM
*				CliParaAllWrite()		Write all parameter on system RAM
*				CliParaRead()			Read specified parameter
*				CliParaWrite()			Write specified parameter
*				CliParaMDCCheck()		Check MDC Bit
*               CliParaSupLevelRead()   Read supervisor No/ Level
*               CliParaMenuPLURead()    Read set menu of PLU
*				CliParaOepRead()		Read OEP Table
*				CliParaOepWrite()		Write OEP Table
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Feb-28-95:hkato      : R3.0
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
#include	<tchar.h>

#include    <ecr.h>
#include    <pif.h>
#include	<paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    "csstubin.h"

/*
*===========================================================================
** Synopsis:    SHORT	CliParaAllRead( UCHAR uchClass,
*                                       UCHAR *puchRcvBuffer,
*                                       USHORT usRcvBufLen,
*                                       USHORT usStartPointer,
*                                       USHORT *pusReturnLen);
*     Input:    uchClass    - Data Class
*               usRcvBufLen - Receive Buffer Length
*               usStartPointer  - Offset to Read
*    Output:    *pusReturnLen   - Read Length 
*               *puchRcvBuffer  - Receive Buffer
*
** Return:       0:  Read All RAM
*               -1:  Can not read All RAM
*
** Description: This function reads all system RAM by Data Class.   
*===========================================================================
*/
SHORT	CliParaAllRead( USHORT usClass,
                        VOID *puchRcvBuffer,
                        USHORT usRcvBufLen,
                        USHORT usStartPointer,
                        USHORT *pusReturnLen)
{
    return  ParaAllRead(usClass, puchRcvBuffer, usRcvBufLen, 
                        usStartPointer, pusReturnLen);
}

/*
*===========================================================================
** Synopsis:    SHORT	CliParaAllWrite(UCHAR uchClass,
*                                       UCHAR *puchWrtBuffer,
*                                       USHORT usRWrtBufLen,
*                                       USHORT usStartPointer,
*                                       USHORT *pusReturnLen);
*     Input:    uchClass    - Data Class
*               usRWrtBufLen    - Write Buffer Length
*               usStartPointer  - Offset to Read
*    Output:    *pusReturnLen   - Read Length 
*               *puchWrtBuffer  - Receive Buffer
*
** Return:       0:  Write Ok
*               -2:  Can not write on RAM
*
** Description: This function writes all system RAM by Data Class.   
*===========================================================================
*/
SHORT	CliParaAllWrite(USHORT usClass,
                        VOID *puchWrtBuffer,
                        USHORT usRWrtBufLen,
                        USHORT usStartPointer,
                        USHORT *pusReturnLen)
{
    SHORT   sError;

    PifRequestSem(husCliExePara);

    sError = ParaAllWrite(usClass, puchWrtBuffer, usRWrtBufLen, 
                          usStartPointer, pusReturnLen);

    PifReleaseSem(husCliExePara);
    return sError;
}

/*
*===========================================================================
** Synopsis:    VOID	CliParaRead(VOID *pDATA);
*     InOut:    pDATA   - Data Pointer
*
** Return:      none.       
*
** Description: This function reads parameter on system RAM.   
*===========================================================================
*/
VOID    CliParaRead(VOID *pDATA)
{
    ParaRead(pDATA);
}

/*
*===========================================================================
** Synopsis:    VOID	CliParaWrite(VOID *pDATA);
*     InOut:    pDATA   - Data Pointer
*
** Return:      none.       
*
** Description: This function writes parameter on system RAM.   
*===========================================================================
*/
VOID    CliParaWrite(VOID *pDATA)
{
    ParaWrite(pDATA);
}

/*
*===========================================================================
** Synopsis:    UCHAR	CliParaMDCCheck(UCHAR uchAddress, 
*                                       UCHAR uchMaskData);
*     InPut:    uchAddress  - MDC address
*               uchMaskData - MDC bit mask
*
** Return:      Masked Bit Patern
*
** Description: This function checks MDC parameter on system RAM.   
*===========================================================================
*/
UCHAR   CliParaMDCCheck(USHORT usAddress, UCHAR uchMaskData)
{
	return  ParaMDCCheck(usAddress, uchMaskData);
}

UCHAR   CliParaMDCCheckField(USHORT usAddress, UCHAR uchMaskType)
{
	USHORT bShiftMask = 1;      // 0 no change, 1 mask, 2 shift right and mask
	UCHAR  uchRetStatus = 0;
	UCHAR  uchMaskData = 0;

	if (usAddress & 0x01) {
		// The odd numbered MDCs are stored in the low nibble so lets remove any high order bits
		// if we are getting either a nibble or a particular bit.  If byte then leave alone.
		bShiftMask = 1;      // default is mask only since odd bits are in lower nibble
		switch (uchMaskType) {
			case MDC_PARAM_BIT_A:
				uchMaskData = ODD_MDC_BIT3;
				break;
			case MDC_PARAM_BIT_B:
				uchMaskData = ODD_MDC_BIT2;
				break;
			case MDC_PARAM_BIT_C:
				uchMaskData = ODD_MDC_BIT1;
				break;
			case MDC_PARAM_BIT_D:
				uchMaskData = ODD_MDC_BIT0;
				break;
			case MDC_PARAM_2_BIT_A:
				uchMaskData = (ODD_MDC_BIT3 | ODD_MDC_BIT2);
				break;
			case MDC_PARAM_2_BIT_C:
				uchMaskData = (ODD_MDC_BIT1 | ODD_MDC_BIT0);
				break;
			case MDC_PARAM_BYTE:
				//This is used to get the whole byte
				uchMaskData = 0xff;
				bShiftMask = 0;       // indicates no change of data with no masking or shifting
				break;
			case MDC_PARAM_NIBBLE_A:
				//This is used to get the alternate or unused nibble
				uchMaskData = ~(ODD_MDC_BIT3 | ODD_MDC_BIT2 | ODD_MDC_BIT1 | ODD_MDC_BIT0);
				bShiftMask = 2;       // indicates shift right then mask
				break;
			case MDC_PARAM_NIBBLE_B:
				//This is used to get the standard or used nibble
				uchMaskData = (ODD_MDC_BIT3 | ODD_MDC_BIT2 | ODD_MDC_BIT1 | ODD_MDC_BIT0);
				break;
			default:
				uchMaskData = 0xff;
				bShiftMask = 0;       // indicates no change of data with no masking or shifting
				break;
		}
		uchRetStatus = CliParaMDCCheck (usAddress, uchMaskData);
		switch (bShiftMask) {
			case 2:   // shift right first then do mask keeping only the upper nibble
				uchRetStatus >>= 4;
				// fall through to do the mask
			case 1:   // mask keeping only the lower nibble
				uchRetStatus &= 0x0f;
				break;

			default:
				break;
		}
	}
	else {
		// The even numbered MDCs are stored in the high nibble so lets shift to right justify
		// if we are getting either a nibble or a particular bit.  If byte then leave alone.
		bShiftMask = 2;      // default is shift right then mask since even bits are in upper nibble
		switch (uchMaskType) {
			case MDC_PARAM_BIT_A:
				uchMaskData = EVEN_MDC_BIT3;
				break;
			case MDC_PARAM_BIT_B:
				uchMaskData = EVEN_MDC_BIT2;
				break;
			case MDC_PARAM_BIT_C:
				uchMaskData = EVEN_MDC_BIT1;
				break;
			case MDC_PARAM_BIT_D:
				uchMaskData = EVEN_MDC_BIT0;
				break;
			case MDC_PARAM_2_BIT_A:
				uchMaskData = (EVEN_MDC_BIT3 | EVEN_MDC_BIT2);
				break;
			case MDC_PARAM_2_BIT_C:
				uchMaskData = (EVEN_MDC_BIT1 | EVEN_MDC_BIT0);
				break;
			case MDC_PARAM_BYTE:
				//This is used to get the whole byte
				uchMaskData = 0xff;
				bShiftMask = 0;       // indicates no change of data with no masking or shifting
				break;
			case MDC_PARAM_NIBBLE_A:
				//This is used to get the alternate or unused nibble
				uchMaskData = ~(EVEN_MDC_BIT3 | EVEN_MDC_BIT2 | EVEN_MDC_BIT1 | EVEN_MDC_BIT0);
				bShiftMask = 1;       // indicates mask only keeping lower nibble
				break;
			case MDC_PARAM_NIBBLE_B:
				//This is used to get the standard or used nibble
				uchMaskData = (EVEN_MDC_BIT3 | EVEN_MDC_BIT2 | EVEN_MDC_BIT1 | EVEN_MDC_BIT0);
				break;
			default:
				uchMaskData = 0xff;
				bShiftMask = 0;       // indicates no change of data with no masking or shifting
				break;
		}
		uchRetStatus = CliParaMDCCheck (usAddress, uchMaskData);
		switch (bShiftMask) {
			case 2:   // shift right first then do mask keeping only the upper nibble
				uchRetStatus >>= 4;
				// fall through to do the mask
			case 1:   // mask keeping only the lower nibble
				uchRetStatus &= 0x0f;
				break;

			default:
				break;
		}
	}

	return uchRetStatus;
}

/*
*===========================================================================
** Synopsis:    SHORT	CliParaSupLevelRead(PARASUPLEVEL *pDATA);
*     InOut:    *pDATA  - Data Pointer
*
** Return:       0:  Read OK
*               -1:  Read Error
*
** Description: This function reads supervisor No/ Level on system RAM.   
*===========================================================================
*/
SHORT   CliParaSupLevelRead(PARASUPLEVEL *pDATA)
{
    return  ParaSupLevelRead(pDATA);
}

/*
*===========================================================================
** Synopsis:    SHORT	CliParaMenuPLURead(PARAMENUPLUTBL *pDATA);
*     InOut:    *pDATA  - Data Pointer
*
** Return:       0:  Read OK
*               -1:  Read Error
*
** Description: This function reads set menu of the PLU on system RAM.   
*===========================================================================
*/
SHORT   CliParaMenuPLURead(PARAMENUPLUTBL *pDATA)
{
    return  ParaMenuPLURead(pDATA);
}

/*
*===========================================================================
** Synopsis:    VOID 	CliParaMenuPLUWrite(UCHAR type, 
*                                           PARAMENUPLUTBL *pDATA);
*     InPut:    type    - Data Type
*     InOut:    *pDATA  - Data Pointer
*
** Return:      none.
*
** Description:
*   This function writes and deletes set menu of the PLU on system RAM.   
*===========================================================================
*/
VOID    CliParaMenuPLUWrite(UCHAR type, PARAMENUPLUTBL *pDATA)
{
    ParaMenuPLUWrite(type, pDATA);
}

/*
*===========================================================================
** Synopsis:    VOID   CliParaOepRead(PARAOEPTBL *pDATA);
*     InOut:    *pDATA  - Data Pointer
*
** Return:      none
*
** Description: This function reads OEP table.   
*===========================================================================
*/
VOID    CliParaOepRead(PARAOEPTBL *pDATA)
{
    ParaOepRead(pDATA);
}

/*
*===========================================================================
** Synopsis:    VOID 	CliParaOepWrite(PARAOEPTBL *pDATA);
*     InOut:    *pDATA  - Data Pointer
*
** Return:      none.
*
** Description:
*   This function writes OEP table.   
*===========================================================================
*/
VOID    CliParaOepWrite(PARAOEPTBL *pDATA)
{
    ParaOepWrite(pDATA);
}

VOID    CliParaSaveFarData (VOID)
{
	ParaUpdateAssrtlogOffset();  // update the ASSRTLOG file offset in the memory resident data base
	PifSaveFarData();            // save the memory resident database after updating Para
}
/*===== END OF SOURCE =====*/
