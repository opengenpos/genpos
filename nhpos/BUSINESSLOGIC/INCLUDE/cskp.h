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
*   Title              : Client/Server KITCHEN PRINTER module, Header file for user
*   Category           : Client/Server KITCHEN PRINTER module, NCR2170 US HOSPITALITY MODEL 
*   Program Name       : CSKP.C
*  ------------------------------------------------------------------------
*    Abstract           :The provided function names are as floolws.
*
*                          define, typedef, struct, prototype, memory
*
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date                 Ver.Rev    :NAME         :Description
*   May-06-92          : 00.00.01   :M.YAMAMOTO   :Initial
*
*** 2172 Rel1.0 (Saratoga)
*   Mar-06-99          : 01.00.00   :K.Iwata      :
*   Mar-09-00          :            :M.Teraki     :Change KPS_COUNTER_QUE
*
** GenPOS **
*   Jan-11-18 : 02.02.02 : R.Chambers  :implementing Alt PLU Mnemonic in Kitchen Printing.
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
/*
-------------------------------------------------------------------------
   DEFINE
-------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------
   TYPEDEF
-------------------------------------------------------------------------
*/
/* Kitchen Printer subsystem error codes
   These error codes are converted by function KpsConvertError()
   into LDT_ type codes for use by function UieErrorMsg() to
   display errors to the operator.

   These codes should be synchronized with similar values used by
   the lower level STUB_ error codes as well as the Totals
   error codes defined in file csttl.h and the Guest Check
   error codes defined in file csgcs.h
 */
/* -- function return value --
	Try to keep in synch with STUB_ values in csstbfcc.h
	with SERV_ values in servmin.h, and SHR_ values in shr.h
*/
#define  KPS_PERFORM               0
#define  KPS_TIMER_ON              1
#define  KPS_PRINTER_BUSY         (-3)
#define  KPS_TIME_OUT             (-7)
#define  KPS_ILLEGAL              (-8)
#define  KPS_NOT_MASTER          (-20)
#define  KPS_PRINTER_DOWN        (-31)
#define  KPS_PRINTER_DOWN_MIN    (-31) /* ###DEBUG */
#define  KPS_BUFFER_FULL         (-33)
#define  KPS_BUFFER_PRINTER_ERR  (-37)    // target printer has error condition

// Following are used to transform Network Layer (STUB_) errors into KPS_ errors
// Notice that error code is same as STUB_ plus STUB_RETCODE.
#define	KPS_M_DOWN_ERROR        (STUB_RETCODE+STUB_M_DOWN)
#define	KPS_BM_DOWN_ERROR       (STUB_RETCODE+STUB_BM_DOWN)
#define KPS_BUSY_ERROR          (STUB_RETCODE+STUB_BUSY)
#define KPS_TIME_OUT_ERROR      (STUB_RETCODE+STUB_TIME_OUT)
#define KPS_UNMATCH_TRNO        (STUB_RETCODE+STUB_UNMATCH_TRNO)
#define KPS_DUR_INQUIRY         (STUB_RETCODE+STUB_DUR_INQUIRY)


/* ### ADD 2172 Rel1.0 */
#define KPS_NOT_DOWN_PRINTER			0x00			/* Printer not down								*/
#define KPS_DOWN_PRINTER                0x01			/* Printer down										*/
#define	KPS_DOWN_AND_OPEN				0x03			/* Printer down and cover open						*/
#define	KPS_DOWN_AND_OUT				0x05			/* Printer down and paper out						*/
#define KPS_DOWN_AND_TROUBLE			0x09			/* Printer down and trouble (com or general errors) */
#define KPS_SHARED_DOWN					0x10			/* Kitchen Printer Down (but is shared as well)     */


/*
-------------------------------------------------------------------------
    Print Data Frame Define
-------------------------------------------------------------------------
*/
#define  KPS_START_FRAME             0x01
#define  KPS_CONTINUE_FRAME          0x02
#define  KPS_END_FRAME               0x80

#define  KPS_RED_CODE                0x0E   /* Print Color is red (0x0e = sift out code) */


#define  KPS_NUMBER_OF_PRINTER     MAX_DEST_SIZE        /* No. of Remote Printer (R3.1)  See also PRT_KPRT_MAX */
#define  KPS_FRAME_SIZE              1024              // increased Jan-11-18 R.Chambers, Alt PLU mnemonic

#define  KPS_TRAILER_SIZE             44

#define KPS_CHK_KPNUMBER(x)     ((1 <= x) && (x <= KPS_NUMBER_OF_PRINTER))

/* ### CAUTION! 16bit only  */
#define KPS_MASK16_ALT_MAN      (0x8000)        /* MANUAL ALT           */
#define KPS_MASK16_ALT_AUTO     (0x4000)        /* AUTO ALT             */

/*
-------------------------------------------------------------------------
   STRUCT
-------------------------------------------------------------------------
*/
/*
-------------------------------------------------------------------------
   PROTOTYPE
-------------------------------------------------------------------------
*/
VOID     KpsInit(VOID);
/* ### MOD 2172 Rel1.0 (Saratoga) */
/*SHORT    KpsPrint(UCHAR *auchRcvBuffer, USHORT usRcvBufLen, 
                  UCHAR uchUniqueAddress, USHORT *usPrintStatus);*/
SHORT    KpsPrint(UCHAR *auchRcvBuffer, USHORT usRcvBufLen, 
                  UCHAR uchUniqueAddress, USHORT *usPrintStatus,USHORT usOutPrinterInfo);
VOID     KpsUnlock(VOID);
USHORT   KpsConvertError(SHORT sError);
SHORT    KpsCheckPrint(VOID);
SHORT    KpsCreateFile(USHORT usSize);
VOID     KpsDeleteFile(VOID);





