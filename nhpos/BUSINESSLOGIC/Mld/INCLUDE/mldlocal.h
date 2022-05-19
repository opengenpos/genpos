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
* Title       : Multiline display module Include file used in this module
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDLOCAL.H
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
* Feb-24-95 : 03.00.00 :  M.Ozawa   : initial
*           :          :            :
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : Added #pragma(...)
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
/* reserved window handle definitions */
//#define MLD_PRECHECK_SCROLL_1_HANDLE    LCDWIN_ID_REG100
//#define MLD_PRECHECK_SCROLL_2_HANDLE    LCDWIN_ID_REG101
//#define MLD_PRECHECK_POP_OEP_HANDLE     LCDWIN_ID_REG102
//#define MLD_PRECHECK_SCROLL_1A_HANDLE   LCDWIN_ID_REG103

//#define MLD_DRIVE_SCROLL_1_HANDLE       LCDWIN_ID_REG200
//#define MLD_DRIVE_SCROLL_2_HANDLE       LCDWIN_ID_REG201
//#define MLD_DRIVE_SCROLL_3_HANDLE       LCDWIN_ID_REG202
//#define MLD_DRIVE_ORDER_1_HANDLE        LCDWIN_ID_REG203
//#define MLD_DRIVE_ORDER_2_HANDLE        LCDWIN_ID_REG204
//#define MLD_DRIVE_ORDER_3_HANDLE        LCDWIN_ID_REG205
//#define MLD_DRIVE_TOTAL_1_HANDLE        LCDWIN_ID_REG206
//#define MLD_DRIVE_TOTAL_2_HANDLE        LCDWIN_ID_REG207
//#define MLD_DRIVE_TOTAL_3_HANDLE        LCDWIN_ID_REG208
//#ifndef TOUCHSCREEN
//#define MLD_DRIVE_POP_OEP_HANDLE        LCDWIN_ID_REG209
//#else
//#define MLD_DRIVE_POP_OEP_HANDLE		LCDWIN_ID_REG102
//#endif //not touchscreen

//#define MLD_SUPER_HANDLE                LCDWIN_ID_SUPER300

/* normal mode display position */
#define MLD_NOR_DSP1A_ROW_POS   0       /* cursor window start row */
#define MLD_NOR_DSP1A_CLM_POS   0       /* cursor window start colum */
//#define MLD_NOR_DSP1A_ROW_LEN   21      /* cursor window row length */ //Moved to termcfg.h
//#define MLD_NOR_DSP1A_CLM_LEN   1       /* cursor window colum length *///Moved to termcfg.h

#define MLD_NOR_DSP1_ROW_POS    0       /* display#1 start row */
#define MLD_NOR_DSP1_CLM_POS    1       /* display#1 start colum */
//#define MLD_NOR_DSP1_ROW_LEN    21      /* display#1 row length *///Moved to termcfg.h
//#define MLD_NOR_DSP1_CLM_LEN    29      /* display#1 colum length was 26 increased for US Customs cwunn 3/19/03*///Moved to termcfg.h

#define MLD_NOR_DSP2_ROW_POS    0       /* display#2 start row */
#define MLD_NOR_DSP2_CLM_POS    30      /* display#2 start colum was 27 increased for US Customs cwunn 3/19/03*/
//#define MLD_NOR_DSP2_ROW_LEN    21      /* display#2 row length *///Moved to termcfg.h
//#define MLD_NOR_DSP2_CLM_LEN    12      /* display#2 colum length *///Moved to termcfg.h

#define MLD_NOR_POP_ROW_POS     0       /* popup start row */
#define MLD_NOR_POP_CLM_POS     0       /* popup start colum */
//#define MLD_NOR_POP_ROW_LEN     21      /* popup row length *///Moved to termcfg.h
//#define MLD_NOR_POP_CLM_LEN     27      /* popup colum length *///Moved to termcfg.h

/* drive through 3 scroll system display position */
#define MLD_DRV3_DSP1_ROW_POS   1       /* display#1 start row */
#define MLD_DRV3_DSP1_CLM_POS   2       /* display#1 start colum */
//#define MLD_DRV3_DSP1_ROW_LEN   19      /* display#1 row length *///Moved to termcfg.h
//#define MLD_DRV3_DSP1_CLM_LEN   12      /* display#1 colum length *///Moved to termcfg.h

#define MLD_DRV3_DSP2_ROW_POS   1       /* display#2 start row */
#define MLD_DRV3_DSP2_CLM_POS   14      /* display#2 start colum */
//#define MLD_DRV3_DSP2_ROW_LEN   19      /* display#2 row length *///Moved to termcfg.h
//#define MLD_DRV3_DSP2_CLM_LEN   12      /* display#2 colum length *///Moved to termcfg.h

#define MLD_DRV3_DSP3_ROW_POS   1       /* display#3 start row */
#define MLD_DRV3_DSP3_CLM_POS   27      /* display#3 start colum */
//#define MLD_DRV3_DSP3_ROW_LEN   19      /* display#3 row length *///Moved to termcfg.h
//#define MLD_DRV3_DSP3_CLM_LEN   12      /* display#3 colum length *///Moved to termcfg.h

#define MLD_DRV3_ORD1_ROW_POS   0       /* order display#1 start row */
#define MLD_DRV3_ORD1_CLM_POS   2       /* order display#1 start colum */
//#define MLD_DRV3_ORD1_ROW_LEN   1       /* order display#1 row length *///Moved to termcfg.h
//#define MLD_DRV3_ORD1_CLM_LEN   12      /* order display#1 colum length *///Moved to termcfg.h

#define MLD_DRV3_ORD2_ROW_POS   0       /* order display#2 start row */
#define MLD_DRV3_ORD2_CLM_POS   14      /* order display#2 start colum */
//#define MLD_DRV3_ORD2_ROW_LEN   1       /* order display#2 row length *///Moved to termcfg.h
//#define MLD_DRV3_ORD2_CLM_LEN   12      /* order display#2 colum length *///Moved to termcfg.h

#define MLD_DRV3_ORD3_ROW_POS   0       /* order display#3 start row */
#define MLD_DRV3_ORD3_CLM_POS   27      /* order display#3 start colum */
//#define MLD_DRV3_ORD3_ROW_LEN   1       /* order display#3 row length *///Moved to termcfg.h
//#define MLD_DRV3_ORD3_CLM_LEN   12      /* order display#3 colum length *///Moved to termcfg.h

#define MLD_DRV3_TTL1_ROW_POS   20      /* total display#1 start row */
#define MLD_DRV3_TTL1_CLM_POS   2       /* total display#1 start colum */
//#define MLD_DRV3_TTL1_ROW_LEN   1       /* total display#1 row length *///Moved to termcfg.h
//#define MLD_DRV3_TTL1_CLM_LEN   12      /* total display#1 colum length *///Moved to termcfg.h

#define MLD_DRV3_TTL2_ROW_POS   20      /* total display#2 start row */
#define MLD_DRV3_TTL2_CLM_POS   14      /* total display#2 start colum */
//#define MLD_DRV3_TTL2_ROW_LEN   1       /* total display#2 row length *///Moved to termcfg.h
//#define MLD_DRV3_TTL2_CLM_LEN   12      /* total display#2 colum length *///Moved to termcfg.h

#define MLD_DRV3_TTL3_ROW_POS   20      /* total display#3 start row */
#define MLD_DRV3_TTL3_CLM_POS   27      /* total display#3 start colum */
//#define MLD_DRV3_TTL3_ROW_LEN   1       /* total display#3 row length *///Moved to termcfg.h
//#define MLD_DRV3_TTL3_CLM_LEN   12      /* total display#3 colum length *///Moved to termcfg.h

/* drive through 3 scroll system popup display position */
#define MLD_DRV3_POP_ROW_POS    0       /* popup start row */
#define MLD_DRV3_POP_CLM_POS    0       /* popup start colum */
//#define MLD_DRV3_POP_ROW_LEN    21      /* popup row length *///Moved to termcfg.h
//#define MLD_DRV3_POP_CLM_LEN    14      /* popup colum length *///Moved to termcfg.h

/* supervisor mode display position */
#define MLD_SUPER_MODE_ROW_POS   0      /* display start row */
#define MLD_SUPER_MODE_CLM_POS   0      /* display start colum */
//#define MLD_SUPER_MODE_ROW_LEN   21     /* display row length *///Moved to termcfg.h
//#define MLD_SUPER_MODE_CLM_LEN   40     /* display colum length *///Moved to termcfg.h

#define MLD_CURRENT_ITEMIZE     -1      /* current itemization position */

/* --- Saratoga --- */
#define MLD_FMT_BUFF_LEN        (40*3)  /* format buffer length for argument conversion*/
#define MLD_FMT_BUFF_LEN2       (40*4)  /* format buffer length for cr/tab conversion */
/* --- Saratoga --- */

#define MLD_UPPER_ARROW         0       /* upper continue display */
#define MLD_LOWER_ARROW         1       /* lower continue display */

// indicators for symbol to display status indicators.
#define MLD_UPPER_SYMBOL        0x18   // indicator symbol for upper contine. see MldDispContinue()
#define MLD_LOWER_SYMBOL        0x19   // indicator symbol for lower continue. see MldDispContinue()
#define MLD_CLEAR_SYMBOL        0x20   // used to clear an indicator symbol. see MldClearContinue()

/* flag definition for each scroll control */
#define MLD_SCROLL_UP           0x01    /* scroll up status */
#define MLD_CURITEM_TMP         0x02    /* flag for temporary buffer */
#define MLD_WAIT_STATUS         0x04    /* wait key used or not */
#define MLD_ITEMDISC_STORE      0x08    /* item discount status for noun storage */
#define MLD_MINUS_10DIGITS      0x10    /* display minus 10 digit at total area */
#define MLD_ALREADY_WRITED      0x20    /* flag for scroll clear control */
#define MLD_STORAGE_OVERFLOW    0x40    /* flag if storage file is overflowed */
#define MLD_EXEC_CONSOL         0x80    /* flag if item consolidation executed */

#define MLD_ZERO_LINE_DISPLAY   0x01    /* zero line is displayed */
#define MLD_MOVE_CURSOR         0x02    /* cursor is moved in the scroll */
#define MLD_UNMATCH_DISPLAY     0x04    /* displayed on scroll without storage */
#define MLD_ORDERDEC_MADE       0x08    /* displayed on scroll without storage */

/* flag definition for special scroll contorl for reg. mode */
#define MLD_REORDER_STATUS      0x01    /* reorder status for service total display */
#define MLD_NEWCHEK_CASHIER     0x02    /* cashier guest check status for charge tips */
#define MLD_REORDER2_STATUS     0x04    /* reorder status for payment display */
#define MLD_CHANGE_STATUS       0x08    /* change display for 2x20 scroll */
#define MLD_TVOID_STATUS        0x10    /* transaction void status */

#define MLD_OUT_OF_SCROLL_AREA  0xFF    // indicates that a cursor is out of range for the displayed scroll area.
#define MLD_SPECIAL_ITEM        0xFF    // indicates that an item exists but is a special item, not in MLD .


//  bit mask flags used with the returned bit mask of function MldTransactionAttributesFound ()
//  These flags provide a means for particular attributes of a MLDCURTRNSCROL buffer
//  to be communicated to other parts of the application.
#define  MLD_TRANS_ATTRIB_ORDERDEC            0x00000001     // indicates order declaration has been written to buffer, MLD_ORDER_DECLARATION

// The following bit mask flags are used with the uchLineStatus[] member of the
// MLDCURTRNSCROL buffer to indicate special conditions for each line whose
// MLD item number is stored in the same array element of uchItemLine[].
#define  MLD_LINESTATUS_FIRSTLINE             0x01           // indicates this is first line for a multiline displayed list
#define  MLD_LINESTATUS_MULTILINE             0x02           // indicates this is a line from a multiline displayed list
#define  MLD_LINESTATUS_CONTLINE              0x04           // indicates this line continues with line 1 of multiline list
#define  MLD_LINESTATUS_SPECIAL_ITEM          0x08           // indicates series of lines associated with MLD_NEW_ITEMIZE2

// following macros are used in the display logic to indicate the form of the display lines.
// these are used to access the two bytes of a USHORT which indicate two different data:
//   - upper byte contains number of display lines for the first complete line of the displayed item data
//   - lower byte contains the total number of display lines for the displayed item
// to see how this data is used when writing to the receipt window display area see function MldWriteScrollDisplay().
#define  MLD_SETLINE_COUNTINCREMENT(x) ((x)+= 0x100)
#define  MLD_SETLINE_COUNT(x)  ((x)>>8)
#define  MLD_SETLINE_BASE(x)  ((x)&0xff)

/*
============================================================================
+                     T Y P E    D E F I N I T I O N S
============================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif


// ** WARNING
//    The first part of this structure, up to and including member
//    usStoVli is kept as persistent storage once initialized.
//    However the latter part of the structure, the part from
//    member usIndexHeadOffset and after, may be zeroed out
//    and reinitialized from time to time during managing of the
//    display.
typedef struct {
	USHORT  usMldBottomCursor;           // last row in displayed scroll window to be used
	USHORT  usMldScrollColumn;           // number of columns of the displayed scroll window
	USHORT  usMldScrollLine;             // number of rows of the displayed scroll window
	USHORT  usScroll;                    // which scroll window am I (MLD_SCROLL_1, etc.)
    USHORT  usWinHandle;                  /* window handle */
    USHORT  usAttrib;                     /* window attribute */
	PifFileHandle   sFileHandle;          /* file handle */
	PifFileHandle   sIndexHandle;         /* index file handle */
    USHORT  usStoFSize;                   /* reserved storage file size */
    USHORT  usStoHSize;                   /* header size of storage file */
	PifFileHandle   sTempFileHandle;      /* Tempfile handle CONDIMENTS JHHJ*/
    USHORT  usStoVli;                     /* actual storage file size */
    USHORT  usIndexHeadOffset;            /* index header offset of GC file */
    USHORT  usStorageOffset;              /* storage offset of GC file */
    UCHAR   uchCurItem;                  /* current itemization */
    UCHAR   uchDispItem;                 /* display itemization for scroll */
    UCHAR   uchItemLine[MLD_NOR_DSP1_ROW_LEN];    /* contains the item number for the item/condiment in a row */
    UCHAR   uchLineStatus[MLD_NOR_DSP1_ROW_LEN];  /* contains the status flags for this item row */
    USHORT  usUniqueID[MLD_NOR_DSP1_ROW_LEN];    /* contains the item's unique identifer for the item/condiment in a row */
    UCHAR   uchInvisiUpperDupItem;       /* invisible line of upper item is same as visible top row item */
    UCHAR   uchInvisiUpperLines;         /* invisible lines of upper item */
    UCHAR   uchInvisiLowerLines;         /* invisible lines of lower item */
    UCHAR   uchLowerItemDisc;            /* invisible line is item discount */
    UCHAR   uchCurCursor;                /* current cursor position */
    UCHAR   uchPreCursor;                /* previous cursor position */
    UCHAR   uchSetCond;                  /* registred condiment */
    UCHAR   uchDispCond;                 /* displayed condiment */
    UCHAR   uchItemDisc;                 /* displayed item discount */
    UCHAR   uchStatus;                   /* flag of scroll buffer status */
    UCHAR   uchStatus2;                  /* flag of scroll buffer status */
    UCHAR   uchSpecialStatus;            /* flag of scroll for reg. mod */
	USHORT  usCurPressed;				 /* Current item pressed */
} MLDCURTRNSCROL;

#define MLDSCROLLHEAD     ((UCHAR *)&(((MLDCURTRNSCROL *)NULL)->usIndexHeadOffset) - (UCHAR *)&(((MLDCURTRNSCROL *)NULL)->usMldBottomCursor))
#define MLDSCROLLDISPLAY  ((UCHAR *)&(((MLDCURTRNSCROL *)NULL)->usCurPressed) - (UCHAR *)&(((MLDCURTRNSCROL *)NULL)->uchCurItem))

#if 0
#define MLDSCROLLHEAD   (sizeof(USHORT) * 7)    /* usWinHandle +
                                                    usAttrib +
                                                    sFileHandle +
                                                    sIndexHandle +
                                                    usStoFSize +
                                                    usStoHSize +
													sTempFileHandle */
#endif

typedef struct {
    USHORT usOrderHandle;               /* order number window handle */
    USHORT usOrderAttrib;               /* order number window attribute */
    USHORT usTotalHandle;               /* total window handle */
    USHORT usTotalAttrib;               /* total window attribute */
} MLDOTHERHANDLE;

/* temporay storage definitions */

typedef struct {
    UCHAR   uchMajorClass;
    UCHAR   uchMinorClass;
    UCHAR   uchNounMajorClass;  /* for item discount e/c */
    UCHAR   uchNounMinorClass;  /* for item discount e/c */
    UCHAR   uchPrevLines;       /* for item discount e/c */
} MLDTEMPSTORAGE;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/*
============================================================================
+                  S T A T I C   R A M s
============================================================================
*/
extern MLDCURTRNSCROL MldScroll1Buff;   /* display#1 data buffer */
extern MLDCURTRNSCROL MldScroll2Buff;   /* display#2 data buffer */
extern MLDCURTRNSCROL MldScroll3Buff;   /* display#3 data buffer */

extern MLDOTHERHANDLE MldOtherHandle1;  /* display #1 data buffer */
extern MLDOTHERHANDLE MldOtherHandle2;  /* display #2 data buffer */
extern MLDOTHERHANDLE MldOtherHandle3;  /* display #3 data buffer */

extern USHORT  usMldPopupHandle;        /* window handle of popup display */
extern UCHAR   uchMldCurScroll;         /* current scroll display */
extern UCHAR   uchMldSystem;            /* current display arrangement */
extern UCHAR   uchMldSystem2;           /* current display arrangement */

extern UCHAR   uchMldScrollColumn1;      /* scroll display column length */
extern UCHAR   uchMldScrollColumn2;      /* scroll display column length */
extern UCHAR   uchMldScrollColumn3;      /* scroll display column length */

extern UCHAR   uchMldScrollLine;        /* scroll display row length */
extern UCHAR   uchMldBottomCursor;      /* scroll display bottom cursor position */

extern USHORT  husMldSemaphore;        /* multiline display semaphore handle */

extern MLDTEMPSTORAGE MldTempStorage;  /* temporary storage buffer data */

/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
MLDCURTRNSCROL * MldPickCurTrnScrol(USHORT usScroll);  /* pick or select MLD scroll buffer */
SHORT   MldLocalDisplayInit(USHORT usDisplay, USHORT usAttrib);  /* Initialize each display */
SHORT   MldLocalECScrollWrite(USHORT usScroll);      /* clear displayed data */
SHORT   MldLocalPutTransToScroll(USHORT usVliSize, PifFileHandle sFileHandle, PifFileHandle sIndexHandle, USHORT usType);    /* set storage file handle */
SHORT   MldLocalMoveCursor(USHORT usType);           /* move cursor to next display */
SHORT   MldLocalSetCursor(USHORT usScroll);
SHORT   MldLocalDispWaitKey(FDTWAITORDER *pData);    /* show order prompt display */


SHORT MldSetPrecheckSystem(USHORT usSystem);
SHORT MldSetDrive3ScrollSystem(USHORT usSystem);
SHORT MldSetSuperModeSystem(VOID);
USHORT MldCreatePrechkScroll1A(UCHAR uchAttr);
USHORT MldCreatePrechkScroll1(UCHAR uchAttr);
USHORT MldCreatePrechkScroll2(UCHAR uchAttr);
USHORT MldCreateDrive3Scroll1(UCHAR uchAttr);
USHORT MldCreateDrive3Scroll2(UCHAR uchAttr);
USHORT MldCreateDrive3Scroll3(UCHAR uchAttr);
USHORT MldCreateSuperScroll1(UCHAR uchAttr);
USHORT MldCreateOrder1(UCHAR uchAttr);
USHORT MldCreateOrder2(UCHAR uchAttr);
USHORT MldCreateOrder3(UCHAR uchAttr);
USHORT MldCreateTotal1(UCHAR uchAttr);
USHORT MldCreateTotal2(UCHAR uchAttr);
USHORT MldCreateTotal3(UCHAR uchAttr);
USHORT MldCreateUpperContinue(UCHAR uchAttr);
USHORT MldCreateLowerContinue(UCHAR uchAttr);
USHORT MldCreatePrechkPopup(UCHAR uchAttr);
USHORT MldCreateDrive3Popup(UCHAR uchAttr);

VOID MldDispOrderEntryPrompt( MLDPOPORDER *pData);

USHORT  MldSPrintf(TCHAR *pszDest, UCHAR uchDestLen, const TCHAR *pszFormat, ...);
SHORT   MldStringAttrSpecial(USHORT usScroll, SHORT sRow, TCHAR *pszString, USHORT usLength, LONG lAttrib);
SHORT	MldStringAttrSpecialColumn(USHORT usScroll, SHORT sRow, TCHAR *pszString, USHORT usLength, LONG *plAttrib);

SHORT    MldWriteScrollDisplay(TCHAR *pszString, USHORT usScroll, USHORT usType, USHORT usRow, USHORT usLength, LONG *plAttrib);


VOID MldSetSpecialStatus(USHORT usScroll, UCHAR uchStatus);
VOID MldResetSpecialStatus(USHORT usScroll, UCHAR uchStatus);
SHORT MldCheckSpecialStatus(USHORT usScroll, UCHAR uchStatus);

VOID MldTempStore(CONST VOID *MldStorage);
SHORT MldTempRestore(VOID *MldStorage);
VOID MldTmpSalesRestore( ITEMSALES *pItemSales);
VOID MldTmpDiscRestore( ITEMDISC *pItemDisc);
UCHAR MldResetItemDiscTmpStore(VOID);
SHORT  MldGetItemSalesDiscData (USHORT usScroll, ITEMSALES *pItemSales, ITEMDISC *pItemDisc, USHORT usItemNumber);

SHORT   MldCreateIndexFile(USHORT usSize, UCHAR uchType, ULONG  *pulCreatedSize);        /* create item/cons./post rec. storage file */
SHORT   MldStoOpenFile(VOID);
VOID    MldStoCloseFile(VOID);
SHORT   MldStoOpen( ITEMTRANSOPEN *ItemTransOpen, USHORT usSize2, USHORT usType);
SHORT   MldStoSales( ITEMSALES *ItemSales, USHORT usSize2, USHORT usType );
SHORT   MldStoDisc( ITEMDISC *ItemDisc, USHORT usSize2, USHORT usType );
SHORT   MldStoTotal( ITEMTOTAL *ItemTotal, USHORT usSize2, USHORT usType );
SHORT   MldStoTender( ITEMTENDER *ItemTender, USHORT usSize2, USHORT usType );
SHORT   MldStoMisc( ITEMMISC *ItemMisc, USHORT usSize2, USHORT usType );
SHORT   MldStoAffect( ITEMAFFECTION *ItemAffection, USHORT usSize2, USHORT usType );
SHORT   MldStoPrint( ITEMPRINT *ItemPrint, USHORT usSize2, USHORT usType );
SHORT   MldStoMulti( ITEMMULTI *ItemMulti, USHORT usSize2, USHORT usType );
SHORT   MldStoCoupon( ITEMCOUPON *pItemCoupon, USHORT usSize2, USHORT usType );
VOID    MldStoNormal( VOID *pData, SHORT sSize );
VOID    MldStorageInit(MLDCURTRNSCROL *pData, USHORT usType);
USHORT MldGetTrnStoToMldSto(USHORT usType, SHORT sTrnFileHandle, SHORT sTrnIndexHandle,
                SHORT sMldFileHandle, SHORT sMldIndexHandle);
SHORT   MldChkStorageSize(SHORT sSize, USHORT usSize2, USHORT usType);                     /* check storage buffer full */
SHORT   MldStoSalesPreviousCondiment( ITEMSALES *pItemSales, USHORT usSize2, USHORT usType);
SHORT   MldExpandFile( SHORT hsFileHandle, USHORT usInquirySize );
SHORT MldReadFile(ULONG ulOffset, VOID *pData,
                        ULONG ulSize, SHORT hsFileHandle,
						ULONG *pulActualBytesRead);
SHORT   MldWriteFile(ULONG ulOffset, VOID *pData,
                         ULONG ulSize, SHORT hsFileHandle);
USHORT MldCalStoSize(USHORT usSize);

VOID    MldDispContinue(USHORT usScroll, USHORT usType);
VOID    MldPrechkDispContinue(USHORT usType);
VOID    MldDriveDispContinue(USHORT usScroll, USHORT usType);
VOID    MldClearContinue(USHORT usScroll, USHORT usType);
VOID    MldPrechkClearContinue(USHORT usType);
VOID    MldDriveClearContinue(USHORT usScroll, USHORT usType);

SHORT   MldCopyGcfToStorage(USHORT usScroll, SHORT sFileHandle);
SHORT   MldLocalPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
SHORT   MldDispLastStoredData(USHORT usScroll);
SHORT   MldDispStoredDataRegion(USHORT usScroll, USHORT usFirstLine);
SHORT   MldDispAllStoredData(USHORT usScroll);
SHORT   MldDispAllReverseData(USHORT usScroll);
BOOL    MldIsSalesIDisc( VOID *puchItem );
VOID    MldSetQualifierStatus(VOID);
VOID    MldSetItemSalesQtyPrice(ITEMSALES *pItem, VOID *pPrtIdx);
SHORT   MldChkDisplayPluNo(CONST ITEMSALES *pItem);
VOID    MldSetSalesCouponQTY(ITEMSALES *pItem, SHORT sFileHandle, USHORT usScroll);
VOID    MldResetItemSalesQtyPrice(ITEMSALES *pItem);
SHORT   MldDispCondiment(USHORT usType);

VOID  MldDispEndMessage(USHORT usType);
SHORT MldDispOrderEntryPrompt2( MLDPOPORDER *pData, USHORT usType);
SHORT MldCheckConsolidation(ITEMSALES *pItem);
SHORT MldSetSalesConsolidation(ITEMSALES *pItem, ITEMSALES *pTempItem);

ULONG  MldTransactionAttributesFound (USHORT usScroll);

SHORT   MldDispGCFQualifier(USHORT usScroll, TRANGCFQUAL *pData);   /* show order# and subtotal from GCF */

/*** End of File ***/
