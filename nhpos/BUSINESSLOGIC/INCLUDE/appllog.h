/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1995       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : APPLICATION LOG                        
* Category    : APPLICATION, NCR 2170 US Hospitality Application Program        
* Program Name: APPLLOG.H
* --------------------------------------------------------------------------
* Abstract: This header defines Module IDs, Error Codes, Event Codes,
*           and Fault Cause Code. 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name     : Description
* Oct-21-92: 00.00.01 : H.Yamaguchi: initial                                   
* May-18-93:          : M.Yamamoto : Add LOG_ERROR_KPS_BUFFER_CLEAR
* Jun-21-93:  1.00.12 : K.Nakajima : Add MODULE_SHR
*          :          :            : Add MODULE_DFL                                   
* Oct-28-93:  2.00.02 : Yoshiko.Jim: Add MODULE_CPM for HOTEL MODEL
* Apr-22-94:  0.00.05 : K.You      : Add LOG_EVENT_CP_CA_SND, LOG_EVENT_CP_CA_RCV
* Feb-27-95: 03.00.00 : H.Ishida   : Add LOG_EVENT_OP_CPN_DELETE_FILE,MODULE_OP_CPN
* Mar- 3-95: 03.00.00 : hkato      : R3.0
* Mar-31-95: 03.00.00 : H.Ishida   : Add LOG_EVENT_OP_CSTR_DELETE_FILE,MODULE_OP_CSTR
* Nov-29-95: 03.01.00 : T.Nakahata : R3.1 Initial
*   Add Total(Individual Financial, Service Time), Opepara(PPI),
*   Programmable Report
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
*  Application Module IDs
*
*  System level and platform level module IDs are contained in file log.h
*  and have the range of 100 through 299.
*===========================================================================
*/

#define MODULE_UI                   300     /* UI Main Module */
#define MODULE_OPEOPEN              310     /* operator open module's module ID */
#define MODULE_TRANSOPEN            311     /* transaction open module ID */
#define MODULE_SALES                312     /* sales module's module ID */
#define MODULE_DISCOUNT             313     /* discount module ID */
#define MODULE_TOTAL                314     /* total key module ID */
#define MODULE_TENDER               315     /* tender module ID */
#define MODULE_MISC                 316     /* misc. transaction module ID */
#define MODULE_MODIFIER             317     /* modifier module ID */
#define MODULE_OTHER                318     /* other module ID */
#define MODULE_OPECLOSE             319     /* operator close module ID */
#define MODULE_MODEIN               320     /* mode in module's module ID */
#define MODULE_COMMON               321     /* common module ID */
#define MODULE_TRANS_BODY           325     /* transaction module ID */
#define MODULE_TRANS_QUAL           326     /* transaction module ID */
#define MODULE_TRANS_ITEM           327     /* transaction module ID */
#define MODULE_TRANS_STOR           328     /* transaction module ID */
#define MODULE_PRINT_TRANS          330     /* Print transaction */
#define MODULE_PRINT_ITEM           331     /* Print Item */
#define MODULE_PRINT_SUPPRG_ID      339     /* super/program */
#define MODULE_MAINT_LOG_ID         340     /* super/program */
#define MODULE_RPT_ID               350     /* report        */
#define MODULE_STB_LOG              360     /* client stub   */
#define MODULE_STB_ABORT            361     /* client stub   */
#define MODULE_CASHIER              370     /* cashier module */
#define MODULE_WAITER               380     /* waiter  module */ 
#define MODULE_TTL_TFL              390     /* total          */
#define MODULE_TTL_TFL_FINANCIAL    391     /* total          */
#define MODULE_TTL_TFL_HOURLY       392     /* total          */
#define MODULE_TTL_TFL_DEPT         393     /* total          */
#define MODULE_TTL_TFL_PLU          394     /* total          */
#define MODULE_TTL_TFL_CASHIER      395     /* total          */
#define MODULE_TTL_TFL_WAITER       396     /* total          */
#define MODULE_TTL_TFL_UPDATE       397     /* total          */
#define MODULE_TTL_TFL_BACKUP       398     /* total          */
#define MODULE_TTL_TFL_NUM          399     /* total          */
#define MODULE_KPS                  400     /* kitchen printer */
#define MODULE_KP                   400     /* kitchen printer */
#define MODULE_TTL_TFL_CPN          401     /* total, R3.0     */
/* ===== Add Service Time / Individual Financial (Release 3.1) BEGIN ===== */
#define MODULE_TTL_TFL_INDFIN       402     /* total, R3.1     */
#define MODULE_TTL_TFL_SERTIME      403     /* total, R3.1     */
/* ===== Add Service Time / Individual Financial (Release 3.1) END ===== */
#define MODULE_GCF                  410     /* gcf module      */
#define MODULE_ISP_LOG              420     /* isp write Log */
#define MODULE_ISP_ABORT            421     /* isp Abort Process */
#define MODULE_SER_LOG              430     /* server log */
#define MODULE_SER_ABORT            431     /* server abort */
#define MODULE_DISP_ID              440     /* display common */
#define MODULE_DISPREG_ID           441     /* display reg */
#define MODULE_DISPSUP_ID           442     /* display super/program */
#define MODULE_OP_PLU               450     /* opepara    */
#define MODULE_OP_DEPT              451     /* opepara    */
#define MODULE_OP_BACKUP            452     /* opepara    */
#define MODULE_OP_CPN               453     /* opepara    */
#define MODULE_OP_CSTR              454     /* opepara    */
/* ===== Add Promotional Pricing Item (Release 3.1) BEGIN ===== */
#define MODULE_OP_PPI               455     /* opepara - PPI,   R3.1    */
/* ===== Add Promotional Pricing Item (Release 3.1) END ===== */
#define MODULE_OP_MLD               456     /* opepara - MLD,   V3.3    */
/* Add Rel 1.0 */
#define MODULE_OP_OEP               457     /* opepara - OEP    */
#define MODULE_UIC_LOG              460     /* uic Write Log */
#define MODULE_UIC_ABORT            461     /* uic Abort Process */
#define MODULE_NB_LOG               470     /* nb Write Log */
#define MODULE_NB_ABORT             471     /* nb Abort Process */
#define MODULE_ETK                  475     /* ETK MODULE     */
#define MODULE_TTL_TUM              480     /* total          */
#define MODULE_TTL_TUM_FINANCIAL    481     /* total          */
#define MODULE_TTL_TUM_HOURLY       482     /* total          */
#define MODULE_TTL_TUM_DEPT         483     /* total          */
#define MODULE_TTL_TUM_PLU          484     /* total          */
#define MODULE_TTL_TUM_CASHIER      485     /* total          */
#define MODULE_TTL_TUM_WAITER       486     /* total          */
#define MODULE_TTL_TUM_UPDATE       487     /* total          */
#define MODULE_TTL_TUM_NUM          489     /* total          */
#define MODULE_PARA_ID              490     /* para           */
#define MODULE_TTL_TUM_CPN          491     /* total, R3.0    */
/* ===== Add Service Time / Individual Financial (Release 3.1) BEGIN ===== */
#define MODULE_TTL_TUM_INDFIN       492     /* total, R3.1     */
#define MODULE_TTL_TUM_SERTIME      493     /* total, R3.1     */
/* ===== Add Service Time / Individual Financial (Release 3.1) END ===== */
#define MODULE_EJ                   495     /* Electronic Journal */
#define MODULE_SHR                  496     /* Shared Printer */
#define MODULE_DFL                  497     /* Display on the fly */
#define MODULE_CPM                  498     /* Charge Posting Manager   */
/* ===== Add Programmable Report Module (Release 3.1) BEGIN ===== */
#define MODULE_PROGRPT              499     /* Programmable Report, R3.1 */
/* ===== Add Programmable Report Module (Release 3.1) END ===== */
#define MODULE_DIF                  500     /* Beverage Dispenser I/F, R3.1 */
#define MODULE_EEPT                 501     /* EEPT Manager   */
#define	MODULE_TTL_TFL_MAJOR		502		/* For TTL Major Class Error Reporting*/
#define MODULE_DEPT					503		/* For Dept Error Messages */
#define MODULE_PLU					504		/* For PLU Error Message */
#define MODULE_PIF					505		/* For Pif Error Messages */
#define MODULE_COMPRESS_DATA		506		/* For Data compression error messages JHHJ */
#define MODULE_FRAMEWORK     		507		/* For PifLogs generated by Framework */
#define MODULE_HTTPSERVER           508     /* For PifLogs generated by httpserver.dll */
#define MODULE_EPTINTERFACE         509     /* For PifLogs generated by EEptInterfaceFiles such as DSI Client */
  // The following define should only be used for temporary debugging.
  // It may be used in a release designer build but it should not be part of
  // a build that is sent to customers.
#define MODULE_DEBUG_TEMP           520     // For PifLogs generated as part of debugging.

// Special macros that are used with the above defines to
// indicate that a piflog is something other than a standard Module= Code= pair
#define MODULE_MODULE_NO(x)    ((x) % 1000)
#define MODULE_LINE_NO(x)      ((x)+5000)
#define MODULE_IS_LINE_NO(x)   (((x)/1000)==5)
#define MODULE_ERROR_NO(x)     ((x)+6000)
#define MODULE_IS_ERROR_NO(x)  (((x)/1000)==6)
#define MODULE_DATA_VALUE(x)     ((x)+7000)
#define MODULE_IS_DATA_VALUE(x)  (((x)/1000)==7)
#define MODULE_MOD_PIFABORT(x)      ((x)+8000)
#define MODULE_IS_MOD_PIFABORT(x)   (((x)/1000)==8)

/*
*===========================================================================
*  Application Error Codes
*===========================================================================
*/
/* LOG_ERROR_UI_   from 3000 to 3049 */

/* LOG_ERROR_ITM_  from 3050 to 3099 */
#define LOG_ERROR_ITM_INVALID_CLASS         3050     /* invalid Class received */
#define LOG_ERROR_ITM_INVALID_DATA          3051     /* invalid data received */
#define LOG_ERROR_ITM_INVALID_RESP          3052     /* invalid response received */
#define LOG_ERROR_ITM_INVALID_ARG           3053     /* invalid argument received */
#define LOG_ERROR_ITM_INVALID_CONDITION     3054     /* invalid condition */

/* LOG_ERROR_TRN_    from 3100 to 3149 */
#define LOG_ERROR_TRN_STO_BUFF_SIZE01		3100     //Found in TrnStoOpeCloseComp JHHJ 8-23-05
#define LOG_ERROR_TRN_STO_BUFF_SIZE02		3101	 //Found in TrnStoSalesComp    JHHJ 8-23-05

/* LOG_ERROR_PRINT_  from 3150 to 3199 */
#define LOG_ERROR_PRT_SUPPRG_CODE_01        3190
#define LOG_ERROR_PRT_SUPPRG_CODE_02        3191
#define LOG_ERROR_PRT_SUPPRG_CODE_03        3192
#define LOG_ERROR_PRT_SUPPRG_CODE_04        3193
#define LOG_ERROR_PRT_SUPPRG_CODE_05        3194
#define LOG_ERROR_PRT_SUPPRG_CODE_06        3195
#define LOG_ERROR_PRT_SUPPRG_ERROR          3199

/* LOG_ERROR_MAINT_  from 3200 to 3149 */
#define LOG_ERROR_MAT_NOTCREATE_FILE        3200  
#define LOG_ERROR_MAT_SUP_ERROR             3201  
#define LOG_ERROR_MAT_NOTREADDEPT           3202  
#define LOG_ERROR_MAT_NOTASSIGNDEPT         3203
#define LOG_ERROR_MAT_NOTDELPLU             3204
#define LOG_ERROR_MAT_NOTREADPLU            3205  
#define LOG_ERROR_MAT_CODE_01				3206 
#define LOG_ERROR_MAT_CODE_02				3207 
#define LOG_ERROR_MAT_CODE_03				3208
#define LOG_ERROR_MAT_CODE_04				3209
#define LOG_ERROR_MAT_CODE_05				3210
#define LOG_ERROR_MAT_BAD_JOINSTATUS		3211    // MaintUnjoinCluster() CheckUnjoinStatus() returned error.
#define LOG_ERROR_MAT_FAILED_JOIN			3212    // MaintUnjoinCluster() CliOpJoinCluster() returned error.
#define LOG_ERROR_MAT_FAILED_UNJOIN			3213    // MaintUnjoinCluster() CliOpUnjoinCluster() returned error.

/* LOG_ERROR_REPORT_  from 3250 to 3299 */
#define LOG_ERROR_REPORT_READ_TTL			3250

/* LOG_ERROR_STUB_    from 3300 to 3399 */

#define LOG_ERROR_STB_BAD_ENVIROMENT        3300    
#define LOG_ERROR_STB_3TIME_ERROR           3301
#define LOG_ERROR_STB_NETWORK_ERROR         3302   // CstComSendReceive() Network error detected. next log message contains error code
#define LOG_ERROR_STB_NETWORK_TIMEOUT       3303
#define LOG_ERROR_STB_COM_DEFAULT           3304
#define LOG_ERROR_STB_COM_BM_DWN_01         3305
#define LOG_ERROR_STB_COM_BM_DWN_02         3306
#define LOG_ERROR_STB_COM_BM_DWN_03         3307
#define LOG_ERROR_STB_COM_BM_DWN_04         3308
#define LOG_ERROR_STB_COM_BM_DWN_05         3309
#define LOG_ERROR_STB_COM_MA_DWN_01         3310
#define LOG_ERROR_STB_COM_MA_DWN_02         3311
#define LOG_ERROR_STB_COM_MA_DWN_03         3312
#define LOG_ERROR_STB_COM_MA_DWN_04         3313
#define LOG_ERROR_STB_COM_MA_DWN_05         3314
#define LOG_ERROR_STB_NETWORK_ERROR_FH      3315   // CstComSendReceiveFH() Network error detected. next log message contains error code
#define LOG_ERROR_STB_JOIN_FAILED           3316   // CliOpJoinCluster() Join broadcast failed.  next log message is CliMsg.sRetCode
#define LOG_ERROR_STB_PLU_TTLCURDAY_FAILED  3317   // CliOpTransferTotals() TtlExportPLU (CLASS_TTLCURDAY, _T("TOTALPLD")) failed.
#define LOG_ERROR_STB_PLU_TTLCURPTD_FAILED  3318   // CliOpTransferTotals() TtlExportPLU (CLASS_TTLCURPTD, _T("TOTALPLP")) failed.

/* LOG_ERROR_CASHIER_  from 3400 to 3449 */
#define LOG_ERROR_CASHIER_READ_CURDAY			3400
#define LOG_ERROR_CASHIER_READ_CURPTD			3401
#define LOG_ERROR_CASHIER_READ_SAVDAY			3402
#define LOG_ERROR_CASHIER_READ_SAVPTD			3403
#define LOG_ERROR_CASHIER_SIGN_OUT				3404
/* LOG_ERROR_WAITER_   from 3450 to 3499 */

/* LOG_ERROR_TOTAL_  from 3500 to 3549 */
#define LOG_ERROR_TTL_FILE_HANDLE           3500    /* File Handle Error */
#define LOG_ERROR_TTL_BACKUP_FILE           3501    /* 3501 - 3505 file back up error */
#define LOG_ERROR_TTL_BACKUP_LOWSTORAGE     3502    /*             low storage */
#define LOG_ERROR_TTL_BACKUP_NOSTORAGE      3503    /*             not enough storage */
#define LOG_ERROR_TTL_BACKUP_NOMEMORY       3504    /*             not enough memory */
#define LOG_ERROR_TTL_OVERMAX               3510    /* Max Number Over */
#define LOG_ERROR_TTL_DEPT                  3511    /* Dept Number Log Base */

#define	LOG_ERROR_TTL_SDCUR					3512		/* Create Status record error */
#define	LOG_ERROR_TTL_IDCUR					3513		/* Insert plu total record */
#define	LOG_ERROR_TTL_FDCUR					3514		/* Find plu total record */
#define	LOG_ERROR_TTL_UDCUR					3515		/* update plu total record */
#define	LOG_ERROR_TTL_TGDCUR				3516		/* Create Status record error */
#define	LOG_ERROR_TTL_TSDCUR				3517		/* Create Status record error */
#define	LOG_ERROR_TTL_SPCUR					3518		/* Create Status record error */
#define	LOG_ERROR_TTL_IPCUR					3519		/* Insert plu total record */
#define	LOG_ERROR_TTL_FPCUR					3520		/* Find plu total record */
#define	LOG_ERROR_TTL_UPCUR					3521		/* update plu total record */
#define	LOG_ERROR_TTL_TGCUR					3522		/* Create Status record error */
#define	LOG_ERROR_TTL_TSPCUR				3523		/* Create Status record error */

#define LOG_ERROR_TTL_AFFECT                3524    /* Affection Lost */
#define LOG_ERROR_TTL_NOTEMPTY              3525    /* Empty Table Full */
#define LOG_ERROR_TTL_FUNC_ILLIGAL          3526    /* Illigal Function(Major/Minor Class) Error */
#define LOG_ERROR_TTL_BACKUP_MASTER         3530    /* backup error from master */
#define LOG_ERROR_TTL_BASEINIT              3531     // error in TtlBaseInit()
#define LOG_ERROR_TTL_DEPTINIT              3532     // error in TtlDeptInit()
#define LOG_ERROR_TTL_CPNINIT               3533     // error in TtlCpnInit()
#define LOG_ERROR_TTL_SERINIT               3534     // error in TtlSerTimeInit()
#define LOG_ERROR_TTL_FININDEXWRITE         3537     // error in TtlIndFinIndexWrite()
#define LOG_ERROR_TTL_FINHEADWRITE          3538     // error in TtlIndFinHeadWrite()
#define LOG_ERROR_TTL_DATEWRITE             3539     // error in TtlDateWrite()
#define LOG_ERROR_TTL_UPDATEEMPTY           3540    /* Read When Update Total File Empty Error */
#define LOG_ERROR_TTL_UPDATEOVERRD          3541    /* Read Pointer or Write Pointer Error */

#define LOG_ERROR_TTL_CODE_01				3542
#define LOG_ERROR_TTL_CODE_02				3543
#define LOG_ERROR_TTL_CODE_03				3544
#define LOG_ERROR_TTL_CODE_04				3545
#define LOG_ERROR_TTL_BM_UPDATE             3546  // CstSendBMasterFH(!CLI_FCBAKGCF)) is negative in function CstTtlUpdateFH()
#define LOG_ERROR_TTL_M_UPDATE              3547  // CstSendMasterFH(!CLI_FCBAKGCF)) is negative in function CstTtlUpdateFH()

/* LOG_ERROR_KPS_  from 3550 to 3599 */
#define LOG_ERROR_KPS_REWIND_FRAME          3550
#define LOG_ERROR_KPS_SPOOL_OVER            3551
#define LOG_ERROR_KPS_SEQUENCE_ERROR        3552
#define LOG_ERROR_KPS_BUFFER_CLEAR          3553    /* BUFFER CLEAR BY TIME UP 5 MINUTES */
#define LOG_ERROR_KPS_PRINTER_ERROR         3560    /* 3560 ~ 3564  RESERVED FOR PRINTE NO */
#define LOG_ERROR_KPS_CODE_01				3561
#define LOG_ERROR_KPS_CODE_02				3562
#define LOG_ERROR_KPS_CODE_03				3563
#define LOG_ERROR_KPS_CODE_04				3564
#define LOG_ERROR_KPS_CODE_05				3565
#define LOG_ERROR_KPS_CODE_06				3566
#define LOG_ERROR_KPS_CODE_07				3567
#define LOG_ERROR_KPS_CODE_08				3568
#define LOG_ERROR_KPS_CODE_09				3569
#define LOG_ERROR_KPS_CONVERTERROR			3570	// KpsConvertError()  reporting error.  See next PifLog.
                                            
/* LOG_ERROR_GCF_  from 3600 to 3649 */
#define LOG_ERROR_GCF_DRIVE_UNMATCH         3600    // Gcf_Index() returned GCF_NOT_IN in GusAllIdRead()
#define LOG_ERROR_GCF_DRIVE_UNMATCH2        3601    // Gcf_Index() returned GCF_NOT_IN in GusAllIdReadBW()
#define LOG_ERROR_GCF_DRIVE_DEFERROR        3602    // Default case taken in GusConvertError()
#define LOG_ERROR_GCF_MAX_LT_CUR            3603    // pGcf_FileHed->usMaxGCN <= pGcf_FileHed->usCurGCN
#define LOG_ERROR_GCF_CUR_ZERO_ERR          3604    // pGcf_FileHed->usMaxGCN <= pGcf_FileHed->usCurGCN
#define LOG_ERROR_GCNUM_GT_MAX_DRIVE        3605    // data error in Gcf_DriveIndexSer()
#define LOG_ERROR_GCF_BACKUP_SIZE_ERR01     3606    // usRcvLen != sizeof(GCF_BACKUP) in GusResBackUpFH()
#define LOG_ERROR_GCF_BACKUP_SIZE_ERR02     3607    // *pusSndLen < sizeof(Gcf_FileHed) in GusResBackUpFH()
#define LOG_ERROR_GCF_FULL_ERR_01           3608    // 
#define LOG_ERROR_GCF_FULL_ERR_02           3609    // 
#define LOG_ERROR_GCF_FULL_ERR_03           3610    // 
#define LOG_ERROR_GCF_FULL_ERR_04           3611    // 
#define LOG_ERROR_GCF_FULL_ERR_05           3612    // 
#define LOG_ERROR_GCF_FULL_ERR_06           3613    // 
#define LOG_ERROR_GCF_BAK_FULL_ERR_01       3614    // function GusBakAutoNo ()
#define LOG_ERROR_GCF_BAK_FULL_ERR_02       3615    // function GusBakAutoNo ()
#define LOG_ERROR_GCF_BAK_FULL_ERR_03       3616    //
#define LOG_ERROR_GCF_BAK_FULL_ERR_04       3617    //
#define LOG_ERROR_GCF_BAK_FULL_ERR_05       3618    //
#define LOG_ERROR_GCF_BAK_FULL_ERR_06       3619    //
#define LOG_ERROR_GCF_INVALID_DRIVETHRU     3620    // Gcf_StoreDataFH() switch(usType) default taken, invalid drive thru type
#define LOG_ERROR_GCF_OPT_UNPAID_NOBAL      3621    // Gcf_OptimizeGcfDataFile() usCurNoPayGCN did not match number unpaid transactions
#define LOG_ERROR_GCF_OPT_PAID_NOBAL        3622    // Gcf_OptimizeGcfDataFile() usCurPayGC did not match number paid transactions
#define LOG_ERROR_GCF_OPT_BLOCKS_NOBAL      3623    // Gcf_OptimizeGcfDataFile() calculated number blocks did not match number blocks counted
#define LOG_ERROR_GCF_OPT_BYTES_BADSIZE     3624    // Gcf_OptimizeGcfDataFile() calculated number of bytes not evenly divided by GCF_DATA_BLOCK_SIZE
#define LOG_ERROR_GCF_OPT_NOCHAIN_BLOCKS    3625    // Gcf_OptimizeGcfDataFile() we have blocks that are not part of chains
#define LOG_ERROR_GCF_OPT_UPAID_GT_MAX      3626    // Gcf_OptimizeGcfDataFile() count of unpaid greater than maximum specified in pGcf_FileHed->usMaxGCN
#define LOG_ERROR_GCF_OPT_UPAID_NO_RESERVE  3627    // Gcf_OptimizeGcfDataFile() an upaid transaction has no more reserve blocks available
#define LOG_ERROR_GCF_OPT_NO_FREE_BLOCKS    3628    // Gcf_OptimizeGcfDataFile() the free block chain is empty and there are no free blocks
#define LOG_ERROR_GCF_OPT_INDEX_BADOFFSET   3629    // Gcf_OptimizeGcfDataFile() offulGCIndFile in file header has bad value
#define LOG_ERROR_GCF_OPT_DATA_BADOFFSET    3630    // Gcf_OptimizeGcfDataFile() offulGCDataFile in file header had bad value
#define LOG_ERROR_GCF_CONVERTERROR          3631    // GusConvertError() reports error.  Next PifLog s error code.
#define LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_1  3632    // GusStoreFileFH() argument hsFileHandle has bad (negative) handle value.
#define LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_2  3633    // GusReadLockFH() argument hsFileHandle has bad (negative) handle value.
#define LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_3  3634    // GusIndReadFH() argument hsFileHandle has bad (negative) handle value.
#define LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_4  3635    // GusCheckAndReadFH() argument hsFileHandle has bad (negative) handle value.
#define LOG_ERROR_GCF_TEMPFILE_FH_HANDLE_5  3636    // 
#define LOG_ERROR_GCF_BACKUPFH_FH_HANDLE_1  3637    // GusReqBackUpFH() argument hsFileHandle has bad (negative) handle value.
#define LOG_ERROR_GCF_BACKUPFH_FH_HANDLE_2  3638    // GusResBackUpFH() argument hsFileHandle has bad (negative) handle value.
#define LOG_ERROR_GCF_FULL_ERR_07           3639    // 
#define LOG_ERROR_GCF_FULL_ERR_08           3640    // 
#define LOG_ERROR_GCF_FULL_ERR_09           3641    // 
#define LOG_ERROR_GCF_FULL_ERR_10           3642    // 
#define LOG_ERROR_GCF_NOTLOCKED_ERR         3643    // Gcf_StoreDataFH() (Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) == 0)


/* LOG_ERROR_ISP_  from 3650 to 3699 */
#define LOG_ERROR_ISP_3TIME_ERROR           3650    /* Detect succeeded 3 times of Communication Error */
#define LOG_ERROR_ISP_INVALID_FUNCODE       3651    /* Received a invalid function code */
#define LOG_ERROR_ISP_INVALID_DATA_LEN      3652    /* Received a invalid data length   */
#define LOG_ERROR_ISP_RECV_INVALID_FUN		3653	/* Invalid function code at IspRecv */
#define LOG_ERROR_ISP_STATUS_INVALID_STATE	3654	/* Invalid state at IspChangeStatus */
#define LOG_ERROR_ISP_RCVCPM_INV_FUN		3655	/* Invalid function at IspRecvCpm */
#define LOG_ERROR_ISP_RCVEJ_INV_FUN			3656	/* Invalid function at IspRecvEj */
#define LOG_ERROR_ISP_RCVETK_INV_FUN		3657	/* Invalid function at IspRecvEtk */
#define LOG_ERROR_ISP_RCVGCF_INV_FUN		3658	/* Invalid function at IspRecvGCF */
#define LOG_ERROR_ISP_RCVMSGHND_INV_ST		3659	/* Invalid state at IspRevMesHand*/
#define LOG_ERROR_ISP_RCVOPR_INV_FUN		3660	/* Invalid function at IspRecvOpr*/
#define LOG_ERROR_ISP_RCVMS_INV_LK			3661	/* Invalid lockstate at IspRecvMasEnterCM*/
#define LOG_ERROR_ISP_RCVMSMNT_INV_FUN		3662	/* Invalid function at IspRecvMasMntCom*/
#define LOG_ERROR_ISP_RECVCAS_INV_FUN		3663	/* Invalid function at IspRecvSatCasWai*/
#define LOG_ERROR_ISP_RECTTL_INV_FUN		3664	/* Invalid function at IspRecvTTL*/
#define LOG_ERROR_ISP_RDTTLPLU_INV_DB		3665	/* Invalid DB at IspRecvTtlReadPluEx*/
#define LOG_ERROR_ISP_TOTALID_ERROR_01      3666    // uchTotalID incorrect in IspLoanPickFCAmount()
#define LOG_ERROR_ISP_LABEL_ANALYSIS_FAIL   3667    // IspLabelAnalysis(pPlu) returned error code in IspRecvOpr()
#define LOG_ERROR_ISP_CHECKFUNCODE_FAIL     3668    // IspCheckFunCode( pReqM->usFunCode ) returned error code in IspCheckNetRcvData()
#define LOG_ERROR_ISP_ERR_DUR_BACKUP_01     3669    // IspComReadStatus() returned (fsIspCleanUP & ISP_LOCKED_KEYBOARD) in IspCheckResponse()
#define LOG_ERROR_ISP_ERR_DUR_BACKUP_02     3670    // IspComReadStatus() returned (fsIspCleanUP & ISP_LOCKED_KEYBOARD) in IspCheckResponse()
#define LOG_ERROR_ISP_MAX_RECEIVE_TIME      3671    // IspTimerRead() returned time longer than ISP_MAX_RECEIVE_TIME in IspERHOther()
#define LOG_ERROR_ISP_MAX_EXE_ERROR         3672    // 3 < sIspExeError in IspERHOther()
#define LOG_ERROR_ISP_CLI_FCRMT_ERROR       3673    // (CLIREQCOM *)pReqMsgH)->usFunCode incorrect in RmtFileServer()
#define LOG_ERROR_ISP_CLI_FCRMT_HANDLE      3674    // ((RMTREQWRITE *)pReqMsgH)->usFile incorrect in RmtFileServer()
#define LOG_ERROR_ISP_CLI_FCRMT_TABLEFULL   3675    // ((RMTREQWRITE *)pReqMsgH)->usFile incorrect in RmtFileServer()
#define LOG_ERROR_ISP_NET_RECEIVE_RETRY     3676    // (usPrevErrorCo > 3) in IspNetReceive()
#define LOG_ERROR_ISP_NET_RECEIVE_PWRFL     3677    // (PIF_ERROR_NET_POWER_FAILURE == sError) in IspNetReceive()

/* LOG_ERROR_SERVER_  from 3700 to 3799 */
#define LOG_ERROR_SER_BAD_ENVIROMENT        3700    
#define LOG_ERROR_SER_3TIME_ERROR           3701
#define LOG_ERROR_SER_STUB_BMOD             3702
#define LOG_ERROR_SER_STUB_MTOD             3703
#define LOG_ERROR_SER_AC85_BMOD             3704
#define LOG_ERROR_SER_AC85_MTOD             3705
#define LOG_ERROR_SER_OFFLINE_BMOD          3706
#define LOG_ERROR_SER_ILLEGAL_DATA          3707
#define LOG_ERROR_SER_LOSS_DATA             3708
#define LOG_ERROR_SER_NET_BUSY              3709
#define LOG_ERROR_SER_INQUIRY_REQ           3710
#define LOG_ERROR_SER_INQUIRY_DATE          3711
#define LOG_ERROR_SER_INQUIRY_RESP          3712
#define LOG_ERROR_SER_INQUIRY_SEND          3713
#define LOG_ERROR_SER_INQUIRY_START         3714    // SerChangeStatus(). indicates SER_STINQUIRY
#define LOG_ERROR_SER_SEND_MULTIPLE         3715    // SerSendMultiple(). SerSendResponseFH() error
#define LOG_ERROR_SER_SEND_MULT_BKUP        3716    // SerSendBakMultiple(). SerSendResponseFH() error
#define LOG_ERROR_SER_INQUIRY_TIMEOUT       3717    // SerERHTimeOut(). Time out
#define LOG_ERROR_SER_CODE_01	            3718
#define LOG_ERROR_SER_CODE_02	            3719

/* LOG_ERROR_DISPLAY_  from 3800 to 3849 */
#define LOG_ERROR_MAJOR_NOT_FOUND           3800
#define LOG_ERROR_REG_MAJOR_NOT_FOUND       3801
#define LOG_ERROR_REG_MINOR_NOT_FOUND_1     3802
#define LOG_ERROR_SUP_ERROR                 3840 

/* LOG_ERROR_OPEPARA_  from 3850 to 3899 */
/* LOG_ERROR_UIC_      from 3900 to 3949 */
#define LOG_ERROR_UIC_TF_CP_CREATE_FILE     3900   // UifCreateMain() call to ItemTendCPCreateFile() failed.
#define LOG_ERROR_UIC_TF_CP_OPEN_FILE       3901   // UifCreateMain() call to ItemTendCPOpenFile() failed.
#define LOG_ERROR_UIC_TF_CHECK_DELETE       3902   // UifCreateMain() call to CliCheckDeleteFile() failed.
#define LOG_ERROR_UIC_TF_CHECK_CREATE       3903   // UifCreateMain() call to CliCheckCreateFile() failed.
#define LOG_ERROR_UIC_TF_FLEX_FILE          3904   // UifCreateMain() call to CliCheckCreateFile() failed.
#define LOG_ERROR_UIC_TF_TOTAL_FILE         3905   // CliCheckCreateFile() total file creation failed, next log is line number

/* LOG_ERROR_NB_       from 3950 to 3999 */
#define LOG_ERROR_NB_DETECT_3TIMES          3950    /* Detect succeeded 3 times of Communication Error */
#define LOG_ERROR_NB_INVALID_FUNCODE        3951    /* Received a invalid function code */
#define LOG_ERROR_NB_NET_NOT_PROVIDED       3952    // NbNetChkBoard() IP address is zero
#define LOG_ERROR_NB_NET_RECV_CONTROL       3953    // NbNetReceive() PifControl() returned error
#define LOG_ERROR_NB_NET_RECV_RECEIVE       3954    // NbNetReceive() PifReceive() returned error
#define LOG_ERROR_NB_NET_RECV_INVALIDDATA   3955    // NbNetReceive() received incorrect number of bytes
#define LOG_ERROR_NB_NET_SEND_RETRYLIMIT    3956    // NbNetSend() PifNetSend () retry limit exceeded
#define LOG_ERROR_NB_NET_SEND_SEND          3957    // NbNetSend() PifNetSend () error returned

#define LOG_ERROR_NB_SEND_PIF_ERROR         3996    // SHORT  NbNetSend()  (0 > sRet)
#define LOG_ERROR_NB_SEND_DISCOVERY         3997    // SHORT  NbNetSend()  sRet == PIF_ERROR_NET_DISCOVERY
#define LOG_ERROR_NB_CHKBOARD_WITHOUTBM     3998    // SHORT  NbNetChkBoard()  NbSysFlag.fsSystemInf |= NB_WITHOUTBM
#define LOG_ERROR_NB_THREADSHUTDOWN         3999    // Processing thread shutdown in NbEndThread ()

/* LOG_ERROR_TOTALUM_  from 4000 to 4049 */
#define LOG_ERROR_TOTALUM_UPDATETRANS       4000    // error processing transaction in  TtlTUMMain()
#define LOG_ERROR_TOTALUM_FILEHEAD_ERROR    4001    // TTL_CHECK_RAM error in TtlDeptCreate()
#define LOG_ERROR_TOTALUM_DEPT_CHECKRAM     4002    // TTL_CHECK_RAM error in TtlDeptCreate()
#define LOG_ERROR_TOTALUM_PLU_CHECKRAM      4003    // TTL_CHECK_RAM error in TtlPLUCreate()
#define LOG_ERROR_TOTALUM_CPN_CHECKRAM      4004    // TTL_CHECK_RAM error in TtlCpnCreate()
#define LOG_ERROR_TOTALUM_CAS_CHECKRAM      4005    // TTL_CHECK_RAM error in TtlCasCreate()
#define LOG_ERROR_TOTALUM_SER_CHECKRAM      4006    // TTL_CHECK_RAM error in TtlSerTimeCreate()
#define LOG_ERROR_TOTALUM_TTL_RESET_WRITE   4007    // TtlWriteFile() returned an error.  See next log.
#define LOG_ERROR_TOTALUM_TTL_RESET_READ    4008    // TtlReadFile() returned an error.  See next log.
#define LOG_ERROR_TOTALUM_TTL_BAD_HANDLE    4009    // file handle is bad.  See next log.
#define LOG_ERROR_TOTALUM_TTL_BAD_CLASS     4010    // uchClass specified is bad.  See next log.
#define LOG_ERROR_TOTALUM_TTL_OUT_OF_RANGE  4011    // offset to a total such as hourly is out of range.

/* LOG_ERROR_PARA_     from 4050 to 4079 */
/* LOG_ERROR_EJ_       from 4080 to 4099 */
#define LOG_ERROR_EJ_UNMATCH1               4080    /* Detect diffrent number of line between left and right */
#define LOG_ERROR_EJ_UNMATCH2               4081    /* Detect diffrent number of line between left and right */
#define LOG_ERROR_EJ_INVALID_DATA           4082    /* Detect diffrent number of line between left and right */
#define LOG_ERROR_EJ_CONVERTERROR           4083    // Function EJConvertError () reports non EJ_PERFORM error
                                            
/* LOG_ERROR_SHR_       from 4100 to 4109 */
#define LOG_ERROR_SHR_DELETE                4100    /* detect deleting data */
#define LOG_ERROR_SHR_LOST                  4101    /* lost data */
#define LOG_ERROR_SHR_CODE_01               4102
#define LOG_ERROR_SHR_CODE_02               4103
#define LOG_ERROR_SHR_CODE_03               4104
#define LOG_ERROR_SHR_TERMADR               4105    // ShrTermLock() bad uchTermAdr specified
#define LOG_ERROR_SHR_FRAMECHK1             4106    // ShrPrint() error in ShrFrameCheck()
#define LOG_ERROR_SHR_UNLOCKFAILED          4107    // ShrPrint() error in ShrUnLock()

/* LOG_ERROR_DFL_       from 4110 to 4119 */

/* LOG_ERROR_PROGPRT_   from 4120 to 4129 */
#define LOG_ERROR_PROGRPT_INVALID_ARG       4120    /* invalid argument received */
#define LOG_ERROR_PROGRPT_INVALID_DATA      4121    /* found invalid data in file */

/* LOG_ERROR_DIF_       from 4130 to 4140 */
#define LOG_ERROR_DIF_LRC_ERROR             4130    /* LRC error at data received */

// LOG_EVENT_           from 6000 to 6200
//    LOG_EVENT_ codes are defined in log.h

/* LOG_ERROR_DEPT_		from 6200 to 6250 */

#define LOG_ERROR_DEPT_READ_CURDAY			6200
#define LOG_ERROR_DEPT_READ_CURPTD			6201
#define LOG_ERROR_DEPT_READ_SAVDAY			6202
#define LOG_ERROR_DEPT_READ_SAVPTD			6203


/* LOG_ERROR_PLU_		from 6251 to 6299 */

#define LOG_ERROR_PLU_READ_CURDAY			6251
#define LOG_ERROR_PLU_READ_CURPTD			6252
#define LOG_ERROR_PLU_READ_SAVDAY			6253
#define LOG_ERROR_PLU_READ_SAVPTD			6254
#define LOG_ERROR_PLU_CREATE_TABLE			6255  // TtlPLUCreate() indicates TTLPLUCREATE_ERR error

/* LOG_ERROR_PIF_		from 6300 to 6349 */

#define LOG_ERROR_PIF_WRNG_LOCK				6300
#define LOG_ERROR_PIF_WRNG_CLSS				6301
#define LOG_ERROR_PIF_HIGHWATER_MATCH       6302  // PifRequestSem () caused high water mark to be met
#define LOG_ERROR_PIF_LOWWATER_MATCH        6302  // PifReleaseSem () caused low water mark to be met


/* LOG_ERROR_CLIENT			6350 to 6399 */

#define LOG_ERROR_CLIENT_WRNG_ADR			6350
#define LOG_ERROR_CLIENT_INVALID_FUNCODE	6351

// LOG_ERR_R_ETK            6400 to 6459
#define LOG_ERROR_ETK_PARAM_OPEN            6400
#define LOG_ERROR_ETK_TOTAL_OPEN            6401
#define LOG_ERROR_ETK_PARA_CREATE           6402
#define LOG_ERROR_ETK_TOTAL_CREATE          6403
#define LOG_ERROR_ETK_ETK_FULL              6404  // Next log is the line number where the file full was detected.

// LOG_ERR_TENDER_          6460 to 6499
#define LOG_ERROR_TENDER_EPT_BAD_CODE       6460  // CPRcv.auchStatus is not one of the EEPT_FC2_ type codes

/*
*===========================================================================
*  Application Event Codes
*===========================================================================
*/
/* LOG_EVENT_UI_    from 7000 to 7049 */
#define LOG_EVENT_UI_POWER_DOWN           7000      /* Power Down Event     */
#define LOG_EVENT_UI_AC_LOST              7001      /* AC Lost Event        */
#define LOG_EVENT_UI_RETURN_SUSPEND       7002      /* Return from Suspend Event        */
#define LOG_EVENT_UI_DISCOUNT_ERR_01      7003      /* uchMinorClass value bad in ItemDiscItemDiscModif()  */
#define LOG_EVENT_UI_DISCOUNT_ERR_02      7004      /* uchMinorClass value bad in ItemDiscGetTaxMDC()  */
#define LOG_EVENT_UI_DISCOUNT_ERR_03      7005      /* uchMinorClass value bad in ItemDiscCommonIF()  */
#define LOG_EVENT_UI_DISCOUNT_ERR_04      7006      /* uchMinorClass value bad in ItemDiscCheckSeq()  */
#define LOG_EVENT_UI_DISCOUNT_ERR_05      7007      /* uchMinorClass value bad in ItemDiscCalculation()  */
#define LOG_EVENT_UI_DISPLAY_ERR_01       7011      /* uchMajorClass value bad in DispSupComData20()  */
#define LOG_EVENT_UI_DISPLAY_ERR_02       7012      /* uchMajorClass value bad in DispSupBiData20()  */
#define LOG_EVENT_UI_DISPLAY_ERR_03       7013      /* uchMajorClass value bad in DispSupDEPTPLU20()  */
#define LOG_EVENT_UI_DISPLAY_ERR_04       7014      /* uchMajorClass value bad in DispSupMnemo20()  */
#define LOG_EVENT_UI_FDT_TERMTYPE_ERR     7015      /* WorkFDT.uchTypeTerm bad in ItemTotalSEStoRecGCFSave() */

/* LOG_EVENT_ITEM_  from 7050 to 7099 */
#define LOG_EVENT_ITM_CLI_FUNC              7050    /* occer during execuitng client function */

/* LOG_EVENT_TRN_     from 7100 to 7149 */
#define LOG_EVENT_TRN_CALSTOSIZE            7100    // value calculated by TrnCalStoSize() greater than 0xffff or USHORT
#define LOG_EVENT_TRN_CREATE_OPENFILE       7101    // Call to TrnOpenFile() failed in function TrnCreateFile()
#define LOG_EVENT_TRN_CREATE_EXPANDFILE     7102    // Call to TrnExpandFile() failed in function TrnCreateFile()
#define LOG_EVENT_TRN_CREATE_INDEXFILE_01   7103    // Call to TrnCreateIndexFile() failed in function TrnCreateFile()
#define LOG_EVENT_TRN_CREATE_INDEXFILE_02   7104    // Call to TrnCreateIndexFile() failed in function TrnCreateFile() for FLEX_ITEMSTORAGE_ADR
#define LOG_EVENT_TRN_CREATE_INDEXFILE_03   7105    // Call to TrnCreateIndexFile() failed in function TrnCreateFile() FLEX_CONSSTORAGE_ADR
#define LOG_EVENT_TRN_CREATE_POSTFILE       7106    // Call to TrnCreatePostRecFile() failed in function TrnCreateFile()
#define LOG_EVENT_TRN_CREATEPOST_OPEN       7107    // Call to TrnOpenFile() failed in function TrnCreatePostRecFile()
#define LOG_EVENT_TRN_CREATEPOST_EXPAND     7108    // Call to TrnExpandFile() failed in function TrnCreatePostRecFile()
#define LOG_EVENT_TRN_CREATEINDEX_OPEN      7109    // Call to TrnOpenFile() failed in function TrnCreateIndexFile()
#define LOG_EVENT_TRN_CREATEINDEX_EXPAND    7110    // Call to TrnExpandFile() failed in function TrnCreateIndexFile()
#define LOG_EVENT_TRN_CONVERTERROR          7111    // TrnConvertError() reports error.  Next PifLog contains code.
#define LOG_EVENT_TRN_OPEN_TRANSACTION      7112    // TrnOpen() called.

/* LOG_EVENT_PRINT_   from 7150 to 7199 */
#define LOG_EVENT_PRINT_TIMERSTOP_EXPIRED   7150   // timer expired when SerSpsTimerStop() called
#define LOG_EVENT_REPORT_RPTPRTERROR        7151   // RptPrtError() called to report an error so log it.

/* LOG_EVENT_MAINT_   from 7200 to 7249 */
#define LOG_EVENT_MAINT_BKUP_NO_BACKUP      7200    // SerStartBackUp () Backup is not provisioned.
#define LOG_EVENT_MAINT_BKUP_MAS_NOTUPDATE  7201    // SerStartBackUp () Master is not up to date
#define LOG_EVENT_MAINT_BKUP_BM_NOTUPDATE   7202    // SerStartBackUp () Backup Master is not up to date.
#define LOG_EVENT_MAINT_BKUP_SATELLITE      7203    // SerStartBackUp () Terminal is Satellite (neither Mas or BMas.
#define LOG_EVENT_MAINT_BKUP_GCF_LOCK_FAIL  7204    // SerStartBackUp () Failed in attempt to lock Guest Check File.
#define LOG_EVENT_MAINT_BKUP_KPS_BUSY       7205    // SerStartBackUp () Kitchen Printer is still busy after retries..
#define LOG_EVENT_MAINT_BKUP_INQ_BUSY       7206    // SerStartBackUp () Terminal inquiry status is busy..

/* LOG_EVENT_REPORT_  from 7250 to 7299 */
/* LOG_EVENT_STUB_    from 7300 to 7399 */
#define LOG_EVENT_STB_CNT_DOWN_SND_RCV      7300    // CstComSendReceive () Count down expired
#define LOG_EVENT_STB_CNT_DOWN_CHK_INQ      7301    // CstComChekInquiry () Count down expired
#define LOG_EVENT_STB_CNT_DOWN_BCK_ERR      7302    // CstBackUpError () Count down expired
#define LOG_EVENT_STB_CNT_DOWN_NET_RCV      7303    // CstNetReceive () Count down expired
#define LOG_EVENT_STB_CNT_DOWN_NET_RCV_FH   7304    // CstComSendReceiveFH () Count down expired
#define LOG_EVENT_STB_CF_FLEX_DEPT_ADR      7305    // CliCreateFile () File create error OpDeptCreatFile()
#define LOG_EVENT_STB_CF_FLEX_PLU_ADR       7306    // CliCreateFile () File create error OpOepCreatFile()
#define LOG_EVENT_STB_CF_FLEX_EJ_ADR        7307    // CliCreateFile () File create error EJCreat()
#define LOG_EVENT_STB_CF_FLEX_ITEMSTORAGE_ADR   7308    // CliCreateFile () File create error KpsCreateFile()
#define LOG_EVENT_STB_CF_FLEX_CONSSTORAGE_ADR   7309    // CliCreateFile () File create error TrnCreateFile()
#define LOG_EVENT_STB_CF_FLEX_CPN_ADR       7310    // CliCreateFile () File create error OpCpnCreatFile()
#define LOG_EVENT_STB_CF_FLEX_CSTRING_ADR   7311    // CliCreateFile () File create error OpCstrCreatFile()
#define LOG_EVENT_STB_CF_FLEX_PPI_ADR       7312    // CliCreateFile () File create error OpPpiCreatFile()
#define LOG_EVENT_STB_CF_FLEX_MLD_ADR       7313    // CliCreateFile () File create error OpMldCreatFile()
#define LOG_EVENT_STB_CF_FLEX_CAS_ADR       7314    // CliCreateFile () File create error CasCreatFile()
#define LOG_EVENT_STB_CF_FLEX_GC_ADR        7315    // CliCreateFile () File create error GusCreateGCF()
#define LOG_EVENT_STB_CF_FLEX_ETK_ADR       7316    // CliCreateFile () File create error EtkCreatFile()
#define LOG_EVENT_STB_CF_FLEX_PROGRPT_ADR   7317    // CliCreateFile () File create error ProgRptCreate()
#define LOG_EVENT_STB_CONVERTERROR          7318    // CstConvertError() reporting error.  See next PifLog.
#define LOG_EVENT_STB_CONVERTERROR_MMM      7319    // CliConvertErrorMMM() reporting error.  See next PifLog.
#define LOG_EVENT_STB_CONVERTERROR_PROGRPT  7320    // ProgRpt_ConvertError() reporting error.  See next PifLog.
#define LOG_EVENT_STB_CSTDISPLAYERROR       7321    // CstDisplayError() reporting error.  See next PifLog.
#define LOG_EVENT_STB_IP_MISMATCH           7322    // CstNetReceive() CliSndBuf.auchFaddr[CLI_POS_UA] != CliRcvBuf.auchFaddr[CLI_POS_UA]
#define LOG_EVENT_STB_RSTCONTCODE_MISMATCH  7323    // CstNetReceive() (pSend->usFunCode & CLI_RSTCONTCODE ) != (pResp->usFunCode & CLI_RSTCONTCODE)
#define LOG_EVENT_STB_SEQNUMBER_MISMATCH    7324    // CstNetReceive() (pSend->usSeqNo & CLI_SEQ_CONT) != (pResp->usSeqNo & CLI_SEQ_CONT)
#define LOG_EVENT_STB_JOIN_PROCESSED        7325    // CliOpJoinCluster() doing a CLI_FCOPJOINCLUSTER, provisioning data follows
#define LOG_EVENT_STB_CONVERTERROR_CPM      7326    // CpmConvertError() reporting error.  See next PifLog.
#define LOG_EVENT_STB_OP_MSG_PROCESSED      7327    // CliOpJoinCluster() doing a CLI_FCOPJOINCLUSTER, provisioning data follows
#define LOG_EVENT_STB_UNJOIN_FORWARDING     7328    // CheckUnjoinStatus() Store and Forward turned on and currently Forwarding
#define LOG_EVENT_STB_UNJOIN_COUNT_MISMATCH 7329    // CheckUnjoinStatus() Store and Forward turned on counts do not agree
#define LOG_ERROR_STB_TEMP_FILE_HANDLE_BAD	7330	// ItemTendCPClearFile() Temp file has a bad handle

/* LOG_EVENT_CASHIER_  from 7400 to 7449 */
#define LOG_EVENT_CAS_DELETE_FILE           7400    /* Delete file     */
#define LOG_EVENT_CAS_OPEN_FILE             7401    /* Error on Open file, line number follows     */

/* LOG_EVENT_WAITER_   from 7450 to 7499 */
#define LOG_EVENT_WAI_DELETE_FILE           7450    /* Delete file     */

/* LOG_EVENT_TOTAL_    from 7500 to 7549 */
#define LOG_EVENT_TTL_DEPT_DELETE_FILE      7500    /* Delete file     */
#define LOG_EVENT_TTL_PLU_DELETE_FILE       7501    /* Delete file     */
#define LOG_EVENT_TTL_CAS_DELETE_FILE       7502    /* Delete file     */
#define LOG_EVENT_TTL_WAI_DELETE_FILE       7503    /* Delete file     */
#define LOG_EVENT_TTL_CPN_DELETE_FILE       7504    /* Delete file, R3.0 */
#define LOG_EVENT_TTL_UPD_BUFFER_FULL_01    7505    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_UPD_BUFFER_FULL_02    7506    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_UPD_BUFFER_FULL_03    7507    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_UPD_TTL_UPBUFF_FULL   7508    // Set buffer full flag  TtlUphdata.uchTtlFlag0 |= TTL_UPBUFF_FULL
#define LOG_EVENT_TTL_UPD_TTL_UPBUFF_RESET  7509    // Reset buffer full flag  TtlUphdata.uchTtlFlag0 &= ~TTL_UPBUFF_FULL
#define LOG_EVENT_TTL_UPD_TTL_FILE_RSET     7510    // Inconsistent file header pointers on startup in TtlTUMMain ()
#define LOG_EVENT_TTL_UPD_TTL_FILE_RSET_CLR 7511    // Inconsistent file header pointers on startup in TtlTUMMain ()
#define LOG_EVENT_TTL_UPD_BUFFER_FULL_04    7512    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_CONVERTERROR          7513    // TtlConvertError() reports error.  Next PifLog contains code.
#define LOG_EVENT_TTL_TAX_INCONSTANCY       7514    // Total inconsistancy was found and corrected in RptRegFinEdit().
#define LOG_EVENT_TTL_READ_ERR				7515	// Error reading cashier file in TtlGetIndFinOffset()
#define LOG_EVENT_TTL_POINTER_ERR			7516	// Error getting pointer offset in TtlwriteFin(), TtlreadHour(), TtlreadCas(), and TtlreadIndFin */
#define	LOG_EVENT_TTL_SAVEFILE_SAVED_NEW	7517	// TtlCreateSavFile() error open auchNEW_FILE_READ_WRITE_SAVTTL
#define	LOG_EVENT_TTL_SAVEFILE_SAVED_OLD	7518	// TtlCreateSavFile() error open auchOLD_FILE_READ_WRITE_SAVTTL
#define	LOG_EVENT_TTL_SAVEFILE_WRT_BADHNDLE	7519	// TtlWriteSavFile() hsTtlSavFileHandle contains error value not file handle
#define	LOG_EVENT_TTL_SAVEFILE_SAVFILE_SIZE	7520	// TtlCreateSavFile() Save file size no match expected size
#define LOG_EVENT_TTL_UPD_NEAR_FULL_01      7521    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_UPD_NEAR_FULL_02      7522    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_UPD_NEAR_FULL_03      7523    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_UPD_NEAR_FULL_04      7524    // indicates file full in TtlUpdateFileFH()
#define LOG_EVENT_TTL_CAS_FILE_HANDLE       7525    // hsTtlCasHandle file handle is invalid
#define LOG_EVENT_TTL_CPN_FILE_HANDLE       7526    // hsTtlCpnHandle file handle is invalid
#define LOG_EVENT_TTL_CASHIER_MAX_EXCEEDED  7527    // max number of cashiers for total file exceeded in TtlCasAssin()
#define LOG_EVENT_TTL_TOTALRESET_START      7528    // TtlTotalReset() function executing.
#define LOG_EVENT_TTL_TOTALUPDATE_LOCK_TTL  7529    // TtlUpdateFileFH(): (chTtlLock == TTL_LOCKED)
#define LOG_EVENT_TTL_TOTALUPDATE_LOCK_TUM  7530    // TtlUpdateFileFH(): (chTtlLock == TTL_LOCKED)

/* LOG_EVENT_KPS_  from 7550 to 7599 */
#define LOG_EVENT_KPS_THREADSTART_RCV_00    7550
#define LOG_EVENT_KPS_THREADSTART_RCV_01    7551
#define LOG_EVENT_KPS_THREADSTART_RCV_02    7552
#define LOG_EVENT_KPS_THREADSTART_RCV_03    7553
#define LOG_EVENT_KPS_THREADSTART_RCV_04    7554
#define LOG_EVENT_KPS_THREADSTART_RCV_05    7555
#define LOG_EVENT_KPS_THREADSTART_RCV_06    7556
#define LOG_EVENT_KPS_THREADSTART_RCV_07    7557
#define LOG_EVENT_KPS_THREADSTART_RCV_08    7558
#define LOG_EVENT_KPS_THREADSTART_SEND      7560
#define LOG_EVENT_KPS_KDS_BACKUP_DOWN       7561
#define LOG_EVENT_KPS_KDS_NET_RECEIVE       7562   // error in KdsNetReceive() from PifNetReceive()
#define LOG_EVENT_KPS_KDS_NO_IP_ADDRESS     7563   // warning in KdsInit() no valid KDS host addresses
#define LOG_EVENT_KPS_KDS_READ_PARMADDR     7564   // bad address specified for ParaKdsIpRead()
#define LOG_EVENT_KPS_KDS_WRITE_PARMADDR    7565   // bad address specified for ParaKdsIpWrite()
#define LOG_EVENT_KPS_TIMERSTOP_EXPIRED     7566   // timer expired when SerKpsTimerStop() called
#define LOG_EVENT_KPS_PORT_OPEN_FAIL        7567   // in KpsMoniter() PifOpenNetComIoEx() returned error, error follows
#define LOG_EVENT_KPS_PORT_OPEN_REOPEN      7568   // in KpsMoniter() PifOpenNetComIoEx() succeeded after printer being down.

/* LOG_EVENT_GCF_  from 7600 to 7649 */
#define LOG_EVENT_GCF_DELETE_FILE           7600    /* Delete file     */
#define LOG_EVENT_GCF_DATALENGTH01          7601    /* usDataLen exceeds receiving buffer size */
#define LOG_EVENT_GCF_DATALENGTH02          7602    /* usDataLen exceeds receiving buffer size */
#define LOG_EVENT_GCF_DATALENGTH03          7603    /* usDataLen exceeds receiving buffer size */
                       
/* LOG_EVENT_ISP_  from 7650 to 7699 */
#define LOG_EVENT_ISP_LOCK_KEYBOARD         7650    /* Lock Keyboard   */
#define LOG_EVENT_ISP_UNLOCK_KEYBOARD       7651    /* Unlock Keyboard */
#define LOG_EVENT_ISP_CREATE_FILE           7660    /* Create File     */
#define LOG_EVENT_ISP_CHANGE_PLU            7661    /* Change PLU      */
#define LOG_EVENT_ISP_CHANGE_PARA           7662    /* Change Para     */
#define LOG_EVENT_ISP_DATA_ERR_01           7663    /* error retrieving data */
#define LOG_EVENT_ISP_DATA_ERR_02           7664    /* error retrieving data */
#define LOG_EVENT_ISP_DATA_ERR_03           7665    /* error retrieving data */
#define LOG_EVENT_ISP_DATA_ERR_04           7666    /* error retrieving data */
#define LOG_EVENT_ISP_DATA_ERR_05           7667    /* error retrieving data */
#define LOG_EVENT_ISP_RELOADLAYOUT          7668    /* error retrieving data */
#define LOG_EVENT_ISP_RELOADOEP             7669    /* error retrieving data */
#define LOG_EVENT_ISP_DELETE_FILE           7670    /* error retrieving data */
#define LOG_EVENT_ISP_NETOPEN               7671    /* IspNetOpen() function has been called */
#define LOG_EVENT_ISP_NETCLOSE              7672    /* IspNetClose() function has been called */
#define LOG_EVENT_ISP_RSET_CASLOCK          7673    /* IspRecvTtlReset() SerCasAllLock() failed */
#define LOG_EVENT_ISP_RSET_FAIL             7674    /* IspRecvTtlReset() SerTtlTotalReset() failed */
#define LOG_EVENT_ISP_PARAM_BROADCAST       7675    /* IspRecvPassBroadcast() SerTtlTotalReset() failed */
#define LOG_EVENT_ISP_TOTAL_RQST_FAIL       7676    /* IspRecvTtl() request failed */
#define LOG_EVENT_ISP_OPR_RQST_FAIL         7677    /* IspRecvOpr() request failed */
#define LOG_EVENT_ISP_EJ_RQST_FAIL          7678    /* IspRecvEj() request failed */
#define LOG_EVENT_ISP_GC_RQST_FAIL          7679    /* IspRecvGCF() request failed */
#define LOG_EVENT_ISP_REMOTE_LOGIN          7680    /* IspRecvPassLogOn() called */
#define LOG_EVENT_ISP_REMOTE_LOGOFF         7681    /* IspRecvPassLogOff() called */
#define LOG_EVENT_ISP_SET_DATE_TIME         7682    /* IspRecvDateTimeWrite() called */
#define LOG_EVENT_ISP_PASSWORD_CHANGE       7683    /* IspRecvPassChange() called */

/* LOG_EVENT_SERVER_  from 7700 to 7799 */
#define LOG_EVENT_SERVER_SPL_REQST          7700    // SstSetFlagWaitDone() sSerExeError != STUB_SUCCESS, next entry is sSerExeError value
#define LOG_EVENT_SER_CNT_DOWN_NET_RCV      7701    // SerNetReceive () Count down expired
#define LOG_EVENT_SER_CAS_BAD_FUNCODE       7702    // SerRecvCas () bad function code received
#define LOG_EVENT_SER_GCF_BAD_FUNCODE       7703    // SerRecvGCF () bad function code received
#define LOG_EVENT_SER_OPR_BAD_FUNCODE       7704    // SerRecvOpr () bad function code received
#define LOG_EVENT_SER_TTL_BAD_FUNCODE       7705    // SerRecvOpr () bad function code received
#define LOG_EVENT_SER_TEMP_FILE_ERR_01      7706    // Server temporary file error see SERVSST.C
#define LOG_EVENT_SER_TEMP_FILE_ERR_02      7707    // Server temporary file error see SERVSST.C
#define LOG_EVENT_SER_TEMP_FILE_ERR_03      7708    // Server temporary file error see SERVSST.C
#define LOG_EVENT_SER_TEMP_FILE_ERR_04      7709    // Server temporary file error see SERVSST.C
#define LOG_EVENT_SER_TEMP_FILE_ERR_05      7710    // Server temporary file error see SERVSST.C
#define LOG_EVENT_SER_AC42_M_NOTUPDATE      7711    // SerESRAC42onBM ()
#define LOG_EVENT_SER_AC42_BM_NOTUPDATE     7712    // SerESRAC42onMT ()
#define LOG_EVENT_SER_AC42_M_OFFLINE        7713    // SerESRAC42onBM ()
#define LOG_EVENT_SER_AC42_BM_OFFLINE       7714    // SerESRAC42onMT ()
#define LOG_EVENT_SER_AC42_M_DUR_INQUIRY    7715    // SerESRAC42onMT ()
#define LOG_EVENT_SER_AC42_BM_DUR_INQUIRY   7716    // SerESRAC42onBM ()
#define LOG_EVENT_SER_AC42_M_ILLEGAL        7717    // SerESRAC42onMT ()
#define LOG_EVENT_SER_AC42_BM_ILLEGAL       7718    // SerESRAC42onBM ()
#define LOG_EVENT_SER_JOINMSG_RECEIVED      7719    // SerOpJoinCluster() received/processing CLIREQJOIN
#define LOG_EVENT_SER_UNJOINMSG_RECEIVED    7720    // SerOpUnjoinCluster() received/processing CLIREQJOIN
#define LOG_EVENT_SER_TRANSFERRQST_RECEIVED 7721    // SerOpTransferTotals() received/processing CLIREQTRANSFER
#define LOG_EVENT_SER_TTOTALUPDATE_FAILED   7722    // TtlUpdateFileFH() failed.  following logs provide additional info.

/* LOG_EVENT_DISPLY_  from 7800 to 7849 */

/* LOG_EVENT_OPEPARA_  from 7850 to 7899 */
#define LOG_EVENT_OP_DEPT_DELETE_FILE       7850    /* Delete file     */
#define LOG_EVENT_OP_PLU_DELETE_FILE        7851    /* Delete file     */
#define LOG_EVENT_OP_CPN_DELETE_FILE        7852    /* Delete file     */
#define LOG_EVENT_OP_CSTR_DELETE_FILE       7853    /* Delete file     */
#define LOG_EVENT_OP_PPI_DELETE_FILE        7854    /* Delete file     */
#define LOG_EVENT_OP_MLD_DELETE_FILE        7855    /* Delete file, V3.3     */
#define LOG_EVENT_OP_OEP_DELETE_FILE        7856    /* Delete file, saratoga     */
#define LOG_EVENT_OP_CPN_AUTO_OPCPNINDREAD  7857    // error with CliOpCpnIndRead() in ItemAutoCouponCheck()
#define LOG_EVENT_OP_CPN_AUTO_GETTABLE_01   7858    // error with ItemCouponGetTable() in ItemAutoCouponCheck()
#define LOG_EVENT_OP_CREATEFILE_EJ          7859    // Op_AllFileCreate(): CliCheckCreateFile() failed.
#define LOG_EVENT_OP_CREATEFILE_FLEX00      7860    // Op_AllFileCreate(): CliCheckCreateFile() failed.
#define LOG_EVENT_OP_CREATEFILE_FLEX        7861    // Op_AllFileCreate(): CliCheckCreateFile() failed.
#define LOG_EVENT_OP_CONVERTERROR           7862    // OpConvertError() reports error.  See next PifLog () for code.
#define LOG_EVENT_OP_SPLU_INVALID_DATA      7863    // OpConvertPluError() reports error.  SPLU_INVALID_DATA.
#define LOG_EVENT_OP_SPLU_FILE_NOT_FOUND    7864    // OpConvertPluError() reports error.  SPLU_FILE_NOT_FOUND.
#define LOG_EVENT_OP_SPLU_DEVICE_FULL       7865    // OpConvertPluError() reports error.  SPLU_DEVICE_FULL.
#define LOG_EVENT_OP_SPLU_FILE_OVERFLOW     7866    // OpConvertPluError() reports error.  SPLU_FILE_OVERFLOW.
#define LOG_EVENT_OP_SPLU_REC_LOCKED        7867    // OpConvertPluError() reports error.  SPLU_REC_LOCKED.
#define LOG_EVENT_OP_SPLU_REC_PROTECTED     7868    // OpConvertPluError() reports error.  SPLU_REC_PROTECTED.
#define LOG_EVENT_OP_SPLU_FILE_LOCKED       7869    // OpConvertPluError() reports error.  SPLU_FILE_LOCKED.
#define LOG_EVENT_OP_SPLU_REC_NOT_FOUND     7870    // OpConvertPluError() reports error.  SPLU_REC_NOT_FOUND.
#define LOG_EVENT_OP_SPLU_FUNC_CONTINUED    7871    // OpConvertPluError() reports error.  SPLU_FUNC_CONTINUED.
#define LOG_EVENT_OP_SPLU_END_OF_FILE       7872    // OpConvertPluError() reports error.  SPLU_END_OF_FILE.
#define LOG_EVENT_OP_SPLU_INVALID_HANDLE    7873    // OpConvertPluError() reports error.  SPLU_INVALID_HANDLE.
#define LOG_EVENT_OP_SPLU_TOO_MANY_USERS    7874    // OpConvertPluError() reports error.  SPLU_TOO_MANY_USERS.
#define LOG_EVENT_OP_SPLU_ADJ_VIOLATION     7875    // OpConvertPluError() reports error.  SPLU_ADJ_VIOLATION.
#define LOG_EVENT_OP_SPLU_FUNC_KILLED       7876    // OpConvertPluError() reports error.  SPLU_FUNC_KILLED.
#define LOG_EVENT_OP_REQFILE_PIFOPENFAIL    7877    // OpReqFile(): PifOpenFile() failed..
#define LOG_EVENT_OP_REQFILE_CSTREQREADFILE 7878    // OpReqFile(): CstReqReadFile() failed..
#define LOG_EVENT_OP_REQFILE_LOCKCHECK      7879    // OpReqFile(): Op_LockCheck(), failed..
#define LOG_EVENT_OP_REQPARA_RQST_FAILED    7880    // OpReqFile(): Op_LockCheck(), failed..
#define LOG_EVENT_OP_REQPARA_RQST_SEQNO     7881    // OpReqFile(): Op_LockCheck(), failed..
#define LOG_EVENT_OP_REQPARA_COMM_ERROR     7882    // OP_COMM_ERROR failure such as buffer size too small.
#define LOG_EVENT_OP_OEP_CREATE_FILE        7883    /* OpOepCreatFile() failed.  Next log is error number     */
#define LOG_EVENT_OP_OEP_OPEN_FILE          7884    /* Op_OepOpenFile() failed.       */


/* LOG_EVENT_UIC_  from 7900 to 7949 */
#define LOG_EVENT_UIC_CHANGE_PLU            7900    /* Change PLU      */
#define LOG_EVENT_UIC_CHANGE_SUP            7901    /* Change Sup      */
#define LOG_EVENT_UIC_CHANGE_PARA           7902    /* Change Para     */
#define LOG_EVENT_UIC_CHANGE_LAY            7903    /* Change Layout     */ //ESMITH LAYOUT
#define LOG_EVENT_UIC_RESET_CONSECUTIVE     7910    /* Reset cons no   */
#define LOG_EVENT_UIC_INVALID_MSR_CARD      7911    // operator sign-in via MSR, card invalid. UifSignInMSR()
#define LOG_EVENT_UIC_CHANGE_LOGOFILES      7912    // changed the printer logo files: CliReqOpPara(CLI_LOAD_PRINTER_LOGOS);

/* LOG_EVENT_NB_       from 7950 to 7999 */
#define LOG_EVENT_NB_NETREOPEN_START        7950    // NbNetReOpen() called, starting into reopen loop
#define LOG_EVENT_NB_NETREOPEN_END          7951    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_MTONLINE_RESET         7952    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_MTUPTODATE_RESET       7953    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_MTONLINE_SET           7954    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_MTUPTODATE_SET         7955    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_BMONLINE_RESET         7956    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_BMUPTODATE_RESET       7957    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_BMONLINE_SET           7958    // NbNetReOpen() called, loop complete, returning to caller
#define LOG_EVENT_NB_BMUPTODATE_SET         7959    // NbNetReOpen() called, loop complete, returning to caller

/* LOG_EVENT_TOTALUM_  from 8000 to 8049 */
#define LOG_EVENT_TOTALUM_JOINMSG_START     8000    // TtlTUMJoinDisconnectedSatellite() reports Join message received
#define LOG_EVENT_TOTALUM_JOINMSG_FINI      8001    // TtlTUMJoinDisconnectedSatellite() completed processing message

/* LOG_EVENT_PARA_     from 8050 to 8079 */
/* LOG_EVENT_EJ_       from 8080 to 8099 */
#define LOG_EVENT_EJ_FFULL                  8080    /* File Full Case */
#define LOG_EVENT_EJ_FCREAT                 8081    /* File Create Case */
#define LOG_EVENT_EJ_RESET                  8082    /* Reset EJ File Case */
#define LOG_EVENT_EJ_FCHECK                 8083    /* File Check and Create Case */
#define LOG_EVENT_EJ_RECONCILE_ERR			8084
#define LOG_EVENT_EJ_SIGNATURE_ERR			8085

/* LOG_EVENT_ETK_  from 8100 to 8129 */
#define LOG_EVENT_ETK_DELETE_FILE           8100    /* Delete file     */
#define LOG_EVENT_ETK_CONVERTERROR          8101    // reporting raw error in function EtkConvertError()
#define LOG_EVENT_ETK_WARN_LOG_01           8102    // Non-specific ETK warning whose meaning varies depending on module.
#define LOG_EVENT_ETK_WARN_LOG_02           8103    // Non-specific ETK warning whose meaning varies depending on module.
#define LOG_EVENT_ETK_WARN_LOG_03           8104    // Non-specific ETK warning whose meaning varies depending on module.
#define LOG_EVENT_ETK_WARN_LOG_04           8105    // Non-specific ETK warning whose meaning varies depending on module.

/* LOG_EVENT_CP_  from 8130 to 8159 */
#define LOG_EVENT_CP_RC_SND                 8131    /* Send Data for Room Charge */
#define LOG_EVENT_CP_RC_RCV                 8132    /* Receive Data for Room Charge */
#define LOG_EVENT_CP_AC_SND                 8133    /* Send Data for Acct. Charge */
#define LOG_EVENT_CP_AC_RCV                 8134    /* Receive Data for Acct. Charge */
#define LOG_EVENT_CP_PREAUTH_SND            8135    /* Send Data for Pre-Autho. */
#define LOG_EVENT_CP_PREAUTH_RCV            8136    /* Receive Data for Pre-Autho. */
#define LOG_EVENT_CP_CA_SND                 8137    /* Send Data for Credit Autho. */
#define LOG_EVENT_CP_CA_RCV                 8138    /* Receive Data for Credit Autho. */
#define LOG_EVENT_CP_ASK_SND                8141    /* Send Data for Ask,       Saratoga */
#define LOG_EVENT_CP_ASK_RCV                8142    /* Receive Data for Ask,    Saratoga */

/* LOG_EVENT_EPT_ from 8160 to 8189 */
#define LOG_EVENT_EPT_SND                   8161    /* Send Data for EPT */
#define LOG_EVENT_EPT_RCV                   8162    /* Receive Data for EPT */
#define LOG_EVENT_EPT_BAD_STATE_01          8163    // uchEEptStatus unexpected value in EEptMainReceiveTask()
#define LOG_EVENT_EPT_BAD_STATE_02          8164    // uchEEptStatus unexpected value in EEptMainSendTask()
#define LOG_EVENT_EPT_BAD_STATE_03          8165    // uchEEptStatus unexpected value
#define LOG_EVENT_EPT_BAD_STATE_04          8166    // uchEEptStatus unexpected value
#define LOG_EVENT_EPT_BAD_STATE_05          8167    // uchEEptStatus unexpected value
#define LOG_EVENT_EPT_SEND_FAIL_ENQ         8168    // EEptSend(EEPT_ENQ_CHAR) failed in EEptMainSendTask()
#define LOG_EVENT_EPT_PORT_OPN_ERR_01       8169    // EEptRecoverPort() failed in EEptEEptReceiveCom()
#define LOG_EVENT_EPT_PORT_OPN_ERR_02       8170    // PifOpenCom() failed in EEptOpenSetPort()
#define LOG_EVENT_EPT_SEND_BAD_DATA_01      8171    // uchDataType in EEptSend() has unexpected value
#define LOG_EVENT_EPT_BAD_EPTCLIENT_TYPE    8172    // ScfGetDLLDevice() in XEEptOpen() returned unexpected type
#define LOG_EVENT_EPT_CANNOT_CREATE_CLIENT  8173    // m_pPaymentClient->Create() in XEEptOpen() failed.
#define LOG_EVENT_EPT_UNKNOWN_PAD_TYPE      8174    // csPINPad in XEEptOpen() has unknown value.
#define LOG_EVENT_EPT_CANNOT_CREATE_PAD     8175    // m_pPaymentClient->m_pPinPad->Create() in XEEptOpen() failed
#define LOG_EVENT_EPT_ERROR_FORWARDING      8176    // EEPT_ERROR returned by XEEptForwardStoreForward() in function ItemTendStoreForward()
                                            
/* LOG_EVENT_PROGRPT_  from 8190 to 8219 */
#define LOG_EVENT_PROGRPT_DELETE_FILE       8190    /* Delete file     */

/* LOG_EVENT_HTTP_    from 8300 to 8349 */
#define LOG_EVENT_HTTP_BAD_FUNC_POINTER     8300    // HttpThreadMyInterface struct has bad function pointers.  see HttpStartUp()
#define LOG_EVENT_HTTP_XML_PARSE_ERROR      8301    // see CHttpserverApp::InterpretXML()

/* LOG_EVENT_EEPTINTFACE_      from 8350 to 8399 */
#define LOG_EVENT_EEPT_XML_PARSE_ERR        8350    // see CHttpserverApp::InterpretXML()
#define LOG_EVENT_EEPT_CONTROL_ERR          8351    // see CHttpserverApp::InterpretXML()
#define LOG_EVENT_EEPT_DECRYPT_ERR          8352    // see CHttpserverApp::InterpretXML()

/* LOG_EVENT_STRING_ from 8400 to 8449 */
#define LOG_EVENT_STRING_CALL_ERROR         8400    // UifRegCallString() in UifRegString() returned an error.  Next log is error code.
#define LOG_EVENT_STRING_PAUSE_ERROR        8401    // UifRegPauseString() in UifRegString() returned an error.  Next log is error code.
#define LOG_EVENT_STRING_LOOKUP_ERROR       8402    // CliOpCstrIndRead() in UifRegString() returned an error.  Next log is error code.
#define LOG_EVENT_STRING_SEQ_ERROR          8403    // CliOpCstrIndRead() in UieCheckSeq() returned an error.  Next logs are error code.
#define LOG_EVENT_STRING_EMPTY_ERROR        8404    // WorkCstr.IndexCstr.usCstrLength == 0 in UieCheckSeq() after CliOpCstrIndRead().

/* LOG_EVENT_UIFUIE_ from 8450 to 8499 */
#define LOG_EVENT_UIFUIE_ACTIONCODE         8450    // Action code used in Supervisor mode, Action Code number follows in next log
#define LOG_EVENT_UIFUIE_REG_SIGNIN         8451    // Action code used in Supervisor mode, Action Code number follows in next log
#define LOG_EVENT_UIFUIE_REG_SIGNOUT        8452    // Action code used in Supervisor mode, Action Code number follows in next log

// LOG_ERROR_CODE_     from 9000 to 9100
//   LOG_ERROR_CODE_ are defined in log.h
//   These codes indicate a condition and the next log entry
//   is an error code as returned by GetLastError ().

// LOG_ERROR_PIFNET_ from 9101 to 9200
//   LOG_ERROR_CODE_ are defined in log.h
//   These codes indicate a condition and the next log entry
//   is an error code as returned by WSAGetLastError ().

// LOG_ERROR_PIFSEM_ from 9201 to 9300
//   LOG_ERROR_CODE_ are defined in log.h
//   These codes indicate a condition and the next log entry
//   is an error code as returned by GetLastError ().

// LOG_ERROR_PIFSIO_ from 9301 to 9400
//   LOG_ERROR_CODE_ are defined in log.h
//   These codes indicate a condition and the next log entry
//   is an error code as returned by GetLastError ().

// LOG_ERROR_PIFTASK_ from 9401 to 9500
//   LOG_ERROR_CODE_ are defined in log.h
//   These codes indicate a condition and the next log entry
//   is an error code as returned by GetLastError ().

// LOG_ERROR_TOTALUM__ from 9501 to 9600
//   LOG_ERROR_CODE_ are defined in log.h
//   These codes indicate a condition and the next log entry
//   is an error code as returned by e.error () of _com_error

// LOG_ERROR_VOS_ from 9601 to 9649
//   LOG_ERROR_VOS_ are defined in log.h

#define LOG_EVENT_COMPRESS_OPEN_DATA	9650
#define LOG_EVENT_COMPRESS_SALES_DATA	9651
#define LOG_EVENT_COMPRESS_DISC_DATA	9652
#define LOG_EVENT_COMPRESS_COUP_DATA	9653
#define LOG_EVENT_COMPRESS_TTL_DATA		9654
#define LOG_EVENT_COMPRESS_TEND_DATA	9655
#define LOG_EVENT_COMPRESS_MISC_DATA	9656
#define LOG_EVENT_COMPRESS_OTHR_DATA	9657
#define LOG_EVENT_COMPRESS_PRNT_DATA	9658
#define LOG_EVENT_COMPRESS_MLTI_DATA	9659
#define LOG_EVENT_COMPRESS_CLOSE_DATA	9660
#define LOG_EVENT_COMPRESS_AFFCT_DATA	9661

/* LOG_EVENT_DB_ from 9700 to 9749 */
#define LOG_EVENT_DB_FLAG_OFF_1			9700
#define LOG_EVENT_DB_FLAG_OFF_2			9701
#define LOG_EVENT_DB_FLAG_ON_1			9702
#define LOG_EVENT_DB_FLAG_ON_2			9703
#define LOG_EVENT_DB_FLAG_ON_3			9704
#define LOG_EVENT_DB_FLAG_OFF_3			9705
#define LOG_EVENT_DB_TTLUPDATE_FAIL		9706  // TtlTumUpdateDelayedBalance() status is not TTL_SUCCESS
#define LOG_EVENT_DB_ITEMREAD_FAIL		9707  // TtlTUMUpRead() failed in function TtlTumUpdateDelayedBalance()
#define LOG_EVENT_DB_TRANQUALREAD_FAIL	9708  // TtlTUMUpRead() failed in function TtlTumUpdateDelayedBalance()
#define LOG_EVENT_DB_TTLUPD_FINISH  	9709  // TtlTumUpdateDelayedBalance() finished the delayed balance update
#define LOG_EVENT_DB_UPD_WAI_ERROR  	9710  // TTL_UPD_WAI_ERROR and TTL_UPTOTAL_WAI_ERR affection error
#define LOG_EVENT_DB_UPD_PLU_ERROR  	9711  // TTL_UPD_PLU_ERROR and TTL_UPTOTAL_PLU_ERR affection error
#define LOG_EVENT_DB_UPD_DEP_ERROR  	9712  // TTL_UPD_DEP_ERROR and TTL_UPTOTAL_DEP_ERR affection error
#define LOG_EVENT_DB_UPD_CAS_ERROR  	9713  // TTL_UPD_CAS_ERROR and TTL_UPTOTAL_CAS_ERR affection error
#define LOG_EVENT_DB_UPD_CPN_ERROR  	9714  // TTL_UPD_CPN_ERROR and TTL_UPTOTAL_CPN_ERR affection error
#define LOG_EVENT_DB_TTLUPD_START	  	9715  // TtlTumUpdateDelayedBalance() started the delayed balance update

/* LOG_EVENT_FW_ from 9750 to 9799  */
#define LOG_EVENT_FW_PRINTER_SPEC_ERR   9750  // printer device in CDeviceEngine::Open()
#define LOG_EVENT_FW_SCANNER_SPEC_ERR   9751  // scanner device in CDeviceEngine::Open()
#define LOG_EVENT_FW_SCALE_SPEC_ERR     9752  // scale device in CDeviceEngine::Open()
#define LOG_EVENT_FW_SERVRLOCK_SPEC_ERR 9753  // server lock device in CDeviceEngine::Open()
#define LOG_EVENT_FW_COIN_OPEN_ERR      9754  // failed m_OPOSCoin.Open() in CDeviceEngine::Open()
#define LOG_EVENT_FW_COIN_STATE_ERR     9755  // failed m_OPOSCoin.GetState() in CDeviceEngine::Open()
#define LOG_EVENT_FW_PRINTER_OPEN_ERR   9756  // failed m_OPOSPrinter.Open() in CDeviceEngine::Open()
#define LOG_EVENT_FW_DISPLAY_OPEN_ERR   9757  // failed m_lineDisplay.Open() in CDeviceEngine::Open()
#define LOG_EVENT_FW_DISPLAY_STATE_ERR  9758  // failed m_lineDisplay.GetState() in CDeviceEngine::Open()
#define LOG_EVENT_FW_MSR_STATE_ERR      9759  // failed m_oposMSR.GetState() in CDeviceEngine::Open()
#define LOG_EVENT_FW_MSR_OPEN_ERR       9760  // failed m_oposMSR.Open() in CDeviceEngine::Open()
#define LOG_EVENT_FW_KEYLOCK_STATE_ERR  9761  // failed m_OposKeyLock.GetState() in CDeviceEngine::Open()
#define LOG_EVENT_FW_KEYLOCK_OPEN_ERR   9762  // failed m_OposKeyLock.Open() in CDeviceEngine::Open()
#define LOG_EVENT_FW_CONTROL_CREATE_ERR 9763  // failed m_pOPOSControl->Create(IDD_BASE_DIALOG) in CDeviceEngine::Open()
#define LOG_EVENT_FW_PRINTER_GETDC_ERR  9764  // failed dlg.GetPrinterDC() in CDeviceEngine::Open()
#define LOG_EVENT_FW_WNDTXT_WNDHNDL_ERR 9765  // m_nIdentifier has wrong LCDWIN_ID_ in CFrameworkWndText::SelectItem()
#define LOG_EVENT_FW_WNDTXT_MLDSYS_ERR  9766  // BlFwIfCheckMldSystemStatus() returned bad system type in CFrameworkWndText::SelectItem()
#define LOG_EVENT_FW_LOAD_HTTP_DLL      9767  // failed to LoadDllProcs(), possible not find httpserver.dll
#define LOG_EVENT_FW_HTTP_DLL_NOLOAD    9768  // ScfGetActiveDevice(SCF_TYPENAME_HTTPSERVER) failed, HTTP server not enabled in registery
#define LOG_EVENT_FW_SET_TIMER_FAIL_1   9769  // SetTimer(m_nIdentifier, 800, NULL) failed. void CFrameworkWndText::OnTimer().
#define LOG_EVENT_FW_SET_TIMER_FAIL_2   9770  // SetTimer(FRAME_TIMER_CMD_RELOADOEP, 5000, NULL) failed. BOOL CFrameworkWndDoc::CreateDocument().
#define LOG_EVENT_FW_SET_TIMER_FAIL_3   9771  // SetTimer(FRAME_TIMER_CMD_RELOADOEP, 2500, NULL) failed. void CFrameworkWndDoc::OnTimer().
#define LOG_EVENT_FW_SET_TIMER_FAIL_4   9772  // SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL) failed. void CFrameworkWndDoc::OnTimer().
#define LOG_EVENT_FW_SET_TIMER_FAIL_5   9773  // SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL) failed. void CFrameworkWndDoc::OnTimer().
#define LOG_EVENT_FW_SET_TIMER_FAIL_6   9774  // SetTimer(FRAME_TIMER_CMD_RELOADLAYOUT, 1000, NULL) failed. BOOL CALLBACK FwReloadLayout2().
#define LOG_EVENT_FW_SET_TIMER_FAIL_7   9775  // SetTimer(FRAME_TIMER_CMD_RELOADOEP, 5000, NULL) failed. BOOL CALLBACK FwReloadOepWindow().
#define LOG_EVENT_FW_SET_TIMER_FAIL_8   9776  // SetTimer(m_nIdentifier, 800, NULL) failed. int CFrameworkWndText::OnCreate().
#define LOG_EVENT_FW_WINCREATE_FAIL_1   9777  // CWindowItem::WindowCreate() failed. BOOL CFrameworkWndItem::WindowCreate().
#define LOG_EVENT_FW_WINCREATE_FAIL_2   9778  // Create() failed. BOOL CFrameworkWndButton::WindowCreate().
#define LOG_EVENT_FW_OEP_BUTTON_FAIL_1  9779  // Auto OEP Button Create() failed. BOOL CFrameworkWndItem::WindowRefreshOEPGroup().
#define LOG_EVENT_FW_OEP_BUTTON_FAIL_2  9780  // Auto OEP DONE Button Create() failed. BOOL CFrameworkWndItem::WindowRefreshOEPGroup().
#define LOG_EVENT_FW_PLUFILETEST_FAIL   9781  // BlFwIfIsPluFileUnLocked() returned FALSE and nRetry expired. BOOL CFrameworkWndItem::WindowRefreshOEPGroup().
#define LOG_EVENT_FW_FCOMSTATUS_FAIL    9782  // fsComStatus & (BLI_STS_M_UPDATE | BLI_STS_BM_UPDATE) failed. BOOL CFrameworkWndItem::WindowRefreshOEPGroup().
#define LOG_EVENT_FW_DEVICE_CLAIM_ERR   9783  // failed ClaimDevice() in CDeviceEngine::Open(), next line indicates line number
#define LOG_EVENT_FW_CONNENGINE_START   9784  // ScfGetActiveDevice(SCF_TYPENAME_CONNENGINEMFC) indicated, Connection Engine started successfully.
#define LOG_EVENT_FW_CONNENGINE_UNUSED  9785  // ScfGetActiveDevice(SCF_TYPENAME_CONNENGINEMFC) was not used to start Connection Engine.
#define LOG_EVENT_FW_CONNENGINE_FAILED  9786  // ScfGetActiveDevice(SCF_TYPENAME_CONNENGINEMFC) failed on startup.
#define LOG_EVENT_FW_LAYOUT_FAILED      9787  // reaching layout file failed on startup.

/* New  fault code definitions to further enhance error recovery.*/

#define	FAULT_ERROR_MSDE_OPEN_DB		9801	   /* MSDE is not responding*/
#define	FAULT_ERROR_TTLUPD_WAI_FILE		9802	   /* Waiter File did not Update correctly */
#define	FAULT_ERROR_TTLUPD_PLU_FILE		9803	   /* PLU File did not Update correctly */
#define	FAULT_ERROR_TTLUPD_DEP_FILE		9804	   /* Department File did not Update correctly */
#define	FAULT_ERROR_TTLUPD_CAS_FILE		9805	   /* Cashier File did not Update correctly */
#define	FAULT_ERROR_TTLUPD_CPN_FILE		9806	   /* Coupon File did not Update correctly */

// LOG_EVENT_SALES_  from 9850 to 9899   normally used with MODULE_SALES
#define LOG_EVENT_SALES_EMPTY_OEP       9850       // ItemSalesOEPMakeMenu() did not return ITM_SUCCESS in ItemSalesOEPNextPlu()
#define LOG_EVENT_SALES_PREV_CONDIMENT  9851       // ItemPreviousCondiment() returned error in ItemSalesOEPMakeNewClass()
#define LOG_EVENT_SALES_MAKEMENU_ERR_1  9852       // ItemSalesOEPMakeMenu() returned error in ItemSalesOEPNextPlu()
#define LOG_EVENT_SALES_ORDERPLU_ERR_1  9853       // ItemSalesOEPOrderPlu() returned error in ItemSalesOEPNextPlu()
#define LOG_EVENT_SALES_MAKEMENU_PLU_1  9854       // CliOpOepPluRead() returned error in ItemSalesOEPMakeMenu()
#define LOG_EVENT_SALES_MAKEMENU_PLU_2  9855       // CliOpPluOepRead() returned error in ItemSalesOEPMakeMenu()

// System wide event codes used to document various kinds of events
#define LOG_EVENT_THREAD_START          9901

/*                                          
*===========================================================================
*  Fault Cause Codes
*  See file LOG.H which contains additional FAULT_aaa codes as in:
*     #define FAULT_GENERAL            1      
*===========================================================================
*/
#define    FAULT_ERROR_AFFECTION       25
#define    FAULT_ERROR_BAD_LENGTH      26
#define    FAULT_ERROR_FILE_HANDLE     27

#define    FAULT_ERROR_FILE_OPEN       30      /* file openning error */
#define    FAULT_ERROR_FILE_SEEK       31      /* file seek error */
#define    FAULT_ERROR_FILE_READ       32      /* file read error */
#define    FAULT_ERROR_FILE_WRITE      33      /* file write error */
#define    FAULT_ERROR_BLOCK_FULL      34      /* block is fll */
#define    FAULT_ERROR_FILE_BROKEN     35      /* BROKEN FILE INDEX IN FILE */
#define    FAULT_ERROR_DELETE_FILE     36      /* file delete error */
#define    FAULT_ERROR_CREATE_FILE     37      /* file create error */

/*=========== End of file ==================*/

