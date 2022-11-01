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
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories    :
*
*   Date           :NAME           :Description
*   May-07-92      :H.NAKASHIMA    :Initial
*
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
#define ISP_TTL_CPN                 0x0040  /* Check or Error for  COUPON   */
#define ISP_TTL_RESET_FLAG          0x006f  /* Reset flag                   */

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
*===========================================================================
*/
SHORT   STUBENTRY   IspLogOn(UCHAR *puchIPAddr, LPCWSTR puchPassWord);
SHORT   STUBENTRY   IspHostLogOn(LPCWSTR awzHostName, LPCWSTR awzPassWord);
SHORT   STUBENTRY   IspChangePassWord(LPCWSTR puchPassWord);
SHORT   STUBENTRY   IspLockKeyBoard(VOID);
SHORT   STUBENTRY   IspUnLockKeyBoard(VOID);
SHORT   STUBENTRY   IspLogOff(VOID);
SHORT   STUBENTRY   IspCheckTotals(SHORT fsCheckTotal);
SHORT   STUBENTRY   IspCheckAsMaster(VOID);
SHORT   STUBENTRY   IspReqBroadcast(UCHAR uchNbFlag);
SHORT   STUBENTRY   IspNoOfSatellite(UCHAR *puchNoOfSat);
SHORT	STUBENTRY	IspCheckModeOnTerm(VOID);

/*===== END OF DEFINITION =====*/
