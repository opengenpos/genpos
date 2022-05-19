/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Storage Save Control
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDSTOR.C
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Apl-05-95 : 03.00.00 : M.Ozawa    : Initial
* 2171 for Win32
* Aug-26-99 : 01.00.00 : K.Iwata    : V1.0 Initial
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
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <rfl.h>
#include    <paraequ.h>
#include    <appllog.h>
#include    <prtprty.h>
#include    "vos.h"
#include    <mld.h>
#include    "mldlocal.h"
#include    "termcfg.h"

SHORT MldStoAppendIndex( TRANSTORAGESALESNON *pTranNon,
                         USHORT               usItemOffset);
SHORT MldStoSetupPrtIdx( PRTIDX *pPrtIdx, VOID *pvTranNon, USHORT usItemOffset );

/*==========================================================================
**   Synopsis:  SHORT   MldScrollFileWrite( VOID *MldStorage )    
*                           
*    Input:     VOID    *MldStorage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*                   
*    Description:   transaction storage
==========================================================================*/

static SHORT    MldScrollFileWriteExecute( VOID *pMldStorage )
{
    SHORT    sStatus;

    switch( *( UCHAR *)pMldStorage ) {                           

    case CLASS_ITEMTRANSOPEN :                                  /* transaction open */
//        if (MldScroll1Buff.usStoVli) {
            /* not store duplicated item(reorder) */
            return (MLD_SUCCESS);
//        }
//        sStatus = MldStoOpen( ( ITEMTRANSOPEN * )pMldStorage, 0, 0);
        break;

    case CLASS_ITEMSALES :
        sStatus = MldStoSales( (ITEMSALES *)pMldStorage, 0, 0);
        break;

    case CLASS_ITEMDISC :                                       /* discount */
        sStatus = MldStoDisc( (ITEMDISC *)pMldStorage, 0, 0);
        break;

    case CLASS_ITEMCOUPON :                                     /* coupon */
        sStatus = MldStoCoupon( (ITEMCOUPON *)pMldStorage, 0, 0);
        break;

    case CLASS_ITEMTOTAL :                                      /* total key */
        sStatus = MldStoTotal( ( ITEMTOTAL * )pMldStorage, 0, 0);    
        break;

    case CLASS_ITEMTENDER :                                     /* tender */
        sStatus = MldStoTender( (ITEMTENDER *)pMldStorage, 0, 0);
        break;

    case CLASS_ITEMMISC :                                       /* misc. transaction */
        sStatus = MldStoMisc( (ITEMMISC *)pMldStorage, 0, 0);
        break;
        
    case CLASS_ITEMAFFECTION :                                  /* affection */
        sStatus = MldStoAffect( ( ITEMAFFECTION * )pMldStorage, 0, 0);
        break;

    case CLASS_ITEMMULTI :                                      /* multi check print */
        sStatus = MldStoMulti( ( ITEMMULTI * )pMldStorage, 0, 0);
        break;

	case CLASS_ITEMPREVCOND:
		sStatus = MldStoSalesPreviousCondiment( (ITEMSALES *)pMldStorage, 0 ,0);
		break;

    default :   /* illegal major class */
        sStatus = MLD_ERROR;
    }

    if (sStatus == MLD_SUCCESS) {
        /* reset temporary storage status */
        if (MldScroll1Buff.uchStatus & MLD_ITEMDISC_STORE) {
            MldScroll1Buff.uchStatus &= ~MLD_ITEMDISC_STORE;
        } else {
            MldScroll1Buff.uchStatus &= ~MLD_CURITEM_TMP;
        }
    } else {
        /* set flag for storage overflow */
//        MldScroll1Buff.uchStatus |= MLD_STORAGE_OVERFLOW;
    }

	return sStatus;
}

SHORT    MldScrollFileWrite( VOID *pMldStorage )
{
    SHORT    sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);
                                                                    
    if ((MldScroll1Buff.sFileHandle < 0) || (MldScroll1Buff.sIndexHandle < 0)) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    if (MldScroll1Buff.uchStatus & MLD_STORAGE_OVERFLOW) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    if (((MldScroll1Buff.uchStatus & MLD_CURITEM_TMP ) == 0) &&
		(*(UCHAR *)pMldStorage != CLASS_ITEMPREVCOND)) {
        /* if not display on lcd */
        /* release semaphore */

        PifReleaseSem(husMldSemaphore);
        return (MLD_ERROR);
    }

    sStatus = MldScrollFileWriteExecute (pMldStorage);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return( sStatus );                                          /* return */
}

SHORT MldScrollFileDeleteExecute( VOID *pMldStorage)
{
	UCHAR       uchNoOfItem;
    PRTIDXHDR   IdxFileInfo;
    PRTIDX      PrtIdxMid;
    SHORT       sMiddleIdx;
	ULONG		ulActualBytesRead;//RPH SR# 201
	USHORT      hsIndexFile = -1;
	TRANSTORAGESALESNON *pTranNon = (TRANSTORAGESALESNON *)pMldStorage;

    /* --- append print priority index to bottom of index file --- */
	hsIndexFile = MldScroll1Buff.sIndexHandle;

    //RPH SR# 201
	MldReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile, &ulActualBytesRead );

	uchNoOfItem = IdxFileInfo.uchNoOfItem;

	for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
		//RPH 11-11-3 SR# 201
		MldReadFile( (sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * sMiddleIdx )), (VOID *)&PrtIdxMid, (USHORT)sizeof( PRTIDX ), hsIndexFile, &ulActualBytesRead );

		// Okay, we have found an existing order declaration item so
		// we replace it with the new one.
		if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
			PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass &&
			PrtIdxMid.usUniqueID == pTranNon->usUniqueID) {
			memset (&PrtIdxMid, 0, sizeof(PRTIDX));
			MldWriteFile( (sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * sMiddleIdx )), &PrtIdxMid, sizeof( PRTIDX ), hsIndexFile );
			return (MLD_SUCCESS);
		}
	}

    return MLD_SUCCESS; /* ### 2171 for Win32 V1.0 ADD */
}

SHORT    MldScrollFileDelete( VOID *pMldStorage )
{
    SHORT    sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);
                                                                    
    if ((MldScroll1Buff.sFileHandle < 0) || (MldScroll1Buff.sIndexHandle < 0)) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    if (MldScroll1Buff.uchStatus & MLD_STORAGE_OVERFLOW) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    sStatus = MldScrollFileDeleteExecute (pMldStorage);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return( sStatus );                                          /* return */
}


SHORT    MldScrollFileWriteSpl ( VOID *pMldStorage )
{
    SHORT    sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);
                                                                    
    if ((MldScroll1Buff.sFileHandle < 0) || (MldScroll1Buff.sIndexHandle < 0)) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    if (MldScroll1Buff.uchStatus & MLD_STORAGE_OVERFLOW) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    if (*(UCHAR *)pMldStorage == 0) {
        /* if major class is zero */
        /* release semaphore */

        PifReleaseSem(husMldSemaphore);
        return (MLD_ERROR);
    }

    sStatus = MldScrollFileWriteExecute (pMldStorage);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return( sStatus );                                          /* return */
}


/*==========================================================================
**   Synopsis:  SHORT   MldScrollFileWrite( VOID *MldStorage )    
*                           
*    Input:     VOID    *MldStorage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*                   
*    Description:   transaction storage
==========================================================================*/

SHORT    MldCheckSize( VOID *pMldStorage, USHORT usSize2 )
{
    SHORT    sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);
                                                                    
    if ((MldScroll1Buff.sFileHandle < 0) || (MldScroll1Buff.sIndexHandle < 0)) {
        /* not get handle value */
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    if (MldScroll1Buff.uchStatus & MLD_STORAGE_OVERFLOW) {
        /* release semaphore */
        PifReleaseSem(husMldSemaphore);
        return(MLD_ERROR);
    }

    switch( *( UCHAR *)pMldStorage ) {                           
		case CLASS_ITEMTRANSOPEN :                                  /* transaction open */
			sStatus = MldStoOpen( ( ITEMTRANSOPEN * )pMldStorage, usSize2, 1);
			break;

		case CLASS_ITEMSALES :
			sStatus = MldStoSales( (ITEMSALES *)pMldStorage, usSize2, 1);
			break;

		case CLASS_ITEMDISC :                                       /* discount */
			sStatus = MldStoDisc( (ITEMDISC *)pMldStorage, usSize2, 1);
			break;

		case CLASS_ITEMCOUPON :                                     /* coupon */
			sStatus = MldStoCoupon( (ITEMCOUPON *)pMldStorage, usSize2, 1);
			break;

		case CLASS_ITEMTOTAL :                                      /* total key */
			sStatus = MldStoTotal( ( ITEMTOTAL * )pMldStorage, usSize2, 1);    
			break;

		case CLASS_ITEMTENDER :                                     /* tender */
			sStatus = MldStoTender( (ITEMTENDER *)pMldStorage, usSize2, 1);
			break;

		case CLASS_ITEMMISC :                                       /* misc. transaction */
			sStatus = MldStoMisc( (ITEMMISC *)pMldStorage, usSize2, 1);
			break;
        
		case CLASS_ITEMAFFECTION :                                  /* affection */
			sStatus = MldStoAffect( ( ITEMAFFECTION * )pMldStorage, usSize2, 1);
			break;

		case CLASS_ITEMMULTI :                                      /* multi check print */
			sStatus = MldStoMulti( ( ITEMMULTI * )pMldStorage, usSize2, 1);    
			break;
    
		default : 
			sStatus = MLD_ERROR;                                    /* illegal major class */
			break;
    }

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return( sStatus );                                          /* return */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoOpen( ITEMTRANSOPEN *ItermTransOpen )    
*   
*    Input:     ITEMTRANSOPEN   *ItemTransOpen
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store transaction open data
==========================================================================*/
        
SHORT MldStoOpen( ITEMTRANSOPEN *pItemTransOpen, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMTRANSOPEN)];
    SHORT   sSize;
    SHORT   sReturn;
    
    sSize = TrnStoOpenComp( pItemTransOpen, &uchBuffer[0] );         /* data compress */

    /* check item, consoli. storage full */
    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoSales( ITEMSALES *ItemSales )    
*
*    Input:     ITEMSALES   *ItemSales
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store sales data
==========================================================================*/

SHORT    MldStoSales( ITEMSALES *pItemSales, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMSALES)];
    SHORT   sSize;
    SHORT   sReturn;

//    if (usType == 0) {
//        pItemSales->usOffset = 0;   /* not save partner offset for void item display */
//    }

    sSize = TrnStoSalesComp( pItemSales, &uchBuffer[0] );            /* data compress */

    /* check item, consoli. storage full */

    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoDisc( ITEMDISC *ItemDisc )    
*
*    Input:     ITEMDISC    *ItemDisc
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store discount data
==========================================================================*/

SHORT    MldStoDisc( ITEMDISC *pItemDisc, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMDISC)];
    SHORT   sSize;
    SHORT   sReturn;
                  
    sSize = TrnStoDiscComp( pItemDisc, &uchBuffer[0] );              /* data compress */
    
    /* check item, consoli. storage full */

    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }
          
    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoDisc( ITEMDISC *ItemDisc )    
*
*    Input:     ITEMDISC    *ItemDisc
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store discount data
==========================================================================*/

SHORT    MldStoCoupon( ITEMCOUPON *pItemCoupon, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMCOUPON)];
    SHORT   sSize;
    SHORT   sReturn;
                  
    sSize = TrnStoCouponComp( pItemCoupon, &uchBuffer[0] );              /* data compress */

    /* check item, consoli. storage full */

    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }
          
    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoTotal( ITEMTOTAL *ItemTotal )    
*
*    Input:     ITEMTOTAL   *ItemTotal
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store total key data
==========================================================================*/

SHORT    MldStoTotal( ITEMTOTAL *pItemTotal, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMTOTAL)];
    SHORT   sSize;
    SHORT   sReturn;

    if (pItemTotal->uchMinorClass == CLASS_TOTAL1) {     /* not save subtotal */
        return (MLD_SUCCESS);
    }  

    sSize = TrnStoTotalComp( pItemTotal, &uchBuffer[0] );        /* data compress */

    /* check item, consoli. storage full */

    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                      /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoTender( ITEMTENDER *ItemTender )    
*
*    Input:     ITEMTENDER  *ItemTender
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none      
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store tender data
==========================================================================*/

SHORT    MldStoTender( ITEMTENDER *pItemTender, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMTENDER)];
    SHORT   sSize;
    SHORT   sReturn;
                  
    sSize = TrnStoTenderComp( pItemTender, &uchBuffer[0] );          /* data compress */

    /* check item, consoli. storage full */
    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoMisc( ITEMMISC *ItemMisc )    
*
*    Input:     ITEMMISC    *ItemMisc
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store misc. transaction data
==========================================================================*/

SHORT    MldStoMisc( ITEMMISC *pItemMisc, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMMISC)];
    SHORT   sSize;
    SHORT   sReturn;
                  
    sSize = TrnStoMiscComp( pItemMisc, &uchBuffer[0] );             /* data compress */

    /* check item, consoli. storage full */
    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                         /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoAffect( ITEMAFFECTION *itemAffection )    
*
*    Input:     ITEMAFFECTION   *ItemAffection
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store affection data
==========================================================================*/

SHORT    MldStoAffect( ITEMAFFECTION *pItemAffection, USHORT usSize2, USHORT usType )
{
    UCHAR   uchBuffer[sizeof(ITEMAFFECTION)];
    SHORT   sSize;
    SHORT   sReturn;
                  
    sSize = TrnStoAffectComp( pItemAffection, &uchBuffer[0] );       /* data compress */

    /* check item, consoli. storage full */
    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: SHORT   MldStoMulti( ITEMMULTI *ItemMulti )    
*                          
*    Input:     ITEMMULTI   *ItemMulti
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store multi check payment print data
==========================================================================*/

SHORT    MldStoMulti( ITEMMULTI *pItemMulti, USHORT usSize2, USHORT usType)
{
    UCHAR   uchBuffer[sizeof(ITEMMULTI)];
    SHORT   sSize;
    SHORT   sReturn;
                  
    sSize = TrnStoMultiComp( pItemMulti, &uchBuffer[0] );            /* data compress */

    /* check item, consoli. storage full */
    if ((sReturn = MldChkStorageSize(sSize, usSize2, usType)) != MLD_SUCCESS) {
        /* clear display */
        /* display "press total key" */
        return ( MLD_ERROR );
    }

    if (usType) {       /* check size only */
        return(MLD_SUCCESS);
    }

    MldStoNormal( &uchBuffer[0], sSize );                   /* store consoli storage    */

    return( MLD_SUCCESS );                                          /* success */
}



/*==========================================================================
**    Synopsis: VOID    MldStoNormal( VOID *pData, SHORT sSize )    
*
*    Input:     VOID    *pData, SHORT   sSize 
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description: write "item" to mld storage
==========================================================================*/

VOID MldStoNormal( VOID *pData, SHORT sSize )
{
    TRANSTORAGESALESNON *pTranNon;
    TRANSTORAGESALESNON  TranNonTemp;
    USHORT               usItemOffset;

    /* --- store source item to item storage file --- */
    usItemOffset = MldScroll1Buff.usStoHSize + MldScroll1Buff.usStoVli;
    MldWriteFile( usItemOffset, pData, sSize, MldScroll1Buff.sFileHandle );

    /* --- determine current system uses print priority --- */
    pTranNon = ( TRANSTORAGESALESNON * )(( UCHAR *)pData + sizeof( UCHAR ));

	// If this is not an ITEMSALES then we will create a dummy for the
	// index entry that will be created.
	if ( pTranNon->uchMajorClass != CLASS_ITEMSALES ) {
		memset (&TranNonTemp, 0, sizeof(TranNonTemp));
		TranNonTemp.uchMajorClass = ((UCHAR *)pData)[1];
		TranNonTemp.uchMinorClass = ((UCHAR *)pData)[2];
		pTranNon = &TranNonTemp;
	}

    /* --- append print priority index to index file --- */
    MldStoAppendIndex( pTranNon, usItemOffset);

    /* --- update size of actual data in item storage file --- */
    MldScroll1Buff.usStoVli += sSize; //PRTIDX_STD_ITEMSIZE;
}

/*==========================================================================
**  Synopsis:   SHORT MldStoAppendIndex( TRANSTORAGESALESNON *pTranNon,
*                                        USHORT               usItemOffset)
*
*   Input:      TRANSTORAGESALESNON *pTranNon       - address of sales data
*               USHORT               usItemOffset   - offset from top of storage
*   Output:     none
*   InOut:      none
*
*   Return:     Mld_SUCCESS - function is successful.
*               TRN_ERROR   - function is failed.
*
*   Description:    append an item index to index file.
==========================================================================*/

SHORT MldStoAppendIndex( TRANSTORAGESALESNON *pTranNon, USHORT usItemOffset)
{
    SHORT       sReturn;
	int         i;
	UCHAR       uchNoOfItem;
    PRTIDX      IndexNew, IndexOld;
    PRTIDXHDR   IdxFileInfo;
    PRTIDX      PrtIdxMid;
    SHORT       sMiddleIdx;
    USHORT      usWriteOffset;
	ULONG		ulActualBytesRead;//RPH SR# 201
	USHORT      hsIndexFile = -1;

    /* --- prepare print priority index to append storage file --- */

    sReturn = MldStoSetupPrtIdx( &IndexNew, pTranNon, usItemOffset );

    if ( sReturn != MLD_SUCCESS ) {
        return ( sReturn );
    }

    /* --- append print priority index to bottom of index file --- */

	hsIndexFile = MldScroll1Buff.sIndexHandle;

    //RPH SR# 201
	MldReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile, &ulActualBytesRead );

	uchNoOfItem = IdxFileInfo.uchNoOfItem;

	// If this is an Order Declaration item then we will check to see if such an
	// item already exists.  If so then we will just change the existing index.
	if (pTranNon != 0 && pTranNon->uchMajorClass == CLASS_ITEMSALES && pTranNon->uchMinorClass == CLASS_ITEMORDERDEC) {

		for (sMiddleIdx = 0; sMiddleIdx < IdxFileInfo.uchNoOfItem; sMiddleIdx++) {
			//RPH 11-11-3 SR# 201
			MldReadFile( (sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * sMiddleIdx )), (VOID *)&PrtIdxMid,
						 (USHORT)sizeof( PRTIDX ), hsIndexFile, &ulActualBytesRead );

			// Okay, we have found an existing order declaration item so
			// we replace it with the new one.
			if (PrtIdxMid.uchMajorClass == pTranNon->uchMajorClass &&
				PrtIdxMid.uchMinorClass == pTranNon->uchMinorClass) {
				MldWriteFile( (sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * sMiddleIdx )),
					&IndexNew, sizeof( PRTIDX ), hsIndexFile );
				return (MLD_SUCCESS);
			}
		}
		// We did not find an existing order declaration item so we are now
		// going to insert this one.
		// We are using the standard that an order declaration is always
		// the first item in the transaction list.  So since this is an
		// order declaration item, we will insert it as the first index.
		for (i = IdxFileInfo.uchNoOfItem - 1; i >= 0; i--) {
			usWriteOffset = MldScroll1Buff.usIndexHeadOffset + ( sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * i ));
			MldReadFile( usWriteOffset, &IndexOld, sizeof( PRTIDX ), hsIndexFile, &ulActualBytesRead );
			usWriteOffset += sizeof( PRTIDX );
			MldWriteFile( usWriteOffset, &IndexOld, sizeof( PRTIDX ), hsIndexFile );
		}
		uchNoOfItem = 0;
    }

    usWriteOffset = MldScroll1Buff.usIndexHeadOffset + ( sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * uchNoOfItem ));

    MldWriteFile( usWriteOffset, &IndexNew, sizeof( PRTIDX ), hsIndexFile );

    /* --- update information about index file --- */
    IdxFileInfo.uchNoOfItem++;
    IdxFileInfo.usIndexVli += sizeof( PRTIDX );

    MldWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), MldScroll1Buff.sIndexHandle );

    return MLD_SUCCESS; /* ### 2171 for Win32 V1.0 ADD */
}


/*
*==========================================================================
**    Synopsis: SHORT   MldChkStorageSize(SHORT sSize)
*                                                  
*       Input:  SHORT   sSize - data size to be written 
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*      Output:  Nothing
*       InOut:  Nothing
*
*    Return:    MLD_SUCCESS     : not file full
*               LDT_TAKETL_ADR  : file full
*                                                                  
**  Description:
*           Check item and consoli. storage file size and input data size.
*==========================================================================
*/
SHORT MldChkStorageSize(SHORT sSize1, USHORT usSize2, USHORT usType)
{
    PRTIDXHDR IdxFileInfo;
	ULONG	ulActualBytesRead;//RPH SR# 201

    /* check index file storage */
    //RPH SR# 201
	MldReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), MldScroll1Buff.sIndexHandle, &ulActualBytesRead );

    if ( (IdxFileInfo.usIndexVli + (usSize2 ? sizeof(PRTIDX)*2 : sizeof(PRTIDX)) ) >
            IdxFileInfo.usIndexFSize - sizeof(PRTIDXHDR) - (usType ? ( sizeof( PRTIDX ) * (PRTIDX_RESERVE-3) ) : 0)) {
                    /* consider reserved area after storage full control */
                    /* subtract 3 for header data by item consolidation */
        return (MLD_ERROR);
    }

    /* check mld storage file vli */
	// We are no longer using the first part of the scroll buffer file to
	// contain the max size of the file.  So do not bother with this check.
	// This check also causes problems if do a transaction then go to Supervisor Mode, do AC23 then
	// return to Register mode and do a transaction.  The size in the scroll file is modified
	// and is set too small so consolidation of the display happens since this function
	// returns an error.
//    if ((MldScroll1Buff.usStoVli + sSize1 + usSize2) > (MldScroll1Buff.usStoFSize - MldScroll1Buff.usStoHSize)) {
//        return ( MLD_ERROR );
//    }

    return(MLD_SUCCESS);                           

}

/*==========================================================================
**  Synopsis: SHORT TrnStoSetupPrtIndex( PRTIDX  *pPrtIdx,
*                                        VOID    *pvTranNon,
*                                        USHORT  usItemOffset )
*
*   Input:    PRTIDX *pPrtIdx       - address of print index
*             VOID   *pvTranNon     - address of sales data
*             USHORT usItemOffset   - offset from top of storage
*   Output:   none
*   InOut:    none
*
*   Return:   TRN_SUCCESS - function is successful.
*             TRN_ERROR   - function is failed.
*
*   Description:    set up print priority index.
==========================================================================*/

SHORT MldStoSetupPrtIdx( PRTIDX *pPrtIdx, VOID *pvTranNon, USHORT usItemOffset )
{
    TRANSTORAGESALESNON *pSalesNon;

    /* set storage file offset data */
    pPrtIdx->usItemOffset  = usItemOffset;

    /* set qty, price data for scroll display */
    pSalesNon = ( TRANSTORAGESALESNON * )pvTranNon;
    if (pSalesNon == NULL) {
		pPrtIdx->usDeptNo      = 0;
        pPrtIdx->lQTY          = 0;
        pPrtIdx->lProduct      = 0;
        pPrtIdx->sNetQTY       = 0; /* V3.3 */
		pPrtIdx->uchMajorClass = 0;
		pPrtIdx->uchMinorClass = 0;
		pPrtIdx->usUniqueID    = 0;
		pPrtIdx->usKdsLineNo   = 0;
		_tcsncpy (pPrtIdx->auchPLUNo, MLD_NONSALESITEM_PLU, STD_PLU_NUMBER_LEN);
	}
	else {
		// The pointer is non-NULL so lets capture the minor class regardless
		// of whether item is sales or non-sales item.
		// Non-sales item are such pseudo items as Total, Cancel, and
		// Order Declare.
		pPrtIdx->uchMajorClass = pSalesNon->uchMajorClass;
		pPrtIdx->uchMinorClass = pSalesNon->uchMinorClass;
		pPrtIdx->usUniqueID    = pSalesNon->usUniqueID;
		pPrtIdx->usKdsLineNo   = pSalesNon->usKdsLineNo;
		pPrtIdx->usDeptNo      = pSalesNon->usDeptNo;
        pPrtIdx->lQTY          = pSalesNon->lQTY;
        pPrtIdx->lProduct      = pSalesNon->lProduct;
        pPrtIdx->sNetQTY       = (SHORT)(pSalesNon->lQTY/1000L); /* V3.3 */
		if (pSalesNon->uchMajorClass == CLASS_ITEMSALES) {
			memcpy (pPrtIdx->auchPLUNo, pSalesNon->auchPLUNo, sizeof(pPrtIdx->auchPLUNo));
		}
		else {
			_tcsncpy (pPrtIdx->auchPLUNo, MLD_NONSALESITEM_PLU, STD_PLU_NUMBER_LEN);
		}
    }

    return ( MLD_SUCCESS );

}

/*==========================================================================
**    Synopsis: SHORT   MldStoSalesPreviousCondiemnt( ITEMSALES *ItemSales )    
*
*    Input:     ITEMSALES   *ItemSales
*               USHORT  usType - 1: check size before write data to storage
*                                0: check size at write data to storage
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS
*               LDT_TAKETL_ADR
*
*    Description:   store condiment to a previous item
==========================================================================*/

SHORT    MldStoSalesPreviousCondiment( ITEMSALES *pItemSales, USHORT usSize2, USHORT usType)
{
	MLDCURTRNSCROL *pMldCurTrnScrol;
    UCHAR     uchBuffer[sizeof(ITEMDATASIZEUNION)];
    SHORT     sSize;
	USHORT    usScroll;
	USHORT    usCurRow, usCurCol;
	USHORT	  i;
	USHORT    uchReadItem, uchReadItemSave;
    PRTIDX    Index;
    UCHAR     auchTempStoWork[sizeof(ITEMSALESDISC)];
    USHORT    usReadOffset, usItemOffset, usReadLen;
	ULONG	  ulActualBytesRead;//RPH SR# 201
	ITEMSALES ItemSales;
    USHORT    readIndex = 0,usReadLocation = 0, usItemCount = 0, usSavedIndex =0;
	USHORT	  usReadSize = 0;
	USHORT    usCursor = 0;

	pItemSales->uchMajorClass = CLASS_ITEMSALES; //assign the class back to ITEMSALES now that we are in this function
    if (usType == 0) {
//        pItemSales->usOffset = 0;   /* not save partner offset for void item display */
    }

	usScroll = MldQueryCurScroll(); //determine which scroll display we are using

	switch (usScroll) {
		case MLD_SCROLL_1:
			pMldCurTrnScrol = &MldScroll1Buff;
			break;

		case MLD_SCROLL_2:
			pMldCurTrnScrol = &MldScroll2Buff;
			break;

		case MLD_SCROLL_3:
			pMldCurTrnScrol = &MldScroll3Buff;
			break;

		default:
			return(MLD_ERROR);
    }

    /* read cursor position item */
    VosGetCurPos(pMldCurTrnScrol->usWinHandle, &usCurRow, &usCurCol); //find out what our position on the display is
    uchReadItem = pMldCurTrnScrol->uchItemLine[usCurRow]; //find out the current Item
    uchReadItemSave = uchReadItem;	//save this number

	if (uchReadItem == MLD_SPECIAL_ITEM) {      /* if cursor directs item discount */
        /* set paraent plu position */
        for (i = (CHAR)(usCurRow - 1); i >= 0; i--) {
            uchReadItem = pMldCurTrnScrol->uchItemLine[i];
            if (uchReadItem != MLD_SPECIAL_ITEM)break;
        }
        if ((uchReadItem == MLD_SPECIAL_ITEM) || (uchReadItem == 0)) {
            for (i = (CHAR)(usCurRow + 1); i <= (CHAR)uchMldBottomCursor; i++) {
                uchReadItem = pMldCurTrnScrol->uchItemLine[i];
                if (uchReadItem != MLD_SPECIAL_ITEM) break;
            }
        }
        if (uchReadItem == MLD_SPECIAL_ITEM) {
            return(MLD_ERROR);
        }
    }

	uchReadItem --; //decrement the readitem, because we are using a zero based array

	usReadOffset = pMldCurTrnScrol->usIndexHeadOffset + ( sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * uchReadItem ));

    //RPH SR# 201
	MldReadFile( usReadOffset, &Index, sizeof( PRTIDX ), pMldCurTrnScrol->sIndexHandle, &ulActualBytesRead );

    /* read itemize data which directed by index file offset */
	MldReadFile(Index.usItemOffset, auchTempStoWork, sizeof(auchTempStoWork), pMldCurTrnScrol->sFileHandle, &ulActualBytesRead);
	
    /* decompress itemize data */
	memset(&ItemSales, 0, sizeof(ITEMSALES));
    		
	//Read from MLD, what is already in the file, and get the size
	usReadLen = RflGetStorageItem(&ItemSales, auchTempStoWork, RFL_WITH_MNEM);

	//Get the new size of the newly edited PLU
    sSize = TrnStoSalesComp( pItemSales, &uchBuffer[0] );            /* data compress */

	// If the new size is less than or equal to the old size then we will
	// just reuse the slot in the MLD file for this item.
	// However if the new item is larger than the existing data then we will
	// need to create a new slot.  The easiest way to do so is to merely add this
	// item to the end of the MLD file and update its index to point to the
	// new slot.
	if(usReadLen < sSize)
	{
		usItemOffset = pMldCurTrnScrol->usStoHSize + pMldCurTrnScrol->usStoVli;
		MldWriteFile (usItemOffset, &uchBuffer[0], sSize, pMldCurTrnScrol->sFileHandle );

		Index.usItemOffset = usItemOffset;

		MldWriteFile(usReadOffset, &Index, sizeof( PRTIDX ), pMldCurTrnScrol->sIndexHandle);

		pMldCurTrnScrol->usStoVli += sSize; //PRTIDX_STD_ITEMSIZE;
	} else
	{
		MldWriteFile( Index.usItemOffset, &uchBuffer[0], sSize, pMldCurTrnScrol->sFileHandle );
	}
	return( MLD_SUCCESS );                                          /* success */
}


/****** End of Source ******/




