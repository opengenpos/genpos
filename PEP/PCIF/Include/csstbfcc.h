#if POSSIBLE_DEAD_CODE
/*========================================================================*\
*   Title              : Client/Server STUB, Header file (Server Commom)
*   Category           : Client/Server STUB, US Hospitality model
*   Program Name       : CSSTBFCC.H
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories:
*
*   Date      :NAME        :Description
*   May-07-92 :H.NAKASHIMA :Initial
*   Sep-24-92 :T.KOJIMA    :Reserved the function codes of Remote
*                           File System
*   Jun-04-93 :H.YAMAGUCHI :MOdified same as HP Int'l
*   Oct-05-93 :H.YAMAGUCHI :Adds EtkStatRead/EtkCurIndWrite by FVT Comm't
*   Oct-20-93 :Yoshiko.Jim :add CLI_PORT_CPM for HOTEL MODEL
*   Oct-28-93 :H.YAMAGUCHI :Adds CPM function "0x0b??"
*   May-17-94 :Yoshiko.Jim :add CLI_FCTTLWAITTLCHK, CLI_FCTTLCASTTLCHK 
*   Apr-26-95 :M.Suzuki    :add CLI_FCGCFDELDLIVQUE, CLI_FCGCFINFOQUE R3.0 
*                          : CLI_FCSPSPOLLING
*   Jun-15-95:hkato        :R3.0
*   Mar-22-96:T.Nakahata  : R3.1 Initial
*                              Increase No. of Employees (200 => 250)
*                              Increase Employee No. ( 4 => 9digits)
*                              Add Employee Name (Max. 16 Char.)
*                              Add E/J Cluster Reset Function
*                              Add Promotional Pricing Item Feature
*   May-07-96:T.Nakahata  : R3.1 V3.01.05
*                               Add Cas/Wai terminal (All/Ind) lock
*
*   Jan-25-00:K.Yanagida  :Saratoga initial
*   May-29-00:K.Iwata     :
\*=======================================================================*/
/*
===========================================================================
    DEFINE
===========================================================================
*/

#if !defined(CSSTBFCC_INCLUDED)
#define CSSTBFCC_INCLUDED

/*------------------------------
    DEFINE FOR RETUR VALUE
------------------------------*/

#define     STUB_SUCCESS            0           /* normal success */

#define     STUB_M_DOWN           (-1)          /* failed request to master */
#define     STUB_BM_DOWN          (-2)          /* failed request to BM */
#define     STUB_BUSY             (-3)          /* busy master server/ stub */
#define     STUB_FRAME_SEQERR     (-4)          /* frame sequence error */
#define     STUB_UNMATCH_TRNO     (-5)          /* transaction # unmatched */
#define     STUB_STOP_UPDATE      (-6)          /* stop request total update */
#define     STUB_TIME_OUT         (-7)          /* time out detected */
#define     STUB_ILLEGAL          (-8)          /* illegal parameter */
#define     STUB_DUR_INQUIRY      (-9)          /* during inquiry state */

#define     STUB_MULTI_SUCCESS    (-10)         /* multiple request success */
#define     STUB_MULTI_SEND       (-11)         /* multiple sending */
#define     STUB_MULTI_RECV       (-12)         /* multiple receiving */
#define     STUB_REQ_CONFIRM      (-13)         /* confirmation request */
#define     STUB_FAITAL           (-14)         /* faital error */
#define     STUB_POWER_DOWN       (-15)         /* power down my terminal */
#define     STUB_INQ_M_DOWN       (-16)         /* Master down at iquiry */
#define     STUB_INQ_BM_DOWN      (-17)         /* B Master down at inquiry */
#define     STUB_NET_NOT_OPENED   (-18)         /* net work not opened */
#define     STUB_IAM_BUSY         (-19)         /* error on my terminal */
#define     STUB_NOT_MASTER       (-20)         /* I'm not master (O/S) */
#define     STUB_SELF             (-21)         /* It's my terminal */
#define     STUB_IGNORE           (-22)         /* ignore this message */

#define     STUB_DB_FULL		  (-23)			/* the delay balance total file is full.*/
#define     STUB_DB_NEARFULL	  (-24)			/* the delay balance total file is near full.*/

#define		STUB_KPS_BUSY		  (-25)			// Indicates failure due to kitchen printer busy
#define		STUB_NO_BACKUP		  (-26)			// Indicates a Backup Master is not provisioned in cluster
#define     STUB_COMERROR         (-37)         // Error found during request
#define		STUB_DB_BSY_UPDTING   (-38)			// Busy Updating Delay Balance File, please wait. JHHJ
#define     STUB_DISCOVERY        (-39)         // target terminal not found.  PIF_ERROR_NET_DISCOVERY
#define     STUB_NOT_UPTODATE     (-40)         // this terminal is not up to date (Master or Backup only)
#define     STUB_BUSY_MULTI       (-43)         /* busy master server/ stub during multi */

#define     STUB_PC_ERROR        (-100)         /* constant of PC STUB error */

/*------------------------------------------------
    Max Size Definition
-------------------------------------------------*/
#define     CLI_ALLTRANSNO      16      /* max. number of terminal */
#define     CLI_CASHIERNAME     20      /* Cashier name, V3.3 */
#define     CLI_CASHIERSTATUS    3      /* cashier status, V3.3 */
#define     CLI_WAITERNAME      10      /* Waiter name */
#define     CLI_MAX_REQDATA     /*477*/1400     /* Maximumm request data - These defines should match PIF_LEN_UDATA in R20_pif.h and pifnet.h*/*/
#define     CLI_MAX_RESDATA     /*477*/1400     /* Maximumm response data - These defines should match PIF_LEN_UDATA in R20_pif.h and pifnet.h*/*/
#define     CLI_MAX_IPDATA      8       /* Maximumm IP address data */
#define     CLI_PASSWORD        10      /* Maximum PC passward */

#define     CLI_PLU_MAX_NO       5      /* Maximum No of PLU,   Saratoga.
										 * Must match value of CLI_PLU_MAX_NO in NHPOS.
										 * This value will be multiplied by the size of various data structures (PLUIF, 
										 * PLUIF_REP, PLUIF_DEP, TTLPLU) to create a request data length. There will be 
										 * problems if this length exceeds the maximum request/response data length (PIF_LEN_UDATA, 
										 * CLI_MAX_REQDATA, CLI_MAX_RESDATA). */
#define     CLI_EMPLOYEE_NAME   /*16*/20      /* Employee Name, R3.1 */
#define     CLI_ISPLOG_LEN      24      /* ISP LOG Length, V3.3 */
#define     CLI_MAX_EEPTDATA    493     /* Maximumm EEPT data,  Saratoga */
#define     CLI_PLU_LEN         STD_PLU_NUMBER_LEN      /* PLU Length,          Saratoga */
#define     CLI_PLU_EX_MAX_NO   20      /* Maximum No of PLU,   Saratoga */
#define     CLI_PLU_NAME_SIZE   20
#define     CLI_OEP_PLU_SIZE    20
#define		CLI_ETKINDJOBREAD_RESHLEN 44 /*Length of response header for server response
										 in CliEtkIndJobRead(), used to provide proper
										 header offset to data.  Added 7/10/2003 by C.Wunn
										 */

/*------------------------------------------------
    Max Size Definition of Enhanced version
-------------------------------------------------*/
#define     CLI_MAX_DFLDATA     600     /* Maximumm DFL data  */
#define     CLI_MAX_EJDATA      (24*18) /* Maximumm EJ  data  */
#define     CLI_MAX_EJREPTDATA  (4*48)  /* Maximumm EJ  data  */
#define     CLI_NEW_ETK_JOB     15      /* ETK      Job codes */
#define     CLI_MAX_ETKDATA    (100)  /* Maximumm ETK data  */
#define     CLI_MAX_CPMDATA    (400)    /* Maximumm CPM data Modifed for new Charge Post IF */
#define     CLI_MAX_EJREAD      48      /* Maximum EJ Read Data, R3.1 */

/*------------------------------------------------
    Function Code Definition
-------------------------------------------------*/
#define     CLI_FCWCONFIRM       0x8000  /* With confirm */
#define     CLI_FCWBACKUP        0x1000  /* With Back up */
#define     CLI_FCCLIENTWB       0x2000  /* Client Function */
#define     CLI_FCSERVERWB       0x4000  /* Server Function */

#define     CLI_SETCONFIRM       0x2000  /* Set Confirm command code */
#define     CLI_SETBMDOWN        0x4000  /* Set B Master Down code */
#define     CLI_RSTBACKUPFG      0x9fff  /* Reset With Back up flag */
#define     CLI_RSTCONTCODE      0x0fff  /* Rest control bit (4 bits) */

#define     CLI_FCINQ            0x0000  /* Inquiry commom */
#define     CLI_FCINQINQUIRY     0x0001  /* Inquiry */
#define     CLI_FCINQCHGDATE     0x0002  /* Change Date */
#define     CLI_FCINQREQINQ      0x0003  /* Request Inquiry */
#define     CLI_FCINQINFSTAT     0x0004  /* Inform Status */
#define     CLI_FCINQREADSTAT    0x0005  /* Read Status */
#define     CLI_FCINQCHGSTAT     0x0006  /* Change Status */

#define     CLI_FCCAS            0x0100  /* Cashier commom */
#define     CLI_FCCASSIGNIN      0x0103  /* Cashier Sign In */  
#define     CLI_FCCASSIGNOUT     0x0104  /* Cashier Sign Out */
#define     CLI_FCCASPRESIGNOUT  0x0114  /* Cashier Pre Sign Out */
#define     CLI_FCCASASSIGN      0x0105  /* Cashier Assign */
#define     CLI_FCCASDELETE      0x0106  /* Cashier Delete */
#define     CLI_FCCASINDCLOSE    0x0107  /* Cashier Ind. Close */
#define     CLI_FCCASCLRSCODE    0x0108  /* Cashier clear secret code */
#define     CLI_FCCASALLIDREAD   0x0109  /* Cashier All Id Read */
#define     CLI_FCCASINDREAD     0x010a  /* Cashier individual Read */
#define     CLI_FCCASINDLOCK     0x010b  /* Cashier individual Lock */
#define     CLI_FCCASINDUNLOCK   0x010c  /* Cashier individual Un Lock */
#define     CLI_FCCASALLLOCK     0x010d  /* Cashier All Lock */
#define     CLI_FCCASALLUNLOCK   0x010e  /* Cashier All UnLock */
#define     CLI_FCCASCHKSIGNIN   0x010f  /* Cashier Check exist sign-in */
#define     CLI_FCCASINDTMLOCK   0x0110  /* Cashier Ind Terminal Lock R3.1 */
#define     CLI_FCCASINDTMUNLOCK 0x0111  /* Cashier Ind Terminal Unlock R3.1 */
#define     CLI_FCCASALLTMLOCK   0x0112  /* Cashier All Terminal Lock R3.1 */
#define     CLI_FCCASALLTMUNLOCK 0x0113  /* Cashier All Terminal Unlock R3.1 */
#define     CLI_FCCASOPENPICLOA  0x0116  /* Cashier Open  pickup/loan,  Saratoga 07/11/00 */
#define     CLI_FCCASCLOSEPICLOA 0x0115  /* Cashier Close pickup/loan,  Saratoga */
#define     CLI_FCBAKCASHIER     0x0150  /* Cashier back up copy */
#define		CLI_FCCASDELAYBALON	 0x0102	 /* delayed balance JHHJ*/

#define     CLI_FCWAI            0x0200  /* Waiter commom */
#define     CLI_FCWAISIGNIN      0x0203  /* Waiter Sign In */
#define     CLI_FCWAISIGNOUT     0x0204  /* Waiter Sign Out */
#define     CLI_FCWAIPRESIGNOUT  0x0214  /* Waiter Pre Sign Out */
#define     CLI_FCWAIASSIGN      0x0205  /* Waiter Assign */
#define     CLI_FCWAIDELETE      0x0206  /* Waiter Delete */
#define     CLI_FCWAIINDCLOSE    0x0207  /* Waiter Ind Close */
#define     CLI_FCWAICLRSCODE    0x0208  /* Waiter clear secret code */
#define     CLI_FCWAIALLIDREAD   0x0209  /* Waiter All Id Read */
#define     CLI_FCWAIINDREAD     0x020a  /* Waiter Ind Read */
#define     CLI_FCWAIINDLOCK     0x020b  /* Waiter individual Lock */
#define     CLI_FCWAIINDUNLOCK   0x020c  /* Waiter individual Un Lock */
#define     CLI_FCWAIALLLOCK     0x020d  /* Waiter All Lock */
#define     CLI_FCWAIALLUNLOCK   0x020e  /* Waiter All UnLock */
#define     CLI_FCWAICHKSIGNIN   0x020f  /* Waiter Check exist sign-in */
#define     CLI_FCWAIINDTMLOCK   0x0210  /* Waiter Ind Terminal Lock R3.1 */
#define     CLI_FCWAIINDTMUNLOCK 0x0211  /* Waiter Ind Terminal Unlock R3.1 */
#define     CLI_FCWAIALLTMLOCK   0x0212  /* Waiter All Terminal Lock R3.1 */
#define     CLI_FCWAIALLTMUNLOCK 0x0213  /* Waiter All Terminal Unlock R3.1 */
#define     CLI_FCBAKWAITER      0x0250  /* Waiter back up copy */

#define     CLI_FCGCF            0x0300  /* GCF commom */
#define     CLI_FCGCFMASSIGN     0x0301  /* GCF manual assign */
#define     CLI_FCGCFSTORE       0x0302  /* GCF Store GCF */
#define     CLI_FCGCFREADLOCK    0x0303  /* GCF Read and Lock */
#define     CLI_FCGCFLOCK        0x0313  /* GCF Lock */
#define     CLI_FCGCFDELETE      0x0305  /* GCF Delete */
#define     CLI_FCGCFAGENGCN     0x0306  /* GCF auto generation */
#define     CLI_FCGCFABAKGCN     0x0316  /* GCF auto generation backup */
#define     CLI_FCGCFINDREAD     0x0307  /* GCF individual read */
#define     CLI_FCGCFREADAGCNO   0x0308  /* GCF read all GCN */
#define     CLI_FCGCFREADADRNO   0x0318  /* GCF read all DRV # */
#define     CLI_FCGCFALLLOCK     0x0309  /* GCF All Lock */
#define     CLI_FCGCFALLUNLOCK   0x030a  /* GCF All Un Lock */
#define     CLI_FCGCFASSIGN      0x030b  /* GCF assign */
#define     CLI_FCGCFRESETFG     0x030c  /* GCF reset read flag */
#define     CLI_FCGCFCHKEXIST    0x030d  /* GCF Check exist */
#define     CLI_FCGCFALLIDBW     0x030e  /* GCF All ID Read By Waiter */
#define     CLI_FCGCFDELDLIVQUE  0x030f  /* GCF Delete delivery queue add R3.0 */
#define     CLI_FCGCFINFOQUE     0x0310  /* GCF inform queue add R3.0 */
#define     CLI_FCGCFINDREADRAM  0x0311  /* GCF individual read ram add R3.0 */
#define     CLI_FCGCFSEARCHQUE   0x0312  /* GCF search queue add R3.0*/
#define     CLI_FCGCFCHECKREAD   0x0314  /* GCF check and read add R3.0*/
#define     CLI_FCGCFRESETDVQUE  0x0315  /* GCF reset delivery queue R3.0*/
#define     CLI_FCGCFDVINDREAD   0x0317  /* GCF Ind. Read for delivery queue R3.0*/
#define     CLI_FCGCFDVINDREADBAK 0x0319  /* GCF Ind. Read for delivery queue backup R3.0*/

#define     CLI_FCTTL            0x0400  /* TTL commom */
#define     CLI_FCTTLUPDATE      0x0401  /* TTL update */
#define     CLI_FCTTLREADTL      0x0402  /* TTL read */
#define     CLI_FCRESETTRANSNO   0x0403  /* reset transaction No. */
#define     CLI_FCTTLRESETTL     0x0411  /* TTL total reset EOD or PTD */
#define     CLI_FCTTLRESETCAS    0x0412  /* TTL total reset CAS */
#define     CLI_FCTTLRESETWAI    0x0413  /* TTL total reset WAI */
#define     CLI_FCTTLISSUE       0x0421  /* TTL total issue out EOD or PTD */
#define     CLI_FCTTLISSUECHK    0x0431  /* TTL total issue check */
#define     CLI_FCTTLISSUERST    0x0441  /* TTL total issue reset */
#define     CLI_FCTTLWAITTLCHK   0x0442  /* TTL waiter total check */
#define     CLI_FCTTLCASTTLCHK   0x0443  /* TTL cashier total check */
#define     CLI_FCTTLREADTLPLU   0x0444  /* TTL plural PLU read,    R3.0 */
/* ===== New Functions (Release 3.1) BEGIN ===== */
#define     CLI_FCTTLRESETINDFIN 0x0446  /* TTL individual fin. reset, R3.1 */
/* ===== New Functions (Release 3.1) END ===== */
#define     CLI_FCTTLREADTLPLUEX 0x0447  /* TTL plural more PLU read, saratoga */

#define		CLI_FCTTLDELAYBALUPD 0x0463

#define     CLI_FCOP             0x0500  /* OP commom */
#define     CLI_FCOPPLUINDREAD   0x0501  /* OP PLU  individual read */
#define     CLI_FCOPDEPTINDREAD  0x0502  /* OP DEPT individual read */
#define     CLI_FCOPPLUASSIGN    0x0503  /* OP PLU  assign          */
#define     CLI_FCOPDEPTASSIGN   0x0504  /* OP DEPT assign          */
#define     CLI_FCOPPLUDELETE    0x0505  /* OP PLU  delete          */
#define     CLI_FCOPPLUFILEUPD   0x0506  /* OP PLU  file update sub index  */
#define     CLI_FCOPPLUALLREAD   0x0507  /* OP PLU  all  read              */
#define     CLI_FCOPPLUREADBRC   0x0508  /* OP PLU  read by report code    */
#define     CLI_FCOPPLUREADBDP   0x0509  /* OP PLU  read by dept number    */
#define     CLI_FCOPLOCK         0x050a  /* OP lock  */
#define     CLI_FCOPUNLOCK       0x050b  /* OP unlock  */
#define     CLI_FCOPDEPTREADBMD  0x050c  /* OP read dept by majort dept #  */

#define     CLI_FCPLUENTERCRITMD 0x050d  /* PLU enter crite mode,   Saratoga */
#define     CLI_FCPLUEXITCRITMD  0x050e  /* PLU exit crite mode,    Saratoga */
#define     CLI_FCPLUREPORTREC   0x050f  /* PLU report record,      Saratoga */

#define     CLI_FCOPREQALL       0x0510  /* OP request ALL parameter */
#define     CLI_FCOPREQALLFIN    0x0520  /* OP request ALL finalize */
#define     CLI_FCOPREQSUP       0x0511  /* OP request SUP parameter */
#define     CLI_FCOPREQSUPFIN    0x0521  /* OP request SUP finalize */
#define     CLI_FCOPREQPLU       0x0512  /* OP request PLU parameter */
#define     CLI_FCOPREQPLUFIN    0x0522  /* OP request PLU finalize */

#define     CLI_FCOPREQDISPMSG   0x0514  /* OP Display message to operator with parameters */

#define     CLI_FCPARA           0x0530  /* PARA common */
#define     CLI_FCPARAALLREAD    0x0531  /* PARA all read  rameter */
#define     CLI_FCPARAALLWRITE   0x0532  /* PARA all write rameter */

#define     CLI_FCOPMLDASSIGN    0x0533  /* OP Mld Mnemonics assign, V3.3  */
#define     CLI_FCOPMLDINDREAD   0x0534  /* OP Mld Mnemonics read, V3.3  */

#define     CLI_FCOPCPNASSIGN    0x0541  /* OP Coupon assign  */
#define     CLI_FCOPCPNINDREAD   0x0542  /* OP Coupon individual read  */
#define     CLI_FCOPCSTRASSIGN   0x0543  /* OP Control String assign  */
#define     CLI_FCOPCSTRDELETE   0x0544  /* OP Control String delete  */
#define     CLI_FCOPCSTRINDREAD  0x0545  /* OP Control String individual read  */
#define     CLI_FCOPOEPPLUREAD   0x0546  /* OP Oep PLU read  */
/* ===== New Functions (Release 3.1) BEGIN ===== */
#define     CLI_FCOPPPIASSIGN    0x0547  /* OP PPI assign  */
#define     CLI_FCOPPPIDELETE    0x0548  /* OP PPI delete  */
#define     CLI_FCOPPPIINDREAD   0x0549  /* OP PPI individual read  */
/* ===== New Functions (Release 3.1) END ===== */

/* #define     CLI_FCBAKOPPARA      0x0550  / reserved for BAK operational parameter */
#define     CLI_FCOPDEPTDELETE   0x0560  /* OP DEPT delete, saratoga  */

#define     CLI_FCKPS            0x0600  /* KPS commom */
#define     CLI_FCKPSPRINT       0x0601  /* KPS print data */
#define     CLI_FCKPSALTMANKP    0x0602  /* KPS alternate manual KP */

#define     CLI_FCDFL            0x0600  /* DFL commom */
#define     CLI_FCDFLPRINT       0x0621  /* DFL Display on flay     */

#define     CLI_FCISP            0x0700  /* ISP commom */
#define     CLI_FCISPLOGON       0x0711  /* ISP Log on */
#define     CLI_FCISPCHANGEPASS  0x0713  /* ISP Change password */
#define     CLI_FCISPLOCKKEYBD   0x0721  /* ISP Lock key board */
#define     CLI_FCISPUNLOCKKEYBD 0x0722  /* ISP Unlock key board */
#define     CLI_FCISPCHKTOTALS   0x0731  /* ISP check totals      */
#define     CLI_FCISPCHKTERM     0x0732  /* ISP check terminal cond. */
#define     CLI_FCISPLOGOFF      0x0741  /* ISP Log off */
#define     CLI_FCISPBROADCAST   0x0742  /* ISP Broadcast */
#define     CLI_FCISPSATELLITE   0x0743  /* ISP read no. of satellite */
#define		CLI_FCISPCHKMODE	 0X0744	 /* ISP check mode on terminal */

/* V3.3 */
#define     CLI_FCISPTODREAD     0x0760  /* ISP Date/Time Read */
#define     CLI_FCISPTODWRITE    0x0761  /* ISP Date/Time Write */
#define     CLI_FCISPSYSCONF     0x0762  /* ISP System Parameter Read */
#define     CLI_FCISPRESETLOG    0x0770  /* ISP Print Reset Log */
#define     CLI_FCISPRESETLOG2   0x0771  /* ISP Print Reset Log by Mnemonic */

/*--------------------------------------------------------------------------*
    Reserved the function codes from 0x0800 to 0x08ff to Remote File System
        -> PLU Parameter (MMM) from GP, Saratoga
    Reserved the function codes from 0x0900 to 0x09ff to MMM    File System
----------------------------------------------------------------------------*/

#define     CLI_FCSPS            0x0900  /* SPS commom */
#define     CLI_FCSPSPRINT       0x0901  /* SPS print  */
#define     CLI_FCSPSTERMLOCK    0x0902  /* SPS Lock   */
#define     CLI_FCSPSTERMUNLOCK  0x0903  /* SPS Unlock */
#define     CLI_FCSPSPOLLING     0x0904  /* SPS Polling Add R3.0 */
#define     CLI_FCSPSPRINTKP     0x0905  /* SPS Print for KP Add R3.0 */

/**********************************************************************
    Move EJ Function to 0x09xx => 0x0cxx, (11/21/95)
    because it duplicates sharead printer function
#define     CLI_FCEJ             0x0900  
#define     CLI_FCEJCLEAR        0x0941  
#define     CLI_FCEJREADFILE     0x0942  
#define     CLI_FCEJREAD         0x0943  
**********************************************************************/

#define     CLI_FCETK            0x0a00  /* ETK commom */
#define     CLI_FCETKTIMEIN      0x0a01  /* ETK time in */
#define     CLI_FCETKTIMEOUT     0x0a02  /* ETK time out */
#define     CLI_FCETKASSIGN      0x0a04  /* ETK assign   */
#define     CLI_FCETKINDJOBREAD  0x0a05  /* ETK ind job read  */
#define     CLI_FCETKDELETE      0x0a06  /* ETK delete        */
#define     CLI_FCETKCURALLIDRD  0x0a07  /* ETK all cur id read   */
#define     CLI_FCETKSAVALLIDRD  0x0a08  /* ETK all sav id read   */
#define     CLI_FCETKCURINDREAD  0x0a09  /* ETK cur ind id read   */
#define     CLI_FCETKSAVINDREAD  0x0a0a  /* ETK sav ind id read   */
#define     CLI_FCETKALLRESET    0x0a0b  /* ETK all reset         */
#define     CLI_FCETKALLLOCK     0x0a0c  /* ETK all lock          */
#define     CLI_FCETKALLUNLOCK   0x0a0d  /* ETK all unlock        */
#define     CLI_FCETKISSUEDCHK   0x0a0e  /* ETK issued check      */
#define     CLI_FCETKISSUEDSET   0x0a0f  /* ETK issued set        */
#define     CLI_FCETKSTATREAD    0x0a10  /* ETK stat read  */
#define     CLI_FCETKCURINDWRITE 0x0a11  /* ETK cur ind field write */
#define     CLI_FCETKCREATTLIDX  0x0a31  /* ETK create total index  (ISP unique)*/

#define     CLI_FCCPM            0x0b00  /* CPM commom */
#define     CLI_FCCPMSENDMESS    0x0b01  /* CPM Send request message */
#define     CLI_FCCPMRECVMESS    0x0b02  /* CPM Recv request message */
#define     CLI_FCCPMRDRSTTLY    0x0b05  /* CPM Read Reset Tally     */

/**********************************************************************
    Move EJ Function to 0x09xx => 0x0cxx, (11/21/95)
    because it duplicates sharead printer function
**********************************************************************/
#define     CLI_FCEJ             0x0c00  /* EJ  commom   */
#define     CLI_FCEJCLEAR        0x0c01  /* EJ  clear    */
#define     CLI_FCEJREADFILE     0x0c02  /* EJ read file */
#define     CLI_FCEJREAD         0x0c03  /* EJ read data */
/* V3.3 */
#define     CLI_FCEJREAD1LINE       0x0c04  /* EJ read 1 line */
#define     CLI_FCEJNOOFTRANS       0x0c05  /* EJ get # of transaction */
#define     CLI_FCEJREADRESET_OVER  0x0c06  /* EJ read/reset(override type) */
#define     CLI_FCEJREADRESET_START 0x0c07  /* EJ read/reset(not override type) */
#define     CLI_FCEJREADRESET       0x0c08  /* EJ read/reset(not override type) */
#define     CLI_FCEJREADRESET_END   0x0c09  /* EJ read/reset(not override type) */
#define     CLI_FCEJLOCK            0x0c0a  /* EJ lock */
#define     CLI_FCEJUNLOCK          0x0c0b  /* EJ unlock */

#define     CLI_FCPROGRPT        0x0d00  /* PROGRAM REPORT */
#define     CLI_FCDUMMY          0x0f00  /* DUMMY command */

#define     CLI_FCBAK            0x0050  /* BAK commom */
#define     CLI_FCBAKCASHIER     0x0150  /* BAK cashier parameter */
#define     CLI_FCBAKWAITER      0x0250  /* BAK waiter parameter */
#define     CLI_FCBAKGCF         0x0350  /* BAK GCF */
#define     CLI_FCBAKTOTAL       0x0450  /* BAK All Total file */
#define     CLI_FCBAKOPPARA      0x0550  /* BAK operational parameter */
#define     CLI_FCBAKPLU         0x0950  /* BAK plu file,   Saratoga */
#define     CLI_FCBAKETK         0x0a50  /* Bak ETK      */
/* ===== New Functions (Release 3.1) BEGIN ===== */
#define     CLI_FCBAKPROGRPT     0x0d50  /* Bak PROGRPT, R3.1 */
/* ===== New Functions (Release 3.1) END ===== */
#define     CLI_FCBAKFINAL       0x0f50  /* BAK finalize */

/*------------------------------------------------
    Terget Server Unique Address
-------------------------------------------------*/
#define     CLI_TGT_MASTER      1       /* Master terminal unique address */
#define     CLI_TGT_BMASTER     2       /* B. Master unique address */

/*------------------------------------------------
    Port Address definition
-------------------------------------------------*/

/* use port no. from 1 to 200 for PIFNET.DLL control, except prot no. 7, 2172 */

#define     CLI_PORT_STUB       10     /* Stub   port number  */
#define     CLI_PORT_SERVER     20     /* Sever  port number  */
#define     CLI_PORT_NB         30     /* Notice Board  port no. */
#define     CLI_PORT_ISPSERVER  40     /* ISP    Server port no. */
#define     CLI_PORT_CPM        50     /* Charge Posting Manager port no. */

/*------------------------------------------------
    Seqence No Control
-------------------------------------------------*/
#define     CLI_SEQ_CONT        0x0fff
#define     CLI_SEQ_SENDEND     0x8000  /* end of send data, STUB */
#define     CLI_SEQ_SENDDATA    0x4000  /* send data exist, STUB */
#define     CLI_SEQ_RECVEND     0x2000  /* end of send data, SERVER */
#define     CLI_SEQ_RECVDATA    0x1000  /* send data exist, SERVER */

/*------------------------------------------------
    MDC Definition
-------------------------------------------------*/
#define     CLI_MDC_SERVER      1       /* receive timer of SERVER */
#define     CLI_MDC_CLIENT      2       /* receive timer of STUB */
#define     CLI_MDC_RETRY       3       /* retry counter of STUB */
#define     CLI_MDC_WAIT        4       /* busy wait timer of STUB */
#define     CLI_MDC_MULTI       5       /* SERVER multi/ backup timer */
#define     CLI_MDC_SHARED     99       /* Shared printer monitor timer */
#define     CLI_MDC_CPMWAIT   198       /* CPM wait response from HOST */

#define     CLI_DEFMDC_SERVER   5       /* default values used for */
#define     CLI_DEFMDC_CLIENT   5       /* ovalue of each MDCs */
#define     CLI_DEFMDC_RETRY    3       /* Three times retry */
#define     CLI_DEFMDC_WAIT     2       /* 2 seconds wait */
#define     CLI_DEFMDC_CPMWAIT  3       /* 3 X 30 seconds wait */
/*--------------------------------------------------------
*  "CLI_DEFMDC_MULTI" changes from 20 to 30 seconds      *
*                                                        *
*   Modified at HPUS Enhanced Model                      *
*-------------------------------------------------------*/  
#define     CLI_DEFMDC_MULTI    30      /* 30 seconds timer  for 800 GCF creation */
#define     CLI_DEFMDC_SHARED   5       /* default values used for Shared */

/*------------------------------------------------
    IP Address, Data Definition 
-------------------------------------------------*/
#define     CLI_POS_UA          3       /* unique address auchFaddr[?] */
#define     CLI_POS_WBM         0       /* positon of back up system */

/*------------------------------------------------
    CREATE TEMP FILE (SerCreateTmpFile)
-------------------------------------------------*/
#define SERV_PERFORM                    0   /* create success */
#define SERV_ERROR                     -1   /* create fail */

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

/*--------------------------------------
    Common Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* Function Code */
    USHORT  usSeqNo;                    /* Sequence number */    
} CLIREQCOM;

/*--------------------------------------
    Common Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* Function code */
    USHORT  usSeqNo;                    /* Sequence number */
    SHORT   sResCode;                   /* Response code */
    SHORT   sReturn;                    /* Return Valu from each module */
} CLIRESCOM;

/*--------------------------------------
    Common Data Message 
---------------------------------------*/
typedef struct {
    USHORT  usDataLen;                  /* data length */
    UCHAR   auchData[1];                /* data */
} CLIREQDATA;

/*--------------------------------------
    Cashier Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  usCashierNo;                    /* cashier number */  
    WCHAR   auchCasName[CLI_CASHIERNAME];   /* cashier name */
    UCHAR   fbStatus[CLI_CASHIERSTATUS];    /* cashier status, V3.3 */
    UCHAR   uchSecretNo;                    /* secret code */
    USHORT  usStartGCN;                     /* start Guest Check Number, V3.3 */
    USHORT  usEndGCN;                       /* end Guest Check Number, V3.3 */
    UCHAR   uchChgTipRate;                  /* charge tips rate, V3.3 */
    UCHAR   uchTeamNo;                      /* team no, V3.3 */
    UCHAR   uchTerminal;                    /* terminal no, V3.3 */
    UCHAR   fbOption;                       /* sign-in option, V3.3 */
    USHORT  usTerminalNo;                   /* terminal# to lock/unlock R3.1 */
} CLIREQCASHIER;

/*--------------------------------------
    Cashier Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usCashierNo;                    /* cashier number */
    WCHAR   auchCasName[CLI_CASHIERNAME];   /* cashier name */
    UCHAR   fbStatus[CLI_CASHIERSTATUS];    /* cashier status, V3.3 */
    UCHAR   uchSecretNo;                    /* secret code */
    USHORT  usStartGCN;                     /* start Guest Check Number, V3.3 */
    USHORT  usEndGCN;                       /* end Guest Check Number, V3.3 */
    UCHAR   uchChgTipRate;                  /* charge tips rate, V3.3 */
    UCHAR   uchTeamNo;                      /* team no, V3.3 */
    UCHAR   uchTerminal;                    /* terminal no, V3.3 */
} CLIRESCASHIER;

/*--------------------------------------
    Waiter Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  usWaiterNo;                     /* waiter number */
    WCHAR   auchWaiName[CLI_WAITERNAME];    /* waiter name */
    UCHAR   fbStatus;                       /* waiter status */
    UCHAR   uchSecretNo;                    /* secet code */
    USHORT  usStartGCN;                     /* start Guest Check Number */
    USHORT  usEndGCN;                       /* end Guest Check Number */
    USHORT  usTerminalNo;                   /* terminal# to lock/unlock R3.1 */
} CLIREQWAITER;

/*--------------------------------------
    Waiter Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usWaiterNo;                     /* waiter number */
    WCHAR   auchWaiName[CLI_CASHIERNAME];   /* waiter name */
    UCHAR   fbStatus;                       /* waiter status */
    UCHAR   uchSecretNo;                    /* secet code */
    USHORT  usStartGCN;                     /* start Guest Check Number */
    USHORT  usEndGCN;                       /* end Guest Check Number */
} CLIRESWAITER;

/*--------------------------------------
    GCF Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  usGCN;                          /* Guest Check Number */
    USHORT  usGCType;                       /* Guest Check Type */
    USHORT  usQueue;                        /* Drive Through Queue Number Add R3.0 */
    USHORT  usForBack;                      /* Forward or Backward Type */
} CLIREQGCF;

/*--------------------------------------
    GCF Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usGCN;                          /* Guest Check Number */
    USHORT  usGCType;                       /* Guest Check Type */
} CLIRESGCF;

/*--------------------------------------
    Total (update) Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  usTransNo;                      /* Transaction Number */
} CLIREQTOTAL;

/*--------------------------------------
    Total (update) Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usTransNo;                      /* Transaction Number */
} CLIRESTOTAL;

/*--------------------------------------------
    Total (reset & issue) Message (Request)
---------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    UCHAR   uchMajorClass;                  /* major class */
    UCHAR   uchMinorClass;                  /* minor class */
    USHORT  usIDNO;                         /* cashier/ waiter ID Number */
    USHORT  usExeMDC;                       /* execute MDC, R3.1 */
    UCHAR   fIssued;                        /* Issued flag */
    USHORT  ausTranNo[CLI_ALLTRANSNO];      /* Transaction Number (each) */
    USHORT  ausPreTranNo[CLI_ALLTRANSNO];   /* Previous Transaction Number */
} CLIREQRESETTL;

/*--------------------------------------
    TTL Cashier Tender Amount,  Saratoga
---------------------------------------*/
typedef struct {
    USHORT      usFunCode;
    USHORT      usSeqNo;
    USHORT      usCashierNumber;    /* Cashier Number   */
} CLIREQCASTEND;

/*--------------------------------------
    TTL Cashier Tender Amount,  Saratoga
---------------------------------------*/
typedef struct {
    USHORT      usFunCode;
    USHORT      usSeqNo;
    SHORT       sResCode;
    SHORT       sReturn;
    USHORT      usCashierNumber;    /* Cashier Number */
    LONG        lCashTender;        /* Cash Tender, Saratoga */
    LONG        lCheckTender;       /* Check Tender */
    LONG        lChargeTender;      /* Charge Tender */
    LONG        lMiscTender[STD_TENDER_MAX-3];     /* Misc Tender 1 to 8 */
    LONG        lForeignTotal[8];   /* Foreign Total 1 to 8,    Saratoga */
} CLIRESCASTEND;

/*--------------------------------------
    OP Para Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    UCHAR   uchMajorClass;                 
    USHORT  usHndorOffset;             /* LockHandle or Start offset */
    USHORT  usRWLen;                   /* read/ write request length */ 
    SHORT   sNoofTerm;                 /* no of terminals */ 
} CLIREQOPPARA;

/*--------------------------------------
    OP Para Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    UCHAR   uchMajorClass;                 
    USHORT  usHndorOffset;             /* LockHandle or Start offset */
    USHORT  usRWLen;                   /* read/ write performed length */
    SHORT   sNoofTerm;                 /* no. of terminal processing */
} CLIRESOPPARA;

/*------------------------------------------
    OP Para Final Data Sturucture (Req/Res)
-------------------------------------------*/
typedef struct {
    USHORT      usSec;
    USHORT      usMin;
    USHORT      usHour;
    USHORT      usMDay;
    USHORT      usMonth;
    USHORT      usYear;
    USHORT      usWDay;
} CLIDATETIME;

typedef struct {                        /* broadcast final data structure */ 
    USHORT      usRegNo;
    CLIDATETIME DateTime;               
} CLIOPFINBLK;

/*--------------------------------------
    KPS (print) Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    /* ### ADD 2172 Rel1.0 (Saratoga) */
    USHORT  usOutPrinterInfo;           /* Alternation flag & out put printer no */
                                        /* 0x8000:Manual 0x4000:Autl | 0x0000:normal (see cskpin.h) */
} CLIREQKITPRINT;

/*--------------------------------------
    KPS (print) Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usKptStatus;                    /* Printer Status */
} CLIRESKITPRINT;

/*--------------------------------------
    KPS (alternate) Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
} CLIREQALTKP;

/*--------------------------------------
    KPS (alternate) Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
} CLIRESALTKP;

/*--------------------------------------
    Back Up (copy) Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
} CLIREQBACKUP;

typedef struct {                        /* Back Up Final data */
    USHORT  ausTransNo[CLI_ALLTRANSNO];
    USHORT  ausPreTranNo[CLI_ALLTRANSNO];   /* Previous Transaction Number */
    USHORT  fsBcasInf;
} CLIREQDATABAK;

/*--------------------------------------
    Back Up Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
} CLIRESBACKUP;

/*--------------------------------------
    Inquiry Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usYear;                         
    USHORT  usMonth;
    USHORT  usDay;
    USHORT  usHour;
    USHORT  usMin;
    USHORT  usSec;
} CLIINQDATE;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    USHORT  fsStatus;                       /* Inquiry Status */
    CLIINQDATE  Date;                       /* Inquiry Date */
    USHORT  ausTranNo[CLI_ALLTRANSNO];      /* Transaction Number (each) */
    USHORT  ausPreTranNo[CLI_ALLTRANSNO];   /* Previous Transaction Number */
} CLIREQINQUIRY;

/*--------------------------------------
    Inquiry Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  fsStatus;                       /* Inquiry Status */
    CLIINQDATE  Date;                       /* Inquiry Date */
    USHORT  ausTranNo[CLI_ALLTRANSNO];      /* Transaction Number (each) */
    USHORT  ausPreTranNo[CLI_ALLTRANSNO];   /* Previous Transaction Number */
} CLIRESINQUIRY;

/*--------------------------------------
    TTL Cashier/ Waiter Commom
---------------------------------------*/
typedef struct {
    UCHAR       uchMajorClass;      /* Major Class Data definition */
    UCHAR       uchMinorClass;      /* Minor Class Data definition */
    USHORT      usNumber;           /* Cashier Number              */
    UCHAR       uchAjectNo;         /* Ajective Number             */
    WCHAR       auchPluNo[CLI_PLU_LEN];    /* PLU No,   Saratoga   */
} CLITTLCASWAI;   

/*--------------------------------------
    PassWord Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    WCHAR   auchPassWord[CLI_PASSWORD];   
    SHORT   fsCheckTotal;
    UCHAR   uchNbFlag;   
} CLIREQISP;

/*--------------------------------------
    PassWord Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    UCHAR   uchNoOfSat;   
} CLIRESISP;

/*-------------------------------------
    ISP Date/Time Read/Write (Request)    V3.3
---------------------------------------*/
typedef struct {
    USHORT      usFunCode;
    USHORT      usSeqNo;
    CLIDATETIME DateTime;
} CLIREQISPTOD;

/*-------------------------------------
    ISP Date/Time Read/Write (Response)    V3.3
---------------------------------------*/
typedef struct {
    USHORT      usFunCode;
    USHORT      usSeqNo;
    SHORT       sResCode;
    SHORT       sReturn;
    CLIDATETIME DateTime;
} CLIRESISPTOD;

/*--------------------------------------
    Reset Log Message (Request),    V3.3
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    UCHAR   uchAction;
} CLIREQISPLOG;

typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    WCHAR   aszMnemonic[CLI_ISPLOG_LEN];
} CLIREQISPLOG2;

/*--------------------------------------
    ISP System Parameter Message (Response)    V3.3
---------------------------------------*/
/* length */
/*----------pif.h--------------------------*
#define PIF_LEN_PORT            7
#define PIF_LEN_OPTION          8
#define PIF_LEN_RMT_PORT        4
#define PIF_LEN_SOFTWARE_ID     17
*------------------------------------------*/
#define CLI_LEN_PORT            7
#define CLI_LEN_OPTION          8
#define CLI_LEN_RMT_PORT        4
#define CLI_LEN_SOFTWARE_ID     64  /* saratoga */
#define CLI_LEN_DEVICE_NAME     32  /* saratota */

typedef struct {
    UCHAR       uchTerminal;                        /* Terminal No. */
    WCHAR       auchComPort[CLI_LEN_PORT];          /* Device Type to connect the port */
    WCHAR       auchComDeviceName[CLI_LEN_PORT][CLI_LEN_DEVICE_NAME];  /* Device Type to connect the port, saratoga */
    USHORT      ausComBaud[CLI_LEN_PORT];           /* Baud Rate for each port */
    UCHAR       auchComByteFormat[CLI_LEN_PORT];    /* Byte Format for each port */
    USHORT      ausOption[CLI_LEN_OPTION];          /* Option Parameter for each port */
    UCHAR       auchRmtPort[2][CLI_LEN_RMT_PORT];   /* Device Type for Protocol Converter */
    UCHAR       uchLoadDevice;                      /* Loaded Device */
    USHORT      usLoadBaud;                         /* Baud Rate for Loaded Device */
    UCHAR       uchLoadCom;                         /* Format for Loaded Device */
    WCHAR       SystemId[CLI_LEN_SOFTWARE_ID];      /* System ID */
    WCHAR       ApplicationId[CLI_LEN_SOFTWARE_ID]; /* Application ID */
    UCHAR       uchROM1Size;                        /* Size of ROM1 */
    UCHAR       uchROM2Size;                        /* Size of ROM2 */
    UCHAR       uchSRAM1Size;                       /* Size of SRAM1 */
    UCHAR       uchSRAM2Size;                       /* Size of SRAM2 */
    UCHAR       uchSRAM3Size;                       /* Size of SRAM3 */
    UCHAR       uchSRAM4Size;                       /* Size of SRAM4 */
    UCHAR       uchSRAM5Size;                       /* Size of SRAM5 */
    UCHAR       uchCom;                             /* Connecting RS232 Board? */
    UCHAR       uchNet;                             /* Connecting IHC Board? */
    UCHAR       uchOperType;                        /* Type of Operator Display 1 */
    UCHAR       uchCustType;                        /* Type of Customer Display 1 */
    UCHAR       uchKeyType;                         /* Type of Key Board 1 */
    UCHAR       uchWaiterType;                      /* Type of Waiter(Server) Lock/Pen */
    UCHAR       uchIntegMSRType;                    /* Type of Integ. MSR1 */
    UCHAR       uchDrawer1;                         /* Type of Drawer 1 */
    UCHAR       uchDrawer2;                         /* Type of Drawer 2 */
    USHORT      usErrorStatusS;                     /* Error Status of Slip (V3.3) */
    ULONG       ulFreeBytesAvailableToCaller;       /* Saratoga */
    ULONG       ulTotalNumberOfBytes;               /* Saratoga */
    ULONG       ulTotalNumberOfFreeBytes;           /* Saratoga */
} CLISYSPARA;

typedef struct {
    USHORT      usFunCode;              /* Function code */
    USHORT      usSeqNo;                /* Sequence number */
    SHORT       sResCode;               /* Response code */
    SHORT       sReturn;                /* Return Valu from each module */
    CLISYSPARA  SysPara;                /* Data */
} CLIRESISPSYSPARA;


/*-------------------------------------------
    Adds enhanced version of HP US 

    Display on Fly
    Electric   Journal
    Shared     Print
    Employee   Time  Keeping
-------------------------------------------*/
/*--------------------------------------
    Display on Fly Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
} CLIREQDFL;

/*--------------------------------------
    Display on Fly Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
} CLIRESDFL;

/*--------------------------------------
    Shared Print Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
} CLIREQSHR;

/*--------------------------------------
    Shared Print Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
} CLIRESSHR;

/*--------------------------------------
    Electric Journal Message (Request)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    ULONG   ulOffset;
    UCHAR   auchEJRead[CLI_MAX_EJREAD ];    /* Add R3.1 (sizeof(EJ_READ)-2) */
    UCHAR   uchType;                        /* Add R3.1 */
    USHORT  usNumTrans;                     /* V3.3 */
} CLIREQEJ;

/*--------------------------------------
    Electric Journal Message (Response)
---------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    ULONG   ulOffset;   
    ULONG	usDataLen;
	//USHORT  usDataLen;   
    USHORT  usNumTrans;                     /* V3.3 */
    WCHAR   auchEJData[CLI_MAX_EJDATA];
} CLIRESEJ;

typedef struct {    /* Add at R3.1 */
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    ULONG   ulOffset;   
    USHORT  usDataLen;   
    WCHAR   auchEJData[CLI_MAX_EJREPTDATA];
    UCHAR   auchEJRead[CLI_MAX_EJREAD ];
} CLIRESEJREPT;     /* Add at R3.1 */

/*------------------------------------------
    Employee Time Keeping Message (Request)
------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    ULONG   ulEmployeeNo;   
    UCHAR   uchJobCode;
    USHORT  usFieldNo;
    WCHAR   auchNewJob[CLI_NEW_ETK_JOB];
    WCHAR   auchEmployeeName[CLI_EMPLOYEE_NAME];    /* Add R3.1 */
} CLIREQETK;

/*------------------------------------------
    Employee Time Keeping Message (Response)
------------------------------------------*/
typedef struct {
    USHORT  usFunCode;
    USHORT  usSeqNo;
    SHORT   sResCode;
    SHORT   sReturn;
    USHORT  usFieldNo;
    SHORT   sTMStatus;   
    USHORT  usDataLen;   
    WCHAR   auchETKData[CLI_MAX_ETKDATA];
    WCHAR   auchEmployeeName[CLI_EMPLOYEE_NAME];    /* Add R3.1 */
} CLIRESETK;


/*-------------------------------------------------------------
    PLU Enter Crit Mode Message (Request)           Saratoga
    CLI_FCPLUENTERCRITMD 0x0904  * PLU enter crit mode *
-------------------------------------------------------------*/

typedef struct {
    USHORT  usFunCode;                  /* Function code */
    USHORT  usSeqNo;                    /* Sequence number */ 
    USHORT  usFile;                     /* Designate a file */
    USHORT  usFunc;                     /* Designate a function */
    WCHAR   auchPara[sizeof(ECMCOND)];  /* Parameter */
} CLIREQPLUECM;

/*---------------------------------------------
    PLU Enter Crit Mode Message (Response)
---------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* Function code */
    USHORT  usSeqNo;                    /* Sequence number */
    SHORT   sResCode;                   /* Response code */
    SHORT   sReturn;                    /* Return vale from each module */
    USHORT  usHandle;                   /* Handle vale */
} CLIRESPLUECM;

/*--------------------------------------------
    Save PLU Crit Mode's Handle 
--------------------------------------------*/
typedef struct {
    USHORT  usMTHand;                   /* Master terminal handle   */
    USHORT  usBMHand;                   /* B-Master terminal handle */
    USHORT  usFile;                     /* File          */
    USHORT  fsOption;                   /* Option data   */
} CLIPLUHAN;

typedef struct {
    CLIPLUHAN  hFst;                     /* First  users handle */
    CLIPLUHAN  hSnd;                     /* Second users handle */
} CLIPLUSAVHAN;

typedef struct {
    USHORT  usFunCode;                  /* Function Code */
    USHORT  usSeqNo;                    /* Sequence number */ 
    USHORT  usFileorHnd;                /* Designate a file or Handle */
    USHORT  usReserve;                  /* Reserved vale */
} CLIREQPLUCOM;

/*--------------------------------------------
    PLU Common Message (Response)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* Function code */
    USHORT  usSeqNo;                    /* Sequence number */
    SHORT   sResCode;                   /* Response code */
    SHORT   sReturn;                    /* Return vale from each module */
} CLIRESPLUCOM;


/*--------------------------------------------
    PLU Individual Read Message (Request)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* function code            */
    USHORT  usSeqNo;                    /* sequential number        */
    WCHAR   aucPluNumber[STD_PLU_NUMBER_LEN];           /* PLU number               */
    UCHAR   uchAdjective;               /* adjective number         */
    USHORT  usDeptNumber;               /* department number        */
    UCHAR   uchItemType;                /* PLU Type                 */
    ULONG   ulUnitPrice;                /* unit price               */
    UCHAR   uchPriceMulWeight;          /* price multiple / weight  */
    WCHAR   aucMnemonic[20];            /* mnemonics                */
    UCHAR   aucStatus[10];              /* status                   *//* ### MOD Saratoga FamilyCode (12 --> 10)(052900) */
    USHORT  usFamilyCode;               /* family code              *//* ### ADD Saratoga FamilyCode (052900) */
    UCHAR   uchMixMatchTare;            /* mix-match / tare code    */
    UCHAR   uchReportCode;              /* report code              */
    USHORT  usLinkNumber;               /* link number              */
    UCHAR   uchRestrict;                /* restriction code         */
    UCHAR   uchTableNumber;             /* Table No for Order Prompt*/
    UCHAR   uchGroupNumber;             /* Group No for Order Prompt*/
    UCHAR   uchPrintPriority;           /* Print Priority           */
    USHORT   uchLinkPPI;                 /* PPI                      */
} CLIREQSCNPLU;

/*--------------------------------------------
    PLU Individual Read Message (Response)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* function code            */
    USHORT  usSeqNo;                    /* sequential number        */
    SHORT   sResCode;                   /* response code            */
    SHORT   sReturn;                    /* return code              */
    WCHAR   aucPluNumber[STD_PLU_NUMBER_LEN];           /* PLU number               */
    UCHAR   uchAdjective;               /* adjective number         */
    USHORT  usDeptNumber;               /* department number        */
    UCHAR   uchItemType;                /* PLU Type                 */
    ULONG   ulUnitPrice;                /* unit price               */
    UCHAR   uchPriceMulWeight;          /* price multiple / weight  */
    WCHAR   aucMnemonic[20];            /* mnemonics                */
    UCHAR   aucStatus[10];              /* status                   *//* ### MOD Saratoga FamilyCode (12 --> 10)(052900) */
    USHORT  usFamilyCode;               /* family code              *//* ### ADD Saratoga FamilyCode (052900) */
    UCHAR   uchMixMatchTare;            /* mix-match / tare code    */
    UCHAR   uchReportCode;              /* report code              */
    USHORT  usLinkNumber;               /* link number              */
    UCHAR   uchRestrict;                /* restriction code         */
    UCHAR   uchTableNumber;             /* Table No for Order Prompt*/
    UCHAR   uchGroupNumber;             /* Group No for Order Prompt*/
    UCHAR   uchPrintPriority;           /* Print Priority           */
    USHORT   uchLinkPPI;                 /* PPI                      */
    UCHAR   uchBonusStatus;             /* bonus total index/status */
} CLIRESSCNPLU;

/*--------------------------------------------
    PLU Add Message (Resuest)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* function code            */
    USHORT  usSeqNo;                    /* sequential number        */
    WCHAR   aucPluNumber[STD_PLU_NUMBER_LEN];           /* PLU number               */
    UCHAR   uchAdjective;               /* adjective number         */
    USHORT  usDeptNumber;               /* department number        */
    UCHAR   uchItemType;                /* PLU Type                 */
    ULONG   ulUnitPrice;                /* unit price               */
    UCHAR   uchPriceMulWeight;          /* price multiple / weight  */
    WCHAR   aucMnemonic[20];            /* mnemonics                */
    UCHAR   aucStatus[10];              /* status                   *//* ### MOD Saratoga FamilyCode (12 --> 10)(052900) */
    USHORT  usFamilyCode;               /* family code              *//* ### ADD Saratoga FamilyCode (052900) */
    UCHAR   uchMixMatchTare;            /* mix-match / tare code    */
    UCHAR   uchReportCode;              /* report code              */
    USHORT  usLinkNumber;               /* link number              */
    UCHAR   uchRestrict;                /* restriction code         */
    UCHAR   uchTableNumber;             /* Table No for Order Prompt*/
    UCHAR   uchGroupNumber;             /* Group No for Order Prompt*/
    UCHAR   uchPrintPriority;           /* Print Priority           */
    USHORT   uchLinkPPI;                 /* PPI                      */
    UCHAR   uchBonusStatus;             /* bonus total index/status */
    USHORT  usMntBatch;                 /* mainte. batch number     */
    USHORT  usMntType;                  /* mainte. function type    */
    ULONG   flMntChanges;               /* mainte. change fields    */
    USHORT  usMntStatus;                /* mainte. applied status   */
	USHORT  usBonusIndex;               // index to the bonus total for this PLU
	USHORT  usTareInformation;
	UCHAR   uchExtendedGroupPriority[12];      // extended group numbers, first byte is group number, second byte is priority within group
	UCHAR   uchPluOptionFlags[6];                 // bit map for option flags
} CLIREQMNTPLU;

/*--------------------------------------------
    PLU Add Message (Response)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* function code            */
    USHORT  usSeqNo;                    /* sequential number        */
    SHORT   sResCode;                   /* response code            */
    SHORT   sReturn;                    /* return code              */
    WCHAR   aucPluNumber[STD_PLU_NUMBER_LEN];           /* PLU number               */
    UCHAR   uchAdjective;               /* adjective number         */
    USHORT  usDeptNumber;               /* department number        */
    UCHAR   uchItemType;                /* PLU Type                 */
    ULONG   ulUnitPrice;                /* unit price               */
    UCHAR   uchPriceMulWeight;          /* price multiple / weight  */
    WCHAR   aucMnemonic[20];            /* mnemonics                */
    UCHAR   aucStatus[10];              /* status                   *//* ### MOD Saratoga FamilyCode (12 --> 10)(052900) */
    USHORT  usFamilyCode;               /* family code              *//* ### ADD Saratoga FamilyCode (052900) */
    UCHAR   uchMixMatchTare;            /* mix-match / tare code    */
    UCHAR   uchReportCode;              /* report code              */
    USHORT  usLinkNumber;               /* link number              */
    UCHAR   uchRestrict;                /* restriction code         */
    UCHAR   uchTableNumber;             /* Table No for Order Prompt*/
    UCHAR   uchGroupNumber;             /* Group No for Order Prompt*/
    UCHAR   uchPrintPriority;           /* Print Priority           */
    USHORT   uchLinkPPI;                 /* PPI                      */
    UCHAR   uchBonusStatus;             /* bonus total index/status */
    USHORT  usMntBatch;                 /* mainte. batch number     */
    USHORT  usMntType;                  /* mainte. function type    */
    ULONG   flMntChanges;               /* mainte. change fields    */
    USHORT  usMntStatus;                /* mainte. applied status   */
	UCHAR	uchColorPaletteCode;		/* Color Palette Code for PLU */
	USHORT  usBonusIndex;               // index to the bonus total for this PLU
	USHORT  usTareInformation;
	UCHAR   uchExtendedGroupPriority[12];      // extended group numbers, first byte is group number, second byte is priority within group
	UCHAR   uchPluOptionFlags[6];                 // bit map for option flags
} CLIRESMNTPLU;

/*--------------------------------------------
    PLU Report Message (Request)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* Function Code */
    USHORT  usSeqNo;                    /* Sequence number */ 
    USHORT  usFileorHnd;                /* Designate a file or Handle */
    ITEMNO  ItemNO;                     /* Item No */
    USHORT  usAdj;                      /* Adjective No */
    USHORT  fsProg;                     /* Programmability */
    UCHAR   fspvBuf;                    /* Return buffer pointer */
} CLIREQPLURPT;

/*--------------------------------------------
    PLU Report Message (Response)
--------------------------------------------*/
typedef struct {
    USHORT  usFunCode;                  /* Function code */
    USHORT  usSeqNo;                    /* Sequence number */
    SHORT   sResCode;                   /* Response code */
    SHORT   sReturn;                    /* Return vale from each module */
    USHORT  usRetLen;                   /* Return length */
    UCHAR   auchData[sizeof(MNTPLU)];   /* Old data or record,  Saratoga */
} CLIRESPLURPT;


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

/*------------------------------
    STUB I/F FUNCTION
-------------------------------*/

SHORT   SerCreateTmpFile(USHORT usSize);
VOID    SerDeleteTmpFile(VOID);

/*
===========================================================================
    MEMORY
===========================================================================
*/

extern  SHORT   hsSerTmpFile;
extern  CLIPLUSAVHAN    CliMMMHand;         /* Save MMM handel */
extern  CLIPLUSAVHAN    SerMMMHand;         /* Save handles,        Saratoga */

#endif      /* CSSTBFCC_INCLUDED */

/*===== END OF DEFINITION =====*/
#endif
