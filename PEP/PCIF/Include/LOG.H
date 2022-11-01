/*hhhh
*****************************************************************************
**    
**  Title:      Header File for the module ID, the fault code and so on
**
**  File Name:  LOG.H
**
**  Categoly:   Header File
**
**  Abstruct:   This header file defines as follows:
**                  
**                  Module ID or Function ID
**                  Fault Module ID (same as the Module ID)
**                  Fault Cause Code
**                  Exception Code
**
**              The Fault Module ID and Fault Cause Code are defined as
**              follows:
**
**                  These ID and codes are displayed fault codes at system
**                  fault occured. When a program detects a fatal error and
**                  then calls PifAbort(usFaultModule, usFaultCode), the
**                  system halts and display them specified by PifAbort().
**                  They consist of 5 digits decimal as follows:
**              
**                  +++----- usFaultModule: (Fault Module ID)
**                  |||         The higher 3 digits indicate module or
**                  XXXXX       function which detects a fatal error.
**                     ||       And the highest digit indicate major
**                     ||       classification as follows:
**                     ||
**                     ||           0: OS or BIOS
**                     ||           1: Portable Platform Interface
**                     ||           2: System Applications
**                     ||           3: Applications
**                     ||
**                     ++--- usFaultCode: (Fault Cause Code)
**                              These 2 digits inicate a cause of fatal error.
**                  
**              The Exception Code is defined as follows:    
**                  
**                  This code is used with the Exception Log Function.
**                  The user specifies this code and the Module ID on 
**                  PifLog(USHORT usModuleId, USHORT usExceptionCode).
**                  This function logs the Module ID and Exception Code
**                  with time stamp. This code is deivided into two classes,
**                  Event Code and Error Code.
**                  This code consists of 4 digits decimal as follows:
**                     
**                     0 -   99 :   Error Code for System Fault
**                   100 -  999 :   Error Code for Common
**                  1000 - 1999 :   Error Code for System
**                  2000 - 2999 :   Error Code for System Applications
**                  3000 - 3999 :   Error Code for Applications
**
**                  5000 - 5999 :   Event Code for Common
**                  6000 - 6999 :   Event Code for System
**                  7000 - 7999 :   Event Code for System Applications
**                  8000 - 8999 :   Event Code for Applications
**                     
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date       Name           Description        
**  1.1       APR-03-92  I.Shimizu      Beta Version
**  00.00.01  AUG-17-92  I.Shimizu      Initial release
**  00.00.02  NOV-25-92  T.Kojima       add MODULE_PIF_CONTROLFILE
**
*****************************************************************************
hhhh*/


/******************************************
 *  Define Module ID and Fault Module ID  *
 ******************************************/
/*****  OS or BIOS  *****/
#define MODULE_SYS_CLASS            0
#define FAULT_AT_SYS_CLASS          MODULE_SYS_CLASS
#define MODULE_XINU_MAIN            1
#define FAULT_AT_XINU_MAIN          MODULE_XINU_MAIN
#define MODULE_SYS_STARTUP          2                   /* don't change */
#define FAULT_AT_STARTUP            MODULE_SYS_STARTUP    

#define MODULE_SYS_xxxx             99
#define FAULT_AT_SYS_xxxx           MODULE_SYS_xxxx


/*****  Portable Platform Interface  ******/
#define MODULE_PIF_CLASS            100
#define FAULT_AT_PIF_CLASS          MODULE_PIF_CLASS
#define MODULE_PIF_ABORT            100
#define FAULT_AT_PIFABORT           MODULE_PIF_ABORT
#define MODULE_PIF_BEGINTHREAD      101
#define FAULT_AT_PIFBEGINTHREAD     MODULE_PIF_BEGINTHREAD
#define MODULE_PIF_ENDTHREAD        102
#define FAULT_AT_PIFENDTHREAD       MODULE_PIF_ENDTHREAD
#define MODULE_PIF_GETTHREADID      103
#define FAULT_AT_PIFGETTHREADID     MODULE_PIF_GETTHREADID
#define MODULE_PIF_GETPROCESSID     103
#define FAULT_AT_PIFGETPROCESSID    MODULE_PIF_GETPROCESSID
#define MODULE_PIF_CREATESEM        104
#define FAULT_AT_PIFCREATESEM       MODULE_PIF_CREATESEM
#define MODULE_PIF_DELETESEM        105
#define FAULT_AT_PIFDELETESEM       MODULE_PIF_DELETESEM
#define MODULE_PIF_REQUESTSEM       106
#define FAULT_AT_PIFREQUESTSEM      MODULE_PIF_REQUESTSEM
#define MODULE_PIF_RELEASESEM       107
#define FAULT_AT_PIFRELEASESEM      MODULE_PIF_RELEASESEM
#define MODULE_PIF_SLEEP            108
#define FAULT_AT_PIFSLEEP           MODULE_PIF_SLEEP
#define MODULE_PIF_GETDATETIME      109
#define FAULT_AT_PIFGETDATETIME     MODULE_PIF_GETDATETIME
#define MODULE_PIF_SETDATETIME      110
#define FAULT_AT_PIFSETDATETIME     MODULE_PIF_SETDATETIME
#define MODULE_PIF_OPENFILE         111
#define FAULT_AT_PIFOPENFILE        MODULE_PIF_OPENFILE
#define MODULE_PIF_READFILE         112
#define FAULT_AT_PIFREADFILE        MODULE_PIF_READFILE
#define MODULE_PIF_WRITEFILE        113
#define FAULT_AT_PIFWRITEFILE       MODULE_PIF_WRITEFILE
#define MODULE_PIF_SEEKFILE         114
#define FAULT_AT_PIFSEEKFILE        MODULE_PIF_SEEKFILE
#define MODULE_PIF_CLOSEFILE        115
#define FAULT_AT_PIFCLOSEFILE       MODULE_PIF_CLOSEFILE
#define MODULE_PIF_DELETEFILE       116
#define FAULT_AT_PIFDELETEFILE      MODULE_PIF_DELETEFILE
#define MODULE_PIF_OPENCOM          117
#define FAULT_AT_PIFOPENCOM         MODULE_PIF_OPENCOM
#define MODULE_PIF_READCOM          118
#define FAULT_AT_PIFREADCOM         MODULE_PIF_READCOM
#define MODULE_PIF_WRITECOM         119
#define FAULT_AT_PIFWRITECOM        MODULE_PIF_WRITECOM
#define MODULE_PIF_CONTROLCOM       120
#define FAULT_AT_PIFCONTROLCOM      MODULE_PIF_CONTROLCOM
#define MODULE_PIF_CLOSECOM         121
#define FAULT_AT_PIFCLOSECOM        MODULE_PIF_CLOSECOM
#define MODULE_PIF_NETOPEN          122
#define FAULT_AT_PIFNETOPEN         MODULE_PIF_NETOPEN
#define MODULE_PIF_NETSEND          123
#define FAULT_AT_PIFNETSEND         MODULE_PIF_NETSEND
#define MODULE_PIF_NETRECEIVE       124
#define FAULT_AT_PIFNETRECEIVE      MODULE_PIF_NETRECEIVE
#define MODULE_PIF_NETCONTROL       125
#define FAULT_AT_PIFNETCONTROL      MODULE_PIF_NETCONTROL
#define MODULE_PIF_NETCLOSE         126
#define FAULT_AT_PIFNETCLOSE        MODULE_PIF_NETCLOSE
#define MODULE_PIF_GETCHAR          127
#define FAULT_AT_PIFGETCHAR         MODULE_PIF_GETCHAR
#define MODULE_PIF_CONTROLKEYBOARD  128
#define FAULT_AT_PIFCONTROLKEYBOARD MODULE_PIF_CONTROLKEYBOARD
#define MODULE_PIF_GETKEYLOCK       129
#define FAULT_AT_PIFGETKEYLOCK      MODULE_PIF_GETKEYLOCK
#define MODULE_PIF_GETWAITER        130
#define FAULT_AT_PIFGETWAITER       MODULE_PIF_GETWAITER
#define MODULE_PIF_DISPLAYSTRING    131
#define FAULT_AT_PIFDISPLAYSTRING   MODULE_PIF_DISPLAYSTRING
#define MODULE_PIF_DISPLAYATTR      132
#define FAULT_AT_PIFDISPLAYATTR     MODULE_PIF_DISPLAYATTR
#define MODULE_PIF_LIGHTDESCR       133
#define FAULT_AT_PIFLIGHTDESCR      MODULE_PIF_LIGHTDESCR
#define MODULE_PIF_OPENDRAWER       134
#define FAULT_AT_PIFOPENDRAWER      MODULE_PIF_OPENDRAWER
#define MODULE_PIF_DRAWERSTATUS     135
#define FAULT_AT_PIFDRAWERSTATUS    MODULE_PIF_DRAWERSTATUS
#define MODULE_PIF_BEEP             136
#define FAULT_AT_PIFBEEP            MODULE_PIF_BEEP
#define MODULE_PIF_PLAYSTANZA       137
#define FAULT_AT_PIFPLAYSTANZA      MODULE_PIF_PLAYSTANZA
#define MODULE_PIF_TONEVOLUME       138
#define FAULT_AT_PIFTONEVOLUME      MODULE_PIF_TONEVOLUME
#define MODULE_PIF_SYSCONFIG        139
#define FAULT_AT_PIFSYSCONFIG       MODULE_PIF_SYSCONFIG
#define MODULE_PIF_DISABLE          140
#define FAULT_AT_PIFDISABLE         MODULE_PIF_DISABLE
#define MODULE_PIF_RESTORE          141
#define FAULT_AT_PIFRESTORE         MODULE_PIF_RESTORE
#define MODULE_PIF_LOG              142
#define FAULT_AT_PIFLOG             MODULE_PIF_LOG
#define MODULE_PIF_READMSR          143
#define FAULT_AT_PIFREADMSR         MODULE_PIF_READMSR
#define MODULE_PIF_LOADFARDATA      144
#define FAULT_AT_PIFLOADFARDATA     MODULE_PIF_LOADFARDATA
#define MODULE_PIF_SAVEFARDATA      145
#define FAULT_AT_PIFSAVEFARDATA     MODULE_PIF_SAVEFARDATA
#define MODULE_PIF_CONTROL          146
#define FAULT_AT_PIFCONTROL         MODULE_PIF_CONTROL
#define MODULE_PIF_RESTART          147
#define FAULT_AT_PIFRESTART         MODULE_PIF_RESTART
#define MODULE_PIF_TURNOFFPOWER     148
#define FAULT_AT_PIFTURNOFFPOWER    MODULE_PIF_TURNOFFPOWER
#define MODULE_PIF_OPENMSR          149
#define FAULT_AT_PIFOPENMSR         MODULE_PIF_OPENMSR
#define MODULE_PIF_CLOSEMSR         150
#define FAULT_AT_PIFCLOSEMSR        MODULE_PIF_CLOSEMSR

#define MODULE_PIF_VIOSTRATTR       151
#define FAULT_AT_PIFVIOSTRATTR      MODULE_PIF_VIOSTRATTR
#define MODULE_PIF_VIOSTR           152
#define FAULT_AT_PIFVIOSTR          MODULE_PIF_VIOSTR
#define MODULE_PIF_VIOCURONOFF      153
#define FAULT_AT_PIFVIOCURONOFF     MODULE_PIF_VIOCURONOFF
#define MODULE_PIF_VIOCURTYPE       154
#define FAULT_AT_PIFVIOCURTYPE      MODULE_PIF_VIOCURTYPE
#define MODULE_PIF_VIOGETCURPOS     155
#define FAULT_AT_PIFVIOGETCURPOS    MODULE_PIF_VIOGETCURPOS
#define MODULE_PIF_VIOSETCURPOS     156
#define FAULT_AT_PIFVIOSETCURPOS    MODULE_PIF_VIOSETCURPOS
#define MODULE_PIF_VIOSCROLLUP      157
#define FAULT_AT_PIFVIOSCROLLUP     MODULE_PIF_VIOSCROLLUP
#define MODULE_PIF_VIOSCROLLDOWN    158
#define FAULT_AT_PIFVIOSCROLLDOWN   MODULE_PIF_VIOSCROLLDOWN
#define MODULE_PIF_VIOMODE          159
#define FAULT_AT_PIFVIOMODE         MODULE_PIF_VIOMODE
#define MODULE_PIF_VIOBACKLIGHT     160
#define FAULT_AT_PIFVIOBACKLIGHT    MODULE_PIF_VIOBACKLIGHT
#define MODULE_PIF_VIOACTIVEPAGE    161
#define FAULT_AT_PIFVIOACTIVEPAGE   MODULE_PIF_VIOACTIVEPAGE
#define MODULE_PIF_VIOLOADCG        162
#define FAULT_AT_PIFVIOLOADCG       MODULE_PIF_VIOLOADCG

#define MODULE_PIF_CONTROLFILE      170
#define FAULT_AT_PIFCONTROLFILE     MODULE_PIF_CONTROLFILE

#define MODULE_PIF_DELETEFILEEX     171
#define FAULT_AT_PIFDELETEFILEEX    MODULE_PIF_DELETEFILEEX
#define MODULE_PIF_MOVEFILE         172
#define FAULT_AT_PIFMOVEEFILE       MODULE_PIF_MOVEFILE
#define MODULE_PIF_GETFILESIZE      173
#define FAULT_AT_PIFGETFILESIZE     MODULE_PIF_GETFILESIZE
#define MODULE_PIF_WRITEFILEEX      174
#define FAULT_AT_PIFWRITEFILEEX     MODULE_PIF_WRITEFILEEX
#define MODULE_PIF_GETFILESIZEEX    175
#define FAULT_AT_PIFGETFILESIZEEX   MODULE_PIF_GETFILESIZEEX
#define MODULE_PIF_GETDISKFREESPACE 176
#define FAULT_AT_PIFGETDISKFREESPACE MODULE_PIF_GETDISKFREESPACE


/* reserved */
#define MODULE_PIF_CREATEQUEUE      190
#define FAULT_AT_PIFCREATEQUEUE     MODULE_PIF_CREATEQUEUE
#define MODULE_PIF_DELETEQUEUE      191
#define FAULT_AT_PIFDELETEQUEUE     MODULE_PIF_DELETEQUEUE
#define MODULE_PIF_WRITEQUEUE       192
#define FAULT_AT_PIFWRITEQUEUE      MODULE_PIF_WRITEQUEUE
#define MODULE_PIF_READQUEUE        193
#define FAULT_AT_PIFREADQUEUE       MODULE_PIF_READQUEUE
#define MODULE_PIF_XINUINIT         194
#define FAULT_AT_PIFXINUINIT        MODULE_PIF_XINUINIT
#define MODULE_PIF_SUBXINUERROR     195
#define FAULT_AT_PIFSUBXINUERROR    MODULE_PIF_SUBXINUERROR
#define MODULE_PIF_LOWEST           196
#define FAULT_AT_PIFLOWEST          MODULE_PIF_LOWEST
#define MODULE_PIF_FAULT            197
#define FAULT_AT_PIFFAULT           MODULE_PIF_FAULT
#define MODULE_PIF_OVERLAY          198
#define FAULT_AT_PIFOVERLAY         MODULE_PIF_OVERLAY
#define MODULE_PIF_SWITCHSTACK      199
#define FAULT_AT_PIFSWITCHSTACK     MODULE_PIF_SWITCHSTACK


/*****  System Applications  *****/       
/* for User Interface Engine */
#define MODULE_UIE_CLASS            200
#define MODULE_UIE_READ_RING_MSG    201
#define MODULE_UIE_RETRY            202
#define MODULE_UIE_TRACK            203
/* 
===== Reserved ======
*/
#define MODULE_UIE_SCALE            208
#define MODULE_UIE_SCANNER          209
/* 
===== Reserved ======
*/
#define MODULE_UIE_INITMENU         210
#define MODULE_UIE_NEXTMENU         211
#define MODULE_UIE_ACCEPT           212
#define MODULE_UIE_REJECT           213
#define MODULE_UIE_EXIT             214
#define MODULE_UIE_OPENSEQ          215
#define MODULE_UIE_CLOSESEQ         216
#define MODULE_UIE_CDIALOG          217
#define MODULE_UIE_DDIALOG          218
#define MODULE_UIE_CHECKSEQ         219

/* for Video Output Service */
#define MODULE_VOS_INITIALIZE       250
#define MODULE_VOS_SETMODE          251
#define MODULE_VOS_SETBLIGHT        252
#define MODULE_VOS_SETBLIGHTOFF     253
#define MODULE_VOS_REFRESH          254
#define MODULE_VOS_CREATEWINDOW     255
#define MODULE_VOS_DELETEWINDOW     256
#define MODULE_VOS_BORDERSTRING     257
#define MODULE_VOS_SETCURTYPE       258
#define MODULE_VOS_SETBLINK         259
#define MODULE_VOS_SETCURPOS        260
#define MODULE_VOS_GETCURPOS        261
#define MODULE_VOS_SCROLLUP         262
#define MODULE_VOS_SCROLLDOWN       263
#define MODULE_VOS_CLS              264
#define MODULE_VOS_STRING           265
#define MODULE_VOS_STRINGATTR       266
#define MODULE_VOS_PRINTF           267
#define MODULE_VOS_PRINTFATTR       268
#define MODULE_VOS_DISPLAYATTR      269
#define MODULE_VOS_SETWINTYPE       270
#define MODULE_VOS_VIRTUALALLOC     271
#define MODULE_VOS_VIRTUALFREE      272
#define MODULE_VOS_MEMORYALLOC      273
#define MODULE_VOS_MEMORYFREE       274
#define MODULE_VOS_MEMORYINFO       275

/* for Installation Menu */
#define MODULE_INS_LOADER           296
#define MODULE_INS_MENU             297

/* for Re-entrant Functions Libaray */
#define MODULE_RFL_FORMSTR          298
#define MODULE_RFL_TAX              299


/*****  Applications  *****/

/* ITEM MODULE  ITEMLOG.H */
/* UI MODULE  UILOG.H */
/* DISPLAY MODULE   DISPLOG.H */

/******************************
 *  Define Fault Cause Codes  *
 ******************************/
#define FAULT_GENERAL            1      /* general */
#define FAULT_TIMEOUT            2      /* timeout occured */
#define FAULT_INVALID_DATA       3      /* unrecognized data detected */
#define FAULT_INVALID_FUNCTION   4      /* this function not provided */
#define FAULT_INVALID_ARGUMENTS  5      /* invalid argument detected */
#define FAULT_INVALID_HANDLE     6      /* invalid handle detected */
#define FAULT_INVALID_MODE       7      /* this function detected in invalid mode */
#define FAULT_BUFFER_OVERFLOW    8      /* buffer overflow occured. */
#define FAULT_SHORT_RESOURCE     9      /* short resource (stack size, memory */ 
                                        /* size, table size or buffer size */
#define FAULT_BAD_ENVIRONMENT   10      /* this function not provided in */
                                        /* the current environment */

#define FAULT_CRC_ROOT          11      /* software CRC error at root */
#define FAULT_CRC_PAGE          12      /* software CRC error at page */

#define FAULT_PROT_ROOT         13      /* Root Protection Error */
#define FAULT_PROT_PAGE         14      /* Page Protection Error */
#define FAULT_PROT_SRAM         15      /* SRAM Protection Error */
#define FAULT_FETCH             16      /* CCh Fetch */
#define FAULT_INT               17      /* Illegal Interrupt */
#define FAULT_SYSTEM            18      /* System Fault */
#define FAULT_INT0              19      /* Divided by 0 */
#define FAULT_INT1              20      /* Single Step */

#define FAULT_xxxx              21


#define FAULT_INVALID_CAUSE_CODE 90     /* invalid fault cause code detected */

#define FAULT_USED_HANDLE       96      /* This error occurs to do not close
                                           a function.                      */
#define FAULT_NOT_EXECUTE_PROCESS 97    /* This error occurs to do not execute
                                           the Simulator's process.         */
#define FAULT_OS2_TIMEOUT       98      /* This error occurs to cause timeout
                                           at API of OS/2.                  */
#define FAULT_OS2_API           99      /* This error occurs at API of OS/2.*/


/****************************
 *  Define Exception Codes  *
 ****************************/
/*****  Error Codes  *****/
/* 0-99 used with PifAbort() */
#define LOG_ERROR_SWINFO        1000    /* Loaded S/W Information Error */
#define LOG_ERROR_CRC_ROOT      1001    /* Root S/W CRC Error */
#define LOG_ERROR_CRC_PAGE      1002    /* Page S/W CRC Error */
#define LOG_ERROR_NICD          1003    /* NiCd Battery Empty */
#define LOG_ERROR_RS232         1004    /* RS232 Local Board Error */
#define LOG_ERROR_IHC           1005    /* IHC/NCR DLC Local Board Error */

/*****  Event Codes  *****/
#define LOG_EVENT_POWER_DOWN                6000    /* don't change */
#define LOG_EVENT_POWER_UP_RUN              6001
#define LOG_EVENT_POWER_UP_RESET            6002
#define LOG_EVENT_POWER_UP_CLEAR_TOTAL      6003
#define LOG_EVENT_POWER_UP_CLEAR_WHOLE      6004
#define LOG_EVENT_ENTER_INS_MENU            6005
#define LOG_EVENT_ENTER_LOADER              6006

#define LOG_EVENT_POWER_UP_CLEAR_CHK        6101
                  
#define LOG_EVENT_

/*****************
 *  End of LOG.H *
 *****************/
