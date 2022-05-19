/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Notice Board Header                        
* Category    : Notice Board , NCR 2170 US Hospitality Model         
* Program Name: NB.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-27-92:00.00.01:H.Yamaguchi: initial                                   
* Apr-17-93:00.00.08:H.Yamaguchi: Adds reset consecutive no.                                   
* Apr-21-95:03.00.00:hkato      : R3.0, Flexible Drive Through
*
** NCR2172 **
*
* Oct-05-99:01.00.00:M.Teraki   :initial (for Win32)
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
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define NB_MAX_MESSAGE      4               /* Max size of Message */
#define NB_MESOFFSET0       0x0             /* Message offset address 0 */
#define NB_MESOFFSET1       0x0001          /* Message offset address 1 */
#define NB_MESOFFSET2       0x0002          /* Message offset address 2 */
#define NB_MESOFFSET3       0x0003          /* Message offset address 3 */

/* System Information Flag */

#define NB_RSTUPANDON       0x2ff0          /* Reset up to date & online flag */
#define NB_RSTSYSFLAG       0x0003          /* Reset system flag              */
#define NB_MTUPTODATE       0x0001          /* Master is up to date           */
#define NB_BMUPTODATE       0x0002          /* B-Master is up to date         */
#define NB_STARTASMASTER    0x0004          /* Starts as master               */
#define NB_STARTONLINE      0x0008          /* Starts Online                  */
#define NB_INQUIRY          0x1000          /* Inquiry state                  */
#define NB_WITHOUTBM        0x2000          /* Without Backup Master          */
#define NB_SETBMONLINE      0x4000          /* Set BM online                  */
#define NB_SETMTONLINE      0x8000          /* Set M online                   */

/* Backup Information Flag
	These flags are used with NbSysFlag.fsMBackupInf and NbSysFlag.fsBMBackupInf to
	communicate various backup types of behaviors between Master Terminal and Backup Master.
**/
#define NB_EXECUTEBKCOPY    0x0001          /* Execute Backup copy  */
#define NB_REQUESTBKCOPY    0x0002          /* Request Backup copy  */
#define NB_STARTBKCOPY      0x0004          /* Start  Backup copy   */
#define NB_STOPALLFUN       0x0008          /* Stop  all function   */
#define NB_AC85_MODE        0x0010          /* AC85 performed so stay Master   */
#define NB_AUTO85_MODE      0x0020          /* Backup Master detected data change and Master not up to date   */

/* Request parameter flag */
// auchMessage[NB_MESOFFSET0]; file/parameter change and file/parameter download request
#define NB_REQPLU           0x0001          /* Request PLU parameter */
#define NB_REQSUPER         0x0002          /* Request SUP parameter */
#define NB_REQALLPARA       0x0004          /* Request All parameter */
#define NB_REQPLEVEL        0x0008          /* Request Price Level para. */
#define	NB_REQLAYOUT		0x0010			/* Request Layout Files ESMITH LAYOUT */
#define NB_REQSETDBFLAG		0x0020			//delayed balance JHHJ used in offset 3.
#define NB_REQ_OPERATOR_MSG	0x0040			//request new Operator Message from master.
#define NB_REQRESETCONSEC   0x0080          /* Request reset consecutive no  */

#define NB_RSTALLPARA       0x000f          /* Reset All   parameter request */
//#define NB_RSTPLUPARA       0x0001          /* Reset PLU   parameter request */
//#define NB_RSTSUPPARA       0x0002          /* Reset Super parameter request */
//#define NB_RSTPLEVEL        0x0008          /* Reset Price Level para request */
//#define NB_RSTLAYREQ        0x0010          /* Reset Layout File request */ //ESMITH LAYOUT
//#define NB_RSTSETDBFLAG     0x0020          //delayed balance JHHJ used in offset 3.
//#define NB_RSTRESETCONSEC   0x0080          /* Reset Reset consecutive no    */

// auchMessage[NB_MESOFFSET1];  FDT Request GCF Status,    R3.0
#define NB_REQPAYMENT1      0x01            /* Exist GCF to Payment Terminal #1 */
#define NB_REQPAYMENT2      0x02            /* Exist GCF to Payment Terminal #2 */
#define NB_REQDELIVERY1     0x04            /* Exist GCF to Delivery Terminal #1 */
#define NB_REQDELIVERY2     0x08            /* Exist GCF to Delivery Terminal #2 */
#define NB_REQORDER1        0x10            /* Exist GCF to Order Terminal #1 */
#define NB_REQORDER2        0x20            /* Exist GCF to Order Terminal #2 */
#define NB_RSTALLFDT        0x3f            /* Mask */



/* Argument of NbWriteInfFlag */

#define NB_SYSTEMFLAG       1               /* System Information Flag          */
#define NB_MTBAKCOPY_FLAG   2               /* Master's backup copy inf. flag   */
#define NB_BMBAKCOPY_FLAG   3               /* B-Master's backup copy inf. flag */

/*
*---------------------------------------------------------------------------
*   Define Return Code 
*---------------------------------------------------------------------------
*/
/* External user interface error code */

#define NB_SUCCESS         0               /* Success                       */
#define NB_ILLEGAL        -1               /* Illegal parameter             */


/* Internal error code */

#define NB_INVALIDDATA    -30              /* Received invalid data         */
#define NB_RCVSELFDATA    -31              /* Received a self transmit data */ 

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

// NBMESSAGE is the struct that is used in several places
// to contain the cluster status information.  This struct
// is filled in from information that is received using a
// struct of NBCONTMES.
// The auchMessage[] array contains the following information
//  auchMessage[0] - used to communicate file and parameter transfer information
//  auchMessage[1] - used to communicate Flexible Drive Thru information
//  auchMessage[2] -  
//  auchMessage[3] -  
typedef struct {
        USHORT  fsSystemInf;
        USHORT  fsMBackupInf;  // NbExecRcvMonMes() puts NbConMes.fsBackupInf from Master here
        USHORT  fsBMBackupInf; // NbExecRcvMonMes() puts NbConMes.fsBackupInf from Backup here
		ULONG   ulMT_ParaHashCurrent;
		ULONG   ulBU_ParaHashCurrent;
        WCHAR   auchMessage[NB_MAX_MESSAGE];
}NBMESSAGE;


#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID   NbInit(VOID);                                      /* Creates UIC Task     */
SHORT  NbWriteMessage(USHORT usOffset, UCHAR uchReqFlag); /* Write degignate flag */
SHORT  NbResetMessage(USHORT usOffset, UCHAR uchRstFlag); /* Reset degignate flag */
SHORT  NbReadAllMessage(NBMESSAGE *pMes);                 /* Read all flag        */        
SHORT  NbWriteInfFlag(USHORT usType, USHORT fsInfFlag);   /* Write inf. flag      */
SHORT  NbClearInfFlag(USHORT usType, USHORT fsInfFlag);   /* Clear a particular inf. flag      */
SHORT  NbStartAsMaster(USHORT fsFlag);                    /* Starts As Master     */      
SHORT  NbStartOnline(USHORT fsFlag);                      /* Starts  Online       */      
SHORT  NbStartInquiry(VOID);                              /* Set INQUIRY flag     */      
VOID   NbWaitForPowerUp(VOID);                            /* Wait , when power up */
VOID   NbPrintPowerDown(SHORT sStatus);                   /* Power Down Log,  Saratoga */
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message("  ====++++====   ----------------------------------    ====++++====\z")
#pragma message("  ====++++====   NbSetMTOnline_Debug() is ENABLED     ====++++====")
USHORT  NbSetMTOnline_Debug(char *aszFilePath, int iLineNo);
#define NbSetMTOnline() NbSetMTOnline_Debug(__FILE__, __LINE__)
USHORT  NbResetMTOnline_Debug(char *aszFilePath, int iLineNo);
#define NbResetMTOnline() NbResetMTOnline_Debug(__FILE__, __LINE__)
USHORT  NbSetMTUpToDate_Debug(char *aszFilePath, int iLineNo);
#define NbSetMTUpToDate() NbSetMTUpToDate_Debug(__FILE__, __LINE__)
USHORT  NbResetMTUpToDate_Debug(char *aszFilePath, int iLineNo);
#define NbResetMTUpToDate() NbResetMTUpToDate_Debug(__FILE__, __LINE__)
USHORT  NbSetBMOnline_Debug(char *aszFilePath, int iLineNo);
#define NbSetBMOnline() NbSetBMOnline_Debug(__FILE__, __LINE__)
USHORT  NbResetBMOnline_Debug(char *aszFilePath, int iLineNo);
#define NbResetBMOnline() NbResetBMOnline_Debug(__FILE__, __LINE__)
USHORT  NbSetBMUpToDate_Debug(char *aszFilePath, int iLineNo);
#define NbSetBMUpToDate() NbSetBMUpToDate_Debug(__FILE__, __LINE__)
USHORT  NbResetBMUpToDate_Debug(char *aszFilePath, int iLineNo);
#define NbResetBMUpToDate() NbResetBMUpToDate_Debug(__FILE__, __LINE__)
#else
USHORT NbSetMTOnline(VOID);               /* Set   Masret online, return previous status   */
USHORT NbResetMTOnline(VOID);             /* Reset Master online, return previous status   */
USHORT NbSetMTUpToDate(VOID);               /* Set   Masret up to date   */
USHORT NbResetMTUpToDate(VOID);             /* Reset Master up to date   */
USHORT NbSetBMOnline(VOID);               /* Set   B-Master online, return previous status   */
USHORT NbResetBMOnline(VOID);             /* Reset B-Master online, return previous status   */
USHORT NbSetBMUpToDate(VOID);               /* Set   B-Master up to date */
USHORT NbResetBMUpToDate(VOID);             /* Reset B-Master up to date */
#endif
USHORT  NbSetMTBM_Online (UCHAR uchFadr);

/*========== END OF DEFINITION ============*/