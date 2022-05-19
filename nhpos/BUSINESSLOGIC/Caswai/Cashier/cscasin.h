/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server CASHIER module, Header file for internal
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCASIN.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories:
*
*    Date           Ver.Rev :NAME       :Description
*    May-06-92     :00.00.01:M.YAMAMOTO :Initial
*    Jun-06-93     :00.00.01:H.YAMAGUCHI:Change No of cashier from 16 to 32
*    Dec-06-95     :03.01.00:M.Ozawa    :Add Cas_IndTermLockCheck()
*
*
*** NCR2172 ***
*
*    Oct-04-99     :01.00.00:M.Teraki   : Initial
*                                         Add #pragma pack(...)
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#define    CAS_SEARCH_ONLY            1        /* FOR Cas_Index() */
#define    CAS_REGIST                 2        /* FOR Cas_Index() */

#define    CAS_CONT                   1
#define    CAS_END                    2

#define CAS_MAX_WORK_BUFFER         512         /* Max. size of work buffer */
#define    CAS_OPEN_PICKUP_LOAN       0x0010    /* Open  for Pickup Loan */

#define CAS_FILE_HED_POSITION       0x0L        /* for file rewind */

#define CAS_COUNTER_OF_SEM           1          /* counter of semaphore */

// The cashier file index uses a bitmap to determine which blocks are in use.
// Following define assumes 8 bits per byte and rounds up to calculate the index size in bytes
#define CAS_PARA_EMPTY_TABLE_SIZE   (CAS_NUMBER_OF_MAX_CASHIER / 8) + ((7 + (CAS_NUMBER_OF_MAX_CASHIER%8)) / 8)

/*
===========================================================================
    TYPEDEF
===========================================================================
*/

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
    USHORT  usSeqNO;                /* Sequnece number */
    ULONG   offulFilePosition;      /* Write and Read Position (Number of byte from file head) */
    UCHAR   uchStatus;              /* Status EOF, CONTINUE */
}CAS_BACKUP;

typedef struct {
    USHORT  usDataLen;              /* Data length  */
    UCHAR   auchData[1];            /* Cashier file data  */
}CAS_BAKDATA;

typedef struct{
    ULONG   ulCasFileSize;
    USHORT  usNumberOfMaxCashier;       /* V3.3 */
    USHORT  usNumberOfResidentCashier;  /* V3.3 */
    USHORT  offusOffsetOfIndexTbl;
    USHORT  offusOffsetOfParaEmpTbl;
    USHORT  offusOffsetOfParaTbl;
}CAS_FILEHED;

typedef struct{
    ULONG   ulCashierNo;
    USHORT  usParaBlock;
}CAS_INDEXENTRY;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*----------------------
    File Name
----------------------*/

/*----------------------
    Intarnal Prototype
----------------------*/
/*
===========================================================================
    MEMORY
===========================================================================
*/


/*========= End of Define =========*/
