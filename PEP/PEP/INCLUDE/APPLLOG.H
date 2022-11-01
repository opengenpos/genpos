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
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
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
* Jan-24-00: 01.00.00 : K.Yanagida : Initial
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
#define MODULE_OP_OEP               457     /* opepara - OEP,	Saratoga */
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
/* LOG_ERROR_PRINT_  from 3150 to 3199 */
#define LOG_ERROR_PRT_SUPPRG_ERROR          3199

/* LOG_ERROR_MAINT_  from 3200 to 3149 */
#define LOG_ERROR_MAT_NOTCREATE_FILE        3200  
#define LOG_ERROR_MAT_SUP_ERROR             3201  
#define LOG_ERROR_MAT_NOTREADDEPT           3202  
#define LOG_ERROR_MAT_NOTASSIGNDEPT         3203
#define LOG_ERROR_MAT_NOTDELPLU             3204
#define LOG_ERROR_MAT_NOTREADPLU            3205  

/* LOG_ERROR_REPORT_  from 3250 to 3299 */
/* LOG_ERROR_STUB_    from 3300 to 3399 */

#define LOG_ERROR_STB_BAD_ENVIROMENT        3300    
#define LOG_ERROR_STB_3TIME_ERROR           3301

/* LOG_ERROR_CASHIER_  from 3400 to 3449 */
/* LOG_ERROR_WAITER_   from 3450 to 3499 */

/* LOG_ERROR_TOTAL_  from 3500 to 3549 */
#define LOG_ERROR_TTL_FILE_HANDLE           3500    /* File Handle Error */
#define LOG_ERROR_TTL_BACKUP_FILE           3501    /* 3501 - 3505 file back up error */
#define LOG_ERROR_TTL_OVERMAX               3510    /* Max Number Over */
#define LOG_ERROR_TTL_DEPT                  3511    /* Dept Number Log Base */
#define LOG_ERROR_TTL_AFFECT                3520    /* Affection Lost */
#define LOG_ERROR_TTL_NOTEMPTY              3521    /* Empty Table Full */
#define LOG_ERROR_TTL_FUNC_ILLIGAL          3522    /* Illigal Function(Major/Minor Class) Error */
#define LOG_ERROR_TTL_BACKUP_MASTER         3530    /* backup error from master */
#define LOG_ERROR_TTL_UPDATEEMPTY           3540    /* Read When Update Total File Empty Error */
#define LOG_ERROR_TTL_UPDATEOVERRD          3541    /* Read Pointer or Write Pointer Error */

/* LOG_ERROR_KPS_  from 3550 to 3599 */
#define LOG_ERROR_KPS_REWIND_FRAME          3550
#define LOG_ERROR_KPS_SPOOL_OVER            3551
#define LOG_ERROR_KPS_SEQUENCE_ERROR        3552
#define LOG_ERROR_KPS_BUFFER_CLEAR          3553    /* BUFFER CLEAR BY TIME UP 5 MINUTES */
#define LOG_ERROR_KPS_PRINTER_ERROR         3560    /* 3560 ~ 3564  RESERVED FOR PRINTE NO */
                                            
/* LOG_ERROR_GCF_  from 3600 to 3649 */
#define LOG_ERROR_GCF_DRIVE_UNMATCH         3600

/* LOG_ERROR_ISP_  from 3650 to 3699 */
#define LOG_ERROR_ISP_3TIME_ERROR           3650    /* Detect succeeded 3 times of Communication Error */
#define LOG_ERROR_ISP_INVALID_FUNCODE       3651    /* Received a invalid function code */
#define LOG_ERROR_ISP_INVALID_DATA_LEN      3652    /* Received a invalid data length   */

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

/* LOG_ERROR_DISPLAY_  from 3800 to 3849 */
#define LOG_ERROR_MAJOR_NOT_FOUND           3800
#define LOG_ERROR_REG_MAJOR_NOT_FOUND       3801
#define LOG_ERROR_REG_MINOR_NOT_FOUND_1     3802
#define LOG_ERROR_SUP_ERROR                 3840 

/* LOG_ERROR_OPEPARA_  from 3850 to 3899 */
/* LOG_ERROR_UIC_      from 3900 to 3949 */

/* LOG_ERROR_NB_       from 3950 to 3999 */
#define LOG_ERROR_NB_DETECT_3TIMES          3950    /* Detect succeeded 3 times of Communication Error */
#define LOG_ERROR_NB_INVALID_FUNCODE        3951    /* Received a invalid function code */

/* LOG_ERROR_TOTALUM_  from 4000 to 4049 */
/* LOG_ERROR_PARA_     from 4050 to 4079 */
/* LOG_ERROR_EJ_       from 4080 to 4099 */
#define LOG_ERROR_EJ_UNMATCH1               4080    /* Detect diffrent number of line between left and right */
#define LOG_ERROR_EJ_UNMATCH2               4081    /* Detect diffrent number of line between left and right */
#define LOG_ERROR_EJ_INVALID_DATA           4082    /* Detect diffrent number of line between left and right */
                                            
/* LOG_ERROR_SHR_       from 4100 to 4109 */
#define LOG_ERROR_SHR_DELETE                4100    /* detect deleting data */
#define LOG_ERROR_SHR_LOST                  4101    /* lost data */

/* LOG_ERROR_DFL_       from 4110 to 4119 */

/* LOG_ERROR_PROGPRT_   from 4120 to 4129 */
#define LOG_ERROR_PROGRPT_INVALID_ARG       4120    /* invalid argument received */
#define LOG_ERROR_PROGRPT_INVALID_DATA      4121    /* found invalid data in file */

/* LOG_ERROR_DIF_       from 4130 to 4140 */
#define LOG_ERROR_DIF_LRC_ERROR             4130    /* LRC error at data received */

/*
*===========================================================================
*  Application Event Codes
*===========================================================================
*/
/* LOG_EVENT_UI_    from 7000 to 7049 */

/* LOG_EVENT_ITEM_  from 7050 to 7099 */
#define LOG_EVENT_ITM_CLI_FUNC              7050    /* occer during execuitng client function */

/* LOG_EVENT_TRN_     from 7100 to 7149 */
/* LOG_EVENT_PRINT_   from 7150 to 7199 */
/* LOG_EVENT_MAINT_   from 7200 to 7249 */
/* LOG_EVENT_REPORT_  from 7250 to 7299 */
/* LOG_EVENT_STUB_    from 7300 to 7399 */

/* LOG_EVENT_CASHIER_  from 7400 to 7449 */
#define LOG_EVENT_CAS_DELETE_FILE           7400    /* Delete file     */

/* LOG_EVENT_WAITER_   from 7450 to 7499 */
#define LOG_EVENT_WAI_DELETE_FILE           7450    /* Delete file     */

/* LOG_EVENT_TOTAL_    from 7500 to 7549 */
#define LOG_EVENT_TTL_DEPT_DELETE_FILE      7500    /* Delete file     */
#define LOG_EVENT_TTL_PLU_DELETE_FILE       7501    /* Delete file     */
#define LOG_EVENT_TTL_CAS_DELETE_FILE       7502    /* Delete file     */
#define LOG_EVENT_TTL_WAI_DELETE_FILE       7503    /* Delete file     */
#define LOG_EVENT_TTL_CPN_DELETE_FILE       7504    /* Delete file, R3.0 */

/* LOG_EVENT_KPS_  from 7550 to 7599 */

/* LOG_EVENT_GCF_  from 7600 to 7649 */
#define LOG_EVENT_GCF_DELETE_FILE           7600    /* Delete file     */
                       
/* LOG_EVENT_ISP_  from 7650 to 7699 */
#define LOG_EVENT_ISP_LOCK_KEYBOARD         7650    /* Lock Keyboard   */
#define LOG_EVENT_ISP_UNLOCK_KEYBOARD       7651    /* Unlock Keyboard */
#define LOG_EVENT_ISP_CREATE_FILE           7660    /* Create File     */
#define LOG_EVENT_ISP_CHANGE_PLU            7661    /* Change PLU      */
#define LOG_EVENT_ISP_CHANGE_PARA           7662    /* Change Para     */

/* LOG_EVENT_SERVER_  from 7700 to 7799 */
/* LOG_EVENT_DISPLY_  from 7800 to 7849 */

/* LOG_EVENT_OPEPARA_  from 7850 to 7899 */
#define LOG_EVENT_OP_DEPT_DELETE_FILE       7850    /* Delete file     */
#define LOG_EVENT_OP_PLU_DELETE_FILE        7851    /* Delete file     */
#define LOG_EVENT_OP_CPN_DELETE_FILE        7852    /* Delete file     */
#define LOG_EVENT_OP_CSTR_DELETE_FILE       7853    /* Delete file     */
/* === Add Promotional Pricing Item (Release 3.1) BEGIN === */
#define LOG_EVENT_OP_PPI_DELETE_FILE        7854    /* Delete file     */
/* === Add Promotional Pricing Item (Release 3.1) END === */
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

/* LOG_EVENT_UIC_  from 7900 to 7949 */
#define LOG_EVENT_UIC_CHANGE_PLU            7900    /* Change PLU      */
#define LOG_EVENT_UIC_CHANGE_SUP            7901    /* Change Sup      */
#define LOG_EVENT_UIC_CHANGE_PARA           7902    /* Change Para     */
#define LOG_EVENT_UIC_RESET_CONSECUTIVE     7910    /* Reset cons no   */

/* LOG_EVENT_NB_       from 7950 to 7999 */
/* LOG_EVENT_TOTALUM_  from 8000 to 8049 */
/* LOG_EVENT_PARA_     from 8050 to 8079 */
/* LOG_EVENT_EJ_       from 8080 to 8099 */
#define LOG_EVENT_EJ_FFULL                  8080    /* File Full Case */
#define LOG_EVENT_EJ_FCREAT                 8081    /* File Create Case */
#define LOG_EVENT_EJ_RESET                  8082    /* Reset EJ File Case */

/* LOG_EVENT_ETK_  from 8100 to 8129 */
#define LOG_EVENT_ETK_DELETE_FILE           8100    /* Delete file     */

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
                                            
/* LOG_EVENT_PROGRPT_  from 8190 to 8219 */
#define LOG_EVENT_PROGRPT_DELETE_FILE       8190    /* Delete file     */

/*                                          
*===========================================================================
*  Fault Cause Codes
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

