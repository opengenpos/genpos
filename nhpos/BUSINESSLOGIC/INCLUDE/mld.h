/*
*===========================================================================
* Title       : Multiline display module Include file used by user of this
* Category    : Multiline display, NCR 2170 US Hospitarity Application
* Program Name: MLD.H                                                      
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-10-95 : 03.00.00 : M.Ozawa    : Initial
* Nov-10-95 : 03.01.00 : M.Ozawa    : Initial for R3.1
*           :          :            :                                    
*
** NCR2172 **
*
* Oct-05-99 : 01.00.00 : M.Teraki   : initial (for Win32)
* Jan-14-00 : 01.00.00 : H.Endo     : change Function name by Display type
* Dec-14-15 : 02.02.01 : R.Chambers : eliminated MLD_NUM_SEAT using standard NUM_SEAT.
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
#if !defined(MLD_H_INCLUDED)
#define MLD_H_INCLUDED

#include "ecr.h"
#include "regstrct.h"
#include "fdt.h"
#include "mld2x20.h"
#include "mldlcd.h"

/**
;========================================================================
            D E F I N E    D E C L A R A T I O N s                        
;========================================================================
**/
/* -- declearation of major class  -- */
#define CLASS_POPUP               100               /* MLD popup major class */

/* -- declearation of minor class  -- */
#define CLASS_POPORDERPMT         101               /* order prompt minor class */
#define CLASS_POPORDERPMT_UP      102               /* order prompt minor class, V3.3 */
#define CLASS_POPORDERPMT_DOWN    103               /* order prompt minor class, V3.3 */
#define CLASS_POPORDERPMT_UPDATE  104				/* order prompt minor class, this is for use
													   when updating the OEP window, we have to 
													   create a window, but dont do all of the logic in a function JHHJ*/
#define CLASS_POPORDERPMT_ADJ	  105				/* Adjective Edit JHHJ*/

#define CLASS_POPORDER_LABELED    106               // allow for an arbitrary text label for order prompting type windows

/* -- selection of display arrangment -- */
/*
 *   We support several different display formats depending on the provisioning of the
 *   the application.  The main things that determine the display formats are whether
 *   the application is provisioned as Pre/Post Guest Check or as a Store Recall system.
 *   Pre/Post Guest Check and the single display of Store Recall are the same display and
 *   layout with the same displayed line length for the display window.
 *   Store Recall also has the Three Scoll Window display used for a Drive Thru to show
 *   Guest Checks and the displayed line length is shorter in order to allow for the display
 *   of three windows.
 */
#define MLD_PRECHECK_SYSTEM         1               /* precheck/postcheck display - Pre/Post Guest Check */
#define MLD_DRIVE_THROUGH_1SCROLL   2               /* drive through 1 scroll display - Store Recall */
#define MLD_DRIVE_THROUGH_3SCROLL   3               /* drive through 3 scroll display - Store Recall */
#define MLD_SUPER_MODE              4               /* supervisor mode display */
#define MLD_SPLIT_CHECK_SYSTEM      5               /* split check after the fact display, uchMldSystem2 */

/* -- declaration of condiment void affection -- */
#define MLD_CONDIMENT_ADD			0				/*SR 192 Condiment Addition/Editing/Deletion JHHJ*/
#define MLD_CONDIMENT_VOID			1				/*SR 192 Condiment Addition/Editing/Deletion JHHJ*/
#define MLD_CURSOR_CHANGE_UP		2
#define MLD_CURSOR_CHANGE_DOWN		3
#define MLD_VOID_ALL_CONDIMENT      4


/* -- declaration of get cursor types -- */
#define MLD_GET_SALES_DISC			0
#define MLD_GET_COND_NUM			1
#define MLD_GET_SALES_ONLY			2

/*--------------------------------------------------------------------------
*   MLD function target display window constants.  These constants are used to
*   indicate the target display window of an MLD function.  The window does need
*   to exist in the layout for the terminal and be displayed in order for any
*   output to be visible.  See function MldPrintf() which uses the usScroll
*   parameter to determine which MLDCURTRNSCROL to use for writing text to
*   the specified window.
--------------------------------------------------------------------------*/
#define MLD_SCROLL_1            0x0001              /* scroll #1 display window */
#define MLD_SCROLL_2            0x0002              /* scroll #2 display window */
#define MLD_SCROLL_3            0x0004              /* scroll #3 display window */
#define MLD_TOTAL_1             0x0008              /* total #1 display window, see function MldDispSubTotal(), often updated along with MLD_SCROLL_1 */
#define MLD_TOTAL_2             0x0010              /* total #2 display window, see function MldDispSubTotal(), often updated along with MLD_SCROLL_2 */
#define MLD_TOTAL_3             0x0020              /* total #3 display window, see function MldDispSubTotal(), often updated along with MLD_SCROLL_3 */
#define MLD_ORDER_1             0x0040              /* order number #1 display */
#define MLD_ORDER_2             0x0080              /* order number #2 display */
#define MLD_ORDER_3             0x0100              /* order number #3 display */
#define MLD_DRIVE_1 (MLD_SCROLL_1|MLD_TOTAL_1|MLD_ORDER_1)
#define MLD_DRIVE_2 (MLD_SCROLL_2|MLD_TOTAL_2|MLD_ORDER_2)
#define MLD_DRIVE_3 (MLD_SCROLL_3|MLD_TOTAL_3|MLD_ORDER_3)

// defines used with the usScroll argument of MldWriteScrollDisplay() to
// indicate special cases to ensure that the MLD transaction file is
// synchronized with the various display management data.
#define MLD_SCROLL_ADDED_FILE    0x0400    // used with MldDummyScrollDisplay() to indicate item added to MLD transaction file

#define MLD_SCROLL_MASK          0x03FF    // used to isolate the MLD scroll window indicator


/*--------------------------------------------------------------------------
*       -- flag whether new itemize or not -- 
*       These values should be in the range of 0 to 255 since
*       they are sometimes combined with Transaction MAJOR CLASS types.
*       See CLASS_ITEMSALES in file regstrct.h.
--------------------------------------------------------------------------*/
#define MLD_NEW_ITEMIZE             0               /* noun item entry */
#define MLD_UPDATE_ITEMIZE          1               /* condiment item entry */
#define MLD_UPPER_CURSOR            2               /* cursor up key */
#define MLD_LOWER_CURSOR            3               /* cursor down key */
#define MLD_TRANS_DISPLAY           4               /* diaplay all transaction */
#define MLD_NEW_ITEMIZE2            5               /* noun item entry for item discount*/
#define MLD_UPDATE_ITEMIZE2         6               /* condiment item entry for item discount */
#define MLD_UPPER_CURSOR2           7               /* cursor up key for item discount */
#define MLD_LOWER_CURSOR2           8               /* cursor down key for item discount */
#define MLD_TRANS_DISPLAY2          9               /* diaplay all transaction for item discount */
#define MLD_REVERSE_DISPLAY        10               /* diaplay all transaction by reverse order */
#define MLD_REVERSE_DISPLAY2       11               /* diaplay all transaction by reverse order for item discount */
#define MLD_NEXT_CONDIMENT         12               /* diaplay next condiment (2x20) */
#define MLD_BEFORE_CONDIMENT       13               /* diaplay before condiment (2x20) */
#define MLD_ONLY_DISPLAY           14               /* diaplay data, ignor storage */
#define MLD_CONDIMENT_UPDATE	   15				/*SR 192 Condiment Addition/Editing/Deletion JHHJ*/
#define MLD_CONDIMENT_UPDATE_PREVIOUS 16			/*SR 192 Condiment Addition/Editing/Deletion JHHJ*/
#define MLD_ORDER_DECLARATION	   17				/*Order Declaration JHHJ*/

#define MLD_TYPE_GET_FLAG(x)     ((x)&0x00FF)              // Provides one of the above MLD_ flags such as MLD_UPDATE_ITEMIZE
#define MLD_TYPE_GET_CLASS(x)    (((x)&0xFF00)>>8)       // Provides the MAJOR CLASS from a usType
#define MLD_TYPE_PUT_CLASS(x,y)  (((x)&0x00FF)|(((y)<<8)&0xFF00))  // Combines a MAJOR CLASS into a usType


/* -- flag for free format display -- */
#define MLD_SCROLL_APPEND           -1              /* disply at current cursor */
#define MLD_SCROLL_TOP               0              /* disply at upper line */
#define MLD_SCROLL_END              -3              /* disply at lower line */
#define MLD_SCROLL_RETRIEVE			-4				/*Condiment Edit JHHJ*/
#define MLD_SCROLL_REPLACE			-5				/* Condiment Edit JHHJ*/
#define	MLD_SCROLL_BEGIN		    -6				/* Order Declaration JHHJ*/
#define	MLD_SCROLL_BEGIN_ADD		-7				/* Order Declaration JHHJ*/


#define MLD_C_BLACK               0x01/*0x07*/      /* black char. and white background (LCD) */ // colorpalette-changes
#define MLD_C_WHITE               0x02/*0x70*/      /* white cnar. and black background (LCD) */
#define MLD_C_BLINK               0x80              /* blink character */

#define MLD_ITEMSTORAGE             0               /* item storage file */
#define MLD_CONSSTORAGE             1               /* consordation storage file */
#define MLD_MLDSTORAGE              2               /* mld storage file */

#define MLD_INVALID_HANDLE          -1              /* invalid file handle */
#define MLD_TENT_MAX				26				// Max Length of Tent mnemonic

//  bit mask flags used with the returned bit mask of function MldTransactionAttributesFound ()
//  These flags provide a means for particular attributes of a MLDCURTRNSCROL buffer
//  to be communicated to other parts of the application.
#define  MLD_TRANS_ATTRIB_ORDERDEC            0x00000001     // indicates order declaration has been written to buffer, MLD_ORDER_DECLARATION


/* -- declearation of lcd back light off limit -- */
#define MLD_BACKLIGHT_LIMIT          5              /* 5 minites */

/* flag definition for global scroll control */
#define MLD_DEFAULT_DISPLAY     0x01    /* auto window clear status */
#define MLD_RELEASE_ON_FINALIZE 0x02    /* auto delete window status */

#define MLD_GUESTNO_NUM             20
//#define MLD_GUESTNO_NUM             10


/* -- error status -- */
#define MLD_SUCCESS                 0
#define MLD_ERROR                   -1
#define MLD_ALREADY_DISPLAYED       -2
#define MLD_INVALID_SCROLL          -3
#define MLD_NOT_DISPLAY             -4
#define MLD_STOP_SCROLL             -5
#define MLD_ALREADY_DISPLAYED_1     -6
#define MLD_ALREADY_DISPLAYED_2     -7
#define MLD_ALREADY_DISPLAYED_3     -8
#define MLD_DISPLAY_CHANGE          -9
#define MLD_CURSOR_TOP              -10
#define MLD_CURSOR_BOTTOM           -11

#define MLD_NO_READ_SIZE			-32 //RPH 11-10-3


#define MLD_CONDIMENT_NUMBER_ITEM    128  // Indicated selected line is Item mnemonic
#define MLD_CONDIMENT_NUMBER_VALID   128  // Condiment Number below this are valid.  Above this, special indicators.  See MldLocalGetCursorDisplay()
#define MLD_CONDIMENT_NUMBER_NUMTYPE 129  // Indicate selected line is #/Type entry

// Following defined constants are used with the ulDisplayFlags member of the
// various types of MLDPOPORDER, MLDPOPORDER_VARY, and MLDPOPORDERUNION_VARY
// to communicate to the Framework functionality particular conditions.
#define MLD_DISPLAYFLAGS_BACK        0x00000001   // display a Back button
#define MLD_DISPLAYFLAGS_MORE        0x00000002   // display a More/Continue button
#define MLD_DISPLAYFLAGS_DONE        0x00000004   // display a None/Done button

#define MLD_DISPLAYFLAG_DTXT_BACK     _T("@dback");
#define MLD_DISPLAYFLAG_DTXT_MORE     _T("@dmore");
#define MLD_DISPLAYFLAG_DTXT_DONE     _T("@ddone");
#define MLD_DISPLAYFLAG_RTXT_BACK     _T("@rback");
#define MLD_DISPLAYFLAG_RTXT_MORE     _T("@rmore");
#define MLD_DISPLAYFLAG_RTXT_DONE     _T("@rdone");

/* Displays Card Number */
#define ITM_MSR							1		// Number is stored in MSR Data
#define ITM_NUMBER						2		// Number is stored in Numer 

/**
;========================================================================
            S T R U C T U E R    D E C L A R A T I O N s                
;========================================================================
**/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

#if !defined(_INC_VOS)
typedef struct {
	USHORT usRow;
	USHORT usCol;
	USHORT usNumRow;
	USHORT usNumCol;
} VOSRECT;
#endif

/* -- argument of MldCreatePopUp() -- */
typedef struct {
    TCHAR aszAdjective[4+1];        // PARA_ADJMNEMO_LEN
    TCHAR aszPluName[20+1];         // PARA_PLU_LEN
    GCNUM uchOrder;
} ORDERPMT;
typedef struct {
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
    TCHAR aszAdjective[4+1];        // PARA_ADJMNEMO_LEN
    TCHAR aszPluName[20+1];         // PARA_PLU_LEN
    TCHAR aszLeadThrough[20+1];     // PARA_LEADTHRU_LEN
	DCURRENCY  lPrice;
	ULONG ulDisplayFlags;
    ORDERPMT OrderPmt[STD_OEP_MAX_NUM+1];        // changed hard-coded '20' to STD_OEP_MAX_NUM - CSMALL
} MLDPOPORDER;

typedef struct {
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
    TCHAR aszAdjective[4+1];        // PARA_ADJMNEMO_LEN
    TCHAR aszPluName[20+1];         // PARA_PLU_LEN
    TCHAR aszLeadThrough[20+1];     // PARA_LEADTHRU_LEN
    DCURRENCY  lPrice;
	ULONG ulDisplayFlags;
    ORDERPMT OrderPmt[1];             // varying number of ORDERPMT
} MLDPOPORDER_VARY;

typedef struct {
    USHORT  usOrder;
	ULONG   ulOffset;                // optional file offset to allow quick finding of an item
	TCHAR   aszLabel[48];
} ORDERPMT_TEXT;

typedef union {
	ORDERPMT_TEXT  OrderPmtText;
	ORDERPMT       OrderPmt;          // varying number of ORDERPMT
} ORDERPMT_UNION;

typedef struct {
    UCHAR uchMajorClass;
    UCHAR uchMinorClass;
    TCHAR aszAdjective[4+1];        // PARA_ADJMNEMO_LEN
    TCHAR aszPluName[20+1];         // PARA_PLU_LEN
    TCHAR aszLeadThrough[20+1];     // PARA_LEADTHRU_LEN
	DCURRENCY  lPrice;
	ULONG ulDisplayFlags;
	ORDERPMT_UNION  u[1];
} MLDPOPORDERUNION_VARY;

typedef struct {
    TrnVliOffset  usVliSize;
    USHORT        sFileHandle;
    USHORT        sIndexHandle;
} MLDTRANSDATA;

typedef struct {
    TCHAR auchSeatNum[NUM_SEAT];
} MLDSEAT;

typedef struct {
	USHORT  usTableNo;
    USHORT  usGuestNo;
    UCHAR   uchCdv;
} GUESTNO;

typedef struct {
    GUESTNO GuestNo[MLD_GUESTNO_NUM];
} MLDGUESTNUM;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s 
;========================================================================
**/
#if defined DISPLAY_MAIN
/*******************************************
* Prottype name of LCD 
********************************************/
/* -- Common Interface -- */
SHORT   MLDLCDMldInit(VOID);                          /* MLD Initialization */
SHORT   MLDLCDMldRefresh(VOID);                       /* MLD Reinitialization */
SHORT   MLDLCDMldSetMode(USHORT usSystem);            /* set type of display */
SHORT   MLDLCDMldSetDescriptor(USHORT usMode);        /* set descriptor mnemonic */
SHORT   MLDLCDMldDisplayInit(USHORT usDisplay, USHORT usAttrib);  /* Initialize each display */
SHORT   MLDLCDMldGetMldStatus(VOID);                  /* return whether mld is provied or not */
SHORT   MLDLCDMldDispCursor(VOID);                    /* display cursor on scroll display */

/* -- Storage File Interface -- */
SHORT   MLDLCDMldCreateFile(USHORT usSize, UCHAR uchType);        /* create mld storage file */
SHORT   MLDLCDMldChkAndCreFile(USHORT usSize, UCHAR uchType);     /* check and create mld storage file */
VOID    MLDLCDMldChkAndDelFile(USHORT usSize, UCHAR uchType);     /* check and delete mld storage file */
SHORT   MLDLCDMldCheckSize(VOID *pData, USHORT usSize2);      /* check storage buffer full */

/* -- Register Mode Interface -- */
SHORT   MLDLCDMldScrollWrite(VOID *pData, USHORT usType); /* display on scroll#1 */
SHORT   MLDLCDMldScrollWrite2(USHORT usScroll, VOID *pData, USHORT usType); /* display on scroll#1/2/3 */
SHORT   MLDLCDMldScrollFileWrite(VOID *pData);        /* put scroll data to storage file */
SHORT   MLDLCDMldScrollFileDelete( VOID *pMldStorage );
SHORT   MLDLCDMldScrollFileWriteSpl(VOID *pData);        /* put scroll data to storage file */
SHORT   MLDLCDMldECScrollWrite(VOID);                 /* clear displayed data */
SHORT   MLDLCDMldECScrollWrite2(USHORT usScroll);     /* clear displayed data */
SHORT   MLDLCDMldPutTransToScroll(MLDTRANSDATA *pData1);
SHORT   MLDLCDMldPutTransToScroll2(MLDTRANSDATA *pData1);
SHORT   MLDLCDMldRedisplayToScroll(USHORT usScroll);
SHORT   MLDLCDMldPutTrnToScrollReverse(MLDTRANSDATA *pData1);
SHORT   MLDLCDMldPutTrnToScrollReverse2(MLDTRANSDATA *pData1);
SHORT   MLDLCDMldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
SHORT   MLDLCDMldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle);
SHORT   MLDLCDMldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1); 
SHORT   MLDLCDMldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1); 
/*SHORT   MLDLCDMldPutCurTransToScroll(MLDTRANSDATA *pData1, VOID *pGCF1); */ 
/*SHORT   MLDLCDMldPutCurTransToScroll2(MLDTRANSDATA *pData1, VOID *pGCF1);*/
SHORT   MLDLCDMldPutAllGcfToScroll(SHORT sFileHandle1, SHORT sFileHandle2, SHORT sFileHandle3);
SHORT   MLDLCDMldPutAllGcfToScroll2(SHORT sFileHandle1, SHORT sFileHandle2, SHORT sFileHandle3);
SHORT   MLDLCDMldGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2, USHORT usType);
SHORT   MLDLCDMldUpCursor(VOID);                      /* move cursor to upper line */
SHORT   MLDLCDMldDownCursor(VOID);                    /* move cursor to lower line */
SHORT   MLDLCDMldMoveCursor(USHORT usType);           /* move cursor to next display */
SHORT   MLDLCDMldSetCursor(USHORT usScroll);          /* move cursor to directed display */
SHORT   MLDLCDMldQueryCurScroll(VOID);                /* returns current active display */
SHORT	MLDLCDMldSetMoveCursor(USHORT usScroll);		/* sets the Cursor status to moved*/
USHORT	MLDLCDMldGetVosWinHandle(USHORT usScroll);		/* get the vos window handle*/
SHORT   MLDLCDMldQueryMoveCursor(USHORT usSCroll);    /* check if cursor is moved in scroll */
SHORT   MLDLCDMldDispSubTotal(USHORT usScroll, DCURRENCY lTotal); /* display total amount */
SHORT   MLDLCDMldDispSeatNum(USHORT usScroll, MLDSEAT *pSeatNum); /* display seat# */
SHORT   MLDLCDMldCreatePopUp(VOID *pData);            /* show order prompt display */
SHORT   MLDLCDMldUpdatePopUp(VOID *pData);            /* change order prompt display */
SHORT   MLDLCDMldDeletePopUp(VOID);                   /* delete order prompt display */
SHORT   MLDLCDMldForceDeletePopUp(VOID);              /* delete order prompt display, used with Auto Sign-out */
SHORT   MLDLCDMldPopUpUpCursor(VOID);                 /* up cursor of order prompt display */
SHORT   MLDLCDMldPopUpDownCursor(VOID);               /* down cursor of order prompt display */
SHORT   MLDLCDMldPopUpRedisplay(USHORT usType);       /* redisplay of order prompt display */
SHORT   MLDLCDMldPopUpGetCursorDisplay(VOID *pData);  /* get data class of current display */
SHORT   MLDLCDMldDispWaitKey(FDTWAITORDER *pData);    /* show order prompt display */
SHORT   MLDLCDMldDispGCFQualifier(USHORT usScroll, VOID *pData);   /* show order# and subtotal from GCF */
SHORT   MLDLCDMldDispGuestNum(MLDGUESTNUM *pGuestNum); /* display guest check no. V3.3 */
SHORT   MLDLCDMldDispTableNum(MLDGUESTNUM *pGuestNum); /* display table number of guest check */

/* -- Supervisor Mode Interface -- */
SHORT MLDLCDMldDispItem(VOID  *pItem, USHORT usStatus);
SHORT MLDLCDMldIRptItemOther(USHORT usScroll);
#if 0
SHORT   MLDLCDMldPrintfAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                CONST UCHAR FAR *pszFmt, ...);  /* free format display */
SHORT   MLDLCDMldPrintf(USHORT usDisplay, SHORT sRow,
                CONST UCHAR FAR *pszFmt, ...);  /* free format display */
#endif
SHORT   MLDLCDMldStringAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                TCHAR *pszString, USHORT usLength);  /* free format display */
SHORT   MLDLCDMldString(USHORT usDisplay, SHORT sRow,
                TCHAR *pszString, USHORT usLength);  /* free format display */
SHORT   MLDLCDMldScrollUp(USHORT usScroll, USHORT usNumRow); /* make lower line to blank */
SHORT   MLDLCDMldScrollDown(USHORT usScroll, USHORT usNumRow);/* make upper line to blank */
SHORT   MLDLCDMldScrollClear(USHORT usScroll);        /* make all line to blank */

/*******************************************
* Prottype name of 2x20 display 
********************************************/
/* -- Common Interface -- */
SHORT   MLD2X20MldInit(VOID);                          /* MLD Initialization */
SHORT   MLD2X20MldRefresh(VOID);                       /* MLD Reinitialization */
SHORT   MLD2X20MldSetMode(USHORT usSystem);            /* set type of display */
SHORT   MLD2X20MldSetDescriptor(USHORT usMode);        /* set descriptor mnemonic */
SHORT   MLD2X20MldDisplayInit(USHORT usDisplay, USHORT usAttrib);  /* Initialize each display */
SHORT   MLD2X20MldGetMldStatus(VOID);                  /* return whether mld is provied or not */
SHORT   MLD2X20MldDispCursor(VOID);                    /* display cursor on scroll display */

/* -- Storage File Interface -- */
SHORT   MLD2X20MldCreateFile(USHORT usSize, UCHAR uchType);        /* create mld storage file */
SHORT   MLD2X20MldChkAndCreFile(USHORT usSize, UCHAR uchType);     /* check and create mld storage file */
VOID    MLD2X20MldChkAndDelFile(USHORT usSize, UCHAR uchType);     /* check and delete mld storage file */
SHORT   MLD2X20MldCheckSize(VOID *pData, USHORT usSize2);      /* check storage buffer full */

/* -- Register Mode Interface -- */
SHORT   MLD2X20MldScrollWrite(VOID *pData, USHORT usType); /* display on scroll#1 */
SHORT   MLD2X20MldScrollWrite2(USHORT usScroll, VOID *pData, USHORT usType); /* display on scroll#1/2/3 */
SHORT   MLD2X20MldScrollFileWrite(VOID *pData);        /* put scroll data to storage file */
SHORT   MLD2X20MldECScrollWrite(VOID);                 /* clear displayed data */
SHORT   MLD2X20MldECScrollWrite2(USHORT usScroll);     /* clear displayed data */
SHORT   MLD2X20MldPutTransToScroll(MLDTRANSDATA *pData1);
SHORT   MLD2X20MldPutTransToScroll2(MLDTRANSDATA *pData1);
SHORT   MLD2X20MldPutTrnToScrollReverse(MLDTRANSDATA *pData1);
SHORT   MLD2X20MldPutTrnToScrollReverse2(MLDTRANSDATA *pData1);
SHORT   MLD2X20MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
SHORT   MLD2X20MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle);
SHORT   MLD2X20MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1); 
SHORT   MLD2X20MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1); 
/*SHORT   MLD2X20MldPutCurTransToScroll(MLDTRANSDATA *pData1, VOID *pGCF1); */
/*SHORT   MLD2X20MldPutCurTransToScroll2(MLDTRANSDATA *pData1, VOID *pGCF1);*/
SHORT   MLD2X20MldPutAllGcfToScroll(SHORT sFileHandle1,
                                SHORT sFileHandle2,
                                SHORT sFileHandle3);
SHORT   MLD2X20MldPutAllGcfToScroll2(SHORT sFileHandle1,
                                SHORT sFileHandle2,
                                SHORT sFileHandle3);
SHORT   MLD2X20MldGetCursorDisplay(USHORT usScroll, VOID *pData1, VOID *pData2, USHORT usType);
SHORT   MLD2X20MldUpCursor(VOID);                      /* move cursor to upper line */
SHORT   MLD2X20MldDownCursor(VOID);                    /* move cursor to lower line */
SHORT   MLD2X20MldMoveCursor(USHORT usType);           /* move cursor to next display */
SHORT   MLD2X20MldSetCursor(USHORT usScroll);          /* move cursor to directed display */
SHORT   MLD2X20MldQueryCurScroll(VOID);                /* returns current active display */
SHORT   MLD2X20MldQueryMoveCursor(USHORT usSCroll);    /* check if cursor is moved in scroll */
SHORT   MLD2X20MldDispSubTotal(USHORT usScroll, DCURRENCY lTotal); /* display total amount */
SHORT   MLD2X20MldDispSeatNum(USHORT usScroll, MLDSEAT *pSeatNum); /* display seat# */
SHORT   MLD2X20MldCreatePopUp(VOID *pData);            /* show order prompt display */
SHORT   MLD2X20MldUpdatePopUp(VOID *pData);            /* change order prompt display */
SHORT   MLD2X20MldDeletePopUp(VOID);                   /* delete order prompt display */
SHORT   MLD2X20MldForceDeletePopUp(VOID);              /* delete order prompt display, used with Auto Sign-out */
SHORT   MLD2X20MldPopUpUpCursor(VOID);                 /* up cursor of order prompt display */
SHORT   MLD2X20MldPopUpDownCursor(VOID);               /* down cursor of order prompt display */
SHORT   MLD2X20MldPopUpRedisplay(USHORT usType);       /* redisplay of order prompt display */
SHORT   MLD2X20MldPopUpGetCursorDisplay(VOID *pData);  /* get data class of current display */
SHORT   MLD2X20MldDispWaitKey(FDTWAITORDER *pData);    /* show order prompt display */
SHORT   MLD2X20MldDispGCFQualifier(USHORT usScroll, VOID *pData);   /* show order# and subtotal from GCF */
SHORT   MLD2X20MldDispGuestNum(MLDGUESTNUM *pGuestNum); /* display guest check no. V3.3 */

/* -- Supervisor Mode Interface -- */
SHORT MLD2X20MldDispItem(VOID  *pItem, USHORT usStatus);
SHORT MLD2X20MldIRptItemOther(USHORT usScroll);
#if 0
SHORT   MLD2X20MldPrintfAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                CONST UCHAR FAR *pszFmt, ...);  /* free format display */
SHORT   MLD2X20MldPrintf(USHORT usDisplay, SHORT sRow,
                CONST UCHAR FAR *pszFmt, ...);  /* free format display */
#endif
/*SHORT   MLD2X20MldStringAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                UCHAR *pszString, USHORT usLength); this function is not for 2x20*/ /* free format display */
SHORT   MLD2X20MldString(USHORT usDisplay, SHORT sRow,
                TCHAR *pszString, USHORT usLength);  /* free format display */
/*SHORT   MLD2X20MldScrollUp(USHORT usScroll, USHORT usNumRow); this function is not for 2x20*/ /* make lower line to blank */
/*SHORT   MLD2X20MldScrollDown(USHORT usScroll, USHORT usNumRow); this function is not for 2x20*//* make upper line to blank */
/*SHORT   MLD2X20MldScrollClear(USHORT usScroll); this function is not for 2x20*/        /* make all line to blank */

#endif

/* -- Common Interface -- */
MLDTRANSDATA *MldMainSetMldData (MLDTRANSDATA *pWorkMLD);
SHORT MldMainCallWithMldData (SHORT (*pFunc)(MLDTRANSDATA *pWorkMLD));
MLDTRANSDATA *MldMainSetMldDataSplit (MLDTRANSDATA *pWorkMLD, TrnStorageType  sStorageType);
SHORT MldMainCallWithMldDataSplit (SHORT (*pFunc)(MLDTRANSDATA *pWorkMLD), TrnStorageType  sStorageType);
SHORT   MldInit(VOID);                          /* MLD Initialization */
SHORT   MldRefresh(VOID);                       /* MLD Reinitialization */
SHORT   MldSetMode(USHORT usSystem);            /* set type of display */
SHORT   MldSetDescriptor(USHORT usMode);        /* set descriptor mnemonic */
SHORT   MldDisplayInit(USHORT usDisplay, USHORT usAttrib);  /* Initialize each display */
SHORT   MldGetMldStatus(VOID);                  /* return whether mld is provied or not */
SHORT   MldDispCursor(VOID);                    /* display cursor on scroll display */


/* -- Storage File Interface -- */
SHORT   MldCreateFile(USHORT usSize, UCHAR uchType);        /* create mld storage file */
SHORT   MldChkAndCreFile(USHORT usSize, UCHAR uchType);     /* check and create mld storage file */
VOID    MldChkAndDelFile(USHORT usSize, UCHAR uchType);     /* check and delete mld storage file */
SHORT   MldCheckSize(VOID *pData, USHORT usSize2);      /* check storage buffer full */

/* -- Register Mode Interface -- */
SHORT   MldScrollWrite(VOID *pData, USHORT usType); /* display on scroll#1 */
SHORT   MldScrollWrite2(USHORT usScroll, VOID *pData, USHORT usType); /* display on scroll#1/2/3 */
SHORT   MldScrollFileWrite(VOID *pData);        /* put scroll data to storage file */
SHORT   MldScrollFileWriteSpl(VOID *pData);     /* put scroll data to storage file with fewer checks on consistency */
SHORT   MldScrollFileDelete( VOID *pMldStorage );
SHORT   MldECScrollWrite(VOID);                 /* clear displayed data */
SHORT   MldECScrollWrite2(USHORT usScroll);     /* clear displayed data */
SHORT   MldPutTransToScroll(MLDTRANSDATA *pData1);
SHORT   MldRedisplayToScroll(USHORT usScroll);
//SHORT   MldPutTransToScroll2(MLDTRANSDATA *pData1);
SHORT   MldPutTrnToScrollReverse(MLDTRANSDATA *pData1);
//SHORT   MldPutTrnToScrollReverse2(MLDTRANSDATA *pData1);
SHORT   MldPutGcfToScroll(USHORT usScroll, SHORT sFileHandle);
//SHORT   MldPutGcfToScroll2(USHORT usScroll, SHORT sFileHandle);
SHORT   MldPutSplitTransToScroll(USHORT usScroll, MLDTRANSDATA *pData1); 
//SHORT   MldPutSplitTransToScroll2(USHORT usScroll, MLDTRANSDATA *pData1); 
/*SHORT   MldPutCurTransToScroll(MLDTRANSDATA *pData1, VOID *pGCF1); */
/*SHORT   MldPutCurTransToScroll2(MLDTRANSDATA *pData1, VOID *pGCF1);*/
SHORT   MldPutAllGcfToScroll(SHORT sFileHandle1,
                                SHORT sFileHandle2,
                                SHORT sFileHandle3);
SHORT   MldPutAllGcfToScroll2(SHORT sFileHandle1,
                                SHORT sFileHandle2,
                                SHORT sFileHandle3);
SHORT   MldGetCursorDisplay(USHORT usScroll,  VOID *pData1, VOID *pData2, USHORT usType);
SHORT   MldUpCursor(VOID);                      /* move cursor to upper line */
SHORT   MldDownCursor(VOID);                    /* move cursor to lower line */
SHORT   MldMoveCursor(USHORT usType);           /* move cursor to next display */
SHORT   MldSetCursor(USHORT usScroll);          /* move cursor to directed display */
SHORT   MldQueryCurScroll(VOID);                /* returns current active display */
SHORT	MldSetMoveCursor(USHORT usScroll);		/* sets the Cursor status to moved*/
USHORT	MldGetVosWinHandle(USHORT usScroll);		/* get the vos window handle*/
SHORT   MldQueryMoveCursor(USHORT usSCroll);    /* check if cursor is moved in scroll */
SHORT   MldDispSubTotal(USHORT usScroll, DCURRENCY lTotal); /* display total amount */
SHORT   MldDispSeatNum(USHORT usScroll, MLDSEAT *pSeatNum); /* display seat# */
SHORT   MldCreatePopUp(VOID *pData);            /* show order prompt display */
SHORT	MldCreatePopUpRectWindowOnly(VOSRECT *pRect); /* create the window only, to get the size OEP*/
SHORT   MldCreatePopUpRect(VOID *pData, VOSRECT *pRect);            /* show order prompt display */
SHORT   MldUpdatePopUp(VOID *pData);            /* change order prompt display */
SHORT   MldDeletePopUp(VOID);                   /* delete order prompt display */
SHORT   MldForceDeletePopUp(VOID);              /* delete order prompt display, used with Auto Sign-out */
SHORT   MldPopUpUpCursor(VOID);                 /* up cursor of order prompt display */
SHORT   MldPopUpDownCursor(VOID);               /* down cursor of order prompt display */
SHORT   MldPopUpRedisplay(USHORT usType);       /* redisplay of order prompt display */
SHORT   MldPopUpGetCursorDisplay(VOID *pData);  /* get data class of current display */
SHORT   MldDispWaitKey(FDTWAITORDER *pData);    /* show order prompt display */
SHORT   MldDispGuestNum(MLDGUESTNUM *pGuestNum); /* display guest check no. V3.3 */
SHORT   MldDispTableNum(MLDGUESTNUM *pGuestNum); /* display table no. of guest checks */
SHORT	MldOrderDecSearch(VOID *pItem);

/* -- Supervisor Mode Interface -- */
SHORT MldDispItem(VOID  *pItem, USHORT usStatus);
SHORT MldIRptItemOther(USHORT usScroll);
SHORT   MldPrintfAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                CONST TCHAR FAR *pszFmt, ...);  /* free format display */
SHORT   MldPrintf(USHORT usDisplay, SHORT sRow,
                CONST TCHAR FAR *pszFmt, ...);  /* free format display */
SHORT   MldStringAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                TCHAR *pszString, USHORT usLength);  /* free format display */
SHORT   MldString(USHORT usDisplay, SHORT sRow,
                TCHAR *pszString, USHORT usLength);  /* free format display */
SHORT   MldScrollUp(USHORT usScroll, USHORT usNumRow); /* make lower line to blank */
SHORT   MldScrollDown(USHORT usScroll, USHORT usNumRow);/* make upper line to blank */
SHORT   MldScrollClear(USHORT usScroll);        /* make all line to blank */

ULONG  MldTransactionAttributesFound (USHORT usScroll);

#endif
/*** end of file ***/
