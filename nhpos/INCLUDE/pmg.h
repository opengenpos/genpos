/**
*===========================================================================
* Update Histories
*
* Data       Ver.      Name         Description
* Aug-15-95  G1        O.Nakada     Added PMG_M825_M820, PMG_VALIDATION and
*                                   PMG_CUTTER.
*
** GenPOS **
*   Jan-11-18 : 02.02.02 : R.Chambers  :implementing Alt PLU Mnemonic in Kitchen Printing.
*===========================================================================
**/
/**
;========================================================================
;+                                                                      +
;+              L O C A L    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/
/* SAME DEFINE ARE IN DEVICEIOPRINTER.H*/

#if !defined(PRINT_PMG_H_INCLUDED)
#define  PRINT_PMG_H_INCLUDED

/*
	The following PMG_xxxx_PRINTER defines are used to indicate the
	printer type.  If new printer types are added, the table in file pmgif.c
	must also be updated.  The table is PrinterFuncStruct printerfuncs []
	and it is used to redirect the actual printer command to the appropriate
	printer specific function needed to perform the action.

	Take a look at how this is done in files pmgOPOS.c and pmgWIN.c with
	the interface redirection located in file pmgif.c all of which are in
	the Pmg subsystem of Sa in BusinessLogic.

	See also file DeviceEngine.cpp in Framework which contains logic in
	function CDeviceEngine::Open() to set the printer type as well as
	providing an interface to query the printer type and to implement
	Windows printing for printer type PMG_WIN_PRINTER.

	The define for PMG_NONE_PRINTER must be 0 for the code in file pmgif.c
	to detect a printer has not been specified and to act appropriately.
 */
#define PMG_NONE_PRINTER        0    /* No printer specified.  Must be 0.*/
#define PMG_OPOS_PRINTER		1    /* OPOS printer specified.          */
#define PMG_WIN_PRINTER			2    /* Windows Driver printer           */
#define PMG_WEB_FILE			3    /* output to file for the http server interface   */
#define PMG_ZEBRA_PRINTER		4	 // Driver for Zebra printer BW 220   ***PDINU
#define PMG_CONNECTION_ENGINE	5	 // Driver for Connection Engine to send print data out Connection Engine
#define PMG_COM_PRINTER         6    // direct to COM printer such as NCR 7194 type of thermal printer bypassing OPOS
 // following defines are special versions of the above list.
#define PMG_OPOS_IMMEDIATE     12    // See CDeviceEngine::DevicePrinterWrite(). requesting to use printImmediate() in OPOS control rather than printNormal() for label type printers


// defines for 
#define PMG_PRT_RECEIPT         1           /* receipt station. See also OPOS define PTR_S_RECEIPT */
#define PMG_PRT_JOURNAL         2           /* journal station. See also OPOS define PTR_S_JOURNAL */
#define PMG_PRT_RCT_JNL         3           /* receipt & journal        */
#define PMG_PRT_VALIDATION      4           /* validation               */
#define PMG_PRT_SLIP            5           /* slip printer. See also OPOS define PTR_S_SLIP  */
/***	alternate file printing selections	***/ //ESMITH PRTFILE
#define PMG_PRT_FILE_SUP        6           /* print to local Supervisor  Mode file      */ 
#define PMG_PRT_FILE_REG        7           /* print to local Register Mode file      */ 
#define PMG_PRT_FILE_PRG        8           /* print to local Program Modefile      */
#define PMG_PRT_LOGO			9			/* print Logo on Receipt*/ 
#define PMG_PRT_CENTERED	    10			/* print centered on line	*/
#define PMG_PRT_WEB_FILE        11          /* print to a web file  */
#define PMG_PRT_CONNENGINE      12          /* print to the Connection Engine  */

// isolated by using macro PMG_GET_PRT_OPTIONS() below so the values for these flags
// must be in the proper range.
#define PMG_PRT_IMMEDIATE       0x10        // indicates to use printImmediate() and not printNormal()
#define PMG_PRT_REPORT_ERROR    0x20        // indicates to return and report error rather than handle through call back

#define PMG_PRT_WEB_MASK        0x0f        /* mask used to isolate PM_PRT type in usPrtType */
#define PMG_MAKE_FULL_PRTTYPE(pmg,prttype)  ( ( ((prttype) & 0xff) | ((pmg) & 0xff) << 8) )
#define PMG_GET_PRT_STATION(x) ((x) & 0x0f)
#define PMG_GET_PRT_TYPE(x)    (((x) >> 8) & 0xff)
#define PMG_GET_PRT_OPTIONS(x) ((x) & 0xf0)


/***    error status            ***/
#define PMG_SUCCESS             0           /* function successed       */
#define PMG_ERROR_PROVIDE       1           /* printer is not provide   */
#define PMG_ERROR_IMPORTANT     2           /* error in important lines */
#define PMG_ERROR_COMM          3           /* communication error      */
#define PMG_ERROR_PRINTER       4           /* printer error            */

/***    secape character        ***/
#define ESC         0x1b                /* escape code          */
#define GS          0x1d                /* group separater      */
#define DLE         0x10                /* data line escape     */
#define DC1         0x11                /* device control 1     */
#define DC2         0x12                /* device control 2     */
#define DC3         0x13                /* device control 3     */
#define DC4         0x14                /* device control 4     */
#define LF          0x0a                /* line feed            */
#define CR          0x0d                /* carriage return      */

/***    printer status of PmgGetPrtStatus (receipt/journal) ***/

//		The status bits for printing must match up in three		
//		files: DeviceIOPrinter.h, ESCP.H, pmg.H. If any of		
//		the status bits change, or new ones are added, they		
//		must be added to all files.								
#define PRT_STAT_R_NEAR_END 0x01        /* receipt paper near end       */
#define PRT_STAT_TAILEND    0x02        /* tail end                     */
#define PRT_STAT_ERROR      0x04        /* not error                    */
#define PRT_STAT_FORM_IN    0x08        /* form in                      */
#define PRT_STAT_J_NEAR_END 0x10        /* journal paper near end       */
#define PRT_STAT_INIT_END   0x20        /* 1:initialize cmplete             */
#define PRT_STAT_POWER_OFF	0x40		/***    printer status of PmgGetPrtStatus (slip)    ***/
#define PRT_STAT_HW_ERROR   0x80        // indicates some kind of hardware error.

/***    EPSON thermal printer font type     ***/
#define PRT_FONT_A      0x00            /* FONT-A 12x24         */
#define PRT_FONT_B      0x01            /* FONT-B  9x 7         */

#define PRT_CAP_LINE_SPACING          1    // indicates to access SetRecLineSpacing(), specify addition to GetRecLineHeight()
#define PRT_CAP_LINE_HEIGHT           1    // indicates to access SetRecLineHeight()
#define PRT_CAP_GET_ONLY         0x4000    // used to indicate only do retrieve of value for capability
#define PRT_CAP_MASK             0x3fff    // used to remove any indicator bits in the capability indicator

/***    # of column     ***/
#define PMG_M825_CHAR           24
#define PMG_M820_CHAR           21
// #define PMG_V1_CHAR         44      Nov/22/2000
#define PMG_V1_CHAR         44+21   // Add Space    Nov/22/2000
#define PMG_THERMAL_CHAR_A      42
#define PMG_THERMAL_CHAR_B      56
#define PMG_THERMAL_CHAR_C      44 //ESMITH EJ
#define PMG_THERMAL_CHAR_D      40 //Zebra RW 220 Printer width (2.25" roll, PAGE-WIDTH 400)
//#define PMG_THERMAL_CHAR_E      30 //Zebra RW 220 Printer width (2" roll, PAGE-WIDTH 350)

#define PMG_7158SLIP_L_MERGIN   21  /* left mergin for 7158 slip 05/15/2001*/

/***    Config   ***/
#define PMG_M825_M820   0x04        /** printer type - 1:M825, 0:M820 **/
#define PMG_VALIDATION  0x02        /** with validation               **/
#define PMG_CUTTER      0x01        /** with cutter                   **/

#define     PRT_BARCODE_TEXT_LEFT      0x0000    // same as FRAMEWORK_IO_PRINTER_TEXT_LEFT and indicates PTR_BC_LEFT
#define     PRT_BARCODE_TEXT_CENTER    0x0001    // same as FRAMEWORK_IO_PRINTER_TEXT_CENTER and indicates PTR_BC_CENTER
#define     PRT_BARCODE_TEXT_RIGHT     0x0002    // same as FRAMEWORK_IO_PRINTER_TEXT_RIGHT and indicates PTR_BC_RIGHT
#define     PRT_BARCODE_TEXT_BELOW     0x0000    // same as FRAMEWORK_IO_PRINTER_TEXT_BELOW and indicates PTR_BC_TEXT_BELOW
#define     PRT_BARCODE_TEXT_ABOVE     0x0010    // same as FRAMEWORK_IO_PRINTER_TEXT_ABOVE and indicates PTR_BC_TEXT_ABOVE
#define     PRT_BARCODE_TEXT_NONE      0x0020    // same as FRAMEWORK_IO_PRINTER_TEXT_NONE and indicates PTR_BC_TEXT_NONE

#define     PRT_BARCODE_CODE_DEFAULT     0    // indicates to use the default bar code symbology
#define     PRT_BARCODE_CODE_CODE128     1    // indicates to use bar code symbology Code128
#define     PRT_BARCODE_CODE_CODE39      2    // indicates to use bar code symbology Code39
#define     PRT_BARCODE_CODE_CODE128C    3    // indicates to use bar code symbology Code128C which is compressed format digits only
#define     PRT_BARCODE_CODE_CODEEAN128  4    // indicates to use bar code symbology CodeEAN128 which is compressed format digits only
#define     PRT_BARCODE_CODE_CODEUPCA    5    // indicates to use bar code symbology CodeUPC-A which is digits only
#define     PRT_BARCODE_CODE_CODEEAN13   6    // indicates to use bar code symbology CodeEAN13 which is digits only

// following constants returned by function PrtChkStorage() which determines
// which value to return based on various settings in TrnInformation.TranCurQual.
// this function also sets the flags in global variable fsPrtStatus.
#define     PRT_NOPORTION        -1
#define     PRT_1PORTION          1
#define     PRT_SAMEBUFF          2
#define     PRT_DIFFBUFF          3    // different print format between RJ (Receipt/Journal) and kitchen printers

// following defines used with uchPrintStorage member of TRANCURQUAL and
// used with uchKitchenStorage member of TRANCURQUAL to determine how the
// transaction data is stored and how it should be retrieved during printing.
//
// see function PrtChkStorage() for how these settings are interpreted for receipt and kitchen printing.
//
// normally pTran->TranCurQual.uchPrintStorage and pTran->TranCurQual.uchKitchenStorage will be set to
// the same PRT_ value. PRT_NOSTORAGE for uchKitchenStorage to to disable kitchen printing during the
// reprinting of a transaction such as Print On Demand or second receipt copy.
#define     PRT_NOSTORAGE         0     // used with system type CURQUAL_PRE_UNBUFFER for printing as items are entered.
#define     PRT_ITEMSTORAGE       1
#define     PRT_CONSOLSTORAGE     2     // used with system type CURQUAL_POSTCHECK for printing entire transaction after completing it.
#define     PRT_CONSOL_CP_EPT     3     /* print EPT consolidate without listing items */
#define     PRT_KIT_NET_STORAGE   3     /* net item (don't print voided items) for kitchen printer R3.1 see PrtKPItem() */
#define     PRT_ITEM_NET_STORAGE  4     // appears to be unused
#define     PRT_CONL_NET_STORAGE  5     // appears to be unused
#define     PRT_SUMMARY_RECEIPT   6     //Print summary receipt with total and tender

// following are used with functions PrtTransactionBegin() and PrtTransactionEnd() for
// double sided printing and are interpreted within the DeviceEngine.cpp printer logic.
#define		PRT_KITCHENPRINTING	  1
#define		PRT_REGULARPRINTING	  2
#define		PRT_POSTPRINTING	  3

#define     PRT_KPRT_MAX          MAX_DEST_SIZE   /* kitchen printer num R3.1  See also KPS_NUMBER_OF_PRINTER */

/**
	These masks are used to communicate various printer state and
	printer requests within the receipt printing source.

	These masks are used with the fsPrintStatus member of the various
	item structs such as ITEMMISC, ITEMPRINT, etc.
**/
#define     PRT_REQKITCHEN			0x0001  /* request kitchen print */
#define     PRT_KPONLY				0x0002  /* request only kitchen printer */
#define     PRT_DEMAND				0x0004  /* print on demand */
#define     PRT_TAKETOKIT			0x0008  /* take to kitchen */
#define     PRT_SERV_JOURNAL		0x0010  /* print on journal by service ttl */
#define     PRT_SPLIT_PRINT			0x0020  /* split check print status, used with Table Service Split FSC R3.1 */
#define     PRT_REC_SLIP			0x0040  /* print out to receipt/slip R3.1 */
#define     PRT_EJ_JOURNAL			0x0080	/* Print EJ when not execute buffered print*/
#define     PRT_SUMMARY_PRINT		0x0100  /* summary check print status, see PrtPrintSummaryReceipt(), not split check print GenPOS Rel 2.2 */

/**
	These masks are used with the fsCurStatus2 member of the
	TRANCURQUAL struct to indicate various system states.
**/
#define		PRT_GIFT_RECEIPT		0x0100	/* Gift Receipt JHHJ, same as CURQUAL_PRT_GIFT_RECEIPT used with TRANCURQUAL.fsCurStatus2 */
#define		PRT_POST_RECEIPT		0x0200	/* Print Post Receipt JHHJ, same as CURQUAL_PRT_POST_RECEIPT used with TRANCURQUAL.fsCurStatus2 */
#define		PRT_DBL_POST_RECEIPT	0x0400	// Special handling for printing post receipt with double sided printing
#define		PRT_PREAUTH_RECEIPT		0x0800	// Special handling for Preauth Capture (Finalization) receipt.
#define		PRT_DUPLICATE_COPY		0x1000	/* this transaction requires a duplicate copy, TRN_TNDR_PRT_DUP_COPY */
#define     PRT_DEMAND_COPY			0x2000  /* print on demand */

#define     PRT_KPABORT     0x0010      /* kitchen prnter abort */

#define     PRT_ELECTRO     0x0001      /* electro provided (electro is an old printer device not used anymore) */
#define     PRT_24CHAR          24      /* 24char printer */
#define     PRT_21CHAR          21      /* 21char printer */
#define     PRT_VL_24           55      /* validation for 24char printer */
#define     PRT_VL_21           47      /* validation for 21char printer */

//#define     PRT_VOID            VOID_MODIFIER  /* mask data for modifier */
//#define     PRT_TAXMOD1         TAX_MODIFIER1     
//#define     PRT_TAXMOD2         TAX_MODIFIER2     
//#define     PRT_TAXMOD3         TAX_MODIFIER3
//#define     PRT_OFFEPTTEND      OFFEPTTEND_MODIF
//#define     PRT_OFFCPTEND       OFFCPTEND_MODIF
//#define     PRT_OFFCPAUTTEND    OFFCPAUTTEND_MODIF

#define     PRT_KPVOID     0x0010     
#define     PRT_TAX_MASK   0x001E     

#define     PRT_SINGLE           0      /*  single  character */
#define     PRT_DOUBLE      0x0012      /*  double character */
#define     PRT_SDOUBLE     0x0013      /*  square double character */
#define		PRT_LOGO		0x0014		/* Print Logo*/
#define		PRT_CENTERED	0x0015		/* print centered on line	*/
#define		PRT_TRANS_BGN	0x0016		/* Begin transaction call for shared printing*/
#define		PRT_TRANS_END	0x0017		/* End Transaction call for shared printing*/

#define     PRT_TTLAMT_LEN      30      /* total amount length(double wide) */

#define     PRT_NODECIMAL       -1      /* no decimal point, see also MLD_NODECIMAL */
#define     PRT_1DECIMAL         1      /* 1 digit under decimal point */
#define     PRT_2DECIMAL         2      /* 2 digit under decimal point, see also MLD_2DECIMAL */
#define     PRT_3DECIMAL         3      /* 3 digit under decimal point, see also MLD_3DECIMAL */

#define     PRT_5DECIMAL         5      /* 5 digit under decimal point, V3.4, see also MLD_5DECIMAL */
#define     PRT_6DECIMAL         6      /* 6 digit under decimal point, V3.4, see also MLD_6DECIMAL */

#define     PRT_DATETIME_LEN    32      /* number line work area size, was 16 however now includes 3 character time zone */

#define     PRT_NUMBERLINE      29      /* number line work area size */

#define     PRT_CASNAME_LEN     12      /* cashier name length, V3.3 */
#define     PRT_CASNAME21_LEN   10      /* cashier name length */
#define     PRT_CASNAMEKP_LEN   14      /* cashier name length for KP */
#define     PRT_DATE_LEN         8      /* date length */
#define     PRT_TIME_LEN        10      /* Time length , was 7 but now adding 3 character time zone */
#define     PRT_TAXMOD_LEN      14      /* tax modifier mnemo. length */
#define     PRT_PLUMNEM_LEN     15      /* adj and plu mnemo. */
#define     PRT_NUMBER_LEN      19      /* number max column */
#define     PRT_AMOUNT_LEN      15      /* amount (11 digits + decimal point + minus) */
#define     PRT_NAT_AMT_LEN     15      /* native mnemonic and amount */
#define     PRT_RATE_LEN         2      /* rate */
#define     PRT_PRINTNO_LEN      4      /* error kitchen priter */
/*#define     PRT_ID_LEN           3       ID */
#define     PRT_ID_LEN           4      /* table number, no. of person (REL3.0) */
#define     PRT_PAGENO_LEN       2      /* page no. length */
#define     PRT_GC_LEN           4      /* G/C no. length */

#define     PRT_KPS_OPERATOR_LEN (4+1+20+1) /* kp trailer line of operator name, V3.3 */
/* #define     PRT_KPS_OPERATOR_LEN (4+1+10)   / kp trailer line of operator name */
                                            /* spec. mnem. + space + name */

#define     PRT_SUCCESS          0      /* status Success */
#define     PRT_FAIL            -1      /* status Fail */

#define     PRT_SPACE         0x20      /* space (ASCII) */
#define     PRT_RETURN        0x0A      /* return (ASCII) */
#define     PRT_SEPARATOR     0xFF      /* separator (ASCII) */

#define     PRT_SLP_CHG         -1      /* paper changed */
#define     PRT_SLP_NOCHG        0      /* paper not changed  */

// STD_ADJMNEMO_LEN + 1 + STD_PLUNAME_LEN + 1 + STD_MAX_COND_NUM * (STD_PLUNAME_LEN + 1)
// PRT_MAXITEM, MLD_MAXITEM, PRT_KPMAXITEM need to be maintained to the correct size
#define     PRT_MAXITEM        446      /* max bytes of adjective, noun, print modifier */
                                        /* 4(adj) + 1(space) + 20(noun) + 1(space)      */
                                        /* + 20 * ( 20(condiment) + 1(space))           */

#define     PRT_CONTINUE         0      /* string continue */
#define     PRT_LAST             1      /* strings end */

#define     PRT_WITHDATA         1      /* with data */
#define     PRT_WITHOUTDATA      0      /* without data */

#define     PRT_KPITEM           1      /* kitchen item */
#define     PRT_NOTKPITEM        0      /* not kitchen item */

#define     PRT_NOT_PRINT_PLU_NO 9950   /* not print plu no */

// See function PrtChkPrintType() determines the print type by inspecting the ITEMSALES
// data and the print context. In addition these are used in functionality such as kitchen
// receipt printing to specify what information should be displayed on the receipt.
//
// See function PrtSetItem() for an example of how receipt printing behavior is governed
// by these various flags.
#define     PRT_ONLY_NOUN            1      /* noun mnemonics without the alt */
#define     PRT_ONLY_PMCOND          2      /* only condiments */
#define     PRT_ALL_ITEM             3      /* all mnemonics */
#define     PRT_NOT_ZERO_COND        4      /* not print zero price condiment */
#define     PRT_ONLY_NOUN_AND_ALT    5      /* noun mnemonics with the alt if it exists */
#define     PRT_ONLY_PMCOND_AND_ALT  6      /* only condiments with the alt if it exists */

#define     PRT_FOREIGN_3DEC  0x04      /* 3digits under dicimal point */
#define     PRT_FOREIGN_2DEC  0x00      /* 2digits under dicimal point */
#define     PRT_FOREIGN_NODEC 0x06      /* no digit under dicimal point */

#define     PRT_CASHIER          0      /* cashier */
#define     PRT_WAITER           1      /* waiter */

#define     PRT_KPMASK       0x000F     /* kitchen printer status mask */
#define     PRT_KPMASK2      0x00F0     /* kitchen printer status mask */
#define     PRT_RED          0x0020     /* kitchen printer status red  */

// STD_ADJMNEMO_LEN + 1 + STD_PLUNAME_LEN + 1 + STD_MAX_COND_NUM * (STD_PLUNAME_LEN + 1)
// PRT_MAXITEM, MLD_MAXITEM, PRT_KPMAXITEM need to be maintained to the correct size
// changes to PRT_KPMAXITEM may require changes in KPS_FRAME_SIZE and KPS_SIZE_FOR_1ITEM
#define     PRT_KPMAXITEM       (699+(21 * 30))  /* kp item max  byte, Alt Mnemonics Rel 2.2.2, 2018  */
//#define     PRT_KPMAXITEM       699     /* kp item max  byte, V3.3     */
                                        /* 2(header)                   */
                                        /* 4+1+1(seat no.)             */
                                        /* 8+4+1(qty)                  */
										/* (US Custons #Type entry)---------
                                        /* 4+1+20+1(noun)                  |---->((21*1)*5)*/
										/* (US Custons #Type entry)---------*/
                                        /* ((21+1) * 20(condiment)     */
										/* Increase buffer size for increase from
										10 condiment to 20 in print buffer */
/*#define     PRT_KPMAXITEM       399     /* kp item max  byte, V3.3     */
                                        /* 2(header)                   */
                                        /* 4+1+1(seat no.)             */
                                        /* 8+4+1(qty)                  */
										/* (US Custons #Type entry)---------
                                        /* 4+1+20+1(noun)                  |---->((21*1)*5)
										/* (US Custons #Type entry)---------
                                        /* ((21+1) * 11(condiment)     */
/*#define     PRT_KPMAXITEM       270     /* kp item max  byte, V3.3     */
                                        /* 2(header)                   */
                                        /* 4+1+1(seat no.)             */
                                        /* 8+4+1(qty)                  */
                                        /* 4+1+20+1(noun)              */
                                        /* ((21+1) * 10(condiment)     */
/* #define     PRT_KPMAXITEM       260     / kp item max  byte, V3.3     */
                                        /* 2(header)                   */
                                        /* 4+1+1(seat no.)             */
                                        /* 8+4+1(qty)                  */
                                        /* 4+1+12+1(noun)              */
                                        /* ((21+1) * 10(condiment)     */
/* #define     PRT_KPMAXITEM       170     / kp item max  byte           */
                                        /* 2(header)                   */
                                        /* 8+4+1(qty)                  */
                                        /* 4+1+12+1(noun)              */
                                        /* (12+1) * 10(condiment)      */
#define     PRT_SP_BACKFEED       5     /* number of backfeed to release slip paper */

#define     PRT_AMOUNTLEN         4     /* length of sales item's amount */

// Flags for kitchen printer target filter to restrict kitchen print to remote printers only.
// Flag indicates to check MDC MDC_DSP_ON_FLY and MDC_DSP_ON_FLY2 to restrict kitchen printing
// only to actual remote printers when check PLU data for actual kitchen printers. In most cases
// for kitchen printing functionality we want the printing to only go to a remote printer and not
// to a remote CRT.
#define     PRT_MDC               0     /* refer to MDC MDC_DSP_ON_FLY settings to remove remote CRTs */
#define     PRT_NOT_MDC        0x01     /* do not refer to MDC MDC_DSP_ON_FLY settings to remove remote CRTs */


#define     PRT_WTIN              0     /* Etk time-out with time-in */
#define     PRT_WOIN           0x01     /* Etk time-out without time-in */

#define     PRT_FULL_SP           1     /* 1 line is filled with space/ NULL */

#define     PRT_TEND_ROOM         1     /* Room Charge tender               */      
#define     PRT_TEND_ACCT         2     /* Acct Charge tender               */      
#define     PRT_TEND_CREAUTH      3     /* Credit Authorization tender      */      
#define     PRT_TEND_EPT          4     /* EPT tender                       */      
#define     PRT_TEND_OTHER        0     /* other tender                     */      

#define     PRT_VATCOLUMN          3    /* VAT shift column, V3.3           */

#define     PRT_ENDORSE_HORIZNTL  0x80    /* DTREE#2, if on then check inserted sideways for endorsement, print narrow */

#define     PRT_TAX_US              0       /* US Tax System, 21RFC05437 */
#define     PRT_TAX_CANADA          1       /* Canada Tax System */
#define     PRT_TAX_INTL            2       /* Int'l VAT System */
#define     PRT_MASKSTATCAN         0x0f	/* Canadian Tax Mask */

/***	Send Prints to specific file or to a printer	*/ //ESMITH PRTFILE
// used as an argument to function PrtSetPrintMode() to set the output device
// allowing output to other places such as a file.
#define		PRT_SENDTO_MASKFUNC		0x7f	/* Mask for file selection */
#define		PRT_SENDTO_PRINTER		0x00	/* Send Printing to printer */ 
#define		PRT_SENDTO_FILE_REG		0x01	/* Send Printing to regiser log file */
#define		PRT_SENDTO_FILE_PRG		0x02	/* Send Printing to program log file */
#define		PRT_SENDTO_FILE_SUP		0x04	/* Send Printing to supervisor log file */
#define     PRT_SENDTO_WEB_TEMP     0x08    /* Send output to a file for output for web interface */

//Defines for use with double sided printing, we have to send information
//to tell the printer (if it can print on two sides), which side to print on. JHHJ
#define		PRT_DBLSIDE_FRONT		0
#define		PRT_DBLSIDE_BACK		1

// Foreign currency decimal point settings
// Set in MDC #79, 80, 337, etc. for the foreign Currency Tender keys.
// There is still a dependency on the MDC physical bit mask so be careful if making changes.
#define PRT_FC_MASK                0x06      // indicates assumed decimal point for foreign currency, DISP_SAVE_FOREIGN_MASK
#define PRT_FC_DECIMAL_0           0x06      // foreign currency entry with no decimal point, DISP_SAVE_DECIMAL_0
#define PRT_FC_DECIMAL_2           0x00      // foreign currency entry with 2 decimal point, DISP_SAVE_DECIMAL_2
#define PRT_FC_DECIMAL_3           0x04      // foreign currency entry with 3 decimal point, DISP_SAVE_DECIMAL_3

/**
;========================================================================
;+                                                                      +
;+              P R O T O T Y P E    D E C L A R A T I O N s            +
;+                                                                      +
;========================================================================
**/

//----------------------------------------------------------
//  PMG_WIN_PRINTER		function prototypes
// function prototypes for standard Windows printer follows
// this is typically for Windows connected printers
VOID    PmgWinInitialize( VOID );
VOID    PmgWinFinalize( VOID );
USHORT  PmgWinPrint( USHORT, TCHAR *, USHORT);
VOID    PmgWinWait( VOID );
USHORT  PmgWinBeginValidation( USHORT );
USHORT  PmgWinEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  PmgWinBeginSmallValidation( USHORT );
USHORT  PmgWinEndSmallValidation( USHORT );
USHORT  PmgWinFeed( USHORT, USHORT );
USHORT  PmgWinBeginImportant( USHORT );
USHORT  PmgWinEndImportant( USHORT );
USHORT  PmgWinGetStatus( USHORT, USHORT * );
USHORT  PmgWinGetStatusOnly( USHORT, USHORT * );
USHORT  PmgWinPrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgWinFontLoad( VOID );
#else
USHORT  PmgWinInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgWinFont( USHORT, USHORT );
#endif
VOID    PmgWinSetValWaitCount( USHORT );
VOID	PmgWinStartReceipt(VOID);
VOID	PmgWinEndReceipt(VOID);
VOID	PmgWinBeginTransaction( SHORT sType, ULONG ulTransNo );
VOID	PmgWinEndTransaction( SHORT sType, ULONG ulTransNo );

//----------------------------------------------------------
// 
VOID    PmgFileInitialize( VOID );
VOID    PmgFileFinalize( VOID );
USHORT  PmgFilePrint( USHORT, TCHAR *, USHORT);
VOID    PmgFileWait( VOID );
USHORT  PmgFileBeginValidation( USHORT );
USHORT  PmgFileEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  PmgFileBeginSmallValidation( USHORT );
USHORT  PmgFileEndSmallValidation( USHORT );
USHORT  PmgFileFeed( USHORT, USHORT );
USHORT  PmgFileBeginImportant( USHORT );
USHORT  PmgFileEndImportant( USHORT );
USHORT  PmgFileGetStatus( USHORT, USHORT * );
USHORT  PmgFileGetStatusOnly( USHORT, USHORT * );
USHORT  PmgFilePrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgFileFontLoad( VOID );
#else
USHORT  PmgFileInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgFileFont( USHORT, USHORT );
#endif
VOID    PmgFileSetValWaitCount( USHORT );
VOID	PmgFileStartReceipt(VOID);
VOID	PmgFileEndReceipt(VOID);
USHORT  PmgFileGetNextPrtType (VOID);
VOID   *PmgFileGetThisFileInfo (USHORT usPrtType);
USHORT  PmgFileGetThisPrtType (VOID *pvOutput);
TCHAR  *PmgFileGetThisFileName (VOID *pvOutput);
VOID   *PmgFileCloseThisFile (USHORT usPrtType);
TCHAR  *PmgFileGetThisFileNamePrt (USHORT usPrtType);

//----------------------------------------------------------
// 
VOID    PmgConnEngineInitialize( VOID );
VOID    PmgConnEngineFinalize( VOID );
USHORT  PmgConnEnginePrint( USHORT, TCHAR *, USHORT);
VOID    PmgConnEngineWait( VOID );
USHORT  PmgConnEngineBeginValidation( USHORT );
USHORT  PmgConnEngineEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  PmgConnEngineBeginSmallValidation( USHORT );
USHORT  PmgConnEngineEndSmallValidation( USHORT );
USHORT  PmgConnEngineFeed( USHORT, USHORT );
USHORT  PmgConnEngineBeginImportant( USHORT );
USHORT  PmgConnEngineEndImportant( USHORT );
USHORT  PmgConnEngineGetStatus( USHORT, USHORT * );
USHORT  PmgConnEngineGetStatusOnly( USHORT, USHORT * );
USHORT  PmgConnEnginePrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgConnEngineFontLoad( VOID );
#else
USHORT  PmgConnEngineInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgConnEngineFont( USHORT, USHORT );
#endif
VOID    PmgConnEngineSetValWaitCount( USHORT );
VOID	PmgConnEngineBeginTransaction( SHORT sType, ULONG ulTransNo );
VOID	PmgConnEngineEndTransaction( SHORT sType, ULONG ulTransNo );

//----------------------------------------------------------
// PMG_OPOS_PRINTER		function prototypes
// function prototypes for standard OPOS printer follows
// this is typically for NCR receipt and validation printers
VOID    PmgOPOSInitialize( VOID );
VOID    PmgOPOSFinalize( VOID );
USHORT  PmgOPOSPrint( USHORT, TCHAR *, USHORT);
VOID    PmgOPOSWait( VOID );
USHORT  PmgOPOSBeginValidation( USHORT );
USHORT  PmgOPOSEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  PmgOPOSBeginSmallValidation( USHORT );
USHORT  PmgOPOSEndSmallValidation( USHORT );
USHORT  PmgOPOSFeed( USHORT, USHORT );
USHORT  PmgOPOSBeginImportant( USHORT );
USHORT  PmgOPOSEndImportant( USHORT );
USHORT  PmgOPOSGetStatus( USHORT, USHORT * );
USHORT  PmgOPOSGetStatusOnly( USHORT, USHORT * );
USHORT  PmgOPOSPrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgOPOSFontLoad( VOID );
#else
USHORT  PmgOPOSInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgOPOSFont( USHORT, USHORT );
#endif
LONG    PmgOPOSGetSetPrtCap (USHORT usPrtType, USHORT usPrtCap, LONG lValue);
USHORT	PmgOPOSPrintBarCode (USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags);
VOID    PmgOPOSSetValWaitCount( USHORT );
VOID	PmgOPOSBeginTransaction( SHORT sType, ULONG ulTransNo );
VOID	PmgOPOSEndTransaction( SHORT sType, ULONG ulTransNo );


//----------------------------------------------------------
// PMG_COM_PRINTER		function prototypes
// function prototypes for standard direct to COM printer follows
// this is typically for NCR receipt and validation printers
VOID    PmgCOMInitialize(VOID);
VOID    PmgCOMFinalize(VOID);
USHORT  PmgCOMPrint(USHORT, TCHAR *, USHORT);
VOID    PmgCOMWait(VOID);
USHORT  PmgCOMBeginValidation(USHORT);
USHORT  PmgCOMEndValidation(USHORT);
//US Customs slip printer control change for short forms
USHORT  PmgCOMBeginSmallValidation(USHORT);
USHORT  PmgCOMEndSmallValidation(USHORT);
USHORT  PmgCOMFeed(USHORT, USHORT);
USHORT  PmgCOMBeginImportant(USHORT);
USHORT  PmgCOMEndImportant(USHORT);
USHORT  PmgCOMGetStatus(USHORT, USHORT *);
USHORT  PmgCOMGetStatusOnly(USHORT, USHORT *);
USHORT  PmgCOMPrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgCOMFontLoad(VOID);
#else
USHORT  PmgCOMInitialData(USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgCOMFont(USHORT, USHORT);
#endif
LONG    PmgCOMGetSetPrtCap(USHORT usPrtType, USHORT usPrtCap, LONG lValue);
USHORT	PmgCOMPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags);
VOID    PmgCOMSetValWaitCount(USHORT);
VOID	PmgCOMBeginTransaction(SHORT sType, ULONG ulTransNo);
VOID	PmgCOMEndTransaction(SHORT sType, ULONG ulTransNo);

//----------------------------------------------------------
// PMG_ZEBRA_PRINTER		function prototypes
// function prototypes for the Zebra OPOS printer follows
// this is typically for Zebra Bluetooth receipt printers
VOID    PmgZEBRAInitialize( VOID );
VOID    PmgZEBRAFinalize( VOID );
USHORT  PmgZEBRAPrint( USHORT, TCHAR *, USHORT);
VOID    PmgZEBRAWait( VOID );
USHORT  PmgZEBRABeginValidation( USHORT );
USHORT  PmgZEBRAEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  PmgZEBRABeginSmallValidation( USHORT );
USHORT  PmgZEBRAEndSmallValidation( USHORT );
USHORT  PmgZEBRAFeed( USHORT, USHORT );
USHORT  PmgZEBRABeginImportant( USHORT );
USHORT  PmgZEBRAEndImportant( USHORT );
USHORT  PmgZEBRAGetStatus( USHORT, USHORT * );
USHORT  PmgZEBRAGetStatusOnly( USHORT, USHORT * );
USHORT  PmgZEBRAPrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgZEBRAFontLoad( VOID );
#else
USHORT  PmgZEBRAInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgZEBRAFont( USHORT, USHORT );
#endif
VOID    PmgZEBRASetValWaitCount( USHORT );
VOID	PmgZEBRABeginTransaction( SHORT sType, ULONG ulTransNo );
VOID	PmgZEBRAEndTransaction( SHORT sType, ULONG ulTransNo );


//----------------------------------------------------------
// PMG_OPOS_PRINTER		function prototypes
// The standard interface into the pmg library.  These functions are
// used to perform printer operations and the pmg library then
// uses one of the specific printer interfaces such as pmgOPOS or pmgWIN
// to do the actual printer operation requrested.
VOID    PmgInitialize( VOID );
#if 0
USHORT  PmgPrint_Debug(USHORT usPrtType, TCHAR *pucBuff, USHORT usLen, char *aszFilePath, int nLineNo);               /* Convert error code  */
#define PmgPrint(usPrtType,pucBuff,usLen) PmgPrint_Debug((usPrtType), (pucBuff), (usLen), __FILE__, __LINE__)
#else
USHORT   PmgPrint( USHORT, TCHAR *, USHORT);
#endif
USHORT  PmgDblShrdPrint( USHORT, TCHAR *, USHORT);
USHORT  PmgPrintf( USHORT, const TCHAR FARCONST *, ...);
VOID    PmgStartReceipt ( VOID );
VOID    PmgEndReceipt ( VOID );
VOID    PmgWait( VOID );
USHORT  PmgBeginValidation( USHORT );
USHORT  PmgEndValidation( USHORT );
//US Customs slip printer control change for short forms
USHORT  PmgBeginSmallValidation( USHORT );
USHORT  PmgEndSmallValidation( USHORT );
#if 0
USHORT  PmgFeed_Debug (USHORT usPrtType, USHORT usLine, char *aszFilePath, int nLineNo);
#define PmgFeed(usPrtType,usLine)  PmgFeed_Debug(usPrtType,usLine, __FILE__,__LINE__)
#else
USHORT  PmgFeed( USHORT, USHORT );
#endif
USHORT  PmgBeginImportant( USHORT );
USHORT  PmgEndImportant( USHORT );
#if 0
USHORT  PmgGetStatus_Debug( USHORT usPrtType, USHORT *pfbStatus, char *aszFile, int nLineNo );
USHORT  PmgGetStatusOnly_Debug( USHORT usPrtType, USHORT *pfbStatus, char *aszFile, int nLineNo );

#define PmgGetStatus(usPrtType,pfbStatus)     PmgGetStatus_Debug(usPrtType, pfbStatus,__FILE__,__LINE__)
#define PmgGetStatusOnly(usPrtType,pfbStatus)    PmgGetStatusOnly_Debug(usPrtType,pfbStatus,__FILE__,__LINE__)
#else
USHORT  PmgGetStatus( USHORT, USHORT * );
USHORT  PmgGetStatusOnly( USHORT, USHORT * );
#endif
USHORT  PmgPrtConfig(USHORT, USHORT *, USHORT *);
#ifdef  TPOS
USHORT  PmgFontLoad( VOID );
#else
USHORT  PmgInitialData( USHORT, CHAR *, SHORT, CHAR *);
USHORT  PmgFont( USHORT, USHORT );
#endif
LONG    PmgGetSetCap( USHORT usPrtType, USHORT usPrtCap, LONG lValue);
USHORT	PmgPrintBarCode(USHORT usPrtType, TCHAR *pucBuff, ULONG ulTextFlags, ULONG ulCodeFlags);
VOID    PmgSetValWaitCount( USHORT usRetryCount);
VOID	PmgBeginTransaction( SHORT sType, ULONG ulTransNo );
VOID	PmgEndTransaction( SHORT sType, ULONG ulTransNo);

#if 0
USHORT  PmgCallBack_Debug (DWORD dwErrorCode, SHORT  usSilent, char *aszFilePath, int nLineNo);
#define PmgCallBack(dwErrorCode,usSilent)  PmgCallBack_Debug(dwErrorCode,usSilent,__FILE__,__LINE__)
#else
USHORT  PmgCallBack (DWORD dwErrorCode, SHORT  usSilent);
#endif
USHORT  PmgCallBackDisplayStatus (DWORD dwErrorCode);
BOOL    PmgCallBackGetLastStatus (DWORD *dwLastErrorCode);

#endif
/***    End of PMG.H    ***/
