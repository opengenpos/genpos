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
*   Title              : Client/Server STUB, Header file for internal
*   Category           : Client/Server STUB, US Hospitality Model
*   Program Name       : PCSTUB.H                                            
*  ------------------------------------------------------------------------
*	Abstract    	   : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*	Update Histories    :
*
*	Date		   :NAME		   :Description
*	May-07-92      :H.NAKASHIMA    :Initial
*       Sep-07-98      :O.Nakada       :Add V3.3
*
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
===========================================================================
    DEFINE
===========================================================================
*/
#define     PCS_DEFMDC_CLIENT   30
#define     PCS_DEFMDC_RETRY     0
#define     PCS_DEFMDC_WAIT      0

/*
------------------------------------------------
    fsCheckTotal
------------------------------------------------
*/
#define ISP_TTL_DEPARTMENT          0x0001  /* Check or Error for  Cashier  */
#define ISP_TTL_PLU                 0x0002  /* Check or Error for  PLU      */
#define ISP_TTL_WAITER              0x0004  /* Check or Error for  WAITER   */
#define ISP_TTL_CASHIER             0x0008  /* Check or Error for  CASHIER  */
#define ISP_TTL_PLU_MAINT           0x0010  /* Check or Error for  PLU Maint */
#define ISP_TTL_ETK                 0x0020  /* Check or Error for  ETK      */
#define ISP_TTL_CPN                 0x0040  /* Check or Error for  CPN R3.0 */
#define ISP_TTL_RESET_FLAG          0x006f  /* Reset flag    Chg R3.0       */

/*
 *    Notice Board indicators for parameter broadcast function IspReqBroadcast().
 *    The following are bit maps used to indicate the type of broadcast requested.
 *    Use only one of these in a call to function IspReqBroadcast().
**/
#if !defined(NB_REQSUPER)
#define NB_REQPLU           0x0001          /* Request PLU/OEP parameter with layout refresh */
#define NB_REQSUPER         0x0002          /* Request SUP parameter */
#define NB_REQALLPARA       0x0004          /* Request All parameter */
#define	NB_REQLAYOUT		0x0010			/* Request Layout Files ESMITH LAYOUT */
#endif

/*
------------------------------------------------
	Current Terminal Mode
------------------------------------------------
*/
#define UIE_POSITION0       0               /* P-2 mode        */
#define UIE_POSITION1       1               /* power off mode  */
#define UIE_POSITION2       2               /* lock mode       */
#define UIE_POSITION3       3               /* register mode   */
#define UIE_POSITION4       4               /* supervisor mode */
#define UIE_POSITION5       5               /* program mode    */

/*
*===========================================================================
*   Function Prototype Declarations

    NOTE:  New functions added to this library
         may also need to have an entry point
		 added to the file NCR2172Stub.def in
		 order to make the entry point visible.

         You will know to do this if you see
		 link errors with your new function specifed
		 as not found, such as
		 error LNK2001: unresolved external symbol
		 fatal error LNK1120

*===========================================================================
*/
SHORT   STUBENTRY   IspLogOn(UCHAR *puchIPAddr, LPCWSTR puchPassWord);
SHORT   STUBENTRY   IspHostLogOn(LPCWSTR awzHostName, LPCWSTR awzPassWord);	//PDINU
SHORT   STUBENTRY   IspChangePassWord(LPCWSTR puchPassWord);
SHORT   STUBENTRY   IspLockKeyBoard(VOID);
SHORT   STUBENTRY   IspUnLockKeyBoard(VOID);
SHORT   STUBENTRY   IspLogOff(VOID);
SHORT   STUBENTRY   IspCheckTotals(SHORT fsCheckTotal);
SHORT   STUBENTRY   IspCheckAsMaster(VOID);
SHORT   STUBENTRY   IspReqBroadcast(UCHAR uchNbFlag);           /* R3.1 */
SHORT   STUBENTRY   IspNoOfSatellite(UCHAR *puchNoOfSat);   /* R3.1 */
SHORT   STUBENTRY   IspDateTimeRead(CLIDATETIME *pDateTime);    /* R3.3 */
SHORT   STUBENTRY   IspDateTimeWrite(CLIDATETIME *pDateTime);   /* R3.3 */
SHORT   STUBENTRY   IspSysConfig(CLISYSPARA *pSysPara);         /* R3.3 */
SHORT   STUBENTRY   IspRecvResetLog(UCHAR   uchAction);             /* R3.3 */
SHORT   STUBENTRY   IspRecvResetLog2(LPCWSTR pszMnemonic);       /* V3.3 */
SHORT   STUBENTRY   IspRecvResetLog3(LPCWSTR pszMnemonic);       /* V3.3 */
SHORT	STUBENTRY	IspCheckModeOnTerm(VOID);

/*===== END OF DEFINITION =====*/
