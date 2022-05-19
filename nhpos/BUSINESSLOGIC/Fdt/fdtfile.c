/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Flexible Drive Through Thread
*   Category           : Flexible Drive Through Thread, NCR2170 US HOSPITALITY MODEL.
*   Program Name       : FDTFILE.C
*  ------------------------------------------------------------------------
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.
*   Memory Model       : Medium Model
*   Options            : /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as follows
*
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME     :Description
*   Apr-17-95:        :hkato      : Initial.
*
*** NCR2171 **
*   Aug-26-99:01.00.00:M.Teraki   : initial (for Win32)
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>

#include    <string.h>
#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "plu.h"
#include    "paraequ.h"
#include    "para.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "trans.h"
#include    "prt.h"
#include    "prtprty.h"
#include    "storage.h"
#include    "csgcs.h"
#include    "csstbgcf.h"
#include    "csstbfcc.h"
#include    "csstbstb.h"
#include    "csstbpar.h"
#include    "fdt.h"
#include    "fdtlocal.h"
#include    "appllog.h"


/*
*==========================================================================
**  Synopsis:   SHORT   FDTCreateFile(USHORT usNoOfItem)
*                                              
*       Input:  USHORT  usNoOfItem - Number of Item
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :
*           Normal End: FDT_SUCCESS
*           Error End:  FDT_ERROR
*
**  Description:    Create FDT File.
*==========================================================================
*/
SHORT   FDTCreateFile(USHORT usNoOfItem)
{
    TCHAR FAR   *lpszFileName1;
    TCHAR FAR   *lpszFileName2;
    SHORT       hsFile1;
    SHORT       hsFile2;
    USHORT      usInquirySize;
    USHORT      usInitialVli1;
    USHORT      usInitialVli2;
    USHORT      usVliOffset;

    lpszFileName1 = aszFDTFile1;
    lpszFileName2 = aszFDTFile2;

    PifDeleteFile(lpszFileName1);
    PifDeleteFile(lpszFileName2);

    if ((hsFile1 = PifOpenFile(lpszFileName1, auchTEMP_NEW_FILE_READ_WRITE)) < 0) { /* saratoga */
        return(FDT_ERROR);
    }
    usInquirySize = FDTCalStoSize(usNoOfItem);
    if (FDTExpandFile(hsFile1, usInquirySize) < 0) {
        PifCloseFile(hsFile1);
        PifDeleteFile(lpszFileName1);
        return(FDT_ERROR);
    }

    if ((hsFile2 = PifOpenFile(lpszFileName2, auchTEMP_NEW_FILE_READ_WRITE)) < 0) { /* saratoga */
        PifCloseFile(hsFile1);
        PifDeleteFile(lpszFileName1);
        return(FDT_ERROR);
    }

    if (FDTExpandFile(hsFile2, usInquirySize) < 0) {
        PifCloseFile(hsFile1);
        PifDeleteFile(lpszFileName1);
        PifCloseFile(hsFile2);
        PifDeleteFile(lpszFileName2);
        return(FDT_ERROR);
    }

    /* --- Store Actual Size --- */
    usInitialVli1 = usInitialVli2 = 0;
    usVliOffset = sizeof(USHORT) + sizeof(USHORT) + sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS);  // TRANCONSSTORAGEHEADER
    FDTWriteFile(usVliOffset, &usInitialVli1, sizeof(USHORT), hsFile1);
    FDTWriteFile(usVliOffset, &usInitialVli2, sizeof(USHORT), hsFile2);
    PifCloseFile(hsFile1);
    PifCloseFile(hsFile2);

    return(FDT_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: SHORT   FDTChkAndCreFile(USHORT usNoOfItem, UCHAR uchType)
*                                                  
*       Input:  USHORT  usNoOfItem - Number of Item
*            :  UCHAR   uchType - Storage Type
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Normal End: FDT_SUCCESS
*               Error End:  FDT_ERROR                                                       
*                                                                  
**  Description:
*           Create specified files if the file is not exist.
*==========================================================================
*/
SHORT   FDTChkAndCreFile(USHORT usNoOfItem)
{
    SHORT       hsStorageFile;
    SHORT       sReturn;    

    if ((hsStorageFile = PifOpenFile(aszFDTFile1, auchTEMP_OLD_FILE_READ)) >= 0) {  /* saratoga */
        PifCloseFile(hsStorageFile);
        sReturn = FDT_SUCCESS;
    } else {
        sReturn = FDTCreateFile(usNoOfItem);
    }

    if ((hsStorageFile = PifOpenFile(aszFDTFile2, auchTEMP_OLD_FILE_READ)) >= 0) {  /* saratoga */
        PifCloseFile(hsStorageFile);
        sReturn = FDT_SUCCESS;
    } else {
        sReturn = FDTCreateFile(usNoOfItem);
    }

    return(sReturn);
}    

/*
*==========================================================================
**    Synopsis: VOID    FDTChkAndDelFile(USHORT usNoOfItem)
*                                              
*       Input:  USHORT  usNoOfItem - Number of Item
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Nothing
*                                                                  
**  Description:    Delete specified files if the size of the file is
*                   different from the parameter 'usNoOfItem'.
*==========================================================================
*/
VOID    FDTChkAndDelFile(USHORT usNoOfItem)
{
    SHORT       hsStorageFile;
    USHORT      usActualSize;
    USHORT      usInquirySize;
	ULONG		ulActualBytesRead;//RPH 11-10-3 SR# 201

    if ((hsStorageFile = PifOpenFile(aszFDTFile1, auchTEMP_OLD_FILE_READ)) < 0) {   /* saratoga */
        return;
    }

    /* --- retrieve actual size of storage file --- */
    //RPH 11-10-3 SR# 201
	FDTReadFile(0, &usActualSize, sizeof(USHORT), hsStorageFile, &ulActualBytesRead);
    PifCloseFile(hsStorageFile);

    /* --- calculate inquiry size of storage file --- */
    usInquirySize = FDTCalStoSize(usNoOfItem);

    /* --- if inquiry size is different from actual, delete existing storage file --- */
    if (usInquirySize != usActualSize) {
        PifDeleteFile(aszFDTFile1);
    }

    if ((hsStorageFile = PifOpenFile(aszFDTFile2, auchTEMP_OLD_FILE_READ)) < 0) {   /* saratoga */
        return;
    }

    /* --- retrieve actual size of storage file --- */
    //RPH 11-10-3 SR# 201
	FDTReadFile(0, &usActualSize, sizeof(USHORT), hsStorageFile, &ulActualBytesRead);
    PifCloseFile(hsStorageFile);

    /* --- if inquiry size is different from actual, delete existing storage file --- */
    if (usInquirySize != usActualSize) {
        PifDeleteFile(aszFDTFile2);
    }
}


/*
*==========================================================================
**    Synopsis: SHORT   FDTExpandFile(SHORT hsFileHandle, USHORT usInquirySize)
*                                                        
*       Input:  SHORT   hsFileHandle  - file handle
*               USHORT  usInquirySize - size of file to expand
*      Output:  Nothing
*
**  Return   :  
*                                                                  
**  Description:    Expand specified file size
*==========================================================================
*/
SHORT   FDTExpandFile(SHORT hsFileHandle, USHORT usInquirySize)
{
#if 1
	return TrnExpandFile( hsFileHandle, usInquirySize);
#else
    ULONG   ulActualSize;
    SHORT   sReturn;

    sReturn = PifSeekFile(hsFileHandle, (ULONG)usInquirySize, &ulActualSize);

    if ((sReturn < 0) || ((ULONG)usInquirySize != ulActualSize)) {
        return(FDT_ERROR);
    }

    /* --- store file size to created file --- */
    sReturn = PifSeekFile(hsFileHandle, 0UL, &ulActualSize);

    if ((sReturn < 0) || (ulActualSize != 0UL)) {
        return(FDT_ERROR);
    }

    PifWriteFile(hsFileHandle, &usInquirySize, sizeof(USHORT));

    return(FDT_SUCCESS);
#endif
}

/*
*==========================================================================
**    Synopsis: USHORT  FDTReadFile(ULONG ulOffset, VOID *pData,
*                                   ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead)
*                                                        
*       Input:  ULONG  ulOffset - Number of bytes from the beginning of the file.
*               VOID    *pData - Pointer to read data buffer.
*               ULONG  ulSize - Number of bytes to be Read.
*               SHORT   hsFileHandle - File Handle.
				ULONG	*pulActualBytesRead - Number of Bytes Read
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :   FDT_NO_READ_SIZE
				 FDT_ERROR
				 FDT_SUCCESS
*
**  Description:    Read data from file.

  11-10-3 RPH Changes for PifReadFile
  FDTReadFile Now returns only an Error/Success Condition
  but the Number of bytes Read is passed back by pulActualBytesRead
*==========================================================================
*/
SHORT  FDTReadFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsFileHandle, ULONG *pulActualBytesRead)
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;

    if (!ulSize) {          /* check read size */
        return(FDT_NO_READ_SIZE);          /* if read nothing, return */
    }

#if 0
	// POSSIBLE_DEAD_CODE to indicate this is candidate for deletion.
	//
	// remove this check on the file size.  similar removal has been
	// done for the following functionality:
	//  MldReadFile()
	//  TrnReadFile()
    sReturn = PifSeekFile(hsFileHandle, 0UL, &ulActPos);
    if ((sReturn < 0) || (ulActPos != 0UL)) {
        return(FDT_ERROR);
    }

//    sReturn = PifReadFile(hsFileHandle, &usFSize, sizeof(USHORT), pulActualBytesRead);
    if ((*pulActualBytesRead != sizeof(USHORT)) || (usFSize < ulOffset)) {
        return(FDT_ERROR);
    }
#endif

    sReturn = PifSeekFile(hsFileHandle, ulOffset, &ulActPos);
    if ((sReturn < 0) || (ulOffset != ulActPos)) {
        return(FDT_ERROR);
    }

	PifReadFile(hsFileHandle, pData, ulSize, pulActualBytesRead);
	if(*pulActualBytesRead != ulSize){
		return(FDT_ERROR);
	}

    return(FDT_SUCCESS);
}

/*
*==========================================================================
**    Synopsis: VOID    FDTWriteFile(ULONG ulOffset, VOID *pData,
*                                    ULONG ulSize, SHORT hsFileHandle)
*                                                        
*       Input:  ULONG  ulOffset - Number of bytes from the biginning of the file.
*               VOID    *pData - Pointer to write data buffer.
*               ULONG  ulSize - Number of bytes to be written.
*               SHORT   hsFileHandle - File Handle.
*      Output:  Nothing
*       InOut:  Nothing
*
**  Return   :  Normal End: FDT_SUCCESS
*               Error End:  FDT_ERROR                                                       
*
**  Description:    Write data to file.

  11-10-3 RPH Changes for PifReadFile
*==========================================================================
*/
SHORT   FDTWriteFile(ULONG ulOffset, VOID *pData, ULONG ulSize, SHORT hsFileHandle)
{
    SHORT   sReturn;
    ULONG   ulActPos;
//    USHORT  usFSize;
//	ULONG	ulActualBytesRead;

    if (!ulSize) {                  /* check writed size */
        return(FDT_ERROR);       /* if nothing, return */
    }

#if 0
	// POSSIBLE_DEAD_CODE to indicate this is candidate for deletion.
	//
	// remove this check on the file size.  similar removal has been
	// done for the following functionality:
	//  MldWriteFile()
	//  TrnWriteFile()
    sReturn = PifSeekFile(hsFileHandle, 0UL, &ulActPos);
    if ((sReturn < 0) || (ulActPos != 0UL)) {
        return(FDT_ERROR);
    }

	// FILE_SIZE_CHECK
//    sReturn = PifReadFile(hsFileHandle, &usFSize, sizeof(USHORT), &ulActualBytesRead);
    if ((ulActualBytesRead != sizeof(USHORT)) || (usFSize < (ulOffset + ulSize))) {
        return(FDT_ERROR);
    }
#endif

    sReturn = PifSeekFile(hsFileHandle, ulOffset, &ulActPos);
    if ((sReturn < 0) || (ulOffset != ulActPos)) {
        return(FDT_ERROR);
    }

    PifWriteFile(hsFileHandle, pData, ulSize);

    return(FDT_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    USHORT FDTCalStoSize(USHORT usNoOfItem)
*
*   Input:      USHORT usNoOfItem     - number of items in storage file
*   Output:     Nothing
*   InOut:      Nothing
*
** Return:      size of storage file in bytes.
*
** Description: calculate size of storage file with specified no. of items.
*               See also functions MldCalStoSize() and TrnCalStoSize() which
*               perform a similar calculation.
*===========================================================================
*/
USHORT  FDTCalStoSize(USHORT usNoOfItem)
{
    ULONG   ulActualSize = TrnCalStoSize(usNoOfItem, FLEX_CONSSTORAGE_ADR);

	NHPOS_ASSERT(ulActualSize < 0xffff);
    return((USHORT)ulActualSize);
}

/*========= End of File =========*/