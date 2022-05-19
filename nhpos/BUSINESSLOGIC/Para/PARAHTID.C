/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter ParaHotelId Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAHTID.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaHotelIdRead()  : read HOTEL ID MNEMONICS 
*               ParaHotelIdWrite() : set HTOEL ID MNEMONICS 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Nov.11.93:           : M.Yamamoto : Init
* Apr.01.94:           : Yoshiko.J  : add Approval Code
* 2171 for Win32
* Aug-26-99: 01.00.00  : K.Iwata    : V1.0 Initial
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
**/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

/*
*=============================================================================
**  Synopsis: VOID ParaHotelIdRead(PARAHOTELID *pData) 
*               
*       Input:  *pData  : pointer to structure for PARAHOTELID
*                  (pData->uchAddress)
*      Output:  pData->aszMnemonics
*
**  Return: nothing 
*               
**  Descruption: This function reads HOTEL ID of appointed address. 
*===============================================================================
*/

VOID ParaHotelIdRead( PARAHOTELID *pData )
{

    memset(pData->aszMnemonics, '\0', sizeof(pData->aszMnemonics)); /* ### Mod (2171 for Win32) V1.0 */

    switch (pData->uchAddress) {
    case HOTEL_ID_ADR:
        _tcsncpy(&pData->aszMnemonics[0], &Para.ParaHotelId.auchHotelId[0], PARA_HOTELID_LEN);
        break;

	case HOTEL_SLD_ADR:
        _tcsncpy(&pData->aszMnemonics[0], &Para.ParaHotelId.auchSld[0], PARA_SLD_LEN);
        break;

    case HOTEL_PDT_ADR:
        _tcsncpy(&pData->aszMnemonics[0], &Para.ParaHotelId.auchProductId[0], PARA_PDT_LEN);
        break;

    case HOTEL_APVL_CODE_ADR:
        _tcsncpy(&pData->aszMnemonics[0], &Para.ParaHotelId.auchApproval[0], PARA_APVL_CODE_LEN);
        break;

    default:
        if (HOTEL_TEND1_ADR <= pData->uchAddress && pData->uchAddress <= HOTEL_TEND20_ADR) {   
            pData->aszMnemonics[0] = Para.ParaHotelId.auchTend[pData->uchAddress - HOTEL_TEND1_ADR];
        }
        break;
    }
}

/*
*=============================================================================
**  Synopsis: VOID ParaHotelIdWrite(PARAHOTELID *pData) 
*               
*       Input:  *pData  : pointer to structure for PARAHOTELID
*                  (pData->uchAddress)
*                  (pData->aszMnemonics)  
*      Output:  Nothing
*
**  Return: nothing 
*               
**  Descruption: This function set HOTEL ID in appointed address. 
*===============================================================================
*/

VOID ParaHotelIdWrite( PARAHOTELID *pData )
{

    switch (pData->uchAddress) {
    case HOTEL_ID_ADR:
        _tcsncpy( &Para.ParaHotelId.auchHotelId[0], &pData->aszMnemonics[0], PARA_HOTELID_LEN);
        break;

    case HOTEL_SLD_ADR:
        _tcsncpy( &Para.ParaHotelId.auchSld[0], &pData->aszMnemonics[0], PARA_SLD_LEN);
        break;

    case HOTEL_PDT_ADR:
        _tcsncpy( &Para.ParaHotelId.auchProductId[0], &pData->aszMnemonics[0], PARA_PDT_LEN);
        break;

    case HOTEL_APVL_CODE_ADR:
        _tcsncpy( &Para.ParaHotelId.auchApproval[0], &pData->aszMnemonics[0], PARA_APVL_CODE_LEN);
        break;

    default:
        if (HOTEL_TEND1_ADR <= pData->uchAddress && pData->uchAddress <= HOTEL_TEND20_ADR) {   
            Para.ParaHotelId.auchTend[pData->uchAddress - HOTEL_TEND1_ADR] = pData->aszMnemonics[0];
        }
        break;
    }
}

/*================ Enf of Source ============================*/

