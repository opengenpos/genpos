/*
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
* Title       : Maint Set Hotel ID.                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program
*             : Hotel Model.        
* Program Name: MATHTID.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function set hotel id.
*
*           The provided function names are as follows: 
* 
*              MaintHotelIdRead()   : read & display Hotel ID
*              MaintHotelIdWrite()  : read & display Hotel ID
*              MaintHotelIdReport() : report & print Hotel ID report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Sep-14-92: 00.00.01 : M.Yamamoto: initial                                   
* Apr-04-94: 00.00.05 : Yoshiko.J : add EPT feature in GPUS
*          :          :           :                                    
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

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintHotelIdRead( PARAHOTELID *pData )
*               
*     Input:    *pData         : ponter to structure for PARAHOTELID
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data
*
** Description: This function reads & displays Hotel ID.
*===========================================================================
*/

SHORT MaintHotelIdRead( PARAHOTELID *pData )
{

    UCHAR uchAddress;

    /* initialize */

/*    memset(MaintWork.HotelId.aszMnemonics, '\0', PARA_HOTELID_LEN+1); */

    /* check status */

    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        uchAddress = ++MaintWork.ParaHotelId.uchAddress;

        /* check address */

        if (uchAddress > HOTELID_ADR_MAX) {                     
            uchAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        uchAddress = pData->uchAddress;
    }        

    /* check address */

    if (uchAddress < 1 || HOTELID_ADR_MAX < uchAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.ParaHotelId.uchMajorClass = pData->uchMajorClass;    /* copy major class */
    MaintWork.ParaHotelId.uchAddress = uchAddress;
    CliParaRead(&(MaintWork.ParaHotelId));                         /* call PARAParaHotelIdRead() */ 
    DispWrite(&(MaintWork.ParaHotelId));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintHotelIdWrite( PARAHOTELID *pData )
*               
*     Input:    *pData         : pointer to structure for PARAParaHotelId
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays Hotel ID.
*===========================================================================
*/
      
SHORT MaintHotelIdWrite( PARAHOTELID *pData )
{
	PARATENDERKEYINFO TendKeyInfo = { 0 };

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    /* Data Check */

    if (MaintWork.ParaHotelId.uchAddress == HOTEL_ID_ADR) {
        memmove(MaintWork.ParaHotelId.aszMnemonics,
                pData->aszMnemonics,
                PARA_HOTELID_LEN * sizeof(TCHAR));
    } else if (MaintWork.ParaHotelId.uchAddress == HOTEL_SLD_ADR) {
        memmove(MaintWork.ParaHotelId.aszMnemonics,
                pData->aszMnemonics,
                PARA_SLD_LEN * sizeof(TCHAR));
    } else if (MaintWork.ParaHotelId.uchAddress == HOTEL_PDT_ADR) {
        memmove(MaintWork.ParaHotelId.aszMnemonics,
                pData->aszMnemonics,
                PARA_PDT_LEN * sizeof(TCHAR));
    } else if (MaintWork.ParaHotelId.uchAddress == HOTEL_APVL_CODE_ADR) {
        memmove(MaintWork.ParaHotelId.aszMnemonics,
                pData->aszMnemonics,
                PARA_APVL_CODE_LEN * sizeof(TCHAR));
    } else {
        memmove(MaintWork.ParaHotelId.aszMnemonics,
                pData->aszMnemonics,
                PARA_TEND_LEN * sizeof(TCHAR));
    }    
	
	/* ---- The tender key information needs to match the hotel id information ---- */
	if( (MaintWork.ParaHotelId.uchAddress >= HOTEL_TEND1_ADR) &&
		(MaintWork.ParaHotelId.uchAddress <= HOTEL_TEND20_ADR)) {

			TendKeyInfo.uchMajorClass = CLASS_PARATENDERKEYINFO;
			TendKeyInfo.uchAddress = (MaintWork.ParaHotelId.uchAddress - HOTEL_TEND1_ADR) + 1;
			CliParaRead(&TendKeyInfo);

			memcpy(&TendKeyInfo.TenderKeyInfo.auchEptTend, 
					MaintWork.ParaHotelId.aszMnemonics,
					PARA_TEND_LEN * sizeof(TCHAR));

			CliParaWrite(&(TendKeyInfo));
	}


    CliParaWrite(&(MaintWork.ParaHotelId));                     /* call PARAParaHotelIdWrite() */

    /* control header item */

    MaintHeaderCtl(PG_HOTELID, RPT_PRG_ADR);

    /* set journal bit & receipt bit */

    MaintWork.ParaHotelId.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.ParaHotelId));

    /* set address for Display next address */

    MaintWork.ParaHotelId.uchAddress++;

    /* check address */

    if (MaintWork.ParaHotelId.uchAddress > HOTELID_ADR_MAX) {
        MaintWork.ParaHotelId.uchAddress = 1;                      /* initialize address */
    }
    MaintHotelIdRead(&(MaintWork.ParaHotelId));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintHotelIdReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints Hotel ID report.
*===========================================================================
*/

VOID MaintHotelIdReport( VOID )
{

    UCHAR     i;
	PARAHOTELID  ParaHotelId = { 0 };

    /* control header */
    MaintHeaderCtl(PG_HOTELID, RPT_PRG_ADR);

    /* set major class */
    ParaHotelId.uchMajorClass = CLASS_PARAHOTELID;

    /* set journal bit & receipt bit */
    ParaHotelId.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= HOTELID_ADR_MAX; i++) {

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        ParaHotelId.uchAddress = i;
        CliParaRead(&ParaHotelId);                             /* call ParaParaHotelIdRead() */
        PrtPrintItem(NULL, &ParaHotelId);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}

