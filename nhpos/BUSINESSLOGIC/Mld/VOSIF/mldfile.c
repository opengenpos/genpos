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
* Title       : Multiline Display Storage File Control
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDFILE.C
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Apl-05-95 : 03.00.00 : M.Ozawa    : Initial
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
#include	<tchar.h>
#include    <string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <trans.h>
#include    <prt.h>
#include    <csgcs.h>
#include    <csstbgcf.h>
#include    <csttl.h>
#include    <csstbttl.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <appllog.h>
#include    <prtprty.h>
#include    <mld.h>
#include    "mldlocal.h"
#include "termcfg.h"

TCHAR CONST  aszMldStorage1File[]  = _T("MLDSTOR1");     /* file Name of storage */
TCHAR CONST  aszMldStorage1Index[] = _T("MLDINDX1");     /* mld storage index */
TCHAR CONST	aszMldStorage1Temp[]  = _T("MLDTMPSTO");	/* Temporary store file for condiment editing*/

/*
*==========================================================================
**    Synopsis: SHORT   MldCreateFile(USHORT usNoOfItem, UCHAR uchType)
*
*       Input:  USHORT  usNoOfItem - Number of Item
*            :  UCHAR   uchType - Storage Type
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*           Normal End: MLD_SUCCESS
*           Error End:  MLD_ERROR
**  Description:
*           create multiline display storage file.
*==========================================================================
*/
SHORT MldCreateFile( USHORT usNoOfItem, UCHAR uchStorageType )
{
    CONST TCHAR *lpszFileName;
    SHORT       hsStorageFile;
    SHORT       sReturn;
    USHORT      usInquirySize;
	TRANCONSSTORAGEHEADER    InitFileHeaderData = { 0 };

    /* --- function always create specified file, if it is existing --- */

    switch ( uchStorageType ) {
		case FLEX_ITEMSTORAGE_ADR:
			return (MLD_SUCCESS);   /* not create as item storage file */

		case FLEX_CONSSTORAGE_ADR:
			lpszFileName = aszMldStorage1File;
			break;
		
		case FLEX_TMP_FILE_ID:
			lpszFileName = aszMldStorage1Temp;
			break;

		default:
			NHPOS_ASSERT_TEXT(0, "**WARNING: MldCreateFile() invalid uchStorageType specified.");
			return ( MLD_ERROR );
    }

    /* ---- close storage file and delete all windows --- */
    if (uchMldSystem) {
        if (uchMldSystem != MLD_SUPER_MODE) {   /* except supervisor mode */
            MldRefresh();
        }
    }

	PifDeleteFile( lpszFileName );

    hsStorageFile = PifOpenFile( lpszFileName, auchTEMP_NEW_FILE_READ_WRITE );  /* saratoga */

    if ( hsStorageFile < 0 ) {
        return ( MLD_ERROR );
    }

    /* --- store size of actual data --- */
	memset (&InitFileHeaderData, 0, sizeof(InitFileHeaderData));
    MldWriteFile(0, &InitFileHeaderData, sizeof(InitFileHeaderData), hsStorageFile );

    /* --- allocate file size to store specified no. of items --- */
    usInquirySize = MldCalStoSize( usNoOfItem );
    sReturn = MldExpandFile( hsStorageFile, usInquirySize );

    if (sReturn < 0) {
		PifCloseFile( hsStorageFile );
		PifDeleteFile( lpszFileName );
        return(MLD_ERROR);
    }

	PifCloseFile( hsStorageFile );

    /* --- create index file for print priority --- */
    sReturn = MldCreateIndexFile( usNoOfItem, uchStorageType, NULL );

    if ( sReturn != MLD_SUCCESS ) {
		PifDeleteFile( lpszFileName );
        return ( sReturn );
    }

    return ( MLD_SUCCESS );
}

/*
*==========================================================================
**  Synopsis :  SHORT MldCreateIndexFile( USHORT usNoOfItem,
*                                         UCHAR  uchType,
*                                         ULONG  *pulCreatedSize )
*
*       Input:  USHORT usNoOfItem   - number of items in storage file
*            :  UCHAR  uchType      - type of storage file to create
*                       FLEX_ITEMSTORAGE_ADR    : item storage
*                       FLEX_CONSSTORAGE_ADR    : consolidate storage
*               ULONG  *pulCreatedSize - size of created index file
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*               MLD_SUCCESS -   file creation is successful
*               MLD_ERROR   -   file creation is failed
**  Description:
*           create multiline display index file for storage control.
*==========================================================================
*/
SHORT MldCreateIndexFile( USHORT usNoOfItem,
                          UCHAR  uchType,
                          ULONG  *pulCreatedSize )
{
    CONST TCHAR *lpszIndexName;
    SHORT       hsIndexFile;
    PRTIDXHDR   IdxFileInfo;
    SHORT       sSeekStatus;
    USHORT       usInquirySize;

    switch ( uchType ) {
		case FLEX_ITEMSTORAGE_ADR:
		case FLEX_TMP_FILE_ID:
			return (MLD_SUCCESS);   /* not create as item storage file */

		case FLEX_CONSSTORAGE_ADR:
			lpszIndexName = aszMldStorage1Index;
			break;

		default:
			return ( MLD_ERROR );
    }

    /* ---- close storage file and delete all windows --- */
    if (uchMldSystem) {
        if (uchMldSystem != MLD_SUPER_MODE) {   /* except supervisor mode */
            MldRefresh();
        }
    }

    /* --- delete current existing index file before new file creation --- */
	PifDeleteFile( lpszIndexName );

    /* --- allocate index file with specified size --- */
    hsIndexFile = PifOpenFile( lpszIndexName, auchTEMP_NEW_FILE_READ_WRITE );   /* saratoga */

    usInquirySize = sizeof( PRTIDXHDR ) + ( sizeof( PRTIDX ) * ( usNoOfItem + PRTIDX_RESERVE ));

    sSeekStatus = MldExpandFile( hsIndexFile, usInquirySize );

    /* --- determine index file creation is successful or not --- */
    if ( sSeekStatus != MLD_SUCCESS ) {
		PifCloseFile( hsIndexFile );
		PifDeleteFile( lpszIndexName );
        return ( MLD_ERROR );
    }

    /* --- store current created file size to formal parameter --- */

    if ( pulCreatedSize != NULL ) {
        *pulCreatedSize = (ULONG)usInquirySize;
    }

    /* --- initialize file header and save it to index file --- */

    IdxFileInfo.usIndexFSize   = usInquirySize;
    IdxFileInfo.usIndexVli     = 0;
    IdxFileInfo.uchNoOfItem    = 0;
    IdxFileInfo.uchNoOfSales   = 0;
    IdxFileInfo.usSalesHighest = 0;
    IdxFileInfo.usSalesLowest  = 0;

    MldWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), hsIndexFile );

	PifCloseFile( hsIndexFile );

    return ( MLD_SUCCESS );
}

/*
*==========================================================================
**    Synopsis: SHORT   MldChkAndCreFile(USHORT usNoOfItem, UCHAR uchType)
*
*       Input:  USHORT  usNoOfItem - Number of Item
*            :  UCHAR   uchType - Storage Type
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*           Normal End: MLD_SUCCESS
*           Error End:  MLD_ERROR
*
**  Description:
*           Create specified files if the file is not exist.
*==========================================================================
*/
SHORT MldChkAndCreFile( USHORT usNoOfItem, UCHAR uchStorageType )
{
    CONST TCHAR *lpszFileName;
    SHORT       hsStorageFile;
    SHORT       sReturn;

    /* --- open current existing file --- */
    switch ( uchStorageType ) {
		case FLEX_ITEMSTORAGE_ADR:
			return (MLD_SUCCESS);   /* not create as item storage file */

		case FLEX_CONSSTORAGE_ADR:
			lpszFileName = aszMldStorage1File;
			break;

		case FLEX_TMP_FILE_ID:
			lpszFileName = aszMldStorage1Temp;
			break;

		default:
			return ( MLD_ERROR );
    }

    /* ---- close storage file and delete all windows --- */
    if (uchMldSystem) {
        if (uchMldSystem != MLD_SUPER_MODE) {   /* except supervisor mode */
            MldRefresh();
        }
    }

    hsStorageFile = PifOpenFile( lpszFileName, auchTEMP_OLD_FILE_READ );    /* saratoga */

    if ( 0 <= hsStorageFile ) { /* file is already existing */
		PifCloseFile( hsStorageFile );
        sReturn = MLD_SUCCESS;
    } else {                    /* file is not existing */
        sReturn = MldCreateFile( usNoOfItem, uchStorageType );
    }
    return ( sReturn );
}

/*
*==========================================================================
**    Synopsis: VOID    MldChkAndDelFile(USHORT usNoOfItem, UCHAR uchType)
*
*       Input:  USHORT  usNoOfItem - Number of Item
*            :  UCHAR   uchType - Storage Type
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*           Delete specified files if the size of the file is
*           different from the parameter 'usNoOfItem'.
*==========================================================================
*/
VOID MldChkAndDelFile( USHORT usNoOfItem, UCHAR uchStorageType )
{
    CONST TCHAR *lpszStorageName;
    CONST TCHAR *lpszIndexName;
    SHORT       hsStorageFile;
    USHORT      usActualSize;
    USHORT      usInquirySize;
	ULONG		ulActualBytesRead;//RPH SR# 201

    /* --- open current existing storage file --- */

    switch ( uchStorageType ) {
		case FLEX_ITEMSTORAGE_ADR:
			return;             /* not create as item storage file */
			break;

		case FLEX_CONSSTORAGE_ADR:
			lpszStorageName = aszMldStorage1File;
			lpszIndexName   = aszMldStorage1Index;
			break;

		default:
			return;
    }

    /* ---- close storage file and delete all windows --- */
    if (uchMldSystem) {
        if (uchMldSystem != MLD_SUPER_MODE) {   /* except supervisor mode */
            MldRefresh();
        }
    }

    hsStorageFile = PifOpenFile( lpszStorageName, auchTEMP_OLD_FILE_READ ); /* saratoga */

    if ( hsStorageFile < 0 ) {  /* file is not existing */
        return;
    }

    /* --- retrieve actual size of storage file --- */
    MldReadFile( 0, &usActualSize, sizeof( USHORT ), hsStorageFile, &ulActualBytesRead);
	PifCloseFile( hsStorageFile );

    /* --- calculate inquiry size of storage file --- */
    usInquirySize = MldCalStoSize( usNoOfItem );

    /* --- if inquiry size is different from actual size, delete
        current existing storage file --- */
    if ( usInquirySize != usActualSize ) {
		PifDeleteFile( lpszStorageName );
		PifDeleteFile( lpszIndexName );
    }
}

/*
*==========================================================================
**    Synopsis: SHORT   MldOpenFile( VOID )
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*           Normal End: MLD_SUCCESS
*           Error End:  LDT_PROHBT_ADR
*
**  Description:
*           Open item, consoli, post rec. each files and save file handle.
*==========================================================================
*/
SHORT MldStoOpenFile( VOID )
{
	ULONG	ulActualBytesRead;//RPH SR# 201

	MldScroll1Buff.sFileHandle = PifOpenFile(aszMldStorage1File, auchTEMP_OLD_FILE_READ_WRITE);   /* saratoga */
	if (MldScroll1Buff.sFileHandle < 0) {   /* saratoga */
		return(LDT_PROHBT_ADR);
    }

	MldScroll1Buff.sTempFileHandle = PifOpenFile(aszMldStorage1Temp, auchTEMP_OLD_FILE_READ_WRITE);   /* saratoga */
	if (MldScroll1Buff.sTempFileHandle < 0) {   /* saratoga */
		PifCloseFile(MldScroll1Buff.sFileHandle);
		MldScroll1Buff.sFileHandle = -1;
		return(LDT_PROHBT_ADR);
    }

    /* --- open index files of item/consolidate storage --- */
	MldScroll1Buff.sIndexHandle = PifOpenFile(aszMldStorage1Index, auchTEMP_OLD_FILE_READ_WRITE); /* saratoga */
	if (MldScroll1Buff.sIndexHandle < 0) {
		PifCloseFile(MldScroll1Buff.sFileHandle);
		PifCloseFile(MldScroll1Buff.sTempFileHandle);
		MldScroll1Buff.sTempFileHandle = MldScroll1Buff.sFileHandle = -1;
		return(LDT_PROHBT_ADR);
	}

	// get the file size 
    MldReadFile(0, &MldScroll1Buff.usStoFSize, sizeof(MldScroll1Buff.usStoFSize), MldScroll1Buff.sFileHandle, &ulActualBytesRead);

	return (MLD_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: VOID    MldCloseFile( VOID )
*
*       Input:  Nothing
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*           Close multiline display each files.
*==========================================================================
*/
VOID MldStoCloseFile( VOID )
{
	PifCloseFile(MldScroll1Buff.sFileHandle); MldScroll1Buff.sFileHandle = -1;
	PifCloseFile(MldScroll1Buff.sIndexHandle); MldScroll1Buff.sIndexHandle = -1;
	PifCloseFile(MldScroll1Buff.sTempFileHandle); MldScroll1Buff.sTempFileHandle = -1;
}

/*==========================================================================
*    Synopsis:  VOID    MldStorageInit(MLDCURTRNSCROL USHORT usType )
*
*    Input:     USHORT  usType
*    Output:    none
*    InOut:     none
*
*    Return:    none
*
*    Description:   initialize specified transaction information
==========================================================================*/

VOID MldStorageInit(MLDCURTRNSCROL *pData, USHORT usType )
{
    USHORT      usHeadSize;
	PRTIDXHDR   IdxFileInfo = { 0 };
	ULONG		ulActualBytesRead;//RPH SR# 201

    memset (&MldTempStorage, 0x00, sizeof(MLDTEMPSTORAGE));

    if ((pData->sFileHandle < 0) || (pData->sIndexHandle < 0)) {
        return;
    }

    memset (&(pData->usStoVli), 0x00, sizeof(MLDCURTRNSCROL) - MLDSCROLLHEAD);
    if ( usType == MLD_PRECHECK_SYSTEM ) {  /* initialize as item storage */
        usHeadSize = sizeof(TRANITEMSTORAGEHEADER);
    } else {                                /* initialize as cons. storage */
        usHeadSize = sizeof(TRANCONSSTORAGEHEADER);
    }

    pData->usStoHSize = usHeadSize + sizeof(pData->usStoVli);    /* add sizeof usStoVli */
    pData->usStoVli = 0;
    MldWriteFile(usHeadSize, &(pData->usStoVli), sizeof(USHORT), pData->sFileHandle);

    /* --- initialize print priority index file --- */
    MldReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), pData->sIndexHandle, &ulActualBytesRead);

    IdxFileInfo.usIndexVli     = 0;
    IdxFileInfo.uchNoOfItem    = 0;
    IdxFileInfo.uchNoOfSales   = 0;
    IdxFileInfo.usSalesHighest = 0;
    IdxFileInfo.usSalesLowest  = 0;

    MldWriteFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), pData->sIndexHandle );
}

/*
*===========================================================================
** Synopsis:    USHORT MldCalStoSize( USHORT usNoOfItem)
*
*   Input:      USHORT usNoOfItem     - number of items in storage file
*   Output:     Nothing
*   InOut:      Nothing
*
** Return:      size of storage file in bytes.
*
** Description: calculate size of storage file with specified no. of items.
*               See also functions FDTCalStoSize() and TrnCalStoSize()
*               which perform a similar calculation.
*===========================================================================
*/
USHORT MldCalStoSize( USHORT usNoOfItem )
{
    ULONG   ulActualSize = TrnCalStoSize(usNoOfItem, FLEX_CONSSTORAGE_ADR);

	NHPOS_ASSERT(ulActualSize < 0xffff);

    return ( (USHORT)ulActualSize );
}

/**===========================================================================
** Synopsis:    USHORT MldGetTrnStoToMldSto(USHORT usVliSize, SHORT sTrnFileHandle,
*                       SHORT sTrnIndexHandle, SHORT sMldFileHandle, SHORT sMldIndexHandle)
*
*     Input:    USHORT  usVliSize
*               SHORT   sTrnFileHandle
*               SHORT   sTrnIndexHandle
*               SHORT   sMldFileHandle
*               SHORT   sMldIndexHandle
*    Output:    nothing
*
** Return:      MLD_SUCCESS, MLD_ERROR
*
** Description: copy transaction storage file to mld storage file
*
*               There are some differences between how the MLD file is
*               used and how the transaction file is used.
*
*               The most important difference is that the index file of the
*               transaction file contains more data than the MLD file.  The MLD
*               file contains basically just the transaction item data.
*
*               The index structure PRTIDXHDR is common to both files but the 
*               MLD file read/write structures do not use two of the structure
*               members, usSalesHighest (index of highest priority sales item)
*               and usSalesLowest, index of lowest priority sales item.  This
*               two members point to the first and last index entries in the 
*               transaction file index for the sales information in the transaction
*               file.
*===========================================================================*/
USHORT MldGetTrnStoToMldSto(USHORT usVliSize, SHORT sTrnFileHandle, SHORT sTrnIndexHandle,
                SHORT sMldFileHandle, SHORT sMldIndexHandle)
{
    ULONG           ulReadOffset;
    ULONG           ulWriteOffset;
    USHORT          usTtlVliSize;
    ULONG           ulTtlWriteSize;
    USHORT          usTry, usRemind;
	USHORT      usItemNumber;
    PRTIDXHDR   IdxFileInfo;
    UCHAR       auchTempStoWork[TRN_STOSIZE_BLOCK];
	ULONG       ulActualBytesRead;//RPH SR# 201

    /* --- copy storage's transaction data to mld storage file --- */

    /* calculate copy size */
    if (uchMldSystem == MLD_PRECHECK_SYSTEM) {
        usTtlVliSize = sizeof(TRANITEMSTORAGEHEADER) + usVliSize;
    } else {
        usTtlVliSize = sizeof(TRANCONSSTORAGEHEADER) + usVliSize;
    }

    ulReadOffset = 0;
    ulWriteOffset  = 0;
    ulTtlWriteSize = 0;

    usTry = usTtlVliSize / sizeof(auchTempStoWork);
    usRemind = usTtlVliSize % sizeof(auchTempStoWork);

    do {
        //RPH SR# 201
		MldReadFile( ulReadOffset, auchTempStoWork, (usTry ? sizeof(auchTempStoWork) : usRemind),
                     sTrnFileHandle, &ulActualBytesRead );

		if (ulActualBytesRead < 1)
			break;

        if (MldWriteFile( ulWriteOffset, auchTempStoWork, ulActualBytesRead, sMldFileHandle ) != MLD_SUCCESS) {
            return (MLD_ERROR);
        }
        ulReadOffset   += ulActualBytesRead;
        ulWriteOffset  += ulActualBytesRead;
        ulTtlWriteSize += ulActualBytesRead;
        usTry--;
    } while ( ulTtlWriteSize < usTtlVliSize );

    /* --- write consolidate's print priority index to post receipt --- */

    //RPH SR# 201
	MldReadFile( 0, &IdxFileInfo, sizeof( PRTIDXHDR ), sTrnIndexHandle, &ulActualBytesRead );

    ulReadOffset = 0;
    ulWriteOffset  = 0;
    ulTtlWriteSize = 0;

    usTry = IdxFileInfo.usIndexFSize / sizeof(auchTempStoWork);
    usRemind = IdxFileInfo.usIndexFSize % sizeof(auchTempStoWork);

    //RPH SR# 201
	MldReadFile( ulReadOffset, &IdxFileInfo, sizeof( PRTIDXHDR ), sTrnIndexHandle, &ulActualBytesRead );

    ulWriteOffset  = 0;
    ulTtlWriteSize = 0;

	// We have copied over the the actual transaction data and now we
	// need to copy over the index file.  The index file provides us the
	// means to find a particular item in the transaction.
	usItemNumber = 0;
	ulReadOffset = sizeof(PRTIDXHDR) + (sizeof(PRTIDX) * usItemNumber);
	while (usItemNumber < IdxFileInfo.uchNoOfItem) {
		//RPH SR# 201
		MldReadFile(ulReadOffset, auchTempStoWork, sizeof(PRTIDX), sTrnIndexHandle, &ulActualBytesRead);

		if (ulActualBytesRead < 1)
			break;

		ulWriteOffset = (sizeof (PRTIDXHDR) + (sizeof (PRTIDX) * usItemNumber));
		if (MldWriteFile(ulWriteOffset, auchTempStoWork, ulActualBytesRead, sMldIndexHandle ) != MLD_SUCCESS) {
			return (MLD_ERROR);
		}
		ulTtlWriteSize += ulActualBytesRead;
		ulReadOffset += ulActualBytesRead;
		usItemNumber++;
	}

	MldWriteFile (0, &IdxFileInfo, sizeof( PRTIDXHDR ), sMldIndexHandle );
    return (MLD_SUCCESS);
}

/*==========================================================================
*    Synopsis:  SHORT   MldCopyGcfToStorage(USHORT usScroll, SHORT sFileHandle)
*
*    Input:     USHORT  usScroll
*               SHORT   sFileHandle
*    Output:    none
*    InOut:     none
*
*    Return:    MLD_SUCCESS -   Function is successful
*               MLD_ERROR   -   Function is failed
*
*    Description:
*       This function copys storage data of gcf file to mld storage file.
==========================================================================*/

SHORT   MldCopyGcfToStorage(USHORT usScroll, SHORT sFileHandle)
{
    PRTIDXHDR   IdxFileInfo;
	TRANCONSSTORAGEHEADER  TranConStorage;
    UCHAR       auchTempStoWork[ sizeof( TRANGCFQUAL ) ];
    ULONG       ulReadOffset;
    ULONG       ulReadOffset2;
    USHORT      usConsStoVli;
	USHORT      usItemNumber;
    ULONG           ulReadLen;
    ULONG           ulWriteOffset;
    ULONG           ulTtlVliSize;
    ULONG           ulTtlWriteSize;
    USHORT      usTry, usRemind;
	ULONG		ulActualBytesRead;

#if DISPTYPE==1
    if (usScroll == MLD_SCROLL_2) {

        MldScroll2Buff.sFileHandle = sFileHandle;
        MldScroll2Buff.sIndexHandle = sFileHandle;
    } else if (usScroll == MLD_SCROLL_3) {

        MldScroll3Buff.sFileHandle = sFileHandle;
        MldScroll3Buff.sIndexHandle = sFileHandle;
    }
#endif

    /* --- retrieve specified GCF into consolidate storage file --- */

    /* --- set retrieved qualifier/itemizer to Transaction Information --- */

    //RPH SR# 201
	MldReadFile( 0, &TranConStorage, sizeof(TranConStorage), sFileHandle, &ulActualBytesRead );

    /* display gcf qualifier data */
    MldDispGCFQualifier(usScroll, &TranConStorage.TranGCFQual);

	usConsStoVli = TranConStorage.usVli;

    ulReadOffset = sizeof(TranConStorage);

    if (usScroll == MLD_SCROLL_1) {
        MldScroll1Buff.usStoVli = usConsStoVli;

        ulTtlVliSize = ulReadOffset + usConsStoVli;

        ulReadOffset2 = 0;
        ulWriteOffset  = 0;
        ulTtlWriteSize = 0;

        usTry = (USHORT)ulTtlVliSize/sizeof(auchTempStoWork);
        usRemind = (USHORT)ulTtlVliSize%sizeof(auchTempStoWork);

        do {
            //RPH SR# 201
			MldReadFile( ulReadOffset2, auchTempStoWork, (USHORT)(usTry ? sizeof(auchTempStoWork) : usRemind), sFileHandle, &ulReadLen);

            if (MldWriteFile( ulWriteOffset, auchTempStoWork, ulReadLen, MldScroll1Buff.sFileHandle ) != MLD_SUCCESS) {
                return (MLD_ERROR);
            }
            ulReadOffset2  += ulReadLen;
            ulWriteOffset  += ulReadLen;
            ulTtlWriteSize += ulReadLen;
            usTry--;

        } while ( ulTtlWriteSize < ulTtlVliSize );

    }

    /* --- get item index information, and save it to index file --- */

    if (usConsStoVli) { /* if index file data exitsted */

        ulReadOffset += usConsStoVli;
#if DISPTYPE==1
        if (usScroll == MLD_SCROLL_2) {
			NHPOS_ASSERT(ulReadOffset <= 0xFFFF);//MAXWORD
            MldScroll2Buff.usIndexHeadOffset = (USHORT)ulReadOffset;
        } else if (usScroll == MLD_SCROLL_3) {
			NHPOS_ASSERT(ulReadOffset <= 0xFFFF);//MAXWORD
            MldScroll3Buff.usIndexHeadOffset = (USHORT)ulReadOffset;
        } else {
            MldScroll1Buff.usIndexHeadOffset = 0;
        }
#elif DISPTYPE==2
        MldScroll1Buff.usIndexHeadOffset = 0;
#endif

        if (usScroll == MLD_SCROLL_1) {
            //RPH SR# 201
			MldReadFile( ulReadOffset, &IdxFileInfo, sizeof( PRTIDXHDR ), sFileHandle, &ulActualBytesRead );

            ulWriteOffset  = 0;
            ulTtlWriteSize = 0;

			// We have copied over the the actual transaction data and now we
			// need to copy over the index file.  The index file provides us the
			// means to find a particular item in the transaction.
			usItemNumber = 0;
			ulReadOffset += sizeof(PRTIDXHDR) + (sizeof(PRTIDX) * usItemNumber);
			while (usItemNumber < IdxFileInfo.uchNoOfItem) {
				//RPH SR# 201
				MldReadFile(ulReadOffset, auchTempStoWork, sizeof(PRTIDX), sFileHandle, &ulActualBytesRead);

				if (ulActualBytesRead < 1)
					break;

				ulWriteOffset = (sizeof (PRTIDXHDR) + (sizeof (PRTIDX) * usItemNumber));
				if (MldWriteFile( ulWriteOffset, auchTempStoWork, ulActualBytesRead, MldScroll1Buff.sIndexHandle ) != MLD_SUCCESS) {
					return (MLD_ERROR);
				}
				ulTtlWriteSize += ulActualBytesRead;
				ulReadOffset += ulActualBytesRead;
				usItemNumber++;
			}

			MldWriteFile (0, &IdxFileInfo, sizeof( PRTIDXHDR ), MldScroll1Buff.sIndexHandle );
        }
    } else {    /* not index file data exitsted */

#if DISPTYPE==1
        if (usScroll == MLD_SCROLL_2) {
            MldScroll2Buff.sIndexHandle = -1;
        } else if (usScroll == MLD_SCROLL_3) {
            MldScroll3Buff.sIndexHandle = -1;
        }
#endif

        return (MLD_ERROR);
    }
    return ( MLD_SUCCESS );
}


/*
*==========================================================================
**    Synopsis: SHORT   MldExpandFile( SHORT  hsFileHandle,
*                                      USHORT usInquirySize )
*
*       Input:  SHORT   hsFileHandle  - file handle
*               USHORT  usInquirySize - size of file to expand
*      Output:  Nothing
*
**  Return   :
*           Normal End: MLD_SUCCESS
*           Error End : MLD_ERROR
*
**  Description:
*           Expand specified file size
*==========================================================================
*/
SHORT MldExpandFile( SHORT hsFileHandle, USHORT usInquirySize )
{
#if 1
	return TrnExpandFile( hsFileHandle, usInquirySize);
#else
    ULONG   ulActualSize;
    SHORT   sReturn;

    sReturn = PifSeekFile( hsFileHandle, ( ULONG )usInquirySize, &ulActualSize );

    if (( sReturn < 0 ) || (( ULONG )usInquirySize != ulActualSize )) {
        return ( MLD_ERROR );
    }

    /* --- store file size to created file --- */
    sReturn = PifSeekFile( hsFileHandle, 0UL, &ulActualSize );

    if (( sReturn < 0 ) || ( ulActualSize != 0UL )) {
        return ( MLD_ERROR );
    }

    PifWriteFile( hsFileHandle, &usInquirySize, sizeof( USHORT ));

    return ( MLD_SUCCESS );
#endif
}

/*
*==========================================================================
**    Synopsis: SHORT  MldReadFile(ULONG ulOffset, VOID *pData,
*                                   ULONG ulSize, SHORT hsFileHandle
									ULONG *pulActualBytesRead)
*
*       Input:  ULONG   ulOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to read data buffer.
*               ULONG   ulSize - Number of bytes to be read.
*               SHORT   hsFileHandle - File Handle.
				ULONG	*pulActualBtesRead - Number of bytes Read
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  MLD_ERROR
				MLD_NO_READ_SIZE
				MLD_SUCCESS
*
**  Description:
*           Read data from item, consoli., pst rec. file.

  RPH 11-10-3 Changes for PifReadFile

  Now returns Success/Error  Size Read is in pulActualBytesRead
*==========================================================================
*/
SHORT TrnReadFile_MemoryForceIfFile (ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle,
				   ULONG *pulActualBytesRead);

SHORT MldReadFile(ULONG ulOffset, VOID *pData,
                   ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead)
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;

    if (ulSize == 0) {          /* check read size */
        return(MLD_NO_READ_SIZE);          /* if read nothing, return */
    }

//    sReturn = PifSeekFile( hsFileHandle, 0UL, &ulActPos );
//    if (( sReturn < 0 ) || ( ulActPos != 0UL )) {
//        return (MLD_ERROR);
//    }

//    sReturn = PifReadFile( hsFileHandle, &usFSize, sizeof( USHORT ), pulActualBytesRead);
//    if (( *pulActualBytesRead != sizeof( USHORT )) || ( usFSize < ulOffset )) {
//        return (MLD_ERROR);
//    }

	if (TrnReadFile_MemoryForceIfFile (ulOffset, pData, ulSize, hsFileHandle, pulActualBytesRead) >= 0)
		return (MLD_SUCCESS);

    sReturn = PifSeekFile( hsFileHandle, ulOffset, &ulActPos );
    if (( sReturn < 0 ) || (ulOffset != ulActPos )) {
		return (MLD_ERROR);
    }

	PifReadFile(hsFileHandle, pData, ulSize, pulActualBytesRead);
	if(*pulActualBytesRead != ulSize){
		// NOTE: Most callers will just gnore the return value from MldReadFile() since
		//       if the read is near the end of the transaction file, there may be a final
		//       record which is smaller than the requested buffer size but still a valid record.
		return(MLD_ERROR);
	}

    return (MLD_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: VOID    TrnWriteFile(USHORT usOffset, VOID *pData,
*                                    USHORT usSize, SHORT hsFileHandle)
*
*       Input:  USHORT  usOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to write data buffer.
*               USHORT  usSize - Number of bytes to be written.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*
**  Description:
*           Write data to item, consoli. post rec. file.
*==========================================================================
*/
SHORT MldWriteFile(ULONG ulOffset, VOID *pData,
                  ULONG ulSize, SHORT hsFileHandle)
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;
//	ULONG	ulActualBytesRead; //RPH 11-10-3

    if (ulSize == 0) {                  /* check writed size */
        return (MLD_ERROR);       /* if nothing, return */
    }

	// The following code designed to check the actual file size against
	// the expected file size stored in the file was commented out years
	// ago because the two no longer agreed.
	// With changes in data structures stored in the transaction file,
	// the file size became larger than a USHORT and the file size stored
	// in the file as a check was no longer correct.
	// This change was made both to this function, MldWriteFile()
	// and the corresponding transaction subsystem function, TrnWriteFile().
	//    Richard Chambers, Jul-22-2021

//    sReturn = PifSeekFile( hsFileHandle, 0UL, &ulActPos );
//    if (( sReturn < 0 ) || ( ulActPos != 0UL )) {
//        return (MLD_ERROR);
//    }

	//RPH 11-10-3 Changes for PifReadFile
//    sReturn = PifReadFile( hsFileHandle, &usFSize, sizeof( USHORT ), &ulActualBytesRead);
//    if (( ulActualBytesRead != sizeof( USHORT )) || ( usFSize < ( ulOffset + ulSize ))) {
//        return (MLD_ERROR);
//    }

    sReturn = PifSeekFile( hsFileHandle, ulOffset, &ulActPos);
    if (( sReturn < 0 ) || (ulOffset != ulActPos )) {
        return (MLD_ERROR);
    }

    PifWriteFile(hsFileHandle, pData, ulSize);

    return (MLD_SUCCESS);
}



/****** End of Source ******/
