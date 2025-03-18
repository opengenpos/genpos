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
    TCHAR       const * const lpszFileName1 = aszFDTFile1;
    TCHAR       const * const lpszFileName2 = aszFDTFile2;
    PifFileHandle   hsFile1;
    PifFileHandle   hsFile2;
    ULONG           ulInquirySize = TrnCalStoSize(usNoOfItem, FLEX_CONSSTORAGE_ADR);

    PifDeleteFile(lpszFileName1);
    PifDeleteFile(lpszFileName2);

    if ((hsFile1 = PifOpenFile(lpszFileName1, auchTEMP_NEW_FILE_READ_WRITE)) < 0) { /* saratoga */
        return(FDT_ERROR);
    }

    if (TrnExpandFile(hsFile1, ulInquirySize) < 0) {
        PifCloseFile(hsFile1);
        PifDeleteFile(lpszFileName1);
        return(FDT_ERROR);
    }

    if ((hsFile2 = PifOpenFile(lpszFileName2, auchTEMP_NEW_FILE_READ_WRITE)) < 0) { /* saratoga */
        PifCloseFile(hsFile1);
        PifDeleteFile(lpszFileName1);
        return(FDT_ERROR);
    }

    if (TrnExpandFile(hsFile2, ulInquirySize) < 0) {
        PifCloseFile(hsFile1);
        PifDeleteFile(lpszFileName1);
        PifCloseFile(hsFile2);
        PifDeleteFile(lpszFileName2);
        return(FDT_ERROR);
    }

    /* --- Store Actual Size --- */
    {
        TrnVliOffset    usInitialVli = 0;
        TrnFileSize     usVliOffset = CLIGCFFILE_DATASTART + sizeof(TRANGCFQUAL) + sizeof(TRANITEMIZERS);  // TRANCONSSTORAGEHEADER
        TrnWriteFile(usVliOffset, &usInitialVli, sizeof(TrnVliOffset), hsFile1);
        TrnWriteFile(usVliOffset, &usInitialVli, sizeof(TrnVliOffset), hsFile2);
    }
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
    PifFileHandle   hsStorageFile;
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
    PifFileHandle   hsStorageFile;
    TrnFileSize     usActualSize;
    ULONG           ulInquirySize = TrnCalStoSize(usNoOfItem, FLEX_CONSSTORAGE_ADR);   /* --- calculate inquiry size of storage file --- */
	ULONG		    ulActualBytesRead;//RPH 11-10-3 SR# 201

    if ((hsStorageFile = PifOpenFile(aszFDTFile1, auchTEMP_OLD_FILE_READ)) < 0) {   /* saratoga */
        return;
    }

    /* --- retrieve actual size of storage file --- */
    //RPH 11-10-3 SR# 201
    TrnReadFile(0, &usActualSize, sizeof(TrnFileSize), hsStorageFile, &ulActualBytesRead);
    PifCloseFile(hsStorageFile);

    /* --- if inquiry size is different from actual, delete existing storage file --- */
    if (ulInquirySize != usActualSize) {
        PifDeleteFile(aszFDTFile1);
    }

    if ((hsStorageFile = PifOpenFile(aszFDTFile2, auchTEMP_OLD_FILE_READ)) < 0) {   /* saratoga */
        return;
    }

    /* --- retrieve actual size of storage file --- */
    //RPH 11-10-3 SR# 201
    TrnReadFile(0, &usActualSize, sizeof(TrnFileSize), hsStorageFile, &ulActualBytesRead);
    PifCloseFile(hsStorageFile);

    /* --- if inquiry size is different from actual, delete existing storage file --- */
    if (ulInquirySize != usActualSize) {
        PifDeleteFile(aszFDTFile2);
    }
}


/*========= End of File =========*/