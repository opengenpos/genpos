/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1998       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Header File of Terminal Configuration
* Category    : 2170 Hospitality US Application
* Program Name: TermCfg.H
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Data  :Ver.Rev.:   Name    : Description
* Jun-24-92:00.00.01:O.Nakada   : Initial
* Sep-07-92:00.01.01:O.Nakada   : Removed Waiter Lock Thread
* Sep-22-92:        :I.Shimizu  : Add Lowest Priority Thread for RAM M/C
*          :        :I.Shimizu  : Delete PRIO_LOW_MAIN
* Oct-08-92:00.01.07:T.Koyama   : Add PRIO_DAG_SLIP and PRIO_DAG_KEY
* Oct-26-92:00.01.0?:H.Shibamoto: change PRIORITIES
* Oct-25-93:00.00.01:Yoshiko.Jim: Add PRIO_CPM_XXX
*
* +-------------------+-------+-------+--------------------------------+
* | define            | OLD   | NEW   |                                |
* +-------------------+-------+-------+--------------------------------+
* | PRIO_SERVER_T     | 18    | 10    |    Server for Terminals        |
* | PRIO_SERVER_W     | 16    |  8    |    Server for Warkstation      |
* | PRIO_PMG_IN       | 10    | 18    |    Print Manager Input         |
* | PRIO_PMG_OUT      | 8     | 16    |    Print Manager Output        |
* +-------------------+-------+-------+--------------------------------+
*
* Nov-09-92:00.01.11:O.Nakada    : Add MODEL_HPUS
* Nov-26-92:01.00.01:O.Nakada    : Add PERIPHERAL_DEVICE_SCALE
* Mar-19-93:01.00.11:O.Nakada    : Removed Operating System definition
* Jun-18-93:01.00.12:NAKAJIMA,Ken: Add PRIO_SHARED_P
*                                : Add PRIO_DISP_FLY
* Mar-27-95:03.00.00:O.Nakada    : Add Video Output Service Control
* Apr-18-95:03.00.00:hkato       : add PRIO_FDT
* Feb-14-96:03.01.00:O.Nakada    : Add PERIPHERAL_DEVICE_OTHER
* Aug-06-98:03.03.00:hrkato      : Support Scanner
* Aug-11-99:03.05.00:M.Teraki    : R3.5 remove WAITER_MODEL
* Dec-19-02:        :C. Wunn : SR 15 for Software Security
* Sep-29-15 : 2.02.01  : R.Chambers : added reflection of messages to customer display windows.
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
#ifndef _INC_TERMCFG
#define _INC_TERMCFG


/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/


/*
*===========================================================================
*   2170 Hospitality Model Define,                          V3.3
*===========================================================================
*/

/*
*===========================================================================
*   Priority of Thread
*===========================================================================
*/
#if (defined _WIN32_WCE || defined WIN32)
// WARNING - the following priorities are pif level priorities, not Windows CE priorities
//   pif level priorities are between 1 and 7 with 7 being high priority and 1 low priority.
//   Windows CE priorities are between 0 and 6 with 0 being high priority and 6 being low priority.
//   See file pif.h for PIF_TASK_aaa defines.
//   See file Winbase.h for the THREAD_PRIORITY_aaa defines.
//
//   See file piftask.c, function PifBeginThread for thread starting and piority setting.
//   Be careful about using 7 (PIF_TASK_TIME_CRITICAL) as it is converted to THREAD_PRIORITY_TIME_CRITICAL
//   which can adversely affect system responsiveness.
//
//   User thread priorities are from 1 (low or idle) to 7 (high or critical).
//   User thread priorities should be from 1 to 6, 7 or critical should not be
//      used since it can lock out other threads.  Threads running at critical
//      are not time sliced according to the Windows CE documentation.
//
//  Changes made to the priorities in order to address a problem that Tim Horton's was
//  encountering.  Under heavy load, drive thru was not sufficiently responsive.
//  Changes made to bump priority of network service, flexible drive thru, and
//  terminal keyboard.
//  PRIO_NET_MGR defined in netmodul.h
//  PRIO_NET_ECHO defined in netmodul.h
//  PRIO_DLC_PC defined in conf.h
//
#define PRIO_POWER_MON    PIF_TASK_HIGHEST      /* Power Down Monitor            */
#define PRIO_UIE_KEYB     PIF_TASK_NORMAL       /* UI Engine Keyboard Input      */
#define PRIO_UIE_SCAN     PIF_TASK_NORMAL       /* UI Engine Scanner Input       */
#define PRIO_UIE_WAIT     PIF_TASK_NORMAL       /* UI Engine Waiter Lock Input   */
#define PRIO_UIE_MSR      PIF_TASK_NORMAL       /* UI Engine MSR Input           */
#define PRIO_UIE_DRAW     PIF_TASK_NORMAL       /* UI Engine Drawer Monitor      */
#define PRIO_NOTICEBD     PIF_TASK_ABOVE_NORMAL /* Notice Board - Changing this can impact sync */
#define PRIO_PMG_IN       PIF_TASK_NORMAL       /* Print Manager Input           */
#define PRIO_PMG_OUT      PIF_TASK_NORMAL       /* Print Manager Output          */
#define PRIO_DIF_RCV      PIF_TASK_NORMAL       /* Beverage Dispenser I/F        */
#define PRIO_KPS_PRT      PIF_TASK_NORMAL       /* Kitchen Printer Manager, KpsPrintProcess () */
#define PRIO_KPS_MON      PIF_TASK_NORMAL       /* Kitchen Printer Monitor, KpsTimerProcess () */
#define PRIO_SHARED_P     PIF_TASK_NORMAL       /* Shared Printer Manager        */
#define PRIO_DISP_FLY     PIF_TASK_NORMAL       /* Dislay on the Fly             */
#define PRIO_ENH_KDS      PIF_TASK_NORMAL       /* Enhanced KDS, 2172            */
#define PRIO_SERVER_T     PIF_TASK_NORMAL       /* Server for Terminals,    SerStartUp ()  */
#define PRIO_SERVER_W     PIF_TASK_NORMAL       /* Server for Warkstation,  IspMain ()     */
#define PRIO_CPM_RCV      PIF_TASK_NORMAL       /* Charge Posting Manager(HOTEL) */
#define PRIO_CPM_SEND     PIF_TASK_NORMAL       /* Charge Posting Manager(HOTEL) */
#define PRIO_UIC          PIF_TASK_NORMAL       /* User Interface Control        */
#define PRIO_FDT          PIF_TASK_NORMAL       /* Flexible Drive Through        */
#define PRIO_EEPT_RCV     PIF_TASK_NORMAL       /* EPT Manager,     Saratoga     */
#define PRIO_EEPT_SEND    PIF_TASK_NORMAL       /* EPT Manager,     Saratoga     */
#define PRIO_MAIN         PIF_TASK_NORMAL       /* Main Application              */
#define PRIO_VOS          PIF_TASK_LOWEST       /* Video Output Service          */
#define PRIO_TOTALS       PIF_TASK_ABOVE_IDLE   /* Totals Update                 */
#else
#define PRIO_UIE_KEYB       30              /* UI Engine Keyboard Input      */
#define PRIO_UIE_SCAN       28              /* UI Engine Scanner Input       */
#define PRIO_UIE_WAIT       26              /* UI Engine Waiter Lock Input   */
#define PRIO_UIE_MSR        24              /* UI Engine MSR Input           */
#define PRIO_UIE_DRAW       22              /* UI Engine Drawer Monitor      */
#define PRIO_NOTICEBD       20              /* Notice Board                  */
#define PRIO_PMG_IN         18              /* Print Manager Input           */
#define PRIO_PMG_OUT        17              /* Print Manager Output          */
#define PRIO_DIF_RCV        16              /* Beverage Dispenser I/F        */
#define PRIO_KPS_PRT        15              /* Kitchen Printer Manager       */
#define PRIO_SHARED_P       14              /* Shared Printer Manager        */
#define PRIO_KPS_MON        13              /* Kitchen Printer Monitor       */
#define PRIO_ENH_KDS        12               /* Enhanced KDS, 2172            */
#define PRIO_DISP_FLY       11              /* Dislay on the Fly             */
#define PRIO_SERVER_T       10              /* Server for Terminals          */
#define PRIO_CPM_RCV        9               /* Charge Posting Manager(HOTEL) */
#define PRIO_EEPT_RCV       9               /* EPT Manager, R2.0 */
#define PRIO_SERVER_W       8               /* Server for Warkstation        */
#define PRIO_CPM_SEND       7               /* Charge Posting Manager(HOTEL) */
#define PRIO_EEPT_SEND      6               /* EPT Manager, R2.0 */
#define PRIO_UIC            6               /* User Interface Control        */
#define PRIO_FDT            5               /* Flexible Drive Through        */
#define PRIO_MAIN           4               /* Main Application              */
#define PRIO_VOS            3               /* Video Output Service          */
#define PRIO_TOTALS         2               /* Totals Update                 */
#endif


/*
===== Reserved =====
===== Reserved =====
*/


/*
*===========================================================================
*   LCD Interface for Video Output Service
*===========================================================================
*/

//These values were in mldlocal.h
//The values in mldlocal.h only affect the Non-Touchscreen application
//The values listed here affect the Touchscreen and Non-Touchscreen
//These values give the Number of Rows and Columns in the various Text Displays

#define ECHO_BACK_ROW_LEN	2	//# of Rows for Echo Back Display
#define ECHO_BACK_CLM_LEN	40	//# of Columns for Echo Back Display - display on bottom left with operator info, prices, etc
#define DESCRIPTOR_ROW_LEN	2	//# of Rows for Descriptor Display
#define DESCRIPTOR_CLM_LEN	46	//# of Columns for Descriptor Display - display on botton center with red indicators

/* normal mode display position */
#define MLD_NOR_DSP1A_ROW_LEN   21      /* cursor window row length */
#define MLD_NOR_DSP1A_CLM_LEN   1       /* cursor window colum length */
#define MLD_NOR_DSP1_ROW_LEN    21      /* display#1 row length */
#define MLD_NOR_DSP1_CLM_LEN    29      /* display#1 colum length */
#define MLD_NOR_DSP2_ROW_LEN    21      /* display#2 row length */
#define MLD_NOR_DSP2_CLM_LEN    12      /* display#2 colum length */
#define MLD_NOR_POP_ROW_LEN     21      /* popup row length */
#define MLD_NOR_POP_CLM_LEN     27      /* popup colum length */
/* drive through 3 scroll system display position */
#define MLD_DRV3_DSP1_ROW_LEN   19      /* display#1 row length */
#define MLD_DRV3_DSP1_CLM_LEN   12      /* display#1 colum length */
#define MLD_DRV3_DSP2_ROW_LEN   19      /* display#2 row length */
#define MLD_DRV3_DSP2_CLM_LEN   12      /* display#2 colum length */
#define MLD_DRV3_DSP3_ROW_LEN   19      /* display#3 row length */
#define MLD_DRV3_DSP3_CLM_LEN   12      /* display#3 colum length */
#define MLD_DRV3_ORD1_ROW_LEN   1       /* order display#1 row length */
#define MLD_DRV3_ORD1_CLM_LEN   12      /* order display#1 colum length */
#define MLD_DRV3_ORD2_ROW_LEN   1       /* order display#2 row length */
#define MLD_DRV3_ORD2_CLM_LEN   12      /* order display#2 colum length */
#define MLD_DRV3_ORD3_ROW_LEN   1       /* order display#3 row length */
#define MLD_DRV3_ORD3_CLM_LEN   12      /* order display#3 colum length */
#define MLD_DRV3_TTL1_ROW_LEN   1       /* total display#1 row length */
#define MLD_DRV3_TTL1_CLM_LEN   12      /* total display#1 colum length */
#define MLD_DRV3_TTL2_ROW_LEN   1       /* total display#2 row length */
#define MLD_DRV3_TTL2_CLM_LEN   12      /* total display#2 colum length */
#define MLD_DRV3_TTL3_ROW_LEN   1       /* total display#3 row length */
#define MLD_DRV3_TTL3_CLM_LEN   12      /* total display#3 colum length */
/* drive through 3 scroll system popup display position */
#define MLD_DRV3_POP_ROW_LEN    21      /* popup row length */
#define MLD_DRV3_POP_CLM_LEN    14      /* popup colum length */
/* supervisor mode display position */
#define MLD_SUPER_MODE_ROW_LEN   21     /* display row length */
#define MLD_SUPER_MODE_CLM_LEN   40     /* display colum length */


/* ----- CAUTION !! ----- */
#define LCDWIN_ID_BIAS          (0x0400 + 0x0300)       /* WM_USER + 0x0300  = 1792*/

/* ----- Window ID ----- */
#define LCDWIN_ID_COMMON001     (LCDWIN_ID_BIAS + 1)    /* UIE - echo back or Lead Thru window, TypeLeadThru  */
#define LCDWIN_ID_COMMON002     (LCDWIN_ID_BIAS + 2)    /* UIE - descriptor window */
#define LCDWIN_ID_COMMON003     (LCDWIN_ID_BIAS + 3)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON004     (LCDWIN_ID_BIAS + 4)    /* UIE - echo back or Lead Thru Customer Display window, TypeLeadThruCustDisplay */
#define LCDWIN_ID_COMMON005     (LCDWIN_ID_BIAS + 5)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON006     (LCDWIN_ID_BIAS + 6)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON007     (LCDWIN_ID_BIAS + 7)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON008     (LCDWIN_ID_BIAS + 8)    /* ----- free window ----- */
#define LCDWIN_ID_LOCK050       (LCDWIN_ID_BIAS + 50)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK051       (LCDWIN_ID_BIAS + 51)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK052       (LCDWIN_ID_BIAS + 52)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK053       (LCDWIN_ID_BIAS + 53)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK054       (LCDWIN_ID_BIAS + 54)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK055       (LCDWIN_ID_BIAS + 55)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK056       (LCDWIN_ID_BIAS + 56)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK057       (LCDWIN_ID_BIAS + 57)   /* ----- free window ----- */
    // Register Mode windows
#define LCDWIN_ID_REG100        (LCDWIN_ID_BIAS + 100)  /* MLD - main scroll, CWindowText::TypeSnglReceiptMain  */
#define LCDWIN_ID_REG101        (LCDWIN_ID_BIAS + 101)  /* MLD - sub display, CWindowText::TypeSnglReceiptSub   */
#define LCDWIN_ID_REG102        (LCDWIN_ID_BIAS + 102)  /* MLD - popup display, CWindowText::TypeOEP            */
#define LCDWIN_ID_REG103        (LCDWIN_ID_BIAS + 103)  /* MLD - cursor display, CWindowText::TypeSnglReceiptScroll  */
#define LCDWIN_ID_REG104        (LCDWIN_ID_BIAS + 104)  /* OEP Customer Display window, TypeOEPCustDisplay  */
#define LCDWIN_ID_REG105        (LCDWIN_ID_BIAS + 105)  /* ----- free window ----- */
#define LCDWIN_ID_REG106        (LCDWIN_ID_BIAS + 106)  /* ----- free window ----- */
#define LCDWIN_ID_REG107        (LCDWIN_ID_BIAS + 107)  /* ----- free window ----- */
#define LCDWIN_ID_REG108        (LCDWIN_ID_BIAS + 108)  /* ----- free window ----- */
#define LCDWIN_ID_REG109        (LCDWIN_ID_BIAS + 109)  /* ----- free window ----- */
#define LCDWIN_ID_REG110        (LCDWIN_ID_BIAS + 110)  /* MLD - customer display, , same size/functionality as LCDWIN_ID_REG100 */
#define LCDWIN_ID_REG111        (LCDWIN_ID_BIAS + 111)  /* ----- free window ----- */
#define LCDWIN_ID_REG112        (LCDWIN_ID_BIAS + 112)  /* ----- free window ----- */
#define LCDWIN_ID_REG113        (LCDWIN_ID_BIAS + 113)  /* ----- free window ----- */
#define LCDWIN_ID_REG114        (LCDWIN_ID_BIAS + 114)  /* ----- free window ----- */
#define LCDWIN_ID_REG115        (LCDWIN_ID_BIAS + 115)  /* ----- free window ----- */
#define LCDWIN_ID_REG200        (LCDWIN_ID_BIAS + 200)  /* MLD - display #1, CWindowText::TypeMultiReceipt1  */
#define LCDWIN_ID_REG201        (LCDWIN_ID_BIAS + 201)  /* MLD - display #2, CWindowText::TypeMultiReceipt2  */
#define LCDWIN_ID_REG202        (LCDWIN_ID_BIAS + 202)  /* MLD - display #3, CWindowText::TypeMultiReceipt3  */
#define LCDWIN_ID_REG203        (LCDWIN_ID_BIAS + 203)  /* MLD - display #1 sub1   */
#define LCDWIN_ID_REG204        (LCDWIN_ID_BIAS + 204)  /* MLD - display #2 sub1   */
#define LCDWIN_ID_REG205        (LCDWIN_ID_BIAS + 205)  /* MLD - display #3 sub1   */
#define LCDWIN_ID_REG206        (LCDWIN_ID_BIAS + 206)  /* MLD - display #1 sub2   */
#define LCDWIN_ID_REG207        (LCDWIN_ID_BIAS + 207)  /* MLD - display #2 sub2   */
#define LCDWIN_ID_REG208        (LCDWIN_ID_BIAS + 208)  /* MLD - display #3 sub3   */
#define LCDWIN_ID_REG209        (LCDWIN_ID_BIAS + 209)  /* MLD - popup display      */
#define LCDWIN_ID_REG210        (LCDWIN_ID_BIAS + 210)  /* ----- free window ----- */
#define LCDWIN_ID_REG211        (LCDWIN_ID_BIAS + 211)  /* ----- free window ----- */
#define LCDWIN_ID_REG212        (LCDWIN_ID_BIAS + 212)  /* ----- free window ----- */
#define LCDWIN_ID_REG213        (LCDWIN_ID_BIAS + 213)  /* ----- free window ----- */
#define LCDWIN_ID_REG214        (LCDWIN_ID_BIAS + 214)  /* ----- free window ----- */
#define LCDWIN_ID_REG215        (LCDWIN_ID_BIAS + 215)  /* ----- free window ----- */
    // Supervisor Mode windows
#define LCDWIN_ID_SUPER300      (LCDWIN_ID_BIAS + 300)  /* MLD - supervisor mode   */
#define LCDWIN_ID_SUPER301      (LCDWIN_ID_BIAS + 301)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER302      (LCDWIN_ID_BIAS + 302)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER303      (LCDWIN_ID_BIAS + 303)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER304      (LCDWIN_ID_BIAS + 304)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER305      (LCDWIN_ID_BIAS + 305)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER306      (LCDWIN_ID_BIAS + 306)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER307      (LCDWIN_ID_BIAS + 307)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER308      (LCDWIN_ID_BIAS + 308)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER309      (LCDWIN_ID_BIAS + 309)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER310      (LCDWIN_ID_BIAS + 310)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER311      (LCDWIN_ID_BIAS + 311)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER312      (LCDWIN_ID_BIAS + 312)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER313      (LCDWIN_ID_BIAS + 313)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER314      (LCDWIN_ID_BIAS + 314)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER315      (LCDWIN_ID_BIAS + 315)  /* ----- free window ----- */
    // Program Mode windows
#define LCDWIN_ID_PROG400       (LCDWIN_ID_BIAS + 400)  /* MLD -                   */
#define LCDWIN_ID_PROG401       (LCDWIN_ID_BIAS + 401)  /* ----- free window ----- */
#define LCDWIN_ID_PROG402       (LCDWIN_ID_BIAS + 402)  /* ----- free window ----- */
#define LCDWIN_ID_PROG403       (LCDWIN_ID_BIAS + 403)  /* ----- free window ----- */
#define LCDWIN_ID_PROG404       (LCDWIN_ID_BIAS + 404)  /* ----- free window ----- */
#define LCDWIN_ID_PROG405       (LCDWIN_ID_BIAS + 405)  /* ----- free window ----- */
#define LCDWIN_ID_PROG406       (LCDWIN_ID_BIAS + 406)  /* ----- free window ----- */
#define LCDWIN_ID_PROG407       (LCDWIN_ID_BIAS + 407)  /* ----- free window ----- */
#define LCDWIN_ID_PROG408       (LCDWIN_ID_BIAS + 408)  /* ----- free window ----- */
#define LCDWIN_ID_PROG409       (LCDWIN_ID_BIAS + 409)  /* ----- free window ----- */
#define LCDWIN_ID_PROG410       (LCDWIN_ID_BIAS + 410)  /* ----- free window ----- */
#define LCDWIN_ID_PROG411       (LCDWIN_ID_BIAS + 411)  /* ----- free window ----- */
#define LCDWIN_ID_PROG412       (LCDWIN_ID_BIAS + 412)  /* ----- free window ----- */
#define LCDWIN_ID_PROG413       (LCDWIN_ID_BIAS + 413)  /* ----- free window ----- */
#define LCDWIN_ID_PROG414       (LCDWIN_ID_BIAS + 414)  /* ----- free window ----- */
#define LCDWIN_ID_PROG415       (LCDWIN_ID_BIAS + 415)  /* ----- free window ----- */

/*
 * Special identifiers for special needs follow.
 * These special window identifiers are not connected to the event
 * system with EvsConnect () in CFrameworkWndText::OnCreate() as is
 * normally done with standard text type windows.
 *
 * The Clock windows used with timer messages which are processed in the
 * method CFrameworkWndText::OnTimer() to update the displayed time.
 */
#define LCDWIN_ID_CLOCK01      (LCDWIN_ID_BIAS + 501)         // clock window #1, CWindowText::TypeClock
#define LCDWIN_ID_CLOCK02      (LCDWIN_ID_BIAS + 502)         // clock window #2, CWindowText::TypeClockCustDisplay
#define LCDWIN_ID_CLOCK03      (LCDWIN_ID_BIAS + 503)         // ----- free window, clock window #3
#define LCDWIN_ID_CLOCK04      (LCDWIN_ID_BIAS + 504)         // ----- free window, clock window #4
#define LCDWIN_ID_CLOCK05      (LCDWIN_ID_BIAS + 505)         // ----- free window, clock window #5
#define LCDWIN_ID_OEPSTANDARD  (LCDWIN_ID_BIAS + 521)         // Used with the OEP and subwindow system.

/* ----- Window Buffer Length ----- */
#define LCDWIN_LEN_COMMON001    160         /* Memory used in Operator Lead Thru window */
#define LCDWIN_LEN_COMMON002    184			/* Memory used in Descriptors (indicators). Will change if UIE_LCD_COLUMN2 changes*/
#define LCDWIN_LEN_COMMON003    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON004    160         /* Memory used in Lead Thru Customer Display, duplicate of LCDWIN_LEN_COMMON001 */
#define LCDWIN_LEN_COMMON005    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON006    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON007    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON008    0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK050      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK051      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK052      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK053      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK054      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK055      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK056      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK057      0           /* ------ not used ------- */
//#define LCDWIN_LEN_REG100     1092		// Saratoga buffer size for 26 column width
#define LCDWIN_LEN_REG100       1218		// LCDWIN_ID_REG100, US Customs buffer size for 29 column width cwunn 3/19/03
#define LCDWIN_LEN_REG101       546         // LCDWIN_ID_REG101 window buffer size 
#define LCDWIN_LEN_REG102       1134        // LCDWIN_ID_REG102 window buffer size, MLD - popup display, CWindowText::TypeOEP
#define LCDWIN_LEN_REG103       42          // LCDWIN_ID_REG103 window buffer size
#define LCDWIN_LEN_REG104       1134        // LCDWIN_ID_REG104 window buffer size, OEP Customer Display window, CWindowText::TypeOEPCustDisplay
#define LCDWIN_LEN_REG105       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG106       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG107       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG108       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG109       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG110       1218        /* LCDWIN_ID_REG110, customer display, same size/functionality as LCDWIN_ID_REG100 */
#define LCDWIN_LEN_REG111       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG112       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG113       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG114       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG115       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG200       456
#define LCDWIN_LEN_REG201       494
#define LCDWIN_LEN_REG202       494
#define LCDWIN_LEN_REG203       24
#define LCDWIN_LEN_REG204       26
#define LCDWIN_LEN_REG205       26
#define LCDWIN_LEN_REG206       24
#define LCDWIN_LEN_REG207       26
#define LCDWIN_LEN_REG208       26

#ifdef TOUCHSCREEN
#define LCDWIN_LEN_REG209		1134
#else
#define LCDWIN_LEN_REG209       588
#endif //Touchscreen

#define LCDWIN_LEN_REG210       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG211       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG212       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG213       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG214       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG215       0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER300     1680
#define LCDWIN_LEN_SUPER301     1680        /* ------ cwunn added for RJC at GSU SR 15 */
#define LCDWIN_LEN_SUPER302     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER303     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER304     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER305     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER306     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER307     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER308     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER309     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER310     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER311     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER312     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER313     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER314     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER315     0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG400      1680
#define LCDWIN_LEN_PROG401      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG402      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG403      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG404      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG405      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG406      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG407      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG408      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG409      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG410      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG411      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG412      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG413      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG414      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG415      0           /* ------ not used ------- */


#endif                                      /* _INC_TERMCFG */
/* ====================================== */
/* ========== End of TermCfg.H ========== */
/* ====================================== */
