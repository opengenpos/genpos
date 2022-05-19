/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file used in print module
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrtPrty.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Apr-07-95 : 00.00.01 : T.Nakahata : Initial
* Jun-23-95 : 00.00.02 : T.Nakahata : increase reserve area ( 10 -> 20 )
* Aug-10-95 : 00.00.03 : T.Nakahata : add members (lQty, lProduct)
*
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : initial (for Win32)
*
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

/*
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/
/*
============================================================================
+                     PRINT PRIORITY DEFINITIONS
============================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


/* --- status about sales item ( uchPrintClass ) --- */
/* ---  bit 7 6 5 4 3 2 1 0                      --- */
/* ---      | | | | +-+-+-+---- sales item class --- */
/* ---      +-+-+-+------------ print priority   --- */

#define PRT_DEPT            0x00
#define PRT_DEPTITEMDISC    0x01
#define PRT_DEPTMODDISC     0x02
#define PRT_PLU             0x03
#define PRT_PLUITEMDISC     0x04
#define PRT_PLUMODDISC      0x05
#define PRT_SETMENU         0x06
#define PRT_SETITEMDISC     0x07
#define PRT_SETMODDISC      0x08
#define PRT_OEPPLU          0x09
#define PRT_OEPITEMDISC     0x0a
#define PRT_OEPMODDISC      0x0b
#define PRT_OTHER           0x0f

#define PRT_GET_CLASS( uch )    ( UCHAR )((( UCHAR )( uch )) & 0x0f )
#define PRT_GET_PRIORITY( uch ) ( UCHAR )(((( UCHAR )( uch )) >> 4 ) & 0x0f )
#define PRT_MAKE_PRICLASS( pri, cls )   \
            (( UCHAR )((( UCHAR )( cls )) | ((( UCHAR )(( UCHAR )( pri ))) << 4 )))


/* --- size of index file --- */
/* --- usIndexFSize = sizeof( PRTIDXHDR ) + usIndexVli + Reserved Area --- */
/* --- usIndexVli   = sizeof( PRTIDX ) * uchNoOfItem --- */

typedef struct {    /* structure for header of item index file */
    TrnFileSize    usIndexFSize;   /* total size, in bytes, of index file */
    TrnVliOffset   usIndexVli;     /* actual data size of index file */
    UCHAR          uchNoOfItem;    /* maximum no. of items in index file */
/* following members are available when use print priority */
    UCHAR          uchNoOfSales;   /* maximum no. of sales items in index file */
    TrnVliOffset   usSalesHighest; /* index of highest priority sales item */
    TrnVliOffset   usSalesLowest;  /* index of lowest priority sales item */
} PRTIDXHDR;

typedef struct {    /* structure for sales item index */
    TrnVliOffset  usItemOffset;   /* sales item offset from top of trans file */
    UCHAR         uchSeatNo;      /* seat no for remote print, V3.3 */
    UCHAR         uchPrintClass;  /* HIBIT : Print Priority, LOBIT :Item Class */
    USHORT        usDeptNo;    /* Sales Item No. ( Dept No. ) 2172 */
    TCHAR         auchPLUNo[NUM_PLU_LEN]; /* Sales Item No. ( PLU No. ) 2172 */
    LONG          lQTY;           /* quantity of sales items */
    DCURRENCY     lProduct;       /* product amount of sales items */
    SHORT         sNetQTY;        /* quantity of sales items for no-cons, V3.3 */
	UCHAR         uchMajorClass;  // the major class from the item information
	UCHAR         uchMinorClass;  // the minor class from the item information
	USHORT        usUniqueID;     // item's unique identifier, unique within this transaction.
	USHORT        usKdsLineNo;    // line number for the KDS for this item.
} PRTIDX;

#define PRTIDX_RESERVE  20       // the reserve for a file full error to allow for total and tender.
#define PRTIDX_MAXITEM  ( 200 + PRTIDX_RESERVE )
#define PRTIDX_MAXSIZE  ( sizeof( PRTIDX ) * PRTIDX_MAXITEM )

#define PRTIDX_OFFSET(ndx) (sizeof(PRTIDXHDR)+(sizeof(PRTIDX)*(ndx)))

#define MAX_CONS_IDX_FILE_SIZE (sizeof(PRTIDXHDR) + sizeof(PRTIDX) * (FLEX_ITEMSTORAGE_MAX + PRTIDX_RESERVE))


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*--------------------------------------------------------------------------
*        prototype (TRANSACTION STORAGE MODULE)
--------------------------------------------------------------------------*/

SHORT TrnStoSetupPrtIdx( PRTIDX *pPrtIdx, VOID   *pvTranNon, TrnVliOffset  usItemOffset );
SHORT TrnStoCompareIndex( PRTIDX *pPrtIdx1, PRTIDX *pPrtIdx2 );
SHORT TrnStoLinerSearch( USHORT usStorageType );
BOOL  TrnStoIsValidIndex( PRTIDX *pPrtIdx );

/* ===== End of File ( PrtPrty.H ) ===== */
